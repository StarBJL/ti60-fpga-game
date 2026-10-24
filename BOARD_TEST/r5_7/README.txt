Ti60 R5.7 COOP-PERF / 2026-10-24
先使用本目录200MHz的ti60_r5_7.bit + ti60_r5_7_debug.elf做对照。
子目录202p083333和205各自含对应bit/ELF；不可混用。
202候选实际202.083333MHz，BSP使用202083333Hz。旧R5.5/R5.6保留。
启动应显示TI60-R5.7-COOP-PERF-20261024；Clock actual与firmware相等。
新bit带CLK7时钟ID，频率不匹配时固件停止；不要只改SDC。
默认BSP: PROJECT/par/ddr_demo_ti60/embedded_sw/soc/bsp
默认IDE build/game.elf为200MHz。OC请用本目录预编译ELF或tools/build_clock_variant.py生成独立工程。
各档.launch都指向固定的版本ELF，并禁用启动前自动重编译。
GitHub克隆后先运行python tools/restore_r57_package.py恢复精确bit/ELF；本地已有完整文件。
普通游戏: P1串口/PC，P2 USB经CH9350L，P3矩阵板；压力模式仍单人。
720P原显示链路保留。1080P试切10秒不确认自动回720P；F10/Fn+S16确认。
1080P用户屏幕黑屏尚未定位；先测硬件彩条。480P尚未实现。
上位机重启后页脚R5.7-COOP-PERF。
Linux: bash Start_Control_Panel.sh --port 8765
Windows: Start_Control_Panel.cmd （原生EXE见GitHub Actions本次构建产物）
详细中英说明: docs/R57_REPORT.html；按键/接线: docs/INPUT_GUIDE.html。

Use matching bit/ELF profiles. Static timing and functional tests pass; no new
board throughput, thermal, keyboard-module or monitor results are claimed.
The 720p display pipeline is preserved. 1080p gets a confirmation timeout,
not a claimed blackout fix. 480p remains a design proposal.
