"""Rebuild the merge's host checks on Linux; all logs stay in this directory."""
from pathlib import Path
import subprocess, json, sys, re
v=Path(__file__).resolve().parent
root=v.parents[1]
game=root/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game'
src=game/'src';tools=game/'tools'
checks=[]
def run(name, files, flags=(), args=()):
    exe=v/('check_'+name)
    cmd=['gcc','-O2','-g','-I'+str(src),'-I'+str(tools),*flags,*map(str,files),'-o',str(exe)]
    p=subprocess.run(cmd,text=True,capture_output=True)
    (v/(name+'_build.log')).write_text(p.stdout+p.stderr)
    if p.returncode:raise RuntimeError(name+' compilation failed: '+p.stderr[-2000:])
    p=subprocess.run([str(exe),*args],cwd=v,text=True,capture_output=True,timeout=180)
    (v/(name+'.log')).write_text(p.stdout+p.stderr)
    checks.append(dict(name=name,passed=p.returncode==0,log=name+'.log'))
    print(name, p.returncode,p.stdout[-450:],flush=True)
    if p.returncode:raise RuntimeError(name+' failed: '+p.stdout[-1500:]+p.stderr[-1500:])
if __name__=='__main__':
    names=set(sys.argv[1:])
    def wanted(n):return not names or n in names
    if wanted('gameplay'):run('gameplay',[root/'verification/r4/test_merge_gameplay.c',src/'game.c'],['-DSPROUT_STATS=1'])
    if wanted('visual'):run('visual',[root/'verification/r4/test_merge_visual.c',*[src/f for f in ['game.c','gfx.c','gfx_hw.c','blit.c','mini_snprintf.c']]],['-no-pie','-fno-pie','-Wl,-Ttext-segment=0x08000000','-DBLIT_HOST_MODEL','-DSPROUT_STATS=1','-DGFX_FONT_ATLAS_CPU_MAX_SCALE=3'])
    if wanted('audio'):run('audio',[root/'verification/r4/test_audio.c',src/'game_sound.c',src/'music.c'])
    if wanted('font'):run('font',[root/'verification/test_font.c'])
    if wanted('demos'):run('demos',[root/'verification/r4/test_demos.c',src/'gfx.c',src/'mini_snprintf.c'])
    if wanted('touhou'):run('touhou',[root/'verification/th_preview.c',*[src/f for f in ['th_tab.c','th_game.c','th_pattern.c','th_stage2.c','th_art.c','th_render.c','th_pix.c','gfx.c','mini_snprintf.c','stress_sizes.c']]])
    if wanted('control'):run('control',[v/'test_control.c',src/'control.c',src/'pad.c'],['-fsanitize=address,undefined','-fno-omit-frame-pointer'])
    if wanted('th_sizes'):run('th_sizes',[root/'verification/r4/test_th_sizes.c',*[src/f for f in ['th_tab.c','th_game.c','th_pattern.c','th_stage2.c','th_art.c','th_render.c','th_pix.c','gfx.c','mini_snprintf.c','stress_sizes.c']]])
    if wanted('main'):
        run('main',[v/'test_main.c',*[src/f for f in ['game.c','gfx.c','render.c','demo.c','gfx_hw.c','blit.c','mini_snprintf.c','pad.c','th_tab.c','th_game.c','th_pattern.c','th_stage2.c','th_art.c','th_render.c','th_pix.c','audio.c','th_sound.c','game_sound.c','music.c','control.c','stress_sizes.c','input_devices.c']]],['-no-pie','-fno-pie','-Wl,-Ttext-segment=0x08000000','-DBLIT_HOST_MODEL','-DSPROUT_STATS=1','-DBOOT_DIAG=0','-DHOST_RES=0x02d00500u','-I'+str(tools/'mock')],['main_uart.log'])
    codes={int(x,16) for x in re.findall(r'0x[0-9a-f]+',(src/'zh_font_data.h').read_text().split('zh_bitmap')[0])}
    used=set()
    for p in src.glob('*.c'):
        for literal in re.findall(r'"([^"\n]*)"',p.read_text()):used.update(ord(c) for c in literal if ord(c)>=128)
    assert not used-codes, 'Missing font characters: '+''.join(map(chr,used-codes))
    checks.append(dict(name='all_screen_glyphs',passed=True,count=len(used)))
    (v/('checks_'+('_'.join(sorted(names)) or 'all')+'.json')).write_text(json.dumps(checks,indent=2))
