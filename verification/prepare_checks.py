from pathlib import Path
import shutil
v=Path(__file__).parent;src=Path('D:/FPGA_competition/Ti60_720p_SQUARE20K_MERGE/verification')
for name in ['test_motion.py','tb_motion.sv','test_square_draw.py','test_cpu_motion.py','rv32_probe.py','test_present.sv','test_motion_arb.sv']:
 shutil.copy2(src/name,v/name)
f=v/'tb_motion.sv';x=f.read_text().replace('wr(16,20001)','wr(16,30001)');f.write_text(x)
f=v/'test_motion.py';x=f.read_text();x=x.replace("(20000,2,0,'dense')","(20000,2,0,'dense'),(30000,1,0,'random')");f.write_text(x)
f=v/'test_square_draw.py';x=f.read_text()
print(x[x.index('for case'):x.index('for case')+180] if 'for case' in x else x[-900:])
font=Path('D:/FPGA_competition/Ti60_11_Dongfang_185MHz_R12_SKILL_ZH/PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/tools/test_r12_font.c')
x=font.read_text(encoding='utf-8').replace('#include "../src/gfx.c"','#include "../PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/gfx.c"')
(v/'test_font.c').write_text(x,encoding='utf-8')
