#!/usr/bin/env python3
"""
tb_boot.py - replay of the SproutRun STATS boot sequence on the RTL (C2 hang hunt).

  python3 tb_boot.py [seed] [rows] [mode]     (TB_WFIFO / TB_WSTRICT as in tb_blit.py)

WIDE stays at its reset value (wide path for w >= 128). Order as on the board:
bandwidth_test (full-width FILL, opaque COPY, 60 % alpha fill, many 8x8 fills),
then seed_bench: APB copies, list via APB words, DDR list with one REC per sprite,
DDR list with SLOT + DRAW (cached 9x9 keyed sprites). Memory checked after each.
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import tb_blit as T


def main():
    seed = int(sys.argv[1]) if len(sys.argv) > 1 else 1
    rows = int(sys.argv[2]) if len(sys.argv) > 2 else 60
    mode = sys.argv[3] if len(sys.argv) > 3 else 'random'
    nspr = int(os.environ.get('TB_NSPR', '300'))
    tb = T.TB(seed, mode)
    tb.reset()
    rnd = tb.rnd
    for a in range(T.SRC_BASE, T.SRC_BASE + 0x200, 2):      # sprite area: some key pixels
        if rnd.random() < 0.25:
            T.wr16(tb.mem, a, 0xF81F)
            T.wr16(tb.ref, a, 0xF81F)

    def stage(name):
        n = tb.run_until_idle()
        T.check(tb, name)
        print("%-28s ok %7d cycles | wide AW %d W %d AR %d" %
              (name, n, tb.stats['xaw'], tb.stats['xwbeats'], tb.stats['xar']), flush=True)

    base = dict(dstr=1280, sstr=1280, aad=T.A_BASE, astr=640, key=0xF81F,
                key_en=0, apl=0, ga=255, sync=0, cache=0, inv=0)
    # ---- bandwidth_test(hw=1)
    tb.push(dict(base, w=640, h=rows, dst=T.DST_BASE, src=0, color=0x14E3, op=0))
    stage("full fill (wide)")
    tb.push(dict(base, w=640, h=rows, dst=T.DST_BASE, src=T.SRC_BASE + 0x1000, color=0, op=1))
    stage("full copy (wide)")
    tb.push(dict(base, w=640, h=rows, dst=T.DST_BASE, src=0, color=0, op=0, ga=150))
    stage("alpha fill (narrow)")
    for i in range(400):
        tb.push(dict(base, w=8, h=8, dst=T.DST_BASE + ((i * 53) % (rows - 8)) * 1280 +
                     2 * ((i * 37) % 632), src=0, color=(i * 2654435761 >> 16) & 0xFFFF, op=0))
    stage("8x8 fills")

    # ---- seed_bench: 9x9 keyed cached sprites
    spr = T.SRC_BASE + 0x40
    pos = [((i * 53) % (rows - 9)) * 1280 + 2 * ((i * 37) % 631) for i in range(nspr)]
    sp = dict(base, w=9, h=9, src=spr, sstr=18, color=0, op=1, key_en=1, cache=1)
    for d in pos:                                       # pass 0: APB gfx_blit calls
        tb.push(dict(sp, dst=T.DST_BASE + d))
    stage("pass 0 APB calls")
    tb.apb_write(T.R_LADDR, T.LIST_BASE)                # pass 1: words through LIST_WORD
    tb.lvia_apb = True
    tb.lrec(dict(dstr=1280, base=T.DST_BASE, src=spr, sstr=18, size=(9 << 16) | 9, key=0xF81F),
            nopush=True)
    tb.lslot(0, 3, 255, cache=1)
    for d in pos:
        tb.ldraw(0, d)
    tb.lvia_apb = False
    stage("pass 1 APB list")
    tb.lrestart(3)                                      # pass 2: one REC per sprite in DDR
    for k, d in enumerate(pos):
        tb.lrec(dict(dst=T.DST_BASE + d, src=spr, sstr=18, size=(9 << 16) | 9), 3, 255, cache=1)
        if k % 32 == 31:
            tb.lpublish()
    tb.lpublish()
    stage("pass 2 DDR calls")
    tb.lrestart(3)                                      # pass 3: SLOT + DRAW in DDR
    tb.lrec(dict(dstr=1280, base=T.DST_BASE), nopush=True)
    tb.lslot(0, 3, 255, cache=1)
    for k, d in enumerate(pos):
        tb.ldraw(0, d)
        if k % 64 == 63:
            tb.lpublish()
    tb.lpublish()
    stage("pass 3 DDR list")
    print("PASS boot seed %d rows %d mode %s (%d cycles)" % (seed, rows, mode, tb.cyc))


if __name__ == '__main__':
    main()
