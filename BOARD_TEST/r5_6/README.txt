R5.6 STRESS-NO-LOCATOR
仅更新 ELF 和上位机；继续使用 R5.5 bit。
已加载 R5.5 bit 时无需重烧 FPGA。兼容 bit 的 SHA256 见 manifest.json。
测量使用 ti60_r5_6_debug.elf；发行对照使用 ti60_r5_6_release.elf。
萝卜压力模式取消定位轮廓、中心标记、渐隐和近身索引；先画角色，后画弹幕。
普通游戏保留定位轮廓。数量 99999、小尺寸、声音、按键和双视频功能保持。
新的实际耗时和稳定弹数增益尚未上板测量。旧日志只能用于有条件的预算估算。
重启上位机后页脚为 R5.6-STRESS-NO-LOCATOR，压力模式回报 locator_on=0 locator_us=0。
Linux: bash Start_Control_Panel.sh --port 8765
Windows: Start_Control_Panel.cmd
BSP 路径仍是原工程 PROJECT/par/ddr_demo_ti60/embedded_sw/soc/bsp。
详细说明：docs/R5_6_LOCATOR_OFF.md

Use the new ELF with the unchanged R5.5 bitstream. No RTL or timing changes.
Carrot stress renders the actor first and projectiles afterwards, with no locator.
Normal gameplay is preserved. Actual saved time/FPS gains await a board A/B run.
