from pathlib import Path
root=Path(__file__).resolve().parents[1];g=root/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src'
f=g/'game.h';s=f.read_text(encoding='utf-8').replace('两张表轮流写','三张表轮流写');f.write_text(s,encoding='utf-8')
f=g/'main.c';s=f.read_text(encoding='utf-8').replace('if (sq_count < 16) sq_count = 16;','if (sq_count < 100) sq_count = 100;');f.write_text(s,encoding='utf-8')
f=root/'tools/serial_monitor.ps1';s=f.read_text(encoding='utf-8-sig')
s=s.replace('720P20K','720P-R2')
old="@(@('测试音 ,',','),@('总音量 .','.'),@('音库试听 /','/'),@('统计面板 f','f'),@('降档 -','-'),@('升档 +','+'),@('东方分辨率 #','#'))"
new="@(@('方块减100 ,',','),@('方块加100 .','.'),@('测试音 (','('),@('总音量 )',')'),@('音库 /','/'),@('统计 f','f'),@('降档 -','-'),@('升档 +','+'),@('第四档 4','4'))"
assert old in s;s=s.replace(old,new)
s=s.replace("$gameQuick.Controls.Count -ne 7 -or $gameQuick.Controls[0].Tag -ne ',' -or $gameQuick.Controls[1].Tag -ne '.' -or $gameQuick.Controls[2].Tag -ne '/'", "$gameQuick.Controls.Count -ne 9 -or $gameQuick.Controls[0].Tag -ne ',' -or $gameQuick.Controls[1].Tag -ne '.' -or $gameQuick.Controls[2].Tag -ne '('")
s=s.replace('nine square controls plus seven audio/display controls','nine square controls plus nine fine-load/audio controls')
s=s.replace('连接即自动保存。点击 @ 进入2万方块；& 对照；! 切换运动；% 切换表提交。','方块：逗号减100、句号加100；音频：左括号测试音、右括号音量。连接即自动保存。')
f.write_text(s,encoding='utf-8-sig')
print('R2 serial shortcuts and labels updated.')
