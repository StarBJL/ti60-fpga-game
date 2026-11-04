# Ti60 FPGA High-Density Bullet-Hell Game Platform

> **Architecture:** Efinix Ti60F225 FPGA · Sapphire RISC-V SoC · BitBlt 2D Accelerator · DDR3 · 720p HDMI Video and Audio
>
> [中文](README.md) | **English**

## Project Files and Entry Points

| Component | Source / Project Path | Description |
|---|---|---|
| FPGA RTL | [rtl/](rtl/) | Top-level integration, DDR3 controller, BitBlt, HDMI, audio, and input peripherals |
| Efinity hardware project | [ddr_demo_ti60.xml](par/ddr_demo_ti60/ddr_demo_ti60.xml) | Project configuration, pin/PLL assignments, timing constraints, and SoC IP |
| RISC-V firmware | [standalone/game/](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/) | Main loop, game logic, rendering, sound, UART telemetry, and Makefile |
| Sapphire BSP | [EfxSapphireSoc](par/ddr_demo_ti60/embedded_sw/soc/bsp/efinix/EfxSapphireSoc/) | Board support package, startup routines, and peripheral headers |
| PC control panel | [tools/control_panel/](tools/control_panel/) | Python serial server, browser UI, input mapping, and telemetry |
| FPGA/firmware images | [BOARD_TEST/verified_225/](BOARD_TEST/verified_225/) | 225 MHz FPGA bitstream and Debug/Release ELF images |

---

## Abstract

This project implements a hardware–software co-designed 2D graphics accelerator and bare-metal game platform on the **Efinix Titanium Ti60F225 FPGA** with a **Sapphire RISC-V RV32IM** soft processor. The main system operates at **225 MHz**, transmitting **1280 × 720 video at approximately 60 Hz** and **48 kHz stereo audio** over a single HDMI link. Four application scenarios—bullet-hell shooting, a cooperative top-down shooter, moving-square stress tests, and full-screen image transitions—exercise graphics rendering, shared-memory access, user input, and audio/video synchronization.

To address CPU command-submission overhead and DDR bandwidth contention under tens of thousands of moving objects, the firmware maintains compact **16-byte hot-state records** for projectiles and constructs stable drawing tables during simulation updates. The accelerator reads those tables autonomously, partitions a 720p frame into **45 horizontal bands of 1280 × 16 pixels**, and renders them into an on-chip window in command order. A **16 KiB source-image cache** and fast paths processing up to **eight RGB565 pixels per clock** reduce repeated external-memory transactions. Frame-completion sequence numbers, three rotating object-table sets, and four framebuffers enable the CPU, drawing engine, and HDMI scan-out to execute as an asynchronous pipeline.

The platform also includes a **16-voice hardware PCM mixer**, up to three independent input devices, Chinese/English UI support, and a local web-based serial control panel. Complete games provide functional workloads while configurable high-density graphics scenarios provide quantitative performance measurements.

# Part I — Project Overview

## 1. Features and Capabilities

The single-FPGA system integrates game execution, hardware 2D graphics, multi-voice audio mixing, HDMI video/audio output, and multi-device input. Its principal capabilities are:

- Four scenarios: the Sprout Danmaku bullet-hell game, a cooperative shooter, a moving-square workload, and full-screen image transitions.
- RGB565 drawing, color-key transparency, global Alpha blending, per-pixel A8 masks, and additive effects.
- A PC serial controller, an external CH9350L USB-to-UART keyboard interface, and a 4 × 4 matrix keypad.
- Up to three local players, distinct character abilities, on-screen statistics, and Chinese/English interfaces.
- JTAG programming, serial control, frame-timing telemetry, and configurable workload parameters.

The design separates scene logic, the common 2D rendering API, hardware-specific drivers, and continuous audio/video output. This makes it possible to compare CPU rasterization and dedicated hardware acceleration using the same scene and drawing operations.

## 2. Applications

The architecture can be used as a small embedded 2D game terminal, an HDMI human–machine-interface prototype, or an FPGA hardware/software co-design platform. Its reusable building blocks are the APB configuration interface, AXI DMA and arbitration, graphics-command processing, on-chip caching, double/multiple buffering, clock-domain crossing, and PCM/HDMI output.

## 3. Key Technical Characteristics

The **RISC-V processor handles control-intensive workloads**, including input parsing, gameplay state transitions, collision detection, compact object-state updates, and drawing-table construction. The **dedicated FPGA engine handles pixel-intensive workloads**, including drawing-command expansion, source-data caching, destination blending, and high-throughput DDR writes. Frame-level overlap reduces idle time between the two.

### System Parameters

| Item | Specification |
|---|---|
| FPGA / timing grade | Efinix Titanium Ti60F225 / I3 |
| EDA tool | Efinity 2026.1.132.4.5 |
| Processor | Sapphire RV32IM, 225 MHz |
| Video | RGB565, 1280 × 720, 74.375 MHz pixel clock, 1650 × 750 total timing |
| Scan-out refresh | Approximately 60.10 Hz |
| Audio | 16-voice hardware mixer, 48 kHz stereo PCM over HDMI |
| Graphics fast paths | Up to eight pixels per clock for supported operations |
| Frame composition | 45 bands, each 1280 × 16 pixels (40 KiB per RGB565 band) |
| Source cache | 16 KiB direct-mapped cache |
| Object capacities | Up to 35,200 squares; 32,768 cooperative-mode enemy projectiles; 16,384 Danmaku enemy projectiles |
| Input | PC serial, CH9350L keyboard interface, 4 × 4 matrix keyboard |

The system has been measured at 60 frames/s in representative high-density tests; the exact workloads and measured frame times are listed in **Part III**. Display refresh, software rendering rate, and presentation rate are separate measurements.

## 4. Main Design Contributions

**Hardware/software comparison under a common API.** CPU rendering and FPGA rendering implement the same drawing operations, allowing meaningful measurements of submission time, rasterization time, and external-memory cost.

**Ordered bulk drawing.** Sprite slots and shared drawing state avoid redundant parameter programming. TAB and FTAB commands let the engine read object tables in DDR. When some objects require clipping, fully visible runs are submitted in batches while boundary objects are rendered individually without changing the visual stacking order.

**On-chip horizontal-band composition.** A 720p image is divided into 45 bands. The accelerator performs repeated color-key, Alpha, and additive operations within an on-chip window, then writes each band as a contiguous DDR burst. This avoids repeatedly reading and rewriting the destination framebuffer for heavily overlapping objects.

**Caching and asynchronous frame pipelines.** A 16 KiB source cache serves repeatedly used sprites and glyphs. Three rotating drawing tables, hardware completion numbers, and four framebuffers allow CPU logic, hardware graphics, and HDMI scan-out to overlap safely.

## 5. Development Process

The implementation evolved from CPU-only software rendering to a basic BitBlt unit, shared-memory command lists, sprite-slot reuse, bulk object tables, source-image caching, and on-chip band composition. Hardware timing counters were used to distinguish logic execution, drawing submission, queue stalls, accelerator completion, and vertical-sync waiting. Multi-voice audio, independent input devices, the games, and the telemetry console were then integrated into the same platform.

---

# Part II — System Architecture and Module Design

## 2.1 Overall System Architecture

The system consists of a Ti60F225 board and expansion peripherals, an HDMI monitor and audio output, USB and matrix keyboards, and an optional PC-based serial console. The FPGA contains the Sapphire RISC-V SoC, DDR3 memory subsystem, 2D graphics engine, PCM mixer, HDMI encoder, and input controller.

~~~mermaid
flowchart TB
  PC["PC Web Console"] <-->|UART| CPU["Sapphire RISC-V RV32IM / 225 MHz"]
  KEY["USB Keyboard / 4×4 Matrix"] --> CPU
  CPU --> APB["APB Control Registers"]
  CPU <-->|"Drawing Tables / Game State"| DDR[("Shared DDR3")]
  APB --> BLIT["BitBlt: TAB / FTAB / Band Renderer"]
  BLIT <-->|AXI DMA| DDR
  DDR --> VIDEO["Framebuffer Scan-out DMA"]
  DDR --> MIX["16-Voice PCM Mixer"]
  VIDEO --> HDMI["HDMI TMDS + Audio Data Islands"]
  MIX --> HDMI
  HDMI --> OUT["720p Video + 48 kHz Stereo"]
~~~

DDR3 stores program code, runtime game state, command lists, sprite/fill tables, compressed art and PCM assets, and framebuffers. The processor configures peripherals via memory-mapped APB registers, while the rendering, display, and audio modules access external memory through the AXI subsystem.

## 2.2 Hardware Design

### 2.2.1 Hardware Composition

The board uses standard HDMI, serial, and keyboard interfaces. Computation, graphics composition, and audio mixing run locally on the FPGA; the HDMI link carries the encoded output to the monitor and an attached audio sink.

**RTL integration:** [ddr3_example_top.v](rtl/ddr3_example_top.v) · **Efinity project:** [ddr_demo_ti60.xml](par/ddr_demo_ti60/ddr_demo_ti60.xml) · **SoC IP:** [soc.v](par/ddr_demo_ti60/ip/soc/soc.v)

| Interface Group | Signals | Responsibility |
|---|---|---|
| Clock/reset | CLK, RST, PLL lock | Clock generation, resets, and domain sequencing |
| Peripheral control | APB address, select, enable, read/write data | Control and status registers |
| Memory access | AXI AR/AW/W/R/B, VALID/READY | Burst transfer of command, pixel, PCM, and framebuffer data |
| Audio | PCM, VALID, READY | Stereo sample transfer to the HDMI block |
| External I/O | ROW, COL, UART RX/TX, TMDS | Keypads, keyboard bridge, PC serial, and display |

### 2.2.2 Clock and Reset Network

The Efinity PLL configuration produces the core, DDR, pixel, and serializer clocks. Domain crossings use dedicated FIFOs and synchronized control paths. Reset release follows PLL lock and memory-initialization conditions.

| Clock Domain | Frequency | Main Consumers |
|---|---:|---|
| Board reference | 25 MHz | PLL input |
| Core / user | 225 MHz | RISC-V, graphics engine, audio mixer, control |
| DDR control side | 100 MHz | DDR controller and clock crossing |
| DDR PHY | 400 MHz | Physical DDR3 transfer |
| HDMI pixel | 74.375 MHz | Scan timing and audio packets |
| HDMI fast serial | 371.875 MHz | 10:1 TMDS serializer |
| Audio sample stream | 48 kHz | HDMI stereo output |

With 1650 horizontal and 750 vertical total pixel periods, the video scan frequency is:

**74,375,000 / (1650 × 750) ≈ 60.10 Hz.**

Project-specific clock, pin, and timing assignments are in [ddr_demo_ti60.peri.xml](par/ddr_demo_ti60/ddr_demo_ti60.peri.xml) and [ddr3.sdc](par/ddr_demo_ti60/sdc/ddr3.sdc).

### 2.2.3 Processor and Peripheral Control

The Sapphire RV32IM processor, on-chip caches, and bus interconnect form the control subsystem. Firmware handles input processing, scene updates, drawing-table generation, audio events, frame ownership, and performance statistics. APB register windows expose each accelerator's control and status interface.

| APB Base | Peripheral | Key Functions |
|---|---|---|
| `0xF8100000` | HDMI framebuffer controller | Framebuffer selection, video state, audio FIFO |
| `0xF8100080` | Input hub | Matrix and UART keyboard input |
| `0xF8101000` | BitBlt graphics engine | Commands, table DMA, blend modes, counters |
| `0xF8102000` | 16-voice PCM mixer | Voice setup, KEYON/OFF, PCM fetching, volume |

The firmware-facing register definitions are in [blit.h](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/blit.h), [hdmi_fb.h](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/hdmi_fb.h), and [audio.h](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/audio.h).

### 2.2.4 Graphics Accelerator

**Main RTL:** [blit_engine.v](rtl/blit/blit_engine.v) · [blit_axi_wide.v](rtl/blit/blit_axi_wide.v) · [blit_axi_rd_arb.v](rtl/blit/blit_axi_rd_arb.v)

#### Command Input, Decoding, and Scheduling

The accelerator accepts direct APB register commands or reads encoded commands from shared DDR lists. Sprite slots and persistent drawing state reduce register updates; TAB and FTAB instructions fetch and expand complete sprite or rectangle lists. Commands enter either the direct pixel-rendering path or the band-binning/rendering path.

| Direction | Information | Function |
|---|---|---|
| Input | APB registers and start command | Configure and launch a direct draw operation |
| Input | Command words, sprite entries, fill entries | Hardware-assisted batch expansion |
| Internal | Opcode, source/destination, size, stride, Alpha | Canonical clipped drawing operation |
| Output | AXI transactions, pixel results, write controls | Image reads, composition, framebuffer writes |
| Status | Busy/error bits, frame-done sequence, counters | Non-blocking firmware synchronization |

#### Three Graphics Data Paths

| Data Path | Typical Operations | Characteristics |
|---|---|---|
| General pixel pipeline | COPY, color key, per-pixel A8 Alpha | Handles operations requiring fine-grained pixel treatment |
| 128-bit burst path | Aligned opaque fills and block copies | Transfers up to eight 16-bit pixels per beat |
| Parallel band path | Cached sprites, rectangles, global Alpha, additive blend | Composes multiple pixels per cycle inside the band window |

The effective throughput depends on alignment, blend mode, cache behavior, and DDR arbitration. The fast-path peak of eight pixels per cycle is a data-path capability rather than the sustained full-frame rate.

#### Band Composition and Frame Completion

~~~mermaid
flowchart LR
  A["Ordered CPU Drawing Commands"] --> B["Calculate Covered Bands"]
  B --> C["45 DDR Command Bins"]
  C --> D["Read and Clip One Band"]
  D --> E["On-Chip 1280×16 RGB565 Window"]
  E --> F["Ordered Color-Key / Alpha / Additive Blends"]
  F --> G["Burst Writeback"]
  G --> H["Confirm Final AXI Responses"]
  H --> I["Advance Frame-Completion Sequence"]
~~~

The 1280 × 720 framebuffer is divided into 45 bands, each 16 rows high. A band window requires:

**1280 × 16 × 2 = 40,960 bytes = 40 KiB.**

During binning, each draw operation is assigned to every band that it intersects, preserving submission order. After the end-of-frame marker, the engine processes the bins one by one, clips operations against the current band, performs composition locally, and writes the completed band back to DDR with a burst. Once the final band's AXI writes have been acknowledged, the frame-completion counter advances. Firmware can then request a display flip without exposing an unfinished frame.

This design is especially effective for scenes with repeated overlapping Alpha layers, where direct blending would otherwise require frequent destination-memory read/modify/write cycles.

#### 16 KiB Source-Image Cache

The direct-mapped source cache has **512 lines × 32 bytes**, totaling **16 KiB**. Repeated sprite and font reads can be supplied directly from local memory; misses trigger DDR line fills. When firmware modifies graphics data in DDR, a memory fence and explicit graphics-cache invalidation maintain visibility for subsequent DMA reads.

### 2.2.5 Shared DDR Access and AXI Arbitration

**RTL:** [blit_axi_rd_arb.v](rtl/blit/blit_axi_rd_arb.v) · [blit_axi_wide.v](rtl/blit/blit_axi_wide.v) · [ddr3_top.v](rtl/ddr3_controller/ddr3_top.v)

The shared memory subsystem arbitrates among CPU requests, accelerated drawing, HDMI scan-out, and PCM sample fetches. Video scanning has continuous delivery requirements; graphics rendering needs bulk throughput. The design combines high-priority scan-out reads, AXI response routing, FIFO buffering, 32-/128-bit data-width adaptation, and limits on outstanding requests.

The read arbiter retains its current grant when a selected AR request is stalled by downstream backpressure, keeping AXI address/control signals stable until the handshake. Read responses are routed to the appropriate client using the return ID. Burst transfers reduce address-phase overhead and improve the efficiency of aligned frame operations.

At 1280 × 720 RGB565 and 60.10 Hz, visible-frame scan-out alone consumes approximately **110.78 MB/s**. Producing a complete new framebuffer each refresh adds another approximately **110.78 MB/s** of framebuffer writes before accounting for sprites, command bins, CPU activity, and PCM. A 16-bit DDR3 interface running at 400 MHz double data rate has a theoretical interface bandwidth of **1.6 GB/s**; usable throughput depends on memory timing and contention.

### 2.2.6 Video Output and Hardware Audio Mixing

**RTL:** [hdmi_fb.v](rtl/hdmi/hdmi_fb.v) · [hdmi_island.v](rtl/hdmi/hdmi_island.v) · [audio_mixer.v](rtl/audio/audio_mixer.v)

The video pipeline consists of framebuffer DMA reads, an asynchronous pixel FIFO, RGB565-to-RGB888 expansion, timing generation, and TMDS coding. A firmware write to the requested framebuffer register initiates a pending flip; the new address is latched at the vertical-sync boundary so that the scanout source remains stable for the displayed frame.

The audio mixer reads PCM buffers directly from DDR through per-voice sample caches. It supports source-rate conversion through linear interpolation, independent left/right gain, multi-voice accumulation, and saturation. The stereo stream crosses into the pixel clock domain through an asynchronous FIFO; HDMI data islands carry audio samples, audio clock regeneration, and required infoframes.

| Interface | Parameters / Signals |
|---|---|
| Video configuration | Framebuffer base, flip request, enable, sync state |
| Video stream | RGB pixels, data enable, HSYNC, VSYNC |
| Audio voices | PCM base/length, playback position, step, left/right gain, loop |
| Audio stream | Stereo samples, VALID/READY, FIFO level |
| HDMI output | TMDS differential data and clock |

The documented audio FIFO target is **64 stereo frames**, or about **1.33 ms** of buffered PCM. At a 74.375 MHz pixel clock, the HDMI ACR values are **N = 6144** and **CTS = 74375**:

**48,000 = 74,375,000 × 6144 / (128 × 74375)** samples/s.

Audio delivery is clocked independently of the software rendering loop, allowing the display to repeat a framebuffer while audio sampling continues.

### 2.2.7 External Input Controller

**RTL:** [input_hub.v](rtl/input/input_hub.v) · **Firmware:** [input_devices.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/input_devices.c)

The input hub integrates a 4 × 4 matrix scanner and a UART receiver for the CH9350L keyboard bridge. Matrix scanning selects one row at a time, samples pulled-up column inputs, synchronizes and debounces the key state, and exposes a 16-bit matrix bitmap to the CPU. UART keyboard reports are buffered in a receive FIFO and interpreted by firmware before being mapped to player actions.

## 2.3 Software Design

### 2.3.1 Software Components

The software consists of three cooperating subsystems: **bare-metal RISC-V firmware**, the **PC web/serial control panel**, and **offline asset conversion utilities**. Firmware is compiled into an ELF image and loaded into DDR through the RISC-V debug/programming path. Gameplay, command generation, and audio-event scheduling run on the board.

| Component | Execution Environment | Language | Responsibilities |
|---|---|---|---|
| Bare-metal firmware | Sapphire RV32IM at 225 MHz | C | Input, game rules, draw tables, drivers, frame scheduling, telemetry |
| PC control panel | Windows/Linux local Python service and browser | Python / JavaScript | Serial commands, FPS/timing plots, CSV logging, P1 input |
| Asset preparation | Development workstation | Python / C | PCM banks, Chinese glyphs, compressed sprites and backgrounds |

#### Software Architecture

~~~mermaid
flowchart TB
  APPS["Scenes: Danmaku / Cooperative / Squares / Images"] --> GFX["Shared Gfx API"]
  APPS --> SOUND["Sound Events / BGM"]
  GFX --> SW["CPU Software Renderer"]
  GFX --> ACC["FPGA BitBlt Renderer"]
  ACC --> QUEUE["APB / LIST / TAB / FTAB"]
  SOUND --> MIX["16-Voice PCM Mixer"]
  MAIN["main.c Frame Controller"] --> APPS
  MAIN --> INPUT["Serial / CH9350L / Matrix Input"]
  MAIN --> FLIP["Four Framebuffers / HDMI Flip"]
~~~

#### Firmware Layering

| Layer | Module | Main Source | Role |
|---|---|---|---|
| Main control | Frame scheduling | [main.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/main.c) | Initialization, framebuffer ownership, overlap, statistics |
| Input/protocol | PC and local input | [control.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/control.c), [input_devices.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/input_devices.c) | UART control, packet parsing, input-source routing |
| Applications | Danmaku | [th_game.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/th_game.c), [th_render.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/th_render.c) | Bullet simulation, stages, spell cards, drawing bins |
| Applications | Cooperative game | [game.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/game.c), [render.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/render.c) | Enemies, collisions, upgrades, three-player combat |
| Applications | Graphics benchmarks | [demo.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/demo.c) | Square loads, Alpha comparisons, image scan |
| Drawing interface | Renderer abstraction | [gfx.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/gfx.c), [gfx.h](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/gfx.h) | Rendering backend selection, clipping, text |
| Graphics driver | BitBlt submission | [blit.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/blit.c), [gfx_hw.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/gfx_hw.c) | Command encoding, memory fences, completion queries |
| Audio layer | PCM and events | [audio.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/audio.c), [th_sound.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/th_sound.c) | Sound-bank access, voice assignment, playback and fades |
| Host UI | Serial monitor | [server.py](tools/control_panel/server.py), [app.js](tools/control_panel/app.js) | Local telemetry and workload control |

#### Startup and Runtime

After hardware/driver initialization, the firmware configures the framebuffers, detects available BitBlt features, prepares graphics assets, enables HDMI and the mixer, and enters the main loop. In the default overlapping pipeline, the CPU submits drawing commands for a prepared scene while executing the next logic update and scheduling sounds. It requests presentation only after the relevant draw frame has completed.

User actions can switch among four workloads and toggle drawing backends, VSync, Alpha effects, and other measurement parameters.

#### DDR Memory Layout

| Base Address / Range | Purpose |
|---|---|
| `0x00001000` and below `0x01000000` | Firmware code, normal data, BSS, and stacks |
| `0x01000000`, `0x01200000`, `0x01400000`, `0x01C00000` | Four RGB565 framebuffers |
| `0x01600000`, `0x01800000`, `0x01A00000`, `0x01E00000` | Full-screen image demonstration buffers |
| `0x02000000` onward | PCM sound bank |
| `0x03000000` onward | Graphics band-command bins and work areas |
| `0x04000000` onward | Expanded runtime art, command lists, and glyph caches |
| `0x06000000` onward | Compressed art assets loaded with the firmware |

The definitive link sections and firmware memory reservations are in [game.ld](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/game.ld). A 720p RGB565 framebuffer occupies **1,843,200 bytes**.

### 2.3.2 Main Frame Controller

**Source:** [main.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/main.c) · [hdmi_fb.h](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/hdmi_fb.h)

The controller coordinates input, scene logic, command submission, engine completion, display flips, and telemetry. It excludes the current scanout framebuffer, the last requested framebuffer, and any still-in-progress drawing target from new allocations. Four framebuffers are required because rendering, pending presentation, and active scanout can overlap.

~~~mermaid
sequenceDiagram
    participant CPU as RISC-V Firmware
    participant BLIT as BitBlt Engine
    participant DDR as Shared DDR3
    participant HDMI as HDMI Scan-out
    CPU->>DDR: Write drawing tables
    CPU->>BLIT: Submit BANDS / LIST / TAB / FTAB
    BLIT->>DDR: Fetch and compose
    par Concurrent work
        CPU->>CPU: Update next game state / sound events
    and Accelerator work
        BLIT->>DDR: Burst-write completed bands
    end
    BLIT-->>CPU: Completed frame sequence
    CPU->>HDMI: Request framebuffer flip
    HDMI-->>CPU: Latch new FB_CUR at VSync boundary
~~~

The HUD statistics are refreshed at **0.5-second** intervals and the UART telemetry window at approximately **one-second** intervals. Measured phases include game logic, submission, queue blocking, tail wait, and VSync wait.

### 2.3.3 Logic and Bulk-Command Submission

Rendering commands and gameplay simulation are timed separately. After the graphics engine receives the current prepared table, the CPU immediately advances the next input/collision/audio state. Dense populations are stored in **8-byte hardware drawing entries** and submitted with TAB (sprites) or FTAB (colored rectangles).

| Table | Entry Layout | Workload | Engine Operation |
|---|---|---|---|
| `GfxItem` | Word 0: `{y[31:16], x[15:0]}`; word 1: sprite slot | Danmaku and cooperative projectiles | Draw sprite at x,y using registered slot attributes |
| `GfxFillItem` | Word 0: `x[11:0], y[23:12], w[31:24]`; word 1: `RGB565[15:0], h[23:16], alpha[31:24]` | Moving squares | Fill a w×h rectangle with optional global Alpha |

The 16-byte gameplay hot-state object and the 8-byte hardware draw-table entry serve different purposes. Danmaku projectiles, cooperative projectiles, and moving squares each use **three rotating table sets** to prevent the CPU from overwriting data the engine is still reading. Fully visible sprites can be issued as a single batch. When a sprite intersects the screen boundary, the renderer splits the stream into ordered in-bounds runs and individually clipped entries—for example **TAB(A,B) → clipped C → TAB(D,E)**—to preserve draw order.

### 2.3.4 Sprout Danmaku

**Sources:** [th_game.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/th_game.c) · [th_stage2.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/th_stage2.c) · [th_render.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/th_render.c)

Sprout Danmaku is the default boot scene. It includes two stages with mid-stage and final bosses, normal/lunatic difficulty modes, multiple spell-card phases, and **14 workload-density settings from ×1 to ×48**. Its enemy projectile pool has **16,384 slots**. In the 720p presentation, a **960 × 720** game composition is centered on screen, with a **576 × 672** active play area and side information panels.

During a logic tick, `bullets_update()` advances active projectile positions, removes out-of-bounds objects, evaluates collisions/grazes, and constructs **nine projectile drawing bins**. Rendering later emits backgrounds, actors, effects, and the nine tables. Workload control can adapt the generation multiplier using per-frame active work time.

### 2.3.5 Cooperative Shooter

**Sources:** [game.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/game.c) · [render.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/render.c) · [carrot_fx.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/carrot_fx.c)

The cooperative game uses native **1280 × 720** graphics and supports three independently controlled players, a shared experience level, individual health/passives/ultimate abilities, and upgrades. The separate projectile stress mode supports an enemy-shot pool of up to **32,768** entries. Stage logic organizes waves, boss appearances, and enemy behavior; rendering combines compressed backgrounds, ordered actors, batched projectiles, multi-stage ultimate effects, and bilingual HUD elements.

#### Character Roles and Ultimate Abilities

| Player | Attack and Passive | Normal Attack Projectiles | Ultimate Effect |
|---|---|---|---|
| P1 Orange — Shooter | Long-range burst fire; mobility boost | Initially 3; maximum 6 | Piercing fire-dragon wave with multiple area-damage phases |
| P2 Blue — Warrior | Close-range circular slash, short reflection window, ranged sword waves | Initially 2; maximum 4 | Expanding blue tiger with circular sword-wave effects |
| P3 Pink — Mage | Homing magic projectiles; shield recovery after avoiding damage | Initially 3; maximum 5 | Expanding phoenix, cherry-blossom particles, explosion rings |

Ten upgrade categories cover attack rate, damage, mobility, shields, and ultimate enhancements, with per-player selection in multiplayer sessions. Blue tiger and phoenix Alpha assets are prepared at four sizes from **160 × 90** to **320 × 180**; animation stages and fadeout are controlled independently of hit detection.

The campaign contains six progressively harder areas. The first five use a **60-second effective combat window** and boss-clear conditions; the sixth uses repeating **50-second endless waves** with larger enemy counts and possible dual bosses. Timers exclude paused/menu states.

### 2.3.6 Moving Squares and Image Transitions

**Source:** [demo.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/demo.c)

The moving-square benchmark supports up to **35,200 independent objects**, selectable fixed sizes or random sizes between **6 and 40 pixels**, and an option to render half of the objects with global Alpha blending. The CPU updates positions and writes a rotating fill table; a single FTAB instruction requests hardware drawing. The image demonstration uses four 720p images and separate buffers to benchmark full-screen transfers and scan-style transitions.

### 2.3.7 Common 2D Rendering API

**Sources:** [gfx.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/gfx.c) · [gfx.h](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/gfx.h)

The common graphics layer exposes identical drawing semantics to CPU and BitBlt backends. It handles viewport clipping, blending modes, text rendering, and batch APIs. If a particular operation is unsupported by the hardware fast path, it falls back to the general drawing route.

| Rendering Interface | Behavior |
|---|---|
| `gfx_begin_hw` / `gfx_end` | Bind a destination framebuffer; submit the completed frame |
| `gfx_fill_rect` / `gfx_fill_alpha` | Solid rectangle / global-Alpha rectangle |
| `gfx_blit` / `gfx_blit_alpha` / `gfx_blit_add` | Sprite copy with color key, Alpha, or additive blending |
| `gfx_blit_table` / `gfx_blit_table_alpha` | Bulk sprite-table submission |
| `gfx_blit_table_clipped` | Ordered batching with individually clipped edge objects |
| `gfx_fill_table` | FTAB bulk rectangle submission |
| `gfx_text` | Chinese/English text drawing with glyph/label caching |

English text can be generated from a compact 5 × 7 font atlas or merged line segments. Chinese text uses prebuilt A8 grayscale glyphs blended onto a colored plane. Frequently repeated HUD and menu labels are cached to reduce per-frame glyph composition. The caches are reset after outstanding work completes when the UI language changes.

### 2.3.8 BitBlt Driver and Completion Protocol

**Sources:** [blit.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/blit.c) · [blit.h](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/blit.h) · [gfx_hw.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/gfx_hw.c)

The driver encodes render operations as direct APB register writes or as a **2 MiB DDR command list**. In band mode, BANDS and ENDB delimit a frame, while a list-end pointer (LEND) publishes the readable command region incrementally or in full. Once the engine has composed all 45 bands and received their final write acknowledgments, a completion sequence becomes visible to firmware. The `blit_band_done(seq)` interface lets the controller check readiness without blocking on every individual command.

This separation between **command publication**, **DDR write completion**, and **VSync address switching** is central to the pipeline's correctness.

### 2.3.9 Software Audio Layer

**Sources:** [audio.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/audio.c) · [th_sound.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/th_sound.c) · [game_sound.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/game_sound.c)

The audio layer indexes the DDR-resident sound bank and programs the 16-voice hardware mixer. The bank contains **39 audio clips**, including scene BGM, combat effects, and **10 mono 22.05 kHz voice clips** for character ultimates and boss introductions. Hardware resampling and mixing produce 48 kHz stereo PCM for HDMI.

Sound events are scheduled from game state, with fade transitions, priority-based mutual exclusion for simultaneous voice announcements, and temporary BGM attenuation during dialogue or key event playback.

### 2.3.10 PC Console and Multi-Source Input

**Sources:** [server.py](tools/control_panel/server.py) · [app.js](tools/control_panel/app.js) · [control.c](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/control.c)

The PC console combines a local Python HTTP server, browser-based controls, serial packet processing, and telemetry. It can configure scenes, player counts, workload density, rendering backend, Alpha effects, language, and audio volume. Live charts show rendering/presentation rates and per-phase timing; CSV logs preserve measurements. PC keyboard/controller actions can be forwarded as P1 input while CH9350L and matrix-keypad devices provide P2/P3 input according to the active player mode.

| Direction | Packet / Format | Purpose |
|---|---|---|
| PC → board | Four-byte binary input-state packet | P1 key mask/check, approximately 60 packets/s |
| PC → board | `:<id> <command> [value]` | Command IDs and scene/workload controls |
| Board → PC | `ACK id=<id> ok=<0|1>` | Command acknowledgment and success status |
| Board → PC | Telemetry record, approximately once per second | FPS, work-phase timing, active object count, configuration epoch |

---

# Part III — Implementation and Performance Results

## 3.1 Integrated Hardware and Software

The Ti60F225 board, expansion peripherals, HDMI display, and PC console form a complete local test platform. The build uses **Efinity 2026.1.132.4.5** with device timing model **I3**, and a **225 MHz** core clock. Software includes the two games, synthetic benchmarks, sound-bank utilities, and a real-time browser monitor.

### FPGA Resource Utilization

| Resource | Used / Available | Utilization |
|---|---:|---:|
| XLR logic cells | 56,479 / 60,800 | **92.89%** |
| Embedded memory blocks | 250 / 256 | **97.66%** |
| DSP blocks | 56 / 160 | **35.00%** |

The high memory utilization is associated with the CPU caches, the 16 KiB sprite cache, the 40 KiB band window, and asynchronous FIFOs. The memory budget is a significant consideration for extending the accelerator.

### Static Timing

At the selected 225 MHz implementation point, the reported static timing results show positive setup and hold margins across seven listed clock relationships. The core clock relationship has **+0.063 ns setup slack** and **+0.012 ns hold slack**, corresponding to approximately **228.258 MHz** for the reported setup path.

## 3.2 Workload Measurement Method

Performance tests use 1280 × 720 display output and a 225 MHz core clock. The on-screen measurement labels include:

- **RENDER:** rendering-loop frequency.
- **SHOW:** newly displayed/presented framebuffer frequency.
- **WORK:** effective per-frame CPU/hardware work time excluding VSync waiting.

A full 60 fps frame has a time budget of approximately **16.64–16.67 ms** under the configured scan timing. Distinguishing RENDER, SHOW, and the physical scan clock is important when a previously rendered framebuffer is repeated.

### 3.2.1 Full-Screen Image Copy: CPU vs. Hardware

Using the 720p image-transition workload, the hardware accelerator maintained an output rate of **60 frames/s** with an effective WORK time of approximately **0.4 ms**. The CPU software renderer required approximately **28 ms** per frame and achieved approximately **36 frames/s** in the comparison. This illustrates the benefit of aligned burst transfers for large pixel blocks under the measured conditions.

### 3.2.2 Moving-Square Alpha Stress Test

The square benchmark compares independent moving rectangles under different geometry sizes, with approximately half of the fill-table entries using global Alpha blending.

| Square Size | Output FPS | Work Time | Number of Squares |
|---|---:|---:|---:|
| 24 × 24 px | 60 | 16.4 ms | 20,900 |
| 32 × 32 px | 60 | 16.4 ms | 13,800 |
| 40 × 40 px | 60 | 16.2 ms | 9,700 |
| Random 6–40 px | 60 | 16.3 ms | 20,100 |

The measured workload uses **1280 × 720 output** and approximately **50% global-Alpha fill operations**.

### 3.2.3 Cooperative Shooter Projectile Stress Test

The independent projectile benchmark uses color-key rendering for enemy bullets and player sprites. Four projectile sizes were measured at high simultaneous object counts.

| Projectile Sprite | Output FPS | Work Time | Configured / Measured Active |
|---|---:|---:|---:|
| 9 × 9 px | 60 | 16.11 ms | 15,400 / 15,395 |
| 13 × 13 px | 60 | 16.05 ms | 15,000 / 14,992 |
| 17 × 17 px | 60 | 16.02 ms | 14,900 / 14,800 |
| 25 × 25 px | 60 | 13.31 ms | 11,000 / 10,996 |

In these measured conditions, effective work times remain within the approximately 16.64 ms frame budget, with 60 frames/s output.

### 3.2.4 Sprout Danmaku High-Density Rendering

At the maximum **×48 projectile-generation density** setting, the measured scene contained **15,021 simultaneously active enemy bullets** while both **RENDER** and **SHOW** remained at **60 frames/s**.

Spell cards use multiple shot shapes, emission patterns, and overlay effects. Alpha blending and additive lighting allow fog, colored environments, shields, magic circles, and meteor effects to be layered over scene graphics.

#### Projectile Types and Rendered Sprite Sizes

Seven base projectile types are divided into nine hardware render bins because several types have directional variants.

| Type | Render Bin | 720p Sprite Canvas |
|---|---|---|
| Pellet | PELLET | 12 × 12 |
| Orb | ORB | 24 × 24 |
| Rice | RICE0 / RICE1 | 24 × 24 |
| Kunai | KUNAI0 / KUNAI1 | 36 × 36 |
| Star | STAR | 24 × 24 |
| Large orb | BIG | 48 × 48 |
| Bubble | BUBBLE | 96 × 96 |

Collision radii are defined separately from sprite canvas dimensions.

---

# Part IV — Summary and Future Development

## 4.1 Potential Extensions

**Write-back cache and explicit coherency.** The current write-through CPU cache simplifies shared-memory visibility for DMA. A write-back design with explicit cache-clean operations at drawing-table publication boundaries could reduce memory traffic during dense projectile updates.

**On-chip memory optimization.** With **250/256 embedded memory blocks** utilized, future changes can explore dynamic allocation between source caching, band storage, and FIFOs. More finely pipelined arithmetic and read/write control may improve clock closure without increasing the total memory footprint.

**Tile-based composition for higher resolutions.** The current 1280 × 16 horizontal window is specialized for 720p. A fixed-size rectangular tile such as 64 × 64 could extend the approach toward 1080p while controlling the on-chip working-set size.

## 4.2 Engineering Observations

Full-system timing analysis is essential for hardware/software co-design. A dense projectile workload spans CPU object updates, command generation, AXI submission, repeated DDR image traffic, and video scan-out. Improving only one stage may expose a different bottleneck. The project therefore measures logic, submission, queue waiting, engine completion, and presentation separately and maps each optimization to a specific stage.

Correctness also depends on explicit asynchronous ownership contracts. Publishing a command table, making its contents visible to DMA, receiving the final framebuffer write response, and switching the HDMI scan address occur at different points in time. Three rotating drawing tables, four exclusive framebuffers, and completion sequence numbers coordinate these operations while allowing parallel progress.

---

# Part V — References

1. [Ti60 FPGA Game — Project Source](https://github.com/StarBJL/ti60-fpga-game)
2. Efinix Inc., *Efinity Software User Guide*, v2026.1.
3. Efinix Inc., *Efinity Timing Closure User Guide*, v7.6, 2026.
4. Efinix Inc., *Sapphire RV32 SoC Data Sheet and BSP Reference Manual*.

---

# Build, Program, and Run

### FPGA Hardware Project

Open [ddr_demo_ti60.xml](par/ddr_demo_ti60/ddr_demo_ti60.xml) in **Efinity 2026.1.132.4.5**. The project targets **Ti60F225 / I3** and includes the required SoC/DDR sources, [pin and PLL configuration](par/ddr_demo_ti60/ddr_demo_ti60.peri.xml), and [DDR3 timing constraints](par/ddr_demo_ti60/sdc/ddr3.sdc). Build the hardware design with the Efinity synthesis, place-and-route, timing, and bitstream tools.

### RISC-V Firmware

The firmware project uses [makefile](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/makefile), [game.ld](par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/game.ld), the Sapphire BSP, and a compatible RISC-V cross-toolchain. The available Debug/Release ELF images are both optimized builds; the Debug profile enables additional statistics.

### Programming Order

1. Program the FPGA with [ti60_v6_225m.bit](BOARD_TEST/verified_225/ti60_v6_225m.bit).
2. Through the RISC-V IDE/JTAG environment, load either [Debug ELF](BOARD_TEST/verified_225/ti60_verified_225_debug.elf) or [Release ELF](BOARD_TEST/verified_225/ti60_verified_225_release.elf), including the audio and graphics load sections.
3. Start the CPU and verify serial output, HDMI video, input devices, and PCM audio.
4. Use the onboard keys or the optional console to switch scenes and benchmark settings.

### PC Serial Control Panel

The Python/web console is launched directly from the project root, without an additional root-level shell or batch wrapper. **Python 3.10+** is required.

Linux:
~~~bash
python3 tools/control_panel/bootstrap.py --open
~~~

Windows (Python launcher):
~~~powershell
py -3 tools/control_panel/bootstrap.py --open
~~~

The launcher prepares a local Python environment and starts the browser UI. Select the serial port and connect from the UI to send commands, view performance metrics, and save telemetry. Board-side gameplay does not require the PC console.

### Third-Party Components

The project integrates DDR/Sapphire SoC IP, TMDS example logic, and third-party runtime components. Copyright notices and applicable licenses are retained in their corresponding source and package files.
