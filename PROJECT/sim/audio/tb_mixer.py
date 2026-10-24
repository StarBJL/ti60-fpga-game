#!/usr/bin/env python3
"""
tb_mixer.py - self-checking testbench for rtl/audio/audio_mixer.v (+ aud_rd_mux).

  python tb_mixer.py [seed] [nsamples]      TB_LAT=min,max[,longp]  AXI read latency

* DDR model: random sample data, AXI read slave with random latency (and an
  occasional very long one), competing HDMI-like bursts through aud_rd_mux,
  every beat checked to come back to the right master in order.
* Sink model: the HDMI FIFO level (pushes - pops), popped at a random rate.
* Golden model: per output sample, every voice in order (key-on -> START,
  linear interpolation, loop / one-shot end, volume, master volume, saturation),
  bit-exact. Configuration changes / KEYON / KEYOFF are applied while the mixer
  is idle (sink held), so the model knows exactly from which sample they count.
* Protocol: AR 8 beats, 32-byte aligned; voice-register writes stalled with
  PREADY while the mixer writes back a position are retried correctly.
"""
import os, sys, random
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'blit'))
from vsim import Sim

RTL = os.path.join(HERE, '..', '..', 'rtl', 'audio')
FILES = [os.path.join(RTL, 'audio_mixer.v'), os.path.join(HERE, 'tb_mixer_top.v')]

LAT = [int(x) for x in os.environ.get('TB_LAT', '4,60,0.02').split(',')[:2]]
LONGP = float(os.environ.get('TB_LAT', '4,60,0.02').split(',')[2]) if os.environ.get('TB_LAT', '').count(',') == 2 else 0.02
MEM = 1 << 20
errors = []


def err(m):
    errors.append(m)
    if len(errors) <= 20:
        print('  ERROR', m)


def s16(x):
    x &= 0xFFFF
    return x - 0x10000 if x & 0x8000 else x


class Voice:
    def __init__(s):
        s.addr = s.len = s.lst = s.step = s.fmt = s.vl = s.vr = s.start = 0
        s.lpen = 0
        s.pos = 0
        s.active = False
        s.kon = False


class Model:
    def __init__(s, mem):
        s.mem = mem
        s.v = [Voice() for _ in range(16)]
        s.mvol = 256

    def sample(s, vo, i):
        fmt = 1 if vo.fmt == 3 else vo.fmt
        if fmt == 0:
            b = s.mem[(vo.addr + i) & 0xFFFFFFF]
            b = b - 256 if b & 0x80 else b
            return b * 256, b * 256
        if fmt == 1:
            a = (vo.addr + 2 * i) & 0xFFFFFFF
            x = s16(s.mem[a] | s.mem[a + 1] << 8)
            return x, x
        a = (vo.addr + 4 * i) & 0xFFFFFFF
        return s16(s.mem[a] | s.mem[a + 1] << 8), s16(s.mem[a + 2] | s.mem[a + 3] << 8)

    def mix(s):
        al = ar = 0
        for vo in s.v:
            if not (vo.active or vo.kon):
                continue
            if vo.kon:
                vo.pos = vo.start << 12
                vo.active = True
                vo.kon = False
            i0 = vo.pos >> 12
            if i0 >= vo.len:
                vo.active = False
                continue
            i1 = i0 + 1
            has1 = i1 < vo.len or vo.lpen
            if i1 >= vo.len:
                i1 = vo.lst
            s0 = s.sample(vo, i0)
            s1 = s.sample(vo, i1) if has1 else s0
            fr = vo.pos & 0xFFF
            il = s0[0] + (((s1[0] - s0[0]) * fr) >> 12)
            ir = s0[1] + (((s1[1] - s0[1]) * fr) >> 12)
            al += il * vo.vl
            ar += ir * vo.vr
            np = vo.pos + vo.step
            ni = np >> 12
            if ni >= vo.len:
                if vo.lpen:
                    nwr = ni - (vo.len - vo.lst)
                    vo.pos = ((vo.lst if nwr >= vo.len else nwr) << 12) | (np & 0xFFF)
                else:
                    vo.pos = np
                    vo.active = False
            else:
                vo.pos = np
        out = []
        for a in (al, ar):
            x = ((a >> 8) * s.mvol) >> 8
            out.append(max(-32768, min(32767, x)) & 0xFFFF)
        return out[0] | out[1] << 16


class TB:
    def __init__(s, seed):
        s.rnd = random.Random(seed)
        s.mem = bytearray(s.rnd.getrandbits(8) for _ in range(MEM))
        # smooth regions too (realistic audio: small deltas)
        for base in range(0x40000, 0x80000, 2):
            x = int(12000 * __import__('math').sin(base / 97.0))
            s.mem[base] = x & 0xFF
            s.mem[base + 1] = (x >> 8) & 0xFF
        s.sim = Sim(FILES, 'tb_mixer_top', seed=seed)
        s.model = Model(s.mem)
        s.level = 0
        s.hold = True
        s.got = []
        s.exp = []
        s.ar = []                # [master, addr, beats, delay]
        s.hd_pending = 0
        s.hd_exp = []            # expected HDMI beats (addr)
        s.t = 0
        s.apbq = []
        s.apb_state = 0
        s.apb_rd = 0
        s.stalls = 0
        i = s.sim
        for n, v in [('rst', 1), ('apb_psel', 0), ('apb_penable', 0), ('apb_pwrite', 0),
                     ('apb_paddr', 0), ('apb_pwdata', 0), ('s_arready', 0), ('s_rvalid', 0),
                     ('s_rdata', 0), ('s_rlast', 0), ('o_ready', 0), ('o_level', 0),
                     ('h_arvalid', 0), ('h_araddr', 0), ('clk', 0)]:
            i[n] = v
        for _ in range(5):
            s.step()
        i['rst'] = 0

    def word(s, a):
        a &= MEM - 1
        return s.mem[a] | s.mem[a + 1] << 8 | s.mem[a + 2] << 16 | s.mem[a + 3] << 24

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
            if i['apb_pready']:
                s.apb_rd = i['apb_prdata']
                i['apb_psel'] = 0; i['apb_penable'] = 0; i['apb_pwrite'] = 0
                s.apb_state = 0
            else:
                s.stalls += 1

    def step(s):
        i = s.sim
        r = s.rnd
        s._apb()
        # competing HDMI-like master: 64-beat bursts now and then
        if not i['h_arvalid'] and s.hd_pending < 3 and r.random() < 0.004:
            i['h_arvalid'] = 1
            i['h_araddr'] = 0x80000 + r.randrange(0, 0x1000) * 256
        i['s_arready'] = 1 if r.random() < 0.6 else 0
        # R channel: head burst returns when its delay expired
        rv = bool(s.ar) and s.ar[0][3] == 0 and not i['rst']
        i['s_rvalid'] = 1 if rv else 0
        if rv:
            m, a, n, d = s.ar[0]
            i['s_rdata'] = s.word(a)
            i['s_rlast'] = 1 if n == 1 else 0
        i['o_level'] = s.level
        i['o_ready'] = 1
        i.comb()
        if rv:
            m = s.ar[0][0]
            got_a, got_b = i['h_rvalid'], i['axi_rvalid_mix']
            if (m == 0 and not (got_a and not got_b)) or (m == 1 and not (got_b and not got_a)):
                err('t%d R beat routed to the wrong master' % s.t)
        if i['s_arvalid'] and i['s_arready'] and not i['rst']:
            ln = i['s_arlen']
            a = i['s_araddr']
            mixer = i['axi_arvalid_mix'] and i['axi_arready_mix']
            if mixer:
                if ln != 7 or a & 31 or i['s_arsize'] != 2 or i['s_arburst'] != 1:
                    err('mixer AR len %d addr %x' % (ln, a))
            d = r.randint(LAT[0], LAT[1])
            if r.random() < LONGP:
                d = r.randint(500, 3000)
            s.ar.append([1 if mixer else 0, a, ln + 1, d])
            if not mixer:
                s.hd_pending += 1
                i['h_arvalid'] = 0
        push = i['o_valid'] and i['o_ready'] and not i['rst']
        if push:
            s.got.append(i['o_data'])
            s.level += 1
        i.clock()
        if s.ar and s.ar[0][3] > 0:
            s.ar[0][3] -= 1
        elif rv:
            b = s.ar[0]
            b[1] += 4
            b[2] -= 1
            if b[2] == 0:
                if b[0] == 0:
                    s.hd_pending -= 1
                s.ar.pop(0)
        # sink pops (48 kHz relative to a fast clock)
        if not s.hold and s.level and r.random() < 0.004:
            s.level -= 1
        s.t += 1

    def run(s, n):
        for _ in range(n):
            s.step()

    def apb_w(s, a, d):
        s.apbq.append((1, a, d))

    def apb_r(s, a):
        s.apbq.append((0, a, 0))
        while s.apbq or s.apb_state:
            s.step()
        return s.apb_rd

    def sync(s):
        """hold the sink until the mixer is idle and full, then compare."""
        s.hold = True
        while True:
            s.step()
            if s.sim['u_mix.st'] == 0 and s.level >= s.sim['u_mix.target'] and not s.apbq and not s.apb_state:
                break
        while len(s.exp) < len(s.got):
            s.exp.append(s.model.mix())
        bad = [k for k, (a, b) in enumerate(zip(s.got, s.exp)) if a != b]
        if bad:
            k = bad[0]
            err('sample %d got %08x exp %08x' % (k, s.got[k], s.exp[k]))
            return False
        return True

    def flush(s):
        """apply queued APB writes while the mixer is idle."""
        while s.apbq or s.apb_state:
            s.step()

    def release(s, n):
        s.hold = False
        target = len(s.got) + n
        while len(s.got) < target:
            # rewrite a voice register with its current value while the mixer
            # runs: exercises the PREADY stall against the position write-back
            if not s.apbq and s.apb_state == 0 and s.rnd.random() < 0.3:
                k = s.rnd.randrange(16)
                vo = s.model.v[k]
                s.apb_w(0x200 + k * 0x20 + 0x10, vo.vl | (vo.vr << 16))
            s.step()
        s.flush()

    # voice programming (model updated at the same sample boundary)
    def voice(s, k, **kw):
        vo = s.model.v[k]
        base = 0x200 + k * 0x20
        for f, val in kw.items():
            setattr(vo, f, val)
        s.apb_w(base + 0x00, vo.addr)
        s.apb_w(base + 0x04, vo.len)
        s.apb_w(base + 0x08, vo.lst | (vo.lpen << 31))
        s.apb_w(base + 0x0C, vo.step | (vo.fmt << 16))
        s.apb_w(base + 0x10, vo.vl | (vo.vr << 16))
        s.apb_w(base + 0x14, vo.start)

    def keyon(s, mask):
        for k in range(16):
            if mask >> k & 1:
                s.model.v[k].kon = True
        s.apb_w(0x010, mask)

    def keyoff(s, mask):
        for k in range(16):
            if mask >> k & 1:
                s.model.v[k].active = False
                s.model.v[k].kon = False
        s.apb_w(0x014, mask)


def rand_voice(tb, k):
    r = tb.rnd
    fmt = r.choice([0, 1, 1, 2, 3])
    bps = {0: 1, 1: 2, 2: 4, 3: 2}[fmt]
    ln = r.choice([r.randint(1, 40), r.randint(40, 3000)])
    smooth = r.random() < 0.5
    lo = 0x40000 if smooth else 0
    addr = lo + r.randrange(0, 0x30000 // bps) * bps
    lpen = 1 if r.random() < 0.6 else 0
    lst = r.randint(0, ln - 1) if lpen else r.randint(0, ln)
    step = r.choice([0x1000, 0x800, 0x1000 * 44100 // 48000, r.randint(1, 0x3FFF), r.randint(0x3000, 0xFFFF)])
    tb.voice(k, addr=addr, len=ln, lst=lst, lpen=lpen, step=step, fmt=fmt,
             vl=r.randint(0, 256), vr=r.randint(0, 256), start=r.randint(0, ln - 1))


def main():
    seed = int(sys.argv[1]) if len(sys.argv) > 1 else 1
    nsmp = int(sys.argv[2]) if len(sys.argv) > 2 else 600
    tb = TB(seed)
    r = tb.rnd
    assert tb.apb_r(0x000) == 0x4D495831
    tb.apb_w(0x00C, 24)                      # TARGET
    tb.apb_w(0x004, 1)                       # enable
    tb.flush()
    tb.sync()
    print('[mixer] seed %d, silence: %d samples OK' % (seed, len(tb.got)))
    for k in range(16):
        rand_voice(tb, k)
    tb.keyon(0xFFFF)
    tb.flush()
    done = 0
    rounds = 0
    while done < nsmp:
        n = r.randint(5, 60)
        tb.release(n)
        if not tb.sync():
            break
        done += n
        rounds += 1
        # change things between rounds
        x = r.random()
        if x < 0.25:
            k = r.randrange(16)
            rand_voice(tb, k)
            tb.keyon(1 << k)
        elif x < 0.35:
            tb.keyoff(1 << r.randrange(16))
        elif x < 0.45:
            k = r.randrange(16)
            vo = tb.model.v[k]
            tb.voice(k, vl=r.randint(0, 256), vr=r.randint(0, 256), step=r.randint(1, 0xFFFF))
        elif x < 0.5:
            mv = r.randint(0, 256)
            tb.model.mvol = mv
            tb.apb_w(0x008, mv)
        elif x < 0.55:
            tb.keyon(r.getrandbits(16))
        tb.flush()
        act = tb.apb_r(0x018)
        exp_act = sum((1 << k) for k, vo in enumerate(tb.model.v) if vo.active)
        # pending key-ons are not active yet in the RTL either
        if act & ~sum((1 << k) for k, vo in enumerate(tb.model.v) if vo.active or vo.kon):
            err('ACTIVE %04x vs model %04x' % (act, exp_act))
    smp = tb.apb_r(0x01C)
    miss = tb.apb_r(0x020)
    fet = tb.apb_r(0x024)
    clip = tb.apb_r(0x028)
    print('  %d samples in %d rounds, %d clocks; SAMPLES %d MISSES %d FETCHES %d CLIPS %d, '
          'APB stalls %d' % (len(tb.got), rounds, tb.t, smp, miss, fet, clip, tb.stalls))
    if smp != len(tb.got):
        err('SAMPLES %d != %d' % (smp, len(tb.got)))
    print('FAIL (%d errors)' % len(errors) if errors else 'PASS')
    return 1 if errors else 0


if __name__ == '__main__':
    sys.exit(main())
