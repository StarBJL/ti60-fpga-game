from pathlib import Path
import ctypes,os,subprocess,json,random,time
root=Path(__file__).resolve().parents[2];out=Path(__file__).parent/'motion';out.mkdir(exist_ok=True)
env=dict(os.environ)
def run(args):
 r=subprocess.run([str(a) for a in args],env=env,text=True,capture_output=True,timeout=180)
 if r.returncode:raise RuntimeError(r.stdout+r.stderr)
 return r.stdout+r.stderr
sv=(root/'verification/tb_motion.sv').read_text(encoding='utf-8')
sv=sv.replace("value!==32'h00020000","value!==32'h00020001")
(out/'tb.sv').write_text(sv)
run(['/home/user/.local/bin/verilator','--binary','--timing','-j','4','-Wno-fatal','--top-module','tb_square_motion','--Mdir',str(out/'obj'),out/'tb.sv',root/'PROJECT/rtl/blit/square_motion.v'])
results=[]
for case,(n,mode,fault,kind) in enumerate([(29984,1,0,'one'),(29984,1,0,'three'),(10047,1,0,'one'),(33,1,0,'edges'),(33,1,1,'one'),(33,1,2,'three')]):
 rng=random.Random(1099+case);mem=[0xa55ac33c]*98304
 for i in range(n):
  size=1 if kind=='one' else 3 if kind in ('three','edges') else rng.randrange(6,41);mx=(1280-size)*16;my=(676-size)*16
  x=rng.randrange(mx+1);y=rng.randrange(640,my+1);vx=rng.randrange(-48,49);vy=rng.randrange(-48,49)
  if kind=='edges':x=[0,1,mx-1,mx][i%4];y=[640,641,my-1,my][(i//4)%4]
  if kind=='dense':x=3200;y=3200;size=6
  mem[16384+2*i]=x|(y<<16);mem[16385+2*i]=(vx&255)|((vy&255)<<8)|(size<<16)|(rng.randrange(8)<<24)
 def advance(data):
  data=data.copy();hist=[0]*880
  for i in range(n):
   xy,m=data[16384+2*i:16386+2*i];x=xy&65535;y=xy>>16;size=(m>>16)&255
   vx=(m&127)-(m&128);vy=((m>>8)&127)-((m>>8)&128);mx=(1280-size)*16;my=(676-size)*16
   if mode==1:
    x+=vx;y+=vy
    if x<0:x=-x;vx=-vx
    elif x>mx:x=2*mx-x;vx=-vx
    if y<640:y=1280-y;vy=-vy
    elif y>my:y=2*my-y;vy=-vy
   data[16384+2*i]=x|(y<<16);data[16385+2*i]=(m&0xffff0000)|((vy&255)<<8)|(vx&255)
   for yy in range(y//512,((y//16)+size-1)//32+1):
    for xx in range(x//512,((x//16)+size-1)//32+1):hist[yy*40+xx]+=1
  return data,hist
 expected,hist=advance(mem);second,hist2=advance(expected)
 for name,data in [('input',mem),('expected',expected),('hist',hist),('expected2',second),('hist2',hist2)]:
  width=4 if name.startswith('hist') else 8
  (out/(name+'.hex')).write_text(''.join(f'{v:0{width}x}\n' for v in data))
 t=time.monotonic();log=run([out/'obj/Vtb_square_motion',*[f'+{name}={out/(name+".hex")}' for name in ['input','expected','hist','expected2','hist2']],f'+count={n}',f'+mode={mode}',f'+fault={fault}'])
 (out/f'case{case}.log').write_text(log)
 entry=dict(count=n,mode=mode,fault=fault,kind=kind,seconds=round(time.monotonic()-t,2),log=log.strip());results.append(entry);print(entry,flush=True)
(out/'results.json').write_text(json.dumps(results,indent=2))
