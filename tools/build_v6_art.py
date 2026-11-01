from pathlib import Path
from PIL import Image,ImageDraw
import numpy as np,ctypes,ctypes.util,json,hashlib
R=Path(__file__).resolve().parents[1];A=R/'assets/v6';S=R/'par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src'
im=Image.open(A/'ultimate_portraits_source.png').convert('RGBA');w,h=im.size;px=[];alpha=[];preview=Image.new('RGB',(3*180,2*180),(20,25,35))
for row in range(2):
 for col in range(3):
  t=im.crop((round(col*w/3),round(row*h/2),round((col+1)*w/3),round((row+1)*h/2))).resize((160,160),Image.Resampling.LANCZOS)
  a=np.asarray(t);rgb=a[:,:,:3].astype(np.uint16);p=(((rgb[:,:,0]>>3)<<11)|((rgb[:,:,1]>>2)<<5)|(rgb[:,:,2]>>3)).astype('<u2');p[a[:,:,3]==0]=0xf81f
  px.append(p.tobytes());alpha.append(a[:,:,3].tobytes());preview.paste(t,(col*180+10,row*180+10),t)
raw=b''.join(px)+b''.join(alpha);lib=ctypes.CDLL(ctypes.util.find_library('lz4'));lib.LZ4_compressBound.argtypes=[ctypes.c_int];lib.LZ4_compress_HC.argtypes=[ctypes.c_void_p,ctypes.c_void_p,ctypes.c_int,ctypes.c_int,ctypes.c_int];buf=ctypes.create_string_buffer(lib.LZ4_compressBound(len(raw)));n=lib.LZ4_compress_HC(raw,buf,len(raw),len(buf),12);assert n>0;data=buf.raw[:n]
c=['#include <stdint.h>','#ifdef __riscv','#define BG __attribute__((section(".bg_assets"),aligned(64)))','#else','#define BG','#endif',f'const unsigned v6_portraits_size={n};','const uint8_t v6_portraits_packed[] BG={'];c += [','.join(map(str,data[i:i+32]))+',' for i in range(0,n,32)];c+=['};'];(S/'v6_art_data.c').write_text('\n'.join(c)+'\n')
(A/'portraits.raw').write_bytes(raw);(A/'portraits.lz4').write_bytes(data);preview.save(A/'portraits_preview.png');(A/'portraits_manifest.json').write_text(json.dumps(dict(count=6,dimensions=[160,160],raw_bytes=len(raw),packed_bytes=n,sha256=hashlib.sha256(raw).hexdigest()),indent=2));print('6 portraits',len(raw),'decoded bytes')
