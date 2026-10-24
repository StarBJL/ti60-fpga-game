#!/usr/bin/env python3
"""
tb_av.py - end-to-end: audio_mixer -> HDMI audio FIFO -> data islands -> TMDS,
with the HDMI scan-out DMA and the mixer sharing one read port (aud_rd_mux),
wired as in ddr3_example_top.v (tb_av_top.v).

  python tb_av.py [frames]

Decodes the TMDS output with tb_hdmi.TB (video bit-exact vs the DDR pattern,
islands, ECC, packets) and checks that the audio samples in the HDMI stream are
exactly what tb_mixer.Model computes for the programmed voices (after the
leading silence before KEYON).
"""
import os, sys, random
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'blit'))
sys.path.insert(0, os.path.join(HERE, '..', 'hdmi'))
from vsim import Sim
import tb_hdmi as H
import tb_mixer as X

FILES = [os.path.join(HERE, '..', '..', 'rtl', 'hdmi', 'hdmi_fb.v'),
         os.path.join(HERE, '..', '..', 'rtl', 'hdmi', 'hdmi_island.v'),
         os.path.join(HERE, '..', '..', 'rtl', 'audio', 'audio_mixer.v'),
         os.path.join(HERE, 'tb_av_top.v')]
SBASE = 0x02000000                      # sample data
V_ACT = 24


class AV(H.TB):
    def __init__(s, seed=1):
        s.v_act = V_ACT
        s.v_tot = V_ACT + H.V_FP + H.V_SYNC + H.V_BP
        s.sim = Sim(FILES, 'tb_av_top', seed=seed, params={'V_ACT': V_ACT})
        s.rnd = random.Random(seed)
        s.t = 0
        s.hv = []
        s.ref = [H.TmdsRef(), H.TmdsRef(), H.TmdsRef()]
        s.apbq, s.apb_state, s.apb_rd = [], 0, None
        s.ar = []
        s.rx, s.rx_t, s.pk = [], [], []
        s.line_syms, s.line_v = [], None
        s.fb_base = 0x01000000
        s.nvid = s.nvid_dma = s.nlines = s.nisl = s.nvpre = 0
        s.hdmi = True
        s.mixer_bursts = 0
        s.smem = bytearray(s.rnd.getrandbits(8) for _ in range(0x10000))
        for k in range(0, 0x10000, 2):          # smooth-ish waveform
            import math
            x = int(9000 * math.sin(k / 41.0) + 3000 * math.sin(k / 7.0))
            s.smem[k] = x & 0xFF
            s.smem[k + 1] = (x >> 8) & 0xFF
        i = s.sim
        for n, v in [('rst', 1), ('pix_rst_n', 0), ('apb_psel', 0), ('apb_penable', 0),
                     ('apb_pwrite', 0), ('apb_paddr', 0), ('apb_pwdata', 0), ('s_arready', 0),
                     ('s_rvalid', 0), ('s_rdata', 0), ('s_rlast', 0), ('clk', 0), ('pix_clk', 0)]:
            i[n] = v
        for _ in range(8):
            s.step(check=False)
        i['rst'] = 0
        i['pix_rst_n'] = 1

    def word(s, a):
        if a >= SBASE and a < SBASE + len(s.smem):
            o = a - SBASE
            return s.smem[o] | s.smem[o + 1] << 8 | s.smem[o + 2] << 16 | s.smem[o + 3] << 24
        return H.fb_word(a)

    def step(s, check=True):
        i = s.sim
        s._apb()
        i['s_arready'] = 1 if s.rnd.random() < 0.7 else 0
        rv = bool(s.ar) and s.ar[0][2] == 0 and not i['rst']
        i['s_rvalid'] = 1 if rv else 0
        if rv:
            i['s_rdata'] = s.word(s.ar[0][0])
            i['s_rlast'] = 1 if s.ar[0][1] == 1 else 0
        else:
            i['s_rlast'] = 0
        i.comb()
        if i['s_arvalid'] and i['s_arready'] and not i['rst']:
            n = i['s_arlen'] + 1
            if n == 8:
                s.mixer_bursts += 1
            s.ar.append([i['s_araddr'], n, s.rnd.randint(2, 30)])
        hv = (i['h_cnt'] if 'h_cnt' in i.idx else i['u_hdmi_fb.h_cnt'],
              i['u_hdmi_fb.v_cnt'])
        sym = (i['tmds_d0'], i['tmds_d1'], i['tmds_d2'])
        en1 = i['u_hdmi_fb.frame_en']
        i.clock()
        if rv:
            a = s.ar[0]
            a[0] += 4
            a[1] -= 1
            if a[1] == 0:
                s.ar.pop(0)
        elif s.ar and s.ar[0][2] > 0:
            s.ar[0][2] -= 1
        s.hv.append(hv + (en1,))
        if len(s.hv) > H.LAT + 1:
            s.hv.pop(0)
        if check and len(s.hv) > H.LAT:
            h, v, en = s.hv[-1 - H.LAT]
            s.char(h, v, en, sym)
        s.t += 1

    def run_frames(s, nf):
        f0 = s.sim['u_hdmi_fb.frame_cnt']
        while s.sim['u_hdmi_fb.frame_cnt'] < f0 + nf:
            s.step()


def main():
    nf = int(sys.argv[1]) if len(sys.argv) > 1 else 4
    tb = AV(seed=7)
    print('[av] mixer -> HDMI, %d frames of %d lines' % (nf, tb.v_tot))
    # model with the same sample memory at SBASE
    model = X.Model(None)

    class MemView:
        def __getitem__(self, a):
            return tb.smem[a - SBASE]
    model.mem = MemView()
    MB = 0x2000
    voices = [
        dict(addr=SBASE + 0x0000, len=4000, lst=100, lpen=1, step=0x1000, fmt=1, vl=200, vr=50, start=0),
        dict(addr=SBASE + 0x4000, len=2000, lst=0, lpen=1, step=0x1000 * 44100 // 48000, fmt=2, vl=128, vr=128, start=0),
        dict(addr=SBASE + 0x9001, len=3000, lst=0, lpen=0, step=0x1800, fmt=0, vl=256, vr=256, start=5),
        dict(addr=SBASE + 0xC000, len=90, lst=10, lpen=1, step=0x0E00, fmt=1, vl=90, vr=180, start=0),
    ]
    tb.apb_w(MB + 0x00C, 32)
    for k, vd in enumerate(voices):
        base = MB + 0x200 + k * 0x20
        tb.apb_w(base + 0x00, vd['addr'])
        tb.apb_w(base + 0x04, vd['len'])
        tb.apb_w(base + 0x08, vd['lst'] | vd['lpen'] << 31)
        tb.apb_w(base + 0x0C, vd['step'] | vd['fmt'] << 16)
        tb.apb_w(base + 0x10, vd['vl'] | vd['vr'] << 16)
        tb.apb_w(base + 0x14, vd['start'])
        vo = model.v[k]
        for f, val in vd.items():
            setattr(vo, f, val)
        vo.kon = True
    tb.apb_w(MB + 0x004, 1)                 # enable (silence until KEYON)
    tb.apb_w(0x00, 0x1)                     # HDMI scan-out on
    tb.run_frames(1)
    tb.apb_w(MB + 0x010, 0xF)               # KEYON voices 0..3
    tb.run_frames(nf)
    got = [(r[0], r[1]) for r in tb.rx]
    k0 = 0
    while k0 < len(got) and got[k0] == (0, 0):
        k0 += 1
    exp = []
    while len(exp) < len(got) - k0:
        w = model.mix()
        exp.append((w & 0xFFFF, w >> 16))
    bad = [k for k, (a, b) in enumerate(zip(got[k0:], exp)) if a != b]
    if bad:
        H.err('av: sample %d got %s exp %s' % (bad[0], got[k0 + bad[0]], exp[bad[0]]))
    print('  av: %d samples (%d leading silence), %d match the mixer model; mixer bursts %d'
          % (len(got), k0, len(got) - k0 - len(bad), tb.mixer_bursts))
    uf = tb.apb_r(0x2C)
    print('  av: AUD_UF %d, mixer SAMPLES %d MISSES %d' % (uf, tb.apb_r(MB + 0x1C), tb.apb_r(MB + 0x20)))
    if uf:
        H.err('av: FIFO underflow %d' % uf)
    if len(got) - k0 < 200:
        H.err('av: too few audio samples')
    H.acr_and_if(tb, 'av')
    print('FAIL (%d errors)' % len(H.errors) if H.errors else 'PASS')
    return 1 if H.errors else 0


if __name__ == '__main__':
    sys.exit(main())
