from pathlib import Path
import re,json
from PIL import Image,ImageFont,ImageDraw
root=Path(__file__).resolve().parents[1];s=root/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src'
mapping={
'STAR SIGN: TWIN GALAXY':'星符：双子银河','RAIN SIGN: BENDING SHOWER':'雨符：曲折骤雨','FINAL: PRISM FLOWER':'终符：棱镜之花',
'SPROUT':'东方','DANMAKU':'弹幕游戏','HISCORE':'最高分','SCORE':'得分','PLAYER':'生命','BOMB':'符卡','POWER':'火力','GRAZE':'擦弹','POINT':'收集','TI60 BITBLT':'TI60 硬件加速',
'START  NORMAL':'开始 普通','START  LUNATIC':'开始 极难','STAGE 2 PRACTICE':'第二关练习','RESUME':'继续','RESTART':'重新开始','TITLE':'标题',
'SPROUTING FLOWER FIELD':'萌芽花田','MOONLIT SEA OF CLOUDS':'月下云海','STAGE 1':'第一关','STAGE 2':'第二关','WARNING!':'首领来袭',
'FINAL: FULL MOON NIGHT':'终符：满月之夜','MOON SIGN: SILVER PILLARS':'月符：银色光柱','PHANTOM SIGN: LUNAR MIRAGE':'幻符：月之幻影','STAR SIGN: METEOR SHOWER':'星符：流星雨',
'BONUS %u':'奖励 %u','GET SPELL CARD BONUS!!':'获得符卡奖励','BULLETS %d':'弹幕 %d','STRESS X%d %s':'压力 X%d %s','AUTO':'自动','MANUAL  - +':'手动 - +',
'LOAD %d%%  CAP %d':'负载 %d%% 上限 %d','LOAD %d%%':'负载 %d%%','AUTOPLAY DEMO':'自动演示','LUNATIC':'极难模式','%d FPS %s':'%d 帧/秒 %s','HW':'硬件','CPU':'处理器',
'TI60 RISC-V + BITBLT':'TI60 硬件加速','ARROWS MOVE  Z SHOT  X BOMB  SHIFT SLOW':'方向移动 Z射击 X符卡 SHIFT低速',
'KEYBOARD: TOOLS/PC PAD.PY  UART: WASD 1 BOMB 2 SLOW':'WASD移动 1符卡 2低速',
'PAUSED':'暂停','GAME OVER':'游戏结束','SCORE %u':'得分 %u','STAGE CLEAR!':'关卡通过','ALL CLEAR!':'全部通关','CLEAR BONUS %d':'通关奖励 %d','GRAZE %d  POINT %d':'擦弹 %d 收集 %d',
'QUICK TRIGGER':'快速射击','HEAVY SEEDS':'重型子弹','RUNNING ROOTS':'移动加速','HEARTY HARVEST':'生命强化','GREEN MAGNET':'经验磁铁','DOUBLE SPROUT':'多重射击',
'SHOOT FASTER':'提高射击速度','-3 TICKS PER SHOT':'射击间隔减少3帧','MORE SEEDS. LESS WAIT.':'连续射击更迅速','DAMAGE +1':'伤害增加1','EVERY SEED HITS HARDER':'每发子弹伤害提升','SMALL SEED. BIG IMPACT.':'火力更强大',
'MOVE SPEED UP':'移动速度提升','DODGE THE SWARM':'灵活躲避怪物','KEEP THOSE ROOTS MOVING.':'保持移动',
'MAX HEALTH +20':'最大生命增加20','RESTORE 40 HEALTH':'恢复40点生命','A LITTLE EXTRA STAYING POWER.':'提高生存能力',
'PICKUP RANGE +40':'拾取范围增加40','COLLECT XP FROM AFAR':'远处经验自动吸引','BRING THE HARVEST TO YOU.':'收集经验更轻松',
'PROJECTILES +1':'子弹数量增加1','FIRE A WIDER VOLLEY':'扩大射击范围','A LITTLE HELP FROM A FRIEND.':'更多子弹同时发射',
'%d FPS':'%d 帧/秒','HW BLIT':'硬件绘制','CPU DRAW':'处理器绘制','DRAW %d US':'绘制 %d 微秒',
'CPU %d FPS %d.%d MS':'处理器 %d帧 %d.%d毫秒','CPU -- FPS':'处理器 --帧','HW %d FPS %d.%d MS':'硬件 %d帧 %d.%d毫秒','HW -- FPS':'硬件 --帧',
'SUB %d QST %d TAIL %d VS %d US  BLT %d%% RD %d WR %d MB/S  N G - + V':'提交%d 队列%d 尾部%d 同步%d微秒 负载%d%% 读%d 写%d MB/s',
'LOGIC %d DRAW %d VSYNC %d US   N NEXT  G CPU/HW  -/+ LOAD  V VSYNC  E ALPHA':'逻辑%d 绘制%d 同步%d微秒 N切换 G绘制 V同步 E透明',
'720P MOVING SQUARES / MOT2':'720p 运动方块压力测试','MOTION ERROR - STOPPED; RESTART THE PROGRAM':'硬件运动出错，已停止；请重新启动程序',
'%d SQUARES  %dx%d  MOTION %s  TICK %u US%s':'方块%d %dx%d 运动%s 耗时%u微秒%s','ERROR':'错误','  ALPHA ON':' 透明开启','  ALPHA OFF':' 透明关闭',
'640X480 COPY EVERY FRAME, PICTURE %d/3, SWITCH EVERY %d FRAME%s':'1280x720 每帧整图复制 图像%d/3 每%d帧切换%s','BITBLT DEMO: IMAGE SWITCHING':'大图切换演示',
}
files=['th_render.c','th_art.c','th_game.c','th_pattern.c','th_stage2.c','game.c','demo.c']
for name in files:
 f=s/name;x=f.read_text(encoding='utf-8')
 def replace(m):
  val=m.group(1);return '"'+mapping.get(val,val)+'"'
 x=re.sub(r'"([^"\n]*)"',replace,x)
 if name=='th_render.c':x=x.replace('char s[48]','char s[160]').replace('char s[64]','char s[160]')
 if name=='demo.c':x=x.replace('char detail[96]','char detail[200]').replace('char text[96]','char text[200]')
 f.write_text(x,encoding='utf-8')
# Collect exactly the glyphs used in C literals; a missing glyph is a build error.
chars=set()
for f in s.glob('*.c'):
 for val in re.findall(r'"([^"\n]*)"',f.read_text(encoding='utf-8')):
  chars.update(c for c in val if ord(c)>=128)
font=ImageFont.truetype('C:/Windows/Fonts/simsun.ttc',12)
codes=sorted(map(ord,chars));bitmaps=[]
for code in codes:
 im=Image.new('1',(12,12));d=ImageDraw.Draw(im);d.text((0,-2),chr(code),font=font,fill=1)
 rows=[sum((1<<(11-x)) for x in range(12) if im.getpixel((x,y))) for y in range(12)]
 assert any(rows),f'Empty glyph {chr(code)}';bitmaps.append(rows)
out='/* Generated from the Simplified Chinese screen strings, SimSun 12px. */\n#include <stdint.h>\n#define ZH_GLYPHS '+str(len(codes))+'\n'
out+='static const uint16_t zh_codes[ZH_GLYPHS]={'+','.join(hex(c) for c in codes)+'};\n'
out+='static const uint16_t zh_bitmap[ZH_GLYPHS][12]={\n'+''.join('{'+','.join(hex(v) for v in rows)+'},\n' for rows in bitmaps)+'};\n'
(s/'zh_font_data.h').write_text(out,encoding='utf-8')
(root/'verification/zh_glyphs.json').write_text(json.dumps(dict(count=len(codes),chars=''.join(map(chr,codes))),ensure_ascii=False,indent=2),encoding='utf-8')
print('Translated screen strings; generated',len(codes),'Chinese glyphs')
