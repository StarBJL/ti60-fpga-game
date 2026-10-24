# 性能分析用离线计数

正式板上源码不作修改。`prepare_probe.py` 根据当前 game.c 生成独立计数副本，给 take_limit_shot 的调用与槽位扫描加计数；host_probe.c 固定场景后调用该副本和原 gfx/render 实现。这里没有 FPGA 时间模拟。

在项目根目录执行 PowerShell：

```powershell
python -X utf8 analysis/perf_probe/prepare_probe.py
gcc -std=c99 -O2 -Wall -Wextra -Ipar/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src analysis/perf_probe/host_probe.c analysis/perf_probe/game.instrumented.c par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/gfx.c par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/render.c par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/mini_snprintf.c -o analysis/perf_probe/host_probe.exe
./analysis/perf_probe/host_probe.exe
python -X utf8 -u analysis/perf_probe/rtl_probe.py
```

本次使用 E:/Anaconda3/python.exe 和 C:/msys64/ucrt64/bin/gcc.exe；编译无警告。`results.txt` 保存主机计数，`rtl_results.txt` 保存 RTL 子集模拟器输出。复现时从当前源码重新生成副本。

`rtl_probe.py` 使用已有 sim/blit/vsim.py 与 tb_blit.py 的模型，快速提供 24 条命令，停止模拟 HDMI 请求，理想 AXI 服务。逐字节比对 golden memory。输出周期包含短批量起始/收尾；不是实际 DDR 性能，也不是完整 SoC 验证。
