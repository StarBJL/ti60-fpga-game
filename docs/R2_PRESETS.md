# R5.4 上位机：R2 上限条件一键复测

本次只更新 Linux / Windows 上位机。继续使用 R5.3 调试 ELF 和配套 R5.2.1 bit，不需要重新编译或下载 FPGA。按钮位于主界面上方的“R2 上限条件复测”区域。

## 六个按钮

| 场景 | 目标数量 | 尺寸 | 模式 |
|---|---:|---|---|
| 萝卜弹幕 | 15,000 | 13px | 自由吞吐，VSync 关 |
| 萝卜弹幕 | 16,000 | 13px | 自由吞吐，VSync 关 |
| 萝卜弹幕 | 15,000 | 13px | 60 帧呈现验证，VSync 开 |
| 运动方块 | 8,800 | 混合 6–40px | 自由吞吐，VSync 关 |
| 运动方块 | 8,900 | 混合 6–40px | 自由吞吐，VSync 关 |
| 运动方块 | 8,800 | 混合 6–40px | 60 帧呈现验证，VSync 开 |

R2 原始日志中，约 15,900 个萝卜在场弹体接近 60 渲染 FPS；8,900 个透明混合尺寸方块的 90 条记录平均约 59.71 渲染 FPS。这里使用 15,000/16,000 和 8,800/8,900 两侧负载进行对照；开启同步的按钮用于进一步检查呈现稳定性，不预判能否通过 60 FPS。

原始依据：`Ti60_720p_MERGE_R2_ZH_FULL/logs/20261023/uart_20261023_204925_104_COM5_c91971_720P-R2.txt`，以及工程已有 `verification/r5_3/performance_comparison.json`。屏幕统计采用 R2 的 STATS=1 调试构建默认值；原日志并未逐窗口记录所有开关，因此不声称恢复了历史测试中的每一次临时操作。

## 统一设置

所有预设都设置：720P60、游戏画面（彩条关）、Alpha 开、自动演示关、背景音乐关、测试音关、屏幕统计开、详细报告开、数量步长 100。恢复 DDR 硬件列表、分带、源缓存、K 通路、引擎弹幕表、逻辑建表、排序和重叠；LIMIT 为 16 / 128 / 64，宽通路使用原默认值。方块还开启硬件运动与紧凑表。

点击萝卜预设会重新开始自定义压力模式；点击方块预设会恢复混合尺寸并设置数量。仍运行当前固件的 R19 玩法、美术和混合尺寸生成逻辑；实际画面状态不等同于旧日志中的某一帧。旧 FPS 不会填入当前测量曲线。

执行后仍可按 100 / 10 / 1 增减数量。数量变化会显示当前目标及预设初始值；若改动 Alpha、分带、自动演示、音频或同步等固定条件，预设区会提示“基准条件已改变”。方块 `overlap_active=0` 是预期状态，因为运动更新和绘制共用对象表。

## 确认与记录

上位机只使用现有固件指令，按顺序逐条等待同 ID 的 ACK。最后核对同一 status 请求的 S4/A4/V5/E5 完整回报，再等待一个更新的 M4/F4/P4/C4 完整窗口，检查 epoch、场景、数量与全部固定条件。成功提示表示配置已确认，实际性能看“实际呈现”“渲染循环”和“重复帧”。

执行期间锁定其他配置写入。设备拒绝、回执超时、状态不完整、回报值不匹配、重启或断连会停止流程；界面不会将部分设置称为成功。已执行的设置不自动回滚；当前状态仍以设备回报为准。断开串口可取消预设。

日志添加 PRESET_BEGIN / PRESET_CONFIRMED / PRESET_FAILED 等事件。CSV 新增：

| 字段 | 含义 |
|---|---|
| `r2_preset` | 本次选择的预设 ID |
| `r2_stage` | applying / verifying / applied / failed 等执行阶段 |
| `r2_conditions_match` | 已完成确认，且该窗口固定条件仍匹配时为 1 |
| `r2_initial_target` | 点击按钮时的基准目标数量；当前数量仍看 target/count |

确认所用的第一个窗口记录为 verifying；后续合格窗口标记为 1。数量细调不改变固定条件匹配标记，Alpha 等条件改变则标记为 0。分析时应同时读取匹配标记、当前 target/count 和实际测量值。

## 启动与兼容性

停止旧上位机服务后，从原工程重新运行：

```bash
bash Start_Control_Panel.sh --port 8765
```

刷新页面，页脚应为 `R5.4-R2-PRESETS`。连接后等一条完整测量数据，六个按钮才会启用。需要能报告加速和统计条件的 R5.3 调试 ELF；发行 ELF 或旧固件缺少字段时会说明原因。

Windows 可用同目录的 `Start_Control_Panel.cmd`，也可下载本次 GitHub Actions 构建的独立 EXE。原有 10 首音乐、音量加减、100/10/1、尺寸、分辨率和串口识别功能保留。

## 验证

14 项预设协议/并发/异常测试通过，包括旧窗口和旧 epoch 不可确认、NACK/超时、固件重启、条件偏差、断连取消、重复请求隔离和数量细调。六个预设通过真实 PTY 串口与 Chrome 回归，共 127 条命令，并验证 CSV 标记、刷新恢复与桌面/手机布局。原有控制完整浏览器回归为 182 条命令；11 项端口、6 项跨平台和5项既有协议测试通过。以上测试未打开真实开发板串口，未测量新的实板上限。

## English

Six host-side presets reproduce the documented R2 comparison settings for carrot bullets (15K/16K at 13px) and mixed-size squares (8.8K/8.9K), with separate free-running and synchronized presentation checks. No firmware or RTL change is required beyond the existing R5.3 measurement firmware.

Each preset waits for matching ACKs, a complete same-request status response and a subsequent complete telemetry window. Partial failure, reboot, disconnect and condition drift are explicit. CSV provenance distinguishes the preset's initial target from later quantity refinement. Historical FPS is not presented as a current result.
