# Contest topic 2: requirements, implementation, and evidence

This project targets **“2D Graphics Rendering Accelerator Based on a Heterogeneous RISC-V and FPGA Architecture.”** Primary references are the [Efinix 2026 topic guide, PDF pages 7–9](references/contest_efinix_2026.pdf) and the [August 22 corrected guide, PDF pages 14–17](references/contest_2026_aug22_corrected.pdf). The corrected version explicitly specifies AXI4, DDR3, and HDMI. The minimum display requirement for **this topic** is **640×480 at 60 Hz**. The 720p60 / 1080p30 mode switch is an additional project feature; it must not be confused with the separate video-stylization topic.

Source implementation, offline verification, target-device implementation, and board validation are different evidence levels. The current deliverables are identified in `BOARD_TEST/manifest.json`; new tests live in `verification/r5/`. The user will add actual board results to [BOARD_TEST_PLAN.csv](BOARD_TEST_PLAN.csv). Old bitstreams and host models are not evidence of R5 board performance.

## Requirement mapping

| Requirement | Implementation and source | Acceptance evidence to collect |
|---|---|---|
| RISC-V runs application logic | Sapphire RV32IM; `main.c`, carrot gameplay in `game.c`, danmaku in `th_game.c` / `th_pattern.c` / `th_stage2.c`, square and image scenes in `demo.c` | Boot identifier, four interactive scenes, independent logic update rate |
| FPGA decodes operation, source, destination, width, and height | APB command registers and DDR command-list parser in `blit_engine.v`; C definitions in `blit.h` / `blit.c` | Register-level command tests and software-reference pixel comparisons |
| Block Copy | General RGB565 copy path, plus a 128-bit AXI wide path for eligible opaque blocks in `blit_axi_wide.v` | Equal output for CPU/HW; clipping, odd widths, alignment, and keyed copies |
| Solid Fill | Color/rectangle commands; ordinary, wide, and on-chip band-window paths | Full-screen and small fills, boundary guards, even/odd sizes |
| HDMI framebuffer output | `hdmi_fb.v` DMA, timing and buffer exchange; `hdmi_island.v` AVI/audio data islands | Monitor mode detection, visible boundaries/color bars, measured scan rate and underflow deltas |
| Accelerator accesses memory directly via the system bus | APB is the control plane; AXI4 masters move pixels and commands directly to/from DDR3 | AXI activity and counters; CPU-only versus hardware backend comparison |
| Bursts and FIFO buffering | Burst splitting, response ring, command/write queues, source cache, outstanding-read limits, wide data path, and arbitration | AR/AW/W stalls, pixel waits, cache hits/misses, busy cycles, outstanding-transaction invariants |
| C register driver | `BLIT_BASE=0xF8101000`; `blit.c`, `gfx_hw.c`, and shared `gfx.c` API | BLT1 identity/capability detection and functioning CPU fallback |
| Double buffering avoids tearing | Separate display and render buffers, completed-frame publication at safe boundaries; pipelining may occupy more than two buffers | Moving-edge/frame-counter patterns and frame exchange/repeat counters |
| Square/image demos and on-screen CPU/HW FPS comparison | Random moving squares and image switching; `g` switches backend, debug `c` alternates every four measurement windows; HUD keeps CPU/HW FPS and time | Use debug ELF with statistics visible; compare identical configurations on the screen, not only in a serial log |
| Hardware Alpha blending | Background, RGB565 source, optional A8 plane, and hardware multiplication in `blit_engine.v` | Reference checks at transparent/midpoint/opaque values and combined global/per-pixel Alpha; DSP mapping and target timing |
| Color Keying | Configurable transparent-color register, default RGB565 magenta `0xF81F`; matching pixels preserve destination | Irregular sprites over a checkerboard; verify combined keying and Alpha |
| High-load interactive game | Seven carrot evolution levels, dual guns, upgrades, active skill and occlusion locator; danmaku shooting, slow mode, bomb, stages/Boss and stress mode | Live input, upgrades, skills, restart, visible movement; not a prerecorded animation |
| Maximum active sprites at stable 60 FPS with Alpha and Keying enabled | Count steps 100/10/1, size presets, Alpha/VSync/backend switches, shared telemetry windows | Measure in **720p60** mode. Physical 1080p30 output cannot present 60 different frames per second |

RTL paths are under `PROJECT/rtl/`. Firmware paths in this document are relative to `PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/`. Legacy comments can contain older geometry values; runtime capability registers, active top-level parameters, and current code determine actual behavior.

## Rendering pipeline and numerical conventions

Input adapters emit held states and one-shot semantic actions. The CPU performs gameplay, collision, upgrades, and draw-table preparation. Sprites, music, framebuffers, and large tables live in DDR3. Their linker memory usage is separate from FPGA block RAM usage.

The hardware engine fetches commands and images, applies copy/fill/key/Alpha operations, and writes completed pixels. Band rendering keeps a small group of scanlines in an on-chip window while ordered layers are composed, reducing repeated DDR background read-modify-write operations. The RGB565 channel blending convention is `(src*a + dst*(256-a) + 128) >> 8`, with an opaque special case for complete copies; optional A8 and global Alpha are combined in hardware. The project also supports additive effects.

The display scans a completed framebuffer. In R5, both physical output modes use a **1280×720 rendering canvas**. The 1920×1080 mode uses **3:2 nearest-neighbor enlargement**. This is a real HDMI timing/AVI mode change, not native 1080p rendering. The existing 74.375 MHz pixel PLL yields approximately 60.101 Hz and 30.051 Hz; the nominal mode names remain 720p60 and 1080p30. Exact monitor compatibility still requires board testing.

Audio uses the existing DDR-based PCM mixer. New scene music does not duplicate the FPGA mixer or add hardware voices.

## Capacity is not a stable frame-rate claim

| Boundary | Capacity | Interpretation |
|---|---:|---|
| Carrot custom hostile bullet pool | 40,000 | Allocated capacity; no claim of stable 60 FPS at this count |
| Danmaku stress bullet pool | 16,384 | Active count still depends on emission pattern and density |
| Square object pool | 30,000 | Large translucent squares can be limited by covered pixel area |
| Logical render canvas | 1280×720 RGB565 | 1080p output does not add independent rendering detail |
| Sound region | 16 MiB | R4 bank is 10,123,520 B with 10 original tracks and 23 retained effects |
| Historical band window | 16 rows × 1280 px; 40 RAM blocks | New target synthesis must report its own actual mapping |

Object count, covered area, Alpha ratio, ordering, CPU logic, and DDR contention jointly determine the stable limit. Doubling a square's side length quadruples its ideal area. Stress bullet size changes presentation only; collision radius, damage, speed, and trajectories retain their original gameplay behavior. Lasers retain their existing widths.

The historical R2 board log is `logs/20261023/uart_20261023_204925_104_COM5_c91971_720P-R2.txt`. Short historical windows showed roughly 60.3 render FPS at 16K carrot hostile bullets, around 20 FPS at 40K; approximately 60 FPS for 8.8–8.9K translucent squares, and around 20 FPS at 20K squares. These windows are **not R5 results**, and their configurations are not all identical. They motivate controlled retesting rather than a new benchmark claim.

The inherited R2/R4 hardware report used 55,848 / 60,800 XLR, 253 / 256 RAM blocks, and 32 / 160 DSP blocks, with +0.053 ns setup and +0.025 ns hold slack at 200 MHz. R5 has fresh passing **Efinity** implementation reports; final comparisons are listed below. Vivado simulation can validate portable RTL behavior but cannot establish Ti60 placement, RAM packing, or timing closure.

R4 host instrumentation of the real font path showed that a warmed title screen reduced per-frame font publication from 59–60 operations to zero, cache resets from 1–2 to zero, and copy calls from 109 to 63. This demonstrates eliminated work, not a measured board FPS improvement.

## Final Ti60 target implementation

Efinity 2026.1 completed the final map, interface, placement/routing, and bitstream generation. All constrained setup/hold relationships in the final report are positive. The core remains at **200 MHz**.

| Metric | Historical R2 / R4 | Final R5 | Change |
|---|---:|---:|---:|
| XLR | 55,848 / 60,800 | **58,518 / 60,800 (96.25%)** | +2,670 |
| On-chip RAM blocks | 253 / 256 | **253 / 256 (98.83%)** | Unchanged total; 3 free |
| DSP | 32 / 160 | **32 / 160** | Unchanged |
| Core 200 MHz setup slack | +0.053 ns | **+0.118 ns** | +0.065 ns |
| Core 200 MHz hold slack | +0.025 ns | **+0.028 ns** | +0.003 ns |

The two shallow blitter queues free **four RAM blocks**. The new scanout line cache uses three, and the CH9350 UART RX FIFO uses one: `253 − 4 + 3 + 1 = 253`. This makes room for the new functions; it is not a four-block reduction in total device RAM use. General Alpha remains two pixels per cycle; a four/eight-pixel Alpha path is a future optimization, not a delivered claim.

The final `ti60_r5.bit` is **2,811,948 bytes**, SHA-256 `25dc85470b3f73f929ef75c60eb06880a7c46533f30c26820a40eb5def85ac83`. This establishes passing constrained target implementation, not measured board sprite limits, power, temperature, or peripheral compatibility. Board acceptance results remain for the user to fill in.

## Repeatable maximum-load measurement

1. Record bit/ELF SHA-256, scene, deterministic seed, size, Alpha, keying, VSync, CPU/HW backend, motion backend, table mode, and all relevant knobs. Measure debug and release builds separately.
2. Warm up for at least 10 seconds. Collect at least 60 seconds per point and a separate 10-minute endurance run. Increase by 100, refine by 10, then by 1 near the limit.
3. Capture render FPS, logic Hz, actual new-frame presentation FPS, scan rate, repeated-frame rate, video/audio underflow deltas, maximum work time, and engine faults. A constant 60 Hz scan alone is not 60 FPS.
4. Define a stable acceptance rule before the run: presentation tracks the scan rate, repeats do not grow continuously, logic rate meets the scenario requirement, underflow deltas are zero, and no pool/list/engine faults occur. Publish measurement windows and tolerances.
5. Re-test the last passing configuration three times. Preserve raw serial logs, configuration events, CSV, video, and display mode information. For bullet modes use the actual active count; a raised cap does not instantly spawn that many bullets.

No new FPS, latency, power, or temperature values are prefilled. The result columns in `BOARD_TEST_PLAN.csv` are intentionally blank for subsequent board validation.
