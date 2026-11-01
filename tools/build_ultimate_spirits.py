#!/usr/bin/env python3
"""Pack the two transparent PNG masters for the existing RGB565+A8 renderer."""
from pathlib import Path
import ctypes, ctypes.util, hashlib, json
import numpy as np
from PIL import Image

R = Path(__file__).resolve().parents[1]
A = R / 'assets/v6/ultimate_spirits_20261101'
S = R / 'par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src'
lib = ctypes.CDLL(ctypes.util.find_library('lz4'))
lib.LZ4_compressBound.argtypes = [ctypes.c_int]
lib.LZ4_compress_HC.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
lib.LZ4_decompress_safe.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
c = ['#include <stdint.h>', '#ifdef __riscv',
     '#define ASSET __attribute__((section(".bg_assets"),aligned(64)))',
     '#else', '#define ASSET', '#endif']
entries = []
for name, symbol, wh in [('blue_sabertooth', 'ultimate_tiger', (640, 360)),
                         ('pink_phoenix', 'ultimate_phoenix', (768, 432))]:
    src = A / (name + '_source.png')
    im = Image.open(src)
    assert im.mode == 'RGBA' and im.getextrema()[3][0] == 0, 'Transparent master required'
    # Representation conversion only: keep the generated alpha and full framing.
    im = im.resize(wh, Image.Resampling.LANCZOS)
    im.save(A / (name + '_runtime.png'))
    arr = np.asarray(im)
    rgb = arr[:, :, :3].astype(np.uint16)
    pix = (((rgb[:, :, 0] >> 3) << 11) | ((rgb[:, :, 1] >> 2) << 5) |
           (rgb[:, :, 2] >> 3)).astype('<u2')
    raw = pix.tobytes() + arr[:, :, 3].tobytes()
    buf = ctypes.create_string_buffer(lib.LZ4_compressBound(len(raw)))
    size = lib.LZ4_compress_HC(raw, buf, len(raw), len(buf), 12)
    assert size > 0
    packed = buf.raw[:size]
    decoded = ctypes.create_string_buffer(len(raw))
    assert lib.LZ4_decompress_safe(packed, decoded, size, len(raw)) == len(raw)
    assert decoded.raw == raw
    (A / (name + '.raw')).write_bytes(raw)
    (A / (name + '.lz4')).write_bytes(packed)
    c += [f'const unsigned {symbol}_size={size};', f'const uint8_t {symbol}_packed[] ASSET={{']
    c += [','.join(map(str, packed[i:i+32])) + ',' for i in range(0, size, 32)]
    c += ['};']
    entries.append(dict(name=name, symbol=symbol, dimensions=wh, raw_bytes=len(raw),
                        packed_bytes=size, raw_sha256=hashlib.sha256(raw).hexdigest(),
                        source_sha256=hashlib.sha256(src.read_bytes()).hexdigest(),
                        transparent_pixels=int((arr[:, :, 3] == 0).sum())))
(S / 'ultimate_spirits_data.c').write_text('\n'.join(c) + '\n')
(A / 'manifest.json').write_text(json.dumps(dict(generator='built-in image_gen',
    format='RGB565 little-endian followed by A8, LZ4 block', entries=entries), indent=2) + '\n')
print(json.dumps(entries, indent=2))
