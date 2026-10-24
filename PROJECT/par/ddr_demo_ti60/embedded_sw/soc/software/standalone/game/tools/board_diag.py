#!/usr/bin/env python3
"""
board_diag.py - 程序跑不起来 / 黑屏 / 串口没输出时，经 OpenOCD 直接看板子状态（2026-10-20）。

  python tools/board_diag.py            # IDE Debug 会话开着（OpenOCD TCL 端口 6666）时运行
  python tools/board_diag.py --resume   # 看完让 CPU 继续跑
  python tools/board_diag.py --dvi --resume   # 另外把 HDMI 切回纯 DVI（没有数据岛 / 音频），看画面是否恢复

打印：CPU 停在哪个函数（按 build/game.elf 的符号表）；DDR 里的代码和 elf 是否一致（下载 / DDR 是否正常）；
hdmi_fb 寄存器（帧计数是否在走 = 像素时钟和扫描在工作，当前显示的帧缓冲，音频控制 / 已发采样数）；
BitBlt 引擎和混音器的 ID；串口状态。把整段输出发给 Claude 即可。
"""
import os, sys, time, socket, struct, argparse

HERE = os.path.dirname(os.path.abspath(__file__))
GAME = os.path.dirname(HERE)


class Tcl:
    def __init__(s, host, port):
        s.s = socket.create_connection((host, port), timeout=5)
        s.s.settimeout(30)

    def __call__(s, cmd):
        s.s.sendall(cmd.encode() + b'\x1a')
        buf = b''
        while not buf.endswith(b'\x1a'):
            d = s.s.recv(65536)
            if not d:
                raise ConnectionError('OpenOCD closed the connection')
            buf += d
        return buf[:-1].decode(errors='replace').strip()


def mdw(t, addr, n):
    r = t('mdw 0x%08x %d' % (addr, n))
    vals = []
    for line in r.splitlines():
        if ':' not in line:
            continue
        for w in line.split(':', 1)[1].split():
            try:
                vals.append(int(w, 16))
            except ValueError:
                pass
    if len(vals) < n:
        raise RuntimeError('mdw 0x%08x: %s' % (addr, r))
    return vals[:n]


def reg(t, name):
    r = t('reg %s' % name)
    for tok in r.replace(':', ' ').split():
        if tok.startswith('0x'):
            return int(tok, 16)
    return None


def elf_info(path):
    d = open(path, 'rb').read()
    if d[:4] != b'\x7fELF' or d[4] != 1:
        return None, []
    shoff, = struct.unpack_from('<I', d, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from('<HHH', d, 0x2E)
    secs = [struct.unpack_from('<IIIIIIIIII', d, shoff + i * shentsize) for i in range(shnum)]
    strtab_sec = secs[shstrndx]
    def sname(off):
        s = strtab_sec[4] + off
        return d[s:d.index(b'\0', s)].decode()
    text = None
    syms = []
    for s in secs:
        nm = sname(s[0])
        if nm == '.text':
            text = (s[3], d[s[4]:s[4] + s[5]])
        if s[1] == 2:                                   # SHT_SYMTAB
            strs = secs[s[6]]
            for k in range(s[5] // 16):
                st_name, st_value, st_size, st_info, _, _ = struct.unpack_from('<IIIBBH', d, s[4] + 16 * k)
                if st_info & 0xF == 2 and st_value:     # FUNC
                    e = d.index(b'\0', strs[4] + st_name)
                    syms.append((st_value, st_size, d[strs[4] + st_name:e].decode()))
    syms.sort()
    return text, syms


def func_at(syms, pc):
    best = None
    for v, sz, nm in syms:
        if v <= pc and (pc < v + max(sz, 1) or best is None or v > best[0]):
            if v <= pc:
                best = (v, nm)
    return '%s+0x%x' % (best[1], pc - best[0]) if best else '?'


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--elf', default=os.path.join(GAME, 'build', 'game.elf'))
    ap.add_argument('--host', default='127.0.0.1')
    ap.add_argument('--port', type=int, default=6666)
    ap.add_argument('--resume', action='store_true')
    ap.add_argument('--dvi', action='store_true', help='switch hdmi_fb to plain DVI (AUD_CTRL = 0x30): '
                    'if the picture comes back, the monitor does not like the HDMI data islands')
    a = ap.parse_args()
    try:
        t = Tcl(a.host, a.port)
    except OSError as e:
        sys.exit('cannot reach OpenOCD at %s:%d (%s) - start the Debug session in the IDE first' % (a.host, a.port, e))
    print('OpenOCD', t('version'))
    print('target state before:', t('targets'))
    text, syms = elf_info(a.elf) if os.path.exists(a.elf) else (None, [])
    print('elf:', a.elf, '%d bytes, %d functions' % (os.path.getsize(a.elf), len(syms)) if syms else '(not found)')
    print('halt:', t('halt') or 'ok')
    pcs = []
    for k in range(3):                                   # sample the PC a few times
        pc = reg(t, 'pc')
        pcs.append(pc)
        if k < 2:
            t('resume'); time.sleep(0.05); t('halt')
    for pc in pcs:
        print('  pc = 0x%08x  %s' % (pc or 0, func_at(syms, pc or 0)))
    for r in ('ra', 'sp', 'mcause', 'mepc', 'mtval'):
        v = reg(t, r)
        if v is not None:
            print('  %-6s = 0x%08x %s' % (r, v, func_at(syms, v) if r in ('ra', 'mepc') else ''))
    if text:
        addr, blob = text
        n = min(len(blob) // 4, 1024)
        got = mdw(t, addr, n)
        exp = list(struct.unpack_from('<%dI' % n, blob))
        bad = [k for k in range(n) if got[k] != exp[k]]
        print('code in DDR vs elf .text (0x%x, %d words): %s' % (addr, n,
              'MATCH' if not bad else '%d DIFFERENT, first at 0x%x: %08x vs %08x' % (len(bad), addr + 4 * bad[0], got[bad[0]], exp[bad[0]])))
    try:
        h = mdw(t, 0xF8100000, 14)
        f0 = h[4]
        t('resume'); time.sleep(0.2); t('halt')
        h2 = mdw(t, 0xF8100000, 14)
        print('hdmi_fb: ID %08x CTRL %x FB_ADDR %08x FB_CUR %08x STATUS %x RES %08x UF %d' %
              (h[7], h[0], h[1], h[2], h[3], h[5], h[6]))
        print('         frames %d -> %d in 0.2 s (%s)' % (f0, h2[4], 'running' if h2[4] != f0 else 'NOT counting: pixel clock / timing stopped'))
        print('         AUD_CTRL %x LEVEL %d SENT %d -> %d UF %d CAPS %x' % (h[8], h[9], h[10], h2[10], h[11], h[13]))
    except Exception as e:
        print('hdmi_fb read failed:', e)
    try:
        b = mdw(t, 0xF8101000, 4)
        print('blit: ID %08x STATUS %08x CTRL %08x CAPS %08x' % tuple(b))
        st = b[1]
        print('      STATUS bits: busy %d list %d lerr %d band %d bovf %d berr %d' %
              (st & 1, st >> 3 & 1, st >> 4 & 1, st >> 5 & 1, st >> 6 & 1, st >> 7 & 1))
        regs = [('LADDR', 0x38), ('LEND', 0x3C), ('PUSHED', 0x50), ('LIMIT', 0x54), ('BUSY', 0x40),
                ('CMDS', 0x44), ('RB', 0x48), ('WB', 0x4C), ('WIDE', 0x84), ('XWBEAT', 0x8C),
                ('XRBEAT', 0x90), ('BAND_GEO', 0x94), ('BIN', 0x9C), ('FRAMES', 0xA0), ('P1', 0xA4),
                ('FLUSH', 0xA8), ('KBEAT', 0xAC), ('RECS', 0xB0), ('BAND', 0xB4), ('KCLK', 0xB8),
                ('CHIT', 0x5C), ('CMISS', 0x60), ('ARSTALL', 0x70), ('AWSTALL', 0x6C), ('WSTALL', 0x64)]
        v1 = [mdw(t, 0xF8101000 + o, 1)[0] for _, o in regs]
        t('resume'); time.sleep(0.2); t('halt')
        v2 = [mdw(t, 0xF8101000 + o, 1)[0] for _, o in regs]
        print('      ' + '  '.join('%s %x%s' % (n, a, '' if a == b2 else '->%x' % b2)
                                 for (n, _), a, b2 in zip(regs, v1, v2)))
    except Exception as e:
        print('blit read failed:', e)
    try:
        if mdw(t, 0xF8101000 + 0xBC, 1)[0] & 2:          # CAPS2[1]: DBG0..3 (0xC0..0xCC)
            for k in range(3):
                d0, d1, d2, d3 = mdw(t, 0xF8101000 + 0xC0, 4)
                print('blit DBG %d: %08x %08x %08x %08x' % (k, d0, d1, d2, d3))
                print('   fetch: rd_words %d rd_pend %d rf_res %d arvalid %d rd_room %d rf_ok %d bd_full %d' %
                      (d0 >> 21, d0 >> 14 & 0x7F, d0 >> 4 & 0x3FF, d0 >> 3 & 1, d0 >> 2 & 1, d0 >> 1 & 1, d0 & 1))
                print('   router: ax_out %d bd_count %d lf_inflight %d t_cnt %d bd_v %d rf_v %d t_fill %d t_hit %d' %
                      (d1 >> 27, d1 >> 16 & 0xFF, d1 >> 8 & 0xFF, d1 >> 1 & 0x7F, d1 & 1, d3 >> 9 & 1, d3 >> 8 & 1, d3 >> 7 & 1))
                print('   K: kf_iss %d kf_done %d head waits %d (ok %d) kq_count %d kq_valid %d kc_v %d kc_hz %d kc_last %d k1/2/3 %d%d%d' %
                      (d2 >> 24, d2 >> 16 & 0xFF, d2 >> 8 & 0xFF, d2 >> 1 & 1, d2 >> 4 & 0xF, d2 >> 3 & 1, d2 >> 2 & 1, d2 & 1,
                       d3 >> 6 & 1, d3 >> 13 & 1, d3 >> 12 & 1, d3 >> 11 & 1))
                print('   K walker: kw_v %d kb_v %d kb_res %d kb_hit %d kb_ev %d kb_rl %d kb_line %d kq_room %d kc_quiet %d'
                      ' c_qok %d c_vq %d kb_fill_raw %d' %
                      tuple(d3 >> b & 1 for b in (31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 5, 21)))
                print('   list: l_len_v %d l_urgent %d  f_state %d bc_state %d x_mode %d' %
                      (d3 >> 20 & 1, d3 >> 19 & 1, d3 >> 17 & 3, d3 >> 14 & 7, d3 >> 10 & 1))
                time.sleep(0.1)
        else:
            print('blit: no DBG registers in this bitstream (CAPS2[1] = 0)')
    except Exception as e:
        print('blit DBG read failed:', e)
    try:
        u = mdw(t, 0xF8010000 + 4, 3)                     # STATUS, CLOCK_DIVIDER, FRAME_CONFIG
        print('uart: STATUS %08x (tx free %d, rx used %d) CLOCK_DIVIDER %d FRAME %08x -> %.0f baud at 200 MHz'
              ' (rx sampling 8 / bit)' % (u[0], u[0] >> 16 & 0xFF, u[0] >> 24, u[1], u[2],
                                          200e6 / ((u[1] + 1) * 8) if u[1] else 0))
    except Exception as e:
        print('uart read failed:', e)
    try:
        m = mdw(t, 0xF8102000, 12)
        print('mixer: ID %08x CTRL %x MVOL %d TARGET %d ACTIVE %04x SAMPLES %d MISSES %d FETCHES %d CAPS %08x' %
              (m[0], m[1], m[2], m[3], m[6], m[7], m[8], m[9], m[11]))
    except Exception as e:
        print('mixer read failed:', e)
    try:
        bank = mdw(t, 0x02000000, 4)
        print('sound bank header at 0x02000000: %08x %08x %08x %08x %s' % (tuple(bank) + ('(SBK1)' if bank[0] == 0x314B4253 else '(none)',)))
    except Exception as e:
        print('DDR 0x02000000 read failed:', e)
    if a.dvi:
        t('mww 0xF8100020 0x30')
        print('AUD_CTRL <- 0x30 (plain DVI, no data islands); read back %x' % mdw(t, 0xF8100020, 1)[0])
    if a.resume:
        print('resume:', t('resume') or 'ok')
    else:
        print('CPU left halted (run with --resume to continue)')


if __name__ == '__main__':
    main()
