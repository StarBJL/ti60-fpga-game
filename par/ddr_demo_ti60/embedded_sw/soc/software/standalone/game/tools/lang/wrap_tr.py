#!/usr/bin/env python3
"""wrap_tr.py (2026-10-28): wrap the user-visible string literals of the HUD sources in TR("...")
(gfx.h: translated when the UI language is Chinese, nothing in English). Only literals inside function
bodies, not in initializers (static arrays: wrap the use site by hand), not in tx_printf / tx_puts /
bsp_printf lines, only literals with two letters in a row. Idempotent (skips TR( already there).
  python3 tools/lang/wrap_tr.py src/render.c src/th_render.c src/demo.c   [--dry]"""
import re, sys

SKIP_LINE = re.compile(r'tx_printf|tx_puts|bsp_printf|#\s*include|#\s*define|(?<![sn])printf\(')

def wrap(src):
    out = []; i = 0; n = len(src)
    stack = []          # 'func' / 'init' / 'block' / 'other'
    prev_sig = ''       # last significant char outside strings / comments
    paren = 0
    count = 0
    line_start = 0
    while i < n:
        c = src[i]
        if c == '\n': line_start = i + 1
        if src.startswith('/*', i):
            j = src.index('*/', i) + 2; out.append(src[i:j]); i = j; continue
        if src.startswith('//', i):
            j = src.find('\n', i); j = n if j < 0 else j; out.append(src[i:j]); i = j; continue
        if c == '#' and src[line_start:i].strip() == '':
            j = i
            while True:                                  # preprocessor line (with continuations)
                k = src.find('\n', j); k = n if k < 0 else k
                if src[k - 1:k] == '\\': j = k + 1; continue
                break
            out.append(src[i:k]); i = k; continue
        if c == "'":
            m = re.match(r"'(?:[^'\\]|\\.)*'", src[i:]); out.append(m.group(0)); i += len(m.group(0)); prev_sig = "'"; continue
        if c == '"':
            # a run of adjacent literals
            m = re.match(r'"(?:[^"\\\n]|\\.)*"(?:\s*"(?:[^"\\\n]|\\.)*")*', src[i:])
            lit = m.group(0)
            le = src.find('\n', i); le = n if le < 0 else le
            line = src[line_start:le]
            in_func = 'func' in stack and 'init' not in stack
            before = ''.join(out)[-4:]
            body = ''.join(re.findall(r'"((?:[^"\\\n]|\\.)*)"', lit))
            if (in_func and not SKIP_LINE.search(line) and re.search(r'[A-Za-z]{2}', body)
                    and not before.endswith('TR(')):
                out.append('TR(' + lit + ')'); count += 1
            else:
                out.append(lit)
            i += len(lit); prev_sig = '"'; continue
        if c == '(': paren += 1
        if c == ')': paren -= 1
        if c == '{':
            if prev_sig == '=' or (stack and stack[-1] == 'init'): stack.append('init')
            elif prev_sig == ')' and not stack: stack.append('func')
            elif not stack: stack.append('other')
            else: stack.append('block')
        if c == '}' and stack: stack.pop()
        if not c.isspace(): prev_sig = c
        out.append(c); i += 1
    return ''.join(out), count

if __name__ == '__main__':
    dry = '--dry' in sys.argv
    for f in [a for a in sys.argv[1:] if not a.startswith('--')]:
        s = open(f, encoding='utf-8').read()
        t, k = wrap(s)
        print(f, k, 'literals wrapped')
        if not dry and t != s: open(f, 'w', encoding='utf-8').write(t)
