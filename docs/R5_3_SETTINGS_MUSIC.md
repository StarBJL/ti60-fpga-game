# R5.3：R2 性能对照、全曲库切换和软件优化

本次以用户指定的 `Ti60_720p_MERGE_R2_ZH_FULL` 为旧版基准。证据来自 R2 的真实串口日志 `logs/20261023/uart_20261023_204925_104_COM5_c91971_720P-R2.txt`，以及新上传的 `uart_141137_380372_R5-CONSOLE.csv`（236 个窗口）和同会话原始输出。解析结果和文件哈希见 `verification/r5_3/performance_comparison.json`。没有将伪串口测试帧率当成实板结果。

## 当前哪些设置影响性能

新 CSV 最后一段为 `submit_mode=3, band=0, motion=0, table=1, cache=1`：APB 列表字提交、分带未生效、方块运动由 CPU 计算。`table=1` 只是紧凑表开关；紧凑表实际生效还要求 DDR 硬件列表，即 `submit_mode=2`。分带也要求该提交模式，因此在模式 3 下反复点分带，可能出现 `band_want=1` 而 `band=0`。

点击“恢复全部加速配置”应得到：

| 项目 | 对照设置 |
|---|---|
| 提交路径 | DDR 硬件列表，`submit_mode=2` |
| 分带 / 源缓存 / K 通路 | `band=1, cache=1, kpath=1` |
| 方块硬件运动 / 紧凑表 | `motion=1, table=1` |
| 弹幕表 / 逻辑建表 / 排序 / 重叠 | 开启 |
| LIMIT | `16 / 128 / 64`，保持默认 |
| 输出 | 720P60，游戏画面（彩条关） |
| 自动演示 | 对照 R2 手动压力记录时关闭 |
| Alpha、尺寸、数量 | 必须分别固定记录；恢复按钮不改变这些负载条件 |

方块场景的 `overlap_active=0` 属于现有设计：运动更新与绘制共用对象表，必须避免同时读写。R2 同样采用这一顺序；反复点 q 不会让方块获得游戏场景的重叠流水。

## R2 的实际记录与本次数据

- R2 萝卜：约 15,900 个在场弹体、13px、Alpha 开、手动、硬件分带与重叠、自由运行时接近 60 渲染 FPS。不能把对象池最大 40,000 当作稳定 60 FPS 上限。
- R2 方块：8,900 个原混合尺寸、Alpha 开、硬件运动、硬件分带、自由运行，90 条记录平均约 59.71 渲染 FPS；8,800 个约 60.5 FPS。
- 新日志中，14,000 个透明混合尺寸方块、加速项全开时约 40.7 FPS，工作时间约 24.47 ms；12,000 个约 46.6 FPS，约 21.41 ms。这两组超过了旧记录接近 60 FPS 的数量，不能直接量化相同数量的回退。
- 新日志末段 13,000 个方块在模式 3、分带关闭、CPU 运动条件下约 19.2 FPS，CPU 提交约 40.39 ms。恢复正确提交路径是第一步，但不保证这个数量与 Alpha 负载能达到 60 FPS。
- 萝卜 14,000 目标在手动段（seq 20–31）逻辑均值约 12.22 ms，CPU 提交约 6.03 ms；开启自动演示后（seq 36–68）逻辑均值约 23.63 ms。自动避弹本身也消耗 CPU。同步开启时，一帧超过约 16.64 ms 后可能错过一个扫描周期，出现约 30 FPS；关闭同步用于测自由吞吐，不会消除计算开销。

## 本次代码改动

1. “下一首”从当前场景两首备选，改为全部 10 首循环；新增上一首、下拉指定曲目和“跟随场景”。手动选择跨场景保留；原自动场景默认曲目保持。BGM 开关和测试音开关独立。
2. R19 人物遮挡轮廓改成 7 个外观 ×（32 个朝向 + 待机）的不可变缓存。首次使用一个姿态时生成，后续直接复用，避免反复写入轮廓像素及发布新源图触发的缓存刷新；原遮挡像素判定、80/65 滞回、淡入淡出和轮廓外观保持。
3. 自动演示把弹池基地址查找移到循环外，不再每个敌弹重复调用容量/模式判断；避弹算法和产生的按键结果保持。
4. 上位机显示提交模式的实际名称，说明分带的前提，并在高级诊断区直接提供恢复按钮。CSV 补齐自动演示、统计、详细报告及音乐选择状态，便于同条件对照。

缓存消耗外部 DDR 程序区约 2.02 MiB 额外空间；调试 ELF 总用量为 14,026,208 B / 15 MiB（89.18%），发行版为 13,954,080 B（88.72%）。没有更改 FPGA RTL、片上 RAM、时钟、队列深度或 Alpha 精度。继续配套 R5.2.1 bit，SHA256 `8d2d7b1c7acb884b79b9728088a72f06f7e05a93f67a4b29001f36b3c9a7caea`，硬件 `video_rev=328193`。

## 已完成验证与未完成的实板指标

- 10 首全部可达、循环/反向/指定曲目、手动跨场景保持、自动场景恢复及静音恢复测试通过。
- 896 个外观/朝向/摆动/压力组合通过独立像素覆盖参考；近邻缓存、溢出和弹池回退等测试通过。
- 本次重新生成的 19 张游戏/菜单/压力画面与 R5.2 逐字节相同，4 种硬件提交模式画面一致。
- 3,200 次已预热姿态变化不再重建/发布轮廓；旧姿态像素内容和地址保持，不影响在途帧。
- 自动演示优化在 64 组位置/时刻上与旧算法输出按键完全相同。
- 真实 C 主循环、游戏玩法、控制解析与音频回归通过；调试和发行 ELF 均完成交叉编译。
- 浏览器和真实 PTY 完整交互回归通过，包含 10 首选曲、音量、数量、尺寸、分辨率及 CSV 导出。测试数据明确标为合成数据。

本次没有自动烧写或更改正在运行的开发板。缓存优化消除了已确认的重复工作，但没有据此编造新的实板 FPS 或稳定数量。请先比较 720P、Alpha 开、混合方块 8,900 个，再比较萝卜 13px、自动演示关、15,000/16,000 目标。测自由吞吐时关闭垂直同步；确认稳定 60 FPS 时再开启，并查看实际呈现和重复帧。

## 使用

加载 `BOARD_TEST/r5_3/ti60_r5_3_debug.elf`，现有 R5.2.1 bit 可继续使用；工程 `game/build/game.elf` 同步为新调试版本。重启源码上位机并刷新页面，页脚应为 `R5.3-MUSIC-CONTROL`。音乐区可直接选择 10 首，或选“跟随场景”。BSP 仍是原 R5 工程的 `PROJECT/par/ddr_demo_ti60/embedded_sw/soc/bsp`。

完整离线包提供 ZIP 和 TAR.XZ，包含配套 bit、两种 ELF、清单和上位机源码。GitHub 完整交付使用 TAR.XZ，解压后文件齐全；源码启动入口支持 Linux/Windows。旧预编译 EXE 不自动更新，最新 Windows 构建从对应 GitHub Actions 产物下载。

## English summary

The old R2 hardware log is the reference: roughly 15.9K carrot bullets approach 60 rendering FPS, while 8.9K alpha-blended mixed-size squares average 59.71 FPS. New measurements contain both different workloads and disabled acceleration paths, so no universal regression percentage is inferred.

R5.3 provides a ten-track manual playlist and automatic scene mode. It also caches immutable hero pose outlines in external DDR and hoists invariant hostile-pool lookup from the autoplay loop. Independent visual and gameplay checks pass; 19 regenerated frames exactly match R5.2. Hardware is unchanged and reuses the verified R5.2.1 bitstream. Actual throughput recovery remains a board-retest item.
