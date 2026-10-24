# Ti60F225 Sapphire SoC → HDMI：SproutRun，CPU 渲染 + BitBlt 硬件加速（赛题二）

目标：RISC-V（Sapphire RV32IM @100 MHz）用 CPU 把整帧画进 DDR 帧缓冲，
硬件 `hdmi_fb` 以 640×480@60 RGB565 扫描输出到 HDMI，测量"只用 RISC-V"能跑多少帧。
这就是后续 BitBlt 硬件加速器的对比基线。

> 当前版本已通过 Efinity 2026.1 综合、布局布线、时序分析和 bitstream 生成；
> 板上 HDMI、DDR 和游戏运行结果仍以实际硬件测试为准。

## 1. 硬件改动（rtl/ 与 par/）

| 文件 | 说明 |
|---|---|
| `rtl/hdmi/hdmi_fb.v` | 新增：640×480@60 时序、AXI 读 DMA（burst）、异步 FIFO、APB 寄存器、vsync 翻页 |
| `rtl/hdmi/dvi_encoder.v`, `encode.v` | 从 DemoBoard 的 HDMI demo 移植的 TMDS 编码器（2026-10-19 起不再使用，由 `hdmi_tmds_ch` 代替，视频编码完全相同） |
| `rtl/hdmi/hdmi_island.v`（音频，2026-10-19） | DVI → 真 HDMI：视频前导 + 保护带、行消隐里的数据岛（TERC4 + BCH ECC）、AVI / Audio InfoFrame、ACR、音频采样包、48 kHz 采样节拍、1 kHz 测试音；`hdmi_tmds_ch` = 每通道编码器。见第 9 节 |
| `rtl/audio/audio_mixer.v`（音频，2026-10-19） | 16 声部 PCM 混音器（APB `0xF810_2000`）+ `aud_rd_mux`（和 HDMI 扫描共用 `blit_axi_rd_arb` 的 m0 读口）。见第 9 节 |
| `rtl/ddr3_example_top.v` | 例化 `u_hdmi_fb`：APB = `io_apbSlave_0`，DMA = `io_ddrMasters_0`（只读，AW/W 置零），`userInterruptA = hdmi_irq`，TMDS 输出按板子 P/N 交换取反 |
| `par/ddr_demo_ti60/*.xml / peri.xml` | 加入 HDMI 源文件、PLL_BR0：VCO 4000 MHz / O=8 → 像素时钟 25.00 MHz（÷20）/ 5× 串行时钟 125 MHz（÷4，phase_setting=2 = 90°，与官方 HDMI demo 一致）、LVDS TX 引脚；时序 794×525（H_BP=42）= 59.97 Hz |
| `rtl/blit/blit_engine.v` | 新增：BitBlt 硬件加速器（APB 命令寄存器 + AXI4 burst 读写 DMA，见第 7 节）。2026-10-07 PIXEL 级改为提前一行准备行参数、命令间不排空（去气泡，见 7.1），只需 Efinity 重新综合，不用重新生成 SoC |
| `rtl/blit/blit_axi_rd_arb.v` | 新增：`io_ddrMasters_0` 读通道仲裁，HDMI 扫描（高优先级）与 BitBlt 共享；AW/W/B 只给 BitBlt |
| `rtl/blit/blit_axi_wide.v` | 新增（2026-10-07，C 方案）：32 位（仲裁器 / 引擎）↔ 128 位 `io_ddrMasters_0` 转换，不用 BRAM。W 拼成 128 位拍，整个 burst 的 W 进了 SoC FIFO 后才发 AW；R 的 128 位拍拆成每拍 1 字。见 7.1 |
| `rtl/blit/blit_engine.v`（D，2026-10-10） | **分带合成**（CAPS[31]）：一帧的命令先由引擎按 16 行一带分桶，再逐带在片上"带缓冲"里合成、整带写回 DDR；小图 / 填充每拍 8 像素（K 通路）。带缓冲 `blit_win` 也在这个文件里（8 路 × 3 × 512×16 = 24 块 BRAM），源缓存数据 RAM 改成 8 个 512×32。没有新文件、SoC 不用重新生成，只需 Efinity 重新综合（预计 BRAM 约 246–248 / 256，看一下 core_clk 时序）。见 7.1 |
| `rtl/blit/blit_engine.v`（加法混合，2026-10-11） | CMD[6] / 列表 REC[26] / SLOT[26] = ADD：每通道 `dst + (src·a + 128) >> 8` 饱和（发光弹、火花、闪光）；CAPS2（0xBC）[0] = 1。分带模式下纯加法（透明度 255、无 A8）的带缓存拷贝 / 填充走 K 通路，在带缓冲里原地读-加-写，和不透明一样快（仿真：满档 1650 颗弹改成加法，分带帧拍数不变；直接画则慢 2 倍多）。只需 Efinity 重新综合 |
| `ip/soc`（SoC IP 配置） | **AXIMasterWidth 32 → 128**（IP Manager 里改，重新生成 SoC），`io_ddrMasters_0` 端口名不变、数据位宽变 128 / strb 16；`ddr3_example_top.v` 里 dm0_* 跟着改成 128 位，原 32 位连线改名 nw_* 接到 `u_blit_wide` |
| `rtl/ddr3_controller/ddr3_parameter.vh` | `MEM_ADDR_ORDER` 由 `"BANK_ROW_COLUMN"` 改为 `"ROW_BANK_COLUMN"`（2026-10-07）：原来 bank 在地址最高位，帧缓冲 / 背景 / 精灵全在同一个 bank，每换一个 2 KB 页都要串行预充电 + 激活；改后相邻 2 KB 轮流落在 8 个 bank。对软件透明。板上 TAB 满档 93 → 118 fps，CPU 逻辑 9.05 → 6.03 ms/帧 |
| `par/ddr_demo_ti60/sdc/ddr3.sdc` | HDMI 时钟（第 16-21 行）、LVDS 输出约束（约第 318-338 行）；第 65 行原模板 `<USER_PERIOD>` 已改为 `40.000`（clk_25m） |

### Efinity 流程
0. （C 方案，2026-10-07 起）先在 IP Manager 里打开 SoC（`ip/soc`），把 **AXI Master Width 改成 128**，其余不动，Generate 重新生成 SoC。
   不重新生成的话 SoC 端口还是 32 位，`ddr3_example_top.v` 的 128 位 dm0_* 接上去只会被截断（综合只报 warning），引擎写进 DDR 的数据全错。
   **生成前先把 `par/ddr_demo_ti60/embedded_sw/soc` 整个目录备份**（IP 生成会重写这个目录里的 BSP / 示例软件，游戏工程和改过的 makefile 都在里面），生成后比对、还原被覆盖的文件。
   （2026-10-07 已备份一份到工程根目录 `backup_before_axi128/`：`embedded_sw_soc` 与 `ip_soc`，改 128 前的状态。）
   生成后可以看 `ip/soc/soc.v` 里 `io_ddrMasters_0_w_payload_data` 是不是 `[127:0]`。SoC 里 128 位的跨时钟 FIFO 比原来 32 位的多用十几块 BRAM（原来全片 229 / 256），布局报告里核对一下。
1. 用 Efinity 打开 `par/ddr_demo_ti60/ddr_demo_ti60.xml` → Interface Designer，检查 HDMI LVDS TX 和 PLL 输出，点 *Generate* 重新生成约束。
2. 对比 `outflow/ddr_demo_ti60.pt.sdc` 中 LVDS 参考引脚名（如 `hdmi_tx_slow_clk~CLKOUT~218~175`），
   与 `ddr3.sdc` 第 318-338 行保持一致，不一致就以 pt.sdc 为准修改。
3. Synthesize → Place & Route → 生成 bitstream，用 Programmer 下载。
4. 综合选项 **retiming 必须是 0**（File → Edit Project → Synthesis，工程 xml 里已改好）。开着时
   Efinity 会把引擎的读计数器 rd_words / rf_res 拆开挪动，复位值算错（复位后 0x7FF / 0x3FF），
   引擎发不出任何读，第一条 COPY 就卡死（串口 `stuck`，RB=0）。综合后可跑
   `python3 sim/blit/check_retime.py` 检查网表，输出 OK 才烧。
5. 分带合成（D，2026-10-10）：只改了 `rtl/blit/blit_engine.v`，不用动 IP / Interface Designer，直接重新综合 + 布局布线。
   看三样：Memory Blocks（预计约 246–248 / 256，带缓冲 +24）、core_clk 时序余量（原 2.6 ns；新逻辑不少，若为负把报告里的最差路径发来）、
   `check_retime.py` 输出 OK。软件要同时重新 `build_game.bat`（驱动、主循环、帧缓冲个数都变了）；新软件在旧位流上会自动不用分带（CAPS[31] = 0）。
6. core_clk 提频（2026-10-11）：CPU、BitBlt 引擎、读仲裁 / 宽桥、HDMI DMA 的 AXI 侧、DDR 控制器用户侧都在 core_clk 上
   （`ASYN_AXI_CLK = 0` → `user_clk = core_clk`；它来自单独的 pll_inst1，与 400 MHz 的 sdram_clk 是异步时钟组），所以可以单独提高。
   10 ns 约束下报告的 fmax（136 MHz）不代表上限：约束放松时 Efinity 不会往紧里优化；把约束改成 5 ns 时同一份网表（D 版）做到了 198.4 MHz，
   最差的路径都在引擎里，已切开（见 7.1"时序"）。提频要改：Interface Designer 里 pll_inst1 的 core_clk 输出频率；`sdc/ddr3.sdc` 第 2 行周期；
   BSP `soc.h` 的 `SYSTEM_CLINT_HZ`（串口分频、所有计时、`main.c` 的 `BLIT_CLK_MHZ` 都从它来；以后若重新生成 SoC，IP 里的 Frequency 也要同步）。
   改完串口首行 `hdmi` 应仍是 60/s，否则 `SYSTEM_CLINT_HZ` 没对上。
   **但 DDR 控制器不能跟着提频**：`ddr3_parameter.vh` 里 `CK_RATIO 4`，控制器的时钟必须正好是 400 MHz 存储器时钟的 1/4 = 100 MHz
   （它的 DDR 时序参数按这个时钟换算）。只把 core_clk 提到 200 MHz 时控制器失常，CPU 第一次访问 DDR 就卡住（OpenOCD `Halt timed out`）。
   所以 2026-10-11 起拆成两个时钟域：core_clk（200 MHz）= CPU / SoC 系统侧（io_systemClk）、io_ddrMasters_0、BitBlt、HDMI DMA；
   pll_fb_clk（pll_inst1 的 100 MHz 反馈输出，同时做设计时钟）= DDR 控制器（ddr3_top 的 core_clk / axi_clk，`ASYN_AXI_CLK` 仍为 0）+ SoC 的 io_memoryClk。
   两者之间由 SoC 自带的跨时钟 FIFO 完成（CPU 路径 StreamFifoCC，io_ddrMasters_0 路径 Axi4CC，原来就在、占的 BRAM 原来就算过）。
   改动：`ddr3_example_top.v` 新增输入 `pll_fb_clk`，DDR 复位计数器 / ddr3_top / io_memoryClk 改接它；`ddr3.sdc` 加 `pll_fb_clk` 10 ns 和独立时钟组。
   改之前的文件在 `backup_before_clksplit/`。

## 2. 内存 / 寄存器映射

DDR（CPU 与 DMA 地址一致，无偏移）：

| 地址 | 用途 |
|---|---|
| `0x0000_1000` – `0x00F0_0FFF` | 程序/数据/bss/栈（`game.ld`，15 MB） |
| `0x0100_0000` | 帧缓冲 0（614400 B，RGB565） |
| `0x0120_0000` | 帧缓冲 1 |
| `0x0140_0000` | 帧缓冲 2（三缓冲） |
| `0x0160_0000` / `0x0180_0000` / `0x01A0_0000` | BitBlt 演示用的 3 张 640×480 图片（启动时生成） |
| `0x01C0_0000` | 帧缓冲 3（2026-10-10，分带流水时多一帧在引擎里：显示中 / 等翻页 / 引擎在画 / CPU 在录） |
| `0x0200_0000` – `0x02FF_FFFF` | 声音库（`tools/wav2bank.py` 生成，`.sound` 段随 elf 下载；混音器直接从这里读采样，见第 9 节） |
| `0x0300_0000` – `0x0377_FFFF` | 分带合成的"桶"（引擎自己读写的暂存区，30 带 × 256 KB；`BLIT_BAND_BIN_BASE` / `BLIT_BAND_BIN_SHIFT`） |

APB 从口 0 按 `paddr[13:12]` 拆分：`0xF810_0000` = `hdmi_fb`，`0xF810_1000` = `blit_engine`（寄存器见第 7 节），`0xF810_2000` = `audio_mixer`（2026-10-19，第 9 节）。

`hdmi_fb` APB 基址 `0xF810_0000`：

| 偏移 | 名称 | 说明 |
|---|---|---|
| 0x00 | CTRL | [0] 使能扫描 [1] 彩条测试 [2] vsync 中断使能 |
| 0x04 | FB_ADDR | 写 = 请求在下一次 vsync 结束时翻页（覆盖尚未生效的请求） |
| 0x08 | FB_CUR | 当前正在扫描的帧缓冲 |
| 0x0C | STATUS | [0] flip_pending [1] vblank [2] underflow(W1C) [3] irq(W1C) |
| 0x10 | FRAME_CNT | 复位以来的帧数 |
| 0x14 | RES | {V_ACT, H_ACT} |
| 0x18 | UF_CNT | FIFO 下溢行数（应为 0） |
| 0x1C | ID | `0x48444D49`（"HDMI"） |
| 0x20 | AUD_CTRL | [0] HDMI 模式（0 = 和以前一样的纯 DVI）[1] 音频包 [2] 1 kHz 测试音代替 FIFO [5:4] 测试音 左/右；复位 0x33 |
| 0x24 | AUD_LEVEL | 音频 FIFO 里的采样数（只读） |
| 0x28 | AUD_SENT | 已发给电视的采样数（只读） |
| 0x2C | AUD_UF | FIFO 空、只能发静音的采样数（只读，应保持不变） |
| 0x30 | AUD_DATA | 写 = 推一个采样 {R[15:0], L[15:0]}（CPU 直接送 PCM 用；有混音器时不用） |
| 0x34 | AUD_CAPS | [0] 有 HDMI 音频，[15:8] FIFO 地址位数（8 = 256 个采样） |

## 3. 软件（`embedded_sw/soc/software/standalone/game`）

- `src/game.c / render.c / gfx.c`：用户 SproutRun 游戏原样移植，gfx CPU 后端逐像素写 DDR。
- `src/hdmi_fb.h`：寄存器驱动。
- `src/main.c`：多缓冲主循环（4 个帧缓冲，分带时流水翻页）、UART 非阻塞输出、自动驾驶 bot、FPS 统计。
- `game.ld`：放大 RAM 区（背景图 600 KB 在 .bss）。
- 敌方弹池的空闲槽位索引（`game.c` / `game.h`）：每个槽 1 位（1 = 空闲）+ 每 32 个槽 1 位汇总，
  取"最低空闲槽"只查两次表，释放只置位，池满时汇总全 0 立即返回，都是 O(1)。
  原来每发子弹线性扫描找空位，池满时每一发都要扫完整个池（4096 × 32 字节）却找不到：
  压力高档最坏一帧 315 发失败 ≈ 129 万次槽位探测，这就是池满时突然掉帧的来源。
  仍取最低空闲槽，子弹落在与原来相同的槽里，游戏结果逐位不变。
  另外 `hostile_active` 让 `game_hostile_count()` 不再扫池，`game_hostile_used()` 给出"此槽以上全空"的上界，
  更新 / 绘制 / bot 只遍历到这里（普通模式十几个槽，而不是 1024）。

编译（Efinity RISC-V SDK 终端或 Efinity RISC-V IDE）：

```
cd par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game
make                       # 默认 -O3 (DEBUG=no BENCH=yes)
make VSYNC=0               # 上电即不等 vsync，直接测最大帧率
make AUTOPLAY=0            # 上电等键盘操作
make GFX_CPU_WORD_FILL=0   # fill_rect 用原始 16 位逐像素循环做对比
make FONT_ATLAS=0          # 文字不用字模图集（对比用，见 7.3）；改选项后先 make clean
make STATS=1               # 调试 / 测量版（build_game.bat STATS=1 同理），见下
make CACHE=0               # 开机不开引擎源缓存（同一位流 A/B 对比 / 排查用）
```

`STATS`（→ `-DSPROUT_STATS`）：

- `STATS=0`（默认，发行版）：屏幕只有 FPS 和底栏 `MS`（压力模式另有 `WORK MS / BUDGET`），串口每秒一行
  `fps | HW/CPU vsync/free | hdmi | work (logic draw)`；不跑启动测速，没有统计面板和 A/B 调试键；
  配置固定为 hwlist + 弹幕表 + 键数组排序 + overlap + 在途读字数 64 + 源缓存开。
  驱动 / 游戏 / 渲染里的分段计时（`render_split`、`game_split`）、队列峰值、qstall 计时、命令 / 列表字计数全部编译掉。
- `STATS=1`：第 4 节的完整 6–8 行报告、启动测速、`f` 统计面板和第 5 节标 * 的键。
- 两个版本都带卡死诊断：等引擎（`blit_wait` / 队列满 / 列表缓冲满）约 1 s 没等到，串口直接打
  `!! BitBlt engine stuck in ...` 寄存器快照（最多 4 份）；翻页等了 1 s 打 `!! HDMI flip pending`。
- makefile 变量写在 `include standalone.mk` 之前才对 .c 编译生效；改任何变量后先 `make clean`
  （`build_game.bat clean` 再 `build_game.bat`），目标文件不随编译选项重新生成。
- 主机测试（`-DBLIT_HOST_MODEL`）总是保留驱动计数；`-DSPROUT_STATS=1` 时再多比对 seed table / seed parts。

下载运行：IDE 中使用 `game_ti.launch`（指向 `build/game.elf`），通过 JTAG 加载到 DDR 0x1000 运行。
串口 115200 8N1。

## 4. FPS 测量方法

- **vsync 模式（默认）**：每个 vblank 最多翻一次页，上限 60 FPS，画面无撕裂。
- **free 模式（按 `v`）**：从不等待，最新帧胜出 → 得到 RISC-V 纯软件的真实渲染帧率。
- 游戏逻辑始终以 1/60 s 步进（每渲染一帧最多一步，落后则丢帧不追赶），与 Windows 版一致。

发行版（`STATS=0`）串口每秒只有一行，例如
`fps 68.5 | HW free | hdmi 60/s uf 0 | work 14600 us (logic 3100 draw 11500)`。
下面是 `STATS=1` 测量版的输出：

启动时先打印全屏 fill / 全屏 copy / 全屏 alpha / 2000 个小方块在 CPU 与 BitBlt 上的耗时和带宽，
以及 2000 个 9×9 色键小图在 5 种提交方式下每条命令的开销（ns/cmd）：APB 逐个 `gfx_blit`、APB 批量 `gfx_blit_list`、
DDR 命令列表逐个、DDR 命令列表批量、`LIST_WORD` 批量；每行给出 queued（CPU 提交完）/ done（引擎画完）以及列表字数和 kick 次数，
然后**每秒 5–6 行**（窗口 1 s；屏幕 HUD 用 0.5 s 窗口）：

```
fps 60.0 | HW scene 0 | vsync auto fx | hdmi 60/s uf 0 | ph 2 foes 57
  us/frame: logic 1480 (max 2210) submit 3020 qstall 0 tail_wait 850 vsync_wait 11290 other 25 | draw 3870 (max 4400) work 5350 -> limit 186.9 fps
  share %: logic 9 submit 18 qstall 0 tail_wait 5 vsync_wait 68 other 0
  submit split: seeds 900 (310 seeds, 2900 ns each, list) hud 1500 scene 620
  scene split: bg+gems 210 sort 40 (keys) enemies 260 pbullets 30 particles 50 | logic split: enemies 400 pbullets 600 hostile 350 rest 130
  blit: busy 21% cmds 3900/frame | queue end 41 peak 118/256 stalls 0 | axi rd 38 wr 55 MB/s (while busy rd 180 wr 262)
  src cache ON: hit 30000 miss 40 line req/frame = 99% hits | INV 1200 total
  blit busy %: write 64 load 2 | wait-read 3 wr-full 0 bubble 30 no-cmd 1 | axi W-stall 20 AW-wait 2 AW-stall 0 AR-stall 5
  cmdlist: DDR words 4052/frame kicks 130/frame wraps 0 | STATUS list 0 lerr 0
  gfx: fill 3890 blit 11 alpha 10/61000 px 420000 | CPU 37 fps 25100 us / HW 60 fps 3870 us
```

每一帧的时间被完整拆成 6 段（CLINT 100 MHz 计时，`main.c` 主循环）：

| 字段 | 含义 |
|---|---|
| `logic` / `max` | 游戏逻辑（或方块演示的移动）每次 tick 的平均耗时 / 窗口内**单帧最大耗时** |
| `submit` | 绘图准备与提交：CPU 模式 = CPU 逐像素画整帧；HW 模式 = 生成并写入 BitBlt 命令，**已扣除 qstall** |
| `qstall` | 提交过程中命令队列满、等待空位的时间（显式队列等待，`blit_refill()` 里计时） |
| `tail_wait` | 最后一条命令写入后，等硬件把整帧画完（`blit_wait()`）；CPU 模式恒为 0。HW + `q` 重叠模式（默认）下帧顺序为 提交 → logic（引擎同时在画）→ 等待，这里只计 logic 结束后还剩的等待 |
| `vsync_wait` | 等上一次翻页在 vsync 生效（`v` 关闭 vsync 后为 0） |
| `other` | 剩余部分：选缓冲、串口收发、统计 |
| `draw (max)` | submit + qstall + tail_wait 及其最大值 |
| `work → limit` | logic + draw，不含 vsync 等待时的理论帧率上限 |
| `share %` | 上面 6 段各占窗口总时间的比例（相加 ≈ 100） |
| `submit split` | 把 submit + qstall 再拆成三段（`render_game` 内部用 CLINT 计时，`render_split`）：`seeds` = 敌方弹幕循环（括号里是每帧画出的弹数、每颗平均耗时、提交方式 `calls` / `list` / `hwlist` / `hwword`，见 `k` 键；之后的 `table` / `pool` 见 `z` 键），`hud` = 顶栏 + 菜单/暂停/升级面板 + 统计面板，`scene` = 其余（背景、宝石、敌人、玩家、玩家子弹、粒子、遮罩，以及 gfx_begin/end）。三段之和 = submit + qstall；方块 / 换图场景全部记入 scene |
| `scene split` | 把 `scene` 里游戏世界部分再拆开（`render.c` 的 `draw_world`，µs/帧）：`bg+gems` 背景 + 宝石，`sort` 敌人按 y 排绘制顺序（括号 `keys` / `structs` 见 `j` 键），`enemies` 敌人 + 玩家，`pbullets` 玩家子弹，`particles` 粒子 + 受伤边框；scene 减去这五项 = gfx_begin/end 等其余部分 |
| `logic split` | 把 `logic` 拆开（`game.c` 的 `game_tick`，`game_clock` 钩子，µs/tick）：`enemies` 刷怪 + 敌人移动，`pbullets` 自动开火 + 玩家子弹与敌人碰撞，`hostile` 敌方弹池更新与命中判定，`rest` 宝石、粒子、升级判定 |
| `blit busy` | 引擎忙碌时钟 / 窗口时间（PERF_BUSY） |
| `cmds/frame` | 每帧驱动写入的命令数 |
| `queue end / peak / 256` | 命令队列占用量：提交结束时仍在排队的条数（平均）/ 窗口内采样到的最大值 / 队列深度。驱动每 64 条命令读一次 STATUS，所以峰值每 64 条采样一次 |
| `stalls` | 窗口内队列满、CPU 被迫等待的次数 |
| `src cache` | 只在引擎带源缓存（CAPS[27]）时打印：ON/OFF（`0` 键），每帧缓存命中 / 缺失的行请求数（PERF_CHIT / PERF_CMISS，一行源像素跨两条缓存行算两次）和命中率，INV 为启动以来的失效命令总数 |
| `blit busy %` | 只在引擎带停顿计数器（CAPS[29]）时打印，按引擎 BUSY 的时钟拆分（%，`write` + 后面 4 项相加 = 100）：`write` 写出一个字（PERF_WBEAT，有用的拍）、`load` 命令数 / busy（去气泡后仅供参考，不再占拍）、`wait-read` 一行开始前等源/目标读数据（读延迟、读带宽）、`wr-full` 写 FIFO 或头 FIFO 满（写不出去：DDR 写口慢）、`bubble` 固定开销（取命令头 / 准备行 / 起行而当拍没出字，小图占比高）、`no-cmd` 没有命令可做（取列表 / CPU 跟不上）。后 4 项是 AXI 口上的停顿（与前面重叠，单独看）：`W-stall` W 有数据而 DDR 不收、`AW-wait` W 数据已在 FIFO 但 AW 还没发出、`AW-stall` AW 有效而 DDR 不收、`AR-stall` AR 有效而读口不收（HDMI / CPU 占着） |
| `band` | 只在引擎带分带合成（CAPS[31]）时打印：`ON` / `off`（`5` 键；`(K path off)` 见 `4` 键），每帧引擎在第 1 阶段（分桶，`phase1`）和第 2 阶段（逐带画，`bands`，括号里是其中整带写回 + 清零的部分）花的时间（PERF_P1 / PERF_BAND / PERF_FLUSH ÷ 帧数 ÷ 100），K 通路忙的时间和写入的拍数（1 拍 8 像素），每帧追加到桶里的命令数（跨带的命令每带算一次），窗口内被列表回绕拆开的帧数（应为 0），末尾 `ok` / `BIN OVERFLOW`（桶满丢命令）/ `BAD RECORD`（帧内命令行距不对）。分带 + 不限帧时第一行显示 `HW band+overlap`，此时 `tail_wait` 是**等上一帧画完**的时间（这一帧交给引擎后不等），`work → limit` 那个串行估算没有意义；`blit busy %` 那一行的拆分在分带帧期间不计数 |
| `cmdlist` | 只在硬件命令列表模式下打印：`DDR`（引擎从 DDR 取命令）或 `LIST_WORD`（经 APB 逐字喂入）；每帧写入的列表字数、`LIST_END` 写入次数（kick）、窗口内列表缓冲回绕次数；`STATUS list` = 采样时列表是否仍在执行，`lerr` = 遇到过非法命令字（应恒为 0） |
| `axi rd / wr` | 引擎实际 AXI 读 / 写吞吐 = PERF_RBEAT / PERF_WBEAT × 4 B ÷ 窗口时间（MB/s，10⁶ B/s）；不含 HDMI 扫描读（固定约 36.9 MB/s） |
| `while busy` | 同样的字节数 ÷ 引擎忙碌时间 = 引擎工作时的有效带宽 |
| `hdmi` / `uf` | HDMI 实际输出帧率（应为 60）/ DMA 下溢计数（非 0 说明带宽不够） |
| `CPU .. / HW ..` | 两个后端最近一次测得的 FPS 与 draw 时间 |

串口发送为非阻塞环形缓冲（4 KB），缓冲满时整行丢弃并在 `gfx` 行尾报告 `uart lines dropped`。

屏幕 HUD（`STATS=1`，`f` 切换）显示同样的拆分：`LOGIC / MAX / VSYNC`、`SUBMIT / QSTALL / TAIL`、
`SEEDS / HUD / SCENE` + `CALLS`/`LIST`/`HWLIST`/`HWWORD`（µs），
HW 模式另有 `Q 结束/峰值:深度 BLT% RD WR MB/S`；底栏为 `x.x MS`（work）和 FPS。

## 5. 串口按键

标 * 的键只在 `make STATS=1` 的测量版里有（发行版按了没反应）。

**PC 键盘（2026-10-11）**：串口终端只能收到"按下"的字符，判断不了"按住"和多键同时按（弹幕游戏要方向 + Shift 低速 + Z 射击一起按住）。
所以用 `game/tools/pc_pad.py` 代替串口终端：`python -m pip install pyserial pygame` 装一次，然后 `python tools/pc_pad.py COM5`（`--list` 列串口，`--log 文件` 同时存串口输出）。
它打开串口、把板子的输出打印在命令行里，并弹出一个小窗口（要保持在前台）：每秒 60 次把按键状态打成 4 字节包发给板子（`0xA5, b0, b1, chk`，见 `src/pad.h`），
方向键 / WASD、Z / 空格 射击、X 符卡、Shift 低速、Esc 暂停、Enter 开始、C / Ctrl 跳过（只支持键盘）。
其余字母数字键照旧当字符发（下表的调试键都还能用）。板子 300 ms 收不到包就当全部松开。SproutRun 里用它时方向键是真正的按住移动，一按就退出自动驾驶，Enter / Z = 开始，Esc = 暂停。
（Windows 上 `python3` 可能是应用商店的空壳，请用 `python` 或 `py`。）

| 键 | 功能 |
|---|---|
| w a s d / 方向键 | 移动（锁存，会关闭自动驾驶） |
| x | 停止 |
| Enter / Space | 开始 |
| p / r / m / l | 暂停 / 重开 / 主菜单 / 压力测试菜单 |
| 1 2 3 | 选择升级 / 档位 |
| b | 自动驾驶开关 |
| v | vsync 开关 |
| f * | 统计面板开关 |
| t | HDMI 彩条测试（排查链路用） |
| e | 半透明特效开关 |
| k * | 提交方式循环（A/B 对比用）：`calls` 每颗一次 `gfx_blit` → `list` 批量 `gfx_blit_list`（APB 寄存器）→ `hwlist` 硬件命令列表（DDR，默认）→ `hwword` 命令列表字经 APB `LIST_WORD` 写入。后两种对整帧所有命令生效；bitstream 不带命令列表（CAPS[24]=0）时只有前两种 |
| o / i * | 引擎 LIMIT：`o` 循环读 burst 上限 16→8→4→2→1，`i` 循环写 burst 上限 128→16→8→4→2→1（越小 CPU 访问 DDR 等得越短，引擎越慢） |
| u * | 引擎 LIMIT：循环在途读字数上限 1024→512→256→128→64→32→16（游戏默认 64，见 7.3） |
| j * | 敌人绘制顺序的排序方式（A/B 对比）：`keys`（默认）先把 192 个敌人的 active / y 顺序读一遍到小数组，再从上一帧的顺序开始插入排序（几乎已排好，接近 O(n)）；`structs` = 原来的写法，插入排序时反复到 44 字节的敌人结构里取 y。两者得到的顺序完全相同（同 y 按编号），画面不变 |
| z * | 弹幕数据来源（A/B 对比，只影响批量模式 `list` / `hwlist` / `hwword`）：`table`（默认）= `game_tick` 更新敌方弹时顺手为每颗存活且在屏内的弹写一条 `GfxItem`（左上角坐标 + 样式，按槽位顺序，`Game.seed_draw[]`，`game_seed_draw()` 取用），render 直接把整张表交给 `gfx_blit_list`，不再第二次遍历 32 字节的 Bullet 弹池、也不再拷贝到 128 项小表；`pool` = 原来的做法（render 再遍历一次弹池）。两者命令流和画面逐帧完全相同；表在新开一局后、第一次 tick 之前无效，此时自动退回 `pool` |
| q * | HW 帧顺序（只影响 BitBlt 后端）：`overlap`（默认）= 先把本帧命令全部提交，再在引擎画图的同时跑这一帧的 `game_tick`，最后 `blit_wait` + 翻页；`serial` = 原顺序 logic → 提交 → 等待。画面序列完全相同，overlap 下每帧显示的是 tick 之前的状态，即画面晚一个 tick（16.7 ms）。串口第一行显示 `HW overlap` / `HW serial`。注意 overlap 下 logic 的 D$ miss 会和引擎抢 DDR（`u` 字数上限可调） |
| y * | 一次性弹幕剖析（仅 BitBlt + 游戏场景）：在当前局面上把弹幕循环拆成 walk（只遍历弹池）/ build（再填绘制表）/ submit（只提交预建表）/ tiny（同 submit，1×1 精灵，引擎几乎不耗时 = 纯 CPU 提交成本）/ full（真实循环），引擎空闲和忙（先排 3 次整屏拷贝）各测一遍，UART 打印每颗 ns：cpu（扣除排队等待）、stall、done（含引擎完成）。该帧本身不具代表性 |
| 0 * | 引擎源缓存（16 KB BRAM，CAPS[27]）开 / 关，A/B 对比用；默认开，只对宽 ≤ 64 的拷贝生效，见 7.1 |
| 7 * | 128 位宽通路（CAPS[30]）：循环 关 → 开 WTH 0（有数据就写）→ 1 → 2 → 4（COPY 攒 64 / 128 / 256 拍再成批写）→ 关。切换前先 `blit_wait`，并清零统计窗口；串口打印当前设置 |
| 6 * | 宽通路在途读字数上限：循环 256（默认）→ 128 → 64 → 512 → 1024 → 32。与 `u`（LIMIT words，只管 32 位通路）互相独立；越大 bg 越快，引擎拷贝时 CPU 访问 DDR 等得越久 |
| 5 * | 分带合成 + 流水帧（CAPS[31]）开 / 关，同一位流 A/B 对比。开：每帧用 BANDS..ENDB 提交，引擎分带画，CPU 不等这一帧画完就去跑逻辑 / 录下一帧，下一圈再翻页（多一帧延迟）；关：逐条直接画（同以前）。只在 `hwlist` 下生效，串口第一行显示 `HW band+overlap`。开机默认开（makefile `BAND=0` 关） |
| 4 * | 分带里的 K 通路（小图 / 填充每拍 8 像素）开 / 关（BAND_GEO[31]），A/B 对比用；关掉后这些命令走 32 位窄通路写带缓冲 |
| 9 * | `hwlist` 下弹幕：引擎读弹幕表（TAB，CAPS[28]，默认）/ CPU 逐颗写 DRAW 字，A/B 对比用，见 7.1、7.3 |
| 8 * | 一次性引擎剖析（仅 BitBlt + 游戏场景 + `hwlist`）：在当前局面上分三部分 —— `bg`（背景）、`seeds`（整个弹幕层）、`frame`（整帧 `render_game`），`rest` = frame − bg − seeds。每部分先把命令全部写进列表但**不发布**（`blit_st.lhold`），CPU 写完后再一次 `blit_list_release()`，引擎单独跑、CPU 只在等，所以量到的是引擎自己的速度，不受 CPU 抢 DDR 影响；然后再正常（边写边跑）画一遍做对照。每行打印：命令数、`eng_us`（发布到画完）、`busyus`（PERF_BUSY）、`clk/wd`（每写一个字花的时钟，1.00 = 写口满速）、与 `blit busy %` 相同的 7 + 4 项拆分（`wide` 列 = PERF_WCLK，`clk/wd` 和 MB/s 把宽通路的 128 位拍按 4 字计入）、引擎忙时的读 / 写 MB/s、`cpu_us`（只写列表）、`live_us`（边写边跑的总时间）。列表缓冲在一部分里写满回绕时该行注明 `list wrapped: not isolated`。比特流没有 CAPS[29] 时拆分全为 0。该帧画面会被覆盖一次，不影响游戏 |
| g | CPU 绘制 / BitBlt 硬件绘制 切换 |
| c * | 自动对比：每 4 秒（8 个统计窗口）自动切换 CPU / HW，屏幕同时显示两者 FPS |
| n | 场景切换：游戏 → 随机移动方块 → 大图切换 |
| - / + | 方块数量（16…4096，×2 / ÷2）或 换图间隔（1…60 帧） |
| h / ? | 帮助 |

## 6. 排查

- 串口打印 `WARNING: hdmi_fb ID = ...`：bitstream 不是新版本。
- 无画面：按 `t` 看彩条；彩条也没有 → 查 PLL/LVDS 引脚/约束；有彩条无游戏 → 查 DMA / 帧缓冲地址。
- `uf` 持续增长：降低 CPU 访存压力或增大 FIFO / burst 长度。
- 帧率极低且启动打印 `-O0/-Og (slow!)`：以 `DEBUG=no` 重新编译。
- 串口打印 `BitBlt engine NOT found`：bitstream 里没有 blitter，程序自动只用 CPU。
- HW 模式画面错乱而 CPU 正常：先按 `g` 回 CPU 确认；看串口 `blit:` 行的 busy / axi 吞吐与 `tail_wait`，再跑第 7.5 节的仿真。
- `qstall` 或 `stalls` 不为 0：CPU 比引擎快了一整个队列（APB 模式下 16 条；hwlist 模式下是列表缓冲写满回绕），瓶颈在引擎 / DDR；`tail_wait` 大而 `qstall` 为 0 同理但程度较轻。

## 7. BitBlt 硬件加速器

### 7.1 结构

```
RISC-V --APB--> 命令寄存器 --> 命令队列(16) --> FETCH --AR--> DDR --R--> 行缓冲环(src / dst / A8)
                                                  |                            |
                                                  +--> PIXEL（色键、alpha 混合，每拍 2 像素）
                                                                  | W FIFO(512)
                                                                  +--> WRITE --AW/W--> DDR

RISC-V --store--> DDR 命令列表 --AR/R（LIST_ADDR..LIST_END）--> 取字 FIFO --> 解码（列表上下文 + 64 个图槽）--> 命令队列
RISC-V --APB LIST_WORD-------------------------------------------------------^
```

- 时钟 `user_clk` 100 MHz，引擎与 HDMI 发 AXI4 32 位 INCR burst（≤64 beat，不跨 256 B 边界），经 `blit_axi_wide` 转成 128 位（≤16 拍）走 `io_ddrMasters_0`。
- 读通道与 HDMI 扫描共享（`blit_axi_rd_arb`，HDMI 固定高优先级），写通道只有 BitBlt 使用。
- 支持：Solid Fill、Block Copy、色键 0xF81F（按字节 strobe 屏蔽，不读目标）、
  全局 8 位 alpha、每像素 A8 alpha 平面（两者相乘）。任意 2 字节对齐的 src/dst。
- 混合公式：`out = (src*a + dst*(256-a) + 128) >> 8`，每个 RGB565 通道。
- 需要读目标（alpha）或带 SYNC 标志的命令会等之前所有写完成（B 响应），保证叠加顺序正确。
- 硬件命令列表（CAPS[24]）：CPU 把命令字写进 DDR 里的数组，再写一次 `LIST_END`，引擎自己用 AXI 把 `[ptr, end)` 读回来解码、
  压入同一个命令队列。列表有独立的一套参数上下文（不动 APB 的 0x10–0x30 寄存器）和 64 个图槽（SRC / SSTR / SIZE / CMD）；
  画一个已登记的小图只要 1 个字。解码开销：REC = 字段数 + 2 拍，DRAW = 2 拍。列表执行期间 APB 写 CMD 会被挂起（PREADY 拉低），
  所以两种入口的命令顺序始终等于 CPU 的写入顺序。
- 源缓存（CAPS[27]，2026-10-04）：引擎内 16 KB 块 RAM，直接映射，512 行 × 32 B，tag = 地址[31:14]（标签也在 BRAM，有效位用触发器）。
  只有带缓存标志的 COPY 的**源**数据走缓存（精灵、弹幕、字模图集这类反复读的小图）；这类源行在 32 B 行边界处切开读，
  命中时完全不发 AXI 读，缺失时读整条对齐的 8 字行并留下。缓存不监听任何写（引擎自己的写和 CPU 的写都不），
  所以 CPU 改了被缓存过的源数据后必须发 INV（整块清有效位）；INV 按命令顺序生效，排在它前面的命令仍看到旧数据。
  目的：弹幕每颗 9×9 小图原来要 9 个很短的 AXI 读 burst，DDR 延迟占大头，还和 CPU 抢 DDR；命中后只剩写。
  仿真（随机 DDR 延迟）600 个 9×9 色键小图：不带缓存约 103 拍/个，带缓存约 70 拍/个（APB 命中 8394 / 缺失 6，列表 8400 / 0）。
  资源：16 KB 数据 RAM + 512 项标签 RAM（Efinity 布局报告全片 BRAM 229 / 256），改 RTL 后要在 Efinity 里重新综合。
- 精灵表记录 TAB（CAPS[28]，2026-10-04）：列表里一条 `TAB(n)` + 一个地址字，引擎自己从 DDR 把 n 项 8 B 的表读回来，
  每项 `{x, y, 精灵号}`（正好是 `GfxItem` 的内存布局）就地算偏移 `y·DSTR + 2x`，等于一个 DRAW(槽 = 精灵号[5:0], 偏移)。
  实现：取数器保存主列表的"下一个字 / 结束地址"，把读指针临时指到表上，读完再切回来（已经预取进 FIFO 的主列表字先丢掉，
  回来后重新读）；解码器多了几个状态（读 xy、读精灵号、乘 DSTR、加 2x、入队），每项 4 拍左右，引擎画一颗 9×9 要 70 拍以上，
  所以不会成为瓶颈。表在引擎读完前不能改；表执行期间 `LIST_WORD` 写会被挂起，`LIST_ADDR/LIST_END` 读到的是主列表的位置。
  用途：A 方案后每颗弹幕 CPU 仍要算偏移、写 1 个 DDR 字（板上约 0.7 μs/颗，3800 颗 ≈ 2.7 ms）；
  有了 TAB，游戏逻辑本来就写好的弹幕表直接交给引擎，CPU 每帧只写 2 个字，与弹幕数无关，弹幕数量、画面都不变。
  仿真（随机 DDR 延迟）600 个 9×9 色键小图：APB 104、DRAW 列表 106、TAB 105 拍/个（瓶颈在画，不在取表）。
- 停顿计数器（CAPS[29]，2026-10-05）：为了弄清引擎写带宽卡在哪，0x64–0x80 加了 8 个 32 位时钟计数器（CTRL[0] 清零）。
  PIXEL 级的 4 个（wait / full / bub / idle）只在 busy 时计数，和 PERF_WBEAT 一起把 BUSY 的每个时钟分到唯一一类：
  `BUSY = WBEAT + PXWAIT + PXFULL + PXBUB + PXIDLE`（`tb_blit.py` 逐个测试断言这个等式）。AXI 口的 4 个（W / AW / AR 停顿）单独看。
  （去气泡之前的位流装命令单独占 1 拍，等式是 `BUSY = WBEAT + CMDS + 这 4 项`；现在 PERF_CMDS 只是命令数，串口 `load` / `ld` 列仅供参考。）
  只是几个比较器和计数器，不改变任何时序行为。
  仿真（理想 DDR）600 个带缓存的 9×9 小图：BUSY 42081 = 写 27000（每颗 45 字）+ 取命令 600 + 等读 56 + **气泡 14394（每颗约 24 拍，34%）** + 无命令 31；
  也就是小图的瓶颈是每行固定的建行 / 起行开销和命令间排空，不是 DDR。大块拷贝（320×40）等读 667 拍，alpha 混合等读 7707 拍（读带宽受限）。
  板上的实际拆分用 `8` 键和串口 `blit busy %` 行看。
- 板上结果（`8` 键，TAB 满档，2026-10-07）：
  原地址顺序 BANK_ROW_COLUMN 时，一帧引擎 7.9 ms：bg 整屏拷贝 3.2 ms，弹幕 3.0 ms，其余 1.7 ms。弹幕 W-stall 41%、AW-stall 38%，
  DDR 每 8.6 拍才收下一行，原因是同一 bank 内换页。改成 ROW_BANK_COLUMN 后一帧 7.4 ms：弹幕 2.7 ms，W/AW 停顿降到约 0，
  剩下的是写 64% + 气泡 34%（引擎自身开销）；bg 不变（3.2 ms，2.06 拍/字，读、写各约 194 MB/s）。
  LIMIT words 64 → 1024 对 bg 无效（写侧反而 W-stall 38%），CPU 提交还变慢（fps 118 → 101），所以 64 保留。
  bg 的上限很可能来自 32 位口：SoC 的 Axi4Upsizer 把 32 位 burst 拼成 128 位，但每个 128 位拍要等 4 个 32 位拍，
  一个事务在 DDR 控制器里占 4 倍时间；控制器内部读写是串行的，所以拷贝 = 读 1 字/拍 + 写 1 字/拍 交替 ≈ 2 拍/字。
- 128 位口（C 方案，`rtl/blit/blit_axi_wide.v`，2026-10-07）：SoC 的 `io_ddrMasters_0` 改成 128 位（= DDR 控制器宽度，SoC 里就不再有 Upsizer），
  32↔128 的转换放到 SoC 的跨时钟 FIFO 前面做，引擎和 `hdmi_fb` 的 32 位接口、`blit_axi_rd_arb` 都不改。
  - AR 直通：地址按 16 B 向下对齐，len 换算成 128 位拍数，size 4，ARID 固定 0（全部按序返回）；每个 burst 记一项 {id, 起始字, 长度}（16 项，寄存器）。
  - R：128 位拍按起始字逐字送出（1 字/拍），在第 3 个字或 burst 最后一个字时才给 RREADY，不用的字丢掉；RID 还原成 {3'b0, id}，仲裁器照旧按 RID[0] 分发。
    控制器的 R 拍先进 SoC 的 128 位 R FIFO，不再像 Upsizer 那样把控制器的读通道卡住 4 拍。
  - AW/W：引擎的 AW 进 4 项队列，W 字拼进 128 位拍（不用的字 strobe 为 0），一个输出寄存器发往 SoC；**一个 burst 的最后一拍 W 被 SoC 收下后才发它的 AW**，
    所以控制器拿到 AW 时 W 已经全部在 FIFO 里，按 1 拍 128 位的速度写（burst ≤16 拍 = SoC W FIFO 深度）。
    保底：W 连续 16 拍被挡住、又没有"W 已齐、等发 AW"的 burst 时（例如从机要求先 AW），提前发正在写的那个 burst 的 AW，不会死锁。
  - B 直通。资源：约 650 个触发器（队列 + 两个 128 位拍寄存器）+ 少量 LUT，无 BRAM；SoC 那边 128 位 FIFO 多十几块 BRAM。
  预期：bg 整屏拷贝从 2.06 拍/字往 1 拍/字靠（≈3.2 → 1.6–2 ms），CPU 访存与引擎抢 DDR 的时间也变短；弹幕（引擎气泡）不受影响，那是 B 方向的事。
  LIMIT words 的含义不变（32 位字），换 128 位后值得用 `u` 键重新试 64 / 128 / 256。
  板上实测（`8` 键，TAB 满档，LIMIT words 64）：118.7 → **143.7 fps**；一帧引擎 6.07 ms：bg 1.73 ms（1.12 拍/字，写 89%），
  弹幕 2.80 ms（3995 条，70 拍/颗：写 64%、气泡 34%），其余 1.54 ms；引擎 busy 87%，是瓶颈。LIMIT words 仍是 64 最好。
- PIXEL 去气泡（B 方案，2026-10-07）：原来 PIXEL 每行先 1 拍建行（C_ROW）、至少 1 拍等数据（C_WAIT）再出字，
  而且换命令前要等 6 级流水线排空（P1/P3/P4 直接用当前命令的参数），9×9 小图 45 个字要多花约 25 拍。现在拆成两部分：
  - 行生成器：从 h1 取命令头（同时交给 WRITE 的 h2），提前一行把下一行的几何参数（奇偶、字数、源/alpha 偏移）算进一个 1 项的行槽，最快 1 行/拍；
  - 发字：行槽里那行的数据已在环里时，上一行最后一个字的**下一拍**就开始出字；`c_ps_*` 指向"下一行在环里的起点"，跑在释放指针 `c_rd_*` 前面，
    行结束时照旧按行释放环（给 FETCH 的额度不变，环不会被覆盖）。
  - 命令参数（op / 色键 / 颜色 / 全局 alpha / 混合标志）跟着每个字走 P1→P4，所以新命令可以紧接着上一条的尾巴进流水线，不再排空。
  WRITE 侧、FETCH、缓存、列表 / TAB 都不变；增加约 100 个触发器。
  仿真（理想 DDR）600 个带缓存的 9×9 小图：70 → **50 拍/颗**，BUSY 42083 → 30343 = 写 27000 + 等读 584 + 气泡 2726 + 无命令 33；
  APB / 列表 / TAB（不带缓存）54 / 55 / 56 拍/颗（原 70）。大块 fill / copy / alpha 不变（1.97 / 1.77 / 0.90 像素/拍）。
  预期板上弹幕 2.8 ms → 约 2.0 ms。只改了 `blit_engine.v`，Efinity 重新综合即可（retiming 仍须为 0，综合后跑 `check_retime.py`）。
  板上实测：143.7 → 152–154 fps。
- 队列改寄存器（2026-10-08）：命令队列 cq（212 位宽）和两个命令头 FIFO h1 / h2 原来用 BRAM，Ti60 一块 RAM 最宽 20 位，
  所以不管多深都要 11 块左右；现在换成触发器实现的 `blit_rfifo`（接口、时序与 `blit_fifo` 相同），cq 深度 256 → 16（CQ_AW = 4）。
  驱动按 STATUS 空位数做 credit，hwlist 模式下命令走 DDR 列表，队列深度对吞吐没有影响。省约 21 块 BRAM，代价是几千个触发器（以 Efinity 综合报告为准）。
- 128 位宽通路（C2，CAPS[30]，2026-10-08）：大块**不透明**拷贝 / 填充直接用 128 位，每拍 8 像素，不经过 32 位的 PIXEL / WRITE 流水线。
  - 条件（`q_wide`）：FILL，或 COPY 且 dst 与 src 地址低 4 位相同、DSTR 与 SSTR 低 4 位相同（每行在 16 B 内的相位一致）；
    无色键、全局 alpha = 255、无 A8、宽 ≥ MINW（WIDE[15:8]×8，默认 128 像素）、WIDE[0] = 0。其它命令照旧走 32 位通路。
  - 宽命令先等 32 位流水线排空（命令顺序不变），然后三个行计数器同时走：xr 发源 AR（只有 COPY）、xa 发目标 AW、xw 出 W 拍；
    burst 在源、目标两侧都停在 256 B 边界，相位相同，所以 R 拍到达的顺序正好就是 W 拍的顺序，缓冲是一个普通 FIFO。
    每行首尾不满 8 像素的拍用 strobe 屏蔽。
  - 缓冲：借用行缓冲环 + 源缓存数据 RAM 拼成 1024 × 128 位（第 0 列 ring_d，第 1 列 s1:s0，第 2 列 a3..a0，第 3 列 cdata），**不新增 BRAM**；
    用了缓存数据 RAM，所以宽命令会把缓存行 0..127 置无效（之后的小图会多几次缺失，像素不受影响）。
  - 发 AR 前先预留缓冲空间（`xres`），所以 R 永远不被挡住；一个 AW 只有在它的数据已全部进缓冲时才发，W 不会等 R。
    WTH（WIDE[7:4]）> 0 时 COPY 先攒 WTH×64 拍再成批写，读写交替少一些；为防死锁，`xres + 16 > 1024` 时不管 WTH 都允许写。
  - 在途读字数单独限（WIDE[26:16]，复位 256 个 32 位字），不用 LIMIT words，所以游戏的 LIMIT 64 不会把宽拷贝卡住；
    LIMIT 的读 burst 数上限仍然生效。这个值越大 bg 越快，但 CPU 在引擎拷贝时访问 DDR 等得越久（见 7.3"CPU 与引擎抢 DDR"）。
  - `blit_axi_wide` 多一组 128 位的 x_* 口：x_ar 与引擎 32 位 AR 共用 AR（32 位优先，已挂起的不抢），读队列给该 burst 标 wide，
    R 拍整拍从 x_r 出去；x_aw 进同一个 AW 队列（带 wide 标志），W 拍绕过拼字器直接进输出寄存器，所以 AW / W / B 顺序仍是发出顺序。
  - 计数：PERF_WCLK（宽模式的时钟数，`BUSY = WBEAT + PXWAIT + PXFULL + PXBUB + PXIDLE + WCLK`）、PERF_XWBEAT / PERF_XRBEAT（128 位拍数）。
  - 仿真（理想 DDR）320×40：fill 1.97 → 7.5 像素/拍，copy 1.77 → 7.0 像素/拍；整屏 640×480 拷贝约 1.72 → 0.43 ms。
    小图、色键、alpha 不走宽通路，数字不变。
  - 注意：游戏的背景图 `background_pixels` 原来链接在 0x00023548（模 16 = 8），帧缓冲是 0x01000000（模 16 = 0），相位不同，
    整屏背景拷贝走不了宽通路；`render.c` 里给它加了 `__attribute__((aligned(64)))`。以后新加的大图要想走宽通路，也要和帧缓冲同相位（16 B 对齐即可）。

- 分带合成（D，CAPS[31]，2026-10-10）：一帧不再逐条直接写 DDR，而是 16 行一带在片上合成好再整带写回（类似手机 GPU 的 tile 渲染，这里的 tile 是整行宽的带）。
  - 带缓冲 `blit_win`：16 行 × 1280 字节 = 8 路像素 × 16 位 × 1280 拍，每路 3 个 512×16 的 RAM（共 24 块），每路有自己的写使能，一拍可以写任意几个像素。
  - 列表里一帧用 `BANDS`（后跟帧缓冲地址）和 `ENDB` 包起来。**第 1 阶段（分桶）**：这段里的每条命令（REC、DRAW、TAB 的每一项）照常解析成完整命令，
    经一条 6 级的"Y 流水线"算出它落在哪几行（行号 = (地址 − 帧基址) / 行距，用倒数乘法，两个 DSP），按 16 行一带把这条命令（8 个字）追加到 DDR 里每带一个的桶（`BAND_BIN`，x_* 口 2 拍 burst）。
    一条命令跨几带就进几个桶。目标不在这一帧里的命令（画别的缓冲）照旧立即执行；尺寸为 0 的命令只执行它的 INV。CPU 可以边录边发布（LIST_END），引擎边分桶。
  - **第 2 阶段（逐带）**：ENDB 之后，对每一带：用列表 DMA 读回这一带的桶 → 同一条 Y 流水线把命令裁剪到这 16 行（跳过带上方的行：dst / src / A 各加 k0 × 行距）→ 命令队列 → 画进带缓冲：
    - **K 通路**（每拍 8 像素）：走源缓存的 COPY（不透明 / 色键）和不透明 FILL。walker 每拍检查一条源缓存行（tag RAM，缺失就发 8 字的行填充，只进缓存），
      每行的几何进一个小队列；compose 每拍从缓存数据 RAM 读相邻两个 16 字节的拍（数据 RAM 拆成 8 个 512×32：字 0–3 = 偶数拍、4–7 = 奇数拍），
      256 → 128 位移位对齐到目标像素通道，色键 / 行首尾得到 8 个像素写使能，写进带缓冲。一颗 9×9 = 9 行 × 2 拍 = 18 拍（直接画约 50 拍），而且不产生任何 DDR 写。
    - **宽通路**：同相位的大块不透明 COPY（背景）读回的 128 位 R 拍直接写进带缓冲，不用 C2 的拍缓冲（也就不再作废源缓存行）。
    - **窄通路**：其余命令（透明混合、A8、不带缓存的色键图）。混合的目标像素在 P1 级从带缓冲读，P6 级写回带缓冲，不再在 DDR 上读-改-写。
    - 三条通路之间保持命令顺序：一条命令只有在别的通路都写完带缓冲后才开始。
    - 这一带的桶读完、三条通路都空了，就把带缓冲用 16 拍 burst 写回帧缓冲（x_* 口），读出的同时清零。最后一带的写响应都回来后 `FRAMES` 加 1，主列表继续。
  - 语义差别：分带帧**从黑色开始**，没画到的像素是 0（不是帧缓冲旧内容，游戏每帧都画满背景所以没区别）；帧内多行命令的 DSTR 必须等于帧行距（否则丢弃并置 STATUS[7]）；
    命令之间的次序、像素结果与直接画完全相同（仿真和主机测试逐像素比对）。BANDS 之后必须有 ENDB，否则引擎一直忙。
    帧里用到的源图（精灵、字模、背景、A8）从 BANDS 到这一帧 FRAMES 加 1 之前都不能改（第 2 阶段才真正读它们）；帧内尺寸为 0 的 INV 在第 1 阶段就执行，
    等于对整帧生效（驱动每帧开头发的就是这种）。帧基址低 8 位被硬件忽略；帧内命令的行不能越过帧的右边界（驱动本来就按屏幕裁剪）。
  - DDR 流量：每帧 = 背景读 0.6 MB + 整帧写回 0.6 MB + 桶（每条命令每带 32 字节，写一遍读一遍）+ 小图源的缓存缺失；3800 颗弹幕不再有 3.4 万次 2 拍的零散写。
  - 仿真（理想 DDR，13 带 = 208 行，`tb_bperf.py`：背景 + 1650 颗 9×9 TAB 弹幕 + 40 个 24×24 + 4 块半透明面板，密度约等于满档）：直接画 125457 拍，分带 88591 拍 =
    分桶 6900（TAB 解码 4 拍/项）+ 写回 18999 + 背景读 16640 + K 通路 33457（18 拍/颗）+ 半透明约 8000（仍是每拍 2 像素）+ 其它约 4600。
    注意理想从机里"直接画"的背景读写是同时进行的，板上 DDR 半双工，背景读 + 整帧写回的总流量与原来的背景拷贝相同，所以板上的差距会比仿真大：
    预计引擎每帧 4.3 ms → 约 2.5 ms（待板上 `8` 键实测）。
  - 资源：BRAM +24（带缓冲），源缓存数据 RAM 由 1 个 4096×32 改成 8 个 512×32（块数基本不变）；新增的队列都是触发器。
    时序：写完后另做了一次独立的 RTL 审查（找到并修了一个潜在问题：带 INV 的非空 K 命令要等 K 通路排空），并把审查列出的最长组合路径都切开了——
    解码器 → Y 流水线之间加 2 项队列（解码器只看寄存器化的"满"）、Y 流水线 7 级（倒数乘法两个 DSP + 加法、行裁剪单独一级）、
    K 通路的 tag 比较结果先打一拍再决定（一个缓存行 token 占 2 拍，弹幕仍是 compose 18 拍/颗为瓶颈）、K 填充与列表取数的 AR 优先级固定、
    桶计数与下一带的桶地址提前一拍算好、移位与色键分两级。综合后看 core_clk 余量。retiming 仍须为 0。

- 时序（2026-10-11，为 core_clk 提到约 200 MHz）：按 Efinity 报告切开引擎里最长的几条组合路径，功能和周期数基本不变（仿真全部重跑通过）——
  宽通路的 W 出口加 4 深预取 FIFO，缓冲 RAM 的读地址改为寄存器（原来 SoC 的 W ready 一路组合到 RAM 读地址，17 级）；
  命令开始条件只用寄存器：各通路"已排空"标志晚一拍、命令类别（K / 宽 / 空 / 需同步）在队头出现后一拍算好（每条命令最多多 1 拍）；
  K 通路行队列的"有空位"和"compose 空闲"改为寄存器（保守：同拍入队算占用）。
  第二轮（5 ns 约束下的报告）：源缓存行填充的 tag 写入和 valid 位晚一拍生效（原来 tag RAM 读出 → 比较 → 发填充 → 512 个 valid 触发器的使能，一拍走完），
  落在填充那一拍和下一拍的查找作废重查（c_qok），不会看到写了一半的行。

- 加法混合（2026-10-11，CAPS2[0]）：CMD[6] ADD（列表 REC[26]、SLOT[26]）。每通道 `out = min(dst + ((src·a + 128) >> 8), 最大值)`，
  a = 全局透明度（255 → 256，即源全部加上）× A8 平面（与普通混合同样的换算），色键照样跳过像素。
  - 窄通路：原来的混合流水线里目标权重固定为 256（不再是 256 − a），打包时每通道饱和；普通混合的结果到不了饱和值，所以对它没有影响。直接画（非分带）时要从 DDR 读目标，约每拍 1 像素，而且每条命令要等之前的写完成。
  - K 通路（分带模式，纯加法 = 透明度 255、无 A8，带缓存的 COPY 或 FILL）：compose 发出某拍的同时读带缓冲里这一拍的目标（读口在 K 通路工作时空闲），K2 级 8 个像素并行饱和相加，K3 写回。
    如果这一拍的目标还在 K1–K3 里没写回（前后两个加法精灵重叠），就等它写完再读（kc_hz，仿真里重叠测试会触发）。速度与不透明相同：每拍 8 像素。
  - 仿真（理想 DDR，13 带）：1650 颗 9×9 带缓存的色键弹改成加法，分带帧 88987 拍（与不透明完全相同），直接画 257076 拍（不透明 125461）。

### 7.2 寄存器（基址 `0xF810_1000`）

| 偏移 | 名称 | 说明 |
|---|---|---|
| 0x00 | ID | `0x424C5431`（"BLT1"） |
| 0x04 | STATUS | [0] busy（含列表未执行完）[1] 队列满 [2] 错误 [3] 列表执行中 [4] LERR：列表里出现过非法命令字（粘滞）[5] 分带帧进行中 [6] 某带的桶满了、丢了命令（粘滞）[7] 帧内命令行距不等于帧行距、已丢弃（粘滞）[31:16] 队列空位数 |
| 0x08 | CTRL | [0] 清性能计数 [1] 清错误（含 LERR、STATUS[6]、[7]） |
| 0x0C | CAPS | [31] 有分带合成（BANDS / ENDB，0x94–0xB8），[30] 有 128 位宽通路（0x84–0x90），[29] 有停顿计数器（0x64–0x80），[28] 列表支持 TAB 精灵表，[27] 有源缓存，[26] LIMIT 有在途读字数字段，[25] 有 LIMIT 寄存器，[24] 支持命令列表，[23:0] {RING_AW, WF_AW, CQ_AW}；当前 `0xFF0A0904`（CQ_AW = 4：16 项寄存器队列） |
| 0x10 / 0x14 | DST / DST_STRIDE | 目标左上角字节地址 / 行跨度（字节） |
| 0x18 / 0x1C | SRC / SRC_STRIDE | 源地址 / 跨度 |
| 0x20 / 0x24 | A_ADDR / A_STRIDE | A8 alpha 平面地址 / 跨度 |
| 0x28 | SIZE | [10:0] 宽 [26:16] 高（像素） |
| 0x2C | COLOR | 填充色 RGB565 |
| 0x30 | KEY | 色键（复位值 0xF81F） |
| 0x34 | CMD | 写入即入队：[0] 0=FILL 1=COPY [1] 色键 [2] A8 平面 [3] SYNC [4] 源经缓存（仅 COPY）[5] INV：先清整个源缓存（可用于尺寸 0 的空命令）[6] ADD 加法混合（CAPS2[0]）[15:8] 全局 alpha（255=不透明） |
| 0x38 | LIST_ADDR | 写：列表读指针 = 结束指针 = 该地址（4 字节对齐）；上一个列表没执行完时写操作被挂起。读：当前读指针 |
| 0x3C | LIST_END | 写：结束指针，引擎开始取 `[ptr, end)`；只能向前推进到一条记录的边界。读：当前结束指针 |
| 0x54 | LIMIT | （CAPS[25]）[4:0] 引擎最多同时挂起的读 burst 数（1–16，复位 16），[15:8] 最多同时挂起的写 burst 数（1–128，复位 128），[26:16]（CAPS[26]）在途读字数上限（1–1024，复位 1024）：已发出、数据还没回来的读字数少于该值才发新的读 burst。见 7.3 "CPU 与引擎抢 DDR" |
| 0x58 | LIST_WORD | 写一个命令字，走同一个解码器（不经 DDR）；只能在没有 DDR 列表在执行时使用 |
| 0x40–0x50 | PERF | busy 时钟数 / 完成命令数 / 读 beat / 写 beat / 已入队命令数 |
| 0x5C / 0x60 | PERF_CHIT / PERF_CMISS | （CAPS[27]）源缓存命中的读请求数 / 缺失（整行重填）次数，CTRL[0] 清零 |
| 0x64 / 0x68 | PERF_WSTALL / PERF_AWWAIT | （CAPS[29]）时钟数：W 有效而 WREADY = 0 / W 数据已在写 FIFO 但对应 AW 还没发出 |
| 0x6C / 0x70 | PERF_AWSTALL / PERF_ARSTALL | （CAPS[29]）时钟数：AW 有效而 AWREADY = 0 / AR 有效而 ARREADY = 0 |
| 0x74 / 0x78 | PERF_PXWAIT / PERF_PXFULL | （CAPS[29]，只在 busy 时计）PIXEL 级等行的读数据 / 写 FIFO 或头 FIFO 满 |
| 0x7C / 0x80 | PERF_PXBUB / PERF_PXIDLE | （CAPS[29]，只在 busy 时计）固定开销（取命令头 / 准备行 / 起行而当拍没出字）/ 没有命令头可做（宽模式时不计）。BUSY = WBEAT + 这 4 项（+ PERF_WCLK） |
| 0x84 | WIDE | （CAPS[30]）[0] OFF（1 = 全部走 32 位通路）[7:4] WTH：COPY 攒够 WTH×64 拍再成批写（0 = 有就写）[15:8] MINW/8（复位 16 = 128 像素）[26:16] 宽通路在途读字数上限（32 位字，1–1024，复位 256；写 0 当 1）。复位值 `0x01001000` |
| 0x88 | PERF_WCLK | （CAPS[30]）busy 期间处于宽模式（`x_mode`）的时钟数 |
| 0x8C / 0x90 | PERF_XWBEAT / PERF_XRBEAT | （CAPS[30]）宽通路的 128 位 W 拍 / R 拍数（1 拍 = 16 B） |
| 0x94 | BAND_GEO | （CAPS[31]）[15:0] 帧行距（字节，16 的倍数，256–1280）[21:16] 带数（1–32，帧高 = 带数 × 16 行）[31] KOFF：1 = 不用 K 通路。复位 30 带 × 1280 |
| 0x98 | BAND_RECIP | [20:0] floor(2^24 / (行距 / 16)) + 1，引擎用它把地址换成行号（1280 → 209716） |
| 0x9C | BAND_BIN | [31:12] 桶区基地址 [4:0] log2(每带桶的字节数)，12–20；每条命令 32 字节，桶满后丢命令并置 STATUS[6]。复位 0x03000000、18（256 KB = 8192 条） |
| 0xA0 | FRAMES | 复位以来画完（最后一带写回的响应都回来）的分带帧数 |
| 0xA4 / 0xA8 | PERF_P1 / PERF_FLUSH | 第 1 阶段（分桶）的时钟数 / 整带写回 + 清零的时钟数 |
| 0xAC / 0xB0 | PERF_KBEAT / PERF_RECS | K 通路写进带缓冲的拍数（1 拍 8 像素）/ 追加到桶里的命令数 |
| 0xB4 / 0xB8 | PERF_BAND / PERF_KCLK | 第 2 阶段（逐带画，含写回）的时钟数 / K 通路忙的时钟数。分带帧期间 0x74–0x80、0x88 不计数；0x8C 把桶和写回的 128 位 W 拍也算进去 |
| 0xBC | CAPS2 | [0] 有加法混合（CMD[6]、REC[26]、SLOT[26]）。旧位流读出 0，驱动据此退化成普通拷贝 / 透明混合 |

命令列表字格式（32 位）：

| 字 | 位域 | 说明 |
|---|---|---|
| DRAW | [31]=1 [30:25] 图槽 [24:0] 偏移 | 用图槽里的 SRC / SSTR / SIZE / CMD 画到 `BASE + 偏移`；一颗弹 = 1 个字 |
| REC | [31:28]=0 [26] ADD [25] INV [24] 缓存（= CMD[6:4]）[23:16] 全局 alpha [15:12] 标志（同 CMD[3:0]）[10] 不入队 [9:0] 字段掩码 | 后面跟掩码里每个置位字段各 1 个字，顺序 DST DSTR SRC SSTR A ASTR SIZE COLOR KEY BASE（bit 0–9）；更新列表上下文，[10]=0 时再入队一条命令 |
| SLOT | [31:28]=1 [26] ADD [24] 缓存 [23:16] 全局 alpha [15:12] 标志 [5:0] 图槽号 | 把上下文当前的 SRC / SSTR / SIZE 和本字的命令位（含缓存位，不含 INV）存进图槽 |
| TAB | [31:28]=2 [15:0] 项数 n，后跟 1 个字 = 表的字节地址（4 字节对齐） | （CAPS[28]）引擎读 n 项 8 B 表：字 0 = {y[31:16], x[15:0]}（只用 x[11:0]、y[27:16]，无符号），字 1 [5:0] = 图槽号；每项 = DRAW(槽, y·DSTR + 2x)。n = 0 只跳过地址字 |
| BANDS | [31:28]=3，后跟 1 个字 = 帧缓冲基地址（256 字节对齐） | （CAPS[31]）开始一个分带帧：之后直到 ENDB 的记录都属于这一帧（不能嵌套） |
| ENDB | [31:28]=4 | 这一帧录完：引擎逐带画并写回，画完 FRAMES 加 1，再继续后面的列表 |
| 其它 | | 置 LERR 并跳过 |

列表上下文和图槽只在硬件复位时清零（DSTR 复位 1280，KEY 复位 0xF81F），写 `LIST_ADDR` 不会清它们。

### 7.3 驱动（`game/src/blit.h, blit.c, gfx_hw.c`）

- `blit.h`：参数寄存器影子副本，只写变化的寄存器；按 STATUS 空位数做 credit，队列不满时 CPU 不会被卡住。
  `blit_fill()` / `blit_copy()` / `blit_wait()` / `blit_dcache_invalidate()`。
  统计：`blit_st.cmds`（已写命令）、`stall_ticks` / `stalls`（队列满等待）、`qpeak`（STATUS 采样到的最大占用），
  `blit_queue_used()` 读取当前排队条数；每次最多用 64 个 credit（`BLIT_CREDIT_MAX`），保证占用量定期被采样。
- `gfx_hw.c`：`GfxOps` 的硬件后端。游戏、HUD、演示场景的全部 `gfx_*` 调用不改一行，
  由 `gfx_begin_hw()` 切到 BitBlt。
- 文字（`gfx.c` 的字模图集）：44 个字模按"颜色 × 放大倍数"预先画成一条位图（背景 = 色键 `0xF81F`，
  第 i 个字在 x = i·5·scale），首次用到时生成，放在 .bss 的 256 KB 池里（游戏实际用到 13 套 ≈ 136 KB）。
  之后每个字符 = 一条带色键的 COPY（只写 DST / SRC / CMD 三个寄存器），不再是每个亮点一条 FILL
  （原来平均 12.5 条/字符）。色键拷贝不读目的，不会触发 alpha 那样的读回等待。
  新图集生成后先 `fence` 再入队；池满时先 `blit_wait()` 再整体清空重建。
  scale > 3（标题、`IMAGE n`）不建图集，改为"横向连续亮点合并成一条、相同的相邻行合并成一个矩形"的 FILL（≈5.6 条/字符）。
  CPU 后端只在 scale 1 用图集，scale 2/3 用合并 FILL（色键拷贝要逐点比较 35·s² 个像素，放大后不如直接填亮点快）；
  两种路径画出的像素完全相同。相关宏：`GFX_FONT_ATLAS`、`GFX_FONT_ATLAS_MAX_SCALE`、
  `GFX_FONT_ATLAS_CPU_MAX_SCALE`、`GFX_FONT_POOL_PIXELS`。
  主机模型统计的每帧命令数：游戏中 3892 → 309，标题 5720 → 483，暂停 4758 → 374，
  MOVING SQUARES 2879 → 672，IMAGE SWITCHING 2589 → 175。
- 批量绘制列表（`gfx_blit_list`，`GfxOps.copy_list` → `gfx_hw.c` 的 `hw_copy_list`）：敌方弹幕在
  `render.c` 里先写进一个 128 项的列表（`GfxItem {x, y, sprite}`，1 KB，留在 D$ 里），每满 128 项提交一次。
  `gfx_blit_list` 把完全在屏幕内的连续项整段交给后端，需要裁剪的单独走 `gfx_blit`，完全在屏外的直接跳过；
  `hw_copy_list` 在一个紧凑循环里提交：影子寄存器和 credit 放在 CPU 寄存器里，整段结束才写回 `blit_st`
  （D$ 写直通，每次 store 都要上总线），换图时才看 SRC/SSTR/SIZE，通常每颗弹只有 DST + CMD 两次 APB 写，
  不再经过 `gfx_blit → 函数指针 → hw_copy → blit_copy` 这条调用链（每颗弹几十条指令 + 十几次压栈/统计 store，
  且 4 KB 直接映射 I$ 里弹幕循环与 `gfx_blit` 地址冲突，来回互相踢出）。
  命令流、像素、FILL/BLIT 计数与逐个调用完全一致（主机测试逐帧比较），只是 CPU 开销不同；`k` 键可现场对比。
- 硬件命令列表（`blit_set_mode()`，`BLIT_MODE_APB / LIST / WORD`）：LIST 模式下 `blit_fill()` / `blit_copy()` 不再写 APB 寄存器，
  而是往 `.bss` 里的 `blit_list_buf`（`BLIT_LIST_WORDS` = 16384 字 = 64 KB，256 B 对齐）追加 REC 记录，只带变化的字段
  （驱动里有一份列表上下文的影子 `blit_st.lctx[]`）；每攒够 `BLIT_LIST_KICK`（32）个字、以及 `hw_end` / `blit_wait()` 时
  `fence` + 写一次 `LIST_END`（kick）。缓冲写满时等引擎读完再从头开始（`blit_list_wrap`，等待时间计入 qstall）；
  每帧开头如果列表空闲就回到缓冲起点（`blit_list_frame`）。
  `hw_copy_list` 在列表模式下走 `hw_copy_list_hw`：图槽号 = sprite 编号 & 63，槽内容变了才补一条 SLOT，
  之后每一项只是一个 DRAW 字（偏移 = y·pitch + x·2），64 项提交一次。
  精灵前缀表（2026-10-04）：弹幕 4 种样式交错，原来"换图"那段（查 Sprite、算 src/size/缓存标志、比较槽表）几乎每颗都跑；
  现在每次调用里编号 < 16 的精灵只在第一次出现时确认/定义槽，DRAW 前缀记进栈上的 `pre[]`，之后每颗只查一次表
  （定义槽时清掉共用该槽的 `pre[]` 项，编号 ≥ 64 的精灵和小编号共用槽也正确）。
  同时 `game_tick` 写弹幕表时顺便判断每颗是否完全在屏内，部分出屏的计数（`game_seed_draw_clip()`）；为 0 时 render 用
  `gfx_blit_list_inside()` 把整张表一次交给后端，省掉 `gfx_blit_list` 逐项边界检查那一遍（有需要裁剪的项时仍走 `gfx_blit_list`）。
  每颗弹的提交约 70 条指令 → 约 17 条；像素、命令数、列表字数与之前完全相同（主机测试 + 600 轮随机列表测试）。
  板上实测（2026-10-04，最高压力档约 3800 颗）：68 → 约 82 fps，seeds 7.05 → 2.75 ms，submit 10.3 → 6.8 ms。
  引擎读表（B 方案，2026-10-04）：`gfx_blit_table()`（`GfxOps.copy_table` → `hw_copy_table`）在 LIST 模式、引擎有 CAPS[28]、
  精灵数 ≤ 64 时，把精灵 0..n-1 放进槽 0..n-1（标签相同不重定义），设好 BASE/DSTR，然后只写 `TAB(n)` + 表地址两个字并立即 kick；
  否则自动退回 `gfx_blit_list_inside`，画面相同。`render.c` 在整表都在屏内时用它画弹幕。表在引擎画完前不能改，所以
  `game.c` 的弹幕表改成两张轮流写（`seed_draw[2][]`，`seed_draw_buf`）：重叠模式下"提交 → 一次 game_tick → blit_wait"，
  game_tick 写的是另一张。主机测试：像素、命令数与 DRAW 路径完全相同，列表字数 41830 → 32289；600 轮随机列表测试里一半调用走表。
  `9` 键（调试版）现场切换 TAB / 逐颗 DRAW 做对比。
  测量用的"攒着不发"（2026-10-05）：`blit_st.lhold = 1` 时 `blit_list_kick()` 不写 `LIST_END`，`blit_list_frame()` 也不回到缓冲起点，
  写好的字都留在 DDR 里等 `blit_list_release()` 一次发布；`blit_wait()` / `blit_busy()` / 缓冲写满回绕（`blit_list_wrap`）会自动解除，
  所以忘了 release 也不会卡死。`blit_perf_snap()` / `blit_perf_diff()` 一次读出全部性能计数（`BlitPerf`，含 8 个停顿计数）。
  只给 `8` 键剖析用，正常帧不设置 lhold。
  WORD 模式生成完全相同的字流，但逐字写 APB `LIST_WORD`（用来在板上区分"DDR store 的开销"和"APB 写的开销"）。
  三种模式的像素和命令条数完全一致（主机测试逐帧比较）。最高压力档主机统计：2924 条命令 → 4052 个字 / 帧。
  板上实测（最高压力档，约 3900 颗弹）：`list` / `hwlist` / `hwword` 三种方式每颗弹都是约 2.9 µs，`hwlist` 下 hud、scene 反而各慢 2–3 ms
  （34–36 fps → 29–30 fps），当时默认仍是 `list`。换成 32 KB 8 路 I$/D$ 之后（2026-10-03）`hwlist` 最快
  （最高压力档约 55 fps，引擎忙 67%），默认改为 `hwlist`；比特流不带命令列表时自动退回 `list`。瓶颈不在命令怎么送给引擎，而在 CPU 自己的访存
  （D$ 写直通，每次 store 都是一次 DDR 总线写）；启动时的 `memory access cost` 表给出单次 store / load / APB 访问的实测开销。
  注意：引擎读的是 CPU 刚写进 DDR 的数据，依赖 D$ 写直通 + `fence` 之后数据已经到达 DDR；
  如果板上出现花屏或 `lerr 1`，先用 `k` 切到 `hwword`（不经 DDR）对比。
- CPU 与引擎抢 DDR（板上 `memory access cost` 实测，ns/次，引擎空闲 / 引擎在做整屏拷贝）：
  DDR store 70–100 / 620，D$ 缺失重填 740 / 4730，APB 写 90 / 96，片上 RAM 读写 70–91 / 70–96（空循环本身 60）。
  也就是引擎空闲时 store 几乎免费，引擎忙时每次 store 多等约 56 拍、每次 D$ 缺失多等约 400 拍。
  原因：SoC 内部的 AXI 仲裁只对地址通道轮转，引擎一次最多挂起 16 个读 burst（行缓冲 1024 字）和上百个写 burst，
  这些都已经排进 DDR 控制器，CPU 之后到达的访问要等它们全部做完。所以游戏里"引擎在画、CPU 同时在准备下一批命令"时，
  CPU 的每次 D$ / I$ 缺失和每次 store 都被拖慢 —— 这才是每颗弹 2.9 µs、每条 HUD 命令十几 µs 的主要来源，与命令走 APB 还是 DDR 列表无关。
  `LIMIT` 寄存器（`blit_set_limit(rd, wr)`，按键 `o` / `i`）限制引擎挂起的 burst 数：调小后 CPU 等得短，但引擎小图吞吐下降
  （仿真、随机 DDR 延迟：不限 ≈ 103 拍/个，4/4 ≈ 176，2/2 ≈ 305，1/1 ≈ 550）。最佳值要在板上看启动时的 `engine LIMIT` 表和实际 fps 来定。
  板上（最高压力档）：只有读上限起作用，写上限 128→2 没有可见差别；读 2 时 fps 36→39，HUD 3.8→1.95 ms，
  但弹幕只从 2.9 降到 2.6 µs/颗，引擎忙碌从 38% 升到 68%（约 17 ms/帧，60 fps 时就不够用了），小图吞吐慢 1.7 倍。
  原因是按 burst 个数限流对大小图一视同仁：CPU 等的是排在它前面的**字节数**（整屏拷贝每个 burst 64 字），
  而小精灵每行只有 2..6 字，需要多个 burst 同时挂起来掩盖 DDR 延迟。所以 LIMIT[26:16] 改为按"在途读字数"限流
  （`blit_set_rdwords(n)`，按键 `u`）：整屏拷贝最多在 CPU 前面排 n 字（+ 一个 burst），小精灵仍可挂 16 个 burst。
  仿真（随机 DDR 延迟）9×9 色键小图：不限 101 拍/个，读字数 64 → 104，32 → 128（对比读 burst 2 → 303）。
  游戏默认 `rd 16 wr 128 words 64`（`GAME_LIMIT_WORDS`，板上 `u` 实测 64 最好）；老比特流没有该字段时退回 `rd 2 wr 2`。
- 源缓存驱动（`blit_set_cache()`，`blit_cache_flag(w)`）：比特流有 CAPS[27] 时默认打开，宽 ≤ `BLIT_CACHE_MAX_W`（64）像素的 COPY
  带缓存位（整屏背景、大图不带，免得把小图挤出去）；图槽的 tag 里含缓存位，开关缓存后图槽会自动重登记。
  INV（`blit_cache_inv()`，APB 下是尺寸 0 的 CMD，列表里是尺寸 0 的 REC）在三处发：每帧 `gfx_begin_hw` 开头、
  字模图集新生成 / 重建发布时、`blit_set_cache()` 时。CPU 每帧都可能改的源（如果以后有）也必须在改完、入队前 INV。
  `STATS=1` 版可用 `0` 键开关做 A/B，串口 `src cache` 行给出命中率。
- 宽通路（C2）驱动：`blit_init()` 看到 CAPS[30] 时置 `blit_st.has_wide` 并写 `BLIT_WIDE_RESET`（开、MINW 128、WTH 0、在途读 256 字）。
  `blit_set_wide(v)` 写 WIDE 寄存器并保存在 `blit_st.wide`（字数字段为 0 时按 1 写），`BLIT_WIDE(minw, wth, words)` 拼值、`BLIT_WIDE_OFF` 关闭。
  驱动和 `gfx_*` 都不需要改：哪条命令走宽通路由硬件按条件自己判断，像素与 32 位通路完全相同。
  `BlitPerf` 多了 `wclk / xwbeat / xrbeat`，`blit_perf_snap()` / `blit_perf_diff()` 在有 CAPS[30] 时一并读出。
- 分带合成（D）的驱动：`blit_set_band(1)` 之后（需要 LIST 模式），`gfx_begin_hw()` 在帧开头自动写 `BANDS` + 帧缓冲地址（`blit_band_begin`，
  目标须 256 字节对齐、行距 256–1280 且为 16 的倍数、行数为 16 的倍数且 ≤ 512，否则这一帧照旧直接画），`gfx_end()` 写 `ENDB` 并发布（`blit_band_end`）。
  `gfx_*`、render、game 都不用改。几点细节：
  - 一帧要连续放在列表缓冲里：开帧前剩余不足 `BLIT_BAND_ROOM`（缓冲的一半）就先回到开头；缓冲末尾始终留 1 个字，
    帧中途万一写满，先补一个 `ENDB`（这一帧到此为止先画出来，`blit_st.bsplits` 加 1），剩下的命令直接画——画面不变，只是这一帧不走流水。`BLIT_LIST_WORDS` 16384 → 65536。
  - `blit_wait()` / `blit_busy()` 遇到打开的帧会先结束它（否则引擎永远不空闲）。`blit_band_done(seq)` 只读 `FRAMES`，不等待。
  - **流水帧**（`main.c`）：这一帧 `gfx_end()` 之后不 `blit_wait()`，直接跑逻辑 / 进下一圈循环录下一帧；每圈末尾等的是**上一帧**（`pend_fb` / `pend_seq`）画完再翻页，
    如果这一帧也已经画完（有 tick 的帧）就一起翻。所以引擎画一帧的时间和 CPU 录下一帧（+ 逻辑）是重叠的：没有 tick 的帧周期 ≈ max(CPU 提交, 引擎一帧)。
    代价是多一帧显示延迟，以及多一个帧缓冲（共 4 个：显示中、等翻页、引擎在画、CPU 在录）。弹幕表 `Game.seed_draw` 相应由 2 张改为 3 张轮流写。
    切到 CPU 后端 / 关分带 / 帧被拆开时，自动先把挂着的那一帧等完。vsync 开着（60 fps 封顶）时不流水：每圈等这一帧画完再翻页，和以前一样，只是引擎画得更快。
  - 串口每秒多一行 `band ON: phase1 … us + bands … us (write-back …) /frame | K busy … | … bin records | split frames … | ok`（引擎计数器，除以帧数）；
    `8` 键先在"分带关"下测 bg / seeds / frame / rest 四行（与以前可比），再把整帧按分带画一次，打印 `band frame: engine … us = phase1 … + bands …` 和相对直接画的倍数。
  - 发行版（STATS=0）也用分带 + 流水（串口行显示 `HW band`）；`make BAND=0` 开机不用。
- 加法混合的软件接口（2026-10-11）：`gfx_fill_add(g,x,y,w,h,颜色,alpha)`、`gfx_blit_add(g,x,y,精灵,色键,alpha)`（有 A8 平面时也参与），
  批量接口 `gfx_blit_list / _inside / gfx_blit_table` 的最后一个"色键"参数现在是模式位：`GFX_BLIT_KEY`（=1，原来的色键）| `GFX_BLIT_ADD`（批量加法，不用 A8 平面）。
  CPU 后端用完全相同的整数公式，所以 CPU / 引擎画面逐像素一致（`host_list_test` 随机比对）。驱动：`BLIT_ADD`、`blit_fill_add()`，`blit_st.has_add`（CAPS2[0]），
  精灵槽标签多一位（加法），没有 CAPS2[0] 的旧位流上自动退化成普通拷贝 / 透明混合。统计里加法算作 alpha 操作。
- 按键：`src/pad.c / pad.h` 解析 `pc_pad.py` 的状态包（`pad_feed` 在 `poll_uart` 里先过一遍每个字节，`pad_held / pad_pressed / pad_active`）。
- CPU 缓存：Sapphire IP 默认 I$ / D$ 各 4 KB 直接映射，而每帧的热代码约 30 KB（render_game 9.6 KB、game_tick 9.3 KB、draw_world 3 KB、gfx / hw 后端约 8 KB），
  板上（最高压力档）每个敌人的绘制 ≈ 55–65 µs、每条非弹幕命令 ≈ 10–15 µs，主要是 I$/D$ 缺失（引擎忙时每次 ≈ 2.8 µs）。
  在 Efinity IP 配置里把缓存加大（Cache/Memory 页：I$ / D$ 1–32 KB，1/2/4/8 路），重新生成 IP 后 `ip/soc/soc.v` 和
  `embedded_sw/soc/bsp/.../soc.h` 一起更新，再重新综合并 `build_game.bat`。启动时打印 `caches (BSP soc.h)` 一行（编译时的 BSP 配置）和
  `D$ probe`（实测：对 2K…128K 的区域第二遍读取每行 ns，命中约 90、缺失约 740，第一个跳到缺失值的大小 = 2 × 实际 D$ 容量），用来确认板上比特流和 BSP 一致。
- 双缓冲 / 三缓冲：CPU 只负责把一帧的命令入队 → `blit_wait()` → `hdmi_flip()`，
  翻页在 vsync 生效，屏幕上永远看不到画了一半的帧。
- Cache 一致性：D$ 为写直通，BitBlt 读 CPU 写的数据前只需 `fence`；
  由 HW 切回 CPU（或测速之后）执行一次 D$ 失效（`.word 0x500F`）。

### 7.4 演示与对比

| 场景（`n` 切换） | 内容 |
|---|---|
| 游戏 | SproutRun 原游戏，含阴影 / 暂停遮罩 / 半透明 HUD / 无敌残影（`e`） |
| MOVING SQUARES | N 个随机移动方块（默认 256，`-`/`+` 调整），`e` 打开后每隔一个半透明 |
| IMAGE SWITCHING | 每帧整屏 640×480 拷贝，3 张图轮换（间隔 `-`/`+` 调整，1 = 每帧换图） |

- 屏幕底栏同时显示 `CPU x FPS y MS` 和 `HW x FPS y MS`（各自最近一次测到的值，当前模式高亮）；
  按 `c` 自动交替测量，满足"屏幕上显示 CPU 与 HW FPS 对比"的要求。
- 统计面板（`f`）显示 submit / qstall / tail_wait / vsync_wait、队列占用、`BLT%` 与 AXI 读写 MB/s（字段含义见第 4 节）。
- 启动时串口打印同一组 `gfx` 调用在两个后端上的耗时与带宽：全屏填充、全屏拷贝、全屏 alpha、2000 个 8×8 小方块；
  另有 5 行 2000 个 9×9 色键小图的提交对比（APB 逐个 / APB 批量 / DDR 列表逐个 / DDR 列表批量 / LIST_WORD 批量）：
  queued（CPU 提交完）/ done（引擎画完）时间、列表字数、kick 次数。
  串口每秒的 `blit:` 行给出队列占用、stalls、实际 AXI 读写吞吐。
- 仿真吞吐（理想 DDR）：fill ≈ 1.95 像素/时钟，copy ≈ 1.77，alpha ≈ 0.90；
  即全屏 fill ≈ 1.6 ms、全屏 copy ≈ 1.7 ms、全屏 alpha ≈ 3.4 ms（实际受 DDR 与 HDMI 抢占影响）。
  有 128 位宽通路（C2）后，不透明且同相位的 fill / copy ≈ 7.5 / 7.0 像素/时钟（全屏 copy ≈ 0.43 ms），alpha 不变。

### 7.5 验证

```
cd sim/blit
python3 tb_blit.py [seed] [命令数] [random|ideal]   # RTL 周期级仿真，对比 golden model
TB_WFIFO=4 python3 tb_blit.py 4                    # SoC W FIFO 只有 4 深（比 burst 短）
TB_WSTRICT=1 python3 tb_blit.py 3                  # 从机必须先收 AW 才收 W（走保底路径）
TB_WIDE=off python3 tb_blit.py 2                   # 关掉 128 位宽通路（默认每次随机 WIDE 设置）
TB_WIDE=0,15,16 python3 tb_blit.py 6               # WIDE = MINW/8, WTH[, 在途读字数]
BLIT_LIMIT=1,1,16 TB_NSPR=100 python3 tb_blit.py 4 10   # 慢 LIMIT 下把 600 颗小图段缩成 100 颗
```

仿真顶层 = 引擎 + 读仲裁 + `blit_axi_wide`，从机模型是 128 位的 SoC 口：W 先进 FIFO（默认 16 深），AW 到了再写进内存；
AR/AW/W 停住时 valid 和内容不许变；128 位 AR/AW 检查 size 4、16 B 对齐、不跨 256 B、len 与 32 位侧一致；
32 位侧逐字检查 R 数据 / RID / RLAST（HDMI 和引擎都查），引擎的 AW/W 照旧检查 burst 长度、边界、WLAST。

测试项（全部对比 golden model）：1 随机命令混合（fill / copy / 色键 / alpha / A8）；2 大量极小命令把队列填满（APB 被挂起、不丢命令）；3 最宽的行和奇数对齐；4 命令列表（随机 REC / SLOT / DRAW，
DDR 列表、`LIST_WORD`、直接 APB 命令混合，中途重设 `LIST_ADDR`；结束时检查 STATUS 低 5 位为 0、ptr == end）；
5 非法命令字置 LERR、CTRL[1] 清除；6 600 个 9×9 色键小图 APB 与列表两种入口的吞吐；7 大块吞吐；
8 源缓存：600 个带缓存的 9×9 小图（APB / 列表，检查命中 / 缺失计数），CPU 改写源图后不 INV 仍画旧图、CMD[5] / REC[25] INV 后画新图。
测试 1 里随机 40% 命令带缓存位、3% 带 INV，测试 4 的图槽一半带缓存位，像素必须与 golden model 一致。
测试 4 里还随机插入 TAB 精灵表（环境变量 `TB_TAB`，默认 0.3 = 30% 的绘制段改用表；项数含 0/1/31/32/33 等边界，
表项的保留位填随机垃圾，检查引擎只用 x[11:0]、y[27:16]、精灵号[5:0]），表放在 DDR 不同位置，和 DRAW / REC / LIST_WORD 混在一起；
测试 6b 是 600 个 9×9 色键小图走一条 TAB 的吞吐，并检查 CAPS[28]。
测试 6、6b、7 和带缓存的小图测试还打印 BUSY 的拆分（停顿计数器，检查 CAPS[29]），并断言 `BUSY = WBEAT + PXWAIT + PXFULL + PXBUB + PXIDLE`。
环境变量 `BLIT_LIMIT=rd,wr[,words]` 让整个仿真在该 LIMIT 下运行，并检查挂起的 burst 数不超过上限、在途读字数不超过 words + 63。
宽通路（C2）：从机模型也检查 x_* 口（AR / AW 128 位对齐、不跨 256 B、宽 R 拍逐拍比对数据和 RLAST、宽 W 必须跟在它自己的 AW 后面、strobe 不全 0，
32 位与宽 AR / AW 不在同一拍）；测试 1 里一半 COPY 故意让 src / dst 同相位，WIDE 设置默认随机（`TB_WIDE` 可固定）；
测试 9 覆盖 16 B 内全部 8 种像素相位、奇数行边、宽 1–1100、FILL / COPY、各档 WTH 和宽通路在途读字数，并断言真的走了宽通路（XWBEAT、WCLK > 0）；
测试 7 的大块吞吐宽通路关 / 开各测一遍。BUSY 等式多了一项 WCLK。`TB_NSPR` 设定测试 6 / 8 的小图数量（默认 600），慢 LIMIT 下用来缩短时间。
2026-10-08 全部通过：随机 seed 1–4（N 10 和默认 N）、`TB_WSTRICT=1`、`TB_WFIFO=4`、`BLIT_LIMIT=4,3,64`、`BLIT_LIMIT=1,1,16`（`TB_NSPR=100`）、
`TB_WIDE=off`、`TB_WIDE=0,0`、ideal seed 5，以及主机 `host_blit_test` / `host_list_test`。
小图吞吐（仿真）：随机 DDR 延迟约 105 拍/个，理想 DDR 约 70 拍/个，两种入口相同 —— 仿真里瓶颈是引擎本身，
命令列表省的是 CPU 的提交时间。

主机端（任意 gcc，无需 RISC-V 工具链），在 `game/` 下：

```
gcc -O2 -no-pie -fno-pie -DBLIT_HOST_MODEL -DGFX_FONT_ATLAS_CPU_MAX_SCALE=3 \
    -Isrc -Itools -o host_blit_test \
    tools/host_blit_test.c src/game.c src/gfx.c src/render.c src/demo.c \
    src/gfx_hw.c src/blit.c src/mini_snprintf.c
./host_blit_test [输出目录]
```

同一帧分别用 CPU 后端和 "驱动 + blitter C 模型" 渲染并逐像素比较：不透明路径必须完全一致（当前 0 差异），
alpha 路径 CPU 用 5 位 alpha、硬件 8 位，允许每通道 ≤2 级误差。
每帧另外用硬件后端画两遍（不带统计面板，因为面板上写着 LIST/CALLS）：弹幕批量列表 vs 逐个 `gfx_blit`，
像素和命令条数都必须完全相同。
再用硬件命令列表的 LIST（DDR）和 WORD（`LIST_WORD`）两种模式各画一遍：像素、命令条数必须与 APB 模式相同，
且结束时 ptr == end、LERR = 0。加 `-DBLIT_LIST_WORDS=300 -DBLIT_LIST_KICK=7` 可以强制测试缓冲回绕和不规则 kick 点。
C 模型（`tools/blit_model.h`）也带同样的源缓存（512 行、不监听写、只有 INV 清除），所以驱动漏发 INV 会在主机测试里表现为旧像素；
测试里有一个"字模图集池满重建"的场景专门检查这一点。加 `-fsanitize=address,undefined` 可做内存越界检查；
加 `-DSPROUT_STATS=1` 编译时额外比对 `z` 两种弹幕来源和 `y` 剖析的各部分（发行版没有这些代码）。
游戏场景每帧还测一次 `8` 键用的"攒着不发"：设 lhold 画整帧，发布前目标缓冲必须一个像素都没画、`LIST_END` 没动，`blit_list_release()` 后与正常列表画面完全相同（小缓冲版本因为回绕会提前发布，只检查最终画面）。
`-DGFX_FONT_ATLAS_CPU_MAX_SCALE=3` 让 CPU 后端走与硬件相同的文字路径，否则统计面板里 `FILL/BLIT` 计数的数字不同，
那一行文字会被报成差异（字模像素本身是一致的）。

分带合成（D，2026-10-10）的验证：

```
cd sim/blit
python3 tb_band.py [seed] [random|ideal]            # 只跑分带的测试（tb_blit.py 第 10 段），TB_BFRAMES / TB_BITEMS 设帧数和每帧项数
TB_KOFF=1 python3 tb_band.py 7                      # K 通路关
TB_WIDE=off TB_WSTRICT=1 python3 tb_band.py 8       # 宽通路关 + 严格从机
BLIT_LIMIT=1,1,16 TB_BITEMS=25 python3 tb_band.py 9 # 最慢的 LIMIT
python3 tb_bperf.py 5 1650 ideal                    # 吞吐：同一帧直接画 vs 分带（TB_PAN = 半透明面板数）
TB_BAND=0 python3 tb_blit.py 1                      # 原来的 1–9 段，不跑分带（整套一起跑也可以，时间较长）
```

- `tb_blit.py` 第 10 段 / `tb_band.py`：13 带 × 1280 字节的帧，随机帧 = 整屏同相位背景（宽通路）+ 随机 fill / 半透明 fill / 带缓存的 copy 和色键 copy（K 通路）/
  不带缓存的 copy、色键、A8、全局 alpha（窄通路）/ 图槽 + DRAW + TAB（跨带的精灵）/ 画别的目标的命令 / 帧内 INV，分几次发布 LIST_END；
  golden model 把帧缓冲先清成黑色再按顺序画，整个内存逐字节比较（桶区从引擎一侧同步）。另有：三帧连续排队（中间夹直接列表命令）、
  换一种几何（行距 640、6 带、另一个地址）、行距不对的帧内命令（丢弃 + STATUS[7]）、桶溢出（STATUS[6]）、分带之后普通列表仍然正常；每帧检查 FRAMES 和 STATUS。
  x_* 口上桶和写回的 AW / W 由原有的协议检查覆盖（16 B 对齐、不跨 256 B、WLAST、strobe 不拆像素、W 跟在自己的 AW 后）。
- 2026-10-10 通过（审查修改之后重跑）：`tb_band.py` 随机 seed 2 / 3 / 4、ideal 5、`TB_KOFF=1`、`TB_WIDE=off TB_WSTRICT=1`、`TB_WFIFO=4 BLIT_LIMIT=4,3,64`、`BLIT_LIMIT=1,1,16`；
  原 1–9 段随机 seed 1、ideal seed 5（直接画的行为和周期数与改动前一致）；`tb_boot.py`。慢配置（宽通路关、严格从机、最小 LIMIT）加 `TB_BEXTRA=0`
  跳过"连续三帧 + 第二种几何"两项，否则一次跑不完 3 分钟。
- 主机：`host_blit_test` 每个场景多画一遍分带帧（BANDS..ENDB），像素、命令条数必须与直接画的列表帧完全相同，模型里恰好多 1 帧、没有会让真引擎卡死的操作（`bm_fault`）；
  小缓冲版本（`-DBLIT_LIST_WORDS=300`）覆盖"帧中途回绕被拆开"。`host_list_test` 每 6 轮有一轮整帧走分带。C 模型按可见规则实现分带：BANDS 把帧清黑、帧内命令裁剪到帧内、
  行距不对丢弃并置 berr、ENDB 计帧；在帧打开时写 CMD / LIST_ADDR 或死等 BUSY 会记 `bm_fault`。
- 主循环的流水逻辑没法在主机上直接跑板子，所以加了 `tools/host_main_test.c`：把真正的 `src/main.c` 和引擎模型编在一起，模拟时钟（每次读 CLINT / 读引擎寄存器前进）、
  模拟 HDMI（每 1/60 s 锁存翻页）、按脚本注入按键（v l 3 5 q g 8 y k 4 0 n e），模型里分带帧在 ENDB 之后 0.3–6 ms 才"画完"。检查：翻页请求的帧必须已经画完；
  不能往正在显示 / 等翻页的缓冲里画；没有 stuck 打印、lerr、bm_fault。STATS=1、STATS=0、小列表缓冲三种编译都通过（这个测试抓到过一个切到 CPU 后端时没等引擎的问题）。

```
gcc -O2 -w -no-pie -fno-pie -DBLIT_HOST_MODEL -DSPROUT_STATS=1 -DBOOT_DIAG=0 -Itools/mock -Isrc -Itools \
    -o host_main_test tools/host_main_test.c src/game.c src/gfx.c src/render.c src/demo.c \
    src/gfx_hw.c src/blit.c src/mini_snprintf.c
./host_main_test [uart.log]
```

加法混合（2026-10-11）的验证：tb 的 golden model 加了 ADD；随机命令 15% 带 ADD（包括 alpha、A8、色键、填充），列表里 20–30% 的图槽带 ADD；
分带测试多了 `addc / addkc / addf / addga` 几种命令；`add_overlap()` 在分带和直接画两种模式下各画一组互相重叠的加法精灵（TAB + DRAW + 加法填充），
专门检查 K 通路"读还没写回的拍"的等待逻辑。`TB_ADDSEED=1 python3 tb_bperf.py 5 1650 ideal` 是加法弹幕的吞吐。全部回归（直接 / 分带、随机 / 理想、
严格从机、W FIFO 4、各档 LIMIT、K 通路关、宽通路关）重跑通过。主机：`host_list_test` 每轮随机三分之一的批量调用带 `GFX_BLIT_ADD`，再加 12 次任意 alpha、
可能带 A8 平面、会被裁剪的 `gfx_blit_add / gfx_fill_add`，CPU 与引擎模型逐像素相同（ASan）。`host_main_test` 注入 `pc_pad` 状态包（含一个校验错的包），
检查收包计数和"按方向键退出自动驾驶"（编译命令里要加 `src/pad.c`）。

## 8. 东方式弹幕 SPROUT DANMAKU（2026-10-11，开机场景）

开机直接进这个游戏；`n` 键依次切到 SproutRun / 方块 / 图片切换（都保留）。**不需要新位流**，
只用到已有的引擎功能（分带合成、TAB 弹幕表、加法混合、128 位宽通路）。

### 8.1 内容（第一版：一关竖切片）

| 部分 | 内容 |
| --- | --- |
| 画面 | 左 384x448 游戏区（屏幕 x 32..415，y 16..463）+ 右侧信息栏（最高分 / 得分 / 残机 / Bomb / 火力 / 擦弹 / 得点 / 弹数 / FPS） |
| 流程 | 标题（NORMAL / LUNATIC / STRESS，压力档位见 8.4）→ 道中约 54 秒（4 种杂兵波次 + 大妖精）→ Boss 3 非符 + 3 符卡 → 过关结算；Game Over |
| 系统 | 低速 + 判定点、擦弹、决死（中弹后 8 tick 内按 Bomb）、Bomb（全屏消弹变星、无敌）、火力 0.00–4.00 与 0–4 个子机（高速诱导弹 / 低速集中）、P 点 / 得点 / 大 P / Bomb / 1UP 道具、上线自动收点、符卡收取奖励（中弹或 Bomb 则失败，奖励随时间降到 1/3）、限时、中弹后无敌 |
| 弹型 | 小弹、中弹、米弹、苦无（各 16 个角度预转好）、星弹（8 帧自转）、大玉、泡弹（后两种加法发光）；每种 8 色 |
| 弹幕行为 | 加速 / 减速、拐弯（角速度）、定时改向（可重新瞄准自机）、反弹；`th_ring` / `th_fan` 按难度倍率出弹（Lunatic x2，Stress x1–x48 可调，见 8.4） |
| 演示 | 自动驾驶（`b`，开机默认开）轮流跑 Stress 和 Normal，演示局自机无敌；Stress x32 峰值约 1 万颗弹 |

### 8.2 文件（`game/src`）

| 文件 | 作用 |
| --- | --- |
| `th.h` | 类型、常量、接口 |
| `th_game.c` | 逻辑：自机、子机、自机弹、杂兵、道具、特效、弹幕池热循环（移动 + 判定 + 分桶写绘制表）、Boss 状态机、自动驾驶 |
| `th_pattern.c` | 道中时间表、5 种杂兵行为、Boss 6 个阶段的弹幕（加关卡 / 符卡只改这里） |
| `th_art.c` | 开机时 CPU 画出弹型、特效、背景、边框，并把 `th_pix.c` 的人物像素图转成精灵（妖精按颜色换发色 / 裙色） |
| `th_pix.c` | **生成的**人物像素图：自机 4 帧（左倾 / 站立 / 右倾 / 站立 2）、妖精和大妖精各 2 帧翅膀、Boss 2 帧（符卡中举手）。改人物：编辑 `tools/art/player.txt`、`fairy.txt`（左半边，自动镜像、自动描黑边）或 `tools/th_pixgen.py`（翅膀、Boss），再跑 `python tools/th_pixgen.py`，会顺带出 `tools/art/preview.png` |
| `th_render.c` | 绘制顺序和 HUD |
| `th_tab.c` | 生成的 sin（Q14，1024 项）/ atan 表 |

弹幕的画法：`bullets_update` 在同一遍循环里给每颗可见的弹写一个 `GfxItem` 并记下它的桶（9 个桶，
每桶 ≤ 64 张图），再按桶计数排序进本 tick 的绘制表（3 张表轮流写，引擎可能还在读前两张）。渲染时每桶
**一次** `gfx_blit_table`：CPU 每桶只写几个列表字，引擎自己读表。整帧约 1000 个列表字，和弹数无关。

### 8.3 操作

| 输入 | 键 |
| --- | --- |
| `tools/pc_pad.py`（真实"按住"） | 方向键 / WASD 移动，Z 或空格 射击（菜单确定），X Bomb，Shift 低速，Esc 暂停，Enter 开始 |
| 只有串口终端 | `w a s d` 锁定方向（`x` 停）并自动射击，`1` Bomb，`2` 低速开关，回车 确定，`p` 暂停，`r` 重来，`m` 回标题，`l` 直接开 Stress 演示，`-` `+` 压力档位（手动），`*` 压力自动档开关，`b` 自动演示开关 |

从自动演示接手（按了方向 / 射击键）时回到标题选难度。串口每秒多一行 `danmaku: ...`
（模式、关卡 tick、Boss 阶段、弹数 / 峰值 / 丢弃数、得分等）。

### 8.4 压力档位与"稳 60 帧"自动档

压力模式的弹数 = 同样的弹幕 x 档位，档位 x1 2 3 4 6 8 10 12 16 20 24 32 40 48（`th_stress_levels`），
开机 x16、自动档开。信息栏显示 `STRESS X16 AUTO` 和 `LOAD 上个 0.5 s 最慢一帧占预算的百分比  CAP 弹数上限`，
串口每秒多一行 `stress: ...`（档位、最慢一帧 us、上限、降 / 升档次数）。

- **手动**：`-` / `+` 换档（同时关掉自动档，弹数上限恢复 16384）。看 LOAD：稳定在 90% 以下就不会掉帧。
- **自动**（`*` 开 / 关，`th_stress_frame`）：每帧把"除了等 vsync 以外的全部时间"（逻辑 + 提交 + 等引擎）交给它，预算
  `TH_FRAME_BUDGET_US` = 16667 us（main.c，可在 makefile 里 `-D` 改）：
  - 单帧超过 97%：弹数上限 = 现有弹数（不再增长，下一颗 `th_fire` 就生效）；超过 125%：上限 = 现有的 90%；
  - 每 30 tick（0.5 s）看这段里最慢的一帧：超过 92%，或这段里上限挡掉了子弹（弹幕会缺一块），降一档；
  - 连续 6 段（3 s）都低于 70% 且上限已放回 16384：升一档；刚因超时降下来的档位 20 s 内不再升回去，
    同一档再失败依次 40 / 80 / 160 s——免得道中空闲时升上去、到了密的符卡又被打回来；
  - 换档只影响之后发的弹，已经在屏幕上的弹还要飞 2–4 s，所以靠上限做即时保护、靠降档做长期调整。
  - 窗口和冷却按游戏 tick 计，不按帧（vsync 关时一个 tick 有好几帧）。

主机上用假设的成本模型（每帧 = 固定开销 + 每颗弹的开销，±4% 抖动）跑完整一局 Stress：60 帧上限分别约
24000 / 11400 / 6800 / 3600 颗时，超预算帧 0 / 0 / 0.06% / 0.02%，没有一帧超过 125%。板上的真实成本不是线性的
（发光弹、泡弹更贵），但自动档只看实测时间，不依赖这个模型。

### 8.5 主机验证（`tools/th_host.c`）

```
gcc -O2 -no-pie -fno-pie -DBLIT_HOST_MODEL -Isrc -Itools -o th_host tools/th_host.c \
    src/th_tab.c src/th_game.c src/th_pattern.c src/th_art.c src/th_render.c src/th_pix.c \
    src/gfx.c src/gfx_hw.c src/blit.c src/mini_snprintf.c
./th_host shots/
```

自动驾驶跑完 Normal / Lunatic / Stress 三局（都要到过关画面）+ 一局会死的脚本玩家（要到 Game Over），
每 37 tick 用 CPU 后端和引擎模型（分带开 / 关交替）各画一帧逐像素比较，并存各阶段截图（`game/preview_th/`）。
结果：1604 帧全部一致（其中 9 帧含半透明遮罩，CPU 是 5 位 alpha、引擎 8 位，差 1–2 LSB，按容差算过），
Stress（固定 x32）峰值 10221 颗、无丢弹；最后再用 4 种成本模型跑自动档（见 8.4）。ASan / UBSan 版同样通过。
编译命令里要加 `src/th_pix.c`（`host_main_test` 也是）。`host_main_test`（真 main.c + 流水帧）两种 STATS 都 PASS。

没有做的：RISC-V 交叉编译（这台机器没有工具链）和上板——板上帧率、开机画图耗时都还没有实测数字。

### 8.6 压力模式性能（2026-10-11 板上实测后的修改）

板上 x20、"星符 双子星河"（黄 + 青星弹）6200 颗只有 59 帧：引擎 busy 99%（分带 16 ms/帧，其中 K 通路 14.9 ms），
源缓存每帧 3.07 万次缺失（31%）；CPU 逻辑 9.6 ms/tick。

- **引擎：精灵按桶连续存放**（`th_art.c`）。源缓存是 16 KB 直接映射（地址位 13:5 选行），原来弹型按颜色交错分配，
  黄星和青星的 8 帧正好落在同一批缓存行上，一带里交替画就互相踢出；每次缺失约 90 个引擎时钟（3 万次 ≈ 15 ms）。
  改成"所有小弹 → 所有中弹 → 所有星弹 → 米弹 → 苦无 → 大玉 → 泡弹"依次分配后，按分带顺序模拟（`th_host` 里的
  `play_cache`）：x32 这一阶段 9559 颗、每帧 14.1 万次行请求，缺失从约 2.5 万降到 2470（1%），引擎估计 ≈ 1.4 ms 行请求 + 1.1 ms 缺失。
  其他阶段都在 1% 左右。`th_host` 超过 10% 就判失败。
- **CPU：弹幕循环直接按桶写表**（每桶一张 16384 项的表，3 套 3.4 MB），省掉"先写临时表 + 桶号、再按桶搬一遍"；
  桶号和图号在发射 / 改角度时算好存在子弹里（`th_bullet_sprite`），循环里不再按种类分支，星弹转动帧由 tick 算出（不再每 tick 写回）；
  自动驾驶的躲弹力在弹幕循环里顺便累加（以前每 tick 另外跨步读 2000 颗子弹，每颗一次缓存缺失）；
  道具 / 特效改成紧凑数组（以前每 tick 扫 1024 + 1536 个槽，渲染时特效还按种类扫 5 遍，都是 DDR 缺失）。
- 串口 `stress:` 行新增本 tick 的分段时间：`fire+boss+enemies`、`shots+bullets`（含每颗 ns）、`items+fx`，板上可以直接看瓶颈在哪。


板上结果（2026-10-11 改完后）：x48 手动、同一张符卡 1.5 万颗（画 1.37 万）时引擎每帧 8.6 ms（分桶 2.2 + 逐带 6.4，K 通路 5.2 ms），
源缓存命中 97%；瓶颈换成 CPU 逻辑：弹幕循环约 1 us/颗（14 ms/tick），整帧 work 约 16 ms，刚好 60 帧。

### 8.7 半透明（alpha）特效（`e` 开 / 关，默认开）

以前弹幕里几乎只有加法发光；现在用上引擎的 alpha 混合（全局 alpha + A8 逐像素 alpha，走 2 像素/拍的窄通路）：

| 效果 | 做法 |
| --- | --- |
| 符卡宣言立绘切入 | Boss 举手帧放大 2 倍（128x160），A8 平面下沿渐隐；从右侧滑入、停留、上飘淡出（约 1.8 s），画在弹幕后面 |
| 符卡魔法阵 | 128x128 的 A8 魔法阵（双环、八芒星、符文带，4 帧转动）垫在 Boss 身后，透明度跟着符卡背景淡入 |
| 符卡背景交叉淡化 | 普通背景上整块 384x448 alpha 拷贝符卡背景（以前是先变黑再出现） |
| 低速魔法阵 | 低速时自机周围 64x64 的 A8 魔法阵（双环 + 6 个菱形 + 刻线，8 帧转动） |
| 无敌时半透明 | 中弹 / Bomb 后的无敌时间里自机半透明闪烁（以前是隔帧不画） |
| 残影 | 被消掉的子弹（Bomb、阶段结束、中弹时，最多 512 个均匀抽样）、被击落的妖精、中弹的自机，原图上飘并淡出 |
| HUD | Boss 血条、符卡名下面的半透明底条 |

`e` 关掉时回到原来的画法（闪烁、变黑过渡、没有魔法阵 / 立绘 / 残影），可以直接比较帧率。精灵池扩到 448 K 像素
（用 424 K）+ 128 KB A8 池。`th_host` 每 4 帧有 3 帧开 alpha（截图都开），alpha 帧按 CPU 5 位 / 引擎 8 位的差异给 2 LSB 容差，
不开 alpha 的帧仍要求逐像素相同。

### 8.8 第 2 关 "MOONLIT SEA OF CLOUDS"（月之公主，2026-10-12）

**流程**：第 1 关打完的结算画面过后（按键，或 8 s 自动）直接进第 2 关，残机 / Bomb / 火力 / 分数 / 擦弹继承；
第 2 关打完是 "ALL CLEAR"。标题菜单多了 `STAGE 2 PRACTICE`（直接从第 2 关开始，火力 2.50）；暂停里的 RESTART 从这一局的
起始关重来。自动演示轮流跑：Stress 第 1 关 → Normal 第 2 关 → Stress 第 2 关 → Normal 第 1 关。

**关卡结构**（`th_stages[]`，`th_game.c`）：每关一个时间表函数 + Boss 阶段表 + 道中 Boss 阶段表；道中 Boss 在场时关卡时钟停住，
打完飞走、道中继续（`ThBoss.mid / leaving`）。加第 3 关：写一个 `th_stage3.c`（时间表 + 阶段表），在 `th_stages[]` 加一行，
`TH_STAGES` 加 1，Boss 图在 `th_pixgen.py` 里加一个函数。

**内容**（`th_stage2.c`）：

| 段落 | 内容 |
| --- | --- |
| 道中 | 鬼火（半透明 A8 精灵，淡入、闪烁，发幻影弹）→ 激光妖精（瞄准激光：半透明虚线预警 → 发光光束）+ 横穿妖精 → 道中 Boss → 流星妖精（拖尾流星 + 星弹）+ 鬼火 → 两只转动双激光的大妖精 + 吹幻影环的鬼火 → 混合冲刺 |
| 道中 Boss | 非符：新月弧米弹 + 幻影环；符卡 MOON SIGN: SILVER PILLARS（随机月光柱 + 小弹雨） |
| Boss | 非符 1 新月弧 + 幻影环；PHANTOM SIGN: LUNAR MIRAGE（显隐沿环转动的幻影弹 + 苦无）；非符 2 四道跟随 Boss 转动的激光 + 螺旋；STAR SIGN: METEOR SHOWER（斜落流星 + 星弹环 + 大玉）；非符 3 两道左右扫动的月光柱 + 环；FINAL: FULL MOON NIGHT（巨大满月从云海升起，弹从月轮边缘出发并加速，六道慢转激光，幻影星弹） |

**新的弹 / 特效**（`th_game.c`、`th_render.c`）：

- **幻影弹** `th_phantom`（最多 384）：透明度按相位在 40–230 之间周期变化，只有显形（≥ 150）时才有判定 / 擦弹——
  alpha 在这里是玩法的一部分。每颗一条 alpha 命令；`e` 关掉时只画显形的那些。
- **激光** `th_laser`（最多 48）：BEAM 任意角度——沿光束每 6–14 像素放一个加法发光珠（按粗细选 16/32/64 像素的光晕，整批
  `gfx_blit_list`），中间一串白色核心；预警是半透明的虚线。PILLAR 是整列月光柱：半透明填充 + 加法核心，可以左右扫。
  COMET 是流星：头 + 8 节拖尾 + 火花。判定取宽度中间 60%，贴着激光每 8 tick 算一次擦弹。
- **伪 3D 云海背景**：天空一张图 + A8 月亮；地平线以下每一行屏幕是**一条** 384 像素的行拷贝，来自 20 档预先缩放的云纹理
  （0.25 × 1.2^l 纹素/像素，每档 64 行），按透视选档和行（深度 = 18944 / 离地平线的行数），纹理往前滚动。每帧约 300 条
  宽通路拷贝；地平线上叠 8 条半透明雾带，月光倒影是一串加法光晕，符卡时整块半透明红色调。
- 新精灵：月之公主（`th_pixgen.py` 的 `boss2`，两帧 + 切入立绘）、鬼火（24×32 A8，4 色 × 2 帧）、月亮（96 / 192 像素 A8，
  带环形山和光晕）。精灵池 512 K 像素（用 507 K），A8 池 256 KB（用 187 KB），云纹理各档 960 KB 另放。

**验证**：`th_host` 现在跑 Normal / Lunatic / Stress 两关连打（必须进第 2 关并 ALL CLEAR）、第 2 关练习、会死的脚本局；
3424 帧 CPU 与引擎模型对比通过（带 alpha 的帧按 2 LSB 容差），ASan / UBSan 通过，`host_main_test` 两种 STATS 都 PASS。
第 2 关截图拼图：`game/preview_th/_sheet_stage2.png`。板上还没跑过。

## 9. HDMI 音频（2026-10-19）

原来 `hdmi_fb` 输出的是纯 DVI（只有视频期和控制期），不能带声音。现在升级成真正的 HDMI：48 kHz、双声道、16 位 L-PCM，
声音由硬件混音器从 DDR 里的 PCM 采样混出来，CPU 只写寄存器"放第几号声音"。

### 9.1 硬件

**HDMI 打包（`rtl/hdmi/hdmi_island.v`，像素时钟 25 MHz 域）**

- 每个有效行前：视频前导 8 拍（CTL0=1）+ 视频保护带 2 拍。
- 行消隐里（每行最多 3 个包）：数据岛前导 8 拍（CTL0=CTL2=1）+ 保护带 2 + 32×n 拍 TERC4 + 保护带 2。
  640×480 的行消隐 154 拍，岛最晚在 h=752 结束，之后到视频前导之间还有 32 个控制字符。
- 包头 24 位 + BCH(32,24)，4 个子包各 56 位 + BCH(64,56)（生成多项式 1+x⁶+x⁷+x⁸，LSB 先发），通道 0 第 3 位只在岛的第一个字符为 0。
- 每行在 h = H_ACT−8 决定这一行发什么包（顺序）：音频采样包（上一行以来采到的 1..4 个采样）、ACR（每 25000 个像素时钟 = 1 ms 一次）、
  AVI InfoFrame（第 V_ACT 行，VIC 1，RGB，4:3）、Audio InfoFrame（第 V_ACT+1 行，2 声道）。
- 采样节拍：每个像素时钟 acc += N（6144），到 128·CTS（3 200 000）就采一个 → 25 MHz × 6144 / 3.2e6 = **正好 48000 Hz**；
  ACR 里 N = 6144、CTS = 25000，像素时钟 25.00 MHz 时是整数，电视据此恢复音频时钟。
- IEC 60958 声道状态：48 kHz、16 位、无版权，每 192 个采样一个 B 标志，奇偶位按规范。
- 采样来源：异步 FIFO（core_clk → 像素时钟，256 × 32 位 = 2 块 BRAM），写入端 = 混音器或 APB `AUD_DATA`；
  FIFO 空时发静音并计数（`AUD_UF`）。`AUD_CTRL[2]` = 硬件 1 kHz 正弦测试音（不需要混音器和声音库，用来先确认电视能出声）。
- `AUD_CTRL[0]` = 0 时和以前的 DVI 输出逐位相同（仿真里用原 encode.v 的算法逐字符比对过）。旧软件不写 0x20，上电默认就是 HDMI + 音频（只是没有声音）。
- `encode.v` / `dvi_encoder.v` 换成了 `hdmi_tmds_ch`（视频部分算法、3 拍延迟都和 encode.v 一样，多了控制 / TERC4 / 固定符号三种模式）。

**混音器（`rtl/audio/audio_mixer.v`，core_clk 200 MHz）**

- 16 个声部，逐个串行处理（每个有效声部约 30 拍，一个采样周期有 4166 拍）；HDMI 侧每取走一个采样，FIFO 少于 TARGET（默认 64 = 1.3 ms）就混下一个。
- 每声部：DDR 地址、长度、循环起点 / 开关、音高（4.12 定点，0x1000 = 源采样率 48 kHz；任意采样率都行）、格式（s8 单声道 / s16 单声道 / s16 立体声）、
  左右音量（0..256）、KEYON 起点。参数放在 BRAM（128 × 36 位，2 块），位置 24.12 定点也在里面。
- 两个相邻采样线性插值（循环点也接得上），乘音量累加，最后 `sat16(((acc >>> 8) × MVOL) >>> 8)`。
- 采样缓存：每声部 2 行 × 32 B（BRAM 2 块），处理完一个采样就预取后面一行，读 DDR 用 8 拍 burst；未命中时混音器停下来等（`MISSES` 计数）。
- DDR 读口：新模块 `aud_rd_mux` 把 HDMI 扫描 DMA（优先）和混音器接到 `blit_axi_rd_arb` 的 m0 上（同一个 ARID，按发出顺序回来；
  混音器同时只有 1 个 burst，记下它前面还有几个 HDMI burst 就知道哪些 R 拍是它的）。引擎、宽桥都没动。带宽约 1.5 MB/s（16 声部全开）。
- 寄存器（`0xF810_2000`）：

| 偏移 | 名称 | 说明 |
|---|---|---|
| 0x000 | ID | `0x4D495831`（"MIX1"） |
| 0x004 | CTRL | [0] 开（复位 0） |
| 0x008 | MVOL | [8:0] 总音量 0..256 |
| 0x00C | TARGET | [8:0] FIFO 填到多少采样（复位 64） |
| 0x010 | KEYON | 写：第 v 位 = 声部 v 从 START 开始放 |
| 0x014 | KEYOFF | 写：第 v 位 = 声部 v 停 |
| 0x018 | ACTIVE | 读：正在放的声部（不循环的放完自己清掉） |
| 0x01C / 0x020 / 0x024 / 0x028 | SAMPLES / MISSES / FETCHES / CLIPS | 混出的采样 / 要等 DDR 的次数 / 读的 32 B 行 / 饱和的输出采样 |
| 0x02C | CAPS | `0x00012010`：16 声部、32 B 行、版本 1 |
| 0x200 + v·0x20 | 声部 v（只写） | +0 ADDR，+4 LEN [23:0]，+8 LOOP [23:0] 起点 [31] 开，+C PITCH [15:0] 步长 [17:16] 格式，+10 VOL [8:0] 左 [24:16] 右，+14 START |

**顶层（`ddr3_example_top.v`）**：APB 按 `paddr[13]` 再拆一份给混音器；`hdmi_fb` 的 DMA 改走 `vd_*` → `aud_rd_mux` → 原来的 `hd_*`（m0）；
混音器输出接 `hdmi_fb` 新端口 `aud_wvalid/aud_wready/aud_wdata/aud_level`。工程 xml 加了 `hdmi_island.v`、`audio_mixer.v`。

**资源估计**：BRAM +6（音频 FIFO 2、声部参数 2、采样缓存 2；原来 246/256 → 约 252）；DSP 约 +8（插值 2、音量 2、总音量 4 个 12×9）；
逻辑约 2–3k XLR。HDMI 打包全在 25 MHz 域，不影响 core_clk。混音器按 200 MHz 写：每级只有一个加法 / 比较 / 窄乘法，标签比较、预取判断、
总音量乘法都拆成了两拍。如果 BRAM 超了：Efinity Debugger 自动例化关掉能省 1 块（第 7 节记录），或者把 `hdmi_fb` 的 `AUD_AW` 改成 5（32 个采样）。

**Efinity**：只需重新综合 + 布局布线（retiming 仍为 0，跑 `sim/blit/check_retime.py`），不用重新生成 SoC、不用动 Interface Designer / sdc
（像素时钟和 core_clk 本来就是异步组）。看三样：Memory Blocks、core_clk 时序余量、check_retime 输出 OK。

### 9.2 软件

- `src/audio.h / audio.c`：驱动。`aud_init()` 检测位流（旧位流返回 -1，其余函数全是空操作）、开混音器、在 0x0200_0000 找声音库；
  `aud_play(snd, vol, pan)` 音效（每种音效固定占一个声部，再响就从头重放，像原作），`aud_voice()` 任意声部 / 音高，
  `aud_bgm(snd, vol, fade)` BGM（声部 0、1 交替，换歌时旧的淡出），`aud_tone()` 测试音，`aud_master()` 总音量。
- `src/th_sound.c`：弹幕场景。`th_game.c` 只在事件发生处置 `t->sfx` 的位（`TS_*`，见 `th.h`；逻辑本身不调驱动，主机测试不受影响），
  `main.c` 每 tick `th_tick` 之后调 `th_sound_tick()`：位 → 音效（表里有每个事件的音量和最短重放间隔），状态 → BGM
  （标题 / 第 N 关道中 / 第 N 关 Boss / 第 2 关道中 Boss，暂停时 BGM 变小声，游戏结束停）。声音按**名字**在库里找，库里没有的事件就不响。
  事件名：`shot enemy_fire hit kill graze item powerup extend bomb_get bomb death spell capture phase boss_dead warning timeout select ok pause clear over laser`；
  BGM 名：`bgm_title bgm_stage1 bgm_boss1 bgm_stage2 bgm_mid2 bgm_boss2`。
- 声音库：`sound/sounds.txt` 写"名字 文件 选项"，`python tools/wav2bank.py` 生成 `sound/sound_bank.bin` + `sound/sound_bank.S`，
  **下载声音库（2026-10-20 起）**：makefile `SOUND ?= 0`（默认）不把库放进 elf——JTAG 只有 800 kHz，2 MB 的库让每次下载程序多等好几分钟，
  看起来像下载卡死。改成上电后 IDE 照常 Debug 运行程序，再运行 `python tools/load_sound.py`（经 OpenOCD TCL 端口 6666：暂停 CPU →
  `load_image` 到 0x02000000 → 继续运行，打出用时和 KB/s；`--speed 4000` 临时提高 JTAG 速度试试，`--verify` 读回比较）。
  游戏每秒查一次 0x02000000，一秒内自动认出声音库（串口 `[audio: sound bank found ...]`）。不断电的话之后重新下载程序都不用再下库。
  `SOUND=1` 时库 `.incbin` 进 elf 的 `.sound` 段（`game.ld` 放在 0x0200_0000），下载 elf 时一起写进 DDR（交付 / 演示版用）。
  选项：`mono / stereo`、`rate=N`（降采样省空间，不写就保持原采样率）、`loop=auto|none|N|Xs`、`gain=`、`trim`、`end=`、`8bit`。
  支持 PCM 8/16/24/32 位和浮点 WAV；有 scipy / numpy 时用它们重采样。体积：立体声 44.1 kHz 16 位每分钟约 10 MB，全部下载时间按 JTAG 速度算
  （脚本会打出估计）；BGM 用 `rate=32000`、音效 `rate=22050` 可以小很多，上限 16 MB。
- `python tools/wav2bank.py --demo`：在 `sound/demo/` 生成一套合成的占位音效 + 5 首短 BGM 并打成库（已经生成好了，换成自己的 WAV 前就能先听效果）。
  `sound/preview_demo.wav` = 主机模型按这套占位音效混出来的 45 秒（第 1 关道中 → WARNING → Boss），就是板子上应该听到的声音。

**串口按键**：`,` 硬件 1 kHz 测试音开 / 关（第一步用它确认电视能出声），`.` 总音量 256 / 128 / 64 / 0 循环并打一行音频状态，
`/` 依次试听库里每个声音。开机打印一行 `audio: mixer | voices .. | sent .. underflow .. | ... | bank N sounds .. KB`；
`STATS=1` 版每秒多一行同样的状态。`underflow` 应该不涨，`miss` 偶尔涨一点正常（新声音第一次读 DDR）。

### 9.3 验证（都不需要板子）

- `sim/hdmi/tb_hdmi.py [dvi|hdmi|tone|uf|full]`：独立的 Python TMDS 解码器逐字符检查：视频字符和 encode.v 算法逐位相同（含 DMA 数据），
  每个消隐字符分类（控制 / 前导 / 保护带 / 岛），TERC4 合法性、通道 0 第 3 位、岛内 HSYNC/VSYNC、包头和 4 个子包的 BCH、
  ACR N/CTS、InfoFrame 校验和与字段、音频包（sample_present、奇偶、声道状态、每 192 个 B 标志）、收到的采样序列 = 推进 FIFO 的序列、
  采样率 48 kHz、ACR 间隔 25000 拍、FIFO 空 → 静音 + `AUD_UF`、测试音 = 正弦表、`AUD_DATA` 推的采样、关音频时没有音频包。
  `full` 是真实 640×480 时序 2 帧。全部 PASS。
- `sim/audio/tb_mixer.py [seed] [n]`：混音器 + `aud_rd_mux` 对 Python 参考模型逐采样逐位比对（随机格式 / 音高到 16 倍 / 循环 / 一次性 /
  KEYON 重启 / KEYOFF / 改音量音高 / 总音量 / 饱和），AXI 读延迟随机（`TB_LAT=min,max,长延迟概率`），同时有 HDMI 风格的 64 拍 burst 抢读口，
  每个 R 拍都检查回到了正确的主机；混音中途写声部寄存器检查 PREADY 等待。多个种子、长延迟、极短延迟都 PASS。
  16 声部稳态：596 拍 / 采样（预算 4166），无未命中；DDR 延迟 40–400 拍时 810 拍 / 采样。
- `sim/audio/tb_av.py`：按顶层接法把 `hdmi_fb_top + audio_mixer + aud_rd_mux` 连起来，从 TMDS 输出里解出音频 = 混音器模型的输出，视频同时逐位正确，FIFO 无下溢。
- `sim/blit/vsim.py` 改动（接受 `always @(posedge clk or posedge rst)`、顶层参数可以传整数）对引擎的仿真没有影响：同一份 tb_blit_top 生成的仿真代码逐字节相同。
- `tools/host_audio_test.c`：真正的游戏逻辑 + 驱动 + `th_sound.c`，寄存器接到 C 写的混音器模型（与 RTL 同一算法），150 秒：
  标题 → 第 1 关 → Boss 的 BGM 切换、各事件出声、音效从不占 BGM 声部、采样地址都在库里，输出 WAV。
  `host_main_test`、`th_host` 加了新文件后照样 PASS。

### 9.4 板上第一次测试建议

1. Efinity 重新综合 → 看 BRAM / 时序 / check_retime → 烧录。
2. `make clean` 后重新编译、下载运行（elf 和以前一样小），串口音频行是 `audio: mixer ... bank 0 sounds`。
   然后 `python tools/load_sound.py`（库已经是 `--demo` 的占位音，2 MB），完成后串口出现 `[audio: sound bank found at 0x02000000: 28 sounds ...]`。
3. 按 `,`：电视应该发出 1 kHz 的"嘀——"。没有声音时先确认电视 / 显示器有喇叭、HDMI 输入的音频没被静音，
   并看 `sent` 在涨（≈48000/s）。DVI 显示器或 HDMI 转 VGA 不会有声音。
4. 再按 `,` 关掉，标题画面应该有 BGM，进游戏有射击 / 擦弹等音效；`/` 逐个试听。
5. 换成自己的 WAV：放进 `sound/`，改 `sound/sounds.txt`，`python tools/wav2bank.py`，再 `build_game.bat`。
