from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SRC=ROOT/"PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src"
import re,json,shutil
from PIL import Image,ImageFont,ImageDraw
p=SRC/'zh_font_data.h';old=p.read_text();codes={int(c,16) for c in re.findall(r'0x[0-9a-f]+',old.split('zh_bitmap')[0])}
for f in SRC.glob('*.c'):
 for lit in re.findall(r'"([^"\n]*)"',f.read_text()):codes.update(ord(c) for c in lit if ord(c)>=128)
fontpath='/usr/share/fonts/opentype/noto/NotoSansCJK-Medium.ttc';font=ImageFont.truetype(fontpath,12,index=2)
rows={}
for code in sorted(codes):
 im=Image.new('L',(12,12));d=ImageDraw.Draw(im);d.text((0,-3),chr(code),font=font,fill=255)
 row=[sum(1<<(11-x) for x in range(12) if im.getpixel((x,y))>=100) for y in range(12)]
 assert any(row),chr(code);rows[code]=row
t='/* R4 UI subset: Noto Sans CJK SC Medium, 12px monochrome; SIL OFL 1.1. */\n#include <stdint.h>\n#define ZH_GLYPHS '+str(len(rows))+'\n'
t+='static const uint16_t zh_codes[ZH_GLYPHS]={'+','.join(hex(c) for c in rows)+'};\n'
t+='static const uint16_t zh_bitmap[ZH_GLYPHS][12]={\n'+''.join('{'+','.join(hex(x) for x in row)+'}, /* '+chr(c)+' */\n' for c,row in rows.items())+'};\n';p.write_text(t)
out=ROOT/'ART/UI_R4';out.mkdir(parents=True,exist_ok=True)
lic=Path('/usr/share/doc/fonts-noto-cjk/copyright')
if lic.exists():shutil.copy2(lic,out/'Noto_CJK_LICENSE.txt')
(out/'font_manifest.json').write_text(json.dumps({'font':'Noto Sans CJK SC Medium','source':fontpath,'size':12,'threshold':100,'glyphs':len(rows),'license':'SIL Open Font License 1.1','ascii':'Existing 5x7 pixel font, vertically aligned to shared line height'},ensure_ascii=False,indent=2))
print('Generated',len(rows),'UI glyphs')
