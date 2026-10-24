# R5.6：压力模式直接覆盖角色 / Direct stress rendering

R5.6 已移除萝卜所有压力档的定位轮廓、中心标记、覆盖率计算、渐隐和近身弹幕索引生成。角色以正常不透明度绘制在弹幕之前，敌弹按原顺序覆盖角色。普通游戏保留轮廓功能；东方、方块、音乐、输入、99,999 容量、尺寸和双视频模式沿用 R5.5。

## 修改内容

- `game.c`：压力模式不再执行逐弹的定位范围判断及索引写入；弹幕移动、寿命、碰撞和绘制表保持原逻辑。
- `render.c`：压力模式不调用 `prepare_player_locator()` / `draw_player_locator()`，不画灰边和中心红白标记；角色始终按普通不透明方式先画，弹幕后画。清理从普通游戏遗留的渐隐状态。
- `hero_locator.h`：即使直接调用准备函数，压力模式也会立即退出。普通游戏的像素并集、阈值和轮廓缓存保持。
- `game.h`：近身索引只供普通游戏最多 1,024 颗敌弹使用，数组由 99,999 项缩为 1,024 项。节省 **395,900 bytes = 386.62 KiB 外部 DDR**；这不是 FPGA 片上 RAM 的减少。
- 遥测新增 `locator_on`；萝卜压力模式应回报 `locator_on=0 locator_us=0`。`locator_us=0` 表示该计算路径没有执行，不代表整个提交阶段耗时为零。

## 能节省多少时间、增加多少弹幕

目前可读取的实板日志仍是 R5.4 控制台 / R5.3 ELF 的 `uart_163553_825766_R5-CONSOLE25.csv`。没有 R5.5（优化后轮廓）或 R5.6（移除轮廓）的实板日志，不能从中直接给出本次净节省的毫秒数，也不能承诺新的稳定 60 FPS 上限。

旧日志中 15,000 颗 13px 弹幕的 `enemies` 绘制合计约 4.44ms，其中还包括角色和敌人绘制，不能全部计作轮廓成本。R5.5 又已优化了轮廓索引和逐位计数，故不能把旧合计直接套到 R5.6。

可以根据旧日志估算**节省预算与弹数的换算量级**：

- 15K → 16K 时，敌弹逻辑增量约 649µs / 999 颗，即约 **0.65µs/颗**。
- 15K 的敌弹逻辑平均约 11,525µs / 14,978 颗，即约 **0.77µs/颗**。
- 因而每实际净省 1ms，大约相当于 **1,300–1,540 颗**这类 13px 弹幕的 CPU 逻辑预算。

| 假设实测每帧净节省 | 对应的 CPU 弹幕预算 | 是否已实测 |
|---|---:|---|
| 0.5ms | 约 650–770 颗 | 否 |
| 1ms | 约 1,300–1,540 颗 | 否 |
| 2ms | 约 2,600–3,080 颗 | 否 |
| 3ms | 约 3,900–4,610 颗 | 否 |

该表是假设情景，不是对本次节省时间的预测。它只在 CPU 主导且仍有 GPU / DDR 余量时有参考价值；额外弹幕仍需要绘制和带宽。移除渐隐后，之前完全隐藏的角色也会恢复绘制，因此净收益还包含这部分变化，不能仅用旧 `locator_us` 相减。

旧日志 15K 那组平均循环工作为 **19.54ms**。达到约 60 次循环/秒，需要先消除约 **2.88ms** 的超预算，再谈增加弹数。此值属于旧版条件，不代表 R5.5 或 R5.6 的耗时。由于流水重叠，该循环工作值也不等于单帧 GPU 完整延迟。

对同一位置、尺寸、Alpha、数量、同步方式和音频条件，可以用下式做局部 CPU 预算估算：

`可增加弹数 ≈ (16,667µs − 旧循环工作µs + 实测净节省µs) / 每颗增量成本µs`

若括号内为负，表示尚未达到 60 FPS 预算。判断“稳定”还要看窗口最大耗时、超预算帧数、实际新画面 FPS、欠流和长期波动，而非只取平均值。

## 交付与验证

新文件：`BOARD_TEST/r5_6/ti60_r5_6_debug.elf` 与 `ti60_r5_6_release.elf`。沿用 `BOARD_TEST/r5_5/ti60_r5_5.bit`，未修改 RTL、PLL、SDC 或 BSP 时钟。已加载该 bit 时只需换 ELF；旧 R5.5 文件保留以便相同条件 A/B。

已验证：七种弹体尺寸的最后可见弹幕像素直接覆盖角色；压力路径不生成近身索引且无轮廓计算；99,999 容量；多绘制路径像素一致；正常游戏 13 张快照与 R5.5 逐字节相同；普通射击/技能/升级和双视频主循环检查。仿真结果不用于代替板上 FPS。

建议先分别用 R5.5 debug 和 R5.6 debug，点击同一个 R2 15K / 13px 自由运行按钮，等待弹数稳定，记录至少 30 个完整窗口。比较 `logic_us`、`hostile_logic_us`、`submit_us`、`locator_us`、`enemy_us`、`work_us`、`work_max_us`、`present_fps`。随后通过 100/10/1 步进重新搜索稳定边界。上位机页脚为 `R5.6-STRESS-NO-LOCATOR`；老 ELF 未回报 `locator_on` 时显示状态未报告。

## English notes

R5.6 removes all locator-related per-frame work from carrot stress modes: proximity indexing, coverage union/counting, fade transitions, outlines and centre markers. The actor is rendered opaque before projectiles. Normal gameplay keeps its original locator and has 13 byte-identical reference frames. Shrinking the now normal-only index saves 395,900 bytes of external DDR; FPGA block RAM and timing are unchanged.

There is no physical R5.5/R5.6 A/B log yet. The old 4.44ms actor-render metric also contained enemy and player rendering, so it is not a measured removable locator cost. The supplied older log suggests 0.65–0.77µs of hostile-logic CPU work per 13px bullet. Therefore a hypothetical measured 1ms net saving corresponds to roughly 1,300–1,540 bullets of CPU budget, subject to GPU/DDR limits. This is a conditional conversion, not a promised FPS or capacity gain. Old 15K windows averaged 19.54ms loop work and first needed about 2.88ms recovery to fit a 16.67ms budget.

Use the new ELF with the unchanged R5.5 bitstream. Detailed hashes, memory usage and verification are supplied separately. Physical stable-load limits remain pending.
