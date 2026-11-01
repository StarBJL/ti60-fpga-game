#!/usr/bin/env python3
"""
load_sound.py - 把声音库 sound/sound_bank.bin 经 JTAG 写进 DDR 0x02000000（2026-10-20）。

  python tools/load_sound.py                  # 用默认 JTAG 速度
  python tools/load_sound.py --speed 4000     # 临时把 JTAG 提到 4 MHz 试试（结束后恢复原速度）
  python tools/load_sound.py --verify         # 写完读回比较（时间翻倍）

用法：IDE 里照常 Debug 下载 / 运行程序（OpenOCD 在跑，TCL 端口 6666），然后运行本脚本：
它让 CPU 暂停、写声音库、再继续运行；游戏一秒内自动认出声音库（串口打一行 "sound bank found"）。
只要不断电，之后重新下载程序都不需要再运行本脚本（程序在 15 MB 以下，不会碰 0x02000000）。
脚本会打出用时和 KB/s，用 --speed 可以比较不同 JTAG 速度（不稳定就回到默认）。

没有开 IDE 时也可以单独起 OpenOCD（在 embedded_sw/soc 目录）：
  openocd -c "set bsp_loc {<embedded_sw/soc 的完整路径>}" -f bsp/efinix/EfxSapphireSoc/openocd/ftdi_ti.cfg \
          -f bsp/efinix/EfxSapphireSoc/openocd/debug_ti.cfg
"""
import os, sys, time, socket, argparse

HERE = os.path.dirname(os.path.abspath(__file__))
GAME = os.path.dirname(HERE)


class Tcl:
    def __init__(s, host, port):
        s.s = socket.create_connection((host, port), timeout=5)
        s.s.settimeout(None)

    def __call__(s, cmd):
        s.s.sendall(cmd.encode() + b'\x1a')
        buf = b''
        while not buf.endswith(b'\x1a'):
            d = s.s.recv(65536)
            if not d:
                raise ConnectionError('OpenOCD closed the connection')
            buf += d
        return buf[:-1].decode(errors='replace').strip()


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--bank', default=os.path.join(GAME, 'sound', 'sound_bank.bin'))
    ap.add_argument('--addr', default='0x02000000')
    ap.add_argument('--host', default='127.0.0.1')
    ap.add_argument('--port', type=int, default=6666)
    ap.add_argument('--speed', type=int, default=0, help='JTAG kHz for this load only (default: keep)')
    ap.add_argument('--verify', action='store_true')
    a = ap.parse_args()
    path = os.path.abspath(a.bank)
    if not os.path.exists(path):
        sys.exit('no %s - run python tools/wav2bank.py first' % path)
    data = open(path, 'rb').read(16)
    if data[:4] != b'SBK1':
        sys.exit('%s is not a sound bank (no SBK1 header)' % path)
    size = os.path.getsize(path)
    try:
        t = Tcl(a.host, a.port)
    except OSError as e:
        sys.exit('cannot reach OpenOCD at %s:%d (%s) - start the Debug session in the IDE first' % (a.host, a.port, e))
    tpath = path.replace('\\', '/')
    old = t('adapter speed')
    print('OpenOCD: %s; JTAG speed now %s' % (t('version'), old))
    if a.speed:
        print('set JTAG speed:', t('adapter speed %d' % a.speed))
    try:
        print('halt:', t('halt') or 'ok')
        print('loading %s (%.1f KB) to %s ...' % (path, size / 1024, a.addr))
        t0 = time.time()
        r = t('load_image {%s} %s bin' % (tpath, a.addr))
        dt = time.time() - t0
        print(r)
        print('%.1f s, %.1f KB/s' % (dt, size / 1024 / dt if dt > 0 else 0))
        if 'error' in r.lower() or 'fail' in r.lower():
            print('LOAD FAILED')
        elif a.verify:
            t0 = time.time()
            r = t('verify_image {%s} %s bin' % (tpath, a.addr))
            print(r, '(%.1f s)' % (time.time() - t0))
    finally:
        if a.speed:
            khz = ''.join(ch for ch in old if ch.isdigit()) or '800'
            print('restore JTAG speed:', t('adapter speed %s' % khz))
        print('resume:', t('resume') or 'ok')


if __name__ == '__main__':
    main()
