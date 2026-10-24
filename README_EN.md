# Ti60 R5: RISC-V + FPGA 2D Game Rendering Engine

**R5.3 firmware update:** [R2 comparison and changes](docs/R5_3_SETTINGS_MUSIC.md). Full ten-track selection, immutable hero-outline caching and invariant autoplay lookup. New ELF files are in `BOARD_TEST/r5_3`; hardware reuses the validated R5.2.1 bit. Actual throughput recovery requires board retesting. Complete bundle: `dist/Ti60_R5_3_MUSIC_PERF.tar.xz`.

**2026-10-24 R5.2 audio/video hardware baseline: matched bit / ELF in [BOARD_TEST/r5_2](BOARD_TEST/r5_2).** HDMI audio status/parity packing and transmitted VSYNC alignment are corrected. Render-path settings are visible in telemetry and can be restored explicitly. See [evidence and retest procedure](docs/BOARD_FIX_R5_2.md). Original R5 results and root-level binaries below remain a historical baseline. Current source console identifies as `R5.2-BOARD-FIX`; the bundled old Windows EXE does not include this patch.

[中文](README.md) · [Interactive input and wiring guide](docs/INPUT_GUIDE.html) · [Photo-oriented wiring diagram](docs/WIRING_DEMO.html) · [Contest requirements](docs/CONTEST_MAPPING_EN.md) · [Board validation worksheet](docs/BOARD_TEST_PLAN.csv)

This project addresses Efinix contest topic 2: **a 2D graphics rendering accelerator based on a heterogeneous RISC-V and FPGA architecture**. A Sapphire RISC-V CPU runs game logic. A custom BitBlt engine directly accesses DDR3 over AXI4 for block copy, solid fill, color keying, and Alpha blending. HDMI carries video and audio.

R5 extends the full R4/R3 project, retaining the R19-derived seven-level carrot character, dual guns, upgrades, active skill, and occlusion locator. Danmaku, moving squares, image switching, Chinese UI, and audio remain available. New work adds Windows control-console support, matrix/CH9350L input, physical 720p60 / 1080p30 switching, and on-chip-memory/read-credit-path optimization.

**Both output modes render at 1280×720. The 1920×1080 mode is 3:2 nearest-neighbor enlargement, not native 1080p rendering.** The current 74.375 MHz pixel PLL produces approximately 60.101 / 30.051 Hz. Firmware maintains an independent nominal 60 Hz gameplay schedule in 1080 mode. A host main-loop model verifies 30 Hz output with 60 Hz logic; actual load limits and monitor compatibility require board testing.

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

## Deliverables and quick start

Use the paired artifacts in `BOARD_TEST/`. [manifest.json](BOARD_TEST/manifest.json) identifies their versions, SHA-256 hashes, and verification state.

| File | Purpose |
|---|---|
| `BOARD_TEST/ti60_r5.bit` | R5 dual-video and input-device hardware |
| `BOARD_TEST/ti60_r5_debug.elf` | Detailed metrics, on-screen CPU/HW comparison, and diagnostics; recommended for contest validation |
| `BOARD_TEST/ti60_r5_release.elf` | Reduced statistics overhead |
| `BOARD_TEST/manifest.json` | Artifact hashes and build/verification evidence |
| `docs/INPUT_GUIDE.html` | Self-contained offline key layout, wiring, protocol, and shortcut guide |

Load the matching bitstream before the ELF. Older R4 bitstreams do not contain the new input and video hardware. Firmware checks `VID5` / `INP5` capability identifiers. Do not mix the R19 185 MHz / 640×480 bitstream with R5 firmware. Build scripts produce files and do not program the board automatically.

On Windows, unpack **`dist/Ti60_R5_Windows_Console.zip`** and run its **`Start_Ti60Control.cmd`** for the prebuilt standalone EXE; no separate Python installation is required. For source operation, double-click the project-root **`Start_Control_Panel.cmd`**. On Linux, run **`./Start_Control_Panel.sh`**. Source operation requires Python 3.10+ and pyserial; the launcher can create a local isolated environment. Select the board’s debug UART at 115200 baud and connect. Opening the browser alone does not open the serial port or send commands. The old `Open_Serial_Log.cmd` redirects to the new launcher.

COM ports, UTF-8/Excel-friendly logs, and Chinese/space-containing paths are supported. A [real Windows CI run](https://github.com/18971355314bjl-gif/ti60-fpga-game/actions/runs/39987343020) on Windows Server 2025 x64 / Python 3.12.10 passed native CMD startup, six portability tests, EXE packaging, and a packaged HTTP/UI-resource smoke test. The executable is `dist/windows/Ti60Control.exe`; evidence is at `verification/r5/windows_console/windows_ci_validation.json`. **The user’s Windows 10/11 USB serial drivers and physical FPGA/keyboard remain validation items.** To rebuild the EXE, run `Build_Windows_Control_Panel.cmd` on Windows. See [operation and packaging](docs/windows_zh-en.md).

## Controls and boundaries

| Function | Controls or range |
|---|---|
| Scenes | Danmaku, carrot survival, moving squares, image switching; GUI or `n` |
| Movement / shooting / skill | Browser or CH: WASD/arrows, Z/Space, X/J, Shift slow, Enter confirm, Esc pause |
| Count steps | **100 / 10 / 1** via F6/F7/F8; matrix S11 cycles |
| Count change | `+ / -` or buttons; target cap and actual active count are separate telemetry |
| Carrot hostile bullets | Custom stress cap **1–40,000**; sizes 9/13/17/25 px |
| Danmaku bullets | Stress cap **1–16,384**; sizes 75/100/125/150%; density is separate |
| Squares | **1–30,000**; mixed 6–40 px or fixed 6/8/12/16/24/32/40 px |
| HDMI modes | F9, matrix S15, or GUI buttons: 720p60 / 1080p30 |
| Rendering comparison | `g` CPU/HW; `e` Alpha; `v` VSync; debug `f` statistics and `c` automatic comparison |
| Music | Ten original chiptune loops and 23 retained sound effects; `;` BGM, backslash next track |

Allocated capacity is **not a guarantee of stable 60 FPS** at the maximum count. Stress bullet size changes visuals, while collision radius, damage, speed, and trajectories retain their existing behavior. Laser widths are unchanged. Covered area, Alpha, CPU logic, and DDR contention affect sustainable load.

Run the contest’s 60 FPS sprite-limit challenge in **720p mode with Alpha and Color Key enabled**. A physical 1080p30 output presents at most about 30 new frames per second; a 60 Hz logic counter cannot replace presentation FPS.

## Input hardware

The passive 4×4 matrix uses eight signals in **1.8 V Bank 1B**. Rows connect to J7 2/4/8/10, columns to 20/22/26/28. CH9350L uses a separate **3.3 V UART**: module TX→J7 32 and module RX←J7 34, with verified 5 V module power and common ground. J7 37/38/39/40 remain available as 3.3 V signal pins.

Numbers refer to the main board’s **J7**, not an assumed orientation of the green 2×20 breakout. Determine cable row/column order and adapter continuity before connection. The manual and schematic disagree at some J7 entries; the guide follows schematic net-to-ball tracing and names the Efinity GPIO resources.

Matrix S4 is Fn, S11 selects count step, S12 size, and S15 output mode. Configure CH9350L as USB Host/downstream, state 2, 115200 8N1, using fixed eight-byte HID boot-keyboard reports. State-0/1 variable frames and proprietary NKRO are not supported. Full mapping, ghost suppression, disconnect limitations, APB registers, and C APIs are in [INPUT_DEVICES.md](docs/INPUT_DEVICES.md) and the [interactive HTML](docs/INPUT_GUIDE.html).

## Implementation and contest coverage

CPU gameplay is in `game.c`, `th_game.c`, `th_pattern.c`, `th_stage2.c`, and `demo.c`. `blit_engine.v` handles COPY/FILL/Alpha/Key and command lists; `blit_axi_wide.v` provides a wide path for eligible operations. APB controls registers while AXI4 moves image data directly to/from DDR3. Bursts, queues, source caching, outstanding-request control, and band composition improve memory efficiency.

`blit.c` / `gfx_hw.c` expose the C hardware interface; `gfx.c` shares the drawing API with a CPU backend. `main.c` handles completed-frame publication and multiple framebuffers. `hdmi_fb.v` scans completed frames, generates modes, and records new/repeated presentations. `hdmi_island.v` supplies HDMI metadata/audio. The debug HUD keeps CPU/HW FPS and timing for the required on-screen comparison.

The full [English mapping](docs/CONTEST_MAPPING_EN.md) and [Chinese mapping](docs/CONTEST_MAPPING_ZH.md) match every base and advanced requirement to code and acceptance evidence. This topic’s minimum display requirement is **640×480 at 60 Hz**; 720/1080 switching is an extension.

## Repository layout and reproducible builds

```text
PROJECT/rtl/                         BitBlt, HDMI, audio, input, DDR RTL
PROJECT/par/ddr_demo_ti60/            Efinity project, constraints, IP, outflow
  embedded_sw/soc/software/standalone/game/
    src/                            RISC-V firmware
    sound/                          Sound bank and original music
    game.ld                         DDR linker regions
tools/control_panel/                Windows/Linux loopback serial console
BOARD_TEST/                         Paired bit/ELF artifacts and manifest
docs/                               Requirements, wiring, optimization, test worksheet
verification/r5/                    Current offline and target-build evidence
verification/r4/                    Historical R4 offline checks
logs/                               Historical board logs and later acquisitions
```

The hardware target is **Efinix Ti60F225**, built with **Efinity 2026.1**. Vivado 2021.2 is used only for portable behavioral RTL analysis/testing; it cannot generate a Ti60 bitstream or certify Ti60 resource/timing results.

```bash
python3 build_software.py --toolchain /path/to/efinity-riscv-ide/toolchain/bin
python3 build_hardware.py --efinity /path/to/efinity/2026.1 --flow compile
python3 tools/configure_launch.py
```

Software builds require GNU make and the Efinity RISC-V SDK. Both debug and release builds are generated by default. The launch configuration helper regenerates absolute IDE paths after moving the project or copying it to Windows.

Both modes use a 1,843,200-byte 1280×720 RGB565 framebuffer in 2 MiB-spaced DDR slots. Linker regions are 15 MiB program RAM, 16 MiB sound, and 32 MiB art. Fonts and music occupy **external DDR**, not those capacities in on-chip block RAM. Native 1080 RGB565 would require about 3.96 MiB per frame and cannot be inserted into the current slots without a memory-map redesign.

## Evidence and remaining board measurements

The [optimization report](docs/optimization_en.md) / [中文报告](docs/optimization_zh.md) covers queue RAM changes, the `rf_res` read-credit path, and the Alpha optimization roadmap. The final target results are above; evidence is in the [Efinity resource report](PROJECT/par/ddr_demo_ti60/outflow/ddr_demo_ti60.place.rpt) and [timing report](PROJECT/par/ddr_demo_ti60/outflow/ddr_demo_ti60.timing.rpt). Behavioral verification remains a separate evidence category.

R5 offline verification includes C input parsing under ASan/UBSan, matrix/UART RTL, dual-video behavior, 30 Hz output / 60 Hz logic host scheduling, native Windows CI startup/packaging/service tests, and a browser connected to a real Linux PTY. The HTML guide was checked in a real browser for offline loading, Fn, step/count controls, mode demo, filtering, light/dark themes, and desktop/390 px layout. Evidence resides in `verification/r5/`; consult its logs and the manifest for individual results.

Historical R2 board logs and R4 host tests remain baseline evidence, not new R5 scores. [BOARD_TEST_PLAN.csv](docs/BOARD_TEST_PLAN.csv) includes boot, display switching, physical input, Windows, gameplay, sizes, Alpha, CPU/HW comparison, maximum load, audio, endurance, and resource checks. Measured values and conclusions are intentionally blank for the user to complete. The console preserves raw UART, configuration events, and complete same-sequence telemetry windows.

Physical input also emits `I5` capability, held-state, keyboard-report, malformed-frame, rollover, ghost, status-report, and UART-error diagnostics. The CH9350L state response is implemented; real-module compatibility still requires board validation.

Record actual active count, render FPS, logic Hz, actual presentation FPS, scan/repeat rate, video/audio underflow deltas, worst work time, engine faults, temperature, and power. Scan rate is not presentation rate, and a requested cap is not the current active count.

## Provenance and licensing

`docs/references/` contains the contest, board, and CH9350 references with SHA-256 hashes. Third-party documents, Efinix IP/SDK, fonts, and inherited art retain their original terms. Noto licensing is at `ART/UI_R4/Noto_CJK_LICENSE.txt`. Original loops are generated by `tools/create_original_music.py`, with source WAVs and manifest under the firmware’s `sound/original_r4/` directory. This project is prepared for the user’s authorized private repository; it does not relicense all bundled third-party materials under a new blanket open-source license.


## Restore the complete GitHub snapshot

This repository is private. Source, IP, images, music, bitstreams, ELF files, the Windows EXE, tests, and logs are stored as ordinary exact files. Only three generated files exceed the accepted GitHub REST request size: the 55,888,638-byte mapped Verilog netlist and the two 43,673,856-byte `game.bin` images. Their complete bytes are preserved in lossless gzip parts under `.github_large_files/`; `tools/large_files_manifest.json` records original paths, byte counts, SHA256, Git blob SHA, and executable modes. Both roughly 29 MB `game.hex` files remain ordinary repository files.

After cloning or downloading, run from the repository root:

```sh
python3 tools/restore_large_files.py
python3 tools/restore_large_files.py --verify
```

On Windows, use `py -3` instead of `python3`. The script uses only the Python standard library. Correct existing files are skipped; locally modified files are protected unless `--force` is explicitly supplied. The complete local delivery already contains the original files and can be checked directly with `--verify`. The restored files return to their original paths and modes. See `verification/github_upload.txt` for the upload-method record: the rejected asset-import automation was not installed or run; the final method uses ordinary GitHub APIs plus explicit local restoration.
