"""Compare actual R5.6 and R5.7 C pressure logic and draw-table hashes.
Pass --baseline pointing to the preserved full R5.6 project; R5.6 header sizes
are intentionally used for the baseline executable, never forced to R5.7 ABI.
"""
from pathlib import Path
import subprocess,json,argparse,shutil
v=Path(__file__).resolve().parent;r=v.parents[1];s=r/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src'
a=argparse.ArgumentParser();a.add_argument('--baseline',type=Path,default=Path('/home/user/fpga_contest/1025/Ti60_R5_DUALVIDEO'));args=a.parse_args()
old=args.baseline/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src';results=[]
for wrap in (0,1):
 hashes=[]
 for label,src in [('old',old),('new',s)]:
  exe=v/f'pressure_{label}_{wrap}';cmd=['gcc','-O2','-DSPROUT_STATS=1',f'-DSTRESS_WRAP={wrap}','-I'+str(src),str(v/'pressure_equivalence.c'),str(src/'game.c'),'-o',str(exe)]
  subprocess.run(cmd,check=True);p=subprocess.run([str(exe)],capture_output=True,text=True,check=True);(v/(exe.name+'.log')).write_text(p.stdout);hashes.append(p.stdout)
 assert hashes[0]==hashes[1],f'pressure states differ for wrap={wrap}'
 results.append({'wrap':wrap,'sizes':7,'frames_per_size':64,'bullets':16000,'all_state_and_draw_hashes_equal':True});print('PASS pressure equivalence wrap',wrap,flush=True)
(v/'pressure_equivalence.json').write_text(json.dumps(results,indent=2)+'\n')
