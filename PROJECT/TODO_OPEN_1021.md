# Open issues (2026-10-21 11:40) — to do later

Current state:
- 720p bitstream: core_clk 201.9 MHz; hold +0.027 ns; check_netlist_cone OK.
- 720p danmaku: full screen with native 1.5x art.
- Tight-box fix delivered (_hd720_game_c). It still has to be compiled and checked on the board.

## A. Verify on the board first
1. Compile _hd720_game_c and run it on the board. In wave 4 (Bending Shower), stress x48, press `#` to compare 480 and 720 on the UART line. Expected:
   - `K busy` clearly lower;
   - source cache hits back to 96 % or more;
   - `tail_wait` smaller.
2. Look at the new HD art on the board: side strips, shaded bullets, Scale3x characters, item cards. Also check the boot UART line "art drawn in N ms": is the HD art generation time acceptable?

## B. Performance (stress x48: 1.5k–15k bullets)
3. **CPU bullet loop (main bottleneck in both modes).** About 1 µs per bullet (200 clk). 15k bullets take 15 ms of logic, which limits 480 to 61 fps and 720 to 57 fps.
   - Plan: split ThBullet into hot and cold fields. Hot, 16 B: x, y, vx, vy, timer, flags, bk, sp. Cold: ang, spd, acc, angv, aux_*.
   - Expected 20–30 % less loop time.
   - Must stay bit-identical in the host model (frame-by-frame compare).
4. **Engine at 720p.**
   - Phase 1 (binning): 2.9 ms. Taller HD sprites cross about 2.4 bands per command.
   - Write-back: about 2.2 ms per frame (fixed).
   - Re-measure after #1; consider more tight boxes (stars, orbs: the corners are key).
5. **Wider source cache use (optional).** BLIT_CACHE_MAX_W = 64, so the 96 px HD sprites (bubble, GLOW64, rings) use the narrow uncached path. Measure before changing: a 96x96 sprite is 18 KB, more than the 16 KB cache.

## C. Display behaviour
6. **480 mode on the 720p bitstream is 640x480 1:1, centred with black borders.** It was full screen on the 480p bitstream because the monitor scaled it. Options:
   - (a) keep the 480p bitstream for full-screen 480;
   - (b) add a 1.5x scan-out upscaler in hdmi_fb, nearest-neighbour, 640x480 → 960x720, with pillarbox;
   - (c) leave it as a comparison mode only.
7. **vsync OFF looks uneven.** Expected: the render rate does not match 60 Hz, so ticks per displayed frame are 0, 1 or 2. Play with vsync ON plus stress AUTO.
   - Note: with vsync ON, a frame over 16.7 ms costs a whole extra refresh (that frame effectively 30 fps). Could add an "adaptive" mode later.

## D. Boot from flash (discussed, not started)
8. **Space needed:** bitstream 2.62 MB + program 0x43860 (270 KB) + sound bank 2.03 MB ≈ 6.1 MB. An 8 MB (64 Mbit) flash is enough; check the chip marking on the board.
9. **Steps:**
   - bootloaderConfig.h: USER_SOFTWARE_SIZE 0x1F000 → about 0x50000.
   - Add a second spiFlash_f2m for the sound bank, flash 0x400000 → DDR 0x02000000.
   - binGen.py, then rebuild the bitstream.
   - Split game.elf with objcopy (game.bin is 35 MB because of the gap).
   - Combine bit + app + sound in the Efinity Programmer.

## E. Housekeeping
10. The container copy of peri.xml is stale (pll_hdmi M 17); the device file is the truth (M 1, CLK0 /119).
11. wip_720p/STATUS_720P.md on the device is outdated (720p is live now).
12. Older items still open:
    - the user's real WAVs into sound/sounds.txt;
    - the write-LIMIT overshoot by one burst (tb only).
