# Ti60 FPGA 游戏与图形加速器 · R6

R6 在 R5.9 三人游戏和输入校准版本上，合入 `09_Ti60F225_hardjtag_demo` 的帧流水、数据布局、全局 Alpha 快速通路、填充表和东方特效。保留两个游戏的同屏三人合作、压力模式 99999 容量、尺寸档位、十首原创音乐、中文界面和现有接线。

**720P 显示 RTL、PLL、引脚约束和时序约束与 R5.9 配套硬件相同。** 本次唯一修改的硬件源码是 `PROJECT/rtl/blit/blit_engine.v`。采用真实 200 MHz 核心时钟，没有用新增伪路径或放宽周期换取通过。1080P30 原有功能和试切换恢复仍保留；PANDA PH27QB2 的实屏兼容问题尚未由这次修改解决，不能把仿真恢复通过视为显示器已通过。

## 直接运行和下载

- Windows：解压 `Ti60_R6_Windows.zip` 后双击 `Ti60Control.exe`。程序内含 Python 和 pyserial，自动打开本机浏览器，不需要安装 Python。选串口并点击连接之后才打开设备；默认自动选空闲网页端口。关闭控制台窗口或 Ctrl+C 结束。
- Linux：在工程根目录运行 `bash Start_Control_Panel.sh`。也可显式指定 `--port 8765`；如果旧服务还在运行，先关闭旧服务，或使用 `--port 0` 自动选端口。
- 配套硬件：`BOARD_TEST/r6/ti60_r6.bit`。
- 带完整指标的软件：`BOARD_TEST/r6/ti60_r6_debug.elf`。`ti60_r6_release.elf` 减少测量开销，用于展示；请不要把两者的测试数据混在一组。
- 新 IDE 工程名为 `game_r6`。导入 `PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game` 的现有工程；常规编译输出和下载路径为 `build/game.elf`，不指向旧 R5 工程。BSP 根位置在 `PROJECT/par/ddr_demo_ti60/embedded_sw/soc`，具体板级文件位于其 `bsp/efinix/EfxSapphireSoc`。
- 移动工程后，可运行 `python3 tools/configure_launch.py` 生成指向配套预编译 ELF 的启动配置。此脚本只写配置，不下载硬件。
- 启动串口横幅应包含 `TI60-R6-PIPE-KBLEND-FTAB-20261026`。状态中的 `kblend=1 ftab=1` 表示检测到本次硬件功能；文件校验值见 `BOARD_TEST/r6/manifest.json`。

## 本次实现

| 部分 | 实现与边界 |
|---|---|
| 帧流水 | 四个帧缓冲区分别避开当前显示、已排队翻页和引擎在绘制的缓冲。等待前一翻页的动作移到真正提交下一翻页之前；完整且未拆分的分带帧允许跨帧重叠。VSync 下每圈最多发一次翻页，可能增加一帧延迟。 |
| 萝卜热数据 | 敌弹 `Seed` 为 16 字节，原版为 24 字节；`active` 存放伤害，0 表示空槽。保留玩家弹所有权、三人碰撞、NOVA、升级及压力模式不自动开枪、不计算定位轮廓的规则。 |
| 东方热数据 | 16 字节热数组保存每帧位置、速度和绘制索引，32 字节冷数据保存转向、加速等低频字段。新发射弹在发射器补完参数后同步；删除、清弹和尺寸重建都读取当前热位置。三人擦弹位保存在独立保留标志位。 |
| 坐标写入 | 对齐且带 `may_alias` 的 32 位存储发布 XY，避免 RV32 编译器把 4 字节 memcpy 拆成四个字节写。 |
| 全局 Alpha | 分带 K 通路支持每拍八像素的全局 Alpha FILL、缓存 COPY 和加法混合。新增 DSP 实现分别计算源、目的加权项，保持完整 Alpha 精度与四级流水冒险检查。逐像素 A8 仍走原通路。 |
| FTAB | 新增指令号 **7**、CAPS2 bit5；每项 8 字节，包含 XY、宽高、颜色和 Alpha。原 SQTAB 指令号 **6**、CAPS2 bit3 和 MOT2 保留。超过 65535 项分成多条表指令，分带容量溢出时结束分带并按原顺序连续绘制。 |
| 方块对照 | 保留 Q4 运动、原八色和奇偶透明图案。CPU＋FTAB 使用三份不可变 DMA 表；CPU＋SQTAB、MOT2＋SQTAB 和逐项提交可直接切换。配置变化的额外建表先排空引擎，避免覆盖在读表。 |
| 东方特效 | 第一关增加三档出生淡入、限量 A8 呼吸光晕及发射闪光；第二关流星头部和拖尾批量提交。两关压力模式独立选择，压力第二关流星按倍率增加并为光束保留槽位。普通两关仍支持三人合作。 |
| 音乐 | 保留 10 首原创循环曲和 23 个音效。BGM 改为 22050 Hz、16 位单声道，混音器仍输出原有 HDMI 音频格式。音库从 10,123,520 降到 7,124,736 字节，曲目时长和循环点校验通过。 |

小量绘制写指针使用引导完成后空闲 OCR 的 `0xF9000C00` 起 144 字节；链接脚本明确保留 512 字节区域，游戏栈仍在 DDR。它没有增加片上 RAM 块。

## 当前资源和时序

| 指标 | R6 最终实现 |
|---|---:|
| 真实核心时钟 | 200 MHz |
| 像素时钟 | 74.375 MHz，原值 |
| XLR | 59,147 / 60,800（97.28%） |
| RAM 块 | 253 / 256（98.83%） |
| DSP | 80 / 160（50.00%） |
| 核心 setup slack | +0.259 ns |
| 核心 hold slack | +0.026 ns |
| 全部 14 个 setup/hold 关系 | 通过 |
| 18 组 Gray 总线偏斜 | setup/hold 均通过；该检查不等于完整 CDC 功能证明 |

初次合入八像素 Alpha 的候选占用 60,603 XLR、56 DSP；后续使用更多 DSP 的等价算术减少了 1,456 XLR。与原 R5 硬件 58,661 XLR、32 DSP 相比，最终增加 486 XLR、48 DSP，换取新增功能；RAM 块数保持相同。

编译后的 DDR 分区占用和每个文件的 SHA-256 以发布清单为准。`99999` 是对象容量上限，**不表示任意尺寸、透明度和场景下都能达到 60 FPS**。本次不提供未经实板测得的帧率提升百分比或十分钟稳定上限。

## 操作与测试

上位机保留音量增减、十首音乐选择、100/10/1 步进、原 R2 负载预设、三人入口、矩阵校准和 USB 原始字节诊断。新增：

- 东方第一关／第二关压力入口。
- VSync 帧流水开／关。
- Alpha 八像素、原两像素通路和关闭 K 通路对照。
- 方块 CPU＋FTAB、硬件运动＋SQTAB、CPU＋SQTAB、逐项提交。
- CSV 记录 `vpipe`、`kmode`、`ftab`、`square_path`、`th_stage`，便于固定条件比较。

P1 使用上位机键盘，P2 为 CH9350L USB 键盘，P3 为 4×4 矩阵。接线和 S1–S16 功能见上位机的离线接线页 `/wiring`；本次没有更换引脚或交换 TX/RX。三人启动仍先确认退出输入检测，防止检测模式吞掉游戏按键。

验证记录在 `verification/r6`，包括 2,105,344 组 Alpha/ADD 输入组合、99999 方块真实 RTL、24 组 FTAB/SQTAB 画面与表缓冲比较、两关单人／三人各 4800 tick 的逻辑等价检查、输入 RTL、720P 原显示链路回归及桌面／手机浏览器 PTY 测试。Windows EXE 在原生 Windows CI 运行过启动、HTTP 资源、中文路径及不自动连接串口检查。**这些是仿真、模型和构建证据，不是 FPGA 实板帧率。**

建议上板先用配套 debug ELF，以原尺寸和数量复测，然后分别只改一个开关。记录 720P60 下实际呈现帧率、逻辑频率、video/audio 欠流、窗口最大耗时、分带拆分和十分钟运行结果；再切换 release ELF验证展示稳定性。1080P30 实屏兼容单独记录，不混入 720P 性能结果。

## 构建

从 GitHub 克隆后，先运行 `python3 tools/restore_large_files.py` 恢复当前版本的音库、配套 bit／ELF 和生成网表；所有分块及原文件均校验 SHA-256。源码交付 ZIP 已含完整音库，不需要恢复。旧的 build/outflow 编译产物已从当前 Git 树清理，历史版本保留在 Git 历史和版本化 BOARD_TEST 目录。

Linux 或 Windows 安装 Efinity 2026.1 和配套 RISC-V 工具链后：

```text
python3 build_software.py --stats both
python3 build_hardware.py --flow compile
```

可用 `--toolchain` 或 `--efinity` 指定安装位置，Windows 下按本机 Python 命令替换 `python3`。音库重建在 game 目录执行 `python3 tools/wav2bank.py --list sound/r6_sounds.txt`。工程自带可直接使用的音库和所有引用的 ROM 初始化文件。硬件构建不会自动上板，只有通过完整时序检查的 bit 才复制到 BOARD_TEST。

私密源码仓库：[StarBJL/ti60-fpga-game](https://github.com/StarBJL/ti60-fpga-game)。Windows 原生构建：[run 40329064648](https://github.com/StarBJL/ti60-fpga-game/actions/runs/40329064648)。
