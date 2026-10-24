Ti60 R5.2.1 board repair — 2026-10-24
1. Load ti60_r5_2.bit in the Efinity Programmer.
2. Load/run ti60_r5_2_debug.elf in Efinity RISC-V IDE.
   The included .launch uses the existing Linux project at /home/user/fpga_contest/1025/Ti60_R5_DUALVIDEO.
   On another host, choose this ELF and your BSP paths manually.
3. Restart the source console; footer: R5.2-BOARD-FIX.
4. Check V5 video_rev=328193 (0x00050201).
5. Restore acceleration; test 720P sound first, then low-load 1080P30 and bars.
Original R5 files remain in the parent BOARD_TEST directory as a rollback baseline.
This package passed simulation and target timing; real-board audio/display/performance retest is pending.
See docs/BOARD_FIX_R5_2.md for evidence and retest procedure.

中文版：先下载本目录的新 bit，再运行配套 debug.elf。只换 ELF 不会修复 HDMI 电路。
上位机重启后点击“恢复加速配置”。先测 720P 的测试音/BGM，再测 1080P30和彩条。
原 R5 交付文件保留在父目录。新包尚未上板确认，不将仿真结果当作实板稳定上限。
