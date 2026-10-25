Ti60 R5.9 INPUT-CAL / 200MHz / 2026-10-25
New software, unchanged R5.8 bitstream. No FPGA board was programmed by this update.
下载 ti60_r5_9_debug.elf；release 关闭完整统计，仅供对照。
已使用R5.8 bit者不用重刷；ti60_r5_8_unchanged.bit是原R5.8文件的逐字节副本。
启动横幅：TI60-R5.9-INPUT-CAL-20261025；上位机：R5.9-INPUT-CAL。
默认IDE路径 build/game.elf 已同步为debug版本；版本化launch固定使用本目录。
BSP：PROJECT/par/ddr_demo_ti60/embedded_sw/soc/bsp（200MHz，未修改）。
接线更正：docs/R59_INPUT_FIX.html；手册与原理图冲突，实际S7=0x4000与手册吻合。
与实测吻合的候选：行J7 20/22/2/4，列14/16/26/28。断电改线并核对J28转接关系。
S15短按仅状态；长按2秒恢复720P。只检测按键时不会触发游戏或矩阵视频切换。
按顺序S1到S16校准，录满并松开后应用；浏览器保存映射，重载ELF后手动载入。
USB：看keyboard_bytes、keyboard_headers、keyboard_reports、keyboard_faults与keyboard_tail。
键盘侧D2闪不能证明UART已通。USB诊断及隔离模块后的32/34回环方法见HTML。
Linux: bash Start_Control_Panel.sh --port 8765 ; Windows: Start_Control_Panel.cmd
Git clone: python tools/restore_r59_package.py
720P display/RTL/PLL and BSP timing unchanged. 1080P monitor compatibility and
physical USB reception remain unverified. Do not treat host/PTY tests as board tests.
