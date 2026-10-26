#!/usr/bin/env python3
"""
tb_kalpha.py - quick test of the K path blends (2026-10-25): band frames made only of
global-alpha FILL / cached COPY (keyed or not) / additive with alpha, mixed with opaque
commands, then the window read-after-write hazard (add_overlap 'blend' / 'addga').
Compared pixel by pixel with the golden model (same as tb_blit.py section 10).

  python3 -u tb_kalpha.py [seed] [mode] [frames] [items]     (mode random / ideal)
  TB_KBOFF=1: blends back on the narrow path (BAND_GEO[30]) - same pixels expected
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import tb_blit as T


def main():
    seed = int(sys.argv[1]) if len(sys.argv) > 1 else 1
    mode = sys.argv[2] if len(sys.argv) > 2 else 'random'
    nfr = int(sys.argv[3]) if len(sys.argv) > 3 else 2
    nit = int(sys.argv[4]) if len(sys.argv) > 4 else 24
    kboff = int(os.environ.get('TB_KBOFF', '0'))
    tb = T.TB(seed, mode)
    tb.reset()
    for a in range(T.SRC_BASE, T.SRC_BASE + 0x10000, 2):    # ~25 % colour-key pixels
        if tb.rnd.random() < 0.25:
            T.wr16(tb.mem, a, 0xF81F)
            T.wr16(tb.ref, a, 0xF81F)
    ident = {}
    tb.apb_read(T.R_CAPS, lambda v: ident.__setitem__('caps', v))
    tb.apb_read(0xBC, lambda v: ident.__setitem__('caps2', v))
    tb.run_until_idle()
    print("CAPS %08x CAPS2 %08x" % (ident['caps'], ident['caps2']))
    assert ident['caps2'] >> 4 & 1, "CAPS2[4] (K path blends) missing"
    if os.environ.get('BLIT_LIMIT'):                # "rd,wr,words"
        v = [int(x) for x in os.environ['BLIT_LIMIT'].split(',')]
        tb.apb_write(0x54, v[2] << 16 | v[1] << 8 | v[0])
    if os.environ.get('TB_WIDE') == 'off':
        tb.apb_write(0x84, 1 | 256 << 16)
    tb.apb_write(T.R_LADDR, T.LIST_BASE)
    tb.apb_write(T.R_BGEO, kboff << 30 | T.BAND_NB << 16 | T.BAND_S)
    tb.apb_write(T.R_BRECIP, (1 << 24) // (T.BAND_S // 16) + 1)
    tb.apb_write(T.R_BBIN, T.BIN_BASE | T.BIN_SH)
    geo = {}
    tb.apb_read(T.R_BGEO, lambda v: geo.__setitem__('g', v))
    tb.run_until_idle()
    assert geo['g'] == kboff << 30 | T.BAND_NB << 16 | T.BAND_S, hex(geo['g'])
    kinds = ['afill', 'afill', 'ga', 'keyga', 'keyga', 'addga', 'addkga', 'fill', 'keyc', 'copyc']
    for fr in range(nfr):
        tb.apb_write(T.R_CTRL, 1)
        tb.lbands(T.DST_BASE)
        for i in range(nit):
            c = tb.band_cmd(tb.rnd.choice(kinds), big=(i % 11 == 5))
            if c['op'] and c['ga'] == 255 and tb.rnd.random() < 0.5:
                c['ga'] = tb.rnd.choice([0, 1, 64, 128, 191, 254])
            tb.lcmd_rec(c)
            if i % 7 == 3:
                tb.lpublish()
        tb.lendb()
        tb.lpublish()
        n = tb.run_until_idle()
        tb.band_sync_bins()
        T.check(tb, "K blend frame %d" % fr)
        bp = {}
        for k, a in (('kbeat', 0xAC), ('kclk', 0xB8), ('st', T.R_STATUS)):
            tb.apb_read(a, lambda v, k=k: bp.__setitem__(k, v))
        tb.run_until_idle()
        assert bp['st'] & 0xE0 == 0, "band STATUS %x" % bp['st']
        print("K blend frame %d ok: %d cycles, K beats %d (K busy %d)" % (fr, n, bp['kbeat'], bp['kclk']))
    for m in ('blend', 'addga', 'add'):
        T.add_overlap(tb, True, m)
    print("PASS kalpha seed %d mode %s kboff %d (%d cycles)" % (seed, mode, kboff, tb.cyc))


if __name__ == '__main__':
    main()
