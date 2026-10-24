R5.3 firmware + source console. Compatible with the unchanged R5.2.1 FPGA bit.
Load ti60_r5_3_debug.elf for measurement; release ELF has reduced statistics.
If video_rev is already 328193, no FPGA reprogramming is needed for this update.
Otherwise load compatible_ti60_r5_2.bit first. Hardware hash is in manifest.json.
Restart the source console: footer R5.3-MUSIC-CONTROL.
Music: select all 10 songs, previous/next wrap, or automatic scene mode.
Performance: restore acceleration, then hold workload/alpha/size constant.
R2 reference: ~15.9K carrot bullets near 60 FPS; ~8.9K alpha mixed-size squares near 60 FPS.
The new outline cache is functionally verified; actual FPS recovery is not yet measured.

中文说明见 docs/R5_3_SETTINGS_MUSIC.md。先运行新 ELF、重启上位机。已加载 R5.2.1 bit 时无需重烧硬件。
Linux BSP 仍在原工程 PROJECT/par/ddr_demo_ti60/embedded_sw/soc/bsp。
