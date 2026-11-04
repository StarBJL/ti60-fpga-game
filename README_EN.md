# Ti60 FPGA 2D Graphics Accelerator — RISC-V SoC

[中文](README.md) · [Efinity project](par/ddr_demo_ti60/ddr_demo_ti60.xml) · [Verification](docs/07_VERIFICATION.md)

This repository is a **technical RTL/SoC project**, with arcade-style gameplay as a reproducible accelerator workload rather than the primary subject.

- **Platform:** Efinix Titanium Ti60F225 (I3), Efinity 2026.1.132, Sapphire RV32IM at 225 MHz.
- **Video/audio:** RGB565 1280×720 scan-out at approximately 60.10 Hz and 48 kHz stereo PCM on one HDMI link.
- **Accelerator:** APB control and DDR command lists; ordered TAB/FTAB bulk submissions, color-key and Alpha blending, a 16 KiB source cache, 45 bands of 1280×16 pixels, and a peak of eight pixels per cycle in eligible fast paths.
- **Memory architecture:** Shared DDR3 with AXI arbitration; CPU execution, DMA rendering, audio, and scan-out compete for bandwidth. Four framebuffers and completion sequencing protect the displayed frame.
- **Host:** Optional local serial telemetry panel; board-side application operates without the host UI.

## Source navigation

| Area | Entry point |
|---|---|
| Integration | [RTL top](rtl/ddr3_example_top.v) |
| Graphics pipeline | [BitBlt engine](rtl/blit/blit_engine.v) · [wide AXI](rtl/blit/blit_axi_wide.v) · [read arbiter](rtl/blit/blit_axi_rd_arb.v) |
| HDMI & sound | [Framebuffer DMA](rtl/hdmi/hdmi_fb.v) · [HDMI packets](rtl/hdmi/hdmi_island.v) · [mixer](rtl/audio/audio_mixer.v) |
| Firmware | [frame loop](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/main.c) · [backend](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/gfx_hw.c) · [hardware ABI](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/blit.h) |
| Build | [Efinity project](par/ddr_demo_ti60/ddr_demo_ti60.xml) · [firmware build script](tools/build_verified.py) · [release hashes](CURRENT_RELEASE.json) |

## Technical documentation

1. [Architecture and clocks](docs/01_SYSTEM_ARCHITECTURE.md)
2. [BitBlt pipeline](docs/02_BITBLT_PIPELINE.md)
3. [AXI/DDR subsystem](docs/03_AXI_DDR_INTERFACE.md)
4. [HDMI and PCM audio](docs/04_HDMI_AUDIO.md)
5. [Firmware/RTL ABI](docs/05_FIRMWARE_ABI.md)
6. [Build and programming](docs/06_BUILD_AND_FLASH.md)
7. [Verification and limitations](docs/07_VERIFICATION.md)
8. [PPA and measured workloads](docs/08_PPA_RESULTS.md)

The detailed engineering notes currently use Chinese to match the project's submitted technical report.

## Evidence and reproducibility

The existing report describes **56,479/60,800 XLR cells**, **250/256 embedded RAM blocks**, **56/160 DSP units**, and **+0.063 ns core setup / +0.012 ns core hold** for its selected 225 MHz implementation. These are **historical report results**; no new synthesis, timing closure, or on-board test is claimed for this documentation change. Measured workload tables and their scope appear in [PPA results](docs/08_PPA_RESULTS.md). A display's 60 Hz scan cadence is not necessarily the rate of new frames.

Run `python3 tools/check_repo.py --hash` for a **static** project reference and artifact-integrity audit. Run `bash verification/axi_read_arb/run.sh` if Icarus Verilog is installed. The complete hardware flow requires the proprietary Efinity toolchain and associated SoC IP.

The Git repository currently retains matching BIT/ELF files and optional portable Windows runtime for compatibility. They can be moved to release assets only after the replacement distribution is verified. Third-party DDR/SoC and TMDS components are identified by their upstream headers and licensing terms; they are not represented as original accelerator RTL.
