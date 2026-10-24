from pathlib import Path
import subprocess,json
v=Path(__file__).resolve().parent;r=v.parents[1];s=r/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src';t=s.parent/'tools'
source=(s/'demo.c').read_text().replace('#include "bsp.h"','').replace('#define MOT_REG(o) (*(volatile uint32_t *)(uintptr_t)(0xF8101300u+(o)))','#define MOT_REG(o) fake_regs[(o)/4]').replace('#define DEMO_CLOCK() clint_getTimeLow(BSP_CLINT)','#define DEMO_CLOCK() test_clock()')
(v/'demo_under_test.h').write_text(source)
checks=[]
def run(name,files,flags):
 cmd=['gcc','-O2','-g','-I'+str(s),'-I'+str(t),'-I'+str(v),*flags,*map(str,files),'-o',str(v/name)]
 p=subprocess.run(cmd,text=True,capture_output=True);(v/(name+'_build.log')).write_text(p.stdout+p.stderr);assert p.returncode==0,p.stderr
 p=subprocess.run([str(v/name)],cwd=v,text=True,capture_output=True,timeout=180);(v/(name+'.log')).write_text(p.stdout+p.stderr);print(p.stdout+p.stderr,flush=True);assert p.returncode==0
 checks.append(dict(name=name,passed=True))
run('capacity',[v/'test_capacity.c',*[s/f for f in ['game.c','th_game.c','th_tab.c','th_pattern.c','th_stage2.c','th_art.c','stress_sizes.c','th_pix.c','gfx.c','mini_snprintf.c']]],['-DSPROUT_STATS=1'])
run('squares',[v/'test_squares.c',*[s/f for f in ['gfx.c','gfx_hw.c','blit.c','mini_snprintf.c']]],['-no-pie','-fno-pie','-DBLIT_HOST_MODEL','-DBLIT_MODEL_SQTAB=1','-DSPROUT_STATS=1'])
(v/'capacity_checks.json').write_text(json.dumps(checks,indent=2))
