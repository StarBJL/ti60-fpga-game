"""Exercise actual SQTAB RTL with 99,999 squares, random AXI backpressure.
Small distributed objects keep ALL four batches in band mode; a dense
negative control deliberately flushes before the first batch as the capacity guard does.
"""
from pathlib import Path
import subprocess,random,json
v=Path(__file__).resolve().parent;r=v.parents[1];old=r/'verification/r5_5';obj=v/'squares_rtl_obj';obj.mkdir(exist_ok=True)
tb=(old/'squares_rtl_tb.sv').read_text().replace('262143','1048575')
tb=tb.replace('  if(s_arvalid&&s_arready)begin','  if(arp>=1048576||awp>=1048576||wp>=1048576)$fatal(1,"test history overflow");\n  if(s_arvalid&&s_arready)begin')
(v/'squares_rtl_tb.sv').write_text(tb)
rtl=[r/'PROJECT/rtl/blit'/f for f in ['blit_engine.v','blit_axi_rd_arb.v','blit_axi_wide.v']]
cmd=['/home/user/.local/bin/verilator','--binary','--timing','-j','4','-Wno-fatal','--top-module','test','--Mdir',str(obj),str(v/'squares_rtl_tb.sv'),str(old/'wrapper.v'),*map(str,rtl)]
with (v/'squares_rtl_build.log').open('w') as f:subprocess.run(cmd,stdout=f,stderr=subprocess.STDOUT,check=True)
colors=[0xf32a,0x3dbb,0xb70d,0xf5eb,0x9bdc,0xeb50,0x7698,0xf77a];results=[]
for size,dense in [(1,False),(3,False),(6,False),(1,True)]:
 n=99999;rng=random.Random(123);pixels=[0]*921600;objects=[]
 for i in range(n):
  x=640 if dense else rng.randrange(1281-size);y=350 if dense else rng.randrange(40,677-size)
  objects.extend([(y*16<<16)|(x*16),(i%8<<24)|(size<<16)|0x0101]);a=151 if i&1 else 256;c=colors[i%8]
  for yy in range(y,y+size):
   for xx in range(x,x+size):
    d=pixels[yy*1280+xx];pixels[yy*1280+xx]=sum((((((c>>sh)&mask)*a+((d>>sh)&mask)*(256-a)+128)>>8)<<sh) for sh,mask in [(11,31),(5,63),(0,31)])
 words=[0x30000000,0x10000,0x602,2560,0x10000]
 for offset in range(0,n,29984):
  if dense and offset==0:words.append(0x40000000)
  words.extend([0x60010000|min(29984,n-offset),0x200000+offset*8])
 if not dense:words.append(0x40000000)
 with (v/'squares_input.hex').open('w') as f:
  for addr,data in [(0x200000,objects),(0x300000,words)]:f.write(f'@{addr//4:x}\n'+''.join(f'{x:08x}\n' for x in data))
 (v/'squares_expected.hex').write_text(''.join(f'{pixels[i]|pixels[i+1]<<16:08x}\n' for i in range(0,len(pixels),2)))
 p=subprocess.run([str(obj/'Vtest'),f'+input={v/"squares_input.hex"}',f'+expected={v/"squares_expected.hex"}',f'+end={0x300000+len(words)*4}'],text=True,capture_output=True,timeout=240)
 (v/f'rtl_square_{size}_{dense}.log').write_text(p.stdout+p.stderr);print(p.stdout+p.stderr,flush=True);assert p.returncode==0
 results.append(dict(count=n,size=size,dense=dense,all_batches_banded=not dense,passed=True,log=p.stdout.strip()))
(v/'squares_rtl.json').write_text(json.dumps(results,indent=2)+'\n')
