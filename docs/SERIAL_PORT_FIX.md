# R5.1 上位机串口识别修复 / Serial-port selection fix

本次更新为 Python / Linux 上位机源代码修复，不更改 FPGA bit 或 RISC-V ELF。原 R5 硬件交付清单和 ZIP 保留为构建基线；本次客户端变更的校验记录在 `verification/r5/serial_ports/fix_validation.json`。

原问题：端口列表包含 `/dev/ttyS31` 等系统串口，界面直接选中枚举结果第一项；FT4232H 的多个接口也显示相同描述，容易选到 JTAG 或未使用的通道。

Ti60F225 V4 原理图确认 **CDBUS0/1 接日志 UART，B 通道为 OpenOCD JTAG**。该主机核对时，USB 0403:6011、接口 `3-4:1.2` 对应 `/dev/ttyUSB2`。端口编号会随插拔改变，因此代码依据 USB 接口号识别通道 C，绝不将数字 2 硬编码为日志口。

修复行为：

- 根据 Linux sysfs 的 `bInterfaceNumber` 或明确的 USB 接口标识区分 FT4232H 通道；Windows 仅在有明确接口标识时推荐，未知情况保持手动选择。
- 默认列表隐藏描述和硬件信息均为 `n/a` 的 `ttyS*` 项，仍允许手动输入设备路径。
- 仅有一个通道 C 候选且输入框未被用户修改时自动填入；多个候选或通道未知时不猜测。
- 显示通道、USB 位置和用途。已知非 Ti60 V4 日志通道在打开前被拦截。
- 快速刷新不会让旧请求覆盖较新的列表或手动输入；浏览器刷新后仍显示服务器实际连接的端口。
- 枚举只读取元数据，不打开端口、不发送探测字符；固件协议由连接后收到的数据独立确认。

当前正在运行的旧服务可以继续记录。结束记录后，按 Ctrl+C 停止它，再运行：

```bash
cd /home/user/fpga_contest/1025/Ti60_R5_DUALVIDEO
bash Start_Control_Panel.sh --port 8765
```

刷新 `http://127.0.0.1:8765`，页脚应显示 `R5.1-PORTS`。选择标注“FT4232H 通道 C”的日志口，115200 baud，再连接。

预编译 Windows EXE/ZIP 属于原 R5 基线；本修复会随最新源码或下一次 Windows CI 构建进入程序，不将旧 EXE 标为已包含该修复。

## Validation / 验证

11 项端口测试、6 项跨平台协议测试、5 项已有日志协议测试通过。Chrome 验证默认推荐、手动输入、多个板卡、未知接口、无自动连接及移动布局；完整伪终端控制回归通过，包含刷新已连接页面、数量步长、尺寸、分辨率、组合键、音乐和导出。真实主机的只读枚举确认通道 C 为 `/dev/ttyUSB2`；测试没有打开真实串口或发送硬件控制命令。

This source-only console update selects FT4232H channel C using USB interface identity, not OS port numbering or enumeration order. It preserves manual choices, avoids ambiguous auto-selection, labels non-UART channels, and does not probe devices. A running logging session can continue; restart the console to load the updated backend. The FPGA bitstream and firmware remain the original R5 artifacts. The prebuilt Windows package is the earlier baseline; build from current sources for this fix.
