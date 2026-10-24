"""R5.7 host checks. Models are not board performance measurements."""
from pathlib import Path
import subprocess,json,sys,re
v=Path(__file__).resolve().parent;r=v.parents[1]
s=r/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src'
t=s.parent/'tools';results=[]
def check(name,files,flags=(),cwd=None):
 cmd=['gcc','-O2','-g','-I'+str(s),'-I'+str(t),*flags,*map(str,files),'-o',str(v/name)]
 p=subprocess.run(cmd,capture_output=True,text=True);(v/(name+'_build.log')).write_text(p.stdout+p.stderr);assert p.returncode==0,p.stderr[-5000:]
 p=subprocess.run([str(v/name)],cwd=cwd or v,capture_output=True,text=True,timeout=240);(v/(name+'.log')).write_text(p.stdout+p.stderr);print(name,p.returncode,p.stdout[-800:],flush=True);assert p.returncode==0,p.stderr[-3000:]
 results.append({'test':name,'passed':True})
base=['game.c','th_game.c','th_pattern.c','th_stage2.c','th_art.c','th_tab.c','th_pix.c','stress_sizes.c']
if '--band-only' not in sys.argv:check('test_coop',[v/'test_coop.c',*[s/x for x in base],s/'pad.c',s/'input_devices.c',s/'gfx.c',s/'mini_snprintf.c'],['-fsanitize=address,undefined','-fno-omit-frame-pointer','-no-pie'])
host=['-no-pie','-fno-pie','-Wl,-Ttext-segment=0x08000000','-DBLIT_HOST_MODEL','-DBLIT_MODEL_SQTAB=1','-DSPROUT_STATS=1']
if '--band-only' not in sys.argv:check('coop_preview',[v/'coop_preview.c',*[s/x for x in base+['render.c','th_render.c','gfx.c','gfx_hw.c','blit.c','mini_snprintf.c','coop_fx.c']]],host)
from PIL import Image
import numpy as np
for n in ['carrot_coop','touhou_coop']:
 a=np.fromfile(v/(n+'.rgb565'),dtype='<u2').reshape(720,1280);rgb=np.stack([((a>>11)&31)*255//31,((a>>5)&63)*255//63,(a&31)*255//31],axis=-1).astype('uint8');Image.fromarray(rgb).save(v/(n+'.png'))
# Inject only the MOT2 MMIO and time source. Keep all production motion,
# capacity admission, batching and rendering code unchanged.
a=(s/'demo.c').read_text()
a=re.sub(r'#define MOT_REG\(o\).*', '#define MOT_REG(o) fake_regs[(o)/4]',a)
a=a.replace('clint_getTimeLow(BSP_CLINT)','test_clock()').replace('#include "bsp.h"','')
(v/'demo_under_test.h').write_text(a)
check('test_band_bound',[v/'test_band_bound.c',*[s/x for x in ['gfx.c','gfx_hw.c','blit.c','mini_snprintf.c']]],[x for x in host if not x.startswith('-Wl,-Ttext')]+['-I'+str(t/'mock')])
(v/'host_checks.json').write_text(json.dumps({'tests':results,'hardware_tested':False},indent=2)+'\n')
