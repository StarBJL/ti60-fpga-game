#!/usr/bin/env python3
"""
tb_hdmi.py - self-checking testbench for rtl/hdmi (hdmi_fb_top + hdmi_island).

  python tb_hdmi.py [test] [frames]     test = all | dvi | hdmi | tone | uf | full
                                        (full = real 640x480 timing, 1 frame)

Single clock: pix_clk and clk tick together (vsim.py has one clock), so the
async FIFOs run with equal clocks. The TMDS output of each channel is decoded
independently of the RTL:

* video characters: bit-exact against a Python model of encode.v (XAPP460),
  decoded RGB checked against the colour bars / the DMA pattern
* every blanking character classified: control token (CTL bits, HSYNC/VSYNC
  checked against the timing), video preamble / guard band only right before
  active lines, data island = preamble 8 + guard 2 + 32*n TERC4 + guard 2
* TERC4 legality, channel 0 bit 3 = 0 only on the first island character,
  HSYNC/VSYNC inside islands, BCH ECC of header and all 4 subpackets
* packets: ACR N/CTS, AVI / Audio InfoFrame checksum and fields, audio sample
  packets (sample_present, parity, channel status bits, block start every 192)
* audio: the samples received equal the samples pushed into the FIFO, in order,
  at 48 kHz (count vs clocks), FIFO underflow -> silence + AUD_UF counter,
  test tone = 1 kHz sine table, ACR every 25000 clocks, InfoFrames every frame
"""
import os, sys, random
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'blit'))
from vsim import Sim

RTL = os.path.join(HERE, '..', '..', 'rtl', 'hdmi')
FILES = [os.path.join(RTL, 'hdmi_fb.v'), os.path.join(RTL, 'hdmi_island.v')]

# TB_HDMI=720: 1280x720@60 parameters (74.375 MHz board clock, positive syncs, CTS 74375, VIC 4 16:9)
MODE720 = os.environ.get('TB_HDMI', '480') == '720'
if MODE720:
    H_ACT, H_FP, H_SYNC, H_BP = 1280, 110, 40, 220
    V_FP, V_SYNC, V_BP = 5, 5, 20
    POL, CTS, F_PIX, VIC, PB2, FIFO_AW = 1, 74375, 74.375e6, 4, 0x28, 11   # board PLL: 74.375 MHz
else:
    H_ACT, H_FP, H_SYNC, H_BP = 640, 16, 96, 42
    V_FP, V_SYNC, V_BP = 10, 2, 33
    POL, CTS, F_PIX, VIC, PB2, FIFO_AW = 0, 25000, 25e6, 1, 0x18, 10
H_TOT = H_ACT + H_FP + H_SYNC + H_BP
LAT = 5                                  # h_cnt -> tmds_d* latency (clocks)

CTRL = {0b1101010100: 0, 0b0010101011: 1, 0b0101010100: 2, 0b1010101011: 3}
TERC4 = [0b1010011100, 0b1001100011, 0b1011100100, 0b1011100010,
         0b0101110001, 0b0100011110, 0b0110001110, 0b0100111100,
         0b1011001100, 0b0100111001, 0b0110011100, 0b1011000110,
         0b1010001110, 0b1001110001, 0b0101100011, 0b1011000011]
TERC4_INV = {v: i for i, v in enumerate(TERC4)}
GB_A, GB_B = 0b1011001100, 0b0100110011

TONE = []
import math
for i in range(48):
    TONE.append(int(round(8192 * math.sin(2 * math.pi * i / 48))) & 0xFFFF)

errors = []


def err(msg):
    errors.append(msg)
    if len(errors) <= 30:
        print('  ERROR', msg)


# ------------------------------------------------------------------ reference models
class TmdsRef:
    """encode.v (XAPP460) video path, one channel."""
    def __init__(s):
        s.cnt = 0

    def video(s, d):
        n1d = bin(d).count('1')
        dec1 = n1d > 4 or (n1d == 4 and (d & 1) == 0)
        q = d & 1
        prev = q
        for i in range(1, 8):
            b = (d >> i) & 1
            nb = (1 - (prev ^ b)) if dec1 else (prev ^ b)
            q |= nb << i
            prev = nb
        q8 = 0 if dec1 else 1
        n1 = bin(q & 0xFF).count('1')
        n0 = 8 - n1
        cnt = s.cnt
        if cnt == 0 or n1 == n0:
            out = ((1 - q8) << 9) | (q8 << 8) | (q if q8 else (~q & 0xFF))
            cnt = cnt + (n0 - n1 if q8 == 0 else n1 - n0)
        elif (cnt > 0 and n1 > n0) or (cnt < 0 and n0 > n1):
            out = (1 << 9) | (q8 << 8) | (~q & 0xFF)
            cnt = cnt + 2 * q8 + (n0 - n1)
        else:
            out = (q8 << 8) | q
            cnt = cnt - 2 * (1 - q8) + (n1 - n0)
        s.cnt = ((cnt + 16) % 32) - 16          # 5-bit counter as in RTL
        return out

    def other(s):
        s.cnt = 0


def tmds_decode(sym):
    d = sym & 0xFF
    if sym & 0x200:
        d ^= 0xFF
    out = d & 1
    for i in range(1, 8):
        b = ((d >> i) ^ (d >> (i - 1))) & 1
        if not (sym & 0x100):
            b ^= 1
        out |= b << i
    return out


def bch(bits):
    e = 0
    for b in bits:
        fb = (e ^ b) & 1
        e >>= 1
        if fb:
            e ^= 0x83
    return e


def bar_rgb(h, v):
    bars = [0xFFFFFF, 0xFFFF00, 0x00FFFF, 0x00FF00, 0xFF00FF, 0xFF0000, 0x0000FF, 0x000000]
    c = bars[(h >> 7) & 7]
    if ((v >> 5) & 0xF) == 0xE:
        g = (h >> 2) & 0xFF
        c = (g << 16) | (g << 8) | g
    return c


def fb_word(addr):
    return (addr * 2654435761 + 0x1234567) & 0xFFFFFFFF


# ------------------------------------------------------------------ testbench
class TB:
    def __init__(s, v_act, seed=1):
        s.v_act = v_act
        s.v_tot = v_act + V_FP + V_SYNC + V_BP
        prm = {'V_ACT': v_act}
        if MODE720:
            prm.update(H_ACT=H_ACT, H_FP=H_FP, H_SYNC=H_SYNC, H_BP=H_BP, V_FP=V_FP, V_SYNC=V_SYNC,
                       V_BP=V_BP, HS_POL=POL, VS_POL=POL, ACR_CTS=CTS, AVI_VIC=VIC, AVI_PB2=PB2,
                       FIFO_AW=FIFO_AW)
        s.sim = Sim(FILES, 'hdmi_fb_top', seed=seed, params=prm)
        s.rnd = random.Random(seed)
        s.t = 0
        s.hv = []
        s.ref = [TmdsRef(), TmdsRef(), TmdsRef()]
        s.apbq = []
        s.apb_state = 0
        s.apb_rd = None
        s.ar = []            # pending read bursts [addr, beats_left, delay]
        s.push = False
        s.push_level = 48
        s.pushed = []        # samples written into the FIFO
        s.k = 0
        s.rx = []            # samples received from audio packets (L, R, b, cl, cr)
        s.rx_t = []
        s.pk = []            # (t, hdr bytes, sub bytes list) all packets
        s.line_syms = []
        s.line_v = None
        s.fb_base = 0x01000000
        s.scan = False       # scan-out enabled (DMA data expected)
        s.frame_start_scan = {}
        s.nvid = s.nvid_dma = s.nlines = s.nisl = s.nvpre = 0
        i = s.sim
        for n, v in [('rst', 1), ('axi_rst', 1), ('pix_rst_n', 0), ('apb_psel', 0),
                     ('apb_penable', 0), ('apb_pwrite', 0), ('apb_paddr', 0), ('apb_pwdata', 0),
                     ('axi_arready', 0), ('axi_rvalid', 0), ('axi_rdata', 0), ('axi_rlast', 0),
                     ('axi_rresp', 0), ('axi_rid', 0), ('aud_wvalid', 0), ('aud_wdata', 0),
                     ('clk', 0), ('pix_clk', 0)]:
            i[n] = v
        for _ in range(8):
            s.step(check=False)
        i['rst'] = 0
        i['axi_rst'] = 0
        i['pix_rst_n'] = 1

    # -------------------------------------------------- APB
    def apb_w(s, a, d):
        s.apbq.append((1, a, d))

    def apb_r(s, a):
        s.apbq.append((0, a, 0))
        while s.apbq or s.apb_state:
            s.step()
        return s.apb_rd

    def _apb(s):
        i = s.sim
        if s.apb_state == 0 and s.apbq:
            w, a, d = s.apbq.pop(0)
            i['apb_psel'] = 1; i['apb_penable'] = 0
            i['apb_pwrite'] = w; i['apb_paddr'] = a; i['apb_pwdata'] = d
            s.apb_state = 1
        elif s.apb_state == 1:
            i['apb_penable'] = 1
            s.apb_state = 2
        elif s.apb_state == 2:
            s.apb_rd = i['apb_prdata']
            i['apb_psel'] = 0; i['apb_penable'] = 0; i['apb_pwrite'] = 0
            s.apb_state = 0

    # -------------------------------------------------- one clock
    def step(s, check=True):
        i = s.sim
        s._apb()
        # AXI slave: accept AR, return beats with a little latency
        i['axi_arready'] = 1 if s.rnd.random() < 0.7 else 0
        i.comb()
        rv = 0
        if s.ar and s.ar[0][2] == 0:
            a = s.ar[0]
            rv = 1
            i['axi_rvalid'] = 1
            i['axi_rdata'] = fb_word(a[0])
            i['axi_rlast'] = 1 if a[1] == 1 else 0
        else:
            i['axi_rvalid'] = 0
            i['axi_rlast'] = 0
        if s.ar and s.ar[0][2] > 0:
            s.ar[0][2] -= 1
        # audio FIFO feeder
        lvl = i['aud_level']
        if s.push and lvl < s.push_level and s.rnd.random() < 0.8:
            smp = ((s.k * 37 + 11) & 0xFFFF) | ((((s.k * 1001) ^ 0x5A5A) & 0xFFFF) << 16)
            i['aud_wvalid'] = 1
            i['aud_wdata'] = smp
        else:
            i['aud_wvalid'] = 0
        i.comb()
        if i['aud_wvalid'] and i['aud_wready']:
            s.pushed.append(i['aud_wdata'])
            s.k += 1
        ar_take = i['axi_arvalid'] and i['axi_arready']
        if ar_take:
            assert i['axi_arlen'] == 63
            s.ar.append([i['axi_araddr'], 64, s.rnd.randint(2, 20)])
        hv = (i['h_cnt'], i['v_cnt'])
        sym = (i['tmds_d0'], i['tmds_d1'], i['tmds_d2'])
        en1 = i['frame_en']
        i.clock()
        if rv:
            a = s.ar[0]
            a[0] += 4
            a[1] -= 1
            if a[1] == 0:
                s.ar.pop(0)
        s.hv.append(hv + (en1,))
        if len(s.hv) > LAT + 1:
            s.hv.pop(0)
        if check and len(s.hv) > LAT:
            h, v, en = s.hv[-1 - LAT]
            s.char(h, v, en, sym)
        s.t += 1

    # -------------------------------------------------- per-character checks
    def char(s, h, v, en, sym):
        if h == 0:
            if s.line_v is not None:
                s.check_line(s.line_v, s.line_syms)
            s.line_syms = []
            s.line_v = v
        s.line_syms.append((h, v, en, sym))

    def sync_levels(s, h, v):
        hs = 1 if H_ACT + H_FP <= h < H_ACT + H_FP + H_SYNC else 0
        vs = 1 if s.v_act + V_FP <= v < s.v_act + V_FP + V_SYNC else 0
        if POL:
            return hs, vs                        # positive polarity (720p)
        return 1 - hs, 1 - vs                    # negative polarity

    def check_line(s, v, syms):
        hdmi = s.hdmi
        n = len(syms)
        if n != H_TOT:
            return                               # partial first line
        s.nlines += 1
        act_line = v < s.v_act
        next_act = (v < s.v_act - 1) or (v == s.v_tot - 1)
        h = 0
        while h < H_TOT:
            hh, vv, en, (c0, c1, c2) = syms[h]
            assert hh == h
            if act_line and h < H_ACT:
                # video
                if hdmi or True:
                    if en:
                        a = s.fb_base + (v * H_ACT + h) * 2
                        w = fb_word(a & ~3)
                        p = (w >> 16) if (a & 2) else (w & 0xFFFF)
                        exp = (((p >> 11) & 31) << 3 | ((p >> 13) & 7)) << 16 | \
                              (((p >> 5) & 63) << 2 | ((p >> 9) & 3)) << 8 | \
                              ((p & 31) << 3 | ((p >> 2) & 7))
                    else:
                        exp = bar_rgb(h, v)
                    ex = [exp & 0xFF, (exp >> 8) & 0xFF, exp >> 16]
                    s.nvid += 1
                    s.nvid_dma += 1 if en else 0
                    for ch, sy in enumerate((c0, c1, c2)):
                        rs = s.ref[ch].video(ex[ch])
                        if rs != sy:
                            err('t%d v%d h%d ch%d video sym %03x != ref %03x' % (s.t, v, h, ch, sy, rs))
                            return
                        if tmds_decode(sy) != ex[ch]:
                            err('v%d h%d ch%d decode' % (v, h, ch))
                h += 1
                continue
            for r in s.ref:
                r.other()
            hs, vs = s.sync_levels(h, v)
            # data island?
            if hdmi and h == H_ACT + 4 and c1 in CTRL and CTRL.get(c2) == 1:
                h = s.check_island(v, syms, h)
                continue
            # video preamble / guard
            if hdmi and next_act and H_TOT - 10 <= h < H_TOT - 2:
                if not (CTRL.get(c1) == 1 and CTRL.get(c2) == 0 and CTRL.get(c0) == (vs << 1 | hs)):
                    err('v%d h%d bad video preamble %03x %03x %03x' % (v, h, c0, c1, c2))
                h += 1
                continue
            if hdmi and next_act and h == H_TOT - 2:
                s.nvpre += 1
            if hdmi and next_act and h >= H_TOT - 2:
                if (c0, c1, c2) != (GB_A, GB_B, GB_A):
                    err('v%d h%d bad video guard' % (v, h))
                h += 1
                continue
            # plain control
            if not (CTRL.get(c0) == (vs << 1 | hs) and CTRL.get(c1) == 0 and CTRL.get(c2) == 0):
                err('t%d v%d h%d bad control %03x %03x %03x (hs%d vs%d)' % (s.t, v, h, c0, c1, c2, hs, vs))
                return
            h += 1

    def check_island(s, v, syms, h0):
        h = h0
        for k in range(8):
            hh, vv, en, (c0, c1, c2) = syms[h]
            hs, vs = s.sync_levels(h, v)
            if not (CTRL.get(c1) == 1 and CTRL.get(c2) == 1 and CTRL.get(c0) == (vs << 1 | hs)):
                err('v%d h%d bad island preamble' % (v, h))
            h += 1
        for k in range(2):
            hh, vv, en, (c0, c1, c2) = syms[h]
            hs, vs = s.sync_levels(h, v)
            if c1 != GB_B or c2 != GB_B or TERC4_INV.get(c0) != (0xC | vs << 1 | hs):
                err('v%d h%d bad leading guard' % (v, h))
            h += 1
        chars = []
        while True:
            hh, vv, en, (c0, c1, c2) = syms[h]
            if c1 == GB_B and c2 == GB_B:
                break
            hs, vs = s.sync_levels(h, v)
            t0, t1, t2 = TERC4_INV.get(c0), TERC4_INV.get(c1), TERC4_INV.get(c2)
            if t0 is None or t1 is None or t2 is None:
                err('v%d h%d illegal TERC4' % (v, h))
                return h + 1
            if (t0 & 3) != (vs << 1 | hs):
                err('v%d h%d island sync bits' % (v, h))
            b3 = (t0 >> 3) & 1
            if b3 != (0 if not chars else 1):
                err('v%d h%d ch0 bit3 = %d' % (v, h, b3))
            chars.append((t0, t1, t2))
            h += 1
        n = len(chars)
        s.nisl += 1
        if n == 0 or n % 32 or n > 96:
            err('v%d island length %d' % (v, n))
            return h
        for k in range(2):
            hh, vv, en, (c0, c1, c2) = syms[h]
            hs, vs = s.sync_levels(h, v)
            if c1 != GB_B or c2 != GB_B or TERC4_INV.get(c0) != (0xC | vs << 1 | hs):
                err('v%d h%d bad trailing guard' % (v, h))
            h += 1
        if h > H_TOT - 12:
            err('v%d island too long for the control period' % v)
        for p in range(n // 32):
            s.packet(v, chars[p * 32:(p + 1) * 32])
        return h

    def packet(s, v, ch):
        hb = [(c[0] >> 2) & 1 for c in ch]
        if bch(hb[:24]) != sum(b << i for i, b in enumerate(hb[24:])):
            err('v%d header ECC' % v)
        hdr = [sum(hb[8 * j + i] << i for i in range(8)) for j in range(3)]
        subs = []
        for j in range(4):
            bits = []
            for c in ch:
                bits.append((c[1] >> j) & 1)
                bits.append((c[2] >> j) & 1)
            if bch(bits[:56]) != sum(b << i for i, b in enumerate(bits[56:])):
                err('v%d subpacket %d ECC' % (v, j))
            subs.append([sum(bits[8 * k + i] << i for i in range(8)) for k in range(7)])
        s.pk.append((s.t, v, hdr, subs))
        ty = hdr[0]
        if ty == 0x02:
            pres = hdr[1] & 0xF
            if hdr[1] & 0xF0:
                err('audio HB1 %02x' % hdr[1])
            if pres not in (1, 3, 7, 15):
                err('audio sample_present %x' % pres)
            if hdr[2] & 0x0F:
                err('audio flat bits set')
            for j in range(4):
                sb = subs[j]
                if not (pres >> j) & 1:
                    if any(sb):
                        err('empty subpacket not zero')
                    if (hdr[2] >> (4 + j)) & 1:
                        err('B bit on empty subpacket')
                    continue
                L = sb[1] | sb[2] << 8
                R = sb[4] | sb[5] << 8
                if sb[0] or sb[3]:
                    err('low audio byte not zero')
                b6 = sb[6]
                cl, cr = (b6 >> 4) & 1, (b6 >> 5) & 1
                if b6 & 0x0F:
                    err('V/U bits set')
                if cl != cr:
                    err('C bits differ')
                pl = (bin(L).count('1') + cl) & 1
                pr = (bin(R).count('1') + cr) & 1
                if (b6 >> 6) & 1 != pl or (b6 >> 7) & 1 != pr:
                    err('audio parity')
                s.rx.append((L, R, (hdr[2] >> (4 + j)) & 1, cl))
                s.rx_t.append(s.t)
        elif ty == 0x01:
            if hdr[1] or hdr[2]:
                err('ACR header')
            for sb in subs:
                cts = (sb[1] & 0xF) << 16 | sb[2] << 8 | sb[3]
                nn = (sb[4] & 0xF) << 16 | sb[5] << 8 | sb[6]
                if (cts, nn, sb[0]) != (CTS, 6144, 0):
                    err('ACR N/CTS %d %d' % (nn, cts))
        elif ty in (0x82, 0x84):
            ln = hdr[2]
            pb = sum(subs, [])
            if (sum(hdr) + sum(pb[:ln + 1])) & 0xFF:
                err('InfoFrame %02x checksum' % ty)
            if any(pb[ln + 1:]):
                err('InfoFrame %02x padding' % ty)
            if ty == 0x82 and (hdr[1], ln, pb[4], pb[1] >> 5, pb[2]) != (2, 13, VIC, 0, PB2):
                err('AVI fields')
            if ty == 0x84 and (hdr[1], ln, pb[1] & 7) != (1, 10, 1):
                err('Audio InfoFrame fields')
        else:
            err('unknown packet type %02x' % ty)

    def run_frames(s, nf):
        f0 = s.sim['frame_cnt']
        while s.sim['frame_cnt'] < f0 + nf:
            s.step()
        print('    (checked %d lines, %d video chars of which %d from DMA, %d video guards, %d islands, %d packets)'
              % (s.nlines, s.nvid, s.nvid_dma, s.nvpre, s.nisl, len(s.pk)))


# ------------------------------------------------------------------ tests
def summary_audio(tb, name, expect, t_from):
    rx = tb.rx
    # sample rate
    if len(tb.rx_t) > 400:
        dt = tb.rx_t[-1] - tb.rx_t[200]
        rate = (len(tb.rx_t) - 1 - 200) / dt * F_PIX
        if abs(rate - 48000) > 300:
            err('%s: sample rate %.0f' % (name, rate))
        print('  %s: %d samples, rate %.1f Hz' % (name, len(rx), rate))
    # block start every 192, channel status bits 2 / 25 / 33
    bidx = [k for k, r in enumerate(rx) if r[2]]
    for a, b in zip(bidx, bidx[1:]):
        if b - a != 192:
            err('%s: B flags %d apart' % (name, b - a))
    if bidx:
        b0 = bidx[0]
        cs = [rx[b0 + k][3] for k in range(min(192, len(rx) - b0))]
        exp = [1 if k in (2, 25, 33) else 0 for k in range(len(cs))]
        if cs != exp:
            err('%s: channel status bits' % name)
    if expect is not None:
        got = [(r[0], r[1]) for r in rx]
        exp = expect(len(got))
        bad = [k for k, (a, b) in enumerate(zip(got, exp)) if a != b]
        if bad:
            err('%s: sample %d got %s exp %s' % (name, bad[0], got[bad[0]], exp[bad[0]]))
        else:
            print('  %s: sample sequence OK (%d)' % (name, len(got)))


def acr_and_if(tb, name):
    acr = [p[0] for p in tb.pk if p[2][0] == 0x01]
    if len(acr) > 3:
        d = [b - a for a, b in zip(acr, acr[1:])]
        if min(d) < CTS - H_TOT or max(d) > CTS + H_TOT:
            err('%s: ACR spacing %d..%d' % (name, min(d), max(d)))
        print('  %s: %d ACR, spacing %d..%d clocks' % (name, len(acr), min(d), max(d)))
    avi = [p[1] for p in tb.pk if p[2][0] == 0x82]
    aif = [p[1] for p in tb.pk if p[2][0] == 0x84]
    if any(v != tb.v_act for v in avi) or any(v != tb.v_act + 1 for v in aif):
        err('%s: InfoFrame lines' % name)
    print('  %s: AVI x%d, AIF x%d' % (name, len(avi), len(aif)))


def test_dvi(v_act=24):
    print('[dvi] HDMI mode off: plain DVI, bit-exact video, no islands')
    tb = TB(v_act, seed=2)
    tb.hdmi = False
    tb.apb_w(0x20, 0x30)                # HDMI off
    tb.apb_w(0x00, 0x1)                 # scan-out on
    tb.run_frames(3)
    if tb.pk:
        err('dvi: packets sent')


def test_hdmi(v_act=24, frames=4):
    print('[hdmi] HDMI + audio from FIFO, scan-out and colour bars')
    tb = TB(v_act, seed=3)
    tb.hdmi = True
    tb.push = True
    tb.apb_w(0x00, 0x1)
    tb.run_frames(frames)
    tb.apb_w(0x00, 0x0)                 # back to colour bars
    tb.run_frames(2)
    # pushed sequence must be received in order (skip the start-up silence)
    got = [(r[0], r[1]) for r in tb.rx]
    exp = [(p & 0xFFFF, p >> 16) for p in tb.pushed]
    k0 = 0
    while k0 < len(got) and got[k0] == (0, 0):
        k0 += 1
    got = got[k0:]
    if got != exp[:len(got)]:
        bad = next(k for k, (a, b) in enumerate(zip(got, exp)) if a != b)
        err('hdmi: sample %d got %s exp %s' % (bad, got[bad], exp[bad]))
    else:
        print('  hdmi: %d samples equal to the pushed sequence (%d leading silence)' % (len(got), k0))
    summary_audio(tb, 'hdmi', None, 0)
    acr_and_if(tb, 'hdmi')
    sent = tb.apb_r(0x28)
    uf = tb.apb_r(0x2C)
    print('  hdmi: AUD_SENT %d AUD_UF %d (rx %d)' % (sent, uf, len(tb.rx)))
    if uf != k0:
        err('hdmi: AUD_UF %d, expected %d' % (uf, k0))
    if abs(sent - len(tb.rx)) > 4:
        err('hdmi: AUD_SENT %d vs received %d' % (sent, len(tb.rx)))


def test_tone(v_act=24):
    print('[tone] 1 kHz test tone, left only, plus APB AUD_DATA push')
    tb = TB(v_act, seed=4)
    tb.hdmi = True
    tb.apb_w(0x20, 0x17)                # HDMI, audio, tone, L only
    tb.run_frames(2)
    rx = tb.rx
    k0 = next(k for k, r in enumerate(rx) if r[0] != 0) - 1
    ph = 0
    ok = True
    for r in rx[k0:]:
        if r[0] != TONE[ph] or r[1] != 0:
            ok = False
            break
        ph = (ph + 1) % 48
    if not ok:
        err('tone: sine sequence')
    else:
        print('  tone: %d samples follow the sine table' % len(rx[k0:]))
    summary_audio(tb, 'tone', None, 0)
    # APB push: 10 samples, then FIFO drains to silence
    n0 = len(tb.rx)
    tb.apb_w(0x20, 0x33)
    for k in range(10):
        tb.apb_w(0x30, (0x1000 + k) | ((0x2000 + k) << 16))
    lvl = tb.apb_r(0x24)
    tb.run_frames(1)
    got = [(r[0], r[1]) for r in tb.rx[n0:] if r[0] >= 0x1000 and r[0] < 0x1010]
    if got != [(0x1000 + k, 0x2000 + k) for k in range(10)]:
        err('tone: APB pushed samples %s' % got[:4])
    else:
        print('  tone: APB AUD_DATA samples received in order (level read %d)' % lvl)


def test_uf(v_act=24):
    print('[uf] audio off -> no audio packets; FIFO empty -> silence and AUD_UF')
    tb = TB(v_act, seed=5)
    tb.hdmi = True
    tb.apb_w(0x20, 0x31)                # HDMI, no audio
    tb.run_frames(2)
    if any(p[2][0] in (0x01, 0x02, 0x84) for p in tb.pk):
        err('uf: audio packets while audio off')
    if not any(p[2][0] == 0x82 for p in tb.pk):
        err('uf: no AVI InfoFrame')
    tb.apb_w(0x20, 0x33)
    tb.run_frames(1)
    uf = tb.apb_r(0x2C)
    if uf == 0 or any(r[0] or r[1] for r in tb.rx):
        err('uf: expected silence + underflow count, uf=%d' % uf)
    print('  uf: %d silent samples, AUD_UF %d' % (len(tb.rx), uf))


def test_full():
    print('[full] %dx%d timing, 2 frames, HDMI + audio' % (H_ACT, 720 if MODE720 else 480))
    tb = TB(720 if MODE720 else 480, seed=6)
    tb.hdmi = True
    tb.push = True
    tb.apb_w(0x00, 0x1)
    tb.run_frames(2)
    summary_audio(tb, 'full', None, 0)
    acr_and_if(tb, 'full')


if __name__ == '__main__':
    which = sys.argv[1] if len(sys.argv) > 1 else 'all'
    tests = {'dvi': test_dvi, 'hdmi': test_hdmi, 'tone': test_tone, 'uf': test_uf, 'full': test_full}
    for name, fn in tests.items():
        if which in ('all', name) and not (which == 'all' and name == 'full'):
            fn()
    print('FAIL (%d errors)' % len(errors) if errors else 'PASS')
    sys.exit(1 if errors else 0)
