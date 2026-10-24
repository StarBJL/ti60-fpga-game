#!/usr/bin/env python3
"""
tb_blit.py - self-checking testbench for rtl/blit (blit_engine + blit_axi_rd_arb
+ blit_axi_wide).

  python3 tb_blit.py [seed] [ncmds] [mode]      mode = random | ideal
  TB_WFIFO=n    depth of the modelled SoC W FIFO (default 16)
  TB_WSTRICT=1  slave takes W only for an AW it already has (fallback path)
  BLIT_LIMIT=rd,wr[,words]   engine LIMIT register (outstanding bursts / read words)
  TB_WIDE=off | minw8,wth[,words]   WIDE register (C2 128-bit path), default random
  TB_NSPR=n     sprites in the 600-sprite sections (smaller for slow LIMIT runs)

* AXI slave model of the 128-bit SoC DDR port: random AR/AW/W ready, random
  read latency, in-order R and B, W beats buffered in a FIFO before their AW
  (SoC clock-crossing FIFO) that drains into the burst at the AW head,
  read data sampled when the AR is accepted.
* The 32-bit side (engine/arbiter -> blit_axi_wide) is checked too: every R
  word (data, id, last) in order, engine AW/W bursts, AR pass-through.
* Fake HDMI master competes for AR/R with 64-beat bursts; its data is checked.
* Protocol checks: burst length, 256-byte boundary, WLAST, strobes, IDs.
* Golden model: every command is applied in order to a reference memory;
  the whole memory is compared when the engine is idle again.
* Command list: random streams of REC / SLOT / DRAW words in "DDR", published
  in pieces through LIST_END, mixed with LIST_WORD (APB) words, direct APB
  commands and LIST_ADDR restarts; the same golden model checks the order.
"""
import os, sys, random
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from vsim import Sim

HERE = os.path.dirname(os.path.abspath(__file__))
RTL = os.path.join(HERE, '..', '..', 'rtl', 'blit')
FILES = [os.path.join(RTL, 'blit_engine.v'), os.path.join(RTL, 'blit_axi_rd_arb.v'),
         os.path.join(RTL, 'blit_axi_wide.v'), os.path.join(HERE, 'tb_blit_top.v')]
WFIFO = int(os.environ.get('TB_WFIFO', '16'))      # SoC W clock-crossing FIFO depth
WSTRICT = os.environ.get('TB_WSTRICT', '0') == '1'  # W only after its AW (fallback path)
# random-mode read latency (clocks); TB_RLAT=min,max (default 6,60)
LAT_MIN, LAT_MAX = [int(x) for x in os.environ.get('TB_RLAT', '6,60').split(',')]

MEMSZ = 1 << 21
DST_BASE, SRC_BASE, A_BASE, HD_BASE = 0x00000, 0x80000, 0x100000, 0x1C0000
LIST_BASE, LIST_TOP = 0x140000, 0x180000
# band mode (D): frame = 13 bands x 16 rows x 1280 bytes at DST_BASE, bins of 16 KB
BIN_BASE, BIN_SH, BAND_NB, BAND_S = 0x180000, 14, 13, 1280
BAND_FB_BYTES = BAND_NB * 16 * BAND_S
SCR_BASE = 0x50000                          # other targets drawn inside a band frame
R_BGEO, R_BRECIP, R_BBIN, R_FRAMES = 0x94, 0x98, 0x9C, 0xA0
TAB_BASE, TAB_TOP = 0x130000, 0x140000      # TAB sprite tables (only ever read)
TAB_P = float(os.environ.get('TB_TAB', '0.3'))   # share of sprite draws done as TAB
NSPR = int(os.environ.get('TB_NSPR', '600'))    # sprites in sections 6/8 (smaller = faster)

# register offsets
R_ID, R_STATUS, R_CTRL, R_CAPS = 0x00, 0x04, 0x08, 0x0C
R_DST, R_DSTR, R_SRC, R_SSTR, R_A, R_ASTR = 0x10, 0x14, 0x18, 0x1C, 0x20, 0x24
R_SIZE, R_COLOR, R_KEY, R_CMD = 0x28, 0x2C, 0x30, 0x34
R_PBUSY, R_PCMDS, R_PRB, R_PWB, R_PUSHED = 0x40, 0x44, 0x48, 0x4C, 0x50
R_LADDR, R_LEND, R_LWORD = 0x38, 0x3C, 0x58
# list context fields in mask-bit order
LFIELDS = ['dst', 'dstr', 'src', 'sstr', 'aad', 'astr', 'size', 'color', 'key', 'base']


def rd16(m, a):
    return m[a] | (m[a + 1] << 8)


def wr16(m, a, v):
    m[a] = v & 0xFF
    m[a + 1] = v >> 8


def golden(m, c):
    w, h = c['w'], c['h']
    if w == 0 or h == 0:
        return
    op, key_en, apl, ga = c['op'], c['key_en'], c['apl'] and c['op'], c['ga']
    add = c.get('add', 0)
    blend = ga != 255 or apl or add
    ga9 = ga + (ga >> 7)
    for r in range(h):
        for p in range(w):
            da = c['dst'] + r * c['dstr'] + 2 * p
            s = rd16(m, c['src'] + r * c['sstr'] + 2 * p) if op else c['color']
            if op and key_en and s == c['key']:
                continue
            if blend:
                if apl:
                    pa = m[c['aad'] + r * c['astr'] + p]
                    e = (pa * ga9) >> 8
                    a9 = e + (e >> 7)
                else:
                    a9 = ga9
                d = rd16(m, da)
                ch = []
                for sh, mk in ((11, 31), (5, 63), (0, 31)):
                    sc, dc = (s >> sh) & mk, (d >> sh) & mk
                    if add:             # additive: dst + src * a, saturated
                        ch.append(min(mk, dc + ((sc * a9 + 128) >> 8)) << sh)
                    else:
                        ch.append(((sc * a9 + dc * (256 - a9) + 128) >> 8) << sh)
                s = ch[0] | ch[1] | ch[2]
            wr16(m, da, s)


class TB:
    def __init__(tb, seed, mode):
        tb.rnd = random.Random(seed)
        tb.mode = mode
        tb.sim = Sim(FILES, 'tb_blit_top', seed=seed)
        if tb.sim.undriven:
            raise SystemExit("undriven signals: %s" % tb.sim.undriven[:20])
        tb.mem = bytearray(tb.rnd.getrandbits(8) for _ in range(MEMSZ))
        tb.ref = bytearray(tb.mem)
        tb.cyc = 0
        # AXI slave state
        tb.rq = []          # 128-bit read bursts: dict(beats[], n, t)
        tb.awq = []         # accepted 128-bit AW: dict(addr, len)
        tb.wq = []          # accepted 128-bit W beats (data, strb, last)
        tb.bq = []          # B responses: ready time
        tb.hold = {}        # stalled downstream payloads (stability check)
        tb.nexp = []        # expected 32-bit R words (id, data, last), in order
        tb.xexp = []        # expected wide (x_r) beats (data, last), in order
        tb.nrd = []         # blitter read bursts in flight: words still to come
        tb.nawq = []        # engine AWs whose W is not complete
        tb.nwr = 0          # engine AWs without B
        tb.r_last_t = 0
        # HDMI master
        tb.hd_pending = None
        tb.hd_expect = []
        tb.hd_next = 0
        tb.hd_bursts = 0
        # APB
        tb.apb_ops = []     # (write?, addr, data, callback)
        tb.apb_state = 0
        tb.stats = dict(rbeats=0, wbeats=0, ar=0, aw=0, dar=0, drbeats=0, daw=0,
                        dwbeats=0, xar=0, xrbeats=0, xaw=0, xwbeats=0)
        tb.last_progress = 0
        # command list: context as the RTL resets it, slots, write pointer
        tb.lctx = dict(dst=0, dstr=1280, src=0, sstr=0, aad=0, astr=0, size=0, color=0,
                       key=0xF81F, base=0)
        tb.slots = {}
        tb.lwr = LIST_BASE          # next free list word in "DDR"
        tb.lpub = LIST_BASE         # LIST_END as last written
        tb.lvia_apb = False         # current record goes through LIST_WORD
        tb.lstat = dict(words=0, draws=0, recs=0, slots=0, apbwords=0, ends=0, restarts=0,
                        tabs=0, titems=0)
        tb.tab_wr = TAB_BASE        # next free table byte
        # band mode (D): frame stride / bands as programmed in BAND_GEO, other targets allowed
        tb.band_s, tb.band_nb, tb.band_scr = BAND_S, BAND_NB, True
        tb.bin_sh = BIN_SH

    # ---------------------------------------------------------------- APB
    def apb_write(tb, addr, data):
        tb.apb_ops.append((1, addr, data, None))

    def apb_read(tb, addr, cb):
        tb.apb_ops.append((0, addr, 0, cb))

    def apb_idle(tb, n):
        tb.apb_ops.append((2, n, 0, None))

    # ---------------------------------------------------------------- list
    def lword(tb, w):
        """one list word: into DDR (published later by LIST_END) or through APB"""
        tb.lstat['words'] += 1
        if tb.lvia_apb:
            tb.lstat['apbwords'] += 1
            tb.apb_write(R_LWORD, w)
            return
        assert tb.lwr + 4 <= LIST_TOP, "list area full"
        for m in (tb.mem, tb.ref):
            m[tb.lwr:tb.lwr + 4] = w.to_bytes(4, 'little')
        tb.lwr += 4

    def lpublish(tb):
        if tb.lpub != tb.lwr:
            tb.lpub = tb.lwr
            tb.apb_write(R_LEND, tb.lwr)
            tb.lstat['ends'] += 1

    def lrestart(tb, gap):
        """new list somewhere else (LIST_ADDR stalls until the old one is done)"""
        tb.lpublish()
        tb.lwr += 4 * gap
        tb.lpub = tb.lwr
        tb.apb_write(R_LADDR, tb.lwr)
        tb.lstat['restarts'] += 1

    def lrec(tb, vals, flags=0, ga=255, nopush=False, extra=0, cache=0, inv=0, gold=True, add=0):
        """REC: set the context fields in vals (+ rewrite those in mask 'extra'), push"""
        ctx, mask = tb.lctx, extra
        for i, f in enumerate(LFIELDS):
            if f in vals and vals[f] != ctx[f]:
                ctx[f] = vals[f]
                mask |= 1 << i
        tb.lword((add << 26) | (inv << 25) | (cache << 24) | (ga << 16) | (flags << 12) |
                 (int(nopush) << 10) | mask)
        for i, f in enumerate(LFIELDS):
            if mask >> i & 1:
                tb.lword(ctx[f])
        tb.lstat['recs'] += 1
        if not nopush and gold:
            golden(tb.ref, tb.lcmd(ctx['dst'], ctx['src'], ctx['sstr'], ctx['size'], flags, ga, add))

    def lcmd(tb, dst, src, sstr, size, flags, ga, add=0):
        ctx = tb.lctx
        return dict(dst=dst, dstr=ctx['dstr'] & 0xFFFF, src=src, sstr=sstr & 0xFFFF,
                    aad=ctx['aad'], astr=ctx['astr'] & 0xFFFF,
                    w=size & 0x7FF, h=(size >> 16) & 0x7FF, color=ctx['color'] & 0xFFFF,
                    key=ctx['key'] & 0xFFFF, op=flags & 1, key_en=flags >> 1 & 1,
                    apl=flags >> 2 & 1, ga=ga, add=add)

    def lslot(tb, n, flags, ga, cache=0, add=0):
        ctx = tb.lctx
        tb.lword((1 << 28) | (add << 26) | (cache << 24) | (ga << 16) | (flags << 12) | n)
        tb.slots[n] = dict(src=ctx['src'], sstr=ctx['sstr'], size=ctx['size'], flags=flags, ga=ga,
                           add=add)
        tb.lstat['slots'] += 1

    def ldraw(tb, n, off):
        sl = tb.slots[n]
        tb.lword((1 << 31) | (n << 25) | off)
        tb.lstat['draws'] += 1
        golden(tb.ref, tb.lcmd((tb.lctx['base'] + off) & 0xFFFFFFFF, sl['src'], sl['sstr'],
                               sl['size'], sl['flags'], sl['ga'], sl['add']))

    def ltab(tb, items, junk=False):
        """TAB: items (slot, x, y) as an 8-byte table in "DDR", the engine draws them"""
        rnd = tb.rnd
        n = len(items)
        if tb.tab_wr + 8 * n > TAB_TOP:
            tb.tab_wr = TAB_BASE                    # only after the engine was idle
        a = tb.tab_wr
        tb.tab_wr += 8 * n + 4 * rnd.choice([0, 0, 1, 7])
        for i, (sl, x, y) in enumerate(items):
            xy = x | (y << 16)
            sp = sl
            if junk:                                # bits the engine must ignore
                xy |= rnd.getrandbits(4) << 12 | rnd.getrandbits(4) << 28
                sp |= rnd.getrandbits(26) << 6
            for m in (tb.mem, tb.ref):
                m[a + 8 * i:a + 8 * i + 4] = xy.to_bytes(4, 'little')
                m[a + 8 * i + 4:a + 8 * i + 8] = sp.to_bytes(4, 'little')
        tb.lword((2 << 28) | n)
        tb.lword(a)
        tb.lstat['tabs'] += 1
        tb.lstat['titems'] += n
        for (sl, x, y) in items:
            s = tb.slots[sl]
            off = (y * (tb.lctx['dstr'] & 0xFFFF) + 2 * x) & 0x1FFFFFF
            golden(tb.ref, tb.lcmd((tb.lctx['base'] + off) & 0xFFFFFFFF, s['src'], s['sstr'],
                                   s['size'], s['flags'], s['ga'], s['add']))

    # ---------------------------------------------------------------- band mode (D)
    def lbands(tb, fb):
        """BANDS: frame at fb; everything up to ENDB is drawn band by band. The
        window starts black, so the golden frame does too."""
        n = tb.band_nb * 16 * tb.band_s
        tb.lword(3 << 28)
        tb.lword(fb)
        tb.band_fb = fb
        tb.ref[fb:fb + n] = bytes(n)
        tb.lstat['bands'] = tb.lstat.get('bands', 0) + 1

    def lendb(tb):
        tb.lword(4 << 28)

    def band_cmd(tb, kind=None, big=False):
        """random command fully inside the band frame (rows on the frame stride)"""
        rnd = tb.rnd
        kinds = ['fill', 'afill', 'copyc', 'keyc', 'copy', 'key', 'wide', 'ga', 'apl', 'key_apl',
                 'addc', 'addkc', 'addf', 'addga']
        kind = kind or rnd.choice(kinds + ['copyc', 'keyc', 'fill', 'addkc'])
        add = kind.startswith('add')
        kind = {'addc': 'copyc', 'addkc': 'keyc', 'addf': 'fill', 'addga': 'ga'}.get(kind, kind)
        rows, S = tb.band_nb * 16, tb.band_s
        W = S // 2
        if big:
            w, h = rnd.randint(200, W), rnd.randint(20, rows)
        elif rnd.random() < 0.7:
            w, h = rnd.randint(1, 40), rnd.randint(1, 20)
        else:
            w, h = rnd.randint(1, 200), rnd.randint(1, 40)
        h, w = min(h, rows), min(w, W)
        x, y = rnd.randint(0, W - w), rnd.randint(0, rows - h)
        c = dict(w=w, h=h, dst=tb.band_fb + y * S + 2 * x, dstr=S,
                 src=SRC_BASE + 2 * rnd.randint(0, 3000), sstr=2 * rnd.randint(w, w + 30),
                 aad=A_BASE + rnd.randint(0, 3000), astr=rnd.randint(w, w + 30),
                 color=rnd.getrandbits(16), key=0xF81F, op=0, key_en=0, apl=0, ga=255,
                 sync=int(rnd.random() < 0.05), cache=0, inv=0, add=int(add))
        if kind == 'afill':
            c['ga'] = rnd.choice([0, 1, 127, 128, 200, 254])
        elif kind != 'fill':
            c['op'] = 1
            c['key_en'] = int(kind in ('key', 'keyc', 'key_apl'))
            c['apl'] = int(kind in ('apl', 'key_apl'))
            c['cache'] = int(kind in ('copyc', 'keyc') or (kind in ('ga', 'apl') and rnd.random() < 0.5))
            if kind in ('ga',) or (kind == 'apl' and rnd.random() < 0.5):
                c['ga'] = rnd.randint(0, 254)
            if kind == 'wide':                      # same 16-byte phase: wide path
                c['src'] = (c['src'] & ~15) | (c['dst'] & 15)
                if c['src'] < SRC_BASE:
                    c['src'] += 16
                c['sstr'] += (c['dstr'] - c['sstr']) % 16
        return c

    def lcmd_rec(tb, c):
        tb.lrec(tb.cmd_vals(c), tb.cmd_flags(c), c['ga'], cache=c.get('cache', 0),
                inv=c.get('inv', 0), add=c.get('add', 0))

    def rand_band_frame(tb, nitems, slots_ok=True, fb=DST_BASE):
        """one band frame: BANDS, random records / slots / DRAW / TAB / other
        targets / INV, ENDB. Published in pieces like the driver does."""
        rnd = tb.rnd
        S, rows = tb.band_s, tb.band_nb * 16
        tb.lbands(fb)
        if rnd.random() < 0.7:                      # background: full frame, wide path
            c = tb.band_cmd('wide')
            c.update(w=S // 2, h=rows, dst=tb.band_fb, src=SRC_BASE + 0x40000 + 16 * rnd.randint(0, 64),
                     sstr=min(S, 1280), key_en=0, apl=0, ga=255, cache=0)   # stays below TAB_BASE
            tb.lcmd_rec(c)
        for i in range(nitems):
            r = rnd.random()
            if r < 0.40:
                tb.lcmd_rec(tb.band_cmd(big=(i % 30 == 29)))
            elif r < 0.50 and slots_ok:             # sprite slot (cached keyed most of the time)
                w, h = rnd.randint(1, 24), rnd.randint(1, 16)
                src = SRC_BASE + 2 * rnd.randint(0, 3000)
                tb.lrec(dict(src=src, sstr=2 * rnd.randint(w, w + 8), size=(h << 16) | w), nopush=True)
                fl = rnd.choice([3, 3, 3, 1, 1, 5, 7, 9, 11])
                ga = 255 if rnd.random() < 0.85 else rnd.randint(0, 254)
                tb.lslot(rnd.randint(0, 63), fl, ga, cache=int(rnd.random() < 0.8),
                         add=int(rnd.random() < 0.3))
            elif r < 0.80 and tb.slots and slots_ok:    # DRAW / TAB inside the frame
                if tb.lctx['dstr'] != S or tb.lctx['base'] != tb.band_fb:
                    tb.lrec(dict(dstr=S, base=tb.band_fb), nopush=True)
                sl = list(tb.slots)
                items = []
                for _ in range(rnd.choice([1, 3, 10, 40, rnd.randint(1, 120)])):
                    n = rnd.choice(sl)
                    s = tb.slots[n]
                    w, h = max(s['size'] & 0x7FF, 1), max(s['size'] >> 16 & 0x7FF, 1)
                    items.append((n, rnd.randint(0, S // 2 - w), rnd.randint(0, rows - h)))
                if rnd.random() < 0.5:
                    tb.ltab(items)
                else:
                    for (n, x, y) in items:
                        tb.ldraw(n, y * S + 2 * x)
            elif r < 0.90 and tb.band_scr:          # another target: drawn in phase 1
                c = tb.rand_cmd()
                c['dst'] = SCR_BASE + (c['dst'] - DST_BASE) % 0x10000
                if c['op'] and c['src'] < SRC_BASE:
                    c['src'] = SRC_BASE
                tb.lcmd_rec(c)
            elif r < 0.93:
                tb.lrec(dict(size=0), inv=1)        # INV inside the frame
            else:
                tb.lrec({}, nopush=True)
            if rnd.random() < 0.3:
                tb.lpublish()
        tb.lendb()
        tb.lpublish()

    def band_sync_bins(tb):
        """bins are scratch: take the engine's copy into the golden memory"""
        top = min(BIN_BASE + (max(16, tb.band_nb) << tb.bin_sh), HD_BASE)
        tb.ref[BIN_BASE:top] = tb.mem[BIN_BASE:top]

    def cmd_vals(tb, c):
        return dict(dst=c['dst'], dstr=c['dstr'], src=c['src'], sstr=c['sstr'], aad=c['aad'],
                    astr=c['astr'], size=(c['h'] << 16) | c['w'], color=c['color'], key=c['key'])

    def cmd_flags(tb, c):
        return c['op'] | (c['key_en'] << 1) | (c['apl'] << 2) | (c.get('sync', 0) << 3)

    def rand_list(tb, nitems):
        """random list traffic; the golden model runs in issue order"""
        rnd = tb.rnd
        if tb.lctx['aad'] < A_BASE:     # slots with an A8 plane use the context A address:
            tb.lrec(dict(aad=A_BASE + 0x100, astr=48), nopush=True)   # never 0 (odd x reads A - 1)
        for i in range(nitems):
            r = rnd.random()
            # a record is either stored in DDR or fed through APB; APB words
            # (and direct commands) come after everything published so far
            via_apb = rnd.random() < 0.15
            if via_apb or r >= 0.90:
                tb.lpublish()
            tb.lvia_apb = via_apb
            if r < 0.40:                                   # generic command record
                c = tb.rand_cmd(big=(i % 40 == 39))
                vals = tb.cmd_vals(c)
                if rnd.random() < 0.5:                     # junk in the unused upper bits
                    vals['dstr'] |= rnd.getrandbits(16) << 16
                    vals['size'] |= rnd.getrandbits(5) << 27
                extra = rnd.getrandbits(10) if rnd.random() < 0.2 else 0
                tb.lrec(vals, tb.cmd_flags(c), c['ga'], extra=extra, cache=c['cache'], inv=c['inv'],
                        add=c.get('add', 0))
            elif r < 0.50:                                 # define a sprite slot
                c = tb.rand_cmd()
                if rnd.random() < 0.7:
                    c['w'], c['h'] = rnd.randint(1, 24), rnd.randint(1, 12)
                    c['sstr'] = 2 * rnd.randint(c['w'], c['w'] + 8)
                fl = rnd.choice([1, 1, 3, 3, 3, 5, 7, 0, 9, 11])
                ga = 255 if rnd.random() < 0.8 else rnd.randint(0, 254)
                tb.lrec(dict(src=c['src'], sstr=c['sstr'], size=(c['h'] << 16) | c['w']),
                        nopush=True)
                tb.lslot(rnd.choice([0, 1, 2, 3, 31, 62, 63, rnd.randint(0, 63)]), fl, ga,
                         cache=int(rnd.random() < 0.5), add=int(rnd.random() < 0.2))
            elif r < 0.90 and tb.slots and rnd.random() < TAB_P:   # sprite table (TAB)
                wmax = max(max(s['size'] & 0x7FF, 1) for s in tb.slots.values())
                dstr = tb.lctx['dstr'] & 0xFFFF            # rows of one sprite must not overlap
                if dstr < 2 * wmax or dstr > 1400 or rnd.random() < 0.1:
                    dstr = 2 * rnd.randint(wmax, 700)
                base = tb.lctx['base']
                if base >= 0x2000 or rnd.random() < 0.1:
                    base = DST_BASE + 2 * rnd.randint(0, 2000)
                if dstr != tb.lctx['dstr'] or base != tb.lctx['base']:
                    tb.lrec(dict(dstr=dstr, base=base), nopush=True)
                n = rnd.choice([0, 1, 2, 3, 31, 32, 33, rnd.randint(1, 80)])
                sl = list(tb.slots)
                tb.ltab([(rnd.choice(sl), rnd.randint(0, 40), rnd.randint(0, 40))
                         for _ in range(n)], junk=rnd.random() < 0.3)
            elif r < 0.90 and tb.slots:                    # sprite draws
                for _ in range(rnd.randint(1, 12)):
                    n = rnd.choice(list(tb.slots))
                    w = max(tb.slots[n]['size'] & 0x7FF, 1)
                    dstr = tb.lctx['dstr'] & 0xFFFF
                    if dstr < 2 * w or dstr > 1400 or rnd.random() < 0.05:
                        dstr = 2 * rnd.randint(w, 700)
                    base = tb.lctx['base']
                    if base >= 0x2000 or rnd.random() < 0.05:
                        base = DST_BASE + 2 * rnd.randint(0, 2000)
                    setup = dict(dstr=dstr, base=base)
                    if rnd.random() < 0.05:
                        setup['color'] = rnd.getrandbits(16)
                    if setup['dstr'] != dstr or any(setup[k] != tb.lctx[k] for k in setup):
                        tb.lrec(setup, nopush=True)
                    tb.ldraw(n, rnd.randint(0, 40) * dstr + 2 * rnd.randint(0, 40))
            elif r < 0.96:                                 # direct APB command in between
                tb.lpublish()                              # (also reached with no slots yet)
                tb.push(tb.rand_cmd())
            elif r < 0.98:
                tb.lrestart(rnd.choice([0, 1, 5, 63, 64, 200]))
            else:
                tb.lrec({}, nopush=True)                   # empty record
            tb.lvia_apb = False
            if rnd.random() < 0.3:
                tb.lpublish()
            if rnd.random() < 0.1:
                tb.apb_idle(rnd.randint(1, 400))
        tb.lpublish()

    # ---------------------------------------------------------------- cycle
    def p(tb, prob):
        return tb.mode == 'ideal' or tb.rnd.random() < prob

    def step(tb):
        s, rnd = tb.sim, tb.rnd
        s['clk'] = 0
        # ---- drive inputs
        # APB
        if tb.apb_ops and tb.apb_ops[0][0] != 2:
            op = tb.apb_ops[0]
            s['apb_psel'] = 1
            s['apb_penable'] = 1 if tb.apb_state == 1 else 0
            s['apb_pwrite'] = op[0]
            s['apb_paddr'] = op[1]
            s['apb_pwdata'] = op[2]
        else:
            s['apb_psel'] = 0
            s['apb_penable'] = 0
            s['apb_pwrite'] = 0
            s['apb_paddr'] = rnd.getrandbits(12)
            s['apb_pwdata'] = rnd.getrandbits(32)
        # HDMI master: one 64-beat burst every ~200 cycles, like 640x480 @ 60 Hz x 2
        if tb.hd_pending is None and tb.cyc >= tb.hd_next and len(tb.hd_expect) < 256:
            a = HD_BASE + ((tb.hd_bursts * 256) & 0x3FFFF)
            tb.hd_pending = a
        s['hd_arvalid'] = 1 if tb.hd_pending is not None else 0
        s['hd_araddr'] = tb.hd_pending if tb.hd_pending is not None else 0
        s['hd_arlen'] = 63
        # AXI slave (128-bit io_ddrMasters_0 behind the SoC clock-crossing FIFOs)
        ar_ready = tb.p(0.6)
        aw_ready = tb.p(0.5)
        if WSTRICT:     # slave that takes W only for an AW it already has
            w_room = bool(tb.awq)
        else:           # SoC W FIFO: W beats are buffered before their AW
            w_room = len(tb.wq) < WFIFO
        w_ready = w_room and tb.p(0.7)
        s['s_arready'] = int(ar_ready)
        s['s_awready'] = int(aw_ready)
        s['s_wready'] = int(w_ready)
        rv = bool(tb.rq) and tb.rq[0]['t'] <= tb.cyc and tb.p(0.8)
        if rv:
            b = tb.rq[0]
            s['s_rvalid'] = 1
            s['s_rdata'] = b['beats'][b['n']]
            s['s_rlast'] = int(b['n'] == len(b['beats']) - 1)
            s['s_rresp'] = 0
        else:
            s['s_rvalid'] = 0
            s['s_rdata'] = rnd.getrandbits(128)
            s['s_rlast'] = rnd.getrandbits(1)
            s['s_rresp'] = 0
        bv = bool(tb.bq) and tb.bq[0] <= tb.cyc and tb.p(0.7)
        s['s_bvalid'] = int(bv)
        s['s_bresp'] = 0
        s['rst'] = 0

        s.comb()

        # ---- sample / handshakes
        if tb.apb_ops and tb.apb_ops[0][0] == 2:
            op = tb.apb_ops[0]
            if op[1] <= 1:
                tb.apb_ops.pop(0)
            else:
                tb.apb_ops[0] = (2, op[1] - 1, 0, None)
        elif tb.apb_ops:
            if tb.apb_state == 0:
                tb.apb_state = 1
            elif s['apb_pready']:
                op = tb.apb_ops.pop(0)
                if op[3]:
                    op[3](s['apb_prdata'])
                tb.apb_state = 0
        # downstream valid/payload must hold until ready
        for ch, flds in (('ar', ('araddr', 'arlen', 'arsize', 'arburst', 'arid')),
                         ('aw', ('awaddr', 'awlen', 'awsize', 'awburst', 'awid')),
                         ('w', ('wdata', 'wstrb', 'wlast'))):
            cur = tuple(s['s_' + f] for f in flds)
            prev = tb.hold.get(ch)
            if prev is not None:
                assert s['s_%svalid' % ch] and cur == prev, "%s dropped/changed while stalled" % ch
            rdy = {'ar': ar_ready, 'aw': aw_ready, 'w': w_ready}[ch]
            tb.hold[ch] = cur if (s['s_%svalid' % ch] and not rdy) else None
        # engine wide side channel (x_*): same AXI rule
        for ch, flds in (('ar', ('araddr', 'arlen')), ('aw', ('awaddr', 'awlen')),
                         ('w', ('wdata', 'wstrb', 'wlast'))):
            cur = tuple(s['x_' + f] for f in flds)
            prev = tb.hold.get('x' + ch)
            if prev is not None:
                assert s['x_%svalid' % ch] and cur == prev, "x_%s dropped/changed while stalled" % ch
            tb.hold['x' + ch] = cur if (s['x_%svalid' % ch] and not s['x_%sready' % ch]) else None

        # ---- 32-bit side (engine / arbiter -> blit_axi_wide)
        n_ar = bool(s['n_arvalid'] and s['n_arready'])
        x_ar = bool(s['x_arvalid'] and s['x_arready'])
        d_ar = bool(s['s_arvalid'] and ar_ready)
        assert not (n_ar and x_ar), "narrow and wide AR in one cycle"
        assert (n_ar or x_ar) == d_ar, "AR not passed through"
        if x_ar:
            a, n = s['x_araddr'], s['x_arlen'] + 1
            assert a % 16 == 0 and (a & 0xFF) + 16 * n <= 256, "x AR %x len %d" % (a, n)
            assert a + 16 * n <= MEMSZ, "x AR out of range %x" % a
            assert s['s_araddr'] == a and s['s_arlen'] == n - 1 and s['s_arsize'] == 4 \
                and s['s_arburst'] == 1 and s['s_arid'] == 0, "x AR not passed through"
            beats = [int.from_bytes(tb.mem[a + 16 * k:a + 16 * k + 16], 'little')
                     for k in range(n)]
            lat = 1 if tb.mode == 'ideal' else rnd.randint(LAT_MIN, LAT_MAX)
            t = max(tb.r_last_t, tb.cyc + lat)
            tb.r_last_t = t
            tb.rq.append(dict(beats=beats, n=0, t=t))
            tb.xexp += [(b, int(k == n - 1)) for k, b in enumerate(beats)]
            tb.nrd.append(4 * n)
            tb.stats['dar'] += 1
            tb.stats['xar'] += 1
        if n_ar:
            a, n, i = s['n_araddr'], s['n_arlen'] + 1, s['n_arid']
            assert s['n_arsize'] == 2 and s['n_arburst'] == 1, "AR size/burst"
            assert a % 4 == 0, "AR unaligned %x" % a
            assert i in (0, 1), "ARID %d" % i
            assert (a & 0xFF) + 4 * n <= 256, "AR crosses 256B: %x len %d" % (a, n)
            assert 0 <= a and a + 4 * n <= MEMSZ, "AR out of range %x" % a
            words = [int.from_bytes(tb.mem[a + 4 * k:a + 4 * k + 4], 'little') for k in range(n)]
            tb.nexp += [(i, w, int(k == n - 1)) for k, w in enumerate(words)]
            if i == 0:
                assert s['hd_arready'], "hd_arready mismatch"
                tb.hd_expect += [(w, int(k == n - 1)) for k, w in enumerate(words)]
                tb.hd_pending = None
                tb.hd_bursts += 1
                # real 640x480@60 scan-out = 64 beats every ~700 cycles (9 % of the port);
                # the random mode loads it 3-7x harder
                tb.hd_next = tb.cyc + (700 if tb.mode == 'ideal' else rnd.randint(100, 300))
            else:
                tb.stats['ar'] += 1
                tb.nrd.append(n)
            # 128-bit AR
            da, dn = s['s_araddr'], s['s_arlen'] + 1
            assert s['s_arsize'] == 4 and s['s_arburst'] == 1, "128 AR size/burst"
            assert da == a & ~15 and dn == ((a >> 2 & 3) + n + 3) // 4, \
                "128 AR %x/%d for %x/%d" % (da, dn, a, n)
            assert (da & 0xFF) + 16 * dn <= 256
            beats = [int.from_bytes(tb.mem[da + 16 * k:da + 16 * k + 16], 'little')
                     for k in range(dn)]
            lat = 1 if tb.mode == 'ideal' else rnd.randint(LAT_MIN, LAT_MAX)
            t = max(tb.r_last_t, tb.cyc + lat)
            tb.r_last_t = t
            tb.rq.append(dict(beats=beats, n=0, t=t))
            tb.stats['dar'] += 1
        elif s['hd_arvalid']:
            assert not s['hd_arready'], "hd_arready without s handshake"
        if s['n_rvalid'] and s['n_rready']:
            assert tb.nexp, "R word without AR"
            i, w, last = tb.nexp.pop(0)
            assert (s['n_rid'] & 1) == i and s['n_rdata'] == w and s['n_rlast'] == last, \
                "R word id %d/%d data %08x/%08x last %d/%d" % (
                    s['n_rid'], i, s['n_rdata'], w, s['n_rlast'], last)
            if i == 1:
                tb.stats['rbeats'] += 1
                tb.nrd[0] -= 1
                if last:
                    assert tb.nrd.pop(0) == 0
        if s['x_rvalid']:
            assert rv and s['s_rready'], "x_rvalid without an accepted R beat"
            assert tb.xexp, "wide R beat without AR"
            d, last = tb.xexp.pop(0)
            assert s['x_rdata'] == d and s['x_rlast'] == last, "wide R beat data/last"
            tb.stats['xrbeats'] += 1
            tb.nrd[0] -= 4
            if last:
                assert tb.nrd.pop(0) == 0, "wide R burst length"
        if s['hd_rvalid']:
            w, last = tb.hd_expect.pop(0)
            assert s['hd_rdata'] == w and s['hd_rlast'] == last, "HDMI data"
        if rv and s['s_rready']:
            b = tb.rq[0]
            b['n'] += 1
            tb.stats['drbeats'] += 1
            if b['n'] == len(b['beats']):
                tb.rq.pop(0)
        if s['n_awvalid'] and s['n_awready']:
            a, n = s['n_awaddr'], s['n_awlen'] + 1
            assert s['n_awsize'] == 2 and s['n_awburst'] == 1 and s['n_awid'] == 1
            assert a % 4 == 0 and (a & 0xFF) + 4 * n <= 256, "AW %x len %d" % (a, n)
            assert a + 4 * n <= MEMSZ
            tb.nawq.append(dict(addr=a, len=n, n=0, wide=0))
            tb.nwr += 1
            tb.stats['aw'] += 1
        if s['x_awvalid'] and s['x_awready']:
            assert not (s['n_awvalid'] and s['n_awready']), "narrow and wide AW in one cycle"
            a, n = s['x_awaddr'], s['x_awlen'] + 1
            assert a % 16 == 0 and (a & 0xFF) + 16 * n <= 256, "x AW %x len %d" % (a, n)
            assert a + 16 * n <= MEMSZ
            tb.nawq.append(dict(addr=a, len=n, n=0, wide=1))
            tb.nwr += 1
            tb.stats['xaw'] += 1
        if s['x_wvalid'] and s['x_wready']:
            assert tb.nawq and tb.nawq[0]['wide'], "wide W without its AW (engine)"
            aw = tb.nawq[0]
            last = aw['n'] == aw['len'] - 1
            assert s['x_wlast'] == int(last), "x WLAST at beat %d/%d" % (aw['n'], aw['len'])
            st = s['x_wstrb']
            assert st != 0, "wide W beat with no strobes"
            for j in range(8):
                assert (st >> 2 * j & 3) in (0, 3), "x WSTRB splits a pixel %04x" % st
            aw['n'] += 1
            tb.stats['xwbeats'] += 1
            tb.last_progress = tb.cyc
            if last:
                tb.nawq.pop(0)
        if s['n_wvalid'] and s['n_wready']:
            assert tb.nawq and not tb.nawq[0]['wide'], "W before AW (engine)"
            aw = tb.nawq[0]
            last = aw['n'] == aw['len'] - 1
            assert s['n_wlast'] == int(last), "WLAST at beat %d/%d" % (aw['n'], aw['len'])
            aw['n'] += 1
            tb.stats['wbeats'] += 1
            tb.last_progress = tb.cyc
            if last:
                tb.nawq.pop(0)
        # ---- 128-bit write side: AW queue + W FIFO, a burst is written when complete
        if s['s_awvalid'] and aw_ready:
            a, n = s['s_awaddr'], s['s_awlen'] + 1
            assert s['s_awsize'] == 4 and s['s_awburst'] == 1
            assert a % 16 == 0 and (a & 0xFF) + 16 * n <= 256, "128 AW %x len %d" % (a, n)
            assert a + 16 * n <= MEMSZ
            tb.awq.append(dict(addr=a, len=n, n=0))
            tb.stats['daw'] += 1
        if s['s_wvalid'] and w_ready:
            tb.wq.append((s['s_wdata'], s['s_wstrb'], s['s_wlast']))
            tb.stats['dwbeats'] += 1
            tb.last_progress = tb.cyc
        while tb.awq and tb.wq:         # W FIFO drains into the burst at the AW head
            aw = tb.awq[0]
            d, st, wl = tb.wq.pop(0)
            k = aw['n']
            assert wl == int(k == aw['len'] - 1), "128 WLAST at beat %d/%d" % (k, aw['len'])
            for j in range(16):
                if st >> j & 1:
                    tb.mem[aw['addr'] + 16 * k + j] = (d >> (8 * j)) & 0xFF
            aw['n'] += 1
            if aw['n'] == aw['len']:
                tb.awq.pop(0)
                tb.bq.append(tb.cyc + (1 if tb.mode == 'ideal' else rnd.randint(2, 30)))
        if bv:
            assert s['s_bready'] and s['n_bvalid'] and s['n_bready']
            tb.bq.pop(0)
            tb.nwr -= 1
        tb.max_rd = max(getattr(tb, 'max_rd', 0), len(tb.nrd))
        tb.max_wr = max(getattr(tb, 'max_wr', 0), tb.nwr)
        tb.max_rw = max(getattr(tb, 'max_rw', 0), sum(tb.nrd))
        s.clock()
        tb.cyc += 1

    def reset(tb):
        s = tb.sim
        for _ in range(4):
            s['rst'] = 1
            for n in ('apb_psel', 'apb_penable', 'hd_arvalid', 's_arready', 's_rvalid',
                      's_awready', 's_wready', 's_bvalid', 's_rlast', 's_rdata', 's_rresp',
                      's_bresp'):
                s[n] = 0
            s.comb()
            s.clock()

    def run_until_idle(tb, limit=2000000):
        start = tb.cyc
        tb.last_progress = tb.cyc
        while True:
            tb.step()
            tb.sim.comb()                    # busy after the clock edge
            if (not tb.apb_ops and not tb.sim['busy'] and not tb.awq and not tb.bq
                    and not tb.wq and tb.nwr == 0):
                break
            if tb.cyc - tb.last_progress > 30000 and not tb.apb_ops:
                raise SystemExit("DEADLOCK at cycle %d" % tb.cyc)
            if tb.apb_ops:
                tb.last_progress = max(tb.last_progress, tb.cyc - 1000)
            if tb.cyc - start > limit:
                raise SystemExit("timeout")
        return tb.cyc - start

    # ---------------------------------------------------------------- commands
    def push(tb, c):
        """program + push one command; the golden model runs in queue order"""
        tb.apb_write(R_DST, c['dst'])
        tb.apb_write(R_DSTR, c['dstr'])
        tb.apb_write(R_SRC, c['src'])
        tb.apb_write(R_SSTR, c['sstr'])
        tb.apb_write(R_A, c['aad'])
        tb.apb_write(R_ASTR, c['astr'])
        tb.apb_write(R_SIZE, (c['h'] << 16) | c['w'])
        tb.apb_write(R_COLOR, c['color'])
        tb.apb_write(R_KEY, c['key'])
        tb.apb_write(R_CMD, c['op'] | (c['key_en'] << 1) | (c['apl'] << 2) |
                     (c.get('sync', 0) << 3) | (c.get('cache', 0) << 4) |
                     (c.get('inv', 0) << 5) | (c.get('add', 0) << 6) | (c['ga'] << 8))
        golden(tb.ref, c)

    def rand_cmd(tb, big=False):
        rnd = tb.rnd
        r = rnd.random()
        if big:
            w = rnd.randint(200, 700)
        elif r < 0.6:
            w = rnd.randint(1, 40)
        elif r < 0.9:
            w = rnd.randint(40, 160)
        else:
            w = rnd.randint(160, 330)
        h = rnd.randint(1, 10) if w < 100 else rnd.randint(1, 4)
        if rnd.random() < 0.02:
            w, h = rnd.choice([(0, 3), (5, 0)])
        dstr = 2 * rnd.randint(max(w, 1), 700)
        dx, dy = rnd.randint(0, 40), rnd.randint(0, 40)
        sstr = 2 * rnd.randint(max(w, 1), max(w, 1) + 30)
        astr = rnd.randint(max(w, 1), max(w, 1) + 30)
        kind = rnd.choice(['fill', 'fill', 'afill', 'copy', 'key', 'ga', 'apl', 'apl_ga', 'key_apl'])
        c = dict(w=w, h=h, dst=DST_BASE + dy * dstr + 2 * dx, dstr=dstr,
                 src=SRC_BASE + 2 * rnd.randint(0, 3000), sstr=sstr,
                 aad=A_BASE + rnd.randint(0, 3000), astr=astr,
                 color=rnd.getrandbits(16), key=0xF81F, op=0, key_en=0, apl=0, ga=255,
                 sync=int(rnd.random() < 0.1))
        # source cache: sources are never written by the engine (dst < SRC_BASE), so
        # any mix of cached / uncached copies and stray INVs must give the same pixels
        c['cache'] = int(rnd.random() < 0.4)
        c['inv'] = int(rnd.random() < 0.03)
        c['add'] = int(rnd.random() < 0.15)           # additive blend (CAPS2[0])
        if rnd.random() < 0.1:
            c['key'] = rnd.getrandbits(16)
        if kind == 'copy' and rnd.random() < 0.5:   # same 16-byte phase: wide path (C2)
            c['src'] = (c['src'] & ~15) | (c['dst'] & 15)
            if c['src'] < SRC_BASE:
                c['src'] += 16
            c['sstr'] += (c['dstr'] - c['sstr']) % 16
        if kind == 'afill':
            c['ga'] = rnd.choice([0, 1, 127, 128, 200, 254])
        elif kind != 'fill':
            c['op'] = 1
            c['key_en'] = int(kind in ('key', 'key_apl'))
            c['apl'] = int(kind in ('apl', 'apl_ga', 'key_apl'))
            if kind in ('ga', 'apl_ga'):
                c['ga'] = rnd.randint(0, 254)
            if rnd.random() < 0.05:            # junk flag bits must be ignored for fills
                c['op'] = 0
        return c


def check(tb, what):
    bad = [i for i in range(0, MEMSZ) if tb.mem[i] != tb.ref[i]]
    if bad:
        print("MISMATCH %s: %d bytes, first at %x: dut %02x ref %02x" %
              (what, len(bad), bad[0], tb.mem[bad[0]], tb.ref[bad[0]]))
        raise SystemExit(1)


def add_overlap(tb, band):
    """additive sprites drawn on top of each other back to back (K path window
    read-after-write hazard in band mode): 3 slots, 90 items around one spot,
    TAB and DRAW, plus an additive fill and an alpha copy in between"""
    rnd = tb.rnd
    fb = DST_BASE
    if band:
        tb.lbands(fb)
    S = BAND_S
    tb.lrec(dict(dstr=S, base=fb, key=0xF81F), nopush=True)
    for n, (w, h) in enumerate(((16, 16), (9, 9), (24, 7))):
        tb.lrec(dict(src=SRC_BASE + 0xB000 + 0x400 * n + 2 * n, sstr=2 * w + 8, size=(h << 16) | w),
                nopush=True)
        tb.lslot(20 + n, 3, 255, cache=1, add=1)
    items = [(20 + rnd.randint(0, 2), 100 + rnd.randint(0, 5), 30 + rnd.randint(0, 4)) for _ in range(60)]
    tb.ltab(items)
    for (n, x, y) in items[:30]:
        tb.ldraw(n, (y + 1) * S + 2 * (x + 3))
    c = dict(w=40, h=12, dst=fb + 32 * S + 2 * 98, dstr=S, src=0, sstr=0, aad=A_BASE, astr=40,
             color=0x18E3, key=0xF81F, op=0, key_en=0, apl=0, ga=255, sync=0, cache=0, inv=0, add=1)
    tb.lcmd_rec(c)
    tb.ltab(items[:20])
    if band:
        tb.lendb()
    tb.lpublish()
    tb.run_until_idle()
    if band:
        tb.band_sync_bins()
    check(tb, "additive overlap (%s)" % ('band' if band else 'direct'))
    print("additive overlap ok (%s)" % ('band' if band else 'direct'))


def band_perf(tb, nseed=None):
    """game-like frame (13 bands = 208 rows): background copy, NSPR seeds (TAB, cached
    keyed 9x9), 40 cached keyed 24x24 sprites, 8 alpha panels - direct, then as a
    band frame. Returns the cycles of each part (ideal slave: engine-only time)."""
    rnd = tb.rnd
    nseed = nseed or NSPR
    rows = BAND_NB * 16
    spr9, spr24 = SRC_BASE + 0x9000 + 6, SRC_BASE + 0xA000 + 10
    seeds = [(1, rnd.randint(0, 631), rnd.randint(0, rows - 9)) for _ in range(nseed)]
    big = [(2, rnd.randint(0, 616), rnd.randint(0, rows - 24)) for _ in range(40)]
    npan = int(os.environ.get('TB_PAN', '8'))
    pan = [(rnd.randint(0, 440), rnd.randint(0, rows - 20)) for _ in range(npan)]
    res = {}

    def frame(band):
        if band:
            tb.lbands(DST_BASE)
        bg = dict(w=640, h=rows, dst=DST_BASE, dstr=BAND_S, src=SRC_BASE + 0x40000, sstr=BAND_S,
                  aad=A_BASE, astr=640, color=0, key=0xF81F, op=1, key_en=0, apl=0, ga=255,
                  sync=0, cache=0, inv=0)
        tb.lcmd_rec(bg)
        tb.lrec(dict(src=spr9, sstr=18, size=(9 << 16) | 9, dstr=BAND_S, base=DST_BASE,
                     key=0xF81F), nopush=True)
        tb.lslot(1, 3, 255, cache=1, add=int(os.environ.get('TB_ADDSEED', '0')))   # additive seeds
        tb.lrec(dict(src=spr24, sstr=48, size=(24 << 16) | 24), nopush=True)
        tb.lslot(2, 3, 255, cache=1)
        for (n, x, y) in big:
            tb.ldraw(n, y * BAND_S + 2 * x)
        tb.ltab(seeds)
        for (x, y) in pan:
            c = dict(bg, w=200, h=20, dst=DST_BASE + y * BAND_S + 2 * x, op=0, ga=150,
                     color=0x39E7)
            tb.lcmd_rec(c)
        if band:
            tb.lendb()
        tb.lpublish()

    for band in (0, 1):
        tb.apb_write(R_CTRL, 1)
        tb.run_until_idle()
        frame(band)
        n = tb.run_until_idle()
        if band:
            tb.band_sync_bins()
        check(tb, "perf frame %s" % ('band' if band else 'direct'))
        p = {}
        for k, a in (('busy', R_PBUSY), ('p1', 0xA4), ('flush', 0xA8), ('kbeat', 0xAC),
                     ('recs', 0xB0), ('band', 0xB4), ('kclk', 0xB8), ('chit', 0x5C),
                     ('cmiss', 0x60), ('rbeat', R_PRB), ('xr', 0x90), ('xw', 0x8C)):
            tb.apb_read(a, lambda v, k=k: p.__setitem__(k, v))
        tb.run_until_idle()
        res[band] = (n, p)
        print("perf frame %-6s: %6d cycles | phase 1 %d bands %d (flush %d) | K beats %d K busy %d "
              "| bin records %d | cache hit/miss %d/%d | rd beats 32b %d 128b %d, wr 128b %d"
              % ('band' if band else 'direct', n, p['p1'], p['band'], p['flush'], p['kbeat'],
                 p['kclk'], p['recs'], p['chit'], p['cmiss'], p['rbeat'], p['xr'], p['xw']))
    print("perf: %d seeds + 40 sprites + %d panels + background on %d rows: direct %d, band %d cycles (x%.2f)"
          % (nseed, npan, rows, res[0][0], res[1][0], res[0][0] / res[1][0]))
    return res


def band_tests(tb, ident):
    """10. band mode (D, CAPS[31]): random frames drawn band by band vs the golden model
    (black start), other targets drawn in phase 1, TAB / DRAW / INV, bin overflow and
    a record the band frame cannot take."""
    assert ident['caps'] >> 31 & 1, "CAPS: no band mode"
    recip = (1 << 24) // (BAND_S // 16) + 1
    koff = int(os.environ.get('TB_KOFF', '0'))        # 1: K path off (narrow path instead)
    tb.apb_write(R_BGEO, koff << 31 | BAND_NB << 16 | BAND_S)
    tb.apb_write(R_BRECIP, recip)
    tb.apb_write(R_BBIN, BIN_BASE | BIN_SH)
    geo = {}
    tb.apb_read(R_BGEO, lambda v: geo.__setitem__('g', v))
    tb.apb_read(R_BBIN, lambda v: geo.__setitem__('b', v))
    tb.apb_read(R_FRAMES, lambda v: geo.__setitem__('f0', v))
    tb.run_until_idle()
    assert geo['g'] == koff << 31 | BAND_NB << 16 | BAND_S and geo['b'] == BIN_BASE | BIN_SH, geo
    nfr = int(os.environ.get('TB_BFRAMES', '4'))
    for fr in range(nfr):
        tb.apb_write(R_CTRL, 1)
        tb.rand_band_frame(int(os.environ.get('TB_BITEMS', '40')), slots_ok=True)
        n = tb.run_until_idle()
        tb.band_sync_bins()
        check(tb, "band frame %d" % fr)
        bp = {}
        for k, a in (('p1', 0xA4), ('flush', 0xA8), ('kbeat', 0xAC), ('recs', 0xB0),
                     ('band', 0xB4), ('kclk', 0xB8), ('busy', R_PBUSY), ('st', R_STATUS),
                     ('fr', R_FRAMES)):
            tb.apb_read(a, lambda v, k=k: bp.__setitem__(k, v))
        tb.run_until_idle()
        assert bp['st'] & 0xE0 == 0, "band STATUS %x" % bp['st']
        assert bp['fr'] == geo['f0'] + fr + 1, "FRAMES %d" % bp['fr']
        print("band frame %d ok: %d cycles = phase 1 %d + bands %d (flush/clear %d), "
              "%d bin records, K beats %d (K busy %d)"
              % (fr, n, bp['p1'], bp['band'], bp['flush'], bp['recs'], bp['kbeat'], bp['kclk']))
    add_overlap(tb, True)
    nfr += 1
    add_overlap(tb, False)
    # frames queued back to back (the driver's pipeline: the next frame is recorded
    # while one is drawn), list traffic and direct commands in between
    # (TB_BEXTRA=0 skips this and the second geometry: slow configurations)
    extra = os.environ.get('TB_BEXTRA', '1') != '0'
    for k in range(3 if extra else 0):
        tb.rand_band_frame(12)
        if k == 1:
            tb.rand_list(6)
    n = tb.run_until_idle()
    tb.band_sync_bins()
    check(tb, "back-to-back band frames")
    tb.apb_read(R_FRAMES, lambda v: geo.__setitem__('f1', v))
    tb.run_until_idle()
    assert geo['f1'] == geo['f0'] + nfr + (3 if extra else 0), "FRAMES %d" % geo['f1']
    print("back-to-back band frames ok (%d cycles)" % n)
    # another geometry: 320 px wide frame (stride 640), 6 bands, at another address
    tb.band_s, tb.band_nb, tb.band_scr = 640, 6, False
    tb.apb_write(R_BGEO, koff << 31 | 6 << 16 | 640)
    tb.apb_write(R_BRECIP, (1 << 24) // (640 // 16) + 1)
    for k in range(2 if extra else 0):
        tb.rand_band_frame(25, fb=0x44000)
        tb.run_until_idle()
        tb.band_sync_bins()
        check(tb, "band frame 320 px wide %d" % k)
    tb.band_s, tb.band_nb, tb.band_scr = BAND_S, BAND_NB, True
    tb.apb_write(R_BGEO, koff << 31 | BAND_NB << 16 | BAND_S)
    tb.apb_write(R_BRECIP, recip)
    tb.run_until_idle()
    print("band geometry 640 B x 96 rows at 0x44000 ok")
    nfr += 5 if extra else 0
    # 720p window (CAPS2[2]): 1280 px rows (stride 2560), 6 bands; many bands (CAPS2[15:8],
    # 45 = 720 rows) on a narrow frame (stride 256) with 4 KB bins
    c2 = {}
    tb.apb_read(0xBC, lambda v: c2.__setitem__('c', v))
    tb.run_until_idle()
    maxb = c2['c'] >> 8 & 0xFF or 32
    geos = []
    if c2['c'] >> 2 & 1:
        geos.append((2560, 6, BIN_SH, 3 if extra else 1))
    if maxb > 32:
        geos.append((256, maxb, 12, 2 if extra else 1))
    for (gs, gnb, gsh, gn) in geos:
        tb.band_s, tb.band_nb, tb.band_scr, tb.bin_sh = gs, gnb, False, gsh
        tb.apb_write(R_BGEO, koff << 31 | gnb << 16 | gs)
        tb.apb_write(R_BRECIP, (1 << 24) // (gs // 16) + 1)
        tb.apb_write(R_BBIN, BIN_BASE | gsh)
        for k in range(gn):
            tb.rand_band_frame(30 if gs > 1280 else 25, fb=DST_BASE)
            tb.run_until_idle()
            tb.band_sync_bins()
            check(tb, "band frame stride %d x %d bands %d" % (gs, gnb, k))
        print("band geometry %d B x %d rows ok (%d frames)" % (gs, gnb * 16, gn))
        nfr += gn
    if geos:
        tb.band_s, tb.band_nb, tb.band_scr, tb.bin_sh = BAND_S, BAND_NB, True, BIN_SH
        tb.apb_write(R_BGEO, koff << 31 | BAND_NB << 16 | BAND_S)
        tb.apb_write(R_BRECIP, recip)
        tb.apb_write(R_BBIN, BIN_BASE | BIN_SH)
        tb.run_until_idle()
    # a record in the frame whose rows are not on the frame stride: dropped + STATUS[7]
    tb.lbands(DST_BASE)
    bad = tb.band_cmd('fill')
    bad.update(h=3, dstr=BAND_S + 16)
    tb.lrec(tb.cmd_vals(bad), 0, 255, gold=False)   # dropped by the engine
    tb.lcmd_rec(tb.band_cmd('keyc'))
    tb.lendb()
    tb.lpublish()
    tb.run_until_idle()
    tb.band_sync_bins()
    check(tb, "band frame with a bad record")
    tb.apb_read(R_STATUS, lambda v: geo.__setitem__('st', v))
    tb.apb_write(R_CTRL, 2)
    tb.apb_read(R_STATUS, lambda v: geo.__setitem__('st2', v))
    tb.run_until_idle()
    assert geo['st'] & 0x80 and not geo['st2'] & 0x80, "STATUS berr %x %x" % (geo['st'], geo['st2'])
    # bin overflow: 4 KB bins (128 records) and 200 records in one band
    tb.apb_write(R_BBIN, BIN_BASE | 12)
    tb.lbands(DST_BASE)
    for i in range(200):
        c = tb.band_cmd('fill')
        c.update(w=4, h=2, dst=DST_BASE + 3 * BAND_S + 8 * (i % 150))
        tb.lrec(tb.cmd_vals(c), 0, 255)
    tb.lendb()
    tb.lpublish()
    tb.run_until_idle()
    tb.apb_read(R_STATUS, lambda v: geo.__setitem__('st', v))
    tb.apb_write(R_CTRL, 2)
    tb.apb_write(R_BBIN, BIN_BASE | BIN_SH)
    tb.run_until_idle()
    assert geo['st'] & 0x40, "no bin overflow flag %x" % geo['st']
    tb.ref[DST_BASE:DST_BASE + BAND_FB_BYTES] = tb.mem[DST_BASE:DST_BASE + BAND_FB_BYTES]
    tb.band_sync_bins()
    check(tb, "after bin overflow")
    # direct commands after band frames still work (pointers back on the main list)
    tb.rand_list(20)
    tb.run_until_idle()
    check(tb, "list after band frames")
    print("band mode ok (%d frames, bad record flag, bin overflow flag)" % nfr)


def main():
    seed = int(sys.argv[1]) if len(sys.argv) > 1 else 1
    ncmd = int(sys.argv[2]) if len(sys.argv) > 2 else 60
    mode = sys.argv[3] if len(sys.argv) > 3 else 'random'
    tb = TB(seed, mode)
    tb.reset()
    s = tb.sim
    # source images: random pixels with ~25 % colour-key magenta
    for a in range(SRC_BASE, SRC_BASE + 0x10000, 2):
        if tb.rnd.random() < 0.25:
            wr16(tb.mem, a, 0xF81F)
            wr16(tb.ref, a, 0xF81F)

    ident = {}
    tb.apb_read(R_ID, lambda v: ident.__setitem__('id', v))
    tb.apb_read(R_KEY, lambda v: ident.__setitem__('key', v))
    tb.apb_read(R_CAPS, lambda v: ident.__setitem__('caps', v))
    tb.run_until_idle()
    assert ident['id'] == 0x424C5431, hex(ident['id'])
    assert ident['key'] == 0xF81F
    print("ID %08x KEY %04x CAPS %06x" % (ident['id'], ident['key'], ident['caps']))
    assert ident['caps'] >> 25 & 1, "CAPS: no LIMIT register"
    assert ident['caps'] >> 26 & 1, "CAPS: no LIMIT read-word field"
    import os
    lim = {}
    tb.apb_read(0x54, lambda v: lim.__setitem__('v', v))
    tb.run_until_idle()
    assert lim['v'] == (1024 << 16 | 128 << 8 | 16), "LIMIT reset %x" % lim['v']
    if os.environ.get('BLIT_LIMIT'):            # "rd,wr[,words]": max outstanding read / write
        v = [int(x) for x in os.environ['BLIT_LIMIT'].split(',')]   # bursts, read words in flight
        rd, wr = v[0], v[1]
        rw = v[2] if len(v) > 2 else 1024
        tb.apb_write(0x54, rw << 16 | wr << 8 | rd)
        tb.apb_read(0x54, lambda v: lim.__setitem__('v', v))
        tb.run_until_idle()
        assert lim['v'] == (rw << 16 | wr << 8 | rd), hex(lim['v'])
        tb.limit = (rd, wr, rw)
        tb.max_rd = tb.max_wr = tb.max_rw = 0
        print("LIMIT rd %d wr %d words %d" % (rd, wr, rw))

    # WIDE (0x84, CAPS[30]): 128-bit path for aligned opaque COPY/FILL (C2)
    assert ident['caps'] >> 30 & 1, "CAPS: no wide path"
    tb.apb_read(0x84, lambda v: lim.__setitem__('x', v))
    tb.run_until_idle()
    assert lim['x'] == (256 << 16 | 16 << 8), "WIDE reset %x" % lim['x']
    # TB_WIDE="off" | "minw8,wth[,words]"; random mode default: any width, random batch
    xs = os.environ.get('TB_WIDE', '')
    xv = [16, 0] if mode == 'ideal' else [tb.rnd.choice([0, 0, 1, 2]), tb.rnd.choice([0, 0, 1, 4])]
    if xs and xs != 'off':
        xv = [int(x) for x in xs.split(',')]
    xr = xv[2] if len(xv) > 2 else (tb.limit[2] if hasattr(tb, 'limit') else 256)
    xw = xr << 16 | xv[0] << 8 | xv[1] << 4 | int(xs == 'off')
    tb.apb_write(0x84, xw)
    tb.apb_read(0x84, lambda v: lim.__setitem__('x', v))
    tb.run_until_idle()
    assert lim['x'] == xw, "WIDE %x != %x" % (lim['x'], xw)
    print("WIDE %08x (%s)" % (xw, 'off' if xw & 1 else
          'min width %d, batch %d beats, words %d' % (8 * (xw >> 8 & 255), 64 * (xw >> 4 & 15), xw >> 16)))

    # 1. random command mix
    for i in range(ncmd):
        tb.push(tb.rand_cmd(big=(i % 25 == 24)))
    cyc = tb.run_until_idle()
    check(tb, "random mix")
    print("random mix: %d cmds, %d cycles, rbeats %d wbeats %d AR %d AW %d, hdmi bursts %d"
          % (ncmd, cyc, tb.stats['rbeats'], tb.stats['wbeats'], tb.stats['ar'],
             tb.stats['aw'], tb.hd_bursts))
    print("  128-bit port: AR %d R beats %d AW %d W beats %d (W FIFO %d%s)"
          % (tb.stats['dar'], tb.stats['drbeats'], tb.stats['daw'], tb.stats['dwbeats'],
             WFIFO, ', strict W-after-AW' if WSTRICT else ''))
    print("  wide path: AR %(xar)d R beats %(xrbeats)d AW %(xaw)d W beats %(xwbeats)d" % tb.stats)

    # 2. many tiny commands: command queue fills, APB must stall and nothing is lost
    #    (only CMD is rewritten: the shadow registers keep their values)
    c = tb.rand_cmd()
    c.update(w=5, h=2, op=0, ga=255, apl=0, key_en=0, sync=0, add=0, dst=DST_BASE + 2 * 3)
    tb.push(c)
    for i in range(599):
        c['ga'] = 255 if tb.rnd.random() < 0.9 else 100
        if tb.rnd.random() < 0.1:
            c['color'] = tb.rnd.getrandbits(16)
            tb.apb_write(R_COLOR, c['color'])
        tb.apb_write(R_CMD, c['ga'] << 8)
        golden(tb.ref, c)
    stalls = [0]
    while tb.apb_ops:
        tb.step()
        if tb.sim['apb_psel'] and tb.sim['apb_penable'] and not tb.sim['apb_pready']:
            stalls[0] += 1
    tb.run_until_idle()
    print("queue stress: APB stalled %d cycles on a full queue" % stalls[0])
    check(tb, "queue stress")
    pushed = {}
    tb.apb_read(R_PUSHED, lambda v: pushed.__setitem__('n', v))
    tb.apb_read(R_STATUS, lambda v: pushed.__setitem__('st', v))
    tb.run_until_idle()
    print("queue stress ok, pushed %d, STATUS after %08x" % (pushed['n'], pushed['st']))
    assert pushed['n'] == ncmd + 600
    assert ident['caps'] >> 24 & 1, "CAPS: no command list"
    assert pushed['st'] & 1 == 0 and (pushed['st'] >> 16) == 1 << (ident['caps'] & 0xFF)

    # 3. long rows: widest command (ring edge) and odd alignments
    for (w, sx, dx) in ((2045, 1, 0), (2045, 0, 1), (1023, 1, 1), (2044, 0, 0)):
        c = dict(w=w, h=2, dst=DST_BASE + 2 * dx, dstr=4200, src=SRC_BASE + 2 * sx, sstr=4200,
                 aad=A_BASE + 3, astr=2049, color=0x1234, key=0xF81F, op=1, key_en=1,
                 apl=1, ga=255, sync=0, cache=sx)
        tb.push(c)
    tb.run_until_idle()
    check(tb, "wide rows")
    print("wide rows ok")

    # 4. command list: DDR words + LIST_END, LIST_WORD, direct commands, restarts
    tb.apb_write(R_LADDR, LIST_BASE)
    stalls = [0]
    tb.rand_list(max(ncmd, 40) * 3)
    while tb.apb_ops:
        tb.step()
        if tb.sim['apb_psel'] and tb.sim['apb_penable'] and not tb.sim['apb_pready']:
            stalls[0] += 1
    tb.run_until_idle()
    check(tb, "command list")
    st = {}
    tb.apb_read(R_STATUS, lambda v: st.__setitem__('st', v))
    tb.apb_read(R_LADDR, lambda v: st.__setitem__('ptr', v))
    tb.apb_read(R_LEND, lambda v: st.__setitem__('end', v))
    tb.run_until_idle()
    assert st['st'] & 0x1F == 0, "STATUS %08x after the list" % st['st']
    assert st['ptr'] == st['end'] == tb.lwr, "list pointers %x %x %x" % (st['ptr'], st['end'], tb.lwr)
    print("command list ok: %(words)d words (%(apbwords)d via APB), %(recs)d records, "
          "%(slots)d slot defs, %(draws)d draws, %(ends)d LIST_END writes, "
          "%(restarts)d restarts, %(tabs)d tables / %(titems)d items" % tb.lstat + ", APB stalled %d cycles" % stalls[0])

    # 5. a reserved list word is skipped and flagged, CTRL[1] clears the flag
    tb.apb_write(R_LWORD, 0x70000000)
    tb.apb_read(R_STATUS, lambda v: st.__setitem__('bad', v))
    tb.apb_write(R_CTRL, 2)
    tb.apb_read(R_STATUS, lambda v: st.__setitem__('clr', v))
    c = tb.rand_cmd()
    tb.lvia_apb = True
    tb.lrec(tb.cmd_vals(c), tb.cmd_flags(c), c['ga'])      # decoder still in step
    tb.lvia_apb = False
    tb.run_until_idle()
    assert st['bad'] & 0x10 and not st['clr'] & 0x10, "list error flag %x %x" % (st['bad'], st['clr'])
    check(tb, "bad list word")
    print("bad list word ok")

    # 6. sprite throughput: 600 keyed 9x9 sprites, APB commands vs DRAW words
    c = dict(w=9, h=9, dstr=1280, src=SRC_BASE + 64, sstr=18, aad=A_BASE, astr=9,
             color=0, key=0xF81F, op=1, key_en=1, apl=0, ga=255, sync=0, dst=0)
    pos = [tb.rnd.randint(0, 300) * 1280 + 2 * tb.rnd.randint(0, 600) for _ in range(NSPR)]
    tb.apb_write(R_DSTR, c['dstr'])
    tb.apb_write(R_SRC, c['src'])
    tb.apb_write(R_SSTR, c['sstr'])
    tb.apb_write(R_SIZE, (9 << 16) | 9)
    tb.apb_write(R_KEY, c['key'])
    tb.run_until_idle()
    for d in pos:
        c['dst'] = DST_BASE + d
        tb.apb_write(R_DST, c['dst'])
        tb.apb_write(R_CMD, 3 | (255 << 8))
        golden(tb.ref, c)
    n_apb = tb.run_until_idle()
    tb.lrestart(3)
    tb.lrec(dict(src=c['src'], sstr=c['sstr'], size=(9 << 16) | 9, dstr=1280, base=DST_BASE,
                 key=0xF81F), nopush=True)
    tb.lslot(7, 3, 255)
    for d in pos:
        tb.ldraw(7, d)
    tb.lpublish()
    n_list = tb.run_until_idle()
    check(tb, "sprite list")
    print("600 keyed 9x9 sprites: APB %d cycles (%d/sprite, 2 APB writes each), "
          "list %d cycles (%d/sprite, 1 DDR word each)" % (n_apb, n_apb // NSPR, n_list, n_list // NSPR))

    # 6b. the same 600 sprites through one TAB record (CAPS[28])
    assert ident['caps'] >> 28 & 1, "CAPS: no TAB record"
    tb.lrestart(3)
    tb.lslot(7, 3, 255)
    tb.ltab([(7, (d % 1280) // 2, d // 1280) for d in pos])
    tb.lpublish()
    n_tab = tb.run_until_idle()
    check(tb, "sprite table")
    print("600 keyed 9x9 sprites: TAB %d cycles (%d/sprite, 0 list words each)"
          % (n_tab, n_tab // NSPR))

    # 8. source cache (CAPS[27]): repeated sprite copies hit, stale lines stay until INV
    assert ident['caps'] >> 27 & 1, "CAPS: no source cache"

    def stalls(name, ident):
        """stall counters (CAPS[29]); each busy clock is exactly one of: a word
        issued (= W beat) or one of the four PIXEL classes (cmds: info only)"""
        assert ident['caps'] >> 29 & 1, "CAPS: no stall counters"
        p = {}
        for k, a in (('busy', R_PBUSY), ('cmds', R_PCMDS), ('wbeat', R_PWB),
                     ('wstall', 0x64), ('awwait', 0x68), ('awstall', 0x6C), ('arstall', 0x70),
                     ('pxwait', 0x74), ('pxfull', 0x78), ('pxbub', 0x7C), ('pxidle', 0x80),
                     ('wclk', 0x88), ('xwbeat', 0x8C), ('xrbeat', 0x90)):
            tb.apb_read(a, lambda v, k=k: p.__setitem__(k, v))
        tb.run_until_idle()
        px = p['pxwait'] + p['pxfull'] + p['pxbub'] + p['pxidle']
        print("  %s: busy %d = wbeat %d + wait %d + full %d + bub %d + idle %d + wide %d (cmds %d)"
              " | wstall %d awwait %d awstall %d arstall %d | wide W %d R %d beats"
              % (name, p['busy'], p['wbeat'], p['pxwait'], p['pxfull'],
                 p['pxbub'], p['pxidle'], p['wclk'], p['cmds'], p['wstall'], p['awwait'],
                 p['awstall'], p['arstall'], p['xwbeat'], p['xrbeat']))
        assert p['busy'] == p['wbeat'] + px + p['wclk'], "stall classes do not add up: %s" % p
        return p

    def perf():
        p = {}
        tb.apb_read(0x5C, lambda v: p.__setitem__('hit', v))
        tb.apb_read(0x60, lambda v: p.__setitem__('miss', v))
        tb.run_until_idle()
        return p

    spr = SRC_BASE + 0x9000 + 6                 # 3 pixels into a 32-byte line
    c = dict(w=9, h=9, dstr=1280, src=spr, sstr=18, aad=A_BASE, astr=9, color=0,
             key=0xF81F, op=1, key_en=1, apl=0, ga=255, sync=0, cache=1, inv=0, dst=0)
    tb.apb_write(R_CTRL, 1)                     # clear the perf counters
    tb.apb_write(R_SRC, spr)
    tb.apb_write(R_SSTR, 18)
    tb.run_until_idle()
    for d in pos:
        c['dst'] = DST_BASE + d
        tb.apb_write(R_DST, c['dst'])
        tb.apb_write(R_CMD, 3 | (1 << 4) | (255 << 8))
        golden(tb.ref, c)
    n_capb = tb.run_until_idle()
    check(tb, "cached sprites (APB)")
    p1 = perf()
    assert p1['miss'] <= 8 and p1['hit'] >= (NSPR - 1) * 9, "cache counters %s" % p1
    tb.apb_write(R_CTRL, 1)
    tb.lrestart(3)
    tb.lrec(dict(src=spr, sstr=18), nopush=True)
    tb.lslot(8, 3, 255, cache=1)
    for d in pos:
        tb.ldraw(8, d)
    tb.lpublish()
    n_clist = tb.run_until_idle()
    check(tb, "cached sprites (list)")
    p2 = perf()
    assert p2['miss'] == 0 and p2['hit'] >= NSPR * 9, "cache counters %s" % p2
    stalls("600 cached sprites (list)", ident)
    print("600 cached 9x9 sprites: APB %d cycles (%d/sprite), list %d cycles (%d/sprite); "
          "hit/miss %d/%d then %d/%d"
          % (n_capb, n_capb // NSPR, n_clist, n_clist // NSPR, p1['hit'], p1['miss'],
             p2['hit'], p2['miss']))

    def new_sprite():                           # the CPU rewrites the sprite behind the cache
        for a in range(spr, spr + 9 * 18, 2):
            v = tb.rnd.getrandbits(16)
            wr16(tb.mem, a, v)
            wr16(tb.ref, a, v)

    # APB: a cached copy without INV still draws the old sprite, CMD[5] drops it
    stale = dict(c, dst=DST_BASE + 50 * 1280 + 100)
    golden(tb.ref, stale)                       # expected with the OLD source
    new_sprite()
    tb.apb_write(R_DST, stale['dst'])
    tb.apb_write(R_CMD, 3 | (1 << 4) | (255 << 8))
    tb.push(dict(c, dst=DST_BASE + 70 * 1280 + 100, inv=1))
    tb.run_until_idle()
    check(tb, "stale line / APB INV")
    # list: DRAW from the cache (old data), REC with INV and size 0 (a no-op), DRAW again
    tb.ldraw(8, 90 * 1280 + 100)                # golden: old sprite
    new_sprite()
    tb.lrec(dict(size=0), inv=1)
    tb.ldraw(8, 110 * 1280 + 100)               # golden: new sprite
    tb.lpublish()
    tb.run_until_idle()
    check(tb, "stale line / list INV")
    print("cache INV ok (APB CMD[5], list REC[25] on a size-0 record)")

    # 9. wide path (C2): every 16-byte phase, odd row edges, FILL/COPY, batch sizes
    #    and read-word limits, narrow commands in between
    rnd = tb.rnd
    xrw = xw >> 16 if not xw & 1 else 256
    for wth, words in ((0, xrw), (1, 16), (4, 1024), (15, 64), (0, 1)):
        if hasattr(tb, 'limit'):
            words = min(words, tb.limit[2])
        tb.apb_write(0x84, words << 16 | wth << 4)          # min width 0: any width
        tb.apb_write(R_CTRL, 1)
        nw = 0
        for k in range(14):
            ph = 2 * rnd.randint(0, 7)
            w = rnd.choice([1, 2, 7, 8, 9, 15, 16, 17, 63, 64, 65, 127, 128, 129, 300, 640,
                            1000, rnd.randint(1, 1100)])
            h = rnd.randint(1, 5)
            dstr = 2 * rnd.randint(w, w + 40)
            sstr = dstr + 16 * rnd.randint(0, 4)            # same 16-byte phase per row
            c = dict(w=w, h=h, dst=DST_BASE + 0x1000 * rnd.randint(0, 40) + ph, dstr=dstr,
                     src=SRC_BASE + 16 * rnd.randint(0, 2000) + ph, sstr=sstr,
                     aad=A_BASE, astr=w, color=rnd.getrandbits(16), key=0xF81F,
                     op=int(rnd.random() < 0.6), key_en=0, apl=0, ga=255,
                     sync=int(rnd.random() < 0.1), cache=int(rnd.random() < 0.3))
            tb.push(c)
            nw += 1
            if rnd.random() < 0.3:
                tb.push(tb.rand_cmd())
        tb.run_until_idle()
        check(tb, "wide path wth %d words %d" % (wth, words))
        p = stalls("wide wth %d words %d" % (wth, words), ident)
        assert p['xwbeat'] > 0 and p['wclk'] > 0, "wide path not used"
    print("wide path ok (phases, edges, batches, read-word limits)")
    tb.apb_write(0x84, xw)

    if os.environ.get('TB_BAND', '1') != '0':
        band_tests(tb, ident)

    # 7. throughput (ideal slave): 320x40 fill / copy / alpha with the wide path off,
    #    then fill / copy with it on, and a 640x60 background copy (both)
    if mode == 'ideal':
        runs = [(k, 1) for k in ('fill', 'copy', 'alpha')] + [('fill', 0), ('copy', 0)] + \
               [('bg', 1), ('bg', 0)]
        for kind, off in runs:
            w, h = (640, 60) if kind == 'bg' else (320, 40)
            c = dict(w=w, h=h, dst=DST_BASE, dstr=1280, src=SRC_BASE, sstr=640 if w == 320 else 1280,
                     aad=A_BASE, astr=320, color=0x07E0, key=0xF81F,
                     op=int(kind != 'fill'), key_en=0, apl=0,
                     ga=128 if kind == 'alpha' else 255, sync=0)
            if kind == 'bg':
                c['src'] = SRC_BASE + 0x40000
            tb.apb_write(0x84, (xw & ~1) | off)
            tb.apb_write(R_CTRL, 1)
            tb.run_until_idle()
            tb.push(c)
            n = tb.run_until_idle()
            name = "%s %dx%d wide %s" % (kind, w, h, 'off' if off else 'on')
            stalls(name, ident)
            print("%-22s: %d cycles = %.2f px/clk (%.0f Mpx/s @100MHz, %.2f ms for 640x480)"
                  % (name, n, w * h / n, w * h / n * 100, n * 640 * 480 / (w * h) / 1e5))
        tb.apb_write(0x84, xw)
        check(tb, "throughput")
    print("max outstanding bursts: read %d write %d, read words in flight %d"
          % (tb.max_rd, tb.max_wr, tb.max_rw))
    if hasattr(tb, 'limit'):
        assert tb.max_rd <= tb.limit[0] and tb.max_wr <= tb.limit[1], "LIMIT not respected"
        # a burst (<= 64 words) may start while fewer than 'words' are in flight
        assert tb.max_rw <= tb.limit[2] - 1 + 64, "LIMIT words not respected"
    print("PASS seed %d mode %s (%d cycles total)" % (seed, mode, tb.cyc))


if __name__ == '__main__':
    main()
