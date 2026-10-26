#!/usr/bin/env python3
"""
tb_ftab.py - quick test of the FTAB fill-table list record (CAPS2[5], 2026-10-26):
  1. direct mode: random list traffic (tb_blit rand_list, with FTAB tables) + golden
  2. band mode: frames with fill tables (opaque / alpha / additive / w or h = 0)
  3. throughput: N squares 6..40 px (every 2nd alpha 150) as one FTAB, band frame
     (720p-like geometry not needed: 13 bands x 1280 B)

  python3 -u tb_ftab.py [seed] [mode] [nsquares]
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import tb_blit as T


def main():
    seed = int(sys.argv[1]) if len(sys.argv) > 1 else 1
    mode = sys.argv[2] if len(sys.argv) > 2 else 'random'
    nsq = int(sys.argv[3]) if len(sys.argv) > 3 else 300
    tb = T.TB(seed, mode)
    tb.reset()
    ident = {}
    tb.apb_read(0xBC, lambda v: ident.__setitem__('caps2', v))
    tb.run_until_idle()
    print("CAPS2 %08x" % ident['caps2'])
    assert ident['caps2'] >> 5 & 1, "CAPS2[5] (FTAB) missing"
    tb.apb_write(T.R_LADDR, T.LIST_BASE)
    # 1. direct mode
    if os.environ.get('TB_FT_DIRECT', '1') != '0':
        tb.rand_list(int(os.environ.get('TB_FT_N', '40')))
        tb.lpublish()
        tb.run_until_idle()
        T.check(tb, "FTAB direct list")
        print("direct list ok: %d fill tables, %d items" % (tb.lstat.get('ftabs', 0), tb.lstat.get('fitems', 0)))
    # 2. band frames
    tb.apb_write(T.R_BGEO, T.BAND_NB << 16 | T.BAND_S)
    tb.apb_write(T.R_BRECIP, (1 << 24) // (T.BAND_S // 16) + 1)
    tb.apb_write(T.R_BBIN, T.BIN_BASE | T.BIN_SH)
    tb.run_until_idle()
    rows, W = T.BAND_NB * 16, T.BAND_S // 2
    for fr in range(int(os.environ.get('TB_FT_FRAMES', '2'))):
        tb.apb_write(T.R_CTRL, 1)
        tb.lbands(T.DST_BASE)
        tb.lrec(dict(dstr=T.BAND_S, base=T.DST_BASE), nopush=True)
        for k in range(4):
            tb.lftab(tb.rand_ftab_items(tb.rnd.choice([1, 20, 60]), W, rows), add=int(k == 2))
            if k == 1:
                tb.lcmd_rec(tb.band_cmd('keyc'))
            tb.lpublish()
        tb.lendb()
        tb.lpublish()
        n = tb.run_until_idle()
        tb.band_sync_bins()
        T.check(tb, "FTAB band frame %d" % fr)
        print("band frame %d ok (%d cycles)" % (fr, n))
    # 3. throughput: the squares demo as one table
    rnd = tb.rnd
    items = []
    for i in range(nsq):
        s = 6 + rnd.randint(0, 34)
        items.append((rnd.randint(0, W - s), rnd.randint(0, rows - s), s, s, rnd.getrandbits(16),
                       150 if i & 1 else 255))
    tb.apb_write(T.R_CTRL, 1)
    tb.lbands(T.DST_BASE)
    tb.lrec(dict(dstr=T.BAND_S, base=T.DST_BASE), nopush=True)
    tb.lftab(items)
    tb.lendb()
    tb.lpublish()
    n = tb.run_until_idle()
    tb.band_sync_bins()
    T.check(tb, "FTAB squares")
    bp = {}
    for k, a in (('p1', 0xA4), ('flush', 0xA8), ('kbeat', 0xAC), ('recs', 0xB0), ('band', 0xB4)):
        tb.apb_read(a, lambda v, k=k: bp.__setitem__(k, v))
    tb.run_until_idle()
    print("squares %d as one FTAB: %d cycles, phase1 %d bands %d (flush %d) records %d K beats %d"
          % (nsq, n, bp['p1'], bp['band'], bp['flush'], bp['recs'], bp['kbeat']))
    print("PASS ftab seed %d mode %s (%d cycles)" % (seed, mode, tb.cyc))


if __name__ == '__main__':
    main()
