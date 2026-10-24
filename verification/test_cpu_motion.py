"""Execute the actual RV32 ELF's CPU motion loop; compare every object to a separate reference.
Instruction counts are not a CPU/DDR timing simulation and are not board FPS.
"""
from pathlib import Path
import importlib.util,random,json
root=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('rvprobe',Path(__file__).parent/'rv32_probe.py')
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
p=m.Probe(root/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/build/game.elf')
rng=random.Random(417);n=20000;at=p.sym['squares'];expected=[]
for i in range(n):
 size=rng.randrange(6,41);mx=(1280-size)*16;my=(676-size)*16
 x=rng.randrange(mx+1);y=rng.randrange(640,my+1);vx=rng.randrange(-48,49);vy=rng.randrange(-48,49);c=rng.randrange(8)
 if i<16:x=[0,1,mx-1,mx][i%4];y=[640,641,my-1,my][i//4]
 p.put(at+i*8,x|(y<<16));p.put(at+i*8+4,((vx&255)|((vy&255)<<8)|(size<<16)|(c<<24)))
 x+=vx;y+=vy
 if x<0:x=-x;vx=-vx
 elif x>mx:x=mx*2-x;vx=-vx
 if y<640:y=1280-y;vy=-vy
 elif y>my:y=my*2-y;vy=-vy
 expected.extend([x|(y<<16),(vx&255)|((vy&255)<<8)|(size<<16)|(c<<24)])
result=p.call('square_cpu_step',[n])
assert [p.get(at+i*4) for i in range(2*n)]==expected
result.update(count=n,exact=True,scope='Production RV32 ELF; instruction count only, no cache/DDR/cycle timing model.')
(Path(__file__).parent/'cpu_motion.json').write_text(json.dumps(result,indent=2))
print(result)
