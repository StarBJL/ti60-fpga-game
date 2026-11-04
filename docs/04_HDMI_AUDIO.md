# 04 — HDMI 视频扫描、音频混音与 CDC

**对应报告：** 2.2.3.6、2.2.3.7。**源码：** [hdmi_fb.v](../rtl/hdmi/hdmi_fb.v)、[hdmi_island.v](../rtl/hdmi/hdmi_island.v)、[audio_mixer.v](../rtl/audio/audio_mixer.v)、[input_hub.v](../rtl/input/input_hub.v)。

## 视频通路

DDR RGB565 framebuffer → AXI 扫描 DMA → 异步像素 FIFO → RGB565→RGB888 → 720p 行场定时 → TMDS 编码及串行输出。目标时序：H = 1280+110+40+220=1650；V = 720+5+5+20=750；像素时钟 74.375 MHz。

`hdmi_fb.v` 暴露 `FB_ADDR`（请求下一帧）和 `FB_CUR`（当前扫描），在**垂直同步脉冲结束的帧边界**锁存翻页并启动下一帧预取。CPU 不得把“请求翻页”误认为“已经被扫描”。

HDMI 扫描 FIFO 与图形引擎帧写回必须解耦；渲染超时时屏幕可能重复上帧，但扫描时钟仍持续。显示稳定性需结合扫描欠流 `UF_CNT` 与实际 `present_fps` 判断。

## HDMI 音频通路

声部 KEYON → DDR PCM 读取与小缓存 → 插值/独立 L/R 增益 → 16 声部累加/饱和 → 音频异步 FIFO → HDMI 音频采样包/ACR → TMDS 输出。

| 参数 | 既有设计 |
|---|---|
| 声部 | 16 |
| 输入格式 | s8 mono、s16 mono、s16 stereo |
| 混音 | 线性插值、双声道独立音量、饱和限幅 |
| 输出 | 48,000 stereo sample frames/s |
| PCM FIFO 目标水位 | 64 帧，约 1.33 ms（不是系统总音频延迟） |
| ACR | N = 6144、CTS = 74375（该 74.375 MHz 像素时钟配置） |

`fs=74,375,000×6144/(128×74375)=48,000 Hz`。音频节拍来自 HDMI 域而不是软件渲染循环；软件慢不会直接改变 48 kHz 目标采样率，但音频样本供给不足可能出现欠流。

## CDC、复位与稳定输出检查

- 像素 FIFO：系统/AXI 域写入，pixel 域消费。
- 音频 FIFO：core 域写入 stereo PCM，pixel 域取样。
- 翻页请求与完成状态：检查同步、在帧边界生效和复位默认帧。
- HDMI 编码器复位：检查锁相环 lock 后的分域释放、复位跨域安全。
- `video underflow`、`audio underflow`、`mix CLIPS`：分别记录，不把没有听见噪声当作检测完成。

相关的 DDR PHY、Sapphire SoC、部分 TMDS 编码来自厂商或 Xilinx 示例（原源码保留归属说明）；在技术展示中应区分集成、修改与自行设计的部分。
