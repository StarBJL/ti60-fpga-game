from pathlib import Path
from fontTools import subset
from fontTools.ttLib import TTFont
import shutil
R=Path(__file__).resolve().parents[1];P=R/'tools/control_panel';out=P/'fonts';out.mkdir(exist_ok=True)
chars=set(chr(i) for i in range(32,127))
for p in P.glob('*'):
 if p.suffix in ['.html','.js','.py']:
  chars.update(p.read_text())
f=TTFont('/usr/share/fonts/opentype/noto/NotoSansCJK-Medium.ttc',fontNumber=2)
options=subset.Options();options.flavor='woff2';options.desubroutinize=True
sub=subset.Subsetter(options=options);sub.populate(text=''.join(chars));sub.subset(f);f.flavor='woff2';f.save(out/'ui-v6.woff2')
shutil.copy2('/usr/share/doc/fonts-noto-cjk/copyright',out/'FONT_LICENSE.txt')
print('font characters',len(chars),'bytes',(out/'ui-v6.woff2').stat().st_size)
