/*
 * SproutRun on Sapphire SoC (Ti60F225) - CPU or BitBlt rendering to HDMI.
 *
 *   game.c / render.c / gfx.c : the portable game (unchanged logic)
 *   gfx.c CPU backend         : RISC-V draws every pixel into a DDR framebuffer
 *   gfx_hw.c + blit.h         : the same gfx calls as commands for the BitBlt
 *                               engine (rtl/blit), which draws by AXI burst DMA
 *   demo.c                    : BitBlt demos (moving squares, image switching)
 *   hdmi_fb (RTL)             : scans the framebuffer out as 640x480@60 HDMI
 *
 * Three framebuffers are used so the CPU never draws into the buffer being
 * scanned out nor into the one waiting for the next vsync:
 *   vsync ON  (default) : at most one flip per vblank -> capped at 60 FPS
 *   vsync OFF ('v' key) : never wait, newest frame wins -> uncapped render FPS,
 *                         shows how fast the RISC-V alone can draw a frame
 * The game itself always advances at most one 1/60 s tick per rendered frame
 * (same rule as the Windows build: a late frame is dropped, not caught up).
 *
 * Measurements (CLINT, 100 MHz) are drawn on the HUD (0.5 s window) and printed
 * on the UART (1 s window). Every frame is split into
 *   vsync_wait : waiting until the previous flip has been latched (vsync ON)
 *   logic      : game / demo update (average per tick, and the worst tick)
 *   submit     : drawing - CPU: the pixels themselves; HW: building + queueing
 *                the BitBlt commands, without qstall
 *   qstall     : inside submit, waiting for a free command-queue slot
 *   tail_wait  : after the last command, waiting until the engine is done
 *   other      : UART, buffer selection, statistics (the remainder)
 * DRAW = submit + qstall + tail_wait. For the engine the UART also shows the
 * queue occupancy, its busy share and the real AXI read / write throughput
 * (PERF_RBEAT / PERF_WBEAT x 4 bytes). The last FPS / draw time of each backend
 * stays on screen for comparison.
 *
 * UART keys (115200 8N1):
 *   w a s d / arrows  move (latched)     x  stop          b  autoplay on/off
 *   Enter / Space     start              p  pause         r  restart
 *   1 2 3             pick upgrade/tier  l  stress menu   m  title menu
 *   f  stats panel    v  vsync on/off    t  colour bars   h  help
 *   e  alpha effects on/off (compare FPS with / without translucency)
 *   g  CPU / BitBlt backend              c  auto compare (switch every 4 s)
 *   n  scene: game -> moving squares -> image switching
 *   - / +  demo load (square count, frames per picture)
 */
#include <stdint.h>
#include <string.h>

#include "bsp.h"
#include "game.h"
#include "gfx.h"
#include "gfx_hw.h"
#include "blit.h"
#include "demo.h"
#include "render.h"
#include "hdmi_fb.h"
#include "mini_snprintf.h"

#ifndef AUTOPLAY_DEFAULT
#define AUTOPLAY_DEFAULT 1
#endif
#ifndef VSYNC_DEFAULT
#define VSYNC_DEFAULT 1
#endif
#ifndef GFX_CPU_WORD_FILL
#define GFX_CPU_WORD_FILL 1                 /* same default as gfx.c */
#endif
#ifndef FB0_ADDR
#define FB0_ADDR 0x01000000u
#endif
#ifndef FB1_ADDR
#define FB1_ADDR 0x01200000u
#endif
#ifndef FB2_ADDR
#define FB2_ADDR 0x01400000u
#endif
#ifndef IMG_ADDR
#define IMG_ADDR 0x01600000u                /* 3 demo pictures, 2 MB apart */
#endif
#ifndef HW_DEFAULT
#define HW_DEFAULT 1                        /* start with the BitBlt backend if found */
#endif
#define BLIT_CLK_MHZ  100u                  /* engine clock (io_ddrMasters_0 clock) */

#define FB_COUNT      3
#define FB_BYTES      ((uint32_t)SCREEN_W * SCREEN_H * sizeof(Pixel))
#define TICKS_PER_US  (BSP_CLINT_HZ / 1000000u)
#define TICK_PERIOD   (BSP_CLINT_HZ / 60u)          /* one game update */
#define STAT_PERIOD   (BSP_CLINT_HZ / 2u)           /* HUD refresh: 0.5 s */
#define UART_PERIOD   (BSP_CLINT_HZ)                /* UART report: 1 s */

static const uint32_t fb_addr[FB_COUNT] = { FB0_ADDR, FB1_ADDR, FB2_ADDR };

static Game game;

static inline uint32_t now(void) { return clint_getTimeLow(BSP_CLINT); }

/* ------------------------------------------------------------------------- */
/* Non-blocking UART TX: text goes into a ring and is drained a few bytes at  */
/* a time, so printing statistics never stalls the frame loop.                */
/* ------------------------------------------------------------------------- */
#define TX_SIZE 4096u
static char tx_buf[TX_SIZE];
static volatile uint32_t tx_head, tx_tail;
static uint32_t tx_dropped;                      /* lines dropped on a full ring */

static void tx_puts(const char *s)
{
    uint32_t len = (uint32_t)strlen(s);
    uint32_t used = (tx_head - tx_tail) & (TX_SIZE - 1u);
    /* whole line or nothing, so a slow frame rate never prints half lines */
    if (len >= TX_SIZE - 1u - used) { ++tx_dropped; return; }
    while (*s) {
        tx_buf[tx_head] = *s++;
        tx_head = (tx_head + 1u) & (TX_SIZE - 1u);
    }
}

static void tx_pump(void)
{
    while (tx_tail != tx_head && uart_writeAvailability(BSP_UART_TERMINAL)) {
        uart_write(BSP_UART_TERMINAL, tx_buf[tx_tail]);
        tx_tail = (tx_tail + 1u) & (TX_SIZE - 1u);
    }
}

static void tx_printf(const char *fmt, ...)
{
    char line[256];
    va_list ap;
    va_start(ap, fmt);
    mini_vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);
    tx_puts(line);
}

static void tx_flush(void)
{
    while (tx_tail != tx_head) tx_pump();
}

/* ------------------------------------------------------------------------- */
/* Input                                                                      */
/* ------------------------------------------------------------------------- */
static int      dir_x, dir_y;         /* latched manual direction */
static uint32_t key_pressed;          /* edge keys waiting for the next tick */
static int      autoplay = AUTOPLAY_DEFAULT;
static int      vsync    = VSYNC_DEFAULT;
static int      show_stats = 1;
static int      alpha_fx   = 1;
static int      test_bars;
static int      esc_state;

/* rendering backend / scene */
enum { SCENE_GAME, SCENE_SQUARES, SCENE_IMAGES, SCENE_COUNT };
static int      hw_avail;             /* BitBlt engine answered at boot */
static int      use_hw;               /* draw with the engine */
static int      auto_compare;         /* switch backend every COMPARE_WINDOWS */
static int      scene = SCENE_GAME;
static int      sq_count   = 256;     /* moving squares */
static int      img_period = 20;      /* frames per picture in the switching demo */
static int      stats_reset;          /* mode changed: restart the measurement window */
#define COMPARE_WINDOWS 8             /* x 0.5 s */

static void print_help(void)
{
    tx_puts("\r\nkeys: wasd/arrows move, x stop, Enter/Space start, p pause, r restart,\r\n"
            "      1 2 3 pick, l stress menu, m menu, f stats, b autoplay, v vsync,\r\n"
            "      t colour bars, e alpha effects, h help\r\n"
            "      g CPU/BitBlt backend, c auto compare, n next scene (game/squares/images),\r\n"
            "      - + demo load\r\n");
}

static void set_hw(int on)
{
    if (on && !hw_avail) { tx_puts("[no BitBlt engine in this bitstream: CPU only]\r\n"); return; }
    if (on == use_hw) return;
    use_hw = on;
    stats_reset = 1;
    tx_puts(use_hw ? "[backend: BitBlt engine]\r\n" : "[backend: CPU]\r\n");
}

static void change_load(int up)
{
    if (scene == SCENE_SQUARES) {
        sq_count = up ? sq_count * 2 : sq_count / 2;
        if (sq_count < 16) sq_count = 16;
        if (sq_count > DEMO_MAX_SQUARES) sq_count = DEMO_MAX_SQUARES;
        tx_printf("[squares: %d]\r\n", sq_count);
    } else if (scene == SCENE_IMAGES) {
        img_period = up ? img_period + (img_period < 5 ? 1 : 5) : img_period - (img_period <= 5 ? 1 : 5);
        if (img_period < 1) img_period = 1;
        if (img_period > 60) img_period = 60;
        tx_printf("[picture switch every %d frames]\r\n", img_period);
    }
    stats_reset = 1;
}

static void manual_move(int x, int y, int set_x, int set_y)
{
    if (set_x) dir_x = x;
    if (set_y) dir_y = y;
    if (autoplay) { autoplay = 0; tx_puts("[autoplay OFF]\r\n"); }
}

static void handle_char(char c)
{
    /* ANSI arrow keys: ESC [ A/B/C/D */
    if (esc_state == 1) { esc_state = (c == '[') ? 2 : 0; if (esc_state) return; }
    else if (esc_state == 2) {
        esc_state = 0;
        if (c == 'A') { manual_move(0, -1, 0, 1); return; }
        if (c == 'B') { manual_move(0,  1, 0, 1); return; }
        if (c == 'C') { manual_move(1,  0, 1, 0); return; }
        if (c == 'D') { manual_move(-1, 0, 1, 0); return; }
        return;
    }
    switch (c) {
    case 27:  esc_state = 1; break;
    case 'w': case 'W': manual_move(0, -1, 0, 1); break;
    case 's': case 'S': manual_move(0,  1, 0, 1); break;
    case 'a': case 'A': manual_move(-1, 0, 1, 0); break;
    case 'd': case 'D': manual_move(1,  0, 1, 0); break;
    case 'x': case 'X': dir_x = 0; dir_y = 0; break;
    case '\r': case '\n': case ' ': key_pressed |= KEY_START; break;
    case 'p': case 'P': key_pressed |= KEY_PAUSE; break;
    case 'r': case 'R': key_pressed |= KEY_RESTART; break;
    case 'm': case 'M': key_pressed |= KEY_MENU; break;
    case 'l': case 'L': key_pressed |= KEY_LIMIT; break;
    case '1': key_pressed |= KEY_PICK1; break;
    case '2': key_pressed |= KEY_PICK2; break;
    case '3': key_pressed |= KEY_PICK3; break;
    case 'f': case 'F': show_stats = !show_stats; stats_reset = 1; break;
    case 'b': case 'B':
        autoplay = !autoplay; dir_x = dir_y = 0;
        tx_puts(autoplay ? "[autoplay ON]\r\n" : "[autoplay OFF]\r\n");
        break;
    case 'v': case 'V':
        vsync = !vsync;
        stats_reset = 1;
        tx_puts(vsync ? "[vsync ON: capped at 60 FPS]\r\n"
                      : "[vsync OFF: uncapped, measures raw CPU render rate]\r\n");
        break;
    case 't': case 'T':
        test_bars = !test_bars; hdmi_test_pattern(test_bars);
        tx_puts(test_bars ? "[HDMI colour bars]\r\n" : "[HDMI framebuffer]\r\n");
        break;
    case 'e': case 'E':
        alpha_fx = !alpha_fx;
        tx_puts(alpha_fx ? "[alpha effects ON: shadows, dim, panels, ghost]\r\n"
                         : "[alpha effects OFF: original opaque drawing]\r\n");
        stats_reset = 1;
        break;
    case 'g': case 'G': set_hw(!use_hw); break;
    case 'c': case 'C':
        if (!hw_avail) { set_hw(1); break; }
        auto_compare = !auto_compare;
        tx_puts(auto_compare ? "[auto compare ON: CPU / BitBlt every 4 s]\r\n"
                             : "[auto compare OFF]\r\n");
        break;
    case 'n': case 'N':
        scene = (scene + 1) % SCENE_COUNT;
        stats_reset = 1;
        tx_puts(scene == SCENE_GAME ? "[scene: game]\r\n" :
                scene == SCENE_SQUARES ? "[scene: moving squares]\r\n" : "[scene: image switching]\r\n");
        break;
    case '-': case '_': case '[': change_load(0); break;
    case '+': case '=': case ']': change_load(1); break;
    case 'h': case 'H': case '?': print_help(); break;
    default: break;
    }
}

static void poll_uart(void)
{
    while (uart_readOccupancy(BSP_UART_TERMINAL))
        handle_char(uart_read(BSP_UART_TERMINAL));
}

/* ------------------------------------------------------------------------- */
/* Autoplay bot: keeps the demo running unattended.                           */
/* Steers away from nearby enemies and red seeds, drifts towards gems and the */
/* arena centre, and handles the menus.                                       */
/* ------------------------------------------------------------------------- */
static Phase bot_phase = TITLE;
static int   bot_wait;

static int iabs(int v) { return v < 0 ? -v : v; }

static uint32_t bot_steer(const Game *g, uint32_t frame)
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
    slots = game_hostile_slots(g);
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
    fx += (320 - px) * 3;
    fy += (252 - py) * 3;
    if (px < 70)  fx += (70 - px) * 40;
    if (px > 570) fx -= (px - 570) * 40;
    if (py < 120) fy += (120 - py) * 40;
    if (py > 385) fy -= (py - 385) * 40;

    if (fx >  300) held |= KEY_RIGHT;
    if (fx < -300) held |= KEY_LEFT;
    if (fy >  300) held |= KEY_DOWN;
    if (fy < -300) held |= KEY_UP;
    return held;
}

static Input bot_input(const Game *g, uint32_t frame)
{
    Input in; in.held = 0; in.pressed = 0;
    if (g->phase != bot_phase) { bot_phase = g->phase; bot_wait = 0; }
    ++bot_wait;
    switch (g->phase) {
    case TITLE:         if (bot_wait == 90)  in.pressed = KEY_START; break;
    case STRESS_SELECT: if (bot_wait == 60)  in.pressed = KEY_PICK1; break;
    case UPGRADE:       if (bot_wait == 45)  in.pressed = KEY_PICK1 << (g->level % 3); break;
    case DEFEAT:        if (bot_wait == 180) in.pressed = KEY_START; break;
    case PLAYING:       in.held = bot_steer(g, frame); break;
    default: break;
    }
    if (bot_wait > 400) bot_wait = 0;            /* retry if a key got lost */
    return in;
}

static Input manual_input(void)
{
    Input in;
    in.held = 0;
    if (dir_x < 0) in.held |= KEY_LEFT;
    if (dir_x > 0) in.held |= KEY_RIGHT;
    if (dir_y < 0) in.held |= KEY_UP;
    if (dir_y > 0) in.held |= KEY_DOWN;
    in.pressed = 0;
    return in;
}

/* ------------------------------------------------------------------------- */
/* Boot-time benchmark: the same gfx calls on the CPU and on the BitBlt engine */
/* ------------------------------------------------------------------------- */
#define BENCH_N 4
static const char *const bench_name[BENCH_N] = {
    "full-screen fill ", "full-screen copy ", "full-screen alpha", "2000 x 8x8 fills " };
static const uint32_t bench_bytes[BENCH_N] = {
    FB_BYTES, FB_BYTES, FB_BYTES, 2000u * 8u * 8u * 2u };

static void begin_target(Gfx *gfx, GfxHwTarget *t, int hw, uint32_t fb)
{
    if (hw) gfx_begin_hw(gfx, t, fb, SCREEN_W, SCREEN_H);
    else    gfx_begin(gfx, (Pixel *)fb, SCREEN_W, SCREEN_H);
}

static uint32_t bench_end(int hw, uint32_t t0)
{
    uint32_t us;
    if (hw) blit_wait();                      /* HW time = queue + execution */
    us = (now() - t0) / TICKS_PER_US;
    return us ? us : 1u;
}

static void bandwidth_test(int hw, uint32_t fb, uint32_t us[BENCH_N])
{
    Gfx gfx;
    GfxHwTarget tgt;
    Sprite src;
    uint32_t t0;
    int i;

    src.width = SCREEN_W; src.height = SCREEN_H;
    src.pixels = (const Pixel *)fb_addr[2];
    src.alpha = 0;
    begin_target(&gfx, &tgt, hw, fb);

    t0 = now();
    gfx_fill_rect(&gfx, 0, 0, SCREEN_W, SCREEN_H, RGB565(20, 30, 28));
    us[0] = bench_end(hw, t0);

    /* copy another framebuffer onto this one = full-screen opaque blit */
    t0 = now();
    gfx_blit(&gfx, 0, 0, &src, 0);
    us[1] = bench_end(hw, t0);

    /* full-screen 60% dim = the pause overlay (read + blend + write per pixel) */
    t0 = now();
    gfx_fill_alpha(&gfx, 0, 0, SCREEN_W, SCREEN_H, RGB565(0, 0, 0), 150);
    us[2] = bench_end(hw, t0);

    /* many small rectangles (text, particles): per-command overhead */
    t0 = now();
    for (i = 0; i < 2000; i++)
        gfx_fill_rect(&gfx, (i * 37) % (SCREEN_W - 8), (i * 53) % (SCREEN_H - 8), 8, 8,
                      (Pixel)((uint32_t)i * 2654435761u >> 16));
    us[3] = bench_end(hw, t0);

    gfx_end(&gfx);
    if (hw) blit_dcache_invalidate();         /* CPU may read these pixels later */
}

static void print_bench(const uint32_t *cpu, const uint32_t *hw)
{
    int i;
    for (i = 0; i < BENCH_N; i++) {
        tx_printf("  %s: CPU %6u us %4u MB/s", bench_name[i], cpu[i], bench_bytes[i] / cpu[i]);
        if (hw) {
            uint32_t x10 = cpu[i] * 10u / hw[i];
            tx_printf("  | HW %6u us %4u MB/s  = %u.%ux\r\n", hw[i], bench_bytes[i] / hw[i],
                      x10 / 10u, x10 % 10u);
        } else {
            tx_puts("\r\n");
        }
        tx_flush();
    }
}


/* ------------------------------------------------------------------------- */
/* Frame time split and measurement windows (all sums in CLINT ticks)         */
/* ------------------------------------------------------------------------- */
typedef struct {                 /* one frame */
    uint32_t vsync;              /* vsync_wait: previous flip not latched yet */
    uint32_t logic;              /* game / demo update (only if ticked) */
    uint32_t submit;             /* drawing / command submission without qstall */
    uint32_t qstall;             /* waiting for a free command-queue slot */
    uint32_t tail;               /* tail_wait: last command queued -> engine idle */
    uint32_t qend, qpeak;        /* queue entries at the end of submit / max seen */
    int      ticked;
} Frame;

typedef struct {                 /* sums over one window */
    uint32_t t0, frames, ticks;
    uint32_t vsync, logic, logic_max, submit, qstall, tail, draw_max;
    uint32_t qend, qpeak;
    uint32_t hdmi0, busy0, rb0, wb0, cmds0, stalls0;
} Win;

typedef struct {                 /* results: us per frame (logic: per tick) */
    uint32_t el_us, frames, fps10, hdmi_fps;
    uint32_t vsync, logic, logic_max, submit, qstall, tail, other;
    uint32_t draw, draw_max, work, limit10;
    uint32_t pct[6];             /* share of the window: logic submit qstall tail vsync other */
    uint32_t qend, qpeak, stalls, cmds;
    uint32_t blt;                /* engine busy, % of the window */
    uint32_t rd_mbs, wr_mbs;     /* engine AXI bytes / window time */
    uint32_t rd_busy, wr_busy;   /* engine AXI bytes / engine busy time */
} WinStats;

static Win hud_w, uart_w;

static void win_begin(Win *w, uint32_t t)
{
    memset(w, 0, sizeof(*w));
    w->t0 = t;
    w->hdmi0 = hdmi_frame_cnt();
    if (hw_avail) {
        w->busy0 = blit_rd(BLIT_R_PERF_BUSY);
        w->rb0   = blit_rd(BLIT_R_PERF_RB);
        w->wb0   = blit_rd(BLIT_R_PERF_WB);
    }
    w->cmds0   = blit_st.cmds;
    w->stalls0 = blit_st.stalls;
}

static void win_add(Win *w, const Frame *f)
{
    uint32_t draw = f->submit + f->qstall + f->tail;
    ++w->frames;
    w->vsync  += f->vsync;
    w->submit += f->submit;
    w->qstall += f->qstall;
    w->tail   += f->tail;
    w->qend   += f->qend;
    if (f->qpeak > w->qpeak) w->qpeak = f->qpeak;
    if (draw > w->draw_max)  w->draw_max = draw;
    if (f->ticked) {
        ++w->ticks;
        w->logic += f->logic;
        if (f->logic > w->logic_max) w->logic_max = f->logic;
    }
}

static uint32_t pct_of(uint32_t part, uint32_t total)
{
    return total ? (uint32_t)(((uint64_t)part * 100u + total / 2u) / total) : 0u;
}

static uint32_t per_us(uint32_t ticks, uint32_t n)
{
    return n ? ticks / (n * TICKS_PER_US) : 0u;
}

static void win_end(const Win *w, uint32_t t, WinStats *s)
{
    uint32_t el = t - w->t0, n = w->frames, used, other;
    uint32_t busy = 0, rb = 0, wb = 0;
    uint64_t rbytes, wbytes;

    memset(s, 0, sizeof(*s));
    if (!el) el = 1u;
    s->el_us  = el / TICKS_PER_US ? el / TICKS_PER_US : 1u;
    s->frames = n;
    s->fps10  = (uint32_t)(((uint64_t)n * BSP_CLINT_HZ * 10u + el / 2u) / el);
    s->hdmi_fps = (uint32_t)(((uint64_t)(hdmi_frame_cnt() - w->hdmi0) * BSP_CLINT_HZ + el / 2u) / el);

    s->vsync     = per_us(w->vsync, n);
    s->logic     = per_us(w->logic, w->ticks);
    s->logic_max = w->logic_max / TICKS_PER_US;
    s->submit    = per_us(w->submit, n);
    s->qstall    = per_us(w->qstall, n);
    s->tail      = per_us(w->tail, n);
    s->draw      = per_us(w->submit + w->qstall + w->tail, n);
    s->draw_max  = w->draw_max / TICKS_PER_US;
    s->work      = s->logic + s->draw;          /* a frame that also ticks */
    s->limit10   = s->work ? 10000000u / s->work : 0u;

    used  = w->vsync + w->logic + w->submit + w->qstall + w->tail;
    other = el > used ? el - used : 0u;         /* UART, buffer pick, statistics */
    s->other  = per_us(other, n);
    s->pct[0] = pct_of(w->logic,  el);
    s->pct[1] = pct_of(w->submit, el);
    s->pct[2] = pct_of(w->qstall, el);
    s->pct[3] = pct_of(w->tail,   el);
    s->pct[4] = pct_of(w->vsync,  el);
    s->pct[5] = pct_of(other,     el);

    s->qend   = n ? (w->qend + n / 2u) / n : 0u;
    s->qpeak  = w->qpeak;
    s->stalls = blit_st.stalls - w->stalls0;
    s->cmds   = n ? (blit_st.cmds - w->cmds0 + n / 2u) / n : 0u;

    if (hw_avail) {
        busy = blit_rd(BLIT_R_PERF_BUSY) - w->busy0;
        rb   = blit_rd(BLIT_R_PERF_RB)   - w->rb0;
        wb   = blit_rd(BLIT_R_PERF_WB)   - w->wb0;
    }
    rbytes = (uint64_t)rb * 4u;                 /* 32-bit AXI beats */
    wbytes = (uint64_t)wb * 4u;
    s->blt     = (uint32_t)((uint64_t)busy * 100u / ((uint64_t)s->el_us * BLIT_CLK_MHZ));
    s->rd_mbs  = (uint32_t)(rbytes / s->el_us);            /* bytes per us = MB/s */
    s->wr_mbs  = (uint32_t)(wbytes / s->el_us);
    s->rd_busy = busy ? (uint32_t)(rbytes * BLIT_CLK_MHZ / busy) : 0u;
    s->wr_busy = busy ? (uint32_t)(wbytes * BLIT_CLK_MHZ / busy) : 0u;
}

static void print_window(const WinStats *s, const RenderInfo *info, const Gfx *gfx)
{
    tx_printf("fps %u.%u | %s scene %d | %s%s%s | hdmi %u/s uf %u | ph %d foes %d\r\n",
              s->fps10 / 10u, s->fps10 % 10u, use_hw ? "HW" : "CPU", scene,
              vsync ? "vsync" : "free", autoplay ? " auto" : "", alpha_fx ? " fx" : "",
              s->hdmi_fps, hdmi_uf_cnt(), (int)game.phase, game.enemy_count);
    tx_printf("  us/frame: logic %u (max %u) submit %u qstall %u tail_wait %u vsync_wait %u other %u"
              " | draw %u (max %u) work %u -> limit %u.%u fps\r\n",
              s->logic, s->logic_max, s->submit, s->qstall, s->tail, s->vsync, s->other,
              s->draw, s->draw_max, s->work, s->limit10 / 10u, s->limit10 % 10u);
    tx_printf("  share %%: logic %u submit %u qstall %u tail_wait %u vsync_wait %u other %u\r\n",
              s->pct[0], s->pct[1], s->pct[2], s->pct[3], s->pct[4], s->pct[5]);
    if (hw_avail)
        tx_printf("  blit: busy %u%% cmds %u/frame | queue end %u peak %u/%u stalls %u"
                  " | axi rd %u wr %u MB/s (while busy rd %u wr %u)\r\n",
                  s->blt, s->cmds, s->qend, s->qpeak, blit_st.depth, s->stalls,
                  s->rd_mbs, s->wr_mbs, s->rd_busy, s->wr_busy);
    tx_printf("  gfx: fill %u blit %u alpha %u/%u px %u | CPU %u fps %u us / HW %u fps %u us",
              gfx->fills, gfx->blits, gfx->alphas, gfx->alpha_pixels, gfx->pixels_written,
              (unsigned)info->fps_mode[0], (unsigned)info->draw_mode[0],
              (unsigned)info->fps_mode[1], (unsigned)info->draw_mode[1]);
    if (tx_dropped) tx_printf(" | uart lines dropped %u", tx_dropped);
    tx_puts("\r\n");
}

/* ------------------------------------------------------------------------- */
int main(void)
{
    uint32_t last_req, frame = 0, back_i = 0, i, next_tick, compare_n = 0;
    uint32_t bench_cpu[BENCH_N], bench_hw[BENCH_N];
    int last_hw = 0;
    RenderInfo info;
    WinStats ws;
    Gfx gfx;
    GfxHwTarget hw_target;

    bsp_init();
    bsp_printf("\r\n[FLICKER PROBE: no_uart]\r\n");
    bsp_printf("\r\n=== SproutRun on Sapphire RV32IM @ %d MHz, CPU / BitBlt rendering ===\r\n",
               (int)(BSP_CLINT_HZ / 1000000u));

    if (hdmi_id() != HDMI_ID_VALUE)
        bsp_printf("WARNING: hdmi_fb ID = %x (expected 48444d49) - is the new bitstream loaded?\r\n",
                   (int)hdmi_id());
    else
        bsp_printf("hdmi_fb found, RES=%x, frames=%d\r\n", (int)HDMI_RES, (int)hdmi_frame_cnt());

    hw_avail = blit_init();
    if (hw_avail)
        bsp_printf("BitBlt engine found, CAPS=%x (command queue %d)\r\n",
                   (int)blit_rd(BLIT_R_CAPS), (int)blit_st.depth);
    else
        bsp_printf("BitBlt engine NOT found (ID=%x, expected 424c5431) - CPU rendering only\r\n",
                   (int)blit_rd(BLIT_R_ID));

    bsp_printf("framebuffers: %x %x %x (%d bytes each, RGB565), demo pictures at %x\r\n",
               (int)fb_addr[0], (int)fb_addr[1], (int)fb_addr[2], (int)FB_BYTES, (int)IMG_ADDR);
    bsp_printf("build: GFX_CPU_WORD_FILL=%d AUTOPLAY=%d VSYNC=%d HW=%d"
#ifdef __OPTIMIZE__
               " optimized"
#else
               " -O0/-Og (slow!)"
#endif
               "\r\n", GFX_CPU_WORD_FILL, AUTOPLAY_DEFAULT, VSYNC_DEFAULT, HW_DEFAULT);

    for (i = 0; i < FB_COUNT; i++) memset((void *)fb_addr[i], 0, FB_BYTES);

    bsp_printf("Benchmark, same gfx calls on both backends (DDR, write-through D$):\r\n");
    bandwidth_test(0, fb_addr[1], bench_cpu);
    if (hw_avail) bandwidth_test(1, fb_addr[1], bench_hw);
    print_bench(bench_cpu, hw_avail ? bench_hw : 0);

    render_init();
    demo_init(IMG_ADDR);
    game_init(&game, 0xC0FFEEu);
    use_hw = hw_avail && HW_DEFAULT;

    /* show a black buffer first, then start the loop */
    memset((void *)fb_addr[0], 0, FB_BYTES);
    hdmi_fence();
    hdmi_enable(fb_addr[0]);
    last_req = fb_addr[0];
    print_help();
    tx_puts("UART report every second:\r\n"
            "  us/frame: logic = game update (max = worst tick), submit = drawing / queueing\r\n"
            "  without qstall, qstall = waiting for a free queue slot, tail_wait = last command\r\n"
            "  -> engine idle, vsync_wait = waiting for the flip, other = UART + stats\r\n");
    tx_puts(use_hw ? "[backend: BitBlt engine]\r\n" : "[backend: CPU]\r\n");
    tx_flush();

    memset(&info, 0, sizeof(info));
    memset(&gfx, 0, sizeof(gfx));
    info.show_stats = show_stats;
    info.alpha_fx = alpha_fx;
    info.mouse_x = -1; info.mouse_y = -1;
    info.hw_avail = hw_avail;
    info.q_depth = (int)blit_st.depth;

    next_tick = now();
    stats_reset = 1;

    for (;;) {
        uint32_t ta, t0, t1, t2, t3, tend, cur, back, stall0;
        Frame f;
        Input in;

        /* a backend / scene / load change starts fresh measurement windows */
        if (stats_reset) {
            uint32_t t = now();
            stats_reset = 0;
            win_begin(&hud_w, t);
            win_begin(&uart_w, t);
            compare_n = 0;
        }
        memset(&f, 0, sizeof(f));

        /* 1. vsync_wait: the previously requested buffer must be on screen */
        ta = now();
        if (vsync)
            while (hdmi_flip_pending()) { tx_pump(); poll_uart(); }
        f.vsync = now() - ta;

        /* pick a buffer that is neither on screen nor queued */
        cur = hdmi_cur();
        for (i = 1; i <= FB_COUNT; i++) {
            uint32_t k = (back_i + i) % FB_COUNT;
            if (fb_addr[k] != cur && fb_addr[k] != last_req) { back_i = k; break; }
        }
        back = fb_addr[back_i];

        /* 2. input */
        poll_uart();
        tx_pump();

        /* 3. logic: at most one 1/60 s step per rendered frame */
        t0 = now();
        f.ticked = vsync || (int32_t)(t0 - next_tick) >= 0;
        if (f.ticked) {
            if (scene == SCENE_GAME) {
                in = autoplay ? bot_input(&game, frame) : manual_input();
                in.pressed |= key_pressed;
                key_pressed = 0;
                game_tick(&game, in);
            } else if (scene == SCENE_SQUARES) {
                demo_squares_tick(sq_count);
            }
            next_tick += TICK_PERIOD;
            if ((int32_t)(t0 - next_tick) > 0) next_tick = t0;   /* 考虑一下低帧率的问题（游戏逻辑频率小于显示频率） */
        }
        t1 = now();
        if (f.ticked) f.logic = t1 - t0;

        /* 4. submit: CPU draws the pixels itself, or queues BitBlt commands */
        info.show_stats = show_stats;
        info.alpha_fx = alpha_fx;
        info.hw = use_hw;
        if (!use_hw && last_hw) blit_dcache_invalidate();   /* drop lines the engine overwrote */
        last_hw = use_hw;
        stall0 = blit_st.stall_ticks;
        blit_st.qpeak = 0;
        if (use_hw) gfx_begin_hw(&gfx, &hw_target, back, SCREEN_W, SCREEN_H);
        else        gfx_begin(&gfx, (Pixel *)back, SCREEN_W, SCREEN_H);
        if (scene == SCENE_SQUARES)     demo_squares_render(&gfx, info, sq_count);
        else if (scene == SCENE_IMAGES) demo_images_render(&gfx, info, frame, img_period);
        else                            render_game(&game, &gfx, info);
        gfx_end(&gfx);
        if (use_hw) {
            f.qend  = blit_queue_used();      /* commands still waiting after the last push */
            f.qpeak = blit_st.qpeak;
        }
        t2 = now();
        f.qstall = blit_st.stall_ticks - stall0;
        f.submit = t2 - t1 > f.qstall ? t2 - t1 - f.qstall : 0u;

        /* 5. tail_wait: the frame must be complete in DDR before the flip */
        if (use_hw) blit_wait();
        t3 = now();
        f.tail = t3 - t2;

        /* 6. present (latched by hardware at the next vsync) */
        hdmi_flip(back);
        last_req = back;
        ++frame;
        win_add(&hud_w, &f);
        win_add(&uart_w, &f);

        /* 7. statistics: HUD every 0.5 s, UART every 1 s */
        tend = now();
        if (tend - hud_w.t0 >= STAT_PERIOD) {
            win_end(&hud_w, tend, &ws);
            info.fps          = (int)((ws.fps10 + 5u) / 10u);
            info.logic_us     = (int)ws.logic;
            info.logic_max_us = (int)ws.logic_max;
            info.render_us    = (int)ws.draw;
            info.work_us      = (int)ws.work;
            info.ms           = (int)((ws.work + 50u) / 100u);
            info.submit_us    = (int)ws.submit;
            info.qstall_us    = (int)ws.qstall;
            info.tail_us      = (int)ws.tail;
            info.vsync_us     = (int)ws.vsync;
            info.q_end        = (int)ws.qend;
            info.q_peak       = (int)ws.qpeak;
            info.q_depth      = (int)blit_st.depth;
            info.blt          = (int)ws.blt;
            info.rd_mbs       = (int)ws.rd_mbs;
            info.wr_mbs       = (int)ws.wr_mbs;
            info.fps_mode[use_hw]  = info.fps;
            info.draw_mode[use_hw] = (int)ws.draw;
            win_begin(&hud_w, tend);
            if (auto_compare && hw_avail && ++compare_n >= COMPARE_WINDOWS) set_hw(!use_hw);
        }
        if (tend - uart_w.t0 >= UART_PERIOD) {
            win_end(&uart_w, tend, &ws);
            /* Diagnostic variant: periodic UART report disabled. */
            win_begin(&uart_w, tend);
        }
    }
    return 0;
}
