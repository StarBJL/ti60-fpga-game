#!/usr/bin/env python3
"""
pc_pad.py - 把 PC 键盘的"按住"状态经串口发给板子，同时显示板子的串口输出
（代替串口终端：一个串口只能被一个程序打开）。协议见 src/pad.h。

    安装一次：  python -m pip install pyserial pygame
    运行：      python tools/pc_pad.py COM5          （端口号看设备管理器）
                python tools/pc_pad.py --list        （列出串口）
                python tools/pc_pad.py COM5 --log run.txt   （串口输出同时存文件）

用法：让弹出的小窗口保持在前台（它要拿键盘焦点），游戏键：
    方向键 / WASD   移动               Z / 空格   射击（菜单里确定）
    Shift           低速（判定点）     X          符卡（菜单里取消）
    Esc             暂停               Enter      开始
    C / Ctrl        跳过 / 备用
其余字母、数字、符号键照旧作为字符发给板子（v 8 5 4 g n l 1 2 3 p r m b f e t h ... 调试键）。
注意：在 Windows 上 "python3" 可能是应用商店的空壳，请用 "python" 或 "py"。
"""
import argparse, sys, threading, time

try:
    import serial
    import serial.tools.list_ports
except ImportError:
    sys.exit("need pyserial:  python -m pip install pyserial")
try:
    import pygame
except ImportError:
    sys.exit("need pygame:  python -m pip install pygame")

# state bits, same as src/pad.h
PAD_UP, PAD_DOWN, PAD_LEFT, PAD_RIGHT = 1 << 0, 1 << 1, 1 << 2, 1 << 3
PAD_SHOT, PAD_BOMB, PAD_SLOW, PAD_PAUSE = 1 << 4, 1 << 5, 1 << 6, 1 << 7
PAD_START, PAD_SKIP = 1 << 8, 1 << 9
NAMES = [(PAD_UP, "UP"), (PAD_DOWN, "DOWN"), (PAD_LEFT, "LEFT"), (PAD_RIGHT, "RIGHT"),
         (PAD_SHOT, "SHOT"), (PAD_BOMB, "BOMB"), (PAD_SLOW, "SLOW"), (PAD_PAUSE, "PAUSE"),
         (PAD_START, "START"), (PAD_SKIP, "SKIP")]

KEYMAP = {
    pygame.K_UP: PAD_UP, pygame.K_w: PAD_UP,
    pygame.K_DOWN: PAD_DOWN, pygame.K_s: PAD_DOWN,
    pygame.K_LEFT: PAD_LEFT, pygame.K_a: PAD_LEFT,
    pygame.K_RIGHT: PAD_RIGHT, pygame.K_d: PAD_RIGHT,
    pygame.K_z: PAD_SHOT, pygame.K_SPACE: PAD_SHOT,
    pygame.K_x: PAD_BOMB,
    pygame.K_LSHIFT: PAD_SLOW, pygame.K_RSHIFT: PAD_SLOW,
    pygame.K_ESCAPE: PAD_PAUSE,
    pygame.K_RETURN: PAD_START, pygame.K_KP_ENTER: PAD_START,
    pygame.K_c: PAD_SKIP, pygame.K_LCTRL: PAD_SKIP, pygame.K_RCTRL: PAD_SKIP,
}


def packet(state):
    b0, b1 = state & 0x7F, (state >> 7) & 0x7F
    return bytes([0xA5, b0, b1, (b0 ^ b1 ^ 0x5A) & 0x7F])


def reader(ser, log, stop):
    """board UART output -> console (+ log file)"""
    while not stop.is_set():
        try:
            data = ser.read(ser.in_waiting or 1)
        except serial.SerialException as e:
            print("\n[serial error: %s]" % e)
            stop.set()
            return
        if data:
            text = data.decode("utf-8", "replace")
            sys.stdout.write(text)
            sys.stdout.flush()
            if log:
                log.write(text)
                log.flush()


def pick_port(name):
    ports = list(serial.tools.list_ports.comports())
    if name:
        return name
    if len(ports) == 1:
        return ports[0].device
    print("serial ports:")
    for p in ports:
        print("  %-8s %s" % (p.device, p.description))
    sys.exit("give the port, e.g.  python tools/pc_pad.py COM5")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("port", nargs="?", help="serial port, e.g. COM5 or /dev/ttyUSB0")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--log", help="also write the board output to this file")
    ap.add_argument("--list", action="store_true", help="list serial ports and exit")
    ap.add_argument("--rate", type=int, default=60, help="state packets per second (default 60)")
    a = ap.parse_args()
    if a.list:
        for p in serial.tools.list_ports.comports():
            print("%-8s %s" % (p.device, p.description))
        return
    port = pick_port(a.port)
    ser = serial.Serial(port, a.baud, timeout=0.05, write_timeout=0.5)
    log = open(a.log, "a", encoding="utf-8") if a.log else None
    stop = threading.Event()
    th = threading.Thread(target=reader, args=(ser, log, stop), daemon=True)
    th.start()

    pygame.init()
    screen = pygame.display.set_mode((560, 210))
    pygame.display.set_caption("pc_pad -> %s (keep this window focused)" % port)
    font = pygame.font.SysFont("consolas,menlo,monospace", 16)
    clock = pygame.time.Clock()
    sent, chars, last_state = 0, 0, -1
    print("[pc_pad: %s %d baud; focus the small window to play]" % (port, a.baud))

    try:
        while not stop.is_set():
            for ev in pygame.event.get():
                if ev.type == pygame.QUIT:
                    stop.set()
                elif ev.type == pygame.KEYDOWN and ev.key not in KEYMAP:
                    ch = ev.unicode
                    if ch and len(ch) == 1 and 32 <= ord(ch) < 127:   # debug keys as plain characters
                        ser.write(ch.encode("ascii"))
                        chars += 1
            keys = pygame.key.get_pressed()
            state = 0
            for k, bit in KEYMAP.items():
                if keys[k]:
                    state |= bit
            ser.write(packet(state))               # every frame: the board times out after 300 ms
            sent += 1
            if state != last_state or sent % 15 == 0:
                last_state = state
                screen.fill((20, 22, 30))
                held = " ".join(n for bit, n in NAMES if state & bit) or "-"
                lines = ["port %s   packets %d   chars %d" % (port, sent, chars),
                         "held: " + held,
                         "arrows/WASD move  Z/space shot  X bomb  Shift slow",
                         "Esc pause  Enter start  C/Ctrl skip  other keys -> chars"]
                for i, t in enumerate(lines):
                    screen.blit(font.render(t, True, (220, 220, 230) if i != 1 else (255, 210, 90)),
                                (12, 12 + 36 * i))
                pygame.display.flip()
            clock.tick(a.rate)
    except KeyboardInterrupt:
        pass
    finally:
        try:
            ser.write(packet(0))
            time.sleep(0.05)
        except serial.SerialException:
            pass
        stop.set()
        pygame.quit()
        ser.close()
        if log:
            log.close()


if __name__ == "__main__":
    main()
