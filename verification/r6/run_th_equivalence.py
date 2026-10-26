from pathlib import Path
import subprocess,json,shutil
v=Path(__file__).resolve().parent;r=v.parents[1];s=r/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src'
old=v/'reference_r59';old.mkdir(exist_ok=True)
# Snapshot of the unchanged current-project gameplay before this merge.
backup=r.parent/'before_th'
if backup.is_dir():
 for p in backup.iterdir():
  if p.suffix in ('.c','.h'):shutil.copy2(p,old/p.name)
common=['th_tab.c','th_pix.c','gfx.c','mini_snprintf.c','stress_sizes.c']
changed=['th_game.c','th_pattern.c','th_stage2.c','th_art.c']
for name,base,flags in [('old',old,[]),('new',s,['-DHOT'])]:
 cmd=['gcc','-O2','-I'+str(base),'-I'+str(s),*flags,str(v/'test_th_equivalence.c'),*[str(base/n) for n in changed],*[str(s/n) for n in common],'-o',str(v/('th_'+name))]
 subprocess.run(cmd,check=True,capture_output=True)
results=[]
for stage,players,stress in [(0,1,0),(0,3,0),(1,1,0),(1,3,0),(0,1,1)]:
 outputs=[]
 for name in ['old','new']:
  p=subprocess.run([str(v/('th_'+name)),str(stage),str(players),str(stress)],check=True,capture_output=True,text=True,timeout=180)
  (v/f'th_{name}_{stage}_{players}_{stress}.log').write_text(p.stdout);outputs.append(p.stdout)
 if outputs[0]!=outputs[1]:
  for a,b in zip(outputs[0].splitlines(),outputs[1].splitlines()):
   if a!=b:print('first difference',stage,players,stress,a,b);break
  raise AssertionError('gameplay changed')
 results.append(dict(stage=stage,players=players,stress=stress,ticks=4800,passed=True))
 print('PASS TH gameplay equality',results[-1],flush=True)
(v/'th_equivalence.json').write_text(json.dumps(results,indent=2))
