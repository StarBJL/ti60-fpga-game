/*
 * host_capture.c - record what the real main.c sends to the BitBlt engine up to the end
 * of the first band frame, for an RTL replay (sim/blit/tb_replay.py), 2026-10-20.
 *
 * Runs src/main.c on the host like host_main_test.c (engine = tools/blit_model.h, which
 * reports the board's CAPS 0xFF0A0904 here so the driver programs the same registers).
 * Writes:
 *   <out>.apb  every engine APB write "off value" (hex), in order
 *   <out>.mem  memory image: records {u32 addr, 4096 bytes} of every non-zero 4 KB page
 *              of the program's data/bss and of 0x01000000..0x01FFFFFF (framebuffers)
 * and exits when the model has processed the ENDB of band frame <N> (default 1).
 *
 *   gcc -O1 -w -no-pie -fno-pie -DBLIT_HOST_MODEL -DSPROUT_STATS=1 -DBOOT_DIAG=0 \
 *       -Itools/mock -Isrc -Itools -o host_capture tools/host_capture.c \
 *       src/game.c src/gfx.c src/render.c src/demo.c src/gfx_hw.c src/blit.c src/mini_snprintf.c src/pad.c \
 *       src/th_tab.c src/th_game.c src/th_pattern.c src/th_stage2.c src/th_art.c src/th_render.c src/th_pix.c \
 *       src/audio.c src/th_sound.c
 *   ./host_capture frame1 [N]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <sys/mman.h>

static uint32_t mock_t;
static uint32_t mock_hdmi[16];
uint32_t mock_ocr[0x400];
static FILE *f_apb;
static const char *out_base;
static uint32_t stop_seq = 1, nwr;
extern char etext, _end;

static void dump_and_exit(uint32_t seq)
{
    char name[512];
    FILE *f;
    uintptr_t a, lo = ((uintptr_t)&etext) & ~(uintptr_t)4095, hi = ((uintptr_t)&_end + 4095) & ~(uintptr_t)4095;
    uint32_t pages = 0;
    static const uint8_t zero[4096];
    snprintf(name, sizeof name, "%s.mem", out_base);
    f = fopen(name, "wb");
    for (int pass = 0; pass < 2; pass++) {
        uintptr_t s = pass ? 0x01000000u : lo, e = pass ? 0x02000000u : hi;
        for (a = s; a < e; a += 4096) {
            if (!memcmp((const void *)a, zero, 4096)) continue;
            uint32_t a32 = (uint32_t)a;
            fwrite(&a32, 4, 1, f);
            fwrite((const void *)a, 1, 4096, f);
            ++pages;
        }
    }
    fclose(f);
    fclose(f_apb);
    printf("band frame %u closed: %u APB writes, %u memory pages (data/bss %lx..%lx), mock time %u us\n",
           seq, nwr, pages, (unsigned long)lo, (unsigned long)hi, mock_t / 100u);
    exit(0);
}

static void on_endb(uint32_t seq) { if (seq >= stop_seq) dump_and_exit(seq); }
static void mock_tick(uint32_t dt)
{
    static uint32_t last_vs;
    mock_t += dt;
    while (mock_t - last_vs >= 1666667u) {
        last_vs += 1666667u;
        ++mock_hdmi[4];
        if ((mock_hdmi[0] & 1u) && mock_hdmi[1] != mock_hdmi[2]) mock_hdmi[2] = mock_hdmi[1];
    }
    mock_hdmi[3] = mock_hdmi[1] != mock_hdmi[2];
    if (mock_t > 100000u * 60000u) { printf("no band frame within 60 s of mock time\n"); exit(1); }
}

#define BLIT_MODEL_PTR(a) ((uint8_t *)(uintptr_t)(a))
#define BLIT_MODEL_TICK() mock_tick(30u)
#define BLIT_MODEL_ON_ENDB(seq) on_endb(seq)
#define blit_model_wr blit_model_wr_real
#define blit_model_rd blit_model_rd_real
#include "blit_model.h"
#undef blit_model_wr
#undef blit_model_rd

uint32_t blit_model_rd(uint32_t off)
{
    if ((off & 0xFFCu) == 0x0Cu) return 0xFF0A0904u;      /* board CAPS (wide path, 16-entry queue) */
    return blit_model_rd_real(off);
}
void blit_model_wr(uint32_t off, uint32_t v)
{
    if (f_apb) { fprintf(f_apb, "%x %x\n", off & 0xFFCu, v); ++nwr; }
    blit_model_wr_real(off, v);
}

#define HDMI_FB_BASE ((uintptr_t)mock_hdmi)
#define main game_main
#include "../src/main.c"
#undef main

void bsp_init(void) { }
void bsp_printf(const char *fmt, ...)
{
    char line[512];
    va_list ap;
    va_start(ap, fmt);
    mini_vsnprintf(line, sizeof line, fmt, ap);
    va_end(ap);
    fputs(line, stdout);
}
uint32_t clint_getTimeLow(int clint) { (void)clint; mock_tick(2000u); return mock_t; }
int uart_readOccupancy(int uart) { (void)uart; return 0; }
char uart_read(int uart) { (void)uart; return 0; }
int uart_writeAvailability(int uart) { (void)uart; return 16; }
void uart_write(int uart, char c) { (void)uart; (void)c; }

int main(int argc, char **argv)
{
    char name[512];
    void *m = mmap((void *)(uintptr_t)0x01000000u, 0x01000000u, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    if (m != (void *)(uintptr_t)0x01000000u) { perror("mmap"); return 2; }
    if ((uintptr_t)&_end > 0xFFFFFFFFu) { printf("need -no-pie\n"); return 2; }
    out_base = argc > 1 ? argv[1] : "frame1";
    if (argc > 2) stop_seq = (uint32_t)atoi(argv[2]);
    snprintf(name, sizeof name, "%s.apb", out_base);
    f_apb = fopen(name, "w");
    mock_hdmi[5] = (480u << 16) | 640u;
    mock_hdmi[7] = 0x48444D49u;
    blit_model_reset();
    return game_main();
}
