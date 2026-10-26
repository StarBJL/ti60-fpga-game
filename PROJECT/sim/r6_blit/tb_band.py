#!/usr/bin/env python3
"""
tb_band.py - band mode (D) tests alone (tb_blit.py section 10), for quick runs.

  python3 tb_band.py [seed] [mode]       TB_BFRAMES / TB_BITEMS / TB_WIDE as in tb_blit.py
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import tb_blit as T


def main():
    seed = int(sys.argv[1]) if len(sys.argv) > 1 else 1
    mode = sys.argv[2] if len(sys.argv) > 2 else 'random'
    tb = T.TB(seed, mode)
    tb.reset()
    for a in range(T.SRC_BASE, T.SRC_BASE + 0x10000, 2):    # ~25 % colour-key pixels
        if tb.rnd.random() < 0.25:
            T.wr16(tb.mem, a, 0xF81F)
            T.wr16(tb.ref, a, 0xF81F)
    ident = {}
    tb.apb_read(T.R_CAPS, lambda v: ident.__setitem__('caps', v))
    tb.run_until_idle()
    print("CAPS %08x" % ident['caps'])
    xs = os.environ.get('TB_WIDE', '')
    if xs == 'off':
        tb.apb_write(0x84, 1 | 256 << 16)
    elif xs:
        v = [int(x) for x in xs.split(',')]
        tb.apb_write(0x84, (v[2] if len(v) > 2 else 256) << 16 | v[0] << 8 | v[1] << 4)
    if os.environ.get('BLIT_LIMIT'):                # "rd,wr,words"
        v = [int(x) for x in os.environ['BLIT_LIMIT'].split(',')]
        tb.apb_write(0x54, v[2] << 16 | v[1] << 8 | v[0])
    tb.apb_write(T.R_LADDR, T.LIST_BASE)
    T.band_tests(tb, ident)
    print("PASS band seed %d mode %s (%d cycles)" % (seed, mode, tb.cyc))


if __name__ == '__main__':
    main()
