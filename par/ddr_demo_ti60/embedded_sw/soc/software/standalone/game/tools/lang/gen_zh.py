#!/usr/bin/env python3
"""
gen_zh.py (2026-10-28) - Chinese UI pack for the board HUD: tools/lang/zh.txt -> src/ui_zh_data.h

  * dictionary: English key -> Chinese text, sorted by the key bytes (gfx.c gfx_tr() binary-searches it,
    behind a pointer cache, so a TR("...") costs one hash probe per call in Chinese mode, nothing in English);
  * glyphs: every CJK character of the Chinese texts, anti-aliased (16 levels), drawn at the native pixel
    size of each text scale (not one small bitmap scaled up): scale 1 -> 12 px, 2 -> 16, 3 -> 22, 4 -> 30,
    5 -> 38, 6 -> 46, 7 -> 52, 8 -> 60, 9 -> 66 (the 5x7 ASCII font is 7*scale px high).
    Scales 1..3 have all characters; bigger scales only the characters of entries marked with a larger
    scale in zh.txt (titles). gfx.c falls back to the biggest smaller size that has the character.
    12 px: Noto Sans CJK SC Regular, >= 16 px: Bold (matches the 1 px / 2 px+ strokes of the pixel font).
    Stored as 4-bit coverage (2 pixels per byte); gfx.c expands a glyph to A8 in DDR the first time it is
    drawn and draws it as one A8 alpha copy (BitBlt narrow path) in the text colour.
  * checks: printf specifiers of the Chinese text = a prefix of the English ones; no ASCII characters
    the 5x7 font does not have; every TR("...") literal in the HUD sources has an entry (warning only).

Font: Noto Sans CJK SC (SIL Open Font License 1.1), /usr/share/fonts/opentype/noto/.
  python3 tools/lang/gen_zh.py [--preview out.png]
"""
import os, re, sys
from PIL import Image, ImageDraw, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
GAME = os.path.normpath(os.path.join(HERE, '..', '..'))
SRC = os.path.join(GAME, 'src')
PX = [12, 16, 22, 30, 38, 46, 52, 60, 66]
FONT_DIR = '/usr/share/fonts/opentype/noto'
ASCII_OK = set(chr(c) for c in range(32,127))

def font(px):
    name = 'NotoSansCJK-Medium.ttc'
    return ImageFont.truetype(os.path.join(FONT_DIR, name), px, index=2)   # index 2 = SC

def specs(s):
    return re.findall(r'%[-+ #0]*\d*(?:\.\d+)?[hl]*[diouxXcsp%]', s)

def load():
    entries = []
    for ln, line in enumerate(open(os.path.join(HERE, 'zh.txt'), encoding='utf-8'), 1):
        line = line.rstrip('\n')
        if not line or line.startswith('#'): continue
        parts = line.split('\t')
        if len(parts) < 2: sys.exit('zh.txt:%d: need English<TAB>Chinese' % ln)
        en, zh = parts[0], parts[1]
        sc = int(parts[2]) if len(parts) > 2 and parts[2] else 3
        se, sz = specs(en), specs(zh)
        if se[:len(sz)] != sz: sys.exit('zh.txt:%d: specifiers %s vs %s' % (ln, se, sz))
        bad = [c for c in zh if ord(c) < 128 and c not in ASCII_OK and c != '%']
        if bad: sys.exit('zh.txt:%d: ASCII %r not in the pixel font' % (ln, ''.join(bad)))
        entries.append((en, zh, sc))
    keys = [e[0] for e in entries]
    dup = {k for k in keys if keys.count(k) > 1}
    if dup: sys.exit('duplicate keys: %s' % dup)
    return entries

def glyph(ch, px):
    f = font(px)
    im = Image.new('L', (px, px), 0)
    ImageDraw.Draw(im).text((0, round(px * 0.88)), ch, font=f, fill=255, anchor='ls')
    if ch=='_' and not im.getbbox():ImageDraw.Draw(im).line((0,px-2,max(1,5*(PX.index(px)+1))-1,px-2),fill=255)
    if ord(ch)<128 and ch!=' ':
        # Keep the established HUD cell advances while replacing the old 5x7
        # bitmap with antialiased, case-sensitive Noto glyphs.
        scale=PX.index(px)+1;cw=5*scale;b=im.getbbox()
        if b:
            tile=im.crop(b)
            if tile.width>cw:tile=tile.resize((cw,tile.height),Image.Resampling.LANCZOS)
            im=Image.new('L',(px,px),0);im.paste(tile,(max(0,(cw-tile.width)//2),b[1]))
    g = 0.80 if px <= 16 else 0.9                       # small sizes: a bit more contrast
    out = []
    for v in im.tobytes():
        a = (v / 255.0) ** g
        out.append(min(15, int(a * 15 + 0.5)))
    return out

def c_str(s):
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"') + '"'

def main():
    entries = load()
    # characters per size
    need = [set(chr(c) for c in range(32,127)) for _ in PX]
    # Direct Chinese subtitles are covered as well as dictionary translations.
    for filename in os.listdir(SRC):
        if filename.endswith(('.c','.h')) and filename!='ui_zh_data.h':
            source=open(os.path.join(SRC,filename),encoding='utf-8').read()
            for literal in re.findall(r'"(?:[^"\\]|\\.)*"',source):
                for ch in literal:
                    if ord(ch)>=128:
                        for chars in need[:3]:chars.add(ch)
    for en, zh, sc in entries:
        for c in zh:
            if ord(c) >= 128:
                for i in range(len(PX)):
                    if i < 3 or i < sc: need[i].add(c)
    sets = []
    for i, px in enumerate(PX):
        codes = sorted(ord(c) for c in need[i])
        if any(c > 0xFFFF for c in codes): sys.exit('characters outside the BMP')
        bits = bytearray()
        for c in codes:
            g = glyph(chr(c), px)
            if not any(g) and c!=32: print('warning: empty glyph %s at %d px' % (chr(c), px))
            if len(g) & 1: g.append(0)
            for k in range(0, len(g), 2): bits.append(g[k] | g[k + 1] << 4)
        sets.append((px, codes, bits))
    total = sum(len(s[1]) for s in sets)
    o = ['/* ui_zh_data.h - generated by tools/lang/gen_zh.py from tools/lang/zh.txt, do not edit.',
         ' * V6 unified Noto Sans CJK Medium: Latin + Chinese anti-aliased glyphs (Noto Sans CJK SC, SIL OFL 1.1), see gfx.c. */',
         '#ifndef UI_ZH_DATA_H', '#define UI_ZH_DATA_H', '#include <stdint.h>',
         '#define UI_ZH_SIZES %d' % len(PX), '#define UI_ZH_ENTRIES %d' % len(entries),
         '#define UI_ZH_GLYPHS %d          /* all sizes together */' % total,
         'typedef struct { const char *en, *zh; } UiZhEntry;',
         'static const UiZhEntry ui_zh_dict[UI_ZH_ENTRIES] = {']
    for en, zh, sc in sorted(entries, key=lambda e: e[0].encode('utf-8')):
        o.append('    { %s, %s },' % (c_str(en), c_str(zh)))
    o.append('};')
    base = 0
    for i, (px, codes, bits) in enumerate(sets):
        o.append('static const uint16_t ui_zh_codes%d[%d] = {' % (i, max(1, len(codes))))
        o.append('    ' + ','.join('0x%04x' % c for c in codes) if codes else '    0')
        o.append('};')
        o.append('static const uint8_t ui_zh_bits%d[%d] = {   /* %d px, 4-bit coverage, low nibble first */' % (i, max(1, len(bits)), px))
        for k in range(0, len(bits), 24):
            o.append('    ' + ','.join(str(b) for b in bits[k:k + 24]) + ',')
        if not bits: o.append('    0')
        o.append('};')
    o.append('typedef struct { uint8_t px; uint16_t n, base; const uint16_t *codes; const uint8_t *bits; } UiZhSet;')
    o.append('static const UiZhSet ui_zh_set[UI_ZH_SIZES] = {')
    for i, (px, codes, bits) in enumerate(sets):
        o.append('    { %d, %d, %d, ui_zh_codes%d, ui_zh_bits%d },' % (px, len(codes), base, i, i))
        base += len(codes)
    o.append('};')
    o.append('#endif')
    open(os.path.join(SRC, 'ui_zh_data.h'), 'w', encoding='utf-8').write('\n'.join(o) + '\n')
    nbytes = sum(len(s[2]) for s in sets)
    print('%d entries, glyphs per size %s, %d glyph bytes' % (len(entries), [len(s[1]) for s in sets], nbytes))
    # TR literals without an entry
    keys = {e[0] for e in entries}
    for f in ('render.c', 'th_render.c', 'demo.c', 'th_art.c'):
        s = open(os.path.join(SRC, f), encoding='utf-8').read()
        for m in re.finditer(r'TRX?\(("(?:[^"\\\n]|\\.)*"(?:\s*"(?:[^"\\\n]|\\.)*")*)[,)]', s):
            k = ''.join(re.findall(r'"((?:[^"\\\n]|\\.)*)"', m.group(1)))
            if k not in keys: print('note: no Chinese for %s: %r (stays English)' % (f, k))
    if '--preview' in sys.argv:
        out = sys.argv[sys.argv.index('--preview') + 1]
        W, rows = 1200, []
        for en, zh, sc in entries[:60]:
            rows.append((zh, min(sc, 3)))
        H = sum(PX[s - 1] + 6 for _, s in rows) + 10
        im = Image.new('RGB', (W, H), (18, 24, 27)); y = 5
        for zh, s in rows:
            px = PX[s - 1]; x = 5
            for c in zh:
                if ord(c) < 128: x += 6 * s; continue
                i = s - 1; codes = sets[i][1]
                if ord(c) in codes:
                    k = codes.index(ord(c)); b = sets[i][2]; off = k * ((px * px + 1) // 2)
                    for p in range(px * px):
                        v = (b[off + p // 2] >> (4 * (p & 1))) & 15
                        if v:
                            a = v / 15; r0 = im.getpixel((x + p % px, y + p // px))
                            im.putpixel((x + p % px, y + p // px), tuple(int(r0[q] * (1 - a) + (240, 232, 210)[q] * a) for q in range(3)))
                x += px
            y += px + 6
        im.save(out)

if __name__ == '__main__':
    main()
