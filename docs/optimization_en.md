# R5 RAM, critical-path and Alpha optimization

The implementation is in `PROJECT/rtl/blit/blit_engine.v`. Baseline utilization and timing come from the original R2 Efinity reports; R4 did not change RTL. **Only a new Efinity build establishes Ti60 utilization and timing.** Vivado / XSim 2021.2 is used here for portable behavioral verification, without mapping this design to an AMD device for Ti60 sign-off.

## Final target implementation result

The final Efinity 2026.1.132.4.5 reports confirm Ti60F225 implementation with positive setup and hold slack for all seven reported clock relationships. **R5 has not been tested on the physical board.** Utilization is **58,518 / 60,800 XLRs (96.25%), 253 / 256 RAMs (98.83%), and 32 / 160 DSPs (20%)**, leaving 2,282 XLRs, three RAMs and 128 DSPs. The core remains at 200 MHz with **+0.118 ns setup** and **+0.028 ns hold** slack. The reported 204.834 MHz core Fmax is an analysis result for this implementation, not a changed operating frequency.

| Reported clock relationship (same domain) | Constrained period ns | Setup slack ns | Hold slack ns |
| --- | ---: | ---: | ---: |
| `core_clk` | 5.000 | +0.118 | +0.028 |
| `pll_fb_clk` | 10.000 | +3.373 | +0.030 |
| `sdram_clk` | 2.500 | +0.321 | +0.039 |
| `rx_cal_clk` | 2.500 | +0.321 | +0.034 |
| `tx_cal_clk` | 2.500 | +0.308 | +0.097 |
| `tx_cal_clk_90edge` | 2.500 | +0.313 | +0.069 |
| `hdmi_tx_slow_clk` | 13.445 | +6.890 | +0.048 |

The final worst core path is now inside the Sapphire CPU, with **8 logic levels** and reported Delay **4.572 ns**:

- Begin: `u_sapphire_soc/u_EfxSapphireSoc/system_cores_0_logic_cpu/dataCache_1/stageB_waysHitsBeforeInvalidate[3]~FF|CLK`
- End: `u_sapphire_soc/u_EfxSapphireSoc/system_cores_0_logic_cpu/CsrPlugin_mscratch[2]~FF|CE`

This table contains the complete Clock Relationship Summary. `hdmi_tx_fast_clk` and `clk_25m` appear in the target-clock list but do not have separate relationship rows in this report; no slack values are invented for them. These results do not replace CDC verification or board measurements.

Evidence: `PROJECT/par/ddr_demo_ti60/outflow/ddr_demo_ti60.timing.rpt` (report date `Sat Oct 24 01:46:06 2026`) and `ddr_demo_ti60.place.rpt` in the same directory. The final bitstream is **2,811,948 bytes**, SHA256 `25dc85470b3f73f929ef75c60eb06880a7c46533f30c26820a40eb5def85ac83`. Mapped-counter arithmetic checks, implementation metrics and source hashes are recorded together in `verification/r5/blit_hdmi_validation.json`.

## Implemented changes

| Area | R2 / R4 baseline | R5 implementation | Trade-off |
| --- | --- | --- | --- |
| Read descriptors `u_bd` | 64 × 23-bit RAM FIFO, two RAM10 blocks, plus output register | 16 × 23-bit register FIFO plus output register | Existing full backpressure bounds total occupancy to 17; less latency-hiding capacity |
| Command prefetch `u_lf` | 64 × 32-bit RAM FIFO, two RAM10 blocks | 32 × 32-bit register FIFO; 32 reserved words maximum; prefetch once 16 slots are free | FF/LUT storage replaces RAM; shorter bursts may affect list-heavy workloads |
| R FIFO reservation | Cache/AR decision followed by several arithmetic terms | Reuse registered `rdi_m`: `rf_res_next = rf_res + rdi_m - rf_pop`; credit uses `rf_eff = rf_res + rdi_m` | Preserves pending reservations and capacity; target timing improvement remains a measurement |
| Window / Alpha pipeline | 1280-pixel × 16-row window; generic Alpha at two pixels/cycle | Arithmetic and interfaces preserved | Avoids changing rendering order, bins or image appearance |

R5 Efinity mapping confirms **four RAM10 blocks released**: `u_bd` and `u_lf` each map to zero instead of two RAMs; total `u_blit` RAM drops from 85 to 81. This is a module-level result; final whole-design utilization is given above. New storage is approximately 1,392 data bits plus pointers and read muxes. The final whole-design resource totals are taken from the completed target report above.

### RAM budget and delivery trade-off

The final implementation keeps **three explicit 512×16 scan-line cache banks** and implements the CH9350 UART's **64-byte receive FIFO in one RAM10 block**. The four blocks released by the blitter fund three scaler-cache blocks and one UART block. The whole-design budget is therefore `253 - 4 + 3 + 1 = 253 / 256`, not 252; the final Efinity mapping and place-and-route reports confirm 253 blocks. UART capacity is retained, together with overflow, framing-error and explicit-release handling.

A trial that forced the UART FIFO into registers used 252 RAM blocks but reached 59,738 / 60,800 XLRs (approximately 98.25%) and **−0.159 ns core setup slack**. That variant is not a delivery candidate. Its worst path moved to the performance-counter-to-SoC APB readback, followed by wide-write data enables, showing congestion and long routing rather than the original reservation arithmetic bottleneck. Retaining one UART RAM recovers registers and read mux logic, prioritizes the original 200 MHz timing target, and preserves receive capacity. Generating a bitstream or completing the tool flow does not establish setup/hold closure.

The baseline used 55,848 / 60,800 XLRs, 253 / 256 RAMs, and 32 / 160 DSPs. Core clock was 200 MHz, setup slack +0.053 ns and hold slack +0.025 ns. The worst path ran from `u_blit/f_addr[1]` to `u_blit/rf_res[9]` through 34 logic levels. Registering the mutually-exclusive issue increment removes multiple late arithmetic terms from that cone. The mapped `rf_res` combinational cone shrinks from 304 cells / 237 inputs to 41 cells / 20 inputs. This structural count is not a routed delay and does not justify increasing the clock without place-and-route evidence.

## Reservation invariant

Let `O(t)` be the previous implementation's reserved-word count, `R(t)` the new accumulator, and `I(t)` the previous cycle's issue increment. Reset establishes `O(t)=R(t)+I(t)=0`. The update is `R(t+1)=R(t)+I(t)-pop(t)` and `I(t+1)=issue_words(t)`, preserving `O(t+1)=R(t+1)+I(t+1)`. Credit checks use the full effective value, and the three narrow AR issue classes are mutually exclusive. `DBG0` also reports the effective value so its external meaning remains unchanged.

## Alpha behavior and next optimization steps

Global alpha expands with `g9=g+(g>>7)`, mapping 255 exactly to 256. With an A8 plane, `e=(A8*g9)>>8` is expanded in the same way. Normal channel blending is `(src*a + dst*(256-a) + 128)>>8`; additive blending retains full destination weight and saturates. R5 keeps this exact convention instead of replacing it with division by 255.

The generic pipeline accepts two RGB565 pixels per clock: 400 Mpx/s is an arithmetic ceiling at 200 MHz, not measured frame throughput. Cache, DDR, command changes, row readiness, overlapping writes, binning and flushing reduce application throughput. The eight-pixel K path supports opaque/keyed operations and full-weight additive blending; it is **not an eight-pixel general-Alpha pipeline**.

Recommended next steps are:

1. Continue reducing submitted work using immutable background/HUD/sprite caches, following the R4 font/static-label work. Measure actual coverage and Alpha pixels instead of object counts alone.
2. Profile binning, band rendering, K work, flushing, cache hit/miss and read waits alongside CPU work and physical presentation rate. Compare equal counts and equal sizes with Alpha enabled/disabled.
3. Prototype a four-pixel Alpha path using spare DSP capacity. Supply matching source/A8/destination-window bandwidth, extend or bypass write hazards, and preserve command order and color-key masking. Random overlapping pixel comparisons must pass before Efinity area/timing comparison.
4. Consider an eight-row band or two-dimensional tile architecture separately. A 720p eight-row window could use 24 rather than 40 RAM blocks, but doubles bands from 45 to 90 and changes counters, bin addresses/capacities, software allocation and DDR traffic. Native 1080p also changes frame-buffer slots and band configuration.

Reducing `RING_AW` to nine is not a safe shortcut: the narrow path waits for a complete row, while a 1280-pixel row needs 640 words and would not fit in a 512-word ring. Halving the depth of a 512×32 cache bank does not necessarily save target RAM blocks because width still requires two blocks in the current mapping. R5's 1080p30 output keeps the 720p rendering canvas and scales scanout, avoiding a larger BitBlt window.

## Verification and remaining measurements

| Check | Evidence |
| --- | --- |
| XSim 2021.2 FIFO ordering, occupancy, full/empty and simultaneous read/write; 12,000 cycles each | `verification/r5/blit_control_xsim_simulate.log` |
| Actual P2…P6 arithmetic; 20,000 RGB565 pairs, all global alpha values, A8 endpoints and additive saturation; one pair per cycle | `verification/r5/blit_alpha_xsim_simulate.log` |
| Integrated AXI/golden-memory regression, seed 41; 1,056,527 cycles including 12 band frames, long rows, list/TAB, cache, wide/narrow and ordered overlap | `verification/r5/blit_random41.log` |
| Seed 73, strict W-after-AW backpressure, DDR read latency 24…160 clocks, non-default LIMIT=2/2/128 | `verification/r5/blit_random73_backpressure.log` and `blit_r4_backpressure.log`: both R4 and R5 deadlock in the wide-band geometry case; inherited limitation, not a PASS. The default-LIMIT full regression passes. |
| Cycle-by-cycle effective-credit conservation, descriptor occupancy ≤17, list reservation ≤32 | `verification/r5/blit_credit_test.log` and `blit_credit_result.json` |
| Default LIMIT, strict W-after-AW, long DDR latency; 1,279,988 cycles and 12 band frames, including 2560-byte rows and 45 bands | `verification/r5/blit_strict_default_limit.log`: PASS |
| Asynchronous 200 MHz / 74.375 MHz clocks; active-video requests; 720→1080→720; 3,916,800 RGB and final decoded-TMDS pixels; fixed 720p DMA boundary; AVI VIC/checksum; zero video underrun | `verification/r5/hdmi_verilator.log` and `hdmi_dualvideo_tb.sv`: PASS |

The Alpha arithmetic test intentionally forces only the P2 input boundary; integrated AXI tests provide separate ordering/data-consistency evidence. Simulation does not prove CDC correctness, target timing or board operation. A legacy wide-skid count-width warning truncates a four-bit formal count to three bits, enough for its five-entry capacity; that behavior was not changed.

After mapping, run `PROJECT/sim/blit/check_retime.py`, `PROJECT/sim/blit/check_netlist_cone.py`, and `verification/r5/blit_check_rf_res_netlist.py NEW_R5.map.v`. Previous Efinity versions/builds exhibited reset-retiming and shifted-single-bit-adder mapping errors in these counters, making netlist checks essential even after RTL simulation passes.

Board measurements remain to be supplied: switching success/count and lock time for both video modes; video/audio underrun deltas; maximum stable object count at each size and Alpha setting; at least ten minutes of sustained operation; temperature and actual clock configuration. Historical R2 observations must not be presented as new R5 board results.
