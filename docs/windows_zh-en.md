# Ti60 R5 Windows / Linux control console

## 中文：如何运行

本上位机是本机 Python 串口服务加浏览器界面。Windows 使用 `COM3`、`COM12` 等设备名，Linux 使用 `/dev/ttyUSB0`、`/dev/ttyACM0` 等路径。服务器只监听 `127.0.0.1`，默认自动选择空闲端口。启动程序不会自动打开串口或向 FPGA 发送命令。

1. 完整解压工程到可写文件夹。中文和空格路径有专门回归测试；不要从 ZIP 预览窗口直接启动。
2. Windows 安装 Python 3.10 或更新版本（打包建议 Python 3.12，启用 Python Launcher）。双击工程根目录 `Start_Control_Panel.cmd`；原入口 `Open_Serial_Log.cmd` 也会转到同一启动器。第一次在工程 `.venv-control` 中安装 `pyserial==3.5`，不会修改系统 Python 的包。
3. 浏览器自动打开。选开发板的调试串口和 `115200`，填写记录标签，点击“连接并记录”。不要把 CH9350L 专用输入串口当成日志串口。
4. 等待设备遥测；若正在静默界面，点击“读取帮助 h”。仅收到启动横幅不会启用扩展控制。R4 控制协议保持兼容；分辨率按钮要求 R5 固件报告 `V5 ... supported=1`，即固件和 bit 均支持。
5. 控制台终端保留打开；结束时先在网页断开，再按终端 `Ctrl+C`。关闭网页不会停止本机服务；失焦、松键和断开会释放组合按键，心跳失效时也会释放。

Linux 使用 `./Start_Control_Panel.sh`。已安装 `pyserial==3.5` 时复用当前 Python，否则尝试创建独立环境；Debian/Ubuntu 缺少 venv 时安装与 Python 版本对应的 `python3-venv`。无需全局 pip 安装或管理员权限启动应用。

### 主要控制

| 功能 | 网页 / 电脑键盘（先启用键盘控制） |
| --- | --- |
| 方向 / 射击 / 低速 | WASD / 方向键；Z / 空格；Shift |
| 技能 / 符卡 | X / J 或“环形技能 / 符卡” |
| 确认 / 暂停 | Enter / Esc |
| 数量步长 | 100 / 10 / 1 按钮；F6 / F7 / F8 |
| 数量增减 | ＋ / − 按钮；+ / − 键 |
| 压力弹幕尺寸 | 下拉框；[ / ] |
| 方块尺寸 | 下拉框；{ / } |
| 720P60 / 1080P30 | 两个输出按钮；F9 切换 |
| 下一首音乐 | “下一首”；反斜杠键 |

模式说明：720P 输出为 1280×720，1080P 输出为 1920×1080；两者内部均渲染 1280×720，1080P 使用 3:2 最近邻放大。界面单独显示渲染尺寸和设备报告的输出尺寸。60 / 30 是模式标称值，实际扫描率应查固件遥测和硬件时序；不得把扫描率当成真实新画面帧率。切换等待硬件回报，显示器可能短暂重新锁定；回执只表示命令被接受，最终输出以 `V5` 状态为准。

数量上限取决于场景：东方 16,384，萝卜自定义压力 40,000，方块 30,000。这些是可配置对象容量，不是这些数量都已达到实时显示指标。输入框显示目标上限，“实际在场”单独来自遥测。

### 日志和导出

源码版保存到工程 `logs/r5/YYYYMMDD`；独立 EXE 保存到 `%LOCALAPPDATA%\Ti60R5\logs`，因此可从只读安装目录启动。可使用 `--log-root "D:\实验记录\Ti60 测试"` 指定目录。原始日志保留设备字节；事件 JSONL 用 UTF-8；CSV 使用 UTF-8 BOM，便于 Windows Excel 识别中文。下载按钮使用 UTF-8 文件名。

每个测量窗口只有在 `M4/F4/P4/C4` 四段相同 `seq` 都到齐时才写入归一化 CSV，避免跨窗口拼接。视频模式字段来自 `V5`。硬件不支持实际呈现计数或性能剖析时，相关项为空，不制造替代数值。导出原始日志、指标 CSV 和操作事件可用于后续实板补测。

常用命令：

```bat
Start_Control_Panel.cmd --port 8765
Start_Control_Panel.cmd --log-root "D:\实验记录\Ti60 测试"
Start_Control_Panel.cmd --no-install
Start_Control_Panel.cmd --check
```

`--no-install` 用于已经准备好依赖的离线机器，不访问网络；环境缺失时明确报错。离线安装可将 `pyserial-3.5-py2.py3-none-any.whl` 复制到机器，再运行 `.venv-control\Scripts\python.exe -m pip install 路径\pyserial-3.5-py2.py3-none-any.whl`。虚拟环境未创建时，先执行 `py -3 -m venv .venv-control`。

### 可选：生成独立 Windows EXE

在 Windows 上双击 `Build_Windows_Control_Panel.cmd`。脚本创建单独的 `.venv-control-build`、安装固定版本 PyInstaller、打包 HTML/CSS/JS 与 pyserial，输出：

- `dist/windows/Ti60Control.exe`
- `dist/windows/Start_Ti60Control.cmd`
- `dist/windows/README.txt`

将整个 `dist/windows` 文件夹交给用户。运行 `Start_Ti60Control.cmd` 会打开浏览器；也可手动 `Ti60Control.exe --open`。目标机器无需另装 Python。打包保留终端窗口以显示错误，不隐藏失败信息。构建脚本只能在 Windows 运行；PyInstaller 不支持从 Linux 直接交叉生成 Windows EXE。参考 [PyInstaller 官方说明](https://pypi.org/project/pyinstaller/6.16.0/) 和 [spec / usage 文档](https://pyinstaller.org/en/stable/usage.html)。

`tools/control_panel/windows-control.yml` 提供手动 GitHub Actions 模板；放到 `.github/workflows/windows-control.yml` 后，可在 Windows Runner 上执行同一构建。模板本身不会执行工作流，也不会上传任何串口日志。

### 已验证与待补测

本轮执行环境为 Linux；未在 Windows 操作系统或真实 FPGA 板卡上验证 Windows 驱动、原生 CMD、EXE 或 HDMI 重新锁定。以下结果区分清楚：

| 项目 | 状态 / 证据 |
| --- | --- |
| COM12 配置分支、不传 POSIX exclusive 参数 | 自动化通过；Windows 驱动用 mock 隔离 |
| 中文与空格日志目录、UTF-8 JSONL、Excel BOM CSV | 自动化通过 |
| 旧 R4 / 不配套 bit 禁止分辨率控制 | 自动化通过 |
| V5 输出 / 渲染尺寸记录、模式命令参数边界 | 自动化通过 |
| localhost API、静态资源、请求 token | 自动化通过 |
| Chrome + 实际 Linux PTY 串口收发 | 64 条命令通过；遥测是明确标注的模拟数据，非实板帧率 |
| 100/10/1、尺寸、组合键/失焦释放、音乐、按钮/F9、CSV、桌面/手机布局 | 浏览器回归通过，无页面错误 |
| 隔离环境启动：中文空格目录、首次安装、离线复查 | 见 `verification/r5/windows_console/bootstrap_results.json` |
| Windows 10/11 原生启动与 USB 串口驱动 | **待用户补测** |
| Windows EXE 构建、首次启动、杀毒软件行为 | **待 Windows 构建/补测** |
| 真板两模式、掉电/重连、长时间压力、掉帧/欠流 | **待用户补测** |

## English: operation and scope

The console runs a loopback-only Python HTTP service and a local browser UI. Run `Start_Control_Panel.cmd` on Windows (Python 3.10+ required; Python 3.12 recommended for packaging), or `./Start_Control_Panel.sh` on Linux. The Windows launcher creates an isolated `.venv-control` environment and installs the pinned `pyserial==3.5` dependency. `--no-install` prevents dependency installation/network access; `--check` only validates the Python environment.

Choose a debug UART such as `COM3` / `COM12` on Windows or `/dev/ttyUSB0` on Linux, set 115200 baud, and click Connect. Startup never opens a serial port automatically. R4 framed commands remain supported. The new output-mode controls are enabled only after firmware reports `V5 supported=1`; an R5 ELF with an older incompatible bitstream must not advertise that capability.

Enable keyboard capture before using WASD/arrows, Z/Space (shoot), X/J (skill), Shift (slow), Enter (start), Esc (pause), F6/F7/F8 (100/10/1 quantity steps), or F9 (720p60/1080p30). Both output modes render a 1280×720 framebuffer. The 1920×1080 mode scales it by 3:2 using nearest-neighbor sampling. The UI reports physical output dimensions separately from render dimensions. Nominal refresh, scan rate, render-loop rate, and actual new-frame presentation rate are distinct metrics.

Source runs save to `logs/r5`; packaged runs save to `%LOCALAPPDATA%\Ti60R5\logs`. Override with `--log-root`. CSV includes a UTF-8 BOM for Windows Excel; JSONL and metadata are UTF-8. Raw UART bytes are retained. Only complete same-sequence telemetry windows are exported as normalized measurements; unavailable hardware metrics remain empty.

To package, run `Build_Windows_Control_Panel.cmd` **on Windows**. It produces `dist/windows/Ti60Control.exe` plus a launcher. PyInstaller bundles the runtime and UI assets; no Python installation is required on the target PC. A manual Windows GitHub Actions template is included under `tools/control_panel/windows-control.yml`.

Validation performed here: six portability/protocol tests and a Chrome + real Linux PTY browser regression with 64 commands, including both output-mode buttons, F9, step arithmetic, held-key release, size controls, music, export, and responsive layout. The fixture explicitly labels its synthetic telemetry. Native Windows CMD/EXE, Windows USB drivers, and physical FPGA/HDMI behavior are **not yet tested**. Add those results after on-board validation; do not report fixture frame rates as hardware performance.
