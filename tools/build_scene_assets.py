#!/usr/bin/env python3
"""Regenerate supplied PNGs as startup-decoded RGB565 LZ4 blocks.
Needs Pillow and liblz4 (or the Python lz4 package); firmware builds use the
already generated scene_assets_data.c and do not need these authoring tools.
"""
from pathlib import Path
import ctypes,ctypes.util,hashlib,json
from PIL import Image
root=Path(__file__).resolve().parents[1];assets=root/'assets'
libname=ctypes.util.find_library('lz4')
if libname:
 lib=ctypes.CDLL(libname);lib.LZ4_compressBound.argtypes=[ctypes.c_int];lib.LZ4_compressBound.restype=ctypes.c_int
 lib.LZ4_compress_HC.argtypes=[ctypes.c_void_p,ctypes.c_void_p,ctypes.c_int,ctypes.c_int,ctypes.c_int];lib.LZ4_compress_HC.restype=ctypes.c_int
 def compress(raw):
  out=ctypes.create_string_buffer(lib.LZ4_compressBound(len(raw)));n=lib.LZ4_compress_HC(raw,out,len(raw),len(out),12)
  if n<=0:raise RuntimeError('LZ4 compression failed')
  return out.raw[:n]
else:
 import lz4.block
 def compress(raw):return lz4.block.compress(raw,mode='high_compression',compression=12,store_size=False)
manifest={};c=['/* Generated RGB565 assets; LZ4 block format, decoded once at startup. */','#include <stdint.h>']
for name in ('gallery_dragons','carrot_arena'):
 source=assets/(name+'_source.png');im=Image.open(source).convert('RGB')
 if name=='gallery_dragons':
  canvas=Image.new('RGB',(1280,720),(15,24,31));im.thumbnail((1280,636),Image.Resampling.LANCZOS);canvas.paste(im,((1280-im.width)//2,40+(636-im.height)//2));im=canvas
 else:im=im.resize((1280,720),Image.Resampling.LANCZOS)
 im.save(assets/(name+'_720p.png'));raw=bytearray()
 for r,g,b in (im.get_flattened_data() if hasattr(im,'get_flattened_data') else im.getdata()):
  pixel=((r>>3)<<11)|((g>>2)<<5)|(b>>3);raw.extend((pixel&255,pixel>>8))
 packed=compress(bytes(raw));(assets/(name+'.rgb565.lz4')).write_bytes(packed)
 manifest[name]={'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),'raw_bytes':len(raw),'packed_bytes':len(packed),'raw_sha256':hashlib.sha256(raw).hexdigest(),'packed_sha256':hashlib.sha256(packed).hexdigest()}
 c.append('const uint8_t '+name+'_lz4[] __attribute__((aligned(64))) = {')
 c.extend(','.join('0x%02x'%v for v in packed[i:i+24])+',' for i in range(0,len(packed),24));c.extend(['};','const unsigned '+name+'_lz4_size=sizeof '+name+'_lz4;'])
(root/'par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/scene_assets_data.c').write_text('\n'.join(c)+'\n')
(assets/'manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2)+'\n')
print(json.dumps(manifest,indent=2))
