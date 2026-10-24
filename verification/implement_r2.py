from pathlib import Path
import re,shutil,json,statistics
root=Path(__file__).resolve().parents[1]; p=root/'PROJECT'
rel=Path('par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game')
g=p/rel;s=g/'src';old=Path('D:/FPGA_competition/Ti60_720p_SQUARE20K_MERGE')
donor=Path('D:/FPGA_competition/Ti60_11_Dongfang_185MHz_R9_30K_BOUNDARY/PROJECT')/rel/'src'
zh=Path('D:/FPGA_competition/Ti60_11_Dongfang_185MHz_R12_SKILL_ZH/PROJECT')/rel/'src'
def read(f):return f.read_text(encoding='utf-8-sig')
def write(f,x):f.write_text(x,encoding='utf-8')
# Read every saved R1 line and split the complete log by explicit user commands.
sessions=[]
for log in sorted((old/'logs').glob('*/*.txt')):
 lines=read(log).splitlines();windows=[];last=None;audio=[];ab=[]
 for line in lines:
  if line.startswith('[square motion '):last='motion '+line.split()[2]
  if line.startswith('[square packed table '):last=line
  if line.startswith('fps '):
   m=re.search(r'fps (\d+\.\d+)',line)
   if m:windows.append((last,float(m[1])))
  if 'square A/B' in line:ab.append(line)
  if line.lstrip().startswith('audio:'):audio.append(line.strip())
 groups={}
 for label,value in windows:groups.setdefault(str(label),[]).append(value)
 sessions.append(dict(path=str(log),lines=len(lines),ab=ab,audio_first=audio[:1],audio_last=audio[-1:],
   fps_windows={k:dict(n=len(a),min=min(a),max=max(a),median=statistics.median(a)) for k,a in groups.items()}))
write(root/'verification/r1_logs_read.json',json.dumps(sessions,ensure_ascii=False,indent=2))
# The R9 game logic adds the custom tier without later character revisions.
for name in ['game.c','game.h']:shutil.copy2(donor/name,s/name)
x=read(s/'sprout_config.h').replace('#define MAX_HOSTILE_BULLETS 4096','#define MAX_HOSTILE_BULLETS 40000')
write(s/'sprout_config.h',x)
for name in ['game.c','render.c']:
 x=read(s/name);x=x.replace('#include <string.h>','#include <string.h>\n/* Native 720p scene; original character artwork is retained. */\n#undef SCREEN_W\n#undef SCREEN_H\n#define SCREEN_W 1280\n#define SCREEN_H 720')
 write(s/name,x)
# Native game coordinates: scale the old fixed arena, not the framebuffer.
x=read(s/'game.c')
x=x.replace('320*FIX','640*FIX').replace('240*FIX','360*FIX')
x=x.replace('632*FIX','1264*FIX').replace('444*FIX','666*FIX').replace('58*FIX','87*FIX')
x=x.replace('24*FIX,616*FIX','36*FIX,1244*FIX').replace('76*FIX,424*FIX','114*FIX,636*FIX')
x=x.replace('16*FIX,624*FIX','24*FIX,1256*FIX').replace('70*FIX,432*FIX','105*FIX,648*FIX')
# Event bits are set at the action, then consumed once by game_sound_tick.
x=x.replace('g->muzzle=4;','g->muzzle=4; g->sfx|=GS_SHOT;')
x=x.replace('e->hp-=b->damage;','e->hp-=b->damage; g->sfx|=GS_HIT;')
x=x.replace('++g->kills;','++g->kills; g->sfx|=GS_KILL;')
x=x.replace('g->hurt_flash=8;','g->hurt_flash=8; g->sfx|=GS_HURT;').replace('g->hurt_flash=10;','g->hurt_flash=10; g->sfx|=GS_HURT;')
x=x.replace('++g->level;','++g->level; g->sfx|=GS_LEVEL;')
write(s/'game.c',x)
x=read(s/'game.h').replace('#define GAME_SEED_HALF 4','#define GAME_SEED_HALF 6')
x=x.replace('typedef struct {\n    Phase phase;', 'enum { GS_SHOT=1,GS_HIT=2,GS_KILL=4,GS_HURT=8,GS_LEVEL=16 };\ntypedef struct {\n    uint32_t sfx;\n    Phase phase;')
write(s/'game.h',x)
# Keep the 720p gfx implementation, transplant only its UTF-8 text implementation.
x=read(s/'gfx.c');z=read(zh/'gfx.c')
x=x.replace('#include "gfx.h"','#include "gfx.h"\n#include "zh_font_data.h"')
start=z.index('static uint32_t font_epoch;');end=z.index('static const Sprite *font_atlas',start)
local=x.index('void gfx_font_reset(Gfx *g)');localend=x.index('static const Sprite *font_atlas',local)
x=x[:local]+z[start:end]+x[localend:]
start=x.index('void gfx_text(Gfx *g');x=x[:start]+z[z.index('/* Strict UTF-8 decoder:'):]
write(s/'gfx.c',x)
# Square capacity and fine load adjustment. @ remains the 20K reference preset.
x=read(s/'demo.h').replace('20000','30000');write(s/'demo.h',x)
x=read(s/'demo.c').replace('MOT2-20261022','MOT2-R2-20261023');write(s/'demo.c',x)
x=read(p/'rtl/blit/square_motion.v').replace('MAX_COUNT=20000','MAX_COUNT=30000').replace('MAX_COUNT = 20000','MAX_COUNT = 30000');write(p/'rtl/blit/square_motion.v',x)
# hdmi top instantiation has an explicit capacity override.
x=read(p/'rtl/ddr3_example_top.v').replace('.MAX_COUNT(20000)','.MAX_COUNT(30000)').replace('.MAX_COUNT (20000)','.MAX_COUNT (30000)');write(p/'rtl/ddr3_example_top.v',x)
x=read(s/'main.c').replace('720P-SQUARE20K-MOT2-20261022','720P-MERGE-R2-ZH-FULL-20261023')
x=x.replace('#include "audio.h"','#include "audio.h"\n#include "game_sound.h"')
x=x.replace('static void handle_char(char c)\n{','''static void handle_char(char c)
{
    if(scene==SCENE_SQUARES && (c==',' || c=='.')){
        if(hw_avail)blit_wait();sq_count += c=='.'?100:-100;
        if(sq_count<100)sq_count=100;if(sq_count>DEMO_MAX_SQUARES)sq_count=DEMO_MAX_SQUARES;
        tx_printf("[squares fine load %d step 100 max %d]\\r\\n",sq_count,DEMO_MAX_SQUARES);stats_reset=1;return;
    }
    if(scene==SCENE_GAME && c=='4' && (game.phase==STRESS_SELECT || game.limit_mode)){
        key_pressed|=KEY_PICK4;return;
    }
    if(scene==SCENE_GAME && game.stress_tier==STRESS_CUSTOM && (c=='-'||c=='+'||c=='=')){
        if(hw_avail)blit_wait();game_set_stress_bullets(&game,game.stress_bullet_target+(c=='-'?-1000:1000));
        tx_printf("[carrot custom cap %d]\\r\\n",game_hostile_slots(&game));stats_reset=1;return;
    }''')
x=x.replace("case ',':", "case '(': case ',':").replace("case '.':", "case ')': case '.':")
x=x.replace('game_tick(&game, in);','game_tick(&game, in);\n            game_sound_tick(&game);')
x=x.replace('if (scene == SCENE_SQUARES) {\n            /* Exercise real DDR-backed audio concurrently with the square DMA. */\n            aud_bgm(aud_find("bgm_title"),200,40);aud_bgm_volume(256);aud_tick();\n        } else if (scene != SCENE_TH) th_sound_idle();', 'if (scene != SCENE_TH && scene != SCENE_GAME) th_sound_idle();')
x=x.replace('if ((scene == SCENE_TH && th_hd) || scene == SCENE_SQUARES)', 'if ((scene == SCENE_TH && th_hd) || scene != SCENE_TH)')
x=x.replace('scene = (scene + 1) % SCENE_COUNT;', 'scene = (scene + 1) % SCENE_COUNT; aud_tone(0);')
write(s/'main.c',x)
# Images are generated at physical size in the pre-existing three 2MB DDR slots.
x=read(s/'demo.c')
a=x.index('static void gen_image');b=x.index('/* ------------------------------------------------------------------------- */\n/* Info bars:',a)
part=x[a:b].replace('SCREEN_W','scene_w').replace('SCREEN_H','scene_h')
part=part.replace('"IMAGE 1", "IMAGE 2", "IMAGE 3"','"图像一", "图像二", "图像三"')
part=part.replace('170, 190, 300, 100','scene_w/2-180, scene_h/2-60, 360, 120')
part=part.replace('320 - gfx_text_width(names[i], 7) / 2, 215','scene_w/2 - gfx_text_width(names[i], 7) / 2, scene_h/2-28')
x=x[:a]+part+x[b:]
a=x.index('void demo_images_render');part=x[a:].replace('SCREEN_W','scene_w').replace('SCREEN_H','scene_h');x=x[:a]+part
write(s/'demo.c',x)
print('R2 core changes written; native render/UI and Chinese strings follow.')
