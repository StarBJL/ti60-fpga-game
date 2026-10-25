# R5.9.1：检测完成后启动三人游戏

本次仅更新上位机，继续使用 R5.9 ELF 和 R5.8 bit。不修改 FPGA、PLL、BSP 或720P显示逻辑。

用户日志 uart_201609_035043_R5-CONSOLE_2017.csv 共94条采样，全部 input_diag=1、players=3、auto=0。USB keyboard_reports 从82增加到271、keyboard_bytes从1217增加到3852、keyboard_faults=0；矩阵也有有效输入，校准为标准0123456789abcdef。输入已进入解码器，但仍处于只检测模式，没有交给P2/P3。旧网页也在该模式下暂停P1的电脑键盘捕获。日志中非零PC方向指令发生在游戏失败之后。

## 使用现有固件恢复

1. 松开矩阵与USB键盘的所有按键。
2. 若正在校准，先应用完整映射或取消本次校准。
3. 旧版上位机：点“结束检测，恢复操作”，再点“启动萝卜三人合作”或“启动东方三人合作”，勾选“启用键盘控制”。
4. 新版R5.9.1：点击任一三人启动按钮即可；它会先读取新状态、确认结束检测，再开新局并勾选电脑键盘控制。
5. 保持浏览器窗口焦点，不在数量、串口等输入框里输入。P1用WASD/方向键，P2用USB键盘，P3用矩阵。
6. 验收：input_diag=0、players=3、auto=0。萝卜新局phase=2；phase=5表示失败，应重新开局。分别按键时对应p1_held/p2_held/p3_held应变化，松开归零。

重启上位机服务并刷新网页后，版本应为R5.9.1-COOP-INPUT；固件启动行仍为TI60-R5.9-INPUT-CAL-20261025，这是正常配套。

## 本次修复

- 三人启动先取得同一请求ID的S4/T7以及I5/I9，避免使用旧周期采样。
- 若检测仍开启，先发送input_diag 0，收到ACK且对应I9确认input_diag=0后才启动coop。
- 保留未完成的校准，不自动取消；按键未松开、拒绝回执、缺少完整状态、连接变化或固件重启都会中止本次流程并显示原因。
- 启动成功后自动勾选电脑键盘控制，明确显示检测阻塞和未开启捕获状态。
- 进入检测时释放已有PC保持键。仅按住键时保留心跳；松开只需发送一次0，不再空闲每100ms发送move 0。

## English

The board log shows valid physical input reports but input_diag=1 in every sample. Diagnostic mode intentionally suppresses physical gameplay events, and the browser also pauses PC key capture. R5.9.1 makes the explicit co-op buttons read request-tagged state, exit diagnostics with a confirmed response, then start a fresh three-player game and enable PC keyboard capture. Incomplete calibration and held keys are preserved/guarded. Reboots, negative ACKs and missing responses stop the operation. Firmware, bitstream and720p rendering are unchanged.

Validation uses a real local HTTP server plus a serial PTY fixture and headless Chromium; no physical serial device is opened. Tests cover both games, PC key down/release, independent player telemetry, diagnostic exit, failure guards, reboot handling, no idle-zero heartbeat and mobile layout. Native Windows packaging is tracked separately in windows_ci.json.
