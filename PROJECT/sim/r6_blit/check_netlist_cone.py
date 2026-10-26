#!/usr/bin/env python3
"""
check_netlist_cone.py - evaluate the logic cone in front of a register bus in the
Efinity netlist (outflow/<proj>.map.v: EFX_LUT4 / EFX_ADD / EFX_FF) and compare it
with a Python formula, for random values of the cone inputs.

Why (2026-10-20): Efinity synthesis turned
    rd_words <= rd_words + ... + {7'd0, rdi_k, 3'd0} + {4'd0, rdi_x, 2'b00} - ...
into an adder at bit 2 with rdi_k on its carry-in (+4 instead of +8). RTL simulation
cannot see that; only the netlist can. This checks the rd_words counter of the
BitBlt engine (default) - run it after every Efinity build that touches the engine:

    python3 check_netlist_cone.py [netlist]          (default ../../par/ddr_demo_ti60/outflow/ddr_demo_ti60.map.v)
    python3 check_netlist_cone.py [netlist] --leaves (only list the cone inputs)

Exit 0 = OK (all random vectors match), 1 = mismatch, 2 = cone inputs not understood.
"""
import os, re, sys, random

here = os.path.dirname(os.path.abspath(__file__))
args = [a for a in sys.argv[1:] if not a.startswith('--')]
path = args[0] if args else os.path.join(here, '..', '..', 'par', 'ddr_demo_ti60', 'outflow', 'ddr_demo_ti60.map.v')
LEAVES_ONLY = '--leaves' in sys.argv

TARGET = 'u_blit/rd_words'      # register bus
WIDTH = 11
# The expected next value is the RTL formula; every RTL signal in it is read from the
# netlist itself (its net, evaluated from the same random cone inputs), so the check
# is about the rd_words adder only. ddr3_example_top.v connects the engine's
# m_rvalid / x_rvalid to bl_rvalid / bx_rvalid.
M_RVALID, X_RVALID = 'bl_rvalid', 'bx_rvalid'


def norm(n):
    n = n.strip()
    if n.startswith('\\'):
        n = n[1:]
    return re.sub(r'\s+\[', '[', n).replace(' ', '')


src = open(path, encoding='utf-8', errors='replace').read()
src = re.sub(r'//[^\n]*', '', src)
drv = {}            # net -> (kind, data)
ffd = {}            # FF Q net -> (D net, D polarity)
port_re = re.compile(r'\.(\w+)\(\s*([^()]*?)\s*\)')
for st in src.split(';'):
    m = re.match(r'\s*(EFX_LUT4|EFX_ADD|EFX_FF)\s+(\S+)\s*\(', st)
    if not m:
        continue
    kind = m.group(1)
    p = {k: norm(v) for k, v in port_re.findall(st)}
    attr = dict(re.findall(r'(\w+)=([0-9]+\'[bh][0-9a-fA-F]+)', st))
    if kind == 'EFX_LUT4':
        mask = int(attr.get('LUTMASK', "16'h0").split("'h")[1], 16)
        drv[p['O']] = ('L', [p.get('I%d' % i) for i in range(4)], mask)
    elif kind == 'EFX_ADD':
        pol = [attr.get('I0_POLARITY', "1'b1").endswith('1'), attr.get('I1_POLARITY', "1'b1").endswith('1')]
        ins = (p.get('I0'), p.get('I1'), p.get('CI'), pol)
        if 'O' in p:
            drv[p['O']] = ('S', ins)
        if 'CO' in p:
            drv[p['CO']] = ('C', ins)
    else:
        if 'Q' in p and 'D' in p:
            ffd[p['Q']] = (p['D'], attr.get('D_POLARITY', "1'b1").endswith('1'))

targets = []
for i in range(WIDTH):
    q = '%s[%d]' % (TARGET, i)
    if q not in ffd:
        sys.exit('register %s not found in %s' % (q, path))
    targets.append(ffd[q])

leaves = set()
def collect(n, seen):
    if n is None or n in seen or n in ("1'b0", "1'b1"):
        return
    seen.add(n)
    d = drv.get(n)
    if d is None:
        leaves.add(n)
        return
    for x in (d[1] if d[0] == 'L' else d[1][:3]):
        collect(x, seen)
seen = set()
for dn, _ in targets:
    collect(dn, seen)
rnd = random.Random(1)
print('netlist: %s\ncone of %s[%d:0]: %d cells, %d inputs' % (path, TARGET, WIDTH - 1, len(seen) - len(leaves), len(leaves)))


def bus(name):
    m = re.match(r'(.*)\[(\d+)\]$', name)
    return (m.group(1), int(m.group(2))) if m else (name, 0)

groups = {}
for l in sorted(leaves):
    b, i = bus(l)
    groups.setdefault(b, []).append(i)
if LEAVES_ONLY:
    for b in sorted(groups):
        print('  input %-40s bits %s' % (b, sorted(groups[b])))
    sys.exit(0)


def ev(n, val, memo):
    if n is None or n == "1'b0":
        return 0
    if n == "1'b1":
        return 1
    if n in memo:
        return memo[n]
    d = drv.get(n)
    if d is None:
        if n not in val:
            val[n] = rnd.getrandbits(1)      # FF / RAM output outside the cone: any value
        r = val[n]
    elif d[0] == 'L':
        idx = 0
        for k, x in enumerate(d[1]):
            idx |= ev(x, val, memo) << k
        r = d[2] >> idx & 1
    else:
        a, b, c, pol = d[1]
        a = ev(a, val, memo) ^ (0 if pol[0] else 1)
        b = ev(b, val, memo) ^ (0 if pol[1] else 1)
        c = ev(c, val, memo)
        r = (a ^ b ^ c) if d[0] == 'S' else ((a & b) | (a & c) | (b & c))
    memo[n] = r
    return r


def exists(n):
    return n in drv or n in leaves or n in ffd or ('%s[0]' % n) in drv or ('%s[0]' % n) in ffd or ('%s[0]' % n) in leaves


def sig(n, w, val, memo):
    if w == 1 and (n in drv or n in ffd or n in leaves):
        return ev(n, val, memo)
    return sum(ev('%s[%d]' % (n, i), val, memo) << i for i in range(w))


# RTL formula, new style (registered increments) or old style (combinational)
NEW = [('u_blit/rdi_m', 7, 1), ('u_blit/rdi_f', 7, 1), ('u_blit/rdi_l', 7, 1), ('u_blit/rdi_k', 1, 8),
       ('u_blit/rdi_x', 5, 4)]
present = [t for t in NEW if exists(t[0])]
if present:
    style = 'registered increments: ' + ' '.join(t[0].split('/')[1] for t in present)
    def inc(val, memo):
        return sum(sig(n, w, val, memo) * k for n, w, k in present)
else:
    need = ['u_blit/f_ar', 'u_blit/l_issue', 'u_blit/kb_fill', 'u_blit/xr_iss']
    miss = [n for n in need if not exists(n)]
    if miss:
        print('  !! old-style engine but nets %s are not in the netlist: cannot check' % miss)
        sys.exit(2)
    style = 'combinational increments (old engine)'
    def inc(val, memo):
        return (sig('u_blit/f_arw', 7, val, memo) if sig('u_blit/f_ar', 1, val, memo) else 0) + \
               (sig('u_blit/l_len', 7, val, memo) if sig('u_blit/l_issue', 1, val, memo) else 0) + \
               8 * sig('u_blit/kb_fill', 1, val, memo) + \
               (4 * sig('u_blit/xr_len', 5, val, memo) if sig('u_blit/xr_iss', 1, val, memo) else 0)
for n in (M_RVALID, X_RVALID):
    if not exists(n):
        print('  !! net %s not in the netlist' % n)
        sys.exit(2)
print('  formula: rd_words + %s - %s - 4 * %s' % (style, M_RVALID, X_RVALID))

rnd = random.Random(1)
bad = 0
N = 3000
for t in range(N):
    val, memo = {}, {}
    for l in leaves:
        val[l] = rnd.getrandbits(1)
    got = 0
    for i, (dn, pol) in enumerate(targets):
        got |= (ev(dn, val, memo) ^ (0 if pol else 1)) << i
    exp = (sig('u_blit/rd_words', WIDTH, val, memo) + inc(val, memo) - sig(M_RVALID, 1, val, memo)
           - 4 * sig(X_RVALID, 1, val, memo)) & ((1 << WIDTH) - 1)
    if got != exp:
        bad += 1
        if bad <= 6:
            d = ((got - exp + 1024) % 2048) - 1024
            ks = ', '.join('%s %d' % (n.split('/')[-1], sig(n, w, val, memo)) for n, w, _ in present) if present else ''
            print('  MISMATCH: netlist %d, RTL %d (netlist %+d)  %s' % (got, exp, d, ks))
print('%s: %d / %d random vectors differ' % ('FAIL' if bad else 'OK', bad, N))
sys.exit(1 if bad else 0)
