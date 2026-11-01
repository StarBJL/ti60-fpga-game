#!/usr/bin/env python3
"""
wav2bank.py - WAV files -> sound bank for the HDMI audio mixer (DDR 0x0200_0000).

  python tools/wav2bank.py                       # reads sound/sounds.txt
  python tools/wav2bank.py --list my.txt --out sound/sound_bank.bin
  python tools/wav2bank.py --demo                # writes placeholder WAVs into sound/demo/ first

sound/sounds.txt, one sound per line (paths relative to the list file):

    # name         file                 options
    shot           se/plst00.wav        rate=24000
    graze          se/graze.wav
    bgm_stage1     bgm/stage1.wav       rate=32000 loop=auto
    bgm_boss1      bgm/boss1.wav        loop=12.345s

  name     <= 15 characters; the game looks sounds up by these names
           (src/th_sound.h lists the names it uses; others can be played with aud_find())
  options  mono | stereo   default: mono for effects, stereo kept for names starting with bgm
           8bit            8-bit samples (half the size, audible hiss; fine for noise-like effects)
           rate=N          resample to N Hz (smaller); default = keep the file's rate (the mixer
                           resamples to 48 kHz while playing, any rate up to 96 kHz works)
           gain=X          multiply (e.g. 0.5); also gain=-6dB
           loop=auto       loop point from the WAV 'smpl' chunk, else loop the whole file
           loop=none       one-shot (default for effects)
           loop=N | loop=Xs  loop start at sample frame N / at X seconds (end = end of file)
           trim            cut leading silence (effects start without delay)
           end=Xs          cut the file at X seconds

Output:
  sound/sound_bank.bin  the bank: header 'SBK1', count, bytes, 0; 32-byte entries
                        {offset, frames, loop (0xFFFFFFFF = none), rate | fmt << 24, name[16]};
                        sample data 32-byte aligned, little endian
  sound/sound_bank.S    .incbin of the .bin into section .sound (game.ld puts it at 0x02000000);
                        makefile SOUND=1 links it into game.elf
  a table with sizes and the estimated JTAG download time

WAV input: PCM 8/16/24/32-bit, IEEE float 32/64, mono or stereo (more channels: first two).
Needs only the Python standard library; resampling uses scipy (best) or numpy if installed,
else a slower pure-Python linear interpolation.
"""
import os, sys, struct, math, argparse

FMT_S8, FMT_S16, FMT_S16ST = 0, 1, 2
MAGIC = 0x314B4253


# ------------------------------------------------------------------ WAV reading
def read_wav(path):
    """returns (rate, channels [list of lists of float -1..1], loop_start or None)"""
    data = open(path, 'rb').read()
    if data[:4] != b'RIFF' or data[8:12] != b'WAVE':
        raise ValueError('%s: not a RIFF/WAVE file' % path)
    pos, fmt, raw, loop = 12, None, None, None
    while pos + 8 <= len(data):
        cid, size = data[pos:pos + 4], struct.unpack_from('<I', data, pos + 4)[0]
        body = data[pos + 8:pos + 8 + size]
        if cid == b'fmt ':
            tag, nch, rate, _, align, bits = struct.unpack_from('<HHIIHH', body)
            if tag == 0xFFFE and len(body) >= 26:          # WAVE_FORMAT_EXTENSIBLE
                tag = struct.unpack_from('<H', body, 24)[0]
            fmt = (tag, nch, rate, align, bits)
        elif cid == b'data':
            raw = body
        elif cid == b'smpl' and len(body) >= 36:
            nloops = struct.unpack_from('<I', body, 28)[0]
            if nloops and len(body) >= 36 + 24:
                loop = struct.unpack_from('<I', body, 36 + 8)[0]       # start (sample frames)
        pos += 8 + size + (size & 1)
    if fmt is None or raw is None:
        raise ValueError('%s: no fmt / data chunk' % path)
    tag, nch, rate, align, bits = fmt
    n = len(raw) // align
    bps = bits // 8
    chans = [[] for _ in range(min(nch, 2))]
    if tag == 1 and bits == 16:
        vals = struct.unpack_from('<%dh' % (n * nch), raw)
        sc = 1.0 / 32768
    elif tag == 1 and bits == 8:
        vals = [b - 128 for b in raw[:n * nch]]
        sc = 1.0 / 128
    elif tag == 1 and bits == 24:
        vals = []
        for i in range(n * nch):
            b = raw[3 * i:3 * i + 3]
            v = b[0] | b[1] << 8 | b[2] << 16
            vals.append(v - (1 << 24) if v & 0x800000 else v)
        sc = 1.0 / 8388608
    elif tag == 1 and bits == 32:
        vals = struct.unpack_from('<%di' % (n * nch), raw)
        sc = 1.0 / 2147483648
    elif tag == 3 and bits in (32, 64):
        vals = struct.unpack_from('<%d%s' % (n * nch, 'f' if bits == 32 else 'd'), raw)
        sc = 1.0
    else:
        raise ValueError('%s: unsupported WAV format tag %d, %d bits' % (path, tag, bits))
    for c in range(len(chans)):
        chans[c] = [v * sc for v in vals[c::nch]]
    return rate, chans, loop


# ------------------------------------------------------------------ resampling
def resample(chans, src, dst):
    if src == dst:
        return chans
    try:
        from scipy.signal import resample_poly
        g = math.gcd(src, dst)
        return [list(resample_poly(c, dst // g, src // g)) for c in chans]
    except ImportError:
        pass
    try:
        import numpy as np
        out = []
        # windowed-sinc interpolation (Kaiser, 48 taps; cut-off 0.9 x the lower Nyquist):
        # no scipy needed, no aliasing of the noise drums / hats when going down to 22 kHz
        half = 24
        fc = 0.9 * min(1.0, dst / src)                     # relative to the source Nyquist
        for c in chans:
            x = np.asarray(c, dtype=float)
            n = int(len(x) * dst / src)
            t = np.arange(n) * (src / dst)
            base = np.floor(t).astype(np.int64)
            y = np.zeros(n)
            xp = np.concatenate([np.zeros(half), x, np.zeros(half + 1)])
            for k in range(-half + 1, half + 1):
                d = t - (base + k)                              # distance in source samples
                r = np.clip(d / half, -1.0, 1.0)
                w = np.i0(8.0 * np.sqrt(1.0 - r * r)) / np.i0(8.0)    # continuous Kaiser window
                h = fc * np.sinc(fc * d) * w
                y += h * xp[base + k + half]
            out.append(list(y))
        return out
    except ImportError:
        pass
    out = []
    for c in chans:                                            # pure Python: linear
        n = int(len(c) * dst / src)
        r = []
        for i in range(n):
            p = i * src / dst
            j = int(p)
            f = p - j
            a = c[j]
            b = c[j + 1] if j + 1 < len(c) else a
            r.append(a + (b - a) * f)
        out.append(r)
    return out


# ------------------------------------------------------------------ list parsing
def parse_list(path):
    items = []
    base = os.path.dirname(os.path.abspath(path))
    for ln, line in enumerate(open(path, encoding='utf-8'), 1):
        line = line.split('#', 1)[0].strip()
        if not line:
            continue
        parts = line.split()
        if len(parts) < 2:
            raise ValueError('%s:%d: need "name file [options]"' % (path, ln))
        name, f, opts = parts[0], parts[1], parts[2:]
        if len(name.encode()) > 15:
            raise ValueError('%s:%d: name "%s" longer than 15 characters' % (path, ln, name))
        o = {}
        for op in opts:
            k, _, v = op.partition('=')
            o[k.lower()] = v
        items.append((name, os.path.join(base, f), o, ln))
    return items


def seconds_or_frames(v, rate):
    v = v.strip().lower()
    if v.endswith('s'):
        return int(round(float(v[:-1]) * rate))
    return int(v)


def convert(name, path, o, verbose=True):
    rate, ch, loop = read_wav(path)
    is_bgm = name.lower().startswith('bgm')
    stereo = ('stereo' in o) or (is_bgm and 'mono' not in o)
    if not stereo or len(ch) == 1:
        if len(ch) == 2:
            ch = [[(a + b) * 0.5 for a, b in zip(ch[0], ch[1])]]
        stereo = False
    if 'end' in o:
        e = seconds_or_frames(o['end'], rate)
        ch = [c[:e] for c in ch]
    if 'trim' in o:
        k = 0
        n = len(ch[0])
        while k < n and all(abs(c[k]) < 1.0 / 512 for c in ch):
            k += 1
        ch = [c[k:] for c in ch]
        if loop is not None:
            loop = max(0, loop - k)
    gain = 1.0
    if 'gain' in o:
        g = o['gain'].lower()
        gain = 10 ** (float(g[:-2]) / 20) if g.endswith('db') else float(g)
    # loop
    lp = o.get('loop', 'auto' if is_bgm else 'none').lower()
    if lp == 'none':
        loop = None
    elif lp == 'auto':
        loop = loop if loop is not None and loop < len(ch[0]) else 0
    else:
        loop = seconds_or_frames(lp, rate)
    if 'rate' in o:
        nr = int(o['rate'])
        if loop is not None:
            loop = int(loop * nr / rate)
        ch = resample(ch, rate, nr)
        rate = nr
    if rate > 96000:
        raise ValueError('%s: %d Hz > 96 kHz, use rate=' % (name, rate))
    frames = len(ch[0])
    if frames == 0:
        raise ValueError('%s: empty' % name)
    if frames >= 1 << 24:
        raise ValueError('%s: %d frames >= 16M (cut it or lower the rate)' % (name, frames))
    eight = '8bit' in o and not stereo
    fmt = FMT_S16ST if stereo else (FMT_S8 if eight else FMT_S16)
    clip = 0
    out = bytearray()
    if eight:
        for v in ch[0]:
            x = int(round(v * gain * 127))
            if x > 127 or x < -128:
                clip += 1
                x = max(-128, min(127, x))
            out.append(x & 0xFF)
    else:
        vals = []
        for i in range(frames):
            for c in ch:
                x = int(round(c[i] * gain * 32767))
                if x > 32767 or x < -32768:
                    clip += 1
                    x = max(-32768, min(32767, x))
                vals.append(x)
        out += struct.pack('<%dh' % len(vals), *vals)
    if loop is not None and loop >= frames:
        loop = 0
    return dict(name=name, rate=rate, fmt=fmt, frames=frames, loop=loop, data=bytes(out), clip=clip)


def build_bank(sounds):
    hdr_len = 16 + 32 * len(sounds)
    off = (hdr_len + 31) & ~31
    table, blob = b'', b''
    for s in sounds:
        s['off'] = off
        pad = (-len(s['data'])) % 32
        blob += s['data'] + b'\0' * pad
        name = s['name'].encode()[:15]
        table += struct.pack('<IIII16s', off, s['frames'],
                             0xFFFFFFFF if s['loop'] is None else s['loop'],
                             s['rate'] | (s['fmt'] << 24), name)
        off += len(s['data']) + pad
    head = struct.pack('<IIII', MAGIC, len(sounds), off, 0) + table
    head += b'\0' * (((hdr_len + 31) & ~31) - len(head))
    return head + blob


# ------------------------------------------------------------------ placeholder sounds
def write_wav(path, rate, samples, stereo=False):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    n = len(samples) // (2 if stereo else 1)
    nch = 2 if stereo else 1
    data = struct.pack('<%dh' % len(samples), *[max(-32768, min(32767, int(v * 32767))) for v in samples])
    with open(path, 'wb') as f:
        f.write(b'RIFF' + struct.pack('<I', 36 + len(data)) + b'WAVE')
        f.write(b'fmt ' + struct.pack('<IHHIIHH', 16, 1, nch, rate, rate * 2 * nch, 2 * nch, 16))
        f.write(b'data' + struct.pack('<I', len(data)) + data)


def demo_sounds(folder):
    """simple synthesized effects + a short looping tune, so the board makes sound before real WAVs exist"""
    import random
    rnd = random.Random(1)
    R = 24000

    def env(n, a=0.002, d=None):
        d = d or n / R
        return [min(1.0, i / (a * R + 1)) * max(0.0, 1 - i / (d * R)) for i in range(n)]

    def tone(f0, f1, dur, wave='sq', vol=0.5, noise=0.0):
        n = int(dur * R)
        e = env(n)
        out, ph = [], 0.0
        for i in range(n):
            f = f0 + (f1 - f0) * i / n
            ph += f / R
            if wave == 'sq':
                v = 1.0 if (ph % 1) < 0.5 else -1.0
            elif wave == 'tri':
                v = 4 * abs((ph % 1) - 0.5) - 1
            else:
                v = math.sin(2 * math.pi * ph)
            v = v * (1 - noise) + (rnd.random() * 2 - 1) * noise
            out.append(v * e[i] * vol)
        return out

    fx = {
        'shot': tone(1800, 900, 0.04, 'sq', 0.25, 0.5),
        'enemy_fire': tone(700, 500, 0.05, 'tri', 0.35),
        'hit': tone(300, 200, 0.03, 'sq', 0.3, 0.7),
        'kill': tone(400, 60, 0.25, 'sq', 0.5, 0.8),
        'graze': tone(2400, 3000, 0.04, 'sin', 0.4),
        'item': tone(1200, 1600, 0.05, 'sin', 0.35),
        'powerup': tone(500, 1500, 0.3, 'sq', 0.35),
        'extend': tone(800, 1600, 0.12, 'sq', 0.4) + tone(1000, 2000, 0.25, 'sq', 0.4),
        'bomb_get': tone(600, 1200, 0.2, 'tri', 0.45),
        'bomb': tone(200, 40, 1.2, 'sq', 0.6, 0.9),
        'death': tone(900, 80, 0.7, 'sq', 0.6, 0.5),
        'spell': tone(300, 900, 0.5, 'tri', 0.5, 0.2),
        'capture': tone(800, 800, 0.1, 'sin', 0.5) + tone(1200, 1200, 0.3, 'sin', 0.5),
        'phase': tone(500, 100, 0.5, 'sq', 0.5, 0.7),
        'boss_dead': tone(300, 30, 2.0, 'sq', 0.7, 0.9),
        'warning': (tone(880, 880, 0.15, 'sq', 0.4) + [0] * 2400) * 3,
        'timeout': tone(1500, 1500, 0.05, 'sin', 0.5),
        'select': tone(1000, 1000, 0.03, 'sq', 0.3),
        'ok': tone(1200, 1800, 0.08, 'sq', 0.35),
        'pause': tone(600, 400, 0.12, 'tri', 0.4),
        'clear': tone(523, 523, 0.15, 'sq', 0.4) + tone(659, 659, 0.15, 'sq', 0.4) + tone(784, 784, 0.4, 'sq', 0.4),
        'over': tone(392, 392, 0.3, 'tri', 0.5) + tone(330, 330, 0.3, 'tri', 0.5) + tone(262, 262, 0.6, 'tri', 0.5),
        'laser': tone(200, 1200, 0.4, 'sin', 0.4, 0.3),
    }
    lines = []
    for k, v in fx.items():
        write_wav(os.path.join(folder, k + '.wav'), R, v)
        lines.append('%-14s demo/%s.wav' % (k, k))
    # a 4-bar arpeggio loop per BGM name (stereo, different keys)
    def tune(root, tempo=140):
        notes = [0, 4, 7, 12, 7, 4, 0, 4, 5, 9, 12, 17, 12, 9, 5, 9,
                 7, 11, 14, 19, 14, 11, 7, 11, 0, 4, 7, 12, 16, 12, 7, 4]
        step = 60.0 / tempo / 4
        L, Rr = [], []
        for k, nt in enumerate(notes):
            f = root * 2 ** (nt / 12)
            a = tone(f, f, step, 'sq', 0.18)
            b = tone(f / 2, f / 2, step, 'tri', 0.3)
            for x, y in zip(a, b):
                L.append(x * 0.8 + y)
                Rr.append(x + y * 0.8)
        return [v for p in zip(L, Rr) for v in p]
    for k, root in [('bgm_title', 220), ('bgm_stage1', 262), ('bgm_boss1', 294), ('bgm_stage2', 247), ('bgm_boss2', 330)]:
        write_wav(os.path.join(folder, k + '.wav'), R, tune(root), stereo=True)
        lines.append('%-14s demo/%s.wav   loop=auto' % (k, k))
    return lines


# ------------------------------------------------------------------ main
def main():
    here = os.path.dirname(os.path.abspath(__file__))
    game = os.path.dirname(here)
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--list', default=os.path.join(game, 'sound', 'sounds.txt'))
    ap.add_argument('--out', default=os.path.join(game, 'sound', 'sound_bank.bin'))
    ap.add_argument('--demo', action='store_true', help='write placeholder WAVs + sound/demo.txt and use that list')
    ap.add_argument('--jtag-kbps', type=float, default=200.0, help='JTAG download speed for the time estimate')
    a = ap.parse_args()
    if a.demo:
        folder = os.path.join(os.path.dirname(a.out), 'demo')
        lines = demo_sounds(folder)
        a.list = os.path.join(os.path.dirname(a.out), 'demo.txt')
        with open(a.list, 'w', encoding='utf-8') as f:
            f.write('# placeholder sounds written by wav2bank.py --demo\n' + '\n'.join(lines) + '\n')
        print('placeholder WAVs in %s, list %s' % (folder, a.list))
    items = parse_list(a.list)
    if not items:
        sys.exit('no sounds in ' + a.list)
    if len(items) > 256:
        sys.exit('at most 256 sounds')
    names = set()
    sounds = []
    for name, path, o, ln in items:
        if name in names:
            sys.exit('%s:%d: duplicate name %s' % (a.list, ln, name))
        names.add(name)
        try:
            sounds.append(convert(name, path, o))
        except (OSError, ValueError) as e:
            sys.exit('%s:%d: %s' % (a.list, ln, e))
    bank = build_bank(sounds)
    if len(bank) > 16 * 1024 * 1024:
        sys.exit('bank is %.1f MB, more than the 16 MB at 0x02000000..0x02FFFFFF' % (len(bank) / 1048576))
    os.makedirs(os.path.dirname(os.path.abspath(a.out)), exist_ok=True)
    open(a.out, 'wb').write(bank)
    rel = os.path.relpath(a.out, game).replace('\\', '/')
    sfile = os.path.splitext(a.out)[0] + '.S'
    with open(sfile, 'w', newline='\n') as f:
        f.write('/* generated by tools/wav2bank.py - sound bank, linked at 0x02000000 (game.ld .sound) */\n'
                '    .section .sound,"a"\n    .balign 64\n    .global sound_bank\nsound_bank:\n'
                '    .incbin "%s"\n' % rel)
    fn = ['s8', 's16', 's16 stereo']
    print('%-15s %7s %-10s %9s %8s %8s %s' % ('name', 'rate', 'format', 'frames', 'seconds', 'KB', 'loop'))
    for s in sounds:
        print('%-15s %7d %-10s %9d %8.2f %8.1f %s%s' % (
            s['name'], s['rate'], fn[s['fmt']], s['frames'], s['frames'] / s['rate'], len(s['data']) / 1024,
            '-' if s['loop'] is None else s['loop'], '  CLIPPED %d' % s['clip'] if s['clip'] else ''))
    print('bank %s: %d sounds, %.1f KB (%.2f MB of 16); JTAG download ~%.0f s at %.0f KB/s'
          % (a.out, len(sounds), len(bank) / 1024, len(bank) / 1048576, len(bank) / 1024 / a.jtag_kbps, a.jtag_kbps))
    print('wrote %s (make SOUND=1 links it into game.elf)' % sfile)


if __name__ == '__main__':
    main()
