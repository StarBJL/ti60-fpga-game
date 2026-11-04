# 07 — 验证计划、已提交用例与证据边界

**对应报告：** 第三部分设计成果和测试结果。区分“已有文档记录”和“本次自动化实际执行”。

## 本分支新增的可运行单元用例

[AXI 读仲裁 Testbench](../verification/axi_read_arb/tb_blit_axi_rd_arb.v) + [运行脚本](../verification/axi_read_arb/run.sh)。使用独立 Verilog 模块，不要求 SoC/DDR 厂商模型；检查：

1. 同时出现 m0/m1 AR 请求，扫描 m0 优先；
2. m1 已被选中但下游 ARREADY=0 时，m0 新请求不能抢走 m1 的未握手地址；
3. 两个主设备的 `RID[0]` 返回分流，以及 RREADY 始终为 1 的假设；
4. 复位之后授予状态清零。

运行（需预装 `iverilog` 与 `vvp`）：

```bash
bash verification/axi_read_arb/run.sh
```

这只是一个**模块级协议测试**，不宣称已验证整个图形引擎和 DDR 仲裁的所有背压/并发情况。

## 推荐验证矩阵

| 模块 | 正常场景 | 边界/异常 | 当前证据状态 |
|---|---|---|---|
| AXI 读仲裁 | 两主同时请求、RID 回程 | 地址背压与授予锁定 | 本分支提供独立 TB（需在 CI/本地实际运行） |
| BitBlt RGB565 | fill/copy/key/alpha | 裁剪、奇偶宽度、重叠、边界 | 有报告描述和软件/图形回归摘要；需补独立 scoreboard |
| TAB/FTAB | 顺序批量提交 | 跨屏裁剪、表生命周期、溢出 | 待补 RTL directed test |
| 分带 | 45 带合成写回 | 同物体跨两带、透明叠加、写响应 | 待补 reference renderer |
| 缓存 | 命中/缺失 | CPU 更新纹理后失效 | 待补定向测试 |
| HDMI | 720p 行场、FB flip | FIFO 欠流、帧边界交接 | 既有 `hdmi_sync_build.json` 描述选定仿真；不等价所有板卡实测 |
| PCM | 声部 keyon、格式/音量 | 混音饱和、FIFO 欠流、CDC | 需补采样比对与板级音频验证 |
| SoC | 固件/图形/音视频并行 | DDR 争用、复位/时钟失锁 | 需 Efinity 和实板验证 |

## 证据分级

- **L0：静态审查**：XML 路径、脚本语法、SHA256、寄存器文档；不能证明 RTL 时序或功能。
- **L1：模块仿真**：有 Testbench、期望结果及 PASS 日志；仅覆盖模拟条件。
- **L2：软件/系统仿真**：有环境、输入、数据比对和可追溯产物；不保证真实 DDR/HDMI 时序。
- **L3：布局布线/STA**：目标器件、seed、约束、setup/hold、利用率；不能独立证明画面或音频无误。
- **L4：实板复现**：BIT/ELF 哈希、温度/电源、输入/场景、日志、帧率/欠流、视频和音频记录。

当前 [docs/evidence](evidence/README.md) 是仓库中**既有记录的摘要**。它们明确注明了 `documentation_only` 或实板音频尚待确认；本次未重跑完整 RTL 综合或实际烧录。发布对外指标前应逐项标注证据等级。

## 最低验收标准

- `tools/check_repo.py --hash` 返回 0；
- AXI TB 在装有 Icarus 的环境打印 `PASS`，CI 收集日志；
- 不修改现有功能 RTL、SDC、BSP 以及 BIT/ELF；若修改则补 PnR 与相同负载的板级回归；
- 性能表与当前技术报告数值一致，并明确测量环境/是否新测；
- README 所有相对路径存在且能打开；第三方来源说明保留。

仓库现阶段**不宣称**已经完成全部上述验收项；凡未运行的检查在 PR 中记录为待执行。
