#!/usr/bin/env python3
# 检查 Efinity 综合网表里引擎 (u_blit) 的寄存器有没有被 retiming 挪动。
#
# 背景（2026-10-04 板上黑屏）：工程开着 retiming=1 时，综合把 rd_words / rf_res
# 这两个计数器（x <= x + 加量 - 1位标志）往加法器前面挪，拆成 "~FF_rt_NN" 寄存器，
# 但 "-m_rvalid / -rf_pop" 那一位（取反后进加法器）复位值算错了：复位后
# rd_words = 0x7FF、rf_res = 0x3FF，rd_room / rf_ok 永远为 0，引擎一个读都发不出，
# 第一条 COPY 就卡死（PERF_RB=0）。RTL 仿真看不出来，只有网表能看到。
# 现在工程里 retiming 已设为 0；以后改了综合选项或换版本，重新综合后跑一下：
#
#   python3 check_retime.py [网表路径]
#   默认网表：../../par/ddr_demo_ti60/outflow/ddr_demo_ti60.map.v
#
# 输出 OK 表示 u_blit 里没有 retiming 过的寄存器；否则列出来并返回 1。
import os, re, sys
from collections import Counter

here = os.path.dirname(os.path.abspath(__file__))
path = sys.argv[1] if len(sys.argv) > 1 else os.path.join(
    here, '..', '..', 'par', 'ddr_demo_ti60', 'outflow', 'ddr_demo_ti60.map.v')
src = open(path, encoding='utf-8', errors='replace').read()
src = re.sub(r'/\*.*?\*/', '', src, flags=re.S)

groups = Counter()
risky = Counter()
for m in re.finditer(r'EFX_FF\s+(\\\S+)\s*\(', src):
    name = m.group(1)
    if '~FF_rt_' not in name:
        continue
    base = re.sub(r'\[\d+\]~FF_rt_\d+$', '', name)
    groups[base] += 1
    if re.search(re.escape('defparam ' + name + ' .D_POLARITY') + r"\s*=\s*1'b0", src):
        risky[base] += 1

blit = {k: v for k, v in groups.items() if k.startswith('\\u_blit/')}
other = {k: v for k, v in groups.items() if not k.startswith('\\u_blit/')}
print('netlist:', os.path.normpath(path))
for k, v in sorted(other.items()):
    print('  (outside u_blit) retimed %-50s %d FF' % (k, v))
if not blit:
    print('OK: no retimed registers in u_blit')
    sys.exit(0)
for k, v in sorted(blit.items()):
    print('  RETIMED %-50s %d FF%s' % (k, v,
          ('   <-- %d with inverted D (reset value suspect)' % risky[k]) if risky[k] else ''))
print('FAIL: u_blit registers were retimed; set retiming = 0 (Project > Synthesis) and rebuild')
sys.exit(1)
