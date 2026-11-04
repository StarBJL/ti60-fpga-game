# 06 — 构建、配对下载与恢复

**基线：** `CURRENT_RELEASE.json` 中 V6 / 225 MHz 交付。主工程入口：[ddr_demo_ti60.xml](../par/ddr_demo_ti60/ddr_demo_ti60.xml)。

## 依赖清单

| 组件 | 要求/说明 |
|---|---|
| Efinity | 工程 XML 记录 `2026.1.132.4.5`；目标 Ti60F225/I3 |
| SoC/DDR IP | 保留 `par/ddr_demo_ti60/ip/soc`、`rtl/ddr3_controller`、DDR PHY 和 BSP |
| RISC-V GCC | `riscv-none-elf-gcc` 与原裸机 BSP/链接脚本兼容的版本 |
| Python | 静态检查使用 Python 3.10+；素材与上位机按各自依赖安装 |
| Icarus Verilog（可选） | 独立 AXI 读仲裁 Testbench |

### 1. 检查仓库而非直接烧录

```bash
python3 tools/check_repo.py
python3 tools/check_repo.py --hash
```

第一个命令检查工程 XML 的设计文件及 IP/SDC/PERI 路径、关键源代码和文档链接；`--hash` 另校验交付 BIT/ELF/声音库以及软件源码摘要。二者**不能替代 Efinity 综合、STA、仿真或板级回归**。

### 2. Efinity 完整硬件工程

- 从仓库根目录打开 `par/ddr_demo_ti60/ddr_demo_ti60.xml`。
- 检查器件 Ti60F225、timing model I3、项目中各 Verilog 文件路径、SoC IP、PLL、DDR3 PHY、引脚 PERI 与 SDC。
- 在本地 Efinity 中执行 elaboration → synthesis → PnR → timing report → bitstream；与已交付 BIT 对比时，记录 Efinity build、seed、优化策略、约束文件哈希。
- 不要只因文件名为 `memory_test` 就删去项目 XML 内的模块。删除前应建立可重复的 PnR/上板对照。

### 3. 225 MHz 裸机固件

```bash
python3 tools/build_verified.py --toolchain /path/to/riscv/toolchain --profile 225
```

该脚本会构建两份优化 ELF（`STATS=1` 与 `STATS=0`），使用临时 BSP 副本修正时钟元数据，原始 BSP 保持只读。需要厂商工具链和全部声音/图片生成数据。修改源图、WAV 时先运行对应 `tools/build_*.py` 或 `wav2bank.py` 再重建 ELF。

### 4. 下载顺序

1. 读取 [版本清单](../CURRENT_RELEASE.json) 并校验 BIT 与两份 ELF 的 SHA-256。
2. JTAG 将对应 BIT 下载至 FPGA。
3. 使用 RISC-V IDE/OpenOCD 完整装载配套 ELF 至 DDR（含音频与图形可加载段），再启动 CPU。
4. 先检查颜色条/HDMI、串口输出、音频与轻载游戏；再逐步提高对象数量，记录 `present_fps`、欠流和温度/电源条件。
5. 如单独重载固件/声音，先确认 DDR 内容是否仍有效；掉电后应完整重新装载。

### 5. PC 上位机

- Linux：`bash Start_Control_Panel.sh`。
- Windows：`Start_Control_Panel.cmd`，当前交付包含便携 Python 运行时。
- 本地 HTTP 服务默认回环监听；串口只在用户主动连接后打开。
- 上位机属于诊断工具；**不属于 FPGA 独立运行的硬件依赖**。

## 可复现性限制

此次重构未实际调用 Efinity、RISC-V 编译器、JTAG 或示波/音频分析仪。旧技术报告中的性能数据可以引用，但必须写出版本、场景、负载和验证来源，不能改写为本次新实测数据。
