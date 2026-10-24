# R5 的 RAM、关键路径与 Alpha 优化说明

本文的 RTL 证据来自 `PROJECT/rtl/blit/blit_engine.v`，基准资源/时序来自原 R2 的 Efinity 报告。R4 没有更改 RTL。R5 的最终资源和时序必须以新 Efinity 编译报告为准；Vivado 2021.2 在这里仅承担通用 RTL 行为验证，不能为 Ti60 提供布局布线签核。

## 最终目标器件实现结果

最终 Efinity 2026.1.132.4.5 报告确认 Ti60F225 已完成实现并通过报告中列出的七组时钟关系的 setup / hold 检查，**尚未进行 R5 实板测试**。资源为 **58,518 / 60,800 XLR（96.25%）、253 / 256 RAM（98.83%）、32 / 160 DSP（20%）**；剩余 2,282 XLR、3 RAM、128 DSP。核心仍运行在 200 MHz，setup **+0.118 ns**、hold **+0.028 ns**。报告给出的核心 Fmax 204.834 MHz 仅是该次实现的分析结果，不是新运行频率。

| 报告时钟关系（同域） | 约束周期 ns | Setup slack ns | Hold slack ns |
| --- | ---: | ---: | ---: |
| `core_clk` | 5.000 | +0.118 | +0.028 |
| `pll_fb_clk` | 10.000 | +3.373 | +0.030 |
| `sdram_clk` | 2.500 | +0.321 | +0.039 |
| `rx_cal_clk` | 2.500 | +0.321 | +0.034 |
| `tx_cal_clk` | 2.500 | +0.308 | +0.097 |
| `tx_cal_clk_90edge` | 2.500 | +0.313 | +0.069 |
| `hdmi_tx_slow_clk` | 13.445 | +6.890 | +0.048 |

最终核心最差路径已位于 Sapphire CPU 内部，逻辑层数 **8**，报告 Delay **4.572 ns**：

- 起点：`u_sapphire_soc/u_EfxSapphireSoc/system_cores_0_logic_cpu/dataCache_1/stageB_waysHitsBeforeInvalidate[3]~FF|CLK`
- 终点：`u_sapphire_soc/u_EfxSapphireSoc/system_cores_0_logic_cpu/CsrPlugin_mscratch[2]~FF|CE`

该表完整列出时序报告的 Clock Relationship Summary；`hdmi_tx_fast_clk` 和 `clk_25m` 出现在目标时钟列表，但该报告未给出它们的独立关系行，因此没有为其虚构 slack。以上结果不替代 CDC 或板级测量。

证据为 `PROJECT/par/ddr_demo_ti60/outflow/ddr_demo_ti60.timing.rpt`（报告日期 `Sat Oct 24 01:46:06 2026`）及同目录 `ddr_demo_ti60.place.rpt`。最终 bit 为 **2,811,948 字节**，SHA256：`25dc85470b3f73f929ef75c60eb06880a7c46533f30c26820a40eb5def85ac83`。结构网表算术检查与这些目标实现结果、源码 hash 一并记录在 `verification/r5/blit_hdmi_validation.json`。

## 已实施的改动

| 项目 | R2 / R4 基准 | R5 改动 | 取舍 |
| --- | --- | --- | --- |
| 读描述符队列 `u_bd` | 64 × 23 bit，2 块 RAM10；另有输出寄存器 | 16 × 23 bit 寄存器队列；输出寄存器保持 | 已有 `bd_full` 回压保护，最多容纳 17 个描述符；长延迟时可能提前暂停发起新读 |
| 命令预取 `u_lf` | 64 × 32 bit，2 块 RAM10 | 32 × 32 bit 寄存器队列；信用上限 32，空位达到 16 后预取 | 用 FF / LUT 换取 2 块 RAM；较短突发可能降低 DDR 延迟隐藏能力 |
| R FIFO 预约计数 `rf_res` | 发起判断之后串联多项加减，位于最差路径终点 | 复用已有 `rdi_m` 寄存增量；`rf_res_next = rf_res + rdi_m - rf_pop`；信用使用 `rf_eff = rf_res + rdi_m` | 没有额外延迟授信，没有更改可接受的 R FIFO 容量；实际时序收益需 Efinity 确认 |
| 窗口与 Alpha | 1280 × 16 行窗口，通用 Alpha 每周期 2 像素 | 保持接口和精确算术 | 避免改变游戏画面、band 分桶、窗口冲突和命令顺序 |

两处浅队列已在 R5 Efinity 映射中释放 **4 块 RAM10**：`u_bd` 和 `u_lf` 各从 2 块变为 0，`u_blit` 总 RAM 从 85 变为 81。这是 blitter 子模块结果；整机最终资源见上表。新增存储位约 1,392 个，另有 FIFO 指针与读选择逻辑；总 XLR 净变化不能简单按存储位相加，应以 Efinity 为准。

### RAM 预算和交付取舍

最终实现保留 **三个显式 512×16 的扫描行缓存 bank**，并将 CH9350 UART 的 **64 字节接收 FIFO 保持为一块 RAM10**。释放的 4 块 RAM 分别供新扫描缓存 3 块和 UART 1 块使用，最终报告确认整机为 `253 - 4 + 3 + 1 = 253 / 256`，不是 252；最终 Efinity 映射及布局布线已确认 253 块。UART 接收容量没有缩减，仍保留溢出、帧错误和显式释放保护。

将 UART FIFO 强制映射为寄存器的试验版本虽只需 252 RAM，但达到 59,738 / 60,800 XLR（约 98.25%），核心 setup slack 为 **−0.159 ns**。该版本不作为交付版本。其最差路径已转为性能计数器到 SoC 的 APB 读回路径，其次是宽写数据的使能路径，表现为拥塞和跨区域布线压力。保留一块 UART RAM 可回收 FF 和读选择逻辑，优先满足原 200 MHz 时序，同时保持接收容量；不能为了表面上的最小 RAM 数量牺牲时序。工具生成 bit 或显示流程成功，不等于 setup/hold 已通过。

原基准为 XLR 55,848 / 60,800、RAM 253 / 256、DSP 32 / 160；200 MHz 核心时钟 setup slack +0.053 ns、hold slack +0.025 ns。原最差路径为 `u_blit/f_addr[1] → u_blit/rf_res[9]`，34 层逻辑。R5 将“cache 命中 → AR 优先级/突发长度 → 多个加法器 → 预约计数”中的多项增量提前收进已有寄存器，不能据此直接承诺更高频率。新映射的 `rf_res` 输入组合锥从基准 304 个单元 / 237 个外部输入缩至 41 个单元 / 20 个输入；该结构计数不是布局布线后的路径延迟。

### 信用恒等式

设旧版预约值为 `O(t)`，新寄存器为 `R(t)`，上一个周期已发起、尚未加到新寄存器的增量为 `I(t)`。复位时三者为 0，且维护 `O(t) = R(t) + I(t)`。每周期新实现执行 `R(t+1) = R(t) + I(t) - pop(t)`，同时保存 `I(t+1) = issue_words(t)`，因此下一周期仍有 `O(t+1) = R(t+1) + I(t+1)`。所有发起者检查 `rf_eff`，不会因 retime 少算一笔已发出的突发。三个窄端发起类别互斥，使已有 `rdi_m` 的按位 OR 等价于求和。

`DBG0` 的预约字段现在输出 `rf_eff`，保留原先“已发起而未路由的总字数”语义。

## Alpha 现状、实测边界与下一步

通用 RGB565 路径有全局 Alpha、每像素 A8、颜色键和加法混合。全局权重先按 `g9 = g + (g >> 7)` 将 255 精确映射为 256；有 A8 时先求 `e = (A8 × g9) >> 8`，再扩展权重。普通通道结果为 `(src × a + dst × (256-a) + 128) >> 8`；加法混合保留完整目的权重并饱和。R5 没有换成不同的 `/255` 公式，避免画面差异。

当前普通 Alpha 管线一拍处理两个 RGB565 像素。200 MHz 下纯算术上限为 400 Mpx/s，实际帧吞吐还受读环、cache、行/命令切换、重叠冲突、分桶和 DDR 影响。K 通路的 8 像素/拍只覆盖不透明、颜色键和全权重加法，不能称为 8 像素 Alpha。

下一阶段建议按以下顺序评估：

1. **先减少工作量。** 文字和静态界面缓存已经在 R4 完成；继续将不变背景、装饰和不动 HUD 合成离线缓存。统计真实覆盖像素及 Alpha 像素，避免仅按对象个数比较负载。
2. **测量拥塞位置。** 同时比较 `P4` 的分桶/绘制/K/flush 时间、cache hit/miss、读等待、Alpha 开关、尺寸与对象数。CPU work、渲染完成、物理显示帧率分别记录。
3. **增加 4 像素 Alpha 分支原型。** 可使用剩余 DSP 扩展算术宽度，但必须同时提供源像素/A8/目的窗口的四像素访问，延长或旁路窗口冲突检查，保持命令顺序和颜色键屏蔽，不能只复制乘法器。以随机重叠逐像素比对为准入条件，再做 Efinity 面积和时序比较。
4. **最后考虑 8 行 band / 二维 tile。** 720p 的 8 行窗口有望从 40 块变为 24 块 RAM，但 band 数从 45 增至 90，并改变分桶计数、地址、容量、软件分配和 DDR 流量；不能只改一个常数。原生 1080p 同时影响帧缓存槽和 band 配置，需要单独版本。

直接缩小 `RING_AW` 不安全：窄路径必须先等整行数据到齐，512 word 的环不能容纳 1280 像素的 640 word 行，会造成等待不满足。单纯将 512×32 的 cache bank 深度减半也不保证节省块数，因为当前映射的宽度限制仍需两块 RAM。R5 的 1080p30 输出采用固定 720p 渲染放大，避免扩大 BitBlt 窗口和游戏资产。

## 验证证据

| 验证 | 证据文件 | 说明 |
| --- | --- | --- |
| Vivado / XSim 2021.2 浅队列测试 | `verification/r5/blit_control_xsim_simulate.log` | 每个队列 12,000 周期；定向满/空、随机暂停、同时读写、顺序/数量检查；明确 `TEST_PASS` |
| 实际 P2…P6 Alpha 算术 | `verification/r5/blit_alpha_xsim_simulate.log` | 20,000 组双像素；所有全局权重、A8 端点、普通/加法混合、饱和；一组/拍；明确 `TEST_PASS` |
| 完整 AXI / golden memory 回归 | `verification/r5/blit_random41.log` | seed 41，1,056,527 周期；direct/cache/list/TAB/wide/band、长行、重叠、bin 错误标志；`PASS` |
| 严格 W 背压与长 DDR 延迟 | `verification/r5/blit_random73_backpressure.log`、`blit_r4_backpressure.log` | 非常规 LIMIT=2/2/128 在 R4 和 R5 均出现宽 band 死锁，属于继承限制，不能标为通过；默认 LIMIT 的完整回归通过 |
| 实际信用守恒 | `verification/r5/blit_credit_test.log`、`blit_credit_result.json` | 每周期核对 `rf_eff` 与真实 issue/pop 之和，并检查描述符 ≤17、list 预约 ≤32 |
| 默认 LIMIT 下严格 W 背压与长 DDR 延迟 | `verification/r5/blit_strict_default_limit.log` | 1,279,988 周期、12 个 band 帧，包含 2560 字节行及 45 bands；PASS |
| 双视频模式与扫描缓存 | `verification/r5/hdmi_verilator.log`、`hdmi_dualvideo_tb.sv` | 200 MHz / 74.375 MHz 异步时钟，活动期请求切换，720→1080→720 三完整画面，3,916,800 个像素及最终 TMDS 解码逐个比对，固定 720p DMA 边界，AVI VIC/校验和，零视频欠载；PASS |

Alpha 测试用层次 force 仅在 P2 输入边界隔离算术验证，不代表整机 AXI 或 CDC 证明；完整像素顺序由独立 AXI 回归补充。XSim 的编译库路径警告不影响本次纯 RTL 仿真；继承的 wide skid count 输出位宽警告为 4 bit 到 3 bit，容量 5 可由 3 bit 表达，本次未改其功能。

新 Efinity 映射之后必须运行原 `PROJECT/sim/blit/check_retime.py`、原 `check_netlist_cone.py` 及新增 `verification/r5/blit_check_rf_res_netlist.py NEW_R5.map.v`。该工程历史上遇到过 Efinity retiming 复位和移位单比特加法映射错误，所以 RTL 仿真通过仍不足以跳过网表检查。最终板上请补：两种视频模式的切换次数/锁定时间、视频和音频欠载增量、最大稳定对象数、各尺寸档位、Alpha 开/关、至少十分钟持续运行、温度和时钟配置。
