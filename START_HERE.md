# Ti60 R5 工程入口 / Project entry

当前版本：**R5.6-STRESS-NO-LOCATOR**。使用 R5.5 bit + `BOARD_TEST/r5_6/ti60_r5_6_debug.elf`；已烧入 R5.5 bit 时只需换 ELF。上位机重启后显示压力轮廓关闭。见 [修改与收益估算](docs/R5_6_LOCATOR_OFF.md)。

请先阅读 [中文说明](README.md) 或 [English README](README_EN.md)。

- 上板文件：[BOARD_TEST](BOARD_TEST/START_HERE.txt)；必须使用配套 R5 bit + ELF。
- Windows：[独立上位机 ZIP](dist/Ti60_R5_Windows_Console.zip)；解压后运行 Ti60Control.exe。
- Linux：运行 `./Start_Control_Panel.sh`。
- 按键与接线：[交互 HTML](docs/INPUT_GUIDE.html)。
- 赛题要求与实现：[中文](docs/CONTEST_MAPPING_ZH.md) / [English](docs/CONTEST_MAPPING_EN.md)。
- 上板指标：[待填测试表](docs/BOARD_TEST_PLAN.csv)。

720p60 / 1080p30 是物理 HDMI 输出切换；内部保持1280×720渲染，1080p按3:2最近邻放大。
原R4工件仅在 `BOARD_TEST/legacy_r4/` 中存档，不能用于本版新功能验收。
