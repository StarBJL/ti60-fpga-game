/*
 * Host run of the real main loop (src/main.c) against the engine model, to check
 * the frame pipeline of band mode (D) without a board:
 *   - mock clock (advances on every clint / engine register read), mock HDMI
 *     (flip latched every 1/60 s), UART keys injected from a script;
 *   - the engine model finishes a band frame some time after its ENDB (0.3..6 ms,
 *     changing), so the CPU really records the next frame while one is "drawn";
 *   - checks: a flip is only requested for a frame that is finished; nothing is
 *     drawn into the buffer on screen or waiting for its flip; no engine hang
 *     (bm_fault) / lerr / stuck dump; every mode switch keeps working.
 *
 *   gcc -O2 -w -no-pie -fno-pie -DBLIT_HOST_MODEL -DSPROUT_STATS=1 -DBOOT_DIAG=0 \
 *       -Itools/mock -Isrc -Itools -o host_main_test tools/host_main_test.c \
 *       src/game.c src/gfx.c src/render.c src/demo.c src/gfx_hw.c src/blit.c src/mini_snprintf.c src/pad.c \
 *       src/th_tab.c src/th_game.c src/th_pattern.c src/th_stage2.c src/th_art.c src/th_render.c src/th_pix.c \
 *       src/audio.c src/th_sound.c
 *   ./host_main_test [uart.log]        (PASS / FAIL; -DSPROUT_STATS=0 for the release loop)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <sys/mman.h>

#define FB_N      4
#define FB_SLOT(a) (((a) - 0x01000000u) >> 21)       /* 0x01000000 / 0120 / 0140 / 01C0 -> 0 1 2 6 */
static uint32_t mock_t;                              /* CLINT ticks (100 MHz) */
static uint32_t mock_hdmi[8];                        /* CTRL FB_ADDR FB_CUR STATUS FRAME_CNT RES UF ID */
uint32_t mock_ocr[0x400];
static uint32_t last_vs, hd_frames, last_req_seen, flips, latches;
static uint32_t errors, stuck_dumps;
static FILE *ulog;

/* engine timing: band frame seq is finished at ready[seq] */
#define RQ 4096u
static uint32_t ready[RQ], vis, eng_free, eng_dur = 250000u, frames_closed;
static uint32_t fb_seq[8];                           /* last band frame drawn into each FB slot */
static uint32_t rnd_s = 2463534242u;
static uint32_t rnd(void) { rnd_s ^= rnd_s << 13; rnd_s ^= rnd_s >> 17; rnd_s ^= rnd_s << 5; return rnd_s; }

static void mock_tick(uint32_t dt);
static uint32_t frames_done(uint32_t n)
{
    while (vis != n && (int32_t)(mock_t - ready[(vis + 1u) % RQ]) >= 0) ++vis;
    return vis;
}
static void fail(const char *what, uint32_t a)
{
    if (++errors <= 20) printf("ERROR at %u us: %s (%x)\n", mock_t / 100u, what, a);
}
static int fb_busy(uint32_t dst)                     /* dst inside the buffer on screen / waiting for its flip */
{
    if (!(mock_hdmi[0] & 1u)) return 0;
    const uint32_t fbb = (mock_hdmi[5] >> 16) * (mock_hdmi[5] & 0xFFFFu) * 2u;   /* bytes of one framebuffer */
    return (dst - mock_hdmi[2] < fbb) || (dst - mock_hdmi[1] < fbb);
}
static void on_bands(uint32_t fb)
{
    if (fb_busy(fb)) fail("band frame into the buffer on screen / queued for a flip", fb);
    fb_seq[FB_SLOT(fb) & 7u] = frames_closed + 1u;
}
static void on_endb(uint32_t seq)
{
    uint32_t start = (int32_t)(eng_free - mock_t) > 0 ? eng_free : mock_t;
    frames_closed = seq;
    ready[seq % RQ] = start + eng_dur / 2u + rnd() % eng_dur;
    eng_free = ready[seq % RQ];
}
static void on_cmd(uint32_t dst)
{
    if (dst - 0x01000000u < 0x00E00000u && fb_busy(dst)) fail("direct draw into the buffer on screen / queued", dst);
}

#define BLIT_MODEL_PTR(a) ((uint8_t *)(uintptr_t)(a))
#define BLIT_MODEL_TICK() mock_tick(30u)
#define BLIT_MODEL_FRAMES_DONE(n) frames_done(n)
#define BLIT_MODEL_ON_BANDS(fb) on_bands(fb)
#define BLIT_MODEL_ON_ENDB(seq) on_endb(seq)
#define BLIT_MODEL_ON_CMD(dst) on_cmd(dst)
#include "blit_model.h"

#define HDMI_FB_BASE ((uintptr_t)mock_hdmi)
#define main game_main
#include "../../PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/main.c"
#undef main

/* ---- key script: {time in ms, key} ---------------------------------------- */
static const struct { uint32_t ms; unsigned char c; } script[] = {
    {  300, 'v' },                          /* vsync off: free run, band + pipeline */
    /* pc_pad.py state packets (0xA5, b0, b1, chk): LEFT held, then a bad checksum,
       then LEFT + SHOT; the pad stops later (timeout = released) */
    {  500, 0xA5 }, {  500, 0x04 }, {  500, 0x00 }, {  500, 0x5E },
    {  520, 0xA5 }, {  520, 0x04 }, {  520, 0x00 }, {  520, 0x11 },
    {  540, 0xA5 }, {  540, 0x14 }, {  540, 0x00 }, {  540, 0x4E },
    {  700, 'l' }, { 760, '3' },            /* stress tier 3: thousands of seeds */
    { 1100, '#' }, { 1300, '#' },           /* danmaku 720p native -> 640x480 -> back (720p bitstream) */
    { 1500, '5' }, { 1900, '5' },           /* band off / on */
    { 2300, 'q' }, { 2700, 'q' },           /* serial / overlap order */
    { 3000, 'g' }, { 3300, 'g' },           /* CPU backend and back */
#if SPROUT_STATS
    { 3600, '8' }, { 3900, 'y' },           /* profiles (switch band off inside) */
    { 4200, 'k' }, { 4400, 'k' }, { 4600, 'k' }, { 4800, 'k' },   /* calls / list / hwlist / hwword */
    { 5200, '4' }, { 5500, '4' },           /* K path off / on */
    { 5700, '0' }, { 5900, '0' },           /* source cache */
#endif
    { 5950, '-' }, { 5980, '+' }, { 6000, '*' }, { 6020, '*' },   /* danmaku stress level / AUTO (scene TH) */
    { 6100, 'n' }, { 6110, '4' }, { 6150, 'J' }, { 6500, 'n' }, { 6520, ',' }, { 6540, '.' }, { 6900, 'n' },   /* squares, images, game */
    { 7300, 'v' },                          /* vsync on */
    { 7800, 'e' }, { 8000, 'e' },
};
#define NSCRIPT (sizeof script / sizeof script[0])
#define END_MS 8600u
static unsigned script_i;
static uint32_t piped_polls, polls, band_iters, max_pend_age, pad_msgs;

static void finish(void)
{
    printf("mock time %u ms: hdmi frames %u, flip requests %u (latched %u), band frames %u (model %u, driver seq %u, splits %u)\n",
           mock_t / 100000u, hd_frames, flips, latches, frames_closed, bm_frames, blit_st.bseq, blit_st.bsplits);
    printf("loop polls %u, with a frame still in the engine %u (%u%%); stuck dumps %u, model fault %u lerr %u berr %u\n",
           polls, piped_polls, polls ? piped_polls * 100u / polls : 0u, stuck_dumps, bm_fault, bm_lerr, bm_berr);
    if (bm_fault || bm_lerr || bm_berr || stuck_dumps) ++errors;
    /* the default script runs band mode with vsync off for several seconds */
    if (!frames_closed || (BAND_DEFAULT && VSYNC_DEFAULT && !piped_polls)) { printf("band pipeline never used\n"); ++errors; }
    if (script_i != NSCRIPT) { printf("script not finished\n"); ++errors; }
    printf("pad: %u packets, %u bad (want 2 / 1), autoplay switched off by the pad: %u\n",
           pad_packets, pad_errors, pad_msgs);
    if (pad_packets != 2u || pad_errors != 1u || !pad_msgs) ++errors;
    assert(game.skill_casts>0);
    printf("R3 skill casts %d; native scenes and controls exercised\n",game.skill_casts);
    printf(errors ? "FAIL (%u)\n" : "PASS\n", errors);
    if (ulog) fclose(ulog);
    exit(errors ? 1 : 0);
}

/* HOST_DUMP=prefix: the buffer on screen as <prefix><ms>.ppm at these times */
static const uint32_t dump_ms[] = { 650, 1050, 1250, 1450, 6050, 6200 };
static unsigned dump_i;
static void dump_screen(uint32_t ms)
{
    const char *pre = getenv("HOST_DUMP");
    const uint32_t w = mock_hdmi[5] & 0xFFFFu, h = mock_hdmi[5] >> 16;
    const uint16_t *px = (const uint16_t *)(uintptr_t)mock_hdmi[2];
    char name[256];
    FILE *f;
    uint32_t i;
    if (!pre || !(mock_hdmi[0] & 1u)) return;
    snprintf(name, sizeof name, "%s%u.ppm", pre, ms);
    if (!(f = fopen(name, "wb"))) return;
    fprintf(f, "P6\n%u %u\n255\n", w, h);
    for (i = 0; i < w * h; i++) {
        const uint16_t c = px[i];
        fputc((c >> 11) << 3, f); fputc(((c >> 5) & 63) << 2, f); fputc((c & 31) << 3, f);
    }
    fclose(f);
}

static void mock_tick(uint32_t dt)
{
    mock_t += dt;
    if (dump_i < sizeof dump_ms / sizeof dump_ms[0] && mock_t / 100000u >= dump_ms[dump_i]) dump_screen(dump_ms[dump_i++]);
    while (mock_t - last_vs >= 1666667u) {           /* vsync: latch the requested buffer */
        last_vs += 1666667u;
        ++hd_frames;
        if ((mock_hdmi[0] & 1u) && mock_hdmi[1] != mock_hdmi[2]) { mock_hdmi[2] = mock_hdmi[1]; ++latches; }
    }
    if (mock_hdmi[1] != last_req_seen) {             /* a flip was requested */
        const uint32_t fb = mock_hdmi[1];
        last_req_seen = fb;
        ++flips;
        if (fb_seq[FB_SLOT(fb) & 7u] - frames_done(frames_closed) - 1u < 0x7FFFFFFFu)   /* seq > done */
            fail("flip requested before its band frame was finished", fb);
    }
    mock_hdmi[3] = mock_hdmi[1] != mock_hdmi[2];
    mock_hdmi[4] = hd_frames;
    /* engine speed changes: CPU-bound, about equal, engine-bound */
    eng_dur = ((mock_t / 40000000u) % 3u == 0u) ? 30000u : ((mock_t / 40000000u) % 3u == 1u) ? 250000u : 600000u;
    if (mock_t / 100000u >= END_MS) finish();
}

void bsp_init(void) { }
void bsp_printf(const char *fmt, ...)
{
    char line[512];
    va_list ap;
    va_start(ap, fmt);
    mini_vsnprintf(line, sizeof line, fmt, ap);
    va_end(ap);
    if (strstr(line, "stuck") || strstr(line, "!!")) ++stuck_dumps;
    if (ulog) fputs(line, ulog);
}
uint32_t clint_getTimeLow(int clint) { (void)clint; mock_tick(2000u); return mock_t; }
int uart_readOccupancy(int uart)
{
    (void)uart;
    ++polls;
    if (pend_fb) ++piped_polls;
    return script_i < NSCRIPT && mock_t / 100000u >= script[script_i].ms;
}
char uart_read(int uart) { (void)uart; return (char)script[script_i++].c; }
int uart_writeAvailability(int uart) { (void)uart; return 16; }
void uart_write(int uart, char c)
{
    static char line[512];
    static unsigned n;
    (void)uart;
    if (ulog) fputc(c, ulog);
    if (n < sizeof line - 1u) line[n++] = c;
    if (c == '\n') {
        line[n] = 0;
        if (strstr(line, "stuck") || strstr(line, "!!")) ++stuck_dumps;
        if (strstr(line, "autoplay OFF: pad")) ++pad_msgs;
        n = 0;
    }
}

int main(int argc, char **argv)
{
    void *m = mmap((void *)(uintptr_t)0x01000000u, 0x01000000u, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    if (m != (void *)(uintptr_t)0x01000000u) { perror("mmap"); return 2; }
    if ((uintptr_t)&game > 0xFFFFFFFFu) { printf("need -no-pie: statics above 4 GB\n"); return 2; }
    ulog = argc > 1 ? fopen(argv[1], "w") : 0;
#ifndef HOST_RES                                    /* -DHOST_RES=0x02D00500u: 1280x720 bitstream */
#define HOST_RES ((480u << 16) | 640u)
#endif
    mock_hdmi[5] = HOST_RES;
    mock_hdmi[7] = 0x48444D49u;
    assert(mmap((void*)0xf8101000u,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED,-1,0)==(void*)0xf8101000u);
    blit_model_reset();
    scene=SCENE_GAME;autoplay=1;key_pressed=0;handle_char('j');
    assert(!autoplay&&(key_pressed&KEY_SKILL));
#if SPROUT_STATS
    {int was=render_sort_keys;handle_char('~');assert(render_sort_keys!=was);}
#endif
    scene=SCENE_TH;autoplay=AUTOPLAY_DEFAULT;key_pressed=0;
    return game_main();
}
