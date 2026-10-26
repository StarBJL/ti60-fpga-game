# Ti60 FPGA Games and Graphics Accelerator — R6

R6 selectively integrates the scheduling, data-layout, Alpha, FTAB and Touhou improvements from `09_Ti60F225_hardjtag_demo` into the existing R5.9 project. Both games retain three-player local co-op, calibrated matrix/USB inputs, 99,999-object stress capacity, size presets, Chinese UI, and ten original music tracks.

The only modified hardware source is `PROJECT/rtl/blit/blit_engine.v`. HDMI RTL, the real PLL settings, pin assignments and timing constraints are unchanged. The core runs at **200 MHz**. This release does not claim to resolve PANDA PH27QB2 compatibility with the existing 1080p30 mode. Video recovery simulations are not monitor validation.

## Run and program

- **Windows:** extract `Ti60_R6_Windows.zip` and double-click `Ti60Control.exe`. Python and pyserial are bundled. A browser opens on an available loopback port; no serial device opens until Connect is clicked. Close the console window or press Ctrl+C to stop it.
- **Linux:** run `bash Start_Control_Panel.sh` from the project root. `--port 0` selects a free port; `--port 8765` requests a fixed port.
- **Matched hardware:** `BOARD_TEST/r6/ti60_r6.bit`.
- **Measured firmware:** `BOARD_TEST/r6/ti60_r6_debug.elf`; use `ti60_r6_release.elf` for lower-overhead presentation. Keep benchmark results from the two builds separate.
- Import the existing Eclipse project under `PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game`. Its project name is **game_r6**, and its normal build/download target is `build/game.elf`. Relative links avoid accidentally compiling the previous project.
- BSP root: `PROJECT/par/ddr_demo_ti60/embedded_sw/soc`; board support files are in `bsp/efinix/EfxSapphireSoc` beneath it.
- `python3 tools/configure_launch.py` generates a launch configuration for the prebuilt debug ELF after moving the project. It never programs hardware.
- The UART boot banner contains `TI60-R6-PIPE-KBLEND-FTAB-20261026`. `kblend=1 ftab=1` confirms the new hardware capabilities. SHA-256 values are recorded in the release manifest.

## Implementation

| Change | Implementation and limits |
|---|---|
| Frame scheduling | Four buffers exclude the scanned, requested and in-flight GPU frame. Waiting for the previous flip is deferred until the next presentation. Complete, unsplit band frames may overlap the following CPU frame with VSync enabled. At most one flip is requested per loop with VSync. This can add one frame of latency. |
| Carrot hot records | Hostile `Seed` records shrink from 24 to 16 bytes. `active` stores damage; zero marks a free slot. Player ownership, NOVA, upgrades and independent co-op collisions remain. Stress mode retains its disabled player auto-shots and disabled locator/proximity/fade/overlay work. |
| Touhou hot/cold split | A 16-byte hot array carries current position, velocity and draw indices; the 32-byte cold array retains pattern parameters. New bullets are synchronized after emitters finish setting parameters. Removal, clearing and resize rebuilding use current hot coordinates. Independent graze flags are preserved. |
| XY publishing | One aligned, alias-safe RV32 word store replaces the four byte stores emitted for the former memcpy helper. |
| Global Alpha | The eight-pixel K path handles banded global-Alpha fills, cached copies and additive blending. Spare DSPs compute both weighted terms, preserving exact rounding. Per-pixel A8 remains on the existing narrow path. K1–K4 dependencies remain checked. |
| Generic FTAB | **Opcode 7**, CAPS2 bit5, eight-byte entries carrying XY, dimensions, color and Alpha. Existing **SQTAB opcode 6**, CAPS2 bit3 and MOT2 remain available. Tables larger than 65,535 entries are split into multiple commands. Capacity overflow ends band mode and continues in original drawing order. |
| Square A/B paths | Original Q4 motion, eight colors and alternating Alpha are retained. CPU+FTAB uses three immutable snapshots. CPU+SQTAB, MOT2+SQTAB and individual fills remain selectable. Exceptional metadata rebuilds drain the engine before ring reuse. |
| Touhou effects | Stage 1 adds three birth-Alpha levels, budgeted A8 halos and emitter blooms. Stage 2 batches comet heads/tails; stress comets scale with density while reserving beam slots. Both stress stages have separate entry buttons. Both normal stages retain co-op. |
| Sound bank | All ten original BGM loops and 23 sound effects are retained. BGM uses 22,050 Hz 16-bit mono PCM; the existing HDMI output format is unchanged. Bank size decreases from 10,123,520 to 7,124,736 bytes. Loop points and durations are checked. |

A 144-byte pointer scratch area uses dead boot OCR space at `0xF9000C00`. The linker reserves 512 bytes explicitly, and the game stack remains in DDR. This does not allocate another block RAM.

## Routed result

| Item | Final R6 |
|---|---:|
| Core / pixel clock | 200 MHz / 74.375 MHz |
| XLR | 59,147 / 60,800, 97.28% |
| Block RAM | 253 / 256, 98.83% |
| DSP | 80 / 160, 50.00% |
| Core setup slack | +0.259 ns |
| Core hold slack | +0.026 ns |
| Setup/hold clock relationships | All 14 pass |
| Gray-bus skew checks | 18 groups pass setup and hold; not a complete functional CDC proof |

The initial integrated Alpha candidate used 60,603 XLR and 56 DSP. Moving both weighted terms into DSPs saved 1,456 XLR. Relative to the original R5 hardware (58,661 XLR, 32 DSP), final R6 adds 486 XLR and 48 DSP for the new functions; the RAM block count stays the same. Constraints were not weakened to obtain closure.

99,999 is an object-capacity limit, **not a demonstrated 60-FPS limit** at every size or effect setting. No post-change FPGA frame-rate or ten-minute stability result has been fabricated.

## Controls and validation

The console retains volume steps, all ten tracks, count steps of 100/10/1, R2 workload presets, co-op entry, matrix calibration and USB byte diagnostics. New controls select VSync frame pipelining, eight-/two-pixel Alpha, all four square paths, and separate stage stress tests. CSV logs include `vpipe`, `kmode`, `ftab`, `square_path` and `th_stage`.

P1 uses the PC console, P2 a USB keyboard through CH9350L, and P3 the 4×4 matrix. Existing wiring and S1–S16 assignments remain documented in the console's offline `/wiring` page. Co-op startup still confirms diagnostic mode is disabled before enabling controls.

Evidence under `verification/r6` includes 2,105,344 blend/add input combinations, real RTL tests with 99,999 squares, 24 FTAB/SQTAB pixel and snapshot comparisons, 4,800-tick old/new gameplay comparisons for each normal stage with one and three players, input RTL checks, unchanged-720p output regression, and desktop/mobile PTY browser tests. Native Windows CI executed the packaged EXE and checked its assets, Unicode paths, loopback API and no automatic serial connection.

Use matched bit/debug ELF for the first board run. Hold workload, object size, Alpha, output mode and audio settings constant; change one optimization switch at a time. Record presented FPS, logic Hz, video/audio underflows, worst window time, band splits and ten-minute stability. Validate physical 1080p30 compatibility separately.

## Build

After cloning GitHub, run `python3 tools/restore_large_files.py` to restore the current sound bank, matched bit/ELF and generated netlist. Both chunks and original files are SHA-256 checked. The source ZIP already contains its complete sound bank. Stale active build/outflow products were removed from the current Git tree; older versions remain in Git history and versioned BOARD_TEST directories.

With Efinity 2026.1 and its RISC-V toolchain installed:

```text
python3 build_software.py --stats both
python3 build_hardware.py --flow compile
```

Use `--toolchain` or `--efinity` to override installation paths. On Windows, use the installed Python command. Rebuild the audio bank from the game directory with `python3 tools/wav2bank.py --list sound/r6_sounds.txt`. The source package already includes the bank and all referenced ROM initialization files. Hardware builds never program a board; only timing-passing output is promoted into BOARD_TEST.

[Private repository](https://github.com/StarBJL/ti60-fpga-game) · [Native Windows build 40329064648](https://github.com/StarBJL/ti60-fpga-game/actions/runs/40329064648).
