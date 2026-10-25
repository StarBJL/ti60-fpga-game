#!/usr/bin/env python3
"""Read EDID without changing display modes. Connect the target monitor's HDMI
to this PC first. Missing VIC34 is not proof that 1080p30 is unsupported.
No monitor serial number is printed. Python standard library only.
"""
from pathlib import Path
import argparse,json,hashlib
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--file',type=Path);p.add_argument('--out',type=Path);args=p.parse_args()
def parse(data,name):
 if len(data)<128 or data[:8]!=b'\x00\xff\xff\xff\xff\xff\xff\x00':return {'connector':name,'error':'invalid EDID'}
 manufacturer=(data[8]<<8)|data[9];out={'connector':name,'manufacturer':''.join(chr(64+((manufacturer>>k)&31)) for k in [10,5,0]),'sha256':hashlib.sha256(data).hexdigest(),'bytes':len(data),'checksum_ok':all(sum(data[k:k+128])%256==0 for k in range(0,len(data),128)),'model':'unknown','detailed_timings':[],'cta_vics':[]}
 def desc(x):
  if len(x)!=18:return
  pixel=int.from_bytes(x[:2],'little')*10000
  if pixel:
   w=x[2]+((x[4]>>4)<<8);hb=x[3]+((x[4]&15)<<8);h=x[5]+((x[7]>>4)<<8);vb=x[6]+((x[7]&15)<<8)
   if w+hb and h+vb:out['detailed_timings'].append({'width':w,'height':h,'refresh_hz':round(pixel/((w+hb)*(h+vb)),3),'interlaced':bool(x[17]&128),'pixel_hz':pixel})
  elif x[3]==0xfc:out['model']=x[5:].decode('ascii',errors='replace').strip('\x00\n ')
  elif x[3]==0xfd:out['range']={'min_vertical_hz':x[5],'max_vertical_hz':x[6],'min_horizontal_khz':x[7],'max_horizontal_khz':x[8]}
 for k in range(54,126,18):desc(data[k:k+18])
 for k in range(128,len(data)-127,128):
  block=data[k:k+128]
  if block[0]!=2:continue
  end=block[2] or 127;i=4
  while i<end:
   tag=block[i]>>5;n=block[i]&31
   if i+1+n>end:break
   if tag==2:out['cta_vics'].extend(x&127 for x in block[i+1:i+1+n])
   i+=n+1
  if block[2]:
   for i in range(block[2],110,18):desc(block[i:i+18])
 out['advertises_1080p30_vic34']=34 in out['cta_vics'];out['note']='Absence of VIC34 is not a definitive rejection; detailed timings, range and real input behavior also matter.';return out
files=[args.file] if args.file else list(Path('/sys/class/drm').glob('card*-*/edid'));result=[]
for path in files:
 try:data=path.read_bytes()
 except OSError:continue
 if data:result.append(parse(data,str(path) if args.file else path.parent.name))
text=json.dumps(result,ensure_ascii=False,indent=2)+'\n'
if args.out:args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(text)
print(text,end='')
