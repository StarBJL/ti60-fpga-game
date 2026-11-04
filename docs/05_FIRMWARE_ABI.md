# 05 — 固件与 RTL 的 APB / 共享内存契约

**报告对应章节：** 2.2.3.3～2.3.1。**权威源码：** [blit.h](../par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/blit.h)、[hdmi_fb.h](../par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/hdmi_fb.h)、[audio.h](../par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/audio.h)、[gfx.h](../par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/gfx.h)。

这里是接口**导航与生命周期规范**，寄存器位定义应以当前版本对应 `.h` 与 RTL 的 decode 为准；尤其早期源码头部的旧时钟注释和历史 CAPS 字段不能作为最新规格。

## APB 地址分配

| 基址 | 外设 | 主体 |
|---|---|---|
| `0xF8100000` | HDMI framebuffer | framebuffer、状态、音频 FIFO |
| `0xF8100080` | 输入 hub | 矩阵扫描与外接 UART |
| `0xF8101000` | BitBlt | 命令、列表、缓存/分带、完成和性能 |
| `0xF8102000` | PCM mixer | 音色声部、地址、音量、循环和计数器 |

### BitBlt 基础寄存器（相对基址）

| 偏移 | 名称 | 用途 |
|---|---|---|
| `0x00` | `BLIT_R_ID` | 固定识别值 BLT1 |
| `0x04` | `BLIT_R_STATUS` | BUSY/FULL/ERR/LIST 等 |
| `0x08` | `BLIT_R_CTRL` | 清性能统计与错误 |
| `0x0C` | `BLIT_R_CAPS` | 设备能力/深度标志 |
| `0x10` / `0x14` | `DST` / `DSTR` | 目标字节地址及行跨度 |
| `0x18` / `0x1C` | `SRC` / `SSTR` | 源字节地址及行跨度 |
| `0x20` / `0x24` | `AADDR` / `ASTR` | A8 掩码地址和跨度 |
| `0x28` | `SIZE` | W/H 像素尺寸 |
| `0x2C` | `COLOR` | RGB565 颜色 |

上述偏移摘自固件头文件；LIST/TAB/FTAB/BANDS 和性能计数等扩展寄存器，请按 [完整 blit.h](../par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/blit.h) 查阅，避免复制与源码可能漂移的长枚举。

### HDMI 常用寄存器（相对 `0xF8100000`）

| 偏移 | 名称 | 语义 |
|---|---|---|
| `0x00` | CTRL | 扫描使能/测试模式 |
| `0x04` | FB_ADDR | 写请求下帧基址 |
| `0x08` | FB_CUR | 当前扫描基址 |
| `0x0C` | STATUS | 翻页等待、vblank、欠流 |
| `0x10` | FRAME_CNT | 帧扫描计数 |
| `0x18` | UF_CNT | 视频 FIFO 欠流计数 |
| `0x20` | AUD_CTRL | HDMI/音频/测试音配置 |

### 两种硬件表项

```c
typedef struct { int16_t x, y; int32_t sprite; } GfxItem; // 8 B; 位置+精灵槽
typedef struct { uint32_t xyw, cha; } GfxFillItem;        // 8 B; packed 矩形
```

`GfxItem` 是实际绘制表项；报告中的“16 字节弹幕热数据”指游戏状态压缩布局，**不要误当作 TAB 表项长度**。表项字段对齐、大小端、打包/解包宏以 [gfx.h](../par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/gfx.h) 为准。

## CPU/FPGA 同步协议

1. CPU 构建命令表及必要素材，执行访存 fence，保证 DMA 可见。
2. CPU 通过 APB 发布 LIST 或 BAND 命令，随后可计算下一帧，但**不能提前重写硬件仍在读取的对象表**。
3. 图形引擎完成最后一带的 DDR 写回确认后，推进完成序号。
4. 固件检查完成序号及 framebuffer 所有权，才写 HDMI `FB_ADDR` 请求翻页。
5. HDMI 在垂直同步边界将 `FB_ADDR` 锁存到 `FB_CUR`；显示扫描未结束前不得复用该缓冲。
6. CPU 读取硬件写回的数据前，应遵循 `blit_wait` 和 D-cache 失效的驱动约定。

**禁止的优化误区：** 将命令写入 DDR 误作已完成渲染；将 `STATUS.BUSY=0` 误作 HDMI 已完成翻页；未 fence 就发布表指针；未等待硬件 DMA 就复用栈/临时数组。
