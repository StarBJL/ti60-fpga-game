"""Exercise dual-output mode while the real TH/carrot/square logic runs (host model)."""
from pathlib import Path
import re,subprocess,tempfile,json
v=Path(__file__).resolve().parent;r=v.parents[1];g=r/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game';s=g/'src'
source=(v/'test_main.c').read_text();start=source.index('static const struct { uint32_t ms; unsigned char c; } script[] = {');lo=source.index('{\n',start)+2;hi=source.index('\n};',lo)
rows=[]
for line in source[lo:hi].splitlines():
 m=re.search(r'\{(\d+),',line)
 if m:rows.append((int(m[1]),line))
for ms,command in [(1000,':301 resolution 1\n'),(6800,':302 resolution 0\n')]:
 rows.extend((ms,f'    {{{ms},{ord(c)}}},') for c in command)
source=source[:lo]+'\n'.join(line for _,line in sorted(rows,key=lambda x:x[0]))+source[hi:]
# An absolute include keeps the compiled fixture in a temporary folder.
source=source.replace('#include "../../PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/main.c"','#include "'+str(s/'main.c')+'"')
files=['game.c','gfx.c','render.c','demo.c','gfx_hw.c','blit.c','mini_snprintf.c','pad.c','th_tab.c','th_game.c','th_pattern.c','th_stage2.c','th_art.c','th_render.c','th_pix.c','audio.c','th_sound.c','game_sound.c','music.c','control.c','stress_sizes.c','input_devices.c']
with tempfile.TemporaryDirectory(prefix='ti60_r5_video_game_') as temp:
 d=Path(temp);(d/'test.c').write_text(source)
 cmd=['gcc','-O2','-g','-no-pie','-fno-pie','-Wl,-Ttext-segment=0x08000000','-DBLIT_HOST_MODEL','-DSPROUT_STATS=1','-DBOOT_DIAG=0','-DHOST_RES=0x02d00500u','-I'+str(s),'-I'+str(g/'tools'),'-I'+str(g/'tools/mock'),str(d/'test.c'),*[str(s/f) for f in files],'-o',str(d/'test')]
 c=subprocess.run(cmd,text=True,capture_output=True);(v/'video_gameplay_build.log').write_text(c.stdout+c.stderr);assert c.returncode==0,c.stderr
 c=subprocess.run([str(d/'test'),str(v/'video_gameplay_uart.log')],text=True,capture_output=True,timeout=180);(v/'video_gameplay.log').write_text(c.stdout+c.stderr);print(c.stdout)
 assert c.returncode==0,c.stderr
(v/'video_gameplay_result.json').write_text(json.dumps({'passed':True,'hardware_tested':False,'description':'Real main loop and engine model, TH/carrot/squares run during physical 1080 mode; repeated mode and backend switches; unchanged quantity/skill/guard assertions.'},indent=2)+'\n')
