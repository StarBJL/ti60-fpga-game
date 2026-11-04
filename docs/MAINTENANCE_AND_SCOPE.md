# 技术化重构的边界与清理记录

## 本次原则

- **权威代码来源：** 上传的“代码工程”中可交付的 FPGA、RISC-V、上位机组件；GitHub 当前源码经抽查与上传工程关键文件匹配。此次不替换/重写任何功能 RTL。
- **主页技术优先：** 用技术型根 README 索引 SoC、RTL、AXI/DDR、CDC、驱动 ABI、验证和 PPA；游戏仅用于功能与负载展示。
- **保留可重建性：** `rtl/memory_test/`、SoC IP、DDR3、BSP、Efinity XML/PERI/SDC 在缺乏实际综合等价证明前不删除。
- **旧版交付：** 对 BIT/ELF、便携 Python、素材源图，在正式 Release/安装包可替代后再迁出主仓库。不要把简单的 `.gitignore` 误以为能移除已追踪二进制。

## 当前清理范围

| 项目 | 处理 | 理由 |
|---|---|---|
| `README.txt` | 删除 | 与中英文 README 重复 |
| `docs/GITHUB_ABOUT.txt` | 删除 | GitHub About 文案不是工程配置，说明已入 README |
| `docs/EXPLANATION_GUIDE.md` | 移到 `docs/appendix/DEFENSE_GUIDE.md` | 答辩讲稿与主技术章节分离，保留历史内容 |
| `tools/control_panel/build_windows.py` | 修正生成包内部过期 R5 文案 | 不改编译、打包或串口功能 |
| `.gitignore` | 增加脚本实际使用的构建目录规则 | 防止产生未跟踪的缓存/发行目录 |

未清理素材和运行时，是因为改动它们会影响当前版本的载荷构建或便携安装。候选清单见先前审查和本文件下节。

## 不应贸然删除的候选

- `rtl/memory_test/`：看起来是早期 DDR 测试文件，但 Efinity XML **仍列为 design_file**。需先证实哪些模块未实例化，并做两次构建对照（包含资源/时序/位流行为）。
- `par/ddr_demo_ti60/src/Axi_Mux.v`、`Interrupt.v`：同理，仍在工程文件列表中。
- `rtl/common/Ddr_Read.v`、`Ddr_Write.v`、`rtl/define.v` 等：当前未发现它们是项目直接引用，但可能有 include/macro、IP 路径或调试用途；须完成全文静态依赖及 Efinity 检查。
- `assets/v4`、`v5`、`v6`：不是几个不同主工程，而是素材生成输入；修改 C 载荷重建可能依赖这些目录。
- `tools/windows_runtime/` 与 `BOARD_TEST/verified_225/`：体积大但当前启动入口和版本哈希引用它们；迁出后需要同步更新脚本/Release URL/操作文档。

## 后续工程化任务（本次没有宣称完成）

1. 基于固定 225 MHz 基线记录 Efinity Elaboration、PnR、WNS/TNS、资源与 CDC；
2. 为 BitBlt TAB/FTAB、带缓存、HDMI FIFO 和 Audio mixer 补 SystemVerilog/Verilog scoreboard；
3. 对每项真正删除的 RTL 路径建立源码前后/性能前后、可复现的等价结论；
4. 在固定 BIT/ELF 和场景参数下重跑高负载与稳定性测试；
5. 若对外发布，补准确的 LICENSE 和第三方 NOTICE，确认声音/美术素材的分发授权。
