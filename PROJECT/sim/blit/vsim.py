"""
vsim.py - tiny cycle-based simulator for a synthesizable Verilog-2001 subset.

Supported: ANSI module headers with parameters, parameter/localparam, wire/reg
(with ranges, initialisers, 1-D memories), assign, always @(*) (blocking),
always @(posedge clk) (non-blocking, memory writes), if/else, case/default,
begin/end, instances with #(.P(v)) and named ports.  All values unsigned.
Expression widths follow the Verilog context-determined rules.
Single clock domain: comb() settles the combinational logic, clock() is one
rising edge (all non-blocking updates applied together).

Written because the build VM has no iverilog/verilator; the RTL is kept inside
this subset on purpose.
"""
import re, random, sys

# ---------------------------------------------------------------- lexer
TOK = re.compile(r"""
 (?P<ws>\s+|//[^\n]*|/\*.*?\*/|`[^\n]*|\(\*\s*[A-Za-z_][^*]*\*\))
|(?P<num>\d*'[bBdDhHoO][0-9a-fA-F_xXzZ]+|\d[\d_]*)
|(?P<id>[A-Za-z_$][A-Za-z0-9_$]*)
|(?P<op><=|>=|==|!=|&&|\|\||<<|>>|[-+*/%&|^~!<>?:;,.(){}\[\]#@=])
""", re.S | re.X)


def lex(src):
    out, pos = [], 0
    while pos < len(src):
        m = TOK.match(src, pos)
        if not m:
            raise SyntaxError("lex error near: " + src[pos:pos + 40])
        pos = m.end()
        if m.lastgroup == 'ws':
            continue
        out.append((m.lastgroup, m.group(m.lastgroup)))
    out.append(('eof', ''))
    return out


def parse_num(t):
    t = t.replace('_', '')
    if "'" not in t:
        return int(t), None
    size, rest = t.split("'")
    base = {'b': 2, 'd': 10, 'h': 16, 'o': 8}[rest[0].lower()]
    digits = rest[1:].lower().replace('x', '0').replace('z', '0')
    v = int(digits, base)
    w = int(size) if size else None
    if w is not None:
        v &= (1 << w) - 1
    return v, w


# ---------------------------------------------------------------- parser
class P:
    BIN = [['||'], ['&&'], ['|'], ['^'], ['&'], ['==', '!='], ['<', '<=', '>', '>='],
           ['<<', '>>'], ['+', '-'], ['*', '/', '%']]

    def __init__(s, toks):
        s.t, s.i = toks, 0

    def peek(s, k=0):
        return s.t[s.i + k][1]

    def kind(s, k=0):
        return s.t[s.i + k][0]

    def nxt(s):
        v = s.t[s.i][1]
        s.i += 1
        return v

    def exp(s, v):
        if s.peek() != v:
            ctx = ' '.join(x[1] for x in s.t[max(0, s.i - 10):s.i + 6])
            raise SyntaxError("expected %r got %r near: %s" % (v, s.peek(), ctx))
        s.i += 1

    def acc(s, v):
        if s.peek() == v:
            s.i += 1
            return True
        return False

    # ---- expressions
    def expr(s):
        c = s.binop(0)
        if s.acc('?'):
            a = s.expr()
            s.exp(':')
            b = s.expr()
            return ('tern', c, a, b)
        return c

    def binop(s, lvl):
        if lvl == len(s.BIN):
            return s.unary()
        a = s.binop(lvl + 1)
        while s.kind() == 'op' and s.peek() in s.BIN[lvl]:
            op = s.nxt()
            b = s.binop(lvl + 1)
            a = ('bin', op, a, b)
        return a

    def unary(s):
        if s.kind() == 'op' and s.peek() in ('~', '!', '-', '&', '|', '^', '+'):
            op = s.nxt()
            return ('un', op, s.unary())
        return s.primary()

    def primary(s):
        k, v = s.t[s.i]
        if k == 'num':
            s.i += 1
            val, w = parse_num(v)
            return ('num', val, w)
        if v == '(':
            s.i += 1
            e = s.expr()
            s.exp(')')
            return e
        if v == '{':
            s.i += 1
            first = s.expr()
            if s.peek() == '{':          # replication
                s.i += 1
                items = [s.expr()]
                while s.acc(','):
                    items.append(s.expr())
                s.exp('}')
                s.exp('}')
                return ('rep', first, ('cat', items))
            items = [first]
            while s.acc(','):
                items.append(s.expr())
            s.exp('}')
            return ('cat', items)
        if k == 'id':
            s.i += 1
            e = ('id', v)
            while s.peek() == '[':
                s.i += 1
                a = s.expr()
                if s.acc(':'):
                    b = s.expr()
                    s.exp(']')
                    e = ('part', e, a, b)
                else:
                    s.exp(']')
                    e = ('idx', e, a)
            return e
        raise SyntaxError("bad primary %r" % v)

    # ---- module
    def range_(s):
        if s.peek() == '[':
            s.i += 1
            a = s.expr()
            s.exp(':')
            b = s.expr()
            s.exp(']')
            return (a, b)
        return None

    def modules(s):
        mods = {}
        while s.kind() != 'eof':
            s.exp('module')
            m = s.module()
            mods[m['name']] = m
        return mods

    def module(s):
        m = {'name': s.nxt(), 'params': [], 'ports': [], 'items': []}
        if s.acc('#'):
            s.exp('(')
            while not s.acc(')'):
                s.acc('parameter')
                rng = s.range_()
                name = s.nxt()
                s.exp('=')
                e = s.expr()
                m['params'].append((name, rng, e, False))
                s.acc(',')
        s.exp('(')
        d = None
        isreg = False
        rng = None
        while not s.acc(')'):
            if s.peek() in ('input', 'output', 'inout'):
                d = s.nxt()
                isreg = False
                if s.peek() in ('wire', 'reg'):
                    isreg = s.nxt() == 'reg'
                rng = s.range_()
            name = s.nxt()
            m['ports'].append((name, d, isreg, rng))
            m['items'].append(('decl', 'reg' if isreg else 'wire', rng, name, None, None))
            s.acc(',')
        s.exp(';')
        while not s.acc('endmodule'):
            s.item(m)
        return m

    def item(s, m):
        v = s.peek()
        if v in ('localparam', 'parameter'):
            s.i += 1
            rng = s.range_()
            while True:
                name = s.nxt()
                s.exp('=')
                e = s.expr()
                m['params'].append((name, rng, e, True))
                if not s.acc(','):
                    break
            s.exp(';')
        elif v in ('wire', 'reg'):
            s.i += 1
            rng = s.range_()
            while True:
                name = s.nxt()
                arr = s.range_()
                init = None
                if s.acc('='):
                    init = s.expr()
                m['items'].append(('decl', v, rng, name, arr, init))
                if not s.acc(','):
                    break
            s.exp(';')
        elif v == 'assign':
            s.i += 1
            lhs = s.lvalue()
            s.exp('=')
            rhs = s.expr()
            s.exp(';')
            m['items'].append(('assign', lhs, rhs))
        elif v == 'always':
            s.i += 1
            s.exp('@')
            s.exp('(')
            if s.acc('*'):
                sens = '*'
            else:
                s.exp('posedge')
                sens = s.nxt()
                # 'or posedge/negedge rst': async reset treated as synchronous
                while s.acc('or'):
                    s.nxt()
                    s.nxt()
            s.exp(')')
            m['items'].append(('always', sens, s.stmt()))
        elif v in ('integer', 'genvar', 'generate', 'function', 'initial'):
            raise SyntaxError("unsupported construct: " + v)
        else:                                   # instance
            mod = s.nxt()
            params = {}
            if s.acc('#'):
                s.exp('(')
                while not s.acc(')'):
                    s.exp('.')
                    pn = s.nxt()
                    s.exp('(')
                    params[pn] = s.expr()
                    s.exp(')')
                    s.acc(',')
            inst = s.nxt()
            s.exp('(')
            conns = {}
            while not s.acc(')'):
                s.exp('.')
                pn = s.nxt()
                s.exp('(')
                conns[pn] = None if s.peek() == ')' else s.expr()
                s.exp(')')
                s.acc(',')
            s.exp(';')
            m['items'].append(('inst', mod, inst, params, conns))

    def lvalue(s):
        name = s.nxt()
        e = ('id', name)
        while s.peek() == '[':
            s.i += 1
            a = s.expr()
            if s.acc(':'):
                b = s.expr()
                s.exp(']')
                e = ('part', e, a, b)
            else:
                s.exp(']')
                e = ('idx', e, a)
        return e

    def stmt(s):
        v = s.peek()
        if v == 'begin':
            s.i += 1
            if s.acc(':'):
                s.nxt()
            body = []
            while not s.acc('end'):
                body.append(s.stmt())
            return ('blk', body)
        if v == 'if':
            s.i += 1
            s.exp('(')
            c = s.expr()
            s.exp(')')
            a = s.stmt()
            b = None
            if s.acc('else'):
                b = s.stmt()
            return ('if', c, a, b)
        if v == 'case':
            s.i += 1
            s.exp('(')
            c = s.expr()
            s.exp(')')
            items = []
            dflt = None
            while not s.acc('endcase'):
                if s.acc('default'):
                    s.acc(':')
                    dflt = s.stmt()
                else:
                    labels = [s.expr()]
                    while s.acc(','):
                        labels.append(s.expr())
                    s.exp(':')
                    items.append((labels, s.stmt()))
            return ('case', c, items, dflt)
        if v == ';':
            s.i += 1
            return ('blk', [])
        lhs = s.lvalue()
        if s.acc('<='):
            rhs = s.expr()
            s.exp(';')
            return ('nba', lhs, rhs)
        s.exp('=')
        rhs = s.expr()
        s.exp(';')
        return ('ba', lhs, rhs)


# ---------------------------------------------------------------- elaboration
class Design:
    def __init__(d):
        d.sig = {}          # name -> idx
        d.width = []        # idx -> width
        d.names = []
        d.mems = {}         # name -> (mid, width, depth)
        d.mem_w = []
        d.mem_depth = []
        d.mem_names = []
        d.comb = []
        d.seq = []
        d.nba_targets = set()

    def add_sig(d, name, w):
        if name in d.sig:
            raise NameError("duplicate signal " + name)
        d.sig[name] = len(d.width)
        d.width.append(w)
        d.names.append(name)


class Scope:
    def __init__(sc, prefix, params):
        sc.prefix = prefix
        sc.params = params  # name -> (val, width)


def cval(e, params):
    """evaluate a constant expression"""
    t = e[0]
    if t == 'num':
        return e[1]
    if t == 'id':
        if e[1] not in params:
            raise NameError("not a constant: " + e[1])
        return params[e[1]][0]
    if t == 'un':
        v = cval(e[2], params)
        return {'-': -v, '~': ~v, '!': int(not v), '+': v}[e[1]]
    if t == 'bin':
        a, b = cval(e[2], params), cval(e[3], params)
        op = e[1]
        return {'+': a + b, '-': a - b, '*': a * b,
                '/': a // b if b else 0, '%': a % b if b else 0,
                '<<': a << b, '>>': a >> b, '&': a & b, '|': a | b, '^': a ^ b,
                '==': int(a == b), '!=': int(a != b), '<': int(a < b), '<=': int(a <= b),
                '>': int(a > b), '>=': int(a >= b),
                '&&': int(bool(a) and bool(b)), '||': int(bool(a) or bool(b))}[op]
    if t == 'tern':
        return cval(e[2], params) if cval(e[1], params) else cval(e[3], params)
    if t == 'idx':
        return (cval(e[1], params) >> cval(e[2], params)) & 1
    if t == 'part':
        hi, lo = cval(e[2], params), cval(e[3], params)
        return (cval(e[1], params) >> lo) & ((1 << (hi - lo + 1)) - 1)
    raise NameError("not a constant expression %r" % (e,))


def cwidth(e, params):
    t = e[0]
    if t == 'num':
        return e[2] or 32
    if t == 'id':
        return params[e[1]][1]
    return 32


def elaborate(mods, top, top_params=None):
    D = Design()
    _elab(D, mods, mods[top], '', top_params or {}, None, None)
    return D


def _elab(D, mods, m, prefix, overrides, parent_scope, conns):
    params = {}
    for (name, rng, e, is_local) in m['params']:
        if not is_local and name in overrides and isinstance(overrides[name], int):
            v, w = overrides[name], 32                 # top-level override (python int)
        elif not is_local and name in overrides:
            v = cval(overrides[name], parent_scope.params)
            w = cwidth(overrides[name], parent_scope.params)
        else:
            v = cval(e, params)
            w = cwidth(e, params)
        if rng is not None:
            hi, lo = cval(rng[0], params), cval(rng[1], params)
            w = hi - lo + 1
            v &= (1 << w) - 1
        params[name] = (v, w)
    for k in overrides:
        if k not in params:
            raise NameError("%s: no parameter %s" % (m['name'], k))
    sc = Scope(prefix, params)
    for it in m['items']:
        if it[0] == 'decl':
            _, kind, rng, name, arr, init = it
            w = 1
            if rng is not None:
                w = cval(rng[0], params) - cval(rng[1], params) + 1
            if arr is not None:
                a, b = cval(arr[0], params), cval(arr[1], params)
                depth = abs(b - a) + 1
                mid = len(D.mem_w)
                D.mems[prefix + name] = (mid, w, depth)
                D.mem_w.append(w)
                D.mem_depth.append(depth)
                D.mem_names.append(prefix + name)
            else:
                D.add_sig(prefix + name, w)
    if conns is not None:
        pnames = set(p[0] for p in m['ports'])
        for k in conns:
            if k not in pnames:
                raise NameError("%s: no port %s" % (m['name'], k))
        for (pname, d, isreg, rng) in m['ports']:
            ce = conns.get(pname)
            if ce is None:
                if d == 'input':
                    raise NameError("unconnected input %s%s" % (prefix, pname))
                continue
            if d == 'input':
                D.comb.append(('assign', ('id', pname), ce, sc, parent_scope))
            else:
                D.comb.append(('assign', ce, ('id', pname), parent_scope, sc))
    for it in m['items']:
        if it[0] == 'decl' and it[5] is not None:
            D.comb.append(('assign', ('id', it[3]), it[5], sc, sc))
        elif it[0] == 'assign':
            D.comb.append(('assign', it[1], it[2], sc, sc))
        elif it[0] == 'always':
            if it[1] == '*':
                D.comb.append(('always', it[2], sc))
            else:
                D.seq.append((it[1], it[2], sc))
        elif it[0] == 'inst':
            _, modname, inst, pov, cn = it
            if modname not in mods:
                raise NameError("unknown module " + modname)
            _elab(D, mods, mods[modname], prefix + inst + '.', pov, sc, cn)
    return D


# ---------------------------------------------------------------- code generation
def M(w):
    return (1 << w) - 1


class Gen:
    def __init__(g, D):
        g.D = D
        g.tmp = 0

    def resolve(g, name, sc):
        if name in sc.params:
            return ('const',) + sc.params[name]
        full = sc.prefix + name
        if full in g.D.sig:
            i = g.D.sig[full]
            return ('sig', i, g.D.width[i])
        if full in g.D.mems:
            return ('mem',) + g.D.mems[full]
        raise NameError("unknown identifier %s" % full)

    def selfw(g, e, sc):
        t = e[0]
        if t == 'num':
            return e[2] or 32
        if t == 'id':
            r = g.resolve(e[1], sc)
            if r[0] in ('const', 'sig'):
                return r[2]
            raise ValueError("memory used without index: " + e[1])
        if t == 'idx':
            if e[1][0] == 'id':
                r = g.resolve(e[1][1], sc)
                if r[0] == 'mem':
                    return r[2]
            return 1
        if t == 'part':
            return cval(e[2], sc.params) - cval(e[3], sc.params) + 1
        if t == 'un':
            if e[1] in ('~', '-', '+'):
                return g.selfw(e[2], sc)
            return 1
        if t == 'bin':
            op = e[1]
            if op in ('+', '-', '*', '/', '%', '&', '|', '^'):
                return max(g.selfw(e[2], sc), g.selfw(e[3], sc))
            if op in ('<<', '>>'):
                return g.selfw(e[2], sc)
            return 1
        if t == 'tern':
            return max(g.selfw(e[2], sc), g.selfw(e[3], sc))
        if t == 'cat':
            return sum(g.selfw(x, sc) for x in e[1])
        if t == 'rep':
            n = cval(e[1], sc.params)
            if n <= 0:
                raise ValueError("replication count %d" % n)
            return n * g.selfw(e[2], sc)
        raise ValueError(t)

    def reads(g, e, sc, acc):
        t = e[0]
        if t == 'id':
            r = g.resolve(e[1], sc)
            if r[0] == 'sig':
                acc.add(r[1])
        elif t in ('idx', 'part'):
            for x in e[1:]:
                g.reads(x, sc, acc)
        elif t == 'un':
            g.reads(e[2], sc, acc)
        elif t == 'bin':
            g.reads(e[2], sc, acc)
            g.reads(e[3], sc, acc)
        elif t == 'tern':
            for x in e[1:]:
                g.reads(x, sc, acc)
        elif t == 'cat':
            for x in e[1]:
                g.reads(x, sc, acc)
        elif t == 'rep':
            g.reads(e[2], sc, acc)

    def ex(g, e, sc, w):
        """python expression for e evaluated in context width w, value in [0, 2^w)"""
        t = e[0]
        if t == 'num':
            return str(e[1] & M(w))
        if t == 'id':
            r = g.resolve(e[1], sc)
            if r[0] == 'const':
                return str(r[1] & M(w))
            return 's[%d]' % r[1]
        if t == 'idx':
            base = e[1]
            if base[0] == 'id':
                r = g.resolve(base[1], sc)
                if r[0] == 'mem':
                    return 'm[%d][%s]' % (r[1], g.ex(e[2], sc, g.selfw(e[2], sc)))
            bw = g.selfw(base, sc)
            try:
                k = cval(e[2], sc.params)
                return '((%s >> %d) & 1)' % (g.ex(base, sc, bw), k)
            except NameError:
                return '((%s >> %s) & 1)' % (g.ex(base, sc, bw), g.ex(e[2], sc, g.selfw(e[2], sc)))
        if t == 'part':
            hi, lo = cval(e[2], sc.params), cval(e[3], sc.params)
            bw = g.selfw(e[1], sc)
            return '((%s >> %d) & %d)' % (g.ex(e[1], sc, bw), lo, M(hi - lo + 1))
        if t == 'un':
            op = e[1]
            if op == '~':
                return '(~%s & %d)' % (g.ex(e[2], sc, w), M(w))
            if op == '-':
                return '(-%s & %d)' % (g.ex(e[2], sc, w), M(w))
            if op == '+':
                return g.ex(e[2], sc, w)
            sw = g.selfw(e[2], sc)
            x = g.ex(e[2], sc, sw)
            if op == '!':
                return 'int(%s == 0)' % x
            if op == '|':
                return 'int(%s != 0)' % x
            if op == '&':
                return 'int(%s == %d)' % (x, M(sw))
            if op == '^':
                return "(bin(%s).count('1') & 1)" % x
        if t == 'bin':
            op = e[1]
            if op in ('+', '-', '*'):
                return '((%s %s %s) & %d)' % (g.ex(e[2], sc, w), op, g.ex(e[3], sc, w), M(w))
            if op == '<<':
                return '((%s << %s) & %d)' % (g.ex(e[2], sc, w),
                                              g.ex(e[3], sc, g.selfw(e[3], sc)), M(w))
            if op in ('&', '|', '^'):
                return '(%s %s %s)' % (g.ex(e[2], sc, w), op, g.ex(e[3], sc, w))
            if op == '>>':
                return '(%s >> %s)' % (g.ex(e[2], sc, w), g.ex(e[3], sc, g.selfw(e[3], sc)))
            if op in ('/', '%'):
                return '_div(%s, %s, %d)' % (g.ex(e[2], sc, w), g.ex(e[3], sc, w),
                                             0 if op == '/' else 1)
            if op in ('==', '!=', '<', '<=', '>', '>='):
                w2 = max(g.selfw(e[2], sc), g.selfw(e[3], sc))
                return 'int(%s %s %s)' % (g.ex(e[2], sc, w2), op, g.ex(e[3], sc, w2))
            if op == '&&':
                return 'int((%s != 0) and (%s != 0))' % (g.ex(e[2], sc, g.selfw(e[2], sc)),
                                                         g.ex(e[3], sc, g.selfw(e[3], sc)))
            if op == '||':
                return 'int((%s != 0) or (%s != 0))' % (g.ex(e[2], sc, g.selfw(e[2], sc)),
                                                        g.ex(e[3], sc, g.selfw(e[3], sc)))
        if t == 'tern':
            c = g.ex(e[1], sc, g.selfw(e[1], sc))
            return '(%s if %s else %s)' % (g.ex(e[2], sc, w), c, g.ex(e[3], sc, w))
        if t == 'cat':
            parts, sh = [], 0
            for x in reversed(e[1]):
                xw = g.selfw(x, sc)
                xe = g.ex(x, sc, xw)
                parts.append('(%s << %d)' % (xe, sh) if sh else xe)
                sh += xw
            return '(' + ' | '.join(parts) + ')'
        if t == 'rep':
            n = cval(e[1], sc.params)
            xw = g.selfw(e[2], sc)
            mul = sum(1 << (xw * i) for i in range(n))
            return '(%s * %d)' % (g.ex(e[2], sc, xw), mul)
        raise ValueError("expr %r" % (e,))

    def rhs(g, e, sc, lw):
        w = max(lw, g.selfw(e, sc))
        x = g.ex(e, sc, w)
        if w > lw:
            x = '(%s & %d)' % (x, M(lw))
        return x

    def lv(g, lhs, sc):
        if lhs[0] == 'id':
            r = g.resolve(lhs[1], sc)
            if r[0] != 'sig':
                raise ValueError("bad lvalue " + lhs[1])
            return r
        if lhs[0] == 'idx' and lhs[1][0] == 'id':
            r = g.resolve(lhs[1][1], sc)
            if r[0] == 'mem':
                return ('mem', r[1], r[2], g.ex(lhs[2], sc, g.selfw(lhs[2], sc)), lhs[2])
        raise ValueError("unsupported lvalue %r" % (lhs,))

    def stmt(g, st, sc, ind, out, mode, rd, wr):
        pad = '    ' * ind
        t = st[0]
        if t == 'blk':
            if not st[1]:
                out.append(pad + 'pass')
            for x in st[1]:
                g.stmt(x, sc, ind, out, mode, rd, wr)
        elif t == 'if':
            g.reads(st[1], sc, rd)
            out.append(pad + 'if %s:' % g.ex(st[1], sc, g.selfw(st[1], sc)))
            g.stmt(st[2], sc, ind + 1, out, mode, rd, wr)
            if st[3] is not None:
                out.append(pad + 'else:')
                g.stmt(st[3], sc, ind + 1, out, mode, rd, wr)
        elif t == 'case':
            g.reads(st[1], sc, rd)
            g.tmp += 1
            tv = '_c%d' % g.tmp
            cw = g.selfw(st[1], sc)
            for labels, _ in st[2]:
                for l in labels:
                    cw = max(cw, g.selfw(l, sc))
            out.append(pad + '%s = %s' % (tv, g.ex(st[1], sc, cw)))
            first = True
            for labels, body in st[2]:
                cond = ' or '.join('%s == %s' % (tv, g.ex(l, sc, cw)) for l in labels)
                for l in labels:
                    g.reads(l, sc, rd)
                out.append(pad + ('if %s:' if first else 'elif %s:') % cond)
                g.stmt(body, sc, ind + 1, out, mode, rd, wr)
                first = False
            if st[3] is not None:
                if first:
                    g.stmt(st[3], sc, ind, out, mode, rd, wr)
                else:
                    out.append(pad + 'else:')
                    g.stmt(st[3], sc, ind + 1, out, mode, rd, wr)
        elif t in ('nba', 'ba'):
            if (t == 'nba') != (mode == 'seq'):
                raise ValueError("blocking/non-blocking mix in %s block: %r" % (mode, st))
            g.reads(st[2], sc, rd)
            L = g.lv(st[1], sc)
            if L[0] == 'sig':
                code = g.rhs(st[2], sc, L[2])
                wr.add(L[1])
                if mode == 'seq':
                    out.append(pad + 'n[%d] = %s' % (L[1], code))
                    g.D.nba_targets.add(L[1])
                else:
                    out.append(pad + 's[%d] = %s' % (L[1], code))
            else:
                g.reads(L[4], sc, rd)
                if mode != 'seq':
                    raise ValueError("memory write in combinational block")
                out.append(pad + 'W.append((%d, %s, %s))' % (L[1], L[3], g.rhs(st[2], sc, L[2])))
        else:
            raise ValueError(t)


def build(D):
    g = Gen(D)
    units = []
    for c in D.comb:
        if c[0] == 'assign':
            _, lhs, rhs, lsc, rsc = c
            L = g.lv(lhs, lsc)
            if L[0] != 'sig':
                raise ValueError("assign to non-signal %r" % (lhs,))
            rd = set()
            g.reads(rhs, rsc, rd)
            units.append((rd, {L[1]}, ['s[%d] = %s' % (L[1], g.rhs(rhs, rsc, L[2]))]))
        else:
            _, st, sc = c
            rd, wr, out = set(), set(), []
            g.stmt(st, sc, 0, out, 'comb', rd, wr)
            units.append((rd - wr, wr, out))
    writer = {}
    for ui, (rd, wr, _) in enumerate(units):
        for x in wr:
            if x in writer:
                raise ValueError("multiple drivers: " + D.names[x])
            writer[x] = ui
    seq_out, seq_wr = [], set()
    for clk, st, sc in D.seq:
        g.stmt(st, sc, 1, seq_out, 'seq', set(), seq_wr)
    for x in seq_wr:
        if x in writer:
            raise ValueError("driven by both assign and always: " + D.names[x])
    deps = [set(writer[x] for x in rd if x in writer and writer[x] != ui)
            for ui, (rd, wr, _) in enumerate(units)]
    order, state = [], [0] * len(units)
    sys.setrecursionlimit(100000)

    def visit(u, stack):
        if state[u] == 2:
            return
        if state[u] == 1:
            cyc = stack[stack.index(u):]
            raise ValueError("combinational loop: " + ' -> '.join(
                ','.join(D.names[x] for x in units[k][1]) for k in cyc))
        state[u] = 1
        stack.append(u)
        for d in deps[u]:
            visit(d, stack)
        stack.pop()
        state[u] = 2
        order.append(u)

    for u in range(len(units)):
        visit(u, [])
    # undriven signals (except top inputs) are reported by the caller
    D.driven = set(writer) | seq_wr
    lines = ['def comb(s, m):']
    for u in order:
        lines += ['    ' + l for l in units[u][2]]
    lines.append('    return')
    lines.append('def seq(s, m):')
    lines.append('    n = {}')
    lines.append('    W = []')
    lines += seq_out
    lines.append('    for k, v in n.items(): s[k] = v')
    lines.append('    for (j, a, v) in W: m[j][a] = v')
    src = '\n'.join(lines)
    env = {'_div': lambda a, b, r: (a % b if r else a // b) if b else 0}
    exec(compile(src, 'vsim_gen', 'exec'), env)
    return env['comb'], env['seq'], src


class Sim:
    def __init__(sim, files, top, seed=1, params=None):
        text = ''
        for f in files:
            text += open(f).read() + '\n'
        mods = P(lex(text)).modules()
        sim.mods = mods
        sim.D = elaborate(mods, top, params)
        sim._comb, sim._seq, sim.src = build(sim.D)
        rnd = random.Random(seed)
        # random power-up state (catches use of unreset registers)
        sim.s = [rnd.getrandbits(w) for w in sim.D.width]
        sim.m = [[rnd.getrandbits(w) for _ in range(d)]
                 for w, d in zip(sim.D.mem_w, sim.D.mem_depth)]
        sim.idx = sim.D.sig
        top_inputs = set(p[0] for p in mods[top]['ports'] if p[1] == 'input')
        sim.undriven = [n for i, n in enumerate(sim.D.names)
                        if i not in sim.D.driven and n not in top_inputs]

    def __getitem__(sim, name):
        return sim.s[sim.idx[name]]

    def __setitem__(sim, name, v):
        i = sim.idx[name]
        sim.s[i] = v & M(sim.D.width[i])

    def comb(sim):
        sim._comb(sim.s, sim.m)

    def clock(sim):
        sim._seq(sim.s, sim.m)
