# R5.2 上板问题修复 / Board regression repair

本次依据用户提供的 `uart_122242_243853_R5-CONSOLE.csv`（407 个窗口），以及同一会话保存在工程 `logs/r5/20261024` 的完整原始输出、操作事件和 CSV（672 个窗口）分析。原始文件及选取窗口的哈希见 `verification/r5_2/board_log_analysis.json`。日志时间为 UTC；不将浏览器伪串口数据当作实板性能。

## 1. 稳定 60 帧上限下降

数目存储上限仍为：东方 16,384、萝卜压力 40,000、方块 30,000。它们不等于稳定 60 FPS 上限；后者必须在相同画面、尺寸、Alpha、同步、输入/自动演示和加速设置下实测。

| 实板窗口 | 负载 | 渲染 / 实际呈现 | 提交耗时 | 分带耗时 |
|---|---|---|---|---|
| seq 569 | 萝卜目标 13,100，13px | 58.5 / 58.5 FPS | 5,829 us | 9,071 us |
| seq 676 | 萝卜目标 13,100，13px | 21.5 / 21.5 FPS | 32,157 us | 0 |
| seq 1006（完整日志） | 方块 20,000，原混合尺寸 | 13.5 / 13.5 FPS | 61,734 us | 0 |

操作事件显示在前两段之间依次使用了 `k`、`z`、`q`、`0`、`5`：提交从硬件 DDR 列表切到 APB 列表字，关闭逻辑建表、重叠、源缓存和分带；后续 `k` 又切到逐对象调用。它们是用于 A/B 对比的降级路径，会跨场景保留。旧上位机只显示“硬件绘制”，没有暴露全部状态，导致难以发现性能条件已改变。不同窗口还含自动演示等变化，不能据此量化 R4 与 R5 在统一条件下的差异。

修复：增加明确的 `:id fast` 命令和“恢复加速配置”按钮，恢复硬件列表、分带、源缓存、K 通路、逻辑建表、引擎弹幕表、排序、重叠、默认 LIMIT/宽通路设置、方块紧凑表及可用的硬件运动。保留数量、尺寸、场景、Alpha、垂直同步和游戏状态。`E5` 回报即时状态，`M4` 把状态和测量绑定到同一 seq/epoch，CSV 增加相应字段。高级 A/B 按钮显示当前状态并说明跨场景保留。

## 2. 无声音：确认并修正 HDMI 音频包布局

日志已显示 33 个音效/音乐条目、音量 256、活动声部以及递增的混音/输出计数；本次记录内音频欠流增量为 0。这排除了“ELF 未包含音库”作为本会话的解释，但计数不能证明 HDMI 接收端实际接受了音频。

HDMI 1.4 表 5-13 规定音频子包第 6 字节按位为 `PR CR UR VR PL CL UL VL`。原 RTL 错写为两个校验位相邻、两个状态位相邻，破坏接收端看到的左右声道状态、有效性及校验。旧测试也采用相同错误布局，曾错误通过。现已修复四个音频子包及旧解码器；新增验证直接从最终三路 TMDS 符号解码，不读取内部音频包数据作为期望值。

同一标准解码检查对原始 R5 输出发现 6,184 个音频校验错误、6,161 个左右状态不一致和 189 个 V/U 错误；修复后为 0。样本来自仿真生成的固定 PCM 序列和 1 kHz 测试音，非板上录音。音库、混音算法、曲目和音量控制未修改。

参考：[HDMI 1.4a 规范（表 5-13）](https://felloff.net/doc/comp/spec/HDMISpecification14a.pdf)、[独立开源 HDMI 音频实现](https://github.com/hdl-util/hdmi/blob/master/src/audio_sample_packet.sv)。

上位机新增“音量 − / 音量 ＋”：每次增减 5 个百分点，0–100% 限幅，保留滑块。采用设备端相对命令 `:id volume_delta -1/1`，连续点击基于固件当前音量计算，避免旧页面状态导致重复写入同一个音量。状态回报驱动百分比和滑块同步。

## 3. 1080P 黑屏：修正同步对齐，增加可区分的诊断

实板 seq 587–597 中，1080P 状态下扫描约 29.9 Hz，视频欠流增量 0。说明扫描计数和 DMA 仍在运行，不能证明显示器锁定。原 VSYNC 在坐标行 x=0 改变；标准逐行时序的垂直同步边沿应与水平同步起始对齐。现将发往 HDMI 的 VSYNC 边沿对齐 HSYNC；内部 DMA 刷新和翻页窗口保持原来的独立逻辑。

保持原像素时钟 74.375 MHz、720P 总时序 1650×750、1080P 总时序 2200×1125；分别约 60.101 Hz 和 30.051 Hz。输出 VIC 为 4 / 34，内部画布仍为 1280×720，1080P 使用 3:2 最近邻放大。

新增只读视频修订号 `0x5c = 0x00050201`，串口 `V5 video_rev=328193`，可确认新 bit 是否真正加载。新增 `:id bars 1/0` 和彩条/恢复画面按钮，方便区分接收端时序锁定与帧缓冲显示问题。若显示器明确不支持 1080P30，需使用支持该格式的接收端；本更新没有把 1080P30 的实际兼容性或可听性标记为已上板通过。

参考：[独立 HDMI 时序实现与同步边沿](https://github.com/hdl-util/hdmi/blob/master/src/hdmi.sv)。

## 验证 / Validation

- 实际 TMDS 输出：10 个完整帧，720→1080→720→1080，13,824,000 个活动像素按固定源图逐像素比较通过。
- 9,736 条完整消隐行：控制符、同步边沿、前导、保护带、TERC4、音频包与 BCH、AVI/VIC、音频 InfoFrame、ACR 检查通过。
- 7,155 个 FIFO PCM 样本跨模式保持顺序；4,797 个稳定测试音样本符合 1 kHz 正弦序列；测得仿真采样率约 47,997.68 Hz（有限观测窗口），硬件音频/视频欠流计数均 0。
- 原始 R5 在同一标准检查下失败 17,643 次；候选修复为 0。保存前后结果，避免只修改期望值让测试通过。
- 真实 C 主循环与引擎模型：控制、场景、流水翻页、分辨率、数量、技能回归通过；新恢复命令的状态与负载保持断言通过。
- Chrome + 真实 PTY：111 个串口命令，含音量 ±5% 连续点击、0/100% 限幅及滑块同步、恢复加速、诊断降级状态、彩条、数量 100/10/1、尺寸、组合键、分辨率及 CSV 导出；桌面/手机宽度无横向溢出。
- 11 项端口识别、6 项跨平台协议测试、5 项旧协议测试通过；调试与发行 ELF 均构建。最终 Efinity：200 MHz 核心 setup +0.079 ns、hold +0.022 ns；7 组报告的时钟关系 setup/hold 全部为正；XLR 58,428/60,800、RAM 253/256、DSP 32/160。采用工具支持的 TIMING_2、布局强度 4、seed 2；约束频率和 RTL 引擎吞吐结构不变。4 个负裕量候选已拒绝，构建脚本增加时序失败禁止交付的检查。

## 复测顺序

1. 下载 `BOARD_TEST/r5_2/ti60_r5_2.bit`，再运行同目录的 `ti60_r5_2_debug.elf`。单独换 ELF 不会修正 HDMI 硬件。保留原 R5 作为回退。
2. 重启上位机服务并刷新页面，页脚为 `R5.2-BOARD-FIX`；读取状态应显示 R5.2.1 HDMI 修正版。确认 `video_rev=328193`。
3. 先留在 720P60，打开测试音；记录是否可听，再关闭测试音检查 BGM。记录实际接收声音的设备/音频输出接口。
4. 点击恢复加速配置，选固定场景、固定尺寸、固定 Alpha，打开垂直同步。等 2 秒稳定后每档采集至少 30 秒，用“实际呈现”、重复帧和最大工作时间判断 60 FPS 稳定性；数量先按 100、再 10、再 1 细化。分别记录萝卜 9px/13px、方块原混合尺寸/固定尺寸。
5. 在低负载状态切 1080P30，记录显示器输入信息和画面。黑屏时试彩条；若仍黑，记录“无信号/不支持”还是已识别 1080P30。可随时通过上位机切回 720P60。
6. 导出 CSV、原始输出和操作事件。补齐显示器型号、旧版同连接出声情况、修复后的测试音/BGM和 1080P 是否正常；在此之前不宣称实板上限已恢复。

## English summary

The supplied session contains persistent diagnostic switches disabling the accelerated render path. R5.2 exposes the effective settings in UI and per-window telemetry and adds an explicit restore command without changing the workload. No new stable-FPS limit is claimed from these heterogeneous samples.

The HDMI Audio Sample Packet status/parity byte was packed incorrectly. The prior test mirrored the same defect. Four subpackets and the decoder are corrected against HDMI Table 5-13, and final TMDS characters are independently decoded with PCM, tone, ECC and InfoFrame checks. Transmitted VSYNC edges are also aligned to HSYNC for CTA progressive timing, while the internal DMA interval stays unchanged. The new video revision register identifies the repaired hardware; a colour-bar command helps isolate display-link problems.

Simulation and software/browser results are documented separately from physical-board results. Audio audibility, display lock/1080P30 support, and stable 60-FPS object limits require the user's retest with the matched bitstream and ELF.
