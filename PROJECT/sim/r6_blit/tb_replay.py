#!/usr/bin/env python3
"""
tb_replay.py - replay a capture of the real program (game/tools/host_capture.c) on the
RTL (blit_engine + blit_axi_rd_arb + blit_axi_wide, tb_blit.TB's AXI / HDMI models).

  python tb_replay.py <capture base> [mode] [seed]      mode = random | ideal

<base>.apb: every APB write of the driver ("off value" hex), <base>.mem: memory pages.
Replays from the last LIMIT write (after the boot benchmarks) with the earlier WIDE / BIN
/ BAND values applied first, and runs until the first band frame is finished (FRAMES = 1)
or nothing moves for HANG cycles; then prints the engine state.
"""
import os, sys, struct, time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import tb_blit as T

HANG = int(os.environ.get('TB_HANG', '60000'))
CHK = os.environ.get('TB_CHKRD', '0') == '1'   # count AR issuers in the same clock (rd_pend OR != sum)


class SparseMem:
    def __init__(s, path):
        s.p = {}
        d = open(path, 'rb').read()
        for k in range(0, len(d), 4100):
            a = struct.unpack_from('<I', d, k)[0]
            s.p[a >> 12] = bytearray(d[k + 4:k + 4100])

    def page(s, n):
        pg = s.p.get(n)
        if pg is None:
            pg = s.p[n] = bytearray(4096)
        return pg

    def __getitem__(s, i):
        if isinstance(i, slice):
            out = bytearray()
            a = i.start
            while a < i.stop:
                n = min(i.stop, (a | 4095) + 1) - a
                out += s.page(a >> 12)[a & 4095:(a & 4095) + n]
                a += n
            return bytes(out)
        return s.page(i >> 12)[i & 4095]

    def __setitem__(s, i, v):
        s.page(i >> 12)[i & 4095] = v


def perf(s):
    names = ['perf_recs', 'perf_kbeat', 'perf_flush', 'perf_p1', 'perf_band', 'perf_kclk', 'perf_cmds',
             'perf_chit', 'perf_cmiss', 'perf_pushed']
    out = []
    for n in names:
        try:
            out.append('%s %x' % (n, s['u_blit.' + n]))
        except KeyError:
            pass
    print('  engine:', '  '.join(out))
    print('  board (stuck): perf_recs 222  perf_kbeat 1c  perf_flush 1634  perf_p1 7b625 (P1 includes waiting for the CPU)'
          '  CMISS 32 / CHIT 2364a are cumulative since boot on the board')


def main():
    base = sys.argv[1]
    mode = sys.argv[2] if len(sys.argv) > 2 else 'random'
    seed = int(sys.argv[3]) if len(sys.argv) > 3 else 1
    writes = [tuple(int(x, 16) for x in l.split()) for l in open(base + '.apb')]
    mark = max(i for i, (o, v) in enumerate(writes) if o == 0x54)
    pre = {}
    for o, v in writes[:mark]:
        if o in (0x84, 0x9C, 0x94, 0x98):
            pre[o] = v
    T.MEMSZ = 1 << 12
    tb = T.TB(seed, mode)
    tb.mem = SparseMem(base + '.mem')
    T.MEMSZ = 1 << 32
    tb.reset()
    for o, v in pre.items():
        tb.apb_write(o, v)
    for o, v in writes[mark:]:
        tb.apb_write(o, v)
    print('replay %s: %d writes from #%d, pre %s, mode %s' % (base, len(writes) - mark, mark,
          ' '.join('%x=%x' % kv for kv in pre.items()), mode))
    s = tb.sim
    last = None
    still = 0
    coin = [0, 0]
    t0 = time.time()
    while True:
        tb.step()
        s.comb()
        if CHK:
            m = s['u_blit.rdi_f'] | s['u_blit.rdi_l'] | (s['u_blit.rdi_k'] << 3)
            x = s['u_blit.rdi_x'] << 2
            n = (s['u_blit.rdi_f'] != 0) + (s['u_blit.rdi_l'] != 0) + s['u_blit.rdi_k'] + (x != 0)
            if n > 1:
                coin[0] += 1
                if (m | x) != m + x:
                    coin[1] += 1
                    if coin[1] <= 5:
                        print('  cycle %d: rdi f %d l %d k %d x %d -> OR %d sum %d' % (tb.cyc,
                              s['u_blit.rdi_f'], s['u_blit.rdi_l'], s['u_blit.rdi_k'], x, m | x, m + x))
        if tb.cyc % 256 == 0:
            key = (tb.stats['rbeats'], tb.stats['wbeats'], tb.stats['xrbeats'], tb.stats['xwbeats'],
                   s['u_blit.bc_frames'], len(tb.apb_ops))
            still = still + 256 if key == last else 0
            last = key
            if s['u_blit.bc_frames'] >= 1 and not tb.apb_ops:
                print('FRAME DONE at cycle %d (%.0f s): %s' % (tb.cyc, time.time() - t0, tb.stats))
                if CHK:
                    print('  AR issuers in one clock: %d, OR != sum: %d' % tuple(coin))
                perf(s)
                return 0
            if still >= HANG:
                print('HANG: nothing moved for %d cycles at cycle %d (%.0f s), stats %s, apb ops left %d'
                      % (still, tb.cyc, time.time() - t0, tb.stats, len(tb.apb_ops)))
                perf(s)
                return 1
            if tb.cyc % 50000 == 0:
                print('  cycle %d: %s frames %d' % (tb.cyc, key, s['u_blit.bc_frames']), flush=True)


if __name__ == '__main__':
    sys.exit(main())
