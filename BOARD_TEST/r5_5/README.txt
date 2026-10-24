R5.5 PRESSURE-99999 / 200 MHz

先加载 ti60_r5_5.bit，再运行 ti60_r5_5_debug.elf；发行版为 ti60_r5_5_release.elf。
新 bit 允许 1px / 3px 方块硬件运动。旧 bit 配新 ELF 时，小方块只用 CPU 运动。
萝卜压力停止玩家自动枪火和技能弹；正常游戏枪火/升级/技能保留。
萝卜自定义、东方压力、方块容量 99,999。容量不代表稳定 60 帧上限。
弹体新增 1/3/6px；方块新增 1/3px；100/10/1 步进保持。
先在上位机用 R2 条件按钮复测基线，再增加数量；新的实际 FPS 尚需上板。
上位机必须重启服务，页脚应为 R5.5-PRESSURE-99999。
Linux: bash Start_Control_Panel.sh --port 8765
Windows: Start_Control_Panel.cmd，或本次 GitHub Actions 的 Ti60Control.exe。
BSP: PROJECT/par/ddr_demo_ti60/embedded_sw/soc/bsp，路径与之前一致。
详细实现、日志结论、DDR 布局和后续优化见 docs/R5_5_PRESSURE.md。

Load the paired bitstream before running the debug ELF for measurement.
All seven setup/hold clock relationships pass at 200 MHz. No timing exceptions
were added. Capacity, RTL and control tests passed; physical FPS is pending.
Release ELF disables detailed profiling. Keep test conditions equal across runs.
File hashes, source identity, exact timing and memory usage: manifest.json.
