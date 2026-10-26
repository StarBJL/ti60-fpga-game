# R6 入口

- Windows 免安装上位机：`dist/windows/Ti60Control.exe`，或解压 `dist/Ti60_R6_Windows.zip` 后双击 EXE。
- Linux 上位机：工程根目录 `bash Start_Control_Panel.sh --port 0`。
- 配套 bit／ELF：`BOARD_TEST/r6/`；文件校验值与真实资源／时序结果见该目录的 `manifest.json`。
- 完整说明：[中文](README.md)、[English](README_EN.md)。
- IDE 工程名 `game_r6`；默认下载 `build/game.elf`。也可运行 `python3 tools/configure_launch.py` 生成明确指向 R6 交付 ELF 的配置。

本版本保留原 720P 显示链路。1080P30 的实屏兼容和各负载十分钟稳定上限仍待上板验证。
