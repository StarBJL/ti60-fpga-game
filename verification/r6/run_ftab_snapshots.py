from pathlib import Path
import subprocess
v=Path(__file__).resolve().parent;r=v.parents[1];s=r/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src';t=s.parent/'tools'
a=(s/'demo.c').read_text().replace('#include "bsp.h"','').replace('#define MOT_REG(o) (*(volatile uint32_t *)(uintptr_t)(0xF8101300u+(o)))','#define MOT_REG(o) fake_regs[(o)/4]').replace('#define DEMO_CLOCK() clint_getTimeLow(BSP_CLINT)','#define DEMO_CLOCK() test_clock()')
(v/'demo_under_test.h').write_text(a)
cmd=['gcc','-O2','-g','-I'+str(s),'-I'+str(t),'-no-pie','-fno-pie','-Wl,-Ttext-segment=0x08000000','-DBLIT_HOST_MODEL','-DBLIT_MODEL_SQTAB=1','-DSPROUT_STATS=1',str(v/'test_ftab_snapshots.c'),*[str(s/f) for f in ['gfx.c','gfx_hw.c','blit.c','mini_snprintf.c']],'-o',str(v/'ftab_snapshots')]
p=subprocess.run(cmd,capture_output=True,text=True);(v/'ftab_snapshots_build.log').write_text(p.stdout+p.stderr);assert p.returncode==0,p.stderr
p=subprocess.run([str(v/'ftab_snapshots')],capture_output=True,text=True,timeout=240);(v/'ftab_snapshots.log').write_text(p.stdout+p.stderr);print(p.stdout,p.stderr);assert p.returncode==0
