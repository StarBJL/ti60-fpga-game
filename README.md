# Ti60 R5：RISC-V + FPGA 2D 游戏渲染引擎

**R5.3 软件更新：** [完整说明与 R2 对照](docs/R5_3_SETTINGS_MUSIC.md)。全 10 首切换、人物轮廓缓存、自动演示循环优化与更明确的加速状态。新 ELF 在 `BOARD_TEST/r5_3`，继续配套已验证的 R5.2.1 bit；上板性能恢复幅度待复测。完整包见 `dist/Ti60_R5_3_MUSIC_PERF.tar.xz`。

**2026-10-24 R5.2 音视频硬件基线： [BOARD_TEST/r5_2](BOARD_TEST/r5_2) 的新 bit / ELF。** 修复 HDMI 音频子包位序、同步边沿；加入加速状态遥测及恢复按钮。见 [问题证据、修复和复测步骤](docs/BOARD_FIX_R5_2.md)。下文 R5 数字及根目录旧二进制是历史基线，不代表 R5.2 的实际板上验收。最新 Linux / Windows 源码上位机页脚应显示 `R5.2-BOARD-FIX`；原预编译 Windows EXE 是旧版。

[English](README_EN.md) · [按键与接线 HTML](docs/INPUT_GUIDE.html) · [照片方向接线图](docs/WIRING_DEMO.html) · [赛题对应说明](docs/CONTEST_MAPPING_ZH.md) · [待填实板测试表](docs/BOARD_TEST_PLAN.csv)

本工程用于全国大学生嵌入式芯片与系统设计竞赛易灵思 **赛题二：基于 RISC-V 与 FPGA 异构架构的 2D 图形渲染加速引擎**。Sapphire RISC-V 执行游戏逻辑，定制 BitBlt 引擎通过 AXI4 直接读写 DDR3，完成图块拷贝、纯色填充、透明色去背与 Alpha 混合，HDMI 输出图像及声音。

R5 基于 R4/R3 的完整工程，保留 R19 移植的七级萝卜人物、双枪、升级、主动技能和遮挡定位，以及东方弹幕、方块、图片、中文 UI 和音频。新增 Windows 适配的上位机、矩阵/CH9350L 输入、物理 720p60 / 1080p30 切换，以及针对片上 RAM 和读取额度关键路径的优化。

**两种输出模式都在 1280×720 画布渲染。1080p30 是 3:2 最近邻放大到 1920×1080，不是原生 1080p 渲染。** 当前 74.375 MHz 像素 PLL 对应约 60.101 / 30.051 Hz。固件为 1080 模式保留独立 60 Hz 游戏逻辑调度；主循环模型已验证 30 Hz 输出与 60 Hz 逻辑，但真实负载上限和显示器兼容性仍由上板测试确定。

## 最终 Ti60 目标实现结果

Efinity 2026.1 已完成最终 map、interface、布局布线和 bit 生成。报告中的全部受约束时钟关系 setup / hold 为正；核心保持 **200 MHz**。

| 指标 | 历史 R2 / R4 | 最终 R5 | 变化 |
|---|---:|---:|---:|
| XLR | 55,848 / 60,800 | **58,518 / 60,800（96.25%）** | +2,670 |
| 片上 RAM block | 253 / 256 | **253 / 256（98.83%）** | 总量不变，仍余 3 |
| DSP | 32 / 160 | **32 / 160** | 不变 |
| core 200 MHz setup slack | +0.053 ns | **+0.118 ns** | 改善 0.065 ns |
| core 200 MHz hold slack | +0.025 ns | **+0.028 ns** | 改善 0.003 ns |

Blitter 两处浅队列确实释放 **4 块 RAM**，新显示扫描缓存使用 3 块，CH9350 UART RX FIFO 使用 1 块，因此整机是 `253 − 4 + 3 + 1 = 253`。这是为新增功能腾出空间，不是整机 RAM 使用下降 4 块。通用 Alpha 仍是每拍两像素，本轮没有声称已扩展为 4/8 像素 Alpha。

最终 `ti60_r5.bit` 为 **2,811,948 B**，SHA-256：`25dc85470b3f73f929ef75c60eb06880a7c46533f30c26820a40eb5def85ac83`。该结果证明本次受约束目标实现通过，不等于已测得板上最大稳定元素数、温度、功耗或外设兼容性。所有实板验收仍待用户填写。

## 取文件与运行

交付文件集中在 `BOARD_TEST/`，以 [manifest.json](BOARD_TEST/manifest.json) 中的版本、SHA-256 和验证状态为准。

| 文件 | 用途 |
|---|---|
| `BOARD_TEST/ti60_r5.bit` | 含双输出模式、输入外设与本轮硬件修改的 R5 位流 |
| `BOARD_TEST/ti60_r5_debug.elf` | 完整测量、屏上 CPU/HW 对照、诊断键；优先用于赛题验收 |
| `BOARD_TEST/ti60_r5_release.elf` | 精简统计版本；用于对比统计开销 |
| `BOARD_TEST/manifest.json` | 交付文件哈希、源码与构建验证状态 |
| `docs/INPUT_GUIDE.html` | 离线可用的 16 键布局、接线、协议、全部快捷键指南 |

先加载配套 bit，再加载 ELF。R4 及更早位流不含 R5 新输入与分辨率功能；R5 固件通过 `VID5` / `INP5` 能力检查保护相关入口。不要把 R19 的 185 MHz / 640×480 位流与本版 ELF 混用。工程构建脚本只生成文件，不自动烧写设备。

Windows 可直接解压 **`dist/Ti60_R5_Windows_Console.zip`** 并双击其中的 **`Start_Ti60Control.cmd`**，使用已构建的独立 EXE，无需另装 Python。源码版双击工程根目录 **`Start_Control_Panel.cmd`**；Linux 运行 **`./Start_Control_Panel.sh`**。源码启动需要 Python 3.10+ 和 pyserial，启动器可建立工程内隔离环境。浏览器连接本机串口服务，选择开发板日志串口、115200 baud，再点击连接。打开页面不会自动发送控制指令。原 `Open_Serial_Log.cmd` 已转到新入口。

Windows 原生 COM 口、UTF-8 中文日志和中文/空格路径已适配。[真正的 Windows CI](https://github.com/18971355314bjl-gif/ti60-fpga-game/actions/runs/39987343020) 已在 Windows Server 2025 x64 / Python 3.12.10 上通过原生 CMD 启动、6 项移植性测试、EXE 构建与打包后 HTTP/UI 资源检查。预编译程序位于 `dist/windows/Ti60Control.exe`，证据在 `verification/r5/windows_console/windows_ci_validation.json`。**用户 Windows 10/11 的实际 USB 串口驱动、键盘和真实板卡仍待验收**。需重新打包时在 Windows 运行 `Build_Windows_Control_Panel.cmd`，详见 [使用与打包说明](docs/windows_zh-en.md)。

## 操作与容量

| 功能 | 控制方式 / 范围 |
|---|---|
| 四个场景 | 东方、萝卜、运动方块、图片；网页选择或 `n` |
| 移动 / 射击 / 技能 | 浏览器或 CH：WASD/方向、Z/空格、X/J、Shift 低速、Enter 确认、Esc 暂停 |
| 数量步长 | **100 / 10 / 1**；F6 / F7 / F8，矩阵 S11 循环 |
| 增减目标数量 | `+ / -` 或网页按钮；上位机分开显示目标和实际活动数量 |
| 萝卜敌弹容量 | 自定义压力 **1—40,000**；尺寸 9 / 13 / 17 / 25 px |
| 东方敌弹容量 | 压力模式 **1—16,384**；尺寸 75 / 100 / 125 / 150%；密度与容量独立 |
| 方块容量 | **1—30,000**；混合 6—40 px 或固定 6 / 8 / 12 / 16 / 24 / 32 / 40 px |
| HDMI 输出 | F9、矩阵 S15 或上位机按钮切 720p60 / 1080p30 |
| 图形对照 | `g` CPU/HW；`e` Alpha；`v` VSync；debug `f` 统计、`c` 自动轮换 CPU/HW |
| 音乐 | 10 首原创像素风循环曲 + 23 个保留音效；`;` BGM，反斜杠下一首 |

容量是对象池和输入边界，**不保证最大数量仍能稳定 60 FPS**。尺寸档位只改变压力弹体显示，碰撞半径、伤害、速度和轨迹沿用既有玩法，激光宽度不受该档位影响。方块面积、Alpha 比例、CPU 逻辑和 DDR 竞争都会改变可持续数量。

赛题要求的 60 FPS 极限挑战应在 **720p 模式且 Alpha + Color Key 开启**时测量。1080p30 的物理新帧呈现上限约 30 FPS，不能用 CPU 的 60 Hz 逻辑或超过 30 的渲染循环数代替它。

## 外设与接线

矩阵是 4 行 × 4 列、8 根信号线；使用同一个 **1.8 V Bank 1B**。行接 J7 2/4/8/10，列接 J7 20/22/26/28。CH9350L 使用独立 **3.3 V UART**：模块 TX→J7 32、模块 RX←J7 34，模块另接经确认的 5 V 电源和地。J7 剩余 3.3 V 信号为 37/38/39/40。

这些数字是主板 **J7** 的针号。绿色 2×20 转接排针方向、矩阵排针线序须通过通断测量确认；不能按彩线颜色直接接。原理图和手册部分 J7 映射不同，文档已按原理图网络→球位交叉核对，并给出 Efinity GPIO 名称。

矩阵 S4 为 Fn，S11 选数量步长，S12 选尺寸，S15 切输出。CH9350L 配置下位机 USB Host、状态 2、115200 8N1，按固定 8 字节 HID 键盘报告解析。当前不支持状态 0/1 可变长帧和专用 NKRO。完整键位、鬼键抑制、热拔插边界、APB 地址与 C 接口见 [输入实现文档](docs/INPUT_DEVICES.md) 和 [HTML 图解](docs/INPUT_GUIDE.html)。

## 赛题为何由本工程实现

| 条款 | 实现位置 |
|---|---|
| RISC-V 游戏逻辑 | `game.c`、`th_game.c`、`th_pattern.c`、`th_stage2.c`、`demo.c` |
| COPY / FILL / Alpha / Color Key | `PROJECT/rtl/blit/blit_engine.v`；软件接口 `blit.c`、`gfx_hw.c` |
| AXI4 直接访问 DDR3、burst/FIFO | `blit_engine.v`、`blit_axi_wide.v`、仲裁器与 DDR3 控制器 |
| C 寄存器驱动与双缓冲 | `blit.h` / `blit.c`、`hdmi_fb.h`、`main.c` 帧提交与交换 |
| HDMI、实际呈现与重复帧计数 | `PROJECT/rtl/hdmi/hdmi_fb.v`、`hdmi_island.v` |
| 方块 / 图片基础 Demo、屏上 CPU/HW 对照 | `demo.c`、`main.c` HUD/测量窗口 |
| 高负载互动游戏、极限测量 | 游戏场景、100/10/1 与尺寸控制、统一遥测、日志导出 |

完整逐条映射、验收方式、技术细节和证据边界见 [中文说明](docs/CONTEST_MAPPING_ZH.md) / [English mapping](docs/CONTEST_MAPPING_EN.md)。本赛题最低显示要求为 **640×480 @ 60 Hz**；不要将其他赛题的最低参数混入验收。

## 工程结构与构建

```text
PROJECT/rtl/                         Ti60 RTL：BitBlt、HDMI、音频、输入、DDR
PROJECT/par/ddr_demo_ti60/            Efinity 工程、约束、IP 与 outflow
  embedded_sw/soc/software/standalone/game/
    src/                            RISC-V C 固件
    sound/                          音库和原创音轨
    game.ld                         DDR 程序/声音/美术区域
tools/control_panel/                Windows/Linux 本机串口上位机
tools/                              音乐/字库/启动配置生成工具
BOARD_TEST/                         配套 bit、ELF、哈希清单
docs/                               中英文设计/接线/赛题/优化/测试表
verification/r5/                    本轮离线验证与目标构建日志
verification/r4/                    继承的历史 R4 离线验证
logs/                               历史实板日志与后续采集目录
```

硬件目标是 **易灵思 Ti60F225**，最终硬件构建使用 **Efinity 2026.1**。Vivado 2021.2 仅用于可移植 RTL 的行为验证/分析，不能生成本器件可用 bit，也不能替代 Ti60 的 timing/resource 报告。

```bash
# 软件：需 Efinity RISC-V SDK、GNU make；默认生成 debug 和 release
python3 build_software.py --toolchain /path/to/efinity-riscv-ide/toolchain/bin

# 硬件：使用配套版本 Efinity，执行完整 map / place / route / bit 流程
python3 build_hardware.py --efinity /path/to/efinity/2026.1 --flow compile

# 工程移动或复制到 Windows 后重新生成 IDE 的绝对路径配置
python3 tools/configure_launch.py
```

两种显示模式仍使用 1280×720 RGB565 framebuffer，每帧 1,843,200 B，放在相隔 2 MiB 的 DDR 槽中。程序区域 15 MiB，音库区域 16 MiB，美术区域 32 MiB；字体和音乐占用的是这些 **外部 DDR** 区域。原生 1080 framebuffer 需约 3.96 MiB，不能直接塞进本版 2 MiB 槽中。详见 linker 与显示实现。

## 验证、优化和后续实板指标

[资源与时序优化报告](docs/optimization_zh.md) / [English report](docs/optimization_en.md) 说明队列 RAM 调整、`rf_res` 读取额度关键路径及 Alpha 后续路线。最终目标实现数见上表；完整证据为 [Efinity 资源报告](PROJECT/par/ddr_demo_ti60/outflow/ddr_demo_ti60.place.rpt) 和 [时序报告](PROJECT/par/ddr_demo_ti60/outflow/ddr_demo_ti60.timing.rpt)，Vivado 行为验证是独立证据。

R5 离线验证包括输入 C 适配器（ASan/UBSan）、矩阵/UART RTL、HDMI 双模式行为、主循环 30 Hz 输出 / 60 Hz 逻辑模型、Windows CI 原生启动/EXE 构建及静态服务检查、Linux PTY 浏览器回归。HTML 指南使用真正浏览器检查离线加载、Fn、步长、数量、分辨率演示、搜索、明暗主题和桌面/390 px 窄屏布局。各项日志保留在 `verification/r5/`；完整结果以其中的记录及 manifest 为准。

历史 R2 日志和 R4 主机模型只作基线，不作为 R5 新成绩。新板卡数据由用户补入 [BOARD_TEST_PLAN.csv](docs/BOARD_TEST_PLAN.csv)：该表已列出启动、双模式、矩阵、CH9350L、Windows、玩法、尺寸、Alpha、CPU/HW、最大负载、音乐、长稳和资源检查，测量值与结论均留空。上位机自动保存原始 UART、配置事件 JSONL 和同序号完整测量窗口 CSV。

实体输入还输出 `I5`：设备能力、保持态、键盘报告、截断帧、rollover、鬼键、状态报告与 UART 错误，便于与上板操作对应。CH9350L 状态帧应答已实现，模块兼容性仍需实测。

建议报告实际活动数、render FPS、logic Hz、present FPS、scan/repeat Hz、视频/音频 UF 增量、最大工作时间、引擎错误、温度和功耗。扫描频率不是新画面帧率，目标数量也不是实际活动数量。

## 来源与许可

`docs/references/` 保留赛题、板卡和 CH9350 协议资料及 SHA-256；第三方文档、Efinix IP/SDK、字体和原有素材保留各自许可。Noto 字体许可位于 `ART/UI_R4/Noto_CJK_LICENSE.txt`。原创循环曲由 `tools/create_original_music.py` 生成；曲目清单和 WAV 在游戏 `sound/original_r4/`。本工程用于用户授权的私有仓库协作，不给整个第三方工程重新套用一个开放源码许可证。


## GitHub 完整工程还原

仓库保持私有。源码、IP、图片、音乐、bit、ELF、Windows EXE、测试与日志按原文件保存。仅 3 个超出 GitHub REST 接口接受范围的生成文件采用无损 gzip 归档；原始字节、大小、SHA256 和 Git blob SHA 均记录在 `tools/large_files_manifest.json`，数据位于 `.github_large_files/`，没有省略这些文件。两份约 29 MB 的 `game.hex` 仍直接按原文件保存。

从 GitHub 克隆或下载后，在工程根目录运行：

```sh
python3 tools/restore_large_files.py
python3 tools/restore_large_files.py --verify
```

Windows 使用 `py -3 tools/restore_large_files.py` 与 `py -3 tools/restore_large_files.py --verify`。还原工具只用 Python 标准库；默认不会覆盖已经修改的文件，校验正确的文件直接跳过。确定需要覆盖本地修改时才使用 `--force`。本地完整交付文件夹已经包含原始大文件，可以直接 `--verify`。

归档仅对应 `PROJECT/par/ddr_demo_ti60/outflow/ddr_demo_ti60.map.v`（55,888,638 字节）、调试 `game/build/game.bin` 与发布 `game/build_release/game.bin`（各 43,673,856 字节，完整路径见清单）。运行还原后，这些文件回到原来的路径及可执行权限。上传方式与自动审批拒绝导入工作流的记录见 `verification/github_upload.txt`；最终方案通过普通 GitHub API 上传及本地恢复，不安装工程导入自动化。
