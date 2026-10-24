# Ti60F225 Sapphire SoC → HDMI：CPU 纯软件渲染 SproutRun（赛题二 第一阶段）

目标：RISC-V（Sapphire RV32IM @100 MHz）用 CPU 把整帧画进 DDR 帧缓冲，
硬件 `hdmi_fb` 以 640×480@60 RGB565 扫描输出到 HDMI，测量"只用 RISC-V"能跑多少帧。
这就是后续 BitBlt 硬件加速器的对比基线。

![SproutRun host simulation](host_sim_screens.png)

> 当前版本已通过 Efinity 2026.1 综合、布局布线、时序分析和 bitstream 生成；
> 板上 HDMI、DDR 和游戏运行结果仍以实际硬件测试为准。

## 1. 硬件改动（rtl/ 与 par/）

| 文件 | 说明 |
|---|---|
| `rtl/hdmi/hdmi_fb.v` | 新增：640×480@60 时序、AXI 读 DMA（burst）、异步 FIFO、APB 寄存器、vsync 翻页 |
| `rtl/hdmi/dvi_encoder.v`, `encode.v` | 从 DemoBoard 的 HDMI demo 移植的 TMDS 编码器 |
| `rtl/ddr3_example_top.v` | 例化 `u_hdmi_fb`：APB = `io_apbSlave_0`，DMA = `io_ddrMasters_0`（只读，AW/W 置零），`userInterruptA = hdmi_irq`，TMDS 输出按板子 P/N 交换取反 |
| `par/ddr_demo_ti60/*.xml / peri.xml` | 加入 HDMI 源文件、PLL_BR0：VCO 4000 MHz / O=8 → 像素时钟 25.00 MHz（÷20）/ 5× 串行时钟 125 MHz（÷4，phase_setting=2 = 90°，与官方 HDMI demo 一致）、LVDS TX 引脚；时序 794×525（H_BP=42）= 59.97 Hz |
| `par/ddr_demo_ti60/sdc/ddr3.sdc` | HDMI 时钟（第 16-21 行）、LVDS 输出约束（约第 318-338 行）；第 65 行原模板 `<USER_PERIOD>` 已改为 `40.000`（clk_25m） |

### Efinity 流程
1. 用 Efinity 打开 `par/ddr_demo_ti60/ddr_demo_ti60.xml` → Interface Designer，检查 HDMI LVDS TX 和 PLL 输出，点 *Generate* 重新生成约束。
2. 对比 `outflow/ddr_demo_ti60.pt.sdc` 中 LVDS 参考引脚名（如 `hdmi_tx_slow_clk~CLKOUT~218~175`），
   与 `ddr3.sdc` 第 318-338 行保持一致，不一致就以 pt.sdc 为准修改。
3. Synthesize → Place & Route → 生成 bitstream，用 Programmer 下载。

## 2. 内存 / 寄存器映射

DDR（CPU 与 DMA 地址一致，无偏移）：

| 地址 | 用途 |
|---|---|
| `0x0000_1000` – `0x00F0_0FFF` | 程序/数据/bss/栈（`game.ld`，15 MB） |
| `0x0100_0000` | 帧缓冲 0（614400 B，RGB565） |
| `0x0120_0000` | 帧缓冲 1 |
| `0x0140_0000` | 帧缓冲 2（三缓冲） |

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

## 3. 软件（`embedded_sw/soc/software/standalone/game`）

- `src/game.c / render.c / gfx.c`：用户 SproutRun 游戏原样移植，gfx CPU 后端逐像素写 DDR。
- `src/hdmi_fb.h`：寄存器驱动。
- `src/main.c`：三缓冲主循环、UART 非阻塞输出、自动驾驶 bot、FPS 统计。
- `game.ld`：放大 RAM 区（背景图 600 KB 在 .bss）。

编译（Efinity RISC-V SDK 终端或 Efinity RISC-V IDE）：

```
cd par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game
make                       # 默认 -O3 (DEBUG=no BENCH=yes)
make VSYNC=0               # 上电即不等 vsync，直接测最大帧率
make AUTOPLAY=0            # 上电等键盘操作
make GFX_CPU_WORD_FILL=0   # fill_rect 用原始 16 位逐像素循环做对比
```

下载运行：IDE 中使用 `game_ti.launch`（指向 `build/game.elf`），通过 JTAG 加载到 DDR 0x1000 运行。
串口 115200 8N1。

仓库中的 `.launch` 文件由当前 Windows 工作区生成，包含本机绝对路径。克隆到其他目录后，
请在 Efinity RISC-V IDE 中重新导入 `game` 工程，或在 Debug Configurations 中更新 BSP、OpenOCD 和 SVD 路径。

## 4. FPS 测量方法

- **vsync 模式（默认）**：每个 vblank 最多翻一次页，上限 60 FPS，画面无撕裂。
- **free 模式（按 `v`）**：从不等待，最新帧胜出 → 得到 RISC-V 纯软件的真实渲染帧率。
- 游戏逻辑始终以 1/60 s 步进（每渲染一帧最多一步，落后则丢帧不追赶），与 Windows 版一致。

启动时先打印全屏 fill / 全屏 copy 的耗时和带宽（上限 FPS），然后每秒一行：

```
fps 23.4 | draw 41200 us (max 45100) game 1500 us work 42700 us -> cpu-limit 23.4 fps | busy 99% | vs 60/s uf 0 | fill .. blit .. px .. | free auto ph 2 foes 57
```

| 字段 | 含义 |
|---|---|
| fps | 实际每秒提交的帧数 |
| draw / max | 每帧 CPU 渲染耗时（µs） |
| game | 每次游戏逻辑耗时（µs） |
| work → cpu-limit | 渲染+逻辑 总耗时及其对应的理论上限帧率 = **只用 RISC-V 能跑到的帧率** |
| busy | CPU 占用率 |
| vs | HDMI 实际输出帧率（应为 60） |
| uf | DMA 下溢计数（非 0 说明 DDR 带宽不够/画面会花） |

屏幕 HUD（`f` 切换）同样显示 FPS / ms / CPU%。

## 5. 串口按键

| 键 | 功能 |
|---|---|
| w a s d / 方向键 | 移动（锁存，会关闭自动驾驶） |
| x | 停止 |
| Enter / Space | 开始 |
| p / r / m / l | 暂停 / 重开 / 主菜单 / 压力测试菜单 |
| 1 2 3 | 选择升级 / 档位 |
| b | 自动驾驶开关 |
| v | vsync 开关 |
| f | 统计面板开关 |
| t | HDMI 彩条测试（排查链路用） |
| h / ? | 帮助 |

## 6. 排查

- 串口打印 `WARNING: hdmi_fb ID = ...`：bitstream 不是新版本。
- 无画面：按 `t` 看彩条；彩条也没有 → 查 PLL/LVDS 引脚/约束；有彩条无游戏 → 查 DMA / 帧缓冲地址。
- `uf` 持续增长：降低 CPU 访存压力或增大 FIFO / burst 长度。
- 帧率极低且启动打印 `-O0/-Og (slow!)`：以 `DEBUG=no` 重新编译。
