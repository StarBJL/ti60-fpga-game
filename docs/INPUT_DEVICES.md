# R5 输入硬件与固件接口 / Input integration

交互版：[按键、接线与协议指南](INPUT_GUIDE.html)。本文件记录可用于集成的接口；实板电气和键盘兼容性验收尚待用户填写。

## 矩阵板的 8 根线与 16 个按键

纯开关 4×4 矩阵使用 4 行、4 列，共 8 根信号线。每个按键连接一个行列交点，不为每个按键分配独立 GPIO。照片中正面由左至右、由上至下为 S1—S16；**8 针排针的实际线序不能由线的颜色或照片推断**，应使用万用表导通挡确认 R0—R3、C0—C3，再按表连接。绿色 2×20 转接排针必须先核对 Pin 1 方向及与主板 J7 的通断映射，以下数字是主板 **J7** 位置。

| 矩阵信号 | J7 | 原理图网络 | FPGA 球位 | 原理图资源标识 | Bank |
|---|---:|---|---|---|---|
| R0 | 2 | LVDS_P3 | B1 | GPIOL_P_17_EXTFB | 1B / 1.8 V |
| R1 | 4 | LVDS_N3 | C2 | GPIOL_N_17 | 1B / 1.8 V |
| R2 | 8 | LVDS_P1 | F1 | GPIOL_P_14 | 1B / 1.8 V |
| R3 | 10 | LVDS_N1 | F2 | GPIOL_N_14 | 1B / 1.8 V |
| C0 | 20 | LVDS_P4 | D1 | GPIOL_P_16 | 1B / 1.8 V |
| C1 | 22 | LVDS_N4 | C1 | GPIOL_N_16 | 1B / 1.8 V |
| C2 | 26 | LVDS_P2 | B2 | GPIOL_P_18_PLLIN0 | 1B / 1.8 V |
| C3 | 28 | LVDS_N2 | A2 | GPIOL_N_18 | 1B / 1.8 V |

依据：`references/Ti60F225A_V4_SCH.pdf` 的 PDF 第 9 页（图纸 Sheet 8）给出网络→球位，PDF 第 11 页（Sheet 10）给出 J7→网络。手册的 J7 表存在不同映射，本工程以两段原理图相互对应的连接关系为依据，再由 Efinity 引脚数据库与实现检查验证。名称带 LVDS 不代表本用途必须使用差分接口：此处作为同 Bank 的独立 1.8 V GPIO 使用。

扫描器每 250 μs 选择一行，完整扫描为 1 ms；连续 8 个相同完整样本后发布 16-bit 消抖状态。选择行驱动低电平，其他行高阻，避免多键把高、低推挽输出短接。列使用 FPGA 上拉至该 Bank 的 1.8 V；如果现场需要更强上拉，只能接至经确认的 1.8 V 电源。纯开关矩阵本身不需要额外 VCC/GND；带 LED、控制芯片或电阻网络的板子需另核对电路。

未确认二极管的矩阵不能保证任意多键无冲突。软件检测两行共享两列形成的矩形；一旦出现，释放矩阵保持态，并抑制该矩阵直到所有键松开。此策略宁可暂时不响应，也不让幽灵键切换分辨率或启动技能。需要同时方向＋射击＋慢速时，CH9350L 键盘更适合；矩阵若要任意多键，应为每个开关加独立方向一致的二极管并另验证。

## 确定的 S1—S16 功能

`Fn` 是 S4，按住后再按第二个键。移动、射击、技能、慢速为保持态，其余只在按下沿触发一次。配置键长按不自动重复，数量档位由 S11 明确选择。

| 物理位置 | 基本功能 | 按住 S4 / Fn 后 |
|---|---|---|
| S1 / R0C0 | 下一场景 | 返回菜单 |
| S2 / R0C1 | 上移 | 选择升级项 1 |
| S3 / R0C2 | 射击 | 选择升级项 2 |
| S4 / R0C3 | Fn | Fn |
| S5 / R1C0 | 左移 | 选择升级项 3 |
| S6 / R1C1 | 下移 | 重新开始 |
| S7 / R1C2 | 右移 | 自动演示开关 |
| S8 / R1C3 | 技能 / Bomb | Alpha 开关 |
| S9 / R2C0 | 数量减少当前步长 | 音量减少 |
| S10 / R2C1 | 数量增加当前步长 | 音量增加 |
| S11 / R2C2 | 步长 100 → 10 → 1 → 100 | 统计面板开关 |
| S12 / R2C3 | 当前场景下一尺寸档 | CPU / 硬件渲染切换 |
| S13 / R3C0 | 菜单确认 / 游戏暂停或继续 | VSync 开关 |
| S14 / R3C1 | 进入压力菜单；萝卜菜单内再按一次选自定义 | BGM 开关 |
| S15 / R3C2 | 720p60 / 1080p30 切换 | 下一首 BGM |
| S16 / R3C3 | 慢速 | 查询状态 |

Fn 新按下时不会把已经按住的普通键重新解释成一次配置动作；应先按住 Fn，再按功能键。矩阵进入 Fn 层时停止其普通移动/射击保持态。萝卜压力菜单内再次按 S14 直接选择自定义档 4，因此不依赖外接电脑也能设置 40K 范围。各场景不适用的动作由公共控制器拒绝，不把它解释为其他旧快捷键。

## CH9350L 接法与工作状态

使用用户给出的 [Xinlu 模块页面](https://www.xinlucity.com/?s=resourcedetail/index/id/423.html)，协议依据 [WCH CH9350 手册 v2.7](references/CH9350DS_V2_7.pdf)，尤其 PDF 协议部分第 14—16 印刷页。请以购入模块的丝印、拨码表和实际电平为准。

| 接线 | 目的 | 电平/供电 |
|---|---|---|
| CH9350L TX → J7 32 | FPGA UART RX，球位 C5，GPIOL_11_PLLIN2 | TL Bank，3.3 V 逻辑 |
| CH9350L RX ← J7 34 | FPGA UART TX，球位 A4，GPIOL_09 | TL Bank，3.3 V 逻辑 |
| 模块 VCC → 经确认的 5 V | USB 键盘及模块供电 | 模块供电为 5 V，不是 1.8 V |
| 模块 GND ↔ 主板 GND | 共地 | 独立于两个 UART 信号脚 |

UART 使用 115200 baud、8 数据位、无奇偶、1 停止位。TX/RX 必须交叉，不能把模块 TX 与板载调试串口 TX 并接。J7 3.3 V 信号剩余 37、38、39、40 四个；这不是四个供电脚。

CH9350 必须作 **下位机 / USB Host**：SEL 上电为高。选 **工作状态 2**：S0=低、S1=高（拨码 ON/OFF 和高低电平不一定相同，应查实际模块接法）。状态 2 将标准 USB 键盘整理成固定 8-byte boot 报告；本驱动不支持状态 0/1 的 variable-length 帧、专用 NKRO 报告或多媒体键。

状态 2 键盘帧是 `57 AB 01` 后跟 8 字节：modifier、reserved、最多 6 个 HID usage。释放所有键的报告为 8 个 00。鼠标帧 `57 AB 02 + 4 bytes` 被完整跳过，不会被当成按键。状态 2 不运行状态 0/1 的启动握手；但按手册 §8.3.9 / §8.3.11，`57 AB 80 + status` 需要状态应答。驱动发出 `INPUT_CH9350_STATE`，应用先排空 RX FIFO，再连续发送 11 字节 `57 AB 12 00 00 00 00 00 FF 00 20`：4 字节 PID 槽为 0，键盘 LED 为 0，USB 状态为 FF，配置为 0，协议版本为 20。115200 8N1 的线上发送约 0.96 ms；等待 TX busy 的软件超时为 20 ms，避免把字节分散到多个渲染帧。`57 AB 87` / `89` 被统计为状态消息。当前应答固定键盘指示灯灭，不提供用户 LED 切换。该应答已在代码和离线模型中实现，尚未对用户具体模块实测。模式 0 页面例程的 `57 AB 88 ...` 不能送进本模式 2 解码器后期待正常使用。

键盘 usage 不等于 ASCII。驱动把 WASD/方向、Z/Space、X/J、Shift、Esc、Enter 变为 PAD 保持态；将 F6/F7/F8、F9、配置字母和符号变为边沿语义动作。重复相同报告不会重复切设置。错误 usage 1—3（rollover 等）释放键盘保持态。先排空 UART RX FIFO，再以软件轮询判断：不完整帧持续 20 ms 没有后续字节才丢弃；下一帧重新寻找 57 AB。软件取字节时间不等于线上到达时间，所以正常帧即使跨越两次相隔 33 ms 的忙碌主循环轮询，也先处理已有续帧，不提前误判。

**状态 2 主要按变化上报，不可用“300 ms 无串口数据”判断松键。** 固件保存独立、无租约的实体保持态；PC 控制使用心跳租约。只有实际按键动作才标记实体输入已经使用，包括矩阵 S13 这种不包含移动位的动作；仅探测到 INP5 不会切换控制方式。实体按键全部松开后保持“已使用且已释放”，避免错误回到旧终端自动射击；未使用实体外设时仍保留终端兼容控制。若模块掉电却没发松键报告，单凭 RX 静默无法区分“仍长按”和“已断开”。本版提供 `input_keyboard_release()`，由明确的断连/串口错误或恢复流程调用；上板要测热拔插与恢复，不宣称具有无法证明的自动拔插检测。

## APB 与 C 对接

输入设备寄存器基址：**0xF8100080**。它是 HDMI APB 地址空间中的独立子区，不占原显示寄存器。

| 偏移 | 含义 |
|---:|---|
| +0x00 | 只读 ID，ASCII `INP5` |
| +0x04 | 消抖矩阵状态，bit 0=S1，bit 15=S16 |
| +0x08 | UART 状态：bit0 RX valid，bit1 FIFO full，bit2 sticky error，bit3 TX busy；bits14:8 RX FIFO 字节数 |
| +0x0C | 读出并弹出一个 RX FIFO 字节；仅在 valid=1 时读取 |
| +0x10 | TX 数据写入；仅在 busy=0 时写 |
| +0x14 | 写 bit0=1 清除错误并丢弃 RX FIFO 中的全部旧数据；清除前记录诊断并释放失信的键盘保持态 |

`input_devices.h` 定义 `InputAction` 和 `InputDevices`；`input_devices.c` 不直接访问 MMIO，也不假设当前场景。应用通过回调把语义映射到现有 `ControlEvent`，因此“next / toggle”始终读取真实当前配置，而不是驱动中一份可能过期的影子状态。

```c
InputDevices devices;
input_devices_init(&devices, on_device_action, application_context);
/* Main loop: read a stable matrix sample, then drain only valid RX bytes. */
input_matrix_snapshot(&devices, matrix_bitmap);
while (uart_rx_valid())
    input_ch9350_feed(&devices, uart_rx_pop(), now_ms);
input_devices_poll(&devices, now_ms);
/* Source-aware integration refreshes physical held state and ORs it with
   the leased PC state. A PC stop must not erase a still-held physical key. */
```

输入诊断通过 `I5` 行输出：`supported`、合并 `held`、`keyboard_reports`、`malformed`、`rollover`、`ghosts`、`status_reports` 和当前 `uart_error`。它记录输入链状态，不是性能成绩；错误恢复会清 FIFO 和 sticky error，串口日志保留恢复提示。

离线验证：`verification/r5/test_input_devices.c` 测试 Fn、保持与松开、两来源合并、矩阵鬼键、完整 65,536 矩阵状态、100/10/1 档、F9、截断帧、USB rollover、无数据长按保持及明确释放，使用 GCC 警告全开和 ASan/UBSan。它不替代 1.8 V 电气、扫描波形和具体 USB 键盘兼容性测试。

## English integration summary

The keypad is a passive 4-row / 4-column network, using eight 1.8 V Bank 1B GPIOs. Row-major bits map to S1–S16. Verify the actual eight-wire order and green 2×20 adapter orientation with continuity measurements. Only the selected row drives low; other rows are high impedance. Columns are pulled up to the same 1.8 V bank. The scanner produces a debounced bitmap after eight identical 1 ms scans. Rectangle ghost patterns release and suppress the keypad until all switches are released.

CH9350L uses a separate 3.3 V UART on J7 32/34, plus verified 5 V module power and common ground. Configure USB Host/downstream mode and state 2 (SEL high, S0 low, S1 high). The decoder accepts `57 AB 01` plus an eight-byte boot keyboard report, ignores complete mouse reports, rejects error usages, and maps HID usages to semantic actions. State-0/1 variable-length reports are not supported. A `57 AB 80 + status` message queues the state-2 response `57 AB 12 00 00 00 00 00 FF 00 20` after RX is drained, transmitted contiguously in about 0.96 ms with a 20 ms software timeout. This is implemented and modeled, not yet validated with the physical module. Reports are change-driven, so lack of UART traffic alone cannot establish disconnection.

The C adapter has no MMIO dependency. `InputAction` callbacks resolve context-dependent actions in the application; matrix and keyboard held states are combined, then merged with the separately leased PC input. The hardware register bank starts at `0xF8100080` with ID `INP5`. Actual input actions mark the physical source as used even when its held mask is zero; capability probing alone does not do so. This preserves legacy terminal behavior until physical interaction, without re-enabling terminal automatic shooting after a physical release. `I5` reports input counters and state. Offline adapter tests are available; electrical, hot-plug, and real-keyboard tests remain board validation items.
