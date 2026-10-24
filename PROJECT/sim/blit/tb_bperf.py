#!/usr/bin/env python3
"""tb_bperf.py - band mode (D) throughput: game-like frame, direct vs band (ideal slave).
   python3 tb_bperf.py [seed] [nseeds] [mode]     TB_KOFF=1: K path off"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import tb_blit as T


def main():
    seed = int(sys.argv[1]) if len(sys.argv) > 1 else 5
    nseed = int(sys.argv[2]) if len(sys.argv) > 2 else 600
    mode = sys.argv[3] if len(sys.argv) > 3 else 'ideal'
    tb = T.TB(seed, mode)
    tb.reset()
    for a in range(T.SRC_BASE, T.SRC_BASE + 0x10000, 2):
        if tb.rnd.random() < 0.25:
            T.wr16(tb.mem, a, 0xF81F)
            T.wr16(tb.ref, a, 0xF81F)
    koff = int(os.environ.get('TB_KOFF', '0'))
    tb.apb_write(T.R_BGEO, koff << 31 | T.BAND_NB << 16 | T.BAND_S)
    tb.apb_write(T.R_BRECIP, (1 << 24) // (T.BAND_S // 16) + 1)
    tb.apb_write(T.R_BBIN, T.BIN_BASE | T.BIN_SH)
    tb.apb_write(T.R_LADDR, T.LIST_BASE)
    tb.run_until_idle()
    T.band_perf(tb, nseed)
    print("PASS bperf seed %d (%d cycles)" % (seed, tb.cyc))


if __name__ == '__main__':
    main()
