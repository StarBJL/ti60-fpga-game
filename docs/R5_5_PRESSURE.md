# R5.5 压力测试修订 / Pressure-test revision

这次修订基于 `uart_163553_825766_R5-CONSOLE25.csv`，并结合工程中同一会话的原始串口日志。表中的帧率属于用户上板运行的 R5.4 控制台 / R5.3 ELF，不是 R5.5 的测试成绩。R5.5 尚需上板填写实测结果。

## 日志结论

| 条件 | 测量窗口 seq | 平均渲染 FPS | 平均新画面 FPS | 逻辑 µs | 提交 µs |
|---|---|---:|---:|---:|---:|
| 萝卜 15,000 / 13px / Alpha / 自由运行 | 59–76 | 51.33 | 50.83 | 12,829 | 6,795 |
| 萝卜 16,000 / 13px / Alpha / 自由运行 | 78–95 | 49.34 | 49.13 | 13,496 | 6,832 |
| 方块 8,800 / 随机 6–40px / Alpha / 自由运行 | 205–236 | 60.04 | 59.98 | 1,030 | 892 |
| 方块 8,900 / 随机 6–40px / Alpha / 自由运行 | 191–202 | 59.51 | 59.42 | 1,042 | 897 |
| 方块 30,000 / 6px / Alpha / 自由运行 | 400–409 | 64.43 | 60.07 | 3,005 | 755 |

这些窗口的硬件列表、分带、源缓存、K 路径和适用的重叠模式均开启；视频和音频欠流增量均为 0。方块原随机尺寸的性能已接近 R2 日志中的 8,800–8,900 个边界。30,000 个 6px 方块仍能呈现约 60 新画面/秒，说明旧 30,000 容量限制已妨碍继续测量小尺寸上限。

`auto=0` 只关闭角色自动移动。旧 `game_tick()` 在压力模式仍调用 `shoot()`。15,000 弹窗口的玩家子弹逻辑平均约 426 µs，玩家子弹绘制约 63 µs，粒子绘制约 275 µs。射击还会影响敌人受击、死亡与补充，因此关闭射击有实际意义；但这些直接耗时不足以单独解释全部回退。

旧 `enemies` 绘制计时包括 `prepare_player_locator()`、敌人和角色绘制。15,000 弹窗口这一合并项平均约 4,438 µs，无法从旧日志拆出轮廓计算本身。代码确认两个额外成本：256 项近身索引满后退回全表扫描；每次遮挡命中都调用逐位计数。新版本修复这两处，并新增独立计时，最终收益以上板结果为准。

## 实现与边界

- 萝卜所有压力档停止自动枪火与玩家技能弹；普通游戏的双枪、升级与技能继续使用原逻辑。压力模式保留敌人出弹、角色移动、碰撞及定位显示。
- 萝卜自定义压力、东方压力、方块容量统一为 **99,999**。这是对象容量，不是稳定 60 FPS 的承诺。东方普通游戏保留原 16,384 容量；既有压力低/中/高档仍为 1,024 / 2,048 / 4,096。
- 萝卜弹体：**1 / 3 / 6 / 9 / 13 / 17 / 25px**。东方压力增加 **1 / 3 / 6px**，保留 **75 / 100 / 125 / 150%**。方块：随机 6–40px，以及 **1 / 3 / 6 / 8 / 12 / 16 / 24 / 32 / 40px**。
- 偶数 6px 的真实边长与中心偏移分开处理，避免生成 7px 的裁剪框。1px 空心弹取可见源像素，避免缩成透明点。弹体尺寸只改变显示，原碰撞半径保持。
- 近身索引覆盖整个弹池；遮挡使用同一像素并集，最后逐行计数，保持原轮廓、阈值和渐变语义。
- TAB 单条最多 65,535 项，沿用驱动的拆分机制。MOT2 / SQTAB 使用 **29,984 + 29,984 + 29,984 + 10,047** 分批处理 99,999 个方块。29,984 是 32 的倍数，保证运动 DMA 起址 256 字节对齐，且保持奇偶 Alpha 顺序。不是把 99,999 截成 16 位。
- MOT2 新版本寄存器 `0x04=0x00020001`；新增 `0x44=0x00280001`，表示合法尺寸 1–40px。旧 bit 的该寄存器为 0，新 ELF 将其按最小 6px 处理，小尺寸自动使用 CPU 运动，不触发错误 DMA。使用配套新 bit 才能让 1px / 3px 方块走硬件运动。
- 单带命令缓冲从 8,192 扩至 32,768 项，使用外部 DDR。驱动按“所有对象都可能集中于同一带”的上界计数；容量不够时先结束当前分带，再按原顺序直接绘制剩余内容。不会默默丢弃对象，也不会重开会清空已有像素的分带帧。大于约 32K 的密集场景可能因此降低性能；这正是后续可优化的限制。
- 新增 CSV 字段：`player_shots`、`locator_us`、`enemy_us`、`player_logic_us`、`hostile_logic_us`、`hud_us`、`band_splits`、`band_overflow`、`motion_min_px`。调试 ELF 的计时可用于定位瓶颈；发行 ELF 用于最终无统计负担的对照。

## DDR 布局

| 区域 | 地址 / 容量 | 用途 |
|---|---|---|
| ram | `0x00001000` / 15 MiB | 代码、栈、常规数据、方块运动表；运动表必须保持在旧 MOT2 允许的低地址范围 |
| 帧缓冲与演示图片 | 原 `0x01000000` 起的 2 MiB 槽 | 保留 1280×720 RGB565 画布 |
| snd | `0x02000000` / 16 MiB | 音库 |
| art | `0x04000000` / 32 MiB | 启动生成美术与缩放素材 |
| stress | `0x06000000` / 32 MiB | Game / Th 的大弹池及三缓冲绘制表；约 30.9 MB |
| band bins | `0x08000000`–`0x0ACFFFFF` / 45 MiB | 45 带，每带 1 MiB |

DDR 控制器实际地址宽度 28 位，覆盖 256 MiB。上述区域不重叠。`.stressbss` 是不下载的 NOBITS 区域，由 `game_init()` / `th_init()` 显式初始化；没有扩大低地址 ram 穿过帧缓冲。新大小与布局不占用新的 FPGA RAM 块。精确链接占用与文件哈希见 `BOARD_TEST/r5_5/manifest.json`。

## 超频与后续优化

按已安装的 AMD Vivado 2021.2 技能确认工具适用范围；本器件是 Efinix Ti60，交付时序以 **Efinity 的布局布线报告** 为准。未用 Vivado 给 Ti60 生成位流，也未添加 false path / multicycle path 掩盖真实路径。

旧 R5.2.1 bit：核心 200MHz，setup +0.079ns、hold +0.022ns，XLR 58,428/60,800、片上 RAM 253/256。只把该 setup 裕量折算为同一路径的零裕量周期，约为 4.921ns（203.2MHz）；这个估算不构成超频保证，尤其不能据此保证 205MHz 或 210MHz。R5.5 本次重新布局布线后，核心 setup **+0.224ns**、hold **+0.026ns**；全部 7 组 setup / hold 均为正。XLR **58,568/60,800**、RAM **253/256**、DSP **32/160**。新的同路径零裕量估算约 209.4MHz，仍不是可直接交付的频率；202/205MHz 只能作为后续独立候选。完整报告见配套 manifest 与 reports。

建议下一步按新计时选择优化对象：

1. 若 `hostile_logic_us` 主导，优先减少 32B 弹幕结构的写穿 DDR 访问，拆分热字段、压缩绘制表写入或加入专用弹幕更新 DMA；必须保持轨迹、碰撞、三缓冲所有权一致。
2. 若 `locator_us` / `hud_us` 主导，继续保持素材与文字缓存，减少变化数字的字体缓存重置，用实测 A/B 验证。不要在 100K 弹池上额外重复扫描来制作统计。
3. 若大数量出现 `band_splits>0`，可研究每带实际占用的分桶方式、带溢出的有序续绘或可读回已有帧的分带流水线。现有严格上界方案保障完整画面，但较保守。
4. 主机 UART 每秒少量控制命令不是逐帧提交瓶颈。应优化的是 RISC-V 到 GPU 的 TAB/SQTAB、列表发布、缓存刷新和 DDR 访问。继续按批次发布，避免退回每对象 APB 写寄存器。
5. 最后才试 202/205MHz 等独立候选：更新实际 PLL、时钟约束、BSP 时基与超时换算，重新检查全部 setup/hold、DDR 校准及 CDC；全部通过后再测视频/音频欠流和长时间稳定性。保留 200MHz 可回退版本。

## 使用与上板记录

1. 加载 `BOARD_TEST/r5_5/ti60_r5_5.bit`，再下载 `ti60_r5_5_debug.elf`。
2. 重启上位机服务，确认页脚 **R5.5-PRESSURE-99999**。Linux：在工程目录运行 `bash Start_Control_Panel.sh --port 8765`；Windows：`Start_Control_Panel.cmd` 或本次 GitHub Actions 制作的 EXE。
3. 先用既有 R2 条件按钮复测 15K/16K 弹幕、8.8K/8.9K 随机方块。按钮恢复公共测试设置；R5.5 的压力无枪火和新轮廓算法不同于 R2 固件，不能把条件匹配解释成所有内部实现相同。
4. 再选小尺寸，从现有负载按 100/10/1 步进增加。等待弹幕实际在场数量稳定，比较实际呈现、渲染、逻辑三种频率。99,999 直接设置用于容量检查，不代替稳定帧率搜索。

| 待用户补充 | 配置 | 实际在场 | 新画面 FPS | 渲染 FPS | 最大工作 µs | 欠流 | 备注 |
|---|---|---|---|---|---|---|---|
| 萝卜基线 | 13px / 15K / Alpha / 自由运行 | | | | | | player_shots 应为 0 |
| 萝卜边界 | 13px / 16K / Alpha / 自由运行 | | | | | | 记录 locator / hostile / HUD |
| 小弹容量 | 1 / 3 / 6px，各测 99,999 | | | | | | 记录 band_splits 与 band_overflow |
| 方块基线 | 混合 / 8,800 与 8,900 | | | | | | 与旧 CSV 同条件 |
| 小方块容量 | 1 / 3px，各测 99,999 | | | | | | motion=1、motion_min_px=1 |

## English notes

R5.5 disables player automatic gunfire and skill volleys **only in carrot stress modes**, raises carrot/Touhou stress and square capacity to **99,999**, and adds small pixel sizes. Normal carrot gameplay retains automatic dual guns, upgrades and skills. The ordinary Touhou bullet cap remains 16,384. Existing percent-based Touhou artwork and mixed square sizes remain available.

The supplied board log shows approximately 51.33 render FPS at 15K carrot bullets, with 12.83ms logic and 6.80ms submission. Player projectile logic accounts for about 0.43ms; it cannot alone explain the regression. The old enemy-render metric also included locator coverage. The bounded 256-entry nearby index could overflow and scan the full table; R5.5 eliminates that fallback during normal table generation and counts coverage once per row. New telemetry separates locator, actors, HUD and player/hostile logic. No new on-board FPS is claimed.

Large pools are in an explicitly initialized, non-loaded 32MiB DDR region at 0x06000000. Band bins occupy 45MiB at 0x08000000. The low-memory square table stays within MOT2's existing address guard. Existing 16-bit table protocols are preserved through multiple commands; four aligned motion/draw batches cover all 99,999 squares without truncation or alpha-parity changes. Band capacity is guarded conservatively; excess records render directly in order instead of silently overflowing a bin.

The revised MOT2 accepts 1–40px and advertises this in register 0x44. New ELF files remain compatible with the old bitstream by falling back to CPU motion for unsupported small sizes. Use the new paired bitstream for hardware movement at 1px/3px. CPU Alpha5 and the GPU's 8-bit blend have different rounding; RTL verification uses the independent GPU arithmetic reference.

The clock remains 200MHz. Overclocking requires a new real PLL configuration, matching constraints and firmware clock constants, positive post-route setup/hold across every reported clock relationship, and physical validation. Timing exceptions are not a performance optimization. Final timing/utilization, hashes and local verification results are stored with the release; real-board limits remain for the user to fill in.
