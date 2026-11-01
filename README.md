# Ti60 FPGA Game · V6 · 225 MHz

**中文** · [English](README_EN.md) · [上板运行](#deployment) · [12 步讲解稿](docs/EXPLANATION_GUIDE.md) · [源码追踪](docs/RENDERING_WALKTHROUGH.md)

在易灵思 Titanium Ti60F225 上，把 RISC-V 软件中的游戏状态变成 HDMI 画面与声音：CPU 负责规则和绘图命令，FPGA 图形引擎负责合成，DDR 保存帧缓冲与素材，HDMI 电路按自己的节拍输出视频和音频。

当前仓库以 **Ti60_V6_225M_CLEAN_20261101** 为工程基线，内容直接放在仓库根目录，沿用原工程相对路径。硬件、软件、素材、上位机和配套下载文件属于同一份交付。版本与下载文件校验值见 [CURRENT_RELEASE.json](CURRENT_RELEASE.json)。

> **当前基线：** 225 MHz 核心、1280×720 RGB565 帧缓冲、48 kHz 双声道音频。压力预设表示请求负载；稳定帧率需要对应条件下的实板记录。当前版本的时序与软件验证范围见[第 11 章](#validation)。

阅读顺序：[项目目标](#goals) → [演示功能](#demo) → [总体架构](#architecture) → [时钟与数据流](#clocks) → [软件帧循环](#frame-loop) → [图形引擎](#graphics) → [提交、字体与 DDR 优化](#ddr) → [HDMI 音视频](#hdmi) → [输入与上位机](#input) → [游戏与素材](#assets) → [性能和验证](#validation) → [部署与后续改进](#deployment)。

<a id="goals"></a>
## 1. 项目目标

这个工程回答一个具体问题：**按下按键之后，角色的位置、技能和声音，怎样一步步成为显示器上的像素和扬声器中的采样？**

软件保存游戏状态和绘制顺序，硬件承担大量重复的拷贝、透明混合、写回和定时扫描。二者通过内存映射寄存器、DDR 命令列表、完成计数和 FIFO 协作。

目标器件为 `Ti60F225`，Efinity 时序模型为 `I3`。交付配置采用 225 MHz Sapphire RV32IM 系统、RGB565 帧缓冲、1280×720 HDMI 输出和 48 kHz 双声道音频。225 MHz 是核心配置频率；实际游戏呈现速度还受软件、DDR 和显示扫描共同约束。

<a id="demo"></a>
## 2. 演示功能

| 演示 | 可以观察什么 | 对应系统能力 |
| --- | --- | --- |
| 萝卜合作战役 | 六个场景、1～3 人、角色成长、Boss、大招、末关无尽波次 | 游戏逻辑、精灵与文字合成、事件音效 |
| 萝卜弹幕压力测试 | 固定尺寸、目标弹数、屏幕边缘裁剪 | 稳定绘制表、TAB、提交耗时与实际呈现 |
| 东方风格弹幕 | 独立的单人玩法、弹幕、透明特效与场景音乐 | 共用图形接口、音频通路与负载调节 |
| 方块压力场景 | 大量矩形及半透明叠加 | FTAB 填充表、分带混合、带宽压力 |
| 图片切换 | RGB565 图片和整屏背景 | 大块拷贝、帧交换、扫描输出 |
| 上位机实验台 | 场景切换、输入控制、音量、统计和日志 | 串口协议、遥测、对照实验 |

萝卜游戏及其弹幕压力测试、方块、图片使用原生 1280×720 绘制。东方风格弹幕保留 640×480 逻辑布局，可绘制为居中的 960×720 画面，两侧各补 160 像素装饰区；切换该绘制视图不会重新配置 HDMI 物理输出时序。

![三人大招渲染预览](docs/images/gameplay_preview.png)

预览与[大招演示视频](docs/demo/compact_ultimate_fx.mp4)来自当前 C 渲染器的主机侧验证程序，采用固定角色位置展示特效。它们展示素材和绘制结果，不是实板 HDMI 录屏，也不提供实板帧率证据。图片演示使用四张图片循环扫描，双龙位于第四张。

建议先用 Release ELF 体验游戏，再用 Debug ELF 观察性能。这里的 Debug/Release 主要区别是 `STATS=1/0` 测量开关；[构建脚本][build]为两者都使用优化构建 `DEBUG=no`。

<a id="architecture"></a>
## 3. 总体架构

```mermaid
flowchart TD
    IN["矩阵键盘 / CH9350 键盘 / PC 串口"] --> CPU["Sapphire RISC-V · 游戏状态"]
    CPU --> DRAW["gfx 绘图接口 · 命令列表 / APB"]
    DRAW --> BLIT["BitBlt · 缓存 · 16 行分带合成"]
    BLIT --> DDR["DDR3 · 帧缓冲 / 图形 / PCM"]
    DDR --> VIDEO["视频 DMA · 异步像素 FIFO"]
    CPU --> EVENT["声音事件 · 声部寄存器"]
    EVENT --> MIX["16 声部 PCM 混音器"]
    DDR --> MIX
    MIX --> AF["异步音频 FIFO"]
    VIDEO --> HDMI["视频编码 + HDMI 音频数据岛"]
    AF --> HDMI
    HDMI --> TX["10:1 TMDS 串行输出"]
    TX --> OUT["显示器 / HDMI 音频"]
```

视频和声音共享游戏状态与 DDR，但各自持续消费数据。软件一帧的绘制时间可以变化；HDMI 的行、帧和音频采样节拍必须连续，因此需要帧缓冲、完成确认和异步 FIFO。

| 部分 | 阅读入口 | 职责 |
| --- | --- | --- |
| 顶层 | [ddr3_example_top.v][top] | 连接 SoC、DDR、APB 外设、DMA 和 HDMI |
| 软件调度 | [main.c][main] | 初始化、输入、逻辑、绘制、呈现和统计 |
| 绘图前端 | [gfx.c][gfx]、[gfx_hw.c][gfx-hw]、[blit.c][blit-sw] | 同一绘图接口下的 CPU 与硬件后端 |
| 图形硬件 | [blit_engine.v][blit-rtl] | 命令执行、精灵缓存、分带合成 |
| 视频 | [hdmi_fb.v][hdmi-fb] | 帧地址交接、DDR 扫描、FIFO、视频时序 |
| 音频 | [audio_mixer.v][mixer]、[hdmi_island.v][island] | PCM 混音、采样节拍和 HDMI 数据包 |

APB 入口：HDMI `0xF8100000`，输入集线器 `0xF8100080`，BitBlt `0xF8101000`，混音器 `0xF8102000`。寄存器承载控制信息；像素、样本和大批量命令通过 DDR 传输。

<a id="clocks"></a>
## 4. 时钟与数据流

| 时钟域 / 节拍 | 当前配置 | 主要使用者 |
| --- | --- | --- |
| 板载参考 | 25 MHz | PLL 输入 |
| `core_clk` / `user_clk` | 225 MHz | CPU、图形引擎、DMA、混音器、输入逻辑 |
| `pll_fb_clk` | 100 MHz | DDR 控制器和 SoC 内存侧 |
| `sdram_clk` | 400 MHz | 16 位 DDR3 物理接口，双沿传输 |
| `hdmi_tx_slow_clk` | 74.375 MHz | 像素计数、数据岛和音视频编码 |
| `hdmi_tx_fast_clk` | 371.875 MHz | 半速率 DDR 串行器，10 bit / 像素 |
| 游戏逻辑目标 | 60 tick/s | 游戏更新、软件音量渐变 |
| 音频输出 | 48,000 个立体声采样帧/s | HDMI 音频消费端 |

依据为 [PLL 配置][peri]、[SDC 约束][sdc]、[BSP 时基][soc-h]及顶层参数。部分源码头部仍保留早期的 200 MHz、25 MHz 或 640×480 注释，理解当前实现时应结合实际传参。

视频总时序为 **1650×750**：水平 `1280+110+40+220`，垂直 `720+5+5+20`，所以扫描频率为 `74,375,000 / (1650×750) ≈ 60.101 Hz`。

软件时基由 BSP 的 `SYSTEM_CLINT_HZ=225000000` 派生：`TICKS_PER_US=225`，目标逻辑周期为 `225000000/60=3750000` tick。若实际计数器运行在 225 MHz，却用 200 MHz 换算，同一段耗时会被放大为真实值的 1.125 倍；这种换算误差也会污染 FPS 和带宽统计。核心频率调整后必须同时核对 BIT、BSP、软件和上位机报告的 `core_hz`。

SoC 的 AXI 跨域通路连接 225 MHz 系统与 100 MHz 内存侧。视频和音频分别通过异步 FIFO 进入像素域；帧边界、状态和复位也需要同步。

| DDR 地址 | 用途 |
| --- | --- |
| `0x00001000` 起，15 MiB | 程序、普通数据、BSS、栈 |
| `0x01000000` / `0x01200000` / `0x01400000` / `0x01C00000` | 四帧缓冲；每幅 1280×720×2 = **1,843,200 B** |
| `0x01600000` / `0x01800000` / `0x01A00000` / `0x01E00000` | 图片演示缓冲 |
| `0x02000000` 起，16 MiB 区域 | `.sound` PCM 声音库 |
| `0x03000000` 起 | 分带命令 bin；每带预留 `2^18` B |
| `0x04000000` 起，32 MiB | `.artbss` 运行时素材，启动后解压或生成，ELF 中为 NOLOAD |
| `0x06000000` 起，16 MiB | `.bg_assets` 压缩图形素材，由 ELF 下载 |

完整链接布局见 [game.ld][linker]，帧缓冲选择见 [main.c][main]。

<a id="frame-loop"></a>
## 5. 软件帧循环

[main.c][main]中的一帧按以下阶段推进：

1. **选择可写缓冲。** 排除 HDMI 正扫描的 `FB_CUR`、最近请求显示的 `last_req`、引擎尚未完成的 `pend_fb`。四块缓冲的职责随运行轮换。
2. **输入与逻辑。** 各输入源转换成统一动作；`run_logic()`推进游戏、碰撞、升级及声音事件。
3. **描述画面。** `th_render()`、`render_game()`或演示函数调用 `gfx_*`。CPU 后端直接写像素，硬件后端生成有序命令。
4. **结束提交。** `gfx_end()`结束列表或分带帧。命令已交出，不等于 DDR 中画面已完成。
5. **确认完成。** 普通路径等待引擎空闲；分带流水用 `blit_band_done()`确认对应序号的帧已写回。
6. **请求显示。** `present()`在需要时等上一次翻页生效，`hdmi_flip()`执行 memory fence 并写 `FB_ADDR`。
7. **硬件交接。** HDMI 在垂直同步脉冲结束的帧边界更新 `FB_CUR`、清除 `flip_pending`，预取下一帧。

```mermaid
flowchart LR
    A["可写缓冲"] --> B["CPU 生成命令"]
    B --> C["引擎合成并写回 DDR"]
    C --> D["完成计数确认"]
    D --> E["FB_ADDR 请求翻页"]
    E --> F["帧边界更新 FB_CUR"]
    F --> G["扫描输出；旧缓冲可复用"]
```

**默认开启逻辑重叠，实际顺序是：** 读输入 → 提交当前状态的画面 → 在引擎绘制期间执行下一次逻辑更新 → 等待应当呈现的帧完成 → 请求翻页。关闭 `overlap` 才采用“先逻辑、后绘制”的串行顺序；两条路径不能混用同一套延迟解释。

默认 `VPIPE=1`。一帧若是完整且未被列表回绕拆开的 `BANDS…ENDB`，CPU 可准备下一帧，引擎同时处理上一帧。另一个 `overlap` 开关允许先提交当前状态的画面，再在引擎运行时执行逻辑更新。这些重叠改变输入到画面的延迟，完成确认和缓冲归属仍必须正确。

逻辑目标 60 tick/s、扫描约 60.101 Hz、渲染循环速度随工作量变化。低帧率时主循环会调整落后的 tick 计划，不会无限追赶所有错过的 tick，因此三种频率要分开观察。

<a id="graphics"></a>
## 6. 图形引擎

[BitBlt 引擎][blit-rtl]支持 RGB565 填充与拷贝、透明色键、全局 alpha、A8 alpha 和加法混合。CPU 可逐条写 APB，也可生成 DDR 命令列表；`SLOT/DRAW`复用精灵描述，`TAB`读取精灵表，`FTAB`批量处理矩形。

分带模式先按命令触及的水平带归类，再按原顺序合成：

```text
一帧命令 → 45 个带的命令 bin → 每次合成 1280×16 像素 → 连续写回 DDR
```

片上窗口只有 `1280×16×2 = 40,960 B`，无需容纳整幅 1.84 MB 帧缓冲。每带从黑色开始，背景必须明确绘制；多层混合在窗口内完成后再写回。跨带精灵进入多个 bin，因此命令归类也有开销。

| 路径 | 典型工作 | 作用 |
| --- | --- | --- |
| 128 位宽路径 | 满足对齐和尺寸条件的不透明拷贝 / 填充 | 每拍传 8 个 RGB565 像素 |
| K 路径 | 分带内缓存精灵、填充、支持的全局 alpha / 加法混合 | 并行处理 8 像素，减少逐精灵 DDR 写入 |
| 窄路径 | A8 等不满足快速路径条件的操作 | 保留完整混合功能 |
| 16 KiB 源缓存 | 重复使用的小精灵、字形 | 命中时无需读取 DDR 源数据 |

源缓存为 512 条、每条 32 B，直接映射。CPU 改写源图后需显式失效缓存；引擎不自动监听 CPU 写入。它与 CPU 的 16 KiB 指令缓存、32 KiB 数据缓存是不同部件。

### 6.1 批量列表与硬件读表有何区别

`gfx_blit_list()`虽然按屏内和裁剪段组织对象，但它的 `copy_list` 后端仍在 CPU 上逐项展开 `DRAW` 或寄存器写入。`gfx_blit_table()`则由硬件读取稳定的 `GfxItem` 数组，提交开销主要是精灵槽描述和表地址；它并不消除后续读表、分带与像素处理成本。

`draw_seeds()`保留整表完全在当前目标内时的一次 TAB 快速路径。存在裁剪项时，`gfx_blit_table_clipped()`按原顺序处理：

```text
原顺序：A、B 屏内；C 需要裁剪；D、E 屏内
提交为：TAB(A,B) → 原单项裁剪 C → TAB(D,E)
```

判断使用当前 `gfx.width/height`和每个精灵的实际尺寸，完全出屏的对象仍由原裁剪流程忽略。每个屏内段分别累计像素数，继续通过 `gfx_blit_table()`检查模式、能力与地址范围，必要时回退；有 `copy_table` 函数指针并不等于所有操作都支持 TAB。全局 Alpha 使用相应的 `*_alpha` 路径，A8 掩码不能直接当作 TAB 自带能力。

TAB 异步读取原表切片，提交函数返回后不能立即改写。当前游戏使用三张轮换的 `seed_draw` 表，配合主循环的完成序号等待保护生命周期；旧的 `seed_list[128]`循环临时表仍走列表展开。详见[源码追踪：裁剪与生命周期](docs/RENDERING_WALKTHROUGH.md#ordered-tab)。

<a id="ddr"></a>
## 7. 提交、字体与 DDR 优化

### 7.1 从分阶段耗时定位瓶颈

先判断时间花在逻辑更新、CPU 提交、队列等待、引擎尾部还是同步翻页，再选择优化对象。TAB 减少 CPU 展开；源缓存减少重复源读取；分带窗口减少目标颜色的 DDR 读改写。三者作用不同，增加其中一项不保证整帧按相同比例变快。

### 7.2 当前字体路径

| 文字内容 | 当前绘制方式 | 关注点 |
| --- | --- | --- |
| ASCII 字母、数字与符号 | 原版 5×7 点阵；小倍率色键图集，大倍率合并笔画填充 | 保留低成本路径，并非全部改成 A8 |
| 中文 | 预生成中文字形、A8 掩码与纯色平面 | 柔和边缘需要对应 Alpha 操作 |
| 重复整串文字 | 硬件后端可复用标签小图；纯 ASCII 保留色键，含中文使用 A8 | 不能分配或不符合条件时退回逐字绘制 |

语言切换保留完成等待与缓存重置；翻译缓存按字符串内容检索并校验，避免同一临时地址换了文字后仍使用旧翻译。新增界面文字同样使用这些通路。缓存首次构建、颜色和倍率变化、标签更新以及硬件排队都可能影响耗时，不能把一次 HUD 峰值全部归因于字形绘制。

**测量边界：** `hud_us`覆盖 HUD、提示及相关界面绘制区间，也可能包含其中的等待；当前 `C4`里的 `font_hit`、`font_miss`、`label_hit`等字段仍固定输出 0，尚不能提供实际命中率。具体入口见 [gfx.c][gfx]与[字体追踪](docs/RENDERING_WALKTHROUGH.md#text-path)。

### 7.3 DDR 访问形态

优化围绕访问形态展开，见 [blit_axi_wide.v][wide]、[blit_axi_rd_arb.v][arb]和引擎队列控制。

- **提前打包为 128 位。** 普通 32 位数据在 SoC 跨域 FIFO 之前打包；宽路径直接传 128 位。写地址通常等完整 burst 数据进入下游 FIFO 后释放，并保留提前发地址的回退逻辑。
- **短而连续的 burst。** 最多 64 个 32 位 word 对应 16 个 128 位 beat，传输不跨 256 B 块边界。
- **先供给扫描。** 视频优先于音频，二者共享读通路又优先于图形引擎普通读请求。已选择但未握手的请求保持选择，保证 AXI 信号稳定。
- **限制在途读取。** 默认 `LIMITW=64`，即普通读路径限额为 64 个 32 位 word；宽路径另有控制。限制引擎排队量可缩短后来 CPU 访问的等待。
- **减少重复读写。** 片上分带完成目标混合，缓存复用源图，列表降低逐寄存器提交成本。

CPU 取指、游戏状态、音频和视频都要访问 DDR；加深所有队列并不必然提升整体速度，需要结合队列等待、缓存命中和欠流计数调节。

按当前扫描时序，仅读取可见帧缓冲约需 **110.78 MB/s**。若每次扫描都生成完整新画面，写回还需约 110.78 MB/s，尚未计背景源读取、精灵、bin、CPU 和音频。16 位 DDR3 在 400 MHz 双沿传输下的物理理论值是 1.6 GB/s，刷新、行切换、读写转换和仲裁会降低有效带宽。

<a id="hdmi"></a>
## 8. HDMI 音视频

**一帧画面出板。** [hdmi_fb.v][hdmi-fb]对当前帧缓冲发起 DMA，把 RGB565 word 放入异步 FIFO。720p 配置使用 2048×32 bit 像素 FIFO。像素域按行列计数取像素、扩展 RGB 分量，生成视频区、消隐和同步，再进行 TMDS 编码。四组 10:1 输出分别承载时钟与三路数据。

**一段声音出板。** 射击、受伤、Boss 或 BGM 切换产生事件，[game_sound.c][game-sound] / [th_sound.c][th-sound]调用 [audio.c][audio-sw]，查找声音库条目并设置声部地址、长度、格式、音高、左右音量和循环点。

```text
游戏事件 → 声部 KEYON → DDR PCM → 插值 / 音量 / 累加 / 饱和
        → 16 位立体声 FIFO → 48 kHz 消费 → HDMI 音频包 → 显示器音频
```

[混音器][mixer]有 16 个声部，支持 s8 单声道、s16 单声道和 s16 立体声。每声部有两条 32 B 样本缓存，用 4.12 步长和线性插值适配源采样率。声部 0、1 用于 BGM，其余用于音效；软件控制切歌、淡出，硬件持续逐采样混音。

混音器按 FIFO 水位补样本，默认目标 64 个立体声采样帧，约 **1.33 ms**。这只是目标排队深度，完整声音延迟还包括事件调度、取样和显示器处理。

[hdmi_island.v][island]在消隐期发送 Audio Sample Packet、ACR、AVI InfoFrame、Audio InfoFrame，并生成 BCH 校验、TERC4 和保护带。当前 `N=6144`、`CTS=74375`：

```text
fs = 74,375,000 × 6144 / (128 × 74,375) = 48,000 Hz
```

音频采样率因此不随软件帧率改变。软件慢时画面可以重复，硬件仍继续混音和发包；供给不足时，应检查视频 `UF_CNT`、音频 `AUD_R_UF`和混音器统计。

### 8.1 当前 HDMI 同步修复位流改了什么

当前下载文件采用 `ti60_v6_225m_hdmi_audio_sync_fix.bit`。RTL 与[构建记录](docs/evidence/hdmi_sync_build.json)对应两处输出修复：

- 音频采样包 SB6 按 `{P_R,C_R,U_R,V_R,P_L,C_L,U_L,V_L}`组织两声道的校验、状态、用户与有效性位。
- 对外 VSYNC 使用与 HSYNC 边沿对齐的 `vs_tx0`，内部帧缓冲交接和 DMA 仍使用原垂直时序。

这些修复不把 48 kHz 音频改成另一采样率。记录中的 TMDS 对照覆盖一整帧 720p 彩条及双声道 1 kHz 测试音；“与 R7 一致”只适用于该仿真范围，该记录未包含接收端实板音频验收结果。

<a id="input"></a>
## 9. 输入与上位机

[input_hub.v][input-rtl]负责矩阵扫描与 CH9350 UART，[input_devices.c][input-sw]解析按键快照和 `57 AB` 键盘报告，经 [input_link.h][input-link]转换成游戏动作。矩阵输入包含去抖、鬼键抑制和逐键映射校准。

| 人数 | 输入归属 |
| --- | --- |
| 1 人 | P1 合并 PC 串口 / 上位机、USB 键盘、矩阵键盘 |
| 2 人 | P1 使用 PC；P2 合并 USB 与矩阵键盘 |
| 3 人 | P1 使用 PC；P2 使用 CH9350 USB 键盘；P3 使用矩阵键盘 |

矩阵常用键：S2/S5/S6/S7 移动，S3 确认 / 开始，S8 大招，S9～S11 升级选项，S12 人数，S13 暂停，S14 重开确认，S16 冲刺或弹幕慢速。完整定义见 [matrix_keys.def][keys]；Fn+S12、Fn+S15 当前保留但未实现。

上位机由本地 Python HTTP 服务和浏览器组成，经 **115200 baud** 串口发送 `:id command [value]`加换行的命令，展示实际呈现率、逻辑 / 提交 / 等待耗时、音视频欠流，并保存日志和 CSV。服务绑定 `127.0.0.1`、默认自动选空闲端口，启动后在页面选择串口并连接。

- Windows：双击 [Start_Control_Panel.cmd](Start_Control_Panel.cmd)，使用便携 Python；默认日志在 `%LOCALAPPDATA%\Ti60V6\logs`。
- Linux：`bash Start_Control_Panel.sh`，需要 Python 3.10+，使用工程内 pySerial。
- 在上位机打开接线和按键页面。接线说明记录了厂商 J7 表的差异，应结合[引脚配置][peri]及逐键诊断核对实际接线；说明引用的历史 PDF 未随此精简工程提供。

上位机负责控制与观测；ELF 下载后，板上的游戏、渲染和 HDMI 音视频可自行运行。

### 9.1 225 MHz 压力预设

[r2_presets.py][presets]根据原预设按 `原值 × 1.1`向下取整到 100 的整数倍；整数实现为 `原值 * 11 // 1000 * 100`。例如缩放后为 11111，保存为 11100。25 px 萝卜弹幕随后单独调整为 **11,000**，不再使用公式算出的 9,000。

| 萝卜弹幕尺寸 | 目标数量 | 方块尺寸 | 目标数量 |
| ---: | ---: | ---: | ---: |
| 1 px | 16,200 | 1 px | 35,200 |
| 3 px | 16,000 | 3 px | 34,100 |
| 6 px | 15,600 | 6 px | 33,000 |
| 9 px | 15,400 | 8 px | 31,900 |
| 13 px | 15,000 | 12 px | 30,800 |
| 17 px | 14,900 | 16 px | 29,700 |
| 25 px | 11,000 | 24 px | 20,900 |
| — | — | 32 px | 13,800 |
| — | — | 40 px | 9,700 |
| — | — | 随机 6–40 px | 20,100 |

这些是**配置目标**。敌弹池容量为 32,768，方块容量为 35,200；实际活跃量和稳定呈现率要在运行中记录。225/200=1.125 也不能推导出整机性能提高 12.5%，预设采用的 1.1 更不是实测加速比。

应用预设后，先确认命令 ACK，再核对新测量窗口中实际报告的 `core_hz`、场景、尺寸、数量、`band/cache/overlap`等状态。`M4/F4/P4/C4`用相同 `seq`关联一个窗口，`epoch`帮助识别配置变化；不要将切换前后的数据拼成一次结果。

<a id="assets"></a>
## 10. 游戏与素材

### 10.1 六场景成长与无尽模式

| 场景 | 等级区间 | 场景 Boss 家族 |
| --- | --- | --- |
| MOSS COURT | 1–3 | ANCIENT ROOT KING |
| JADE PALACE | 4–5 | JADE QILIN |
| THUNDER CITY | 6 | THUNDER ENGINE |
| EMBER PALACE | 7 | EMBER PHOENIX |
| ASTRAL SANCTUM | 8 | ASTRAL ORACLE |
| TWIN DRAGONS | 9 起 | TWIN DRAGON KING，并混入既有家族与终极 ECLIPSE EMPEROR |

前五关的有效战役时间预算为 60 秒，最早过关门槛为 40 秒；满足 Boss 击败和成长条件后才能推进，超时进入失败。时间来自匹配 225 MHz 的计数器，`PLAYING`和`UPGRADE`阶段计入，暂停或不可见时不累加。上位机可直接进入指定场景，初始化对应等级与能力，便于演示。

末关按 50 秒划分波次，混入前面场景的敌人及 Boss，周期性出现终极 Boss；敌人强度与角色能力继续成长。小怪上限从 156 增长到 360，同时至多保留两只 Boss，增长受到对象池和数值上限约束。

当前预留 384 个敌人槽、每玩家 160 个子弹槽，敌方弹幕最多 32768 槽供压力场景使用。这些是容量配置，不代表每个场景同时使用的数量或满负载帧率。规则与容量见 [game.c][game]、[game.h][game-h]、[sprout_config.h][config]。

### 10.2 三职业、升级与大招

| 角色 | 普攻与被动 | 当前大招表现 |
| --- | --- | --- |
| 橙色射手 | 更快移动、远程多弹成长 | 发光龙波、火焰和冲击拖尾 |
| 蓝色战士 | 短距离圆周伤害与飞刃；近战命中打开短反弹窗口，受次数与冷却限制 | 剑齿虎分阶段展开，配合环形剑气 |
| 粉色法师 | 多弹法术；满足无伤间隔与冷却后补充护罩层数 | 凤凰分阶段展开，配合附近樱花落点与环形冲击 |

升级池包含射速、伤害、移动、生命、拾取、多弹、大招强度、范围、冷却与职业被动共十类。普通战役会过滤已达上限的选项，随机抽取；仍有有效大招升级时保留一个大招候选，再打乱卡片顺序。伤害与冷却由逻辑计算，发光纹理和粒子本身不直接承担碰撞判定。

蓝虎与凤凰由母图在初始化时生成 `160×90 → 224×126 → 288×162 → 320×180`四个稳定阶段，运行时调整位置、透明度、展开与收尾。当前采用收缩后的效果与局部伤害范围，早期全屏概念图不能代表最终战斗画面。见 [ultimate_spirits.c][spirits]、[carrot_fx.c][carrot-fx]和 [game.c][game]。

### 10.3 图形与声音资源

![V6 大招头像素材预览](assets/v6/ultimate_portraits_source.png)

*随工程提供的 V6 大招头像素材预览。*

| 素材 | 离线阶段 | 板上阶段 |
| --- | --- | --- |
| 背景、角色、大招、字体 | PNG / RGBA → RGB565、色键或 A8 → 必要时 LZ4 → C 数据 | ELF 下载只读素材；启动后解压 / 生成到 `.artbss`，绘图命令引用 |
| BGM、音效、配音 | WAV + [sounds.txt][sounds] → `wav2bank.py` → `sound_bank.bin` / `.S` | ELF 下载到 `0x02000000`，驱动按名字查条目，混音器读取 PCM |

图形脚本在 [tools](tools/)，包括 `build_campaign_art.py`、`build_v5_art.py`、`build_v6_art.py`、`build_ultimate_spirits.py`。声音转换器在[软件工程 tools][sound-tools]。当前生成数据已保留，普通编译无需重跑素材转换；修改源图 / WAV 时才重跑对应脚本并重建 ELF。

声音库含 39 个条目，其中角色大招 3 段、Boss 入场 7 段；声音条目数量与混音器的 16 个并发声部是不同概念。当前配音素材使用 22.05 kHz 单声道 PCM，由混音器转换到 48 kHz 双声道输出。配音整合保留原采样点数与事件时长，减少字幕、入场保护和伤害阶段错位。

语音调度同一时间只保留一段旁白：同时释放大招时轮换选取发声角色，三个动画仍可同时出现；Boss 入场语音优先，并压低 BGM。结束、切场景或技能失效时停止对应语音，不排队补播已经结束的招式。调度入口见 [game_sound.c][game-sound]。

`assets/v4`、`v5`、`v6`是当前素材的来源目录，不是多个可选硬件工程。第三方运行时和字体的许可证随文件保留。

<a id="validation"></a>
## 11. 性能和验证

先区分“算得快”与“显示了新画面”。HDMI 约 60.101 Hz 扫描可以重复同一缓冲；解除同步后渲染 FPS 也可能高于实际呈现 FPS。

| 指标 | 回答的问题 |
| --- | --- |
| `render_fps` / `logic_hz` / `present_fps` / `scan_hz` | 提交画面、更新游戏、换上新画面和扫描各有多快 |
| `logic` / `submit` / `qstall` / `tail_wait` / `vsync_wait` | CPU、提交、队列、引擎尾部、翻页各花多久 |
| 缓存命中、读写 beat、分带 phase / flush | DDR 和片上合成开销在哪里 |
| 视频 / 音频欠流、bin 溢出、混音 clipping | 输出连续性、命令容量和音量是否正常 |
| 最大工作时间、超过 16.7 ms 的帧 | 平均帧率之外是否存在突发卡顿 |

Debug ELF 提供启动测速、详细统计和对照开关；Release ELF 关闭大部分测量开销。对比 CPU/硬件、缓存、分带和流水时，应固定场景、输入、人数、压力档并记录 BIT/ELF 哈希。

### 11.1 指标应如何解释

- Debug 的 `submit`从提交区间中扣除了 `qstall`，Release 的简化统计将排队等待计入提交。比较两个 ELF 时先统一口径。
- `present_fps`来自软件观察 `FB_CUR`变化，表示采样到的新缓冲呈现；高吞吐、未同步运行时，这不是外部仪器对每一帧的完整捕获。
- `hud_us`不是纯字体耗时；字体命中字段目前是占位零值。不能用这些字段得出“字体缓存没有命中”的结论。
- `8 像素/拍 × 225 MHz`是特定内部路径的理想处理率。DDR 等待、源读取、分带归类、A8 与 CPU 提交都会影响整帧，不能写成实测持续吞吐。

### 11.2 可重复的板上测量流程

1. 固定 BIT/ELF 哈希、显示器、人数、场景、尺寸、透明效果和音频状态；选择 Debug ELF 取得分阶段统计。
2. 应用上位机预设并核对 ACK 与实际状态。切换后丢弃旧窗口，等待画面与活跃数量稳定。
3. 用相同窗口的 `seq/epoch`记录目标数、实际数、逻辑/渲染/呈现/扫描速率、最大工作耗时，以及音视频欠流增量。可从预热 5 秒、记录 60 秒开始，明确写下实际时长。
4. 比较一个开关时保持其他条件相同。尤其观察弹幕到达边缘后的 `submit`，不要只记录开场几秒。
5. 用[性能记录模板](docs/PERFORMANCE_RECORD_TEMPLATE.csv)保存结果。模板中的待测行是占位示例，不是测量数据。

### 11.3 现有证据与范围

验证材料按硬件时序、HDMI 仿真、软件回归和产物一致性组织。下表列出当前版本附带的结果及适用范围；稳定帧率和接收端兼容性应结合对应环境的实板记录判断。

| 已有记录 | 记录的结果 | 范围 |
| --- | --- | --- |
| [时序摘要](docs/evidence/timing_summary.json) | 核心 setup +0.063 ns、hold +0.012 ns；列出的关系均通过 | 225 MHz 选定构建的摘要，余量较小；完整工具报告未打包 |
| [HDMI 同步构建](docs/evidence/hdmi_sync_build.json) | 1,237,587 个周期的 TMDS 对比与参考 R7 一致 | 一整帧 720p、彩条和双声道 1 kHz 测试音；该摘要不含实板音频验收记录 |
| [软件本机测试](docs/evidence/software_native.json) | 4 项测试、87 帧渲染、400 秒无尽模式、ASan/UBSan | 主机侧验证，`physical_board_tested=false` |
| [软件交付核验](docs/evidence/software_payload.json) | 当前源码摘要、两份 ELF 哈希、384 敌人槽及素材保持情况 | 对应当前软件交付；`physical_board_tested=false` |

`verified_225`是原交付目录名。已有摘要中的绝对路径指向原构建环境；本仓库可用的下载入口是 `BOARD_TEST/verified_225/`。上文时钟、容量和理论带宽来自配置与计算，已有摘要不构成所有压力档持续 FPS 或所有 HDMI 显示器兼容性的保证。

<a id="deployment"></a>
## 12. 部署与后续改进

```text
.
├── README.md / README_EN.md       中英文架构与部署说明
├── CURRENT_RELEASE.json          BIT / ELF 信息与 SHA-256
├── BOARD_TEST/verified_225/       1 份 BIT + Debug / Release ELF
├── rtl/                          DDR、图形、HDMI、音频、输入 RTL
├── par/ddr_demo_ti60/             Efinity 工程、引脚、约束、SoC IP
│   └── embedded_sw/soc/           RISC-V 游戏、BSP、驱动、链接脚本
├── assets/                       当前图像、配音母版及派生素材
├── docs/                         随工程提供的演示资料与验证摘要
└── tools/                        素材工具、构建工具、上位机和运行时
```

**使用交付文件：**

1. 获取完整仓库并保持相对路径，准备 Ti60F225 板、JTAG、HDMI 显示设备及输入设备；访问私有仓库时，克隆账号需要相应权限。
2. 使用 Efinity 2026.1 系列工具，工程记录版本为 `2026.1.132.4.5`。在 Efinity Programmer 加载 [225 MHz BIT][bit]。
3. 通过 Efinity RISC-V IDE 完整下载 [Release ELF][release-elf]用于演示，或 [Debug ELF][debug-elf]用于测量。包含全部可加载段，尤其是 `0x02000000`声音区、`0x06000000`图形区；`.artbss`在启动后构建。
4. 运行程序，确认 HDMI 画面和声音，再用上位机连接 115200 串口检查身份、分辨率、输入及欠流。
5. 按 [CURRENT_RELEASE.json](CURRENT_RELEASE.json)核对下载文件，使用同一份 225 MHz BIT/ELF 组合。

首次运行可依次检查：启动信息中的时钟与分辨率 → 静态图片和音频 → 单人控制 → 多人游戏 → 固定压力档位。若无声，先查声音库条目是否已加载、音量/静音、混音器和 `AUD_R_UF`；不要直接把 CPU 频率当成音调控制参数。

**重建源码：** 硬件打开 [ddr_demo_ti60.xml][project]；软件在 RISC-V IDE 导入 `par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game`下的现有工程。也可从仓库根目录运行：

```bash
python3 tools/build_verified.py --toolchain /path/to/efinity-riscv-toolchain --profile 225
```

工具链目录应含 `bin/riscv-none-elf-gcc`，或直接指向编译器所在目录，系统还需 `make`。Windows 使用对应 Python、编译器和 make 环境运行同一脚本。精简仓库未携带编译中间目录；IDE 中指向 `build/game.elf`的启动项需要先构建，直接体验请用 `BOARD_TEST/verified_225/`下的成品。脚本生成 `build/`、`build_release/`并同步两份 ELF 到 `BOARD_TEST/verified_225/`，不烧录硬件。自行改动或重建后应重新核对并更新发布清单的哈希。

配套说明：[源码追踪与设计取舍](docs/RENDERING_WALKTHROUGH.md)、[现场讲解稿](docs/EXPLANATION_GUIDE.md)、[性能记录模板](docs/PERFORMANCE_RECORD_TEMPLATE.csv)、[About 文案](docs/GITHUB_ABOUT.txt)。讲解稿适合演示与答辩，源码追踪补充实现细节，性能模板用于保存可比较的实验条件和结果。

后续改进沿数据流展开：保存完整 225 MHz 时序和资源报告；补充帧交换、AXI 背压和音频跨域的可重复仿真；用固定输入记录逐帧呈现和欠流；降低命令 bin 与源素材带宽；在更多 HDMI 接收端验证时序、ACR 和音频；完善资产生成依赖和接线资料，使新机器上的部署更容易复现。

[top]: rtl/ddr3_example_top.v
[project]: par/ddr_demo_ti60/ddr_demo_ti60.xml
[peri]: par/ddr_demo_ti60/ddr_demo_ti60.peri.xml
[sdc]: par/ddr_demo_ti60/sdc/ddr3.sdc
[soc-h]: par/ddr_demo_ti60/embedded_sw/soc/bsp/efinix/EfxSapphireSoc/include/soc.h
[main]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/main.c
[gfx]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/gfx.c
[gfx-hw]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/gfx_hw.c
[blit-sw]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/blit.c
[blit-rtl]: rtl/blit/blit_engine.v
[wide]: rtl/blit/blit_axi_wide.v
[arb]: rtl/blit/blit_axi_rd_arb.v
[hdmi-fb]: rtl/hdmi/hdmi_fb.v
[island]: rtl/hdmi/hdmi_island.v
[mixer]: rtl/audio/audio_mixer.v
[audio-sw]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/audio.c
[game-sound]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/game_sound.c
[th-sound]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/th_sound.c
[input-rtl]: rtl/input/input_hub.v
[input-sw]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/input_devices.c
[input-link]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/input_link.h
[keys]: tools/control_panel/matrix_keys.def
[game]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/game.c
[game-h]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/game.h
[config]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/sprout_config.h
[linker]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/game.ld
[sounds]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/sound/sounds.txt
[sound-tools]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/tools/
[build]: tools/build_verified.py
[bit]: BOARD_TEST/verified_225/ti60_v6_225m_hdmi_audio_sync_fix.bit
[debug-elf]: BOARD_TEST/verified_225/ti60_verified_225_debug.elf
[release-elf]: BOARD_TEST/verified_225/ti60_verified_225_release.elf


[render]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/render.c
[presets]: tools/control_panel/r2_presets.py
[spirits]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/ultimate_spirits.c
[carrot-fx]: par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/carrot_fx.c
