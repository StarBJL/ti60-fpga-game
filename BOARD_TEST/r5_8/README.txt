Ti60 R5.8 VIDEO-COOP / 200MHz / 2026-10-25
先加载ti60_r5_8.bit，再加载ti60_r5_8_debug.elf；release为关闭完整统计的对照版。
新的链路恢复需要新bit和新ELF。旧R5.7版本及超频候选保留。
默认BSP仍为PROJECT/par/ddr_demo_ti60/embedded_sw/soc/bsp。
上位机版本R5.8-VIDEO-COOP；启动串口TI60-R5.8-VIDEO-COOP-20261025。
三人入口：启动萝卜三人合作 / 启动东方三人合作；auto=0，players=3。
黑屏恢复：上位机按钮 / USB模块键盘F12 / 矩阵S15单独长按2秒。
1080P确认会取消自动回退；黑屏时不要确认保留。DVI诊断没有音频。
720P正常输出保留；PH27QB2实屏结果仍待验证，480P热切换未实现。
接线：docs/R58_INPUT_SCHEMATIC.html；问题报告：docs/R58_BOARD_FIX.html。
Linux: bash Start_Control_Panel.sh --port 8765
Windows: Start_Control_Panel.cmd
GitHub克隆后：python tools/restore_r58_package.py，恢复精确的bit/ELF。
Matched bit/ELF required. Static timing and functional tests passed; no physical
monitor recovery or new board throughput result is claimed.
