# Ti60 R5 工程入口 / Project entry

**当前使用 R5.9 输入修订版。** 下载`BOARD_TEST/r5_9/ti60_r5_9_debug.elf`，bit沿用R5.8；[修正后的矩阵接线与USB诊断](docs/R59_INPUT_FIX.html)。矩阵建议行J7 20/22/2/4、列14/16/26/28，依据与实测一致的手册，先核对实板。旧图针脚冲突已明确标出。上位机版本应为R5.9-INPUT-CAL。

最新R5.8入口：[显示恢复、双键盘、三人启动说明](docs/R58_BOARD_FIX.html)。使用`BOARD_TEST/r5_8`配套bit+ELF。以下保留历史版本入口。

当前候选 **R5.7**：先从`BOARD_TEST/r5_7`使用200MHz配套bit+debug ELF，上位机需重启。202.083333/205子目录是独立候选，禁止混用bit/ELF。详见[三人、性能、显示及下载说明](docs/R57_REPORT.html)。720P显示链路保留；1080P试切10秒未确认返回720P；480P未实现。下面是历史R5.6入口。

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
