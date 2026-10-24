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
static uint32_t mock_hdmi[64];                        /* CTRL FB_ADDR FB_CUR STATUS FRAME_CNT RES UF ID */
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
    const uint32_t fbb = (mock_hdmi[0x58/4] >> 16) * (mock_hdmi[0x58/4] & 0xFFFFu) * 2u;   /* bytes of one framebuffer */
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
    {300,'v'},
    {500,0xA5},
    {500,0x04},
    {500,0x00},
    {500,0x5E},
    {520,0xA5},
    {520,0x04},
    {520,0x00},
    {520,0x11},
    {540,0xA5},
    {540,0x14},
    {540,0x00},
    {540,0x4E},
    {700,'l'},
    {760,'3'},
    {1100,'#'},
    {1300,'#'},
    {1500,'5'},
    {1900,'5'},
    {2300,'q'},
    {2700,'q'},
    {3000,'g'},
    {3300,'g'},
    {3600,'8'},
    {3900,'y'},
    {4200,'k'},
    {4400,'k'},
    {4600,'k'},
    {4800,'k'},
    {5200,'4'},
    {5500,'4'},
    {5700,'0'},
    {5900,'0'},
    {5950,'-'},
    {5980,'+'},
    {6000,'*'},
    {6020,'*'},
    {6100,'n'},
    {6110,'4'},
    {6150,'J'},
    {6200,58},
    {6200,49},
    {6200,48},
    {6200,49},
    {6200,32},
    {6200,115},
    {6200,116},
    {6200,101},
    {6200,112},
    {6200,32},
    {6200,49},
    {6200,10},
    {6220,58},
    {6220,49},
    {6220,48},
    {6220,50},
    {6220,32},
    {6220,99},
    {6220,111},
    {6220,117},
    {6220,110},
    {6220,116},
    {6220,32},
    {6220,49},
    {6220,52},
    {6220,48},
    {6220,48},
    {6220,48},
    {6220,10},
    {6240,58},
    {6240,49},
    {6240,48},
    {6240,51},
    {6240,32},
    {6240,100},
    {6240,101},
    {6240,108},
    {6240,116},
    {6240,97},
    {6240,32},
    {6240,49},
    {6240,10},
    {6260,58},
    {6260,49},
    {6260,48},
    {6260,52},
    {6260,32},
    {6260,115},
    {6260,116},
    {6260,101},
    {6260,112},
    {6260,32},
    {6260,49},
    {6260,48},
    {6260,10},
    {6280,58},
    {6280,49},
    {6280,48},
    {6280,53},
    {6280,32},
    {6280,100},
    {6280,101},
    {6280,108},
    {6280,116},
    {6280,97},
    {6280,32},
    {6280,49},
    {6280,10},
    {6300,58},
    {6300,49},
    {6300,48},
    {6300,54},
    {6300,32},
    {6300,98},
    {6300,117},
    {6300,108},
    {6300,108},
    {6300,101},
    {6300,116},
    {6300,32},
    {6300,50},
    {6300,53},
    {6300,10},
    {6340,58},
    {6340,49},
    {6340,48},
    {6340,55},
    {6340,32},
    {6340,98},
    {6340,117},
    {6340,108},
    {6340,108},
    {6340,101},
    {6340,116},
    {6340,32},
    {6340,49},
    {6340,51},
    {6340,10},
    {6500,'n'},
    {6520,','},
    {6540,'.'},
    {6600,58},
    {6600,49},
    {6600,48},
    {6600,56},
    {6600,32},
    {6600,115},
    {6600,113},
    {6600,117},
    {6600,97},
    {6600,114},
    {6600,101},
    {6600,32},
    {6600,56},
    {6600,10},
    {6640,58},
    {6640,49},
    {6640,48},
    {6640,57},
    {6640,32},
    {6640,99},
    {6640,111},
    {6640,117},
    {6640,110},
    {6640,116},
    {6640,32},
    {6640,49},
    {6640,48},
    {6640,48},
    {6640,48},
    {6640,10},
    {6680,58},
    {6680,49},
    {6680,49},
    {6680,48},
    {6680,32},
    {6680,115},
    {6680,116},
    {6680,101},
    {6680,112},
    {6680,32},
    {6680,49},
    {6680,48},
    {6680,48},
    {6680,10},
    {6720,58},
    {6720,49},
    {6720,49},
    {6720,49},
    {6720,32},
    {6720,100},
    {6720,101},
    {6720,108},
    {6720,116},
    {6720,97},
    {6720,32},
    {6720,49},
    {6720,10},
    {6760,58},
    {6760,49},
    {6760,49},
    {6760,50},
    {6760,32},
    {6760,115},
    {6760,113},
    {6760,117},
    {6760,97},
    {6760,114},
    {6760,101},
    {6760,32},
    {6760,48},
    {6760,10},
    {6900,'n'},
    {7000,58},
    {7000,49},
    {7000,49},
    {7000,51},
    {7000,32},
    {7000,110},
    {7000,101},
    {7000,120},
    {7000,116},
    {7000,10},
    {7100,58},
    {7100,49},
    {7100,49},
    {7100,52},
    {7100,32},
    {7100,98},
    {7100,103},
    {7100,109},
    {7100,32},
    {7100,48},
    {7100,10},
    {7200,58},
    {7200,49},
    {7200,49},
    {7200,53},
    {7200,32},
    {7200,98},
    {7200,103},
    {7200,109},
    {7200,32},
    {7200,49},
    {7200,10},
    {7300,'v'},
    {7800,'e'},
    {8000,'e'},
    {8100,58},
    {8100,50},
    {8100,48},
    {8100,49},
    {8100,32},
    {8100,114},
    {8100,101},
    {8100,115},
    {8100,111},
    {8100,108},
    {8100,117},
    {8100,116},
    {8100,105},
    {8100,111},
    {8100,110},
    {8100,32},
    {8100,49},
    {8100,10},
    {9300,58},
    {9300,50},
    {9300,48},
    {9300,50},
    {9300,32},
    {9300,114},
    {9300,101},
    {9300,115},
    {9300,111},
    {9300,108},
    {9300,117},
    {9300,116},
    {9300,105},
    {9300,111},
    {9300,110},
    {9300,32},
    {9300,48},
    {9300,10},
    {9600,58},
    {9600,50},
    {9600,48},
    {9600,51},
    {9600,32},
    {9600,114},
    {9600,101},
    {9600,115},
    {9600,111},
    {9600,108},
    {9600,117},
    {9600,116},
    {9600,105},
    {9600,111},
    {9600,110},
    {9600,32},
    {9600,49},
    {9600,10},
    {10000,58},
    {10000,50},
    {10000,48},
    {10000,52},
    {10000,32},
    {10000,114},
    {10000,101},
    {10000,115},
    {10000,111},
    {10000,108},
    {10000,117},
    {10000,116},
    {10000,105},
    {10000,111},
    {10000,110},
    {10000,32},
    {10000,48},
    {10000,10},

};
#define NSCRIPT (sizeof script / sizeof script[0])
#define END_MS 11000u
static unsigned script_i,video_active,video_frames[2];
static uint32_t piped_polls, polls, band_iters, max_pend_age, pad_msgs;

static uint32_t bot_steer_reference(const Game *g, uint32_t frame)
{
    static const int8_t wander[8][2] = {
        {1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1} };
    int px = g->x / FIX, py = g->y / FIX, i, slots;
    int32_t fx = 0, fy = 0;
    int best = 220 * 220, gx = 0, gy = 0, have_gem = 0;
    uint32_t held = 0;

    for (i = 0; i < MAX_ENEMIES; i++) {
        const Enemy *e = &g->enemies[i];
        int dx, dy, d2;
        if (!e->active) continue;
        dx = px - e->x / FIX; dy = py - e->y / FIX;
        if (iabs(dx) > 150 || iabs(dy) > 150) continue;
        d2 = dx * dx + dy * dy;
        if (d2 > 150 * 150) continue;
        if (d2 < 64) d2 = 64;
        {
            int w = e->kind == 3 ? 3 : 1;
            fx += (dx * 65536 / d2) * w;
            fy += (dy * 65536 / d2) * w;
        }
    }
    slots = game_hostile_used(g);                    /* slots above are all free */
    for (i = 0; i < slots; i++) {
        const Bullet *b = game_hostile_at(g, 0) + i;   /* contiguous pool */
        int dx, dy, d2;
        if (!b || !b->active) continue;
        dx = px - b->x / FIX; dy = py - b->y / FIX;
        if (iabs(dx) > 110 || iabs(dy) > 110) continue;
        d2 = dx * dx + dy * dy;
        if (d2 > 110 * 110) continue;
        if (d2 < 64) d2 = 64;
        fx += (dx * 65536 / d2) * 3;
        fy += (dy * 65536 / d2) * 3;
    }
    for (i = 0; i < MAX_GEMS; i++) {
        const Gem *m = &g->gems[i];
        int dx, dy, d2;
        if (!m->active) continue;
        dx = m->x / FIX - px; dy = m->y / FIX - py;
        if (iabs(dx) > 220 || iabs(dy) > 220) continue;
        d2 = dx * dx + dy * dy;
        if (d2 < best) { best = d2; gx = dx; gy = dy; have_gem = 1; }
    }
    if (have_gem) {
        int n = iabs(gx) + iabs(gy) + 1;
        fx += gx * 700 / n;
        fy += gy * 700 / n;
    }
    /* wander + centre pull + walls */
    {
        const int8_t *w = wander[(frame / 90u) & 7u];
        fx += w[0] * 350; fy += w[1] * 350;
    }
    fx += (640 - px) * 3;
    fy += (378 - py) * 3;
    if (px < 105)  fx += (105 - px) * 40;
    if (px > 1175) fx -= (px - 1175) * 40;
    if (py < 180) fy += (180 - py) * 40;
    if (py > 577) fy -= (py - 577) * 40;

    if (fx >  300) held |= KEY_RIGHT;
    if (fx < -300) held |= KEY_LEFT;
    if (fy >  300) held |= KEY_DOWN;
    if (fy < -300) held |= KEY_UP;
    return held;
}

static int finishing;
static void finish(void)
{
    finishing=1;
    int old_x=game.x,old_y=game.y;
    for(unsigned trial=0;trial<64;trial++){
        game.x=(100+(trial*173)%1080)*FIX;game.y=(180+(trial*59)%400)*FIX;
        assert(bot_steer(&game,trial*97)==bot_steer_reference(&game,trial*97));
    }
    game.x=old_x;game.y=old_y;
    puts("PASS autoplay optimization: exact held-key equality to previous algorithm over 64 positions/frames");
    int keep_scene=scene,keep_alpha=alpha_fx,keep_vsync=vsync;
    int keep_bullet=game_seed_size(),keep_square=demo_square_size();
    int keep_sq=sq_count,keep_cap=game.stress_bullet_target;
    blit_wait();band_want=0;set_seed_mode(0);blit_set_cache(0);blit_band_set_koff(1);
    overlap=0;render_seed_table=0;render_sort_keys=0;blit_st.has_table=0;
    if(demo_table_enabled())demo_table_toggle();
    ControlEvent restore={CT_FAST,0,999,0};
    assert(control_apply(restore));
    assert(use_hw&&seed_mode()==2&&blit_st.band&&blit_st.cflag&&!blit_st.band_koff&&overlap);
    assert(render_seed_table&&render_sort_keys&&blit_st.has_table&&demo_table_enabled());
    assert(scene==keep_scene&&alpha_fx==keep_alpha&&vsync==keep_vsync);
    assert(game_seed_size()==keep_bullet&&demo_square_size()==keep_square&&sq_count==keep_sq&&game.stress_bullet_target==keep_cap);
    ControlEvent bars={CT_BARS,1,1000,1};assert(control_apply(bars));assert(HDMI_CTRL&HDMI_CTRL_TEST);
    bars.value=0;assert(control_apply(bars));assert(!(HDMI_CTRL&HDMI_CTRL_TEST));
    bars.value=2;assert(!control_apply(bars));
    ControlEvent volstep={CT_VOLUME_DELTA,-1,1001,1};aud_master(256);
    for(int q=0;q<10;q++)assert(control_apply(volstep));assert(aud.master==128);
    volstep.value=1;for(int q=0;q<10;q++)assert(control_apply(volstep));assert(aud.master==256);
    assert(control_apply(volstep)&&aud.master==256);
    volstep.value=-1;for(int q=0;q<21;q++)assert(control_apply(volstep));assert(aud.master==0);
    volstep.value=2;assert(!control_apply(volstep));
    puts("PASS volume +/- 5 percent, rapid relative steps, zero and full clamps");
    puts("PASS real fast dispatcher restores accelerated path, preserves workload and gameplay; bars control validated");
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
    assert(game.skill_casts==0&&game.player_active==0); /* pressure suppresses player volleys */assert(game.stress_bullet_target==14011);assert(sq_count==1100);assert(count_step==100);assert(demo_square_size()==0);
    printf("R3 skill casts %d; native scenes and controls exercised\n",game.skill_casts);
    printf("video model fullframes 720=%u 1080=%u; render canvas=%ux%u\n",video_frames[0],video_frames[1],disp_w,disp_h);
    assert(video_frames[0]>50 && video_frames[1]>20 && disp_w==1280 && disp_h==720);
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
    while (mock_t - last_vs >= (video_active?3333334u:1666667u)) {           /* vsync: latch the requested buffer */
        last_vs += video_active?3333334u:1666667u;
        video_active=mock_hdmi[0x50/4]&1u; ++video_frames[video_active];
        mock_hdmi[5]=video_active?0x04380780u:HOST_RES;
        ++hd_frames;
        if(mock_hdmi[0]&1u){++mock_hdmi[0x48/4];if(mock_hdmi[1]!=mock_hdmi[2])++mock_hdmi[0x40/4];else ++mock_hdmi[0x44/4];}
        if ((mock_hdmi[0] & 1u) && mock_hdmi[1] != mock_hdmi[2]) { mock_hdmi[2] = mock_hdmi[1]; ++latches; }
    }
    if (mock_hdmi[1] != last_req_seen) {             /* a flip was requested */
        const uint32_t fb = mock_hdmi[1];
        last_req_seen = fb;
        ++flips;
        if (fb_seq[FB_SLOT(fb) & 7u] - frames_done(frames_closed) - 1u < 0x7FFFFFFFu)   /* seq > done */
            fail("flip requested before its band frame was finished", fb);
    }
    mock_hdmi[0x50/4]=(mock_hdmi[0x50/4]&1u)|(video_active<<1)|(((mock_hdmi[0x50/4]&1u)!=video_active)?4u:0u);
    mock_hdmi[3] = mock_hdmi[1] != mock_hdmi[2];
    mock_hdmi[4] = hd_frames;
    /* engine speed changes: CPU-bound, about equal, engine-bound */
    eng_dur = ((mock_t / 40000000u) % 3u == 0u) ? 30000u : ((mock_t / 40000000u) % 3u == 1u) ? 250000u : 600000u;
    if (mock_t / 100000u >= END_MS && !finishing) finish();
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
    mock_hdmi[5] = HOST_RES;mock_hdmi[0x58/4]=HOST_RES;mock_hdmi[0x54/4]=0x56494435u;
    mock_hdmi[7] = 0x48444D49u;mock_hdmi[0x4c/4]=0x50525332u;
    assert(mmap((void*)0xf8101000u,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED,-1,0)==(void*)0xf8101000u);
    blit_model_reset();
    {WinStats check={0};check.bp[BLIT_BP_P1]=123*BLIT_CLK_MHZ;check.bp[BLIT_BP_BAND]=456*BLIT_CLK_MHZ;
     print_telemetry(&check);assert(strstr(tx_buf,"phase1_us=123 bands_us=456"));
     memset(tx_buf,0,sizeof tx_buf);tx_head=tx_tail=0;telemetry_seq=0;}
    scene=SCENE_GAME;autoplay=1;key_pressed=0;handle_char('j');
    assert(!autoplay&&(key_pressed&KEY_SKILL));
#if SPROUT_STATS
    {int was=render_sort_keys;handle_char('~');assert(render_sort_keys!=was);}
#endif
    scene=SCENE_TH;autoplay=AUTOPLAY_DEFAULT;key_pressed=0;
    return game_main();
}
