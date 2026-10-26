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
 * Four framebuffers are used so the CPU never draws into the buffer being
 * scanned out nor into the one waiting for the next vsync (the 4th one is for
 * band mode, where the engine may still be drawing the previous frame):
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
 *
 * 编译开关 SPROUT_STATS（make STATS=1）：默认 0 = 发行版，HUD 只显示 FPS / MS，
 * 串口每秒一行；'f' 'c' 'k' 'o' 'i' 'u' 'j' 'z' 'q' 'y' '0' '9' '8' '7' '6' '5' '4' 和启动测速只在 STATS=1 版里有。
 * 分带合成（引擎 CAPS[31]，STATS 版 '5' 开关 / '4' K 通路，makefile BAND=0 开机不用）：见 blit.h BLIT_R_BAND_GEO。
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
#include "pad.h"
#include "th.h"
#include "audio.h"
#include "control.h"
#include "input_devices.h"
#include "music.h"
#include "game_sound.h"                         /* HDMI 音频（2026-10-19） */
#include "th_sound.h"

#ifndef SPROUT_STATS
#define SPROUT_STATS 0                      /* makefile: make STATS=1 -> -DSPROUT_STATS=1 */
#endif
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
#ifndef FB3_ADDR
#define FB3_ADDR 0x01C00000u                /* 第 4 个帧缓冲（图片之后）：分带流水时多一帧在引擎里 */
#endif
#ifndef BAND_DEFAULT
#define BAND_DEFAULT 1                      /* 引擎有分带合成（CAPS[31]）时开机就用；makefile BAND=0 关 */
#endif
#ifndef HW_DEFAULT
#define HW_DEFAULT 1                        /* start with the BitBlt backend if found */
#endif
#ifndef GFX_FONT_ATLAS
#define GFX_FONT_ATLAS 1                    /* gfx.c: text = keyed copies from a glyph atlas */
#endif
/* 引擎时钟 = core_clk = CPU 时钟（ddr3_example_top.v：ASYN_AXI_CLK = 0 -> user_clk = core_clk），
 * 所以直接跟 BSP 的 SYSTEM_CLINT_HZ 走；改 core_clk 频率时只需改 soc.h 里那一个数。 */
#define BLIT_CLK_MHZ  (BSP_CLINT_HZ / 1000000u)

#ifndef TH_FRAME_BUDGET_US
#define TH_FRAME_BUDGET_US 16667u           /* 弹幕压力自动档的每帧预算：60 帧 */
#endif

#define FB_COUNT      4
#define FB_BYTES      ((uint32_t)SCREEN_W * SCREEN_H * sizeof(Pixel))
#define TICKS_PER_US  (BSP_CLINT_HZ / 1000000u)
#define TICK_PERIOD   (BSP_CLINT_HZ / 60u)          /* one game update */
#define STAT_PERIOD   (BSP_CLINT_HZ / 2u)           /* HUD refresh: 0.5 s */
#define UART_PERIOD   (BSP_CLINT_HZ)                /* UART report: 1 s */

static const uint32_t fb_addr[FB_COUNT] = { FB0_ADDR, FB1_ADDR, FB2_ADDR, FB3_ADDR };

/* 显示几何（2026-10-21，720p 过渡）：hdmi_fb 的 RES 寄存器给出画面大小（640x480 或 1280x720）。
 * 游戏画面仍是 SCREEN_W x SCREEN_H；画面比它大时居中放在帧缓冲里（四周黑边）：
 *   disp_pitch  帧缓冲一行的字节数        disp_bytes  一个帧缓冲的字节数（<= 2 MB，帧缓冲相距 2 MB）
 *   view_off    游戏画面 (0,0) 相对帧缓冲起点的字节偏移
 *   view_row    游戏画面第 0 行的行首偏移（分带帧从这里开始，256 字节对齐） */
static uint32_t disp_w = SCREEN_W, disp_h = SCREEN_H;
static uint32_t disp_pitch = SCREEN_W * 2u, disp_bytes = FB_BYTES;
static uint32_t view_off, view_row;

static void disp_init(void)
{
    const uint32_t r = hdmi_dual_supported()?HDMI_CANVAS:HDMI_RES, w = r & 0xFFFFu, h = r >> 16;
    if (w >= SCREEN_W && h >= SCREEN_H && w * h * 2u <= 0x200000u && (w & 7u) == 0u) {
        disp_w = w; disp_h = h;
    }
    disp_pitch = disp_w * 2u;
    disp_bytes = disp_pitch * disp_h;
    view_row = ((disp_h - SCREEN_H) / 2u) * disp_pitch;
    view_off = view_row + ((disp_w - SCREEN_W) / 2u) * 2u;
    if (view_row & 255u) {                     /* 分带帧起点要 256 字节对齐：退回左上角 */
        view_row = 0u; view_off = 0u;
    }
}

/* 开始画一帧（游戏画面那一块）：HW = 引擎命令（分带帧从 view_row 行首开始），CPU = 直接写像素 */
static void view_begin(Gfx *gfx, GfxHwTarget *t, int hw, uint32_t fb)
{
    if (hw) gfx_begin_hw_view(gfx, t, fb + view_off, disp_pitch, fb + view_row, SCREEN_W, SCREEN_H);
    else    gfx_begin_stride(gfx, (Pixel *)(fb + view_off), SCREEN_W, SCREEN_H, (int)(disp_pitch / 2u));
}

/* 弹幕场景原生 720p（2026-10-21）：逻辑仍按 640x480 布局，画面整体 1.5 倍 = 960x720，
 * 放在 1280x720 正中；绘图目标是整个 1280x720，两侧 160 像素是边框图的装饰条
 * （分带模式整行写回帧缓冲，所以装饰条和边框一样每帧画）。'#' 键 / makefile HD=0|1 切换。 */
#ifndef TH_HD_DEFAULT
#define TH_HD_DEFAULT 1
#endif
static int th_hd;                       /* 现在的弹幕画面：1 = 960x720（1.5 倍），0 = 640x480 */
static int th_hd_want = TH_HD_DEFAULT;  /* '#' 改它，主循环开头 view_apply() 生效 */
static int view_dirty;                  /* 主循环开头：切换画面大小 / 清帧缓冲（换场景时的残留） */

static int th_hd_possible(void) { return disp_w == 1280u && disp_h == 720u; }   /* frame picture is 1280x720 */

static void view_begin_th_hd(Gfx *gfx, GfxHwTarget *t, int hw, uint32_t fb)
{
    if (hw) gfx_begin_hw_view(gfx, t, fb, disp_pitch, fb, (int)disp_w, (int)disp_h);
    else    gfx_begin_stride(gfx, (Pixel *)fb, (int)disp_w, (int)disp_h, (int)(disp_pitch / 2u));
}

#ifdef __riscv
#define STRESS_POOL __attribute__((section(".stressbss"),aligned(256)))
#else
#define STRESS_POOL __attribute__((aligned(256)))
#endif
static Game game STRESS_POOL;
static Th   th STRESS_POOL;                          /* 东方式弹幕（开机场景，th.h） */

static inline uint32_t now(void) { return clint_getTimeLow(BSP_CLINT); }

/* ------------------------------------------------------------------------- */
/* Non-blocking UART TX: text goes into a ring and is drained a few bytes at  */
/* a time, so printing statistics never stalls the frame loop.                */
/* ------------------------------------------------------------------------- */
#define TX_SIZE 8192u
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
    char line[1024];
    va_list ap;
    va_start(ap, fmt);
    int n=mini_vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);
    if(n<0||(unsigned)n>=sizeof(line)){++tx_dropped;return;}
    tx_puts(line);
}

static void tx_flush(void)
{
    while (tx_tail != tx_head) tx_pump();
}

/*
 * 开机排查（2026-10-08，C2 宽通路开机卡死）：BOOT_DIAG=1 时，启动测速的每一步前后
 * 直接（不经 tx 缓冲）打一行进度，并把代码段 0x1000.._etext 按 256 B 一块算校验和：
 * 第一次调用记参考值，之后哪块变了就打出块地址和前 4 个字（看像不像填充色/画面数据）。
 * 读之前先丢 D$，读到的就是 DDR 里的真实内容。卡死时最后一行 = 卡在哪一步。
 */
#ifndef BOOT_DIAG
#define BOOT_DIAG SPROUT_STATS
#endif
#if BOOT_DIAG
extern char _etext[];
#define DIAG_BLK  256u
#define DIAG_NBLK 1024u                          /* 256 KB 代码段以内 */
static uint32_t diag_ref[DIAG_NBLK];
static int diag_have_ref;
static void diag_text(const char *tag)
{
    const uint32_t e = (uint32_t)(uintptr_t)_etext;
    uint32_t a, b, i, s, bad = 0;
    tx_flush();
    blit_dcache_invalidate();
    for (a = 0x1000u, b = 0; a < e && b < DIAG_NBLK; a += DIAG_BLK, b++) {
        const volatile uint32_t *p = (const volatile uint32_t *)(uintptr_t)a;
        const uint32_t n = (e - a < DIAG_BLK ? e - a : DIAG_BLK) / 4u;
        for (s = 0x9E3779B9u, i = 0; i < n; i++) s = (s << 5 | s >> 27) ^ p[i];
        if (!diag_have_ref) diag_ref[b] = s;
        else if (s != diag_ref[b] && ++bad <= 8)
            bsp_printf("  !! text block %x changed: %x %x %x %x\r\n", (int)a,
                       (int)p[0], (int)p[1], (int)p[2], (int)p[3]);
    }
    bsp_printf("[diag] %s: text %x..%x %s (%d blocks changed)\r\n", tag, 0x1000, (int)e,
               diag_have_ref ? (bad ? "CORRUPTED" : "ok") : "reference", (int)bad);
    diag_have_ref = 1;
}
#define DIAG(tag) diag_text(tag)
#else
#define DIAG(tag) ((void)0)
#endif

/* ------------------------------------------------------------------------- */
/* Input                                                                      */
/* ------------------------------------------------------------------------- */
static int      dir_x, dir_y;         /* latched manual direction */
static int player_pick[3],player_slow[3],matrix_autofire=1;
static uint32_t key_pressed;          /* edge keys waiting for the next tick */
static int      autoplay = AUTOPLAY_DEFAULT;
static int      vsync    = VSYNC_DEFAULT;
static int      vpipe    = 1; /* R6: one completed band frame may overlap the next CPU frame */
static int      show_stats = SPROUT_STATS;   /* 统计面板 'f'：只有 STATS=1 版有 */
static int      alpha_fx   = 1;
static int      batch_list = 1;       /* hostile seeds via gfx_blit_list ('k') */
static int      list_mode = BLIT_MODE_APB; /* engine submission when batch_list ('k'):
                                         APB registers / HW command list in DDR / LIST_WORD */
static int      test_bars;
static int      flip_stuck;           /* 翻页超时诊断只打一次 */
static int      esc_state;

/* rendering backend / scene */
enum { SCENE_TH, SCENE_GAME, SCENE_SQUARES, SCENE_IMAGES, SCENE_COUNT };
static int      hw_avail;             /* BitBlt engine answered at boot */
static int      use_hw;               /* draw with the engine */
static int      auto_compare;         /* switch backend every COMPARE_WINDOWS */
static int      scene = SCENE_TH;     /* 开机进东方式弹幕；'n' 切到 SproutRun / 方块 / 图片 */
static uint32_t th_edge;              /* 串口键给弹幕的单次按键（PAD_*：菜单上下、Bomb…） */
static int      th_slow;              /* 串口 '2'：低速锁定开关 */
static int      th_bot_last;
static int      sq_count   = 256;     /* moving squares */
static int      img_period = 20;      /* frames per picture in the switching demo */
static int      stats_reset;          /* mode changed: restart the measurement window */
#if SPROUT_STATS
static int      seed_prof_req;        /* 'y': run seed_profile() in the next frame */
static int      eng_prof_req;         /* '8': run engine_profile() in the next frame */
#endif
static int      overlap = 1;          /* 'q', HW only: queue the frame first, then run the game tick
                                         while the engine draws (shows the state one tick later) */
/* 分带合成 + 流水帧（引擎 CAPS[31]，'5' 键开关）：每帧用 BANDS..ENDB 提交，引擎分带画。
 * 这一帧提交完不等它画完，直接去跑逻辑 / 录下一帧；下一圈循环里等它画完再翻页
 * （所以多一帧延迟，要 4 个帧缓冲：正在显示、等翻页、引擎在画、CPU 在录）。
 * pend_fb = 引擎还在画、还没翻页的那一帧（0 = 没有），pend_seq = 它的帧序号（blit_band_done）。 */
static int      band_want = BAND_DEFAULT;
static uint32_t pend_fb, pend_seq;
#define COMPARE_WINDOWS 8             /* x 0.5 s */

/* SPROUT_STATS（make STATS=1）：调试 / 测量版 —— 每秒 6 行详细报告、统计面板 'f'、启动测速、
 * A/B 键 k o i u j z q y c。默认 0 = 发行版：HUD 只有 FPS（和 MS），串口每秒一行 fps，
 * 固定配置 hwlist + 弹幕表 + 键数组排序 + overlap + 在途读字数 GAME_LIMIT_WORDS。 */
#if !SPROUT_STATS
static void print_help(void)
{
    tx_puts("R4: :id command [value] LF; [/] bullet size; {/} square size; + / - selected step 100/10/1; backslash next BGM; semicolon BGM on/off.\r\n");
    tx_puts("R5.2: :id fast LF restores acceleration; :id bars 0/1 LF selects test bars. Carrot: J/j ring skill; ~ sort diagnostic (debug); other scene keys unchanged.\r\n");
    tx_puts("\r\ndanmaku (boot scene): tools/pc_pad.py = real held keys (arrows, Z shot, X bomb, Shift slow, Esc pause);\r\n"
            "      UART: wasd move (latched, auto shot), 1 bomb, 2 slow on/off, Enter ok, p pause,\r\n"
            "      r restart, m title, l stress demo, - + stress level (manual), * stress AUTO (keeps 60 fps), b autoplay demo\r\n"
            "keys: wasd/arrows move, x stop, Enter/Space start, p pause, r restart,\r\n"
            "      1 2 3 pick, l stress menu, m menu, b autoplay, v vsync,\r\n"
            "      t colour bars, e alpha effects, g CPU/BitBlt backend,\r\n"
            "      n next scene (game/squares/images), - + demo load, h help,\r\n"
            "      # danmaku 1280x720 native (x1.5) / 640x480 (720p bitstream)\r\n"
            "sound: , HDMI 1 kHz test tone on/off, . volume 256/128/64/0 + status, / play the next bank sound\r\n"
            "      (release build: make STATS=1 for the detailed report, stats panel and A/B keys)\r\n");
}
#else
static void print_help(void)
{
    tx_puts("R4: :id command [value] LF; [/] bullet size; {/} square size; + / - selected step 100/10/1; backslash next BGM; semicolon BGM on/off.\r\n");
    tx_puts("R5.2: :id fast LF restores acceleration; :id bars 0/1 LF selects test bars. Carrot: J/j ring skill; ~ sort diagnostic (debug); other scene keys unchanged.\r\n");
    tx_puts("\r\ndanmaku (boot scene): tools/pc_pad.py = real held keys (arrows, Z shot, X bomb, Shift slow, Esc pause);\r\n"
            "      UART: wasd move (latched, auto shot), 1 bomb, 2 slow on/off, Enter ok, p pause,\r\n"
            "      r restart, m title, l stress demo, - + stress level (manual), * stress AUTO (keeps 60 fps), b autoplay demo\r\n"
            "keys: wasd/arrows move, x stop, Enter/Space start, p pause, r restart,\r\n"
            "      1 2 3 pick, l stress menu, m menu, f stats, b autoplay, v vsync,\r\n"
            "      t colour bars, e alpha effects, h help,\r\n"
            "      k seeds: calls -> list (APB) -> hwlist (DDR command list) -> hwword (LIST_WORD)\r\n"
            "      o i engine LIMIT: max outstanding read / write bursts (smaller = CPU waits less for DDR)\r\n"
            "      u   engine LIMIT: max read words in flight 1024/512/256/128/64/32/16\r\n"
            "      ~   enemy draw order (j/J outside carrot): key-array sort / original struct sort (same order, A/B speed)\r\n"
            "      z   seeds: draw table written by game logic / walk the bullet pool again (same picture)\r\n"
            "      y   one-shot seed profile: walk / build / submit / tiny / full, engine idle and busy\r\n"
            "      q   HW frame order: submit -> logic while the engine draws -> wait / logic -> submit -> wait\r\n"
            "      0   engine source cache (16 KB BRAM, copies w <= 64) on / off\r\n"
            "      9   hwlist seeds: engine reads the seed table (TAB) / CPU writes one word per seed\r\n"
            "      8   one-shot engine profile (hwlist): bg / seeds / frame / rest, engine alone, stall split\r\n"
            "      7   128-bit wide path (opaque aligned COPY/FILL >= 128 px): off / on, write batch 0/64/128/256\r\n"
            "      6   wide path read words in flight 256/128/64/512/1024/32\r\n"
            "      5   band mode (frames composed on chip 16 rows at a time + pipelined frames) on / off\r\n"
            "      4   band mode K path (8 px/clk sprites and fills) on / off\r\n"
            "      g CPU/BitBlt backend, c auto compare, n next scene (game/squares/images),\r\n"
            "      - + demo load, # danmaku 1280x720 native (x1.5) / 640x480 (720p bitstream)\r\n"
            "sound: , HDMI 1 kHz test tone on/off, . volume 256/128/64/0 + status, / play the next bank sound\r\n");
}
#endif

/* seed submission as shown on HUD / UART: 0 calls, 1 list, 2 hwlist, 3 hwword */
static int seed_mode(void) { return batch_list ? 1 + list_mode : 0; }
#if SPROUT_STATS
static const char *const seed_mode_name[4] = { "calls", "list", "hwlist", "hwword" };
#endif

static void set_seed_mode(int m)
{
    batch_list = m != 0;
    list_mode  = m >= 2 ? m - 1 : BLIT_MODE_APB;
    if (hw_avail) list_mode = blit_set_mode(list_mode);    /* waits for the engine */
    if (hw_avail) blit_set_band(band_want);                /* 分带只在 hwlist（LIST）模式下生效 */
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
    if (scene == SCENE_TH) {                 /* 压力档位（手动，关掉自动档） */
        th_stress_step(&th, up ? 1 : -1);
        tx_printf("[danmaku stress: x%d, manual (* = auto 60 fps)]\r\n", th_stress_dens(&th));
        return;
    }
    if (scene == SCENE_SQUARES) {
        sq_count = up ? sq_count * 2 : sq_count / 2;
        if (sq_count < 100) sq_count = 100;
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
    pad_use_terminal();
    if (set_x) dir_x = x;
    if (set_y) dir_y = y;
    th_edge |= y < 0 ? PAD_UP : y > 0 ? PAD_DOWN : x < 0 ? PAD_LEFT : x > 0 ? PAD_RIGHT : 0u;
    if (autoplay) { autoplay = 0; tx_puts("[autoplay OFF]\r\n"); }
}

static void handle_legacy_char(char c)
{
    if(scene==SCENE_SQUARES && (c==',' || c=='.')){
        if(hw_avail)blit_wait();sq_count += c=='.'?100:-100;
        if(sq_count<1)sq_count=1;if(sq_count>DEMO_MAX_SQUARES)sq_count=DEMO_MAX_SQUARES;
        tx_printf("[squares fine load %d step 100 max %d]\r\n",sq_count,DEMO_MAX_SQUARES);stats_reset=1;return;
    }
    if(scene==SCENE_GAME && c=='4'){
        if(hw_avail)blit_wait();autoplay=0;key_pressed=0;
        game_start_stress(&game,0xC0FFEEu,STRESS_CUSTOM);stats_reset=1;return;
    }
    if(scene==SCENE_GAME && (c=='l'||c=='L')){
        if(hw_avail)blit_wait();autoplay=0;key_pressed=0;
        game.phase=STRESS_SELECT;stats_reset=1;return;
    }
    if(scene==SCENE_GAME && game.stress_tier==STRESS_CUSTOM && (c=='-'||c=='+'||c=='=')){
        if(hw_avail)blit_wait();game_set_stress_bullets(&game,game.stress_bullet_target+(c=='-'?-1000:1000));
        tx_printf("[carrot custom cap %d]\r\n",game_hostile_slots(&game));stats_reset=1;return;
    }
    if(scene==SCENE_GAME && (c=='j'||c=='J')){
        autoplay=0;key_pressed|=KEY_SKILL;return;
    }
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
#if SPROUT_STATS
    case 'f': case 'F': show_stats = !show_stats; stats_reset = 1; break;
#endif
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
#if SPROUT_STATS                          /* A/B 测量键：只在 STATS=1 版里有 */
    case 'k': case 'K': {
        static const char *const msg[4] = {
            "[seeds: one gfx_blit call per seed]\r\n",
            "[seeds: batched draw list, gfx_blit_list -> APB DST+CMD per seed]\r\n",
            "[HW command list: every command -> DDR list, 1 word per seed, LIST_END kicks]\r\n",
            "[seeds: 1 APB LIST_WORD write per seed, other commands via APB registers]\r\n" };
        set_seed_mode((seed_mode() + 1) % (hw_avail && blit_st.has_list ? 4 : 2));
        stats_reset = 1;
        tx_puts(msg[seed_mode()]);
        break;
    }
    case 'o': case 'O': case 'i': case 'I': {
        /* engine LIMIT presets: o = read bursts, i = write bursts */
        static const uint8_t rd_set[5] = { 16, 8, 4, 2, 1 }, wr_set[6] = { 128, 16, 8, 4, 2, 1 };
        uint32_t rd = blit_st.rd_max, wr = blit_st.wr_max; int i;
        if (!hw_avail || !blit_st.has_limit) { tx_puts("[no LIMIT register in this bitstream]\r\n"); break; }
        if (c == 'o' || c == 'O') { for (i = 0; i < 5 && rd_set[i] != rd; i++) ; rd = rd_set[(i + 1) % 5]; }
        else          { for (i = 0; i < 6 && wr_set[i] != wr; i++) ; wr = wr_set[(i + 1) % 6]; }
        blit_set_limit(rd, wr);
        stats_reset = 1;
        tx_printf("[engine LIMIT: read bursts %u, write bursts %u]\r\n", blit_st.rd_max, blit_st.wr_max);
        break;
    }
    case 'u': case 'U': {
        /* engine LIMIT: read words in flight (limits bytes queued ahead of the CPU) */
        static const uint16_t rw_set[7] = { 1024, 512, 256, 128, 64, 32, 16 };
        int i;
        if (!hw_avail || !blit_st.has_limit_words) { tx_puts("[no LIMIT read-word field in this bitstream]\r\n"); break; }
        for (i = 0; i < 7 && rw_set[i] != blit_st.rw_max; i++) ;
        blit_set_rdwords(rw_set[(i + 1) % 7]);
        stats_reset = 1;
        tx_printf("[engine LIMIT: read words in flight %u (read bursts %u, write bursts %u)]\r\n",
                  blit_st.rw_max, blit_st.rd_max, blit_st.wr_max);
        break;
    }
    case 'j': case 'J': case '~': /* ~ retains R2 sort diagnostic in carrot. */
        render_sort_keys = !render_sort_keys;
        stats_reset = 1;
        tx_puts(render_sort_keys ? "[enemy draw order: y keys copied once, sorted from last frame's order]\r\n"
                                 : "[enemy draw order: original sort reading the enemy structs]\r\n");
        break;
    case 'z': case 'Z':
        render_seed_table = !render_seed_table;
        stats_reset = 1;
        tx_puts(render_seed_table ? "[seeds: draw table written by game logic (no second pool walk)]\r\n"
                                  : "[seeds: walk the bullet pool in render (old)]\r\n");
        break;
    case 'q': case 'Q':
        overlap = !overlap;
        stats_reset = 1;
        tx_puts(overlap ? "[frame order (HW): submit, then logic while the engine draws, then wait]\r\n"
                        : "[frame order: logic, submit, wait (serial)]\r\n");
        break;
    case 'y': case 'Y':
        if (!hw_avail || !use_hw || scene != SCENE_GAME) { tx_puts("[seed profile needs the BitBlt backend and the game scene]\r\n"); break; }
        seed_prof_req = 1;
        tx_puts("[seed profile: next frame]\r\n");
        break;
    case '8':                             /* 引擎单独跑：背景 / 弹幕 / 整帧 / 其余 的引擎时间和停顿构成 */
        if (!hw_avail || !use_hw || scene != SCENE_GAME) { tx_puts("[engine profile needs the BitBlt backend and the game scene]\r\n"); break; }
        eng_prof_req = 1;
        tx_puts("[engine profile: next frame]\r\n");
        break;
    case 'c': case 'C':
        if (!hw_avail) { set_hw(1); break; }
        auto_compare = !auto_compare;
        tx_puts(auto_compare ? "[auto compare ON: CPU / BitBlt every 4 s]\r\n"
                             : "[auto compare OFF]\r\n");
        break;
    case '0':                             /* 引擎源缓存开 / 关（字母都占用了，用 0） */
        if (!hw_avail || !blit_st.has_cache) { tx_puts("[no source cache in this bitstream]\r\n"); break; }
        blit_wait();
        tx_puts(blit_set_cache(!blit_st.cflag) ? "[engine source cache ON (copies w <= 64)]\r\n"
                                               : "[engine source cache OFF]\r\n");
        stats_reset = 1;
        break;
    case '9':                             /* 弹幕表交给引擎读（TAB）/ CPU 逐颗写列表字，A/B 对比 */
        if (!hw_avail || !(blit_rd(BLIT_R_CAPS) & BLIT_CAPS_TABLE)) { tx_puts("[no TAB record in this bitstream]\r\n"); break; }
        blit_wait();
        blit_st.has_table = !blit_st.has_table;
        tx_puts(blit_st.has_table ? "[hwlist seeds: engine reads the table (TAB)]\r\n"
                                  : "[hwlist seeds: one list word per seed (CPU)]\r\n");
        stats_reset = 1;
        break;
    case '7': case '6': {                 /* 128 位宽通路（C2）：7 = 关 / 开 + 成批写档位，6 = 宽通路在途读字数 */
        static const uint8_t  wth_set[4] = { 0, 1, 2, 4 };          /* 成批写：0 / 64 / 128 / 256 拍 */
        static const uint16_t xw_set[6]  = { 256, 128, 64, 512, 1024, 32 };
        uint32_t v = blit_st.wide, wth = (v >> 4) & 15u, xw = (v >> 16) & 0x7FFu; int i;
        if (!hw_avail || !blit_st.has_wide) { tx_puts("[no wide path (CAPS[30]) in this bitstream]\r\n"); break; }
        if (c == '7') {                   /* off -> wth 0 -> 1 -> 2 -> 4 -> off */
            if (v & BLIT_WIDE_OFF) { v &= ~(BLIT_WIDE_OFF | (15u << 4)); }
            else {
                for (i = 0; i < 4 && wth_set[i] != wth; i++) ;
                if (i >= 3) v |= BLIT_WIDE_OFF;
                else        v = (v & ~(15u << 4)) | ((uint32_t)wth_set[i + 1] << 4);
            }
        } else {
            for (i = 0; i < 6 && xw_set[i] != xw; i++) ;
            v = (v & ~(0x7FFu << 16)) | ((uint32_t)xw_set[(i + 1) % 6] << 16);
        }
        blit_wait();
        blit_set_wide(v);
        stats_reset = 1;
        if (blit_st.wide & BLIT_WIDE_OFF) tx_puts("[wide path OFF: every command on the 32-bit path]\r\n");
        else tx_printf("[wide path ON: w >= %u px, COPY write batch %u beats, wide read words in flight %u]\r\n",
                       ((blit_st.wide >> 8) & 0xFFu) * 8u, ((blit_st.wide >> 4) & 15u) * 64u,
                       (blit_st.wide >> 16) & 0x7FFu);
        break;
    }
    case '5':                             /* 分带合成 + 流水帧 开 / 关（同一位流 A/B） */
        if (!hw_avail || !blit_st.has_band) { tx_puts("[no band mode (CAPS[31]) in this bitstream]\r\n"); break; }
        band_want = !band_want;
        blit_set_band(band_want);
        stats_reset = 1;
        tx_puts(blit_st.band ? "[band mode ON: frames composed 16 rows at a time on chip, CPU records the next frame meanwhile]\r\n"
                : band_want  ? "[band mode needs the HW command list: press 'k' until hwlist]\r\n"
                             : "[band mode OFF: every command drawn directly in DDR]\r\n");
        break;
    case '4':                             /* 分带里的 K 通路（每拍 8 像素）开 / 关 */
        if (!hw_avail || !blit_st.has_band) { tx_puts("[no band mode (CAPS[31]) in this bitstream]\r\n"); break; }
        blit_band_set_koff(!(blit_st.band_koff & BLIT_BAND_KOFF));
        stats_reset = 1;
        tx_puts(blit_st.band_koff ? "[band K path OFF: sprites / fills on the 2 px/clk path]\r\n"
                                  : "[band K path ON: cached copies and fills at 8 px/clk]\r\n");
        break;
#endif
    case 'g': case 'G': set_hw(!use_hw); break;
    case 'n': case 'N':
        scene = (scene + 1) % SCENE_COUNT; aud_tone(0);
        stats_reset = 1;
        view_dirty = 1;                   /* 720p 下各场景画的范围不同：清掉残留 */
        tx_puts(scene == SCENE_TH ? "[scene: danmaku (Touhou-style)]\r\n" :
                scene == SCENE_GAME ? "[scene: SproutRun]\r\n" :
                scene == SCENE_SQUARES ? "[scene: moving squares]\r\n" : "[scene: image switching]\r\n");
        break;
    case '-': case '_': case '[': change_load(0); break;
    case '+': case '=': case ']': change_load(1); break;
    case '*':                             /* 弹幕压力档位：自动保 60 帧 开 / 关 */
        th_stress_set_auto(&th, !th.stress_auto);
        tx_printf(th.stress_auto ? "[danmaku stress: AUTO, keeps every frame under %u us]\r\n"
                                 : "[danmaku stress: manual x%u (- + to change)]\r\n",
                  th.stress_auto ? (unsigned)(hdmi_mode_1080()?2u*TH_FRAME_BUDGET_US:TH_FRAME_BUDGET_US) : (unsigned)th_stress_dens(&th));
        break;
    case '@':
        if(hw_avail)blit_wait();scene=SCENE_SQUARES;sq_count=20000;
        set_hw(1);band_want=1;set_seed_mode(2);alpha_fx=1;vsync=1;view_dirty=1;stats_reset=1;
        tx_printf("[720p squares 20000: motion %s available %d; ! CPU/HW motion, %% table/calls, & same-state A/B]\r\n",
                  demo_motion_enabled()?"HW":"CPU",demo_motion_available());break;
    case '!':
        if(scene==SCENE_SQUARES){tx_printf("[square motion %s available %d error %d]\r\n",
          demo_motion_toggle()?"HW":"CPU",demo_motion_available(),demo_motion_error());stats_reset=1;}break;
    case '%':
        if(scene==SCENE_SQUARES){if(hw_avail)blit_wait();tx_printf("[square packed table %s]\r\n",demo_table_toggle()?"ON":"off");stats_reset=1;}break;
    case '&':
        if(scene==SCENE_SQUARES){unsigned cpu=0,hw=0;int ok=demo_motion_benchmark(sq_count,&cpu,&hw);
          tx_printf("[square A/B count %d exact %s CPU %u cycles HW wall %u cycles engine %u cycles; speedup x%u.%u]\r\n",
            sq_count,ok==1?"PASS":ok<0?"UNAVAILABLE":"FAIL",cpu,hw,demo_motion_cycles(),
            hw?cpu/hw:0,hw?(unsigned)((uint64_t)cpu*10u/hw)%10u:0);stats_reset=1;}break;
    case 'h': case 'H': case '?': print_help(); break;
    case '#':                             /* 弹幕画面 1280x720 原生（1.5 倍）/ 640x480 */
        if (!th_hd_possible()) { tx_puts("[danmaku 720p needs the 1280x720 bitstream (hdmi_fb RES)]\r\n"); break; }
        th_hd_want = !th_hd_want;
        view_dirty = 1;
        stats_reset = 1;
        break;
    case '(': case ',':                             /* HDMI 音频：硬件 1 kHz 测试音（不用混音器、不用声音库） */
        if (!aud.hdmi) { tx_puts("[no HDMI audio in this bitstream]\r\n"); break; }
        aud_tone(!aud.tone);
        tx_puts(aud.tone ? "[audio: 1 kHz test tone ON (left + right)]\r\n" : "[audio: test tone off]\r\n");
        break;
    case ')': case '.': {                           /* 总音量 256 -> 128 -> 64 -> 0 -> 256，顺便打一行状态 */
        char line[200];
        aud_master(aud.master >= 256 ? 128 : aud.master >= 128 ? 64 : aud.master >= 64 ? 0 : 256);
        aud_status_line(line, sizeof line);
        tx_printf("[%s]\r\n", line);
        break;
    }
    case '/': {                           /* 声音库试听：依次放每个声音（BGM 也在声部 15 上放一遍） */
        static int snd_test = -1;
        if (!aud.mixer || !aud.nsnd) { tx_puts("[audio: no mixer or no sound bank at 0x02000000]\r\n"); break; }
        snd_test = (snd_test + 1) % aud.nsnd;
        aud_voice(AUD_VOICES - 1, snd_test, 220, 0, 4096, 0);
        tx_printf("[sound %d/%d: %s, %u Hz, %u frames]\r\n", snd_test + 1, aud.nsnd, aud_name(snd_test),
                  (unsigned)aud_rate(snd_test), (unsigned)aud_frames(snd_test));
        break;
    }
    default: break;
    }
}

#include "control_runtime.h"
#include "input_runtime.h"

static void poll_uart(void)
{
    poll_input_devices();poll_video_trial();
    while (uart_readOccupancy(BSP_UART_TERMINAL)) {
        const uint8_t c = (uint8_t)uart_read(BSP_UART_TERMINAL);
        if (!pad_feed(c, now())) handle_char((char)c);   /* PC 端 pc_pad.py 的按键状态包另行处理 */
    }
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
    slots = game_hostile_used(g);                    /* slots above are all free */
    /* The pool cannot change during steering. Resolve its mode/capacity once,
       rather than calling the out-of-line lookup for every hostile bullet. */
    const Seed *pool = slots ? game_hostile_at(g,0) : 0;
    for (i = 0; pool && i < slots; i++) {
        const Seed *b = pool + i;
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

#define TICKS_PER_MS (BSP_CLINT_HZ / 1000u)
static Input manual_input(void)
{
    Input in;
    in.held = 0;
    if (pad_has_state()) {          /* pc_pad.py：真正的"按住"状态 */
        const uint32_t h = pad_held(now(), TICKS_PER_MS);
        if (h & PAD_UP)    in.held |= KEY_UP;
        if (h & PAD_DOWN)  in.held |= KEY_DOWN;
        if (h & PAD_LEFT)  in.held |= KEY_LEFT;
        if (h & PAD_RIGHT) in.held |= KEY_RIGHT;
        in.pressed = 0;
        return in;
    }
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
#if SPROUT_STATS                          /* 启动测速和 'y' 剖析：只在 STATS=1 版里编译 */
#define BENCH_N 4
static const char *const bench_name[BENCH_N] = {
    "full-screen fill ", "full-screen copy ", "full-screen alpha", "2000 x 8x8 fills " };
static const uint32_t bench_bytes[BENCH_N] = {
    FB_BYTES, FB_BYTES, FB_BYTES, 2000u * 8u * 8u * 2u };

static void begin_target(Gfx *gfx, GfxHwTarget *t, int hw, uint32_t fb)
{
    view_begin(gfx, t, hw, fb);
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

/* Per-command CPU cost of small keyed sprites (the bullet-hell case): the
 * same 2000 9x9 sprites once through gfx_blit one by one, once through
 * gfx_blit_list. "queued" = CPU time until the last command is in the queue,
 * "done" = until the engine has finished as well. */
/* ---------------------------------------------------------------------------
 * Primitive memory-access costs (ns per operation, loop overhead included, see
 * the "empty loop" column): what does ONE store / load / APB access cost the CPU?
 * The D$ is write-through, so every store to DDR is a bus write; this shows how
 * expensive that is, alone and while the BitBlt engine is using the DDR too.
 * OCR = the 4 KB on-chip RAM at SYSTEM_RAM_A_CTRL (last word: only the dead
 * bootloader stack lives there; the value is restored).
 * ------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------
 * D$ size probe (measures the real hardware, whatever BSP was compiled in):
 * read S bytes at 64 B stride twice, time the 2nd pass. Fits in the D$ ->
 * ~hit cost (~90 ns/line), larger than the D$ -> ~miss cost (~740 ns/line).
 * The first size that jumps to miss cost is twice the D$ size. Engine idle.
 * ------------------------------------------------------------------------- */
static void cache_probe(uint32_t addr)
{
    static const uint32_t kb[7] = { 2, 4, 8, 16, 32, 64, 128 };
    volatile uint32_t *const p = (volatile uint32_t *)addr;
    uint32_t sink = 0, t0, t, lines;
    int k, pass, i;

    blit_dcache_invalidate();
    bsp_printf("  D$ probe, ns per 64 B line on the 2nd pass over S KB (hit ~90, miss ~740):");
    for (k = 0; k < 7; k++) {
        lines = kb[k] * 1024u / 64u;
        t = 0;
        for (pass = 0; pass < 2; pass++) {
            t0 = now();
            for (i = 0; i < (int)lines; i++) sink += p[i * 16];
            t = now() - t0;
        }
        bsp_printf(" %dK %d", (int)kb[k], (int)((uint64_t)t * 1000u / TICKS_PER_US / lines));
    }
    bsp_printf("%s\r\n", sink == 0x12345678u ? " " : "");
}

#define MEM_BENCH_N 2000
#define MEM_BARRIER() __asm__ volatile("")
static void mem_bench(uint32_t fb_dst, uint32_t fb_src)
{
    static volatile uint32_t buf[MEM_BENCH_N];
    static const char *const name[9] = {
        "empty loop", "DDR store same word", "DDR store sequential", "DDR load hit",
        "DDR load miss (64 B stride)", "APB write", "APB read", "OCR store", "OCR load" };
    volatile uint32_t *const ocr = (volatile uint32_t *)(SYSTEM_RAM_A_CTRL + SYSTEM_RAM_A_CTRL_SIZE - 4u);
    volatile uint32_t *const far = (volatile uint32_t *)fb_src;
    uint32_t res[2][9], sink = 0, t0;
    const uint32_t ocr_old = *ocr;
    int busy, test, i;

    for (busy = 0; busy < 2; busy++) {
        for (test = 0; test < 9; test++) {
            blit_wait();
            if (busy)                         /* 3 full-screen copies: engine reads + writes DDR meanwhile */
                for (i = 0; i < 3; i++)
                    blit_copy(fb_dst, SCREEN_W * 2u, fb_src, SCREEN_W * 2u, SCREEN_W, SCREEN_H, 0, BLIT_OPAQUE, 0, 0);
            t0 = now();
            switch (test) {
            case 0: for (i = 0; i < MEM_BENCH_N; i++) MEM_BARRIER(); break;
            case 1: for (i = 0; i < MEM_BENCH_N; i++) buf[0] = (uint32_t)i; break;
            case 2: for (i = 0; i < MEM_BENCH_N; i++) buf[i] = (uint32_t)i; break;
            case 3: for (i = 0; i < MEM_BENCH_N; i++) sink += buf[0]; break;
            case 4: for (i = 0; i < MEM_BENCH_N; i++) sink += far[i * 16]; break;
            case 5: for (i = 0; i < MEM_BENCH_N; i++) blit_wr(BLIT_R_KEY, blit_st.key); break;
            case 6: for (i = 0; i < MEM_BENCH_N; i++) sink += blit_rd(BLIT_R_ID); break;
            case 7: for (i = 0; i < MEM_BENCH_N; i++) *ocr = (uint32_t)i; break;
            default: for (i = 0; i < MEM_BENCH_N; i++) sink += *ocr; break;
            }
            res[busy][test] = now() - t0;
        }
    }
    blit_wait();
    *ocr = ocr_old;
    buf[0] = sink;
    blit_dcache_invalidate();
    tx_printf("  memory access cost, ns per operation (%d ops each; engine idle / engine busy copying):\r\n",
              MEM_BENCH_N);
    for (test = 0; test < 9; test++)
        tx_printf("    %5u / %5u  %s\r\n",
                  (uint32_t)((uint64_t)res[0][test] * 1000u / TICKS_PER_US / MEM_BENCH_N),
                  (uint32_t)((uint64_t)res[1][test] * 1000u / TICKS_PER_US / MEM_BENCH_N), name[test]);
    tx_flush();
}
#endif /* SPROUT_STATS */

/* Game default for the engine LIMIT (after the boot benches).
 * Board 2026-10-03: read bursts 2 gave ~39 fps vs ~36 at 16, but makes small
 * sprites 1.7x slower. A read-word limit keeps big copies from queueing more
 * than a few hundred bytes ahead of the CPU while small sprites keep 16 bursts.
 * 板上 'u' 实测 64 字最好（发行版没有 'u' 键，所以默认直接用 64）。 */
#ifndef GAME_LIMIT_WORDS
#define GAME_LIMIT_WORDS 64u
#endif
static void set_game_limit(void)
{
    if (!blit_st.has_limit) return;
    if (blit_st.has_limit_words) {
        blit_set_limit(BLIT_LIMIT_RD_MAX, BLIT_LIMIT_WR_MAX);
        blit_set_rdwords(GAME_LIMIT_WORDS);
    } else {
        blit_set_limit(2, 2);                 /* older bitstream: best burst limit on the board */
    }
}

#if SPROUT_STATS

/* ---------------------------------------------------------------------------
 * Engine LIMIT trade-off: for each (read, write) outstanding-burst limit,
 *   store / miss = CPU cost of one DDR store / one D$ miss while the engine copies,
 *   copy         = one full-screen copy (large bursts),
 *   sprites      = 2000 keyed 9x9 copies until the engine is idle (small bursts).
 * ------------------------------------------------------------------------- */
static void limit_bench(uint32_t fb_dst, uint32_t fb_src)
{
    /* {read bursts, write bursts, read words in flight} */
    static const uint16_t set[11][3] = { {16,128,1024}, {8,16,1024}, {4,8,1024}, {2,8,1024}, {2,2,1024},
                                         {1,1,1024}, {16,128,256}, {16,128,128}, {16,128,64}, {16,128,32},
                                         {16,128,16} };
    static volatile uint32_t buf[1000];
    volatile uint32_t *const far = (volatile uint32_t *)fb_src;
    uint32_t sink = 0, t0, st, ms, cp, sp;
    int k, i;

    if (!blit_st.has_limit) return;
    tx_puts("  engine LIMIT (rd/wr bursts, rd words): CPU store ns, CPU miss ns (engine busy) | full copy us | 2000 sprites us\r\n");
    for (k = 0; k < 11; k++) {
        if (set[k][2] != 1024u && !blit_st.has_limit_words) break;
        blit_st.rw_max = BLIT_LIMIT_RW_MAX;
        blit_set_rdwords(set[k][2]);
        blit_set_limit(set[k][0], set[k][1]);
        blit_wait();
        for (i = 0; i < 4; i++)
            blit_copy(fb_dst, SCREEN_W * 2u, fb_src, SCREEN_W * 2u, SCREEN_W, SCREEN_H, 0, BLIT_OPAQUE, 0, 0);
        t0 = now();
        for (i = 0; i < 1000; i++) buf[i] = (uint32_t)i;
        st = now() - t0;
        blit_wait();
        for (i = 0; i < 4; i++)
            blit_copy(fb_dst, SCREEN_W * 2u, fb_src, SCREEN_W * 2u, SCREEN_W, SCREEN_H, 0, BLIT_OPAQUE, 0, 0);
        t0 = now();
        for (i = 0; i < 500 && blit_busy(); i++) sink += far[(k * 500 + i) * 16];
        ms = i ? (now() - t0) / (uint32_t)i : 0;
        blit_wait();
        t0 = now();
        blit_copy(fb_dst, SCREEN_W * 2u, fb_src, SCREEN_W * 2u, SCREEN_W, SCREEN_H, 0, BLIT_OPAQUE, 0, 0);
        blit_wait();
        cp = now() - t0;
        t0 = now();
        for (i = 0; i < 2000; i++)
            blit_copy(fb_dst + (uint32_t)((i * 53) % (SCREEN_H - 9)) * SCREEN_W * 2u + (uint32_t)((i * 37) % (SCREEN_W - 9)) * 2u,
                      SCREEN_W * 2u, fb_src, SCREEN_W * 2u, 9, 9, BLIT_KEY, BLIT_OPAQUE, 0, 0);
        blit_wait();
        sp = now() - t0;
        tx_printf("    rd %2u wr %3u words %4u: store %5u  miss %5u | copy %6u | sprites %6u\r\n",
                  (uint32_t)set[k][0], (uint32_t)set[k][1], (uint32_t)set[k][2],
                  (uint32_t)((uint64_t)st * 1000u / TICKS_PER_US / 1000u),
                  (uint32_t)((uint64_t)ms * 1000u / TICKS_PER_US),
                  cp / TICKS_PER_US, sp / TICKS_PER_US);
        tx_flush();
    }
    buf[0] = sink + buf[1];
    blit_set_rdwords(BLIT_LIMIT_RW_MAX);
    blit_set_limit(BLIT_LIMIT_RD_MAX, BLIT_LIMIT_WR_MAX);
    blit_dcache_invalidate();
}

/*
 * 'y': where the ~2.6 us per seed goes, on the live game state.
 * Each part of render_seed_part() runs twice: engine idle (blit_wait first) and
 * engine busy (3 full-screen background copies queued first, as in a frame
 * where the CPU's DDR reads queue behind engine bursts).
 *   cpu   = CPU time minus queue-full stalls, stall = waiting for queue room,
 *   done  = until the engine finished as well (busy: includes the 3 copies).
 * The draws land in the back buffer and are overwritten by the frame's background.
 */
static uint32_t ns_per(uint32_t ticks, int n)
{
    return n > 0 ? (uint32_t)((uint64_t)ticks * 1000u / TICKS_PER_US / (uint32_t)n) : 0u;
}

static void seed_profile(Gfx *gfx, GfxHwTarget *tgt, uint32_t fb, const Game *g)
{
    static const char *const name[5] = { "walk  ", "build ", "submit", "tiny  ", "full  " };
    uint32_t cpu[2][5], stl[2][5], done[2][5], t0, s0, t;
    int n[5], slots, busy, p, k;
    const int band_was = blit_st.band;

    if (band_was) blit_set_band(0);              /* 这个剖析量的是逐条直接画的提交路径 */
    view_begin(gfx, tgt, 1, fb);
    slots = render_seed_part(gfx, g, SEED_PART_PREP);
    for (busy = 0; busy < 2; busy++)
        for (p = 0; p < 5; p++) {
            blit_wait();
            if (busy) for (k = 0; k < 3; k++) render_seed_part(gfx, g, SEED_PART_BG);
            gfx_end(gfx);                        /* LIST mode: start the copies now */
            s0 = blit_st.stall_ticks;
            t0 = now();
            n[p] = render_seed_part(gfx, g, p);
            t = now();
            gfx_end(gfx);
            stl[busy][p] = blit_st.stall_ticks - s0;
            cpu[busy][p] = t - t0 > stl[busy][p] ? t - t0 - stl[busy][p] : 0u;
            blit_wait();
            done[busy][p] = now() - t0;
        }
    tx_printf("seed profile: %d seeds drawn, %d pool slots walked, mode %s (full: %s), LIMIT rd %u wr %u words %u\r\n",
              n[SEED_PART_FULL], slots, seed_mode_name[seed_mode()], render_seed_table ? "table" : "pool",
              blit_st.rd_max, blit_st.wr_max, blit_st.rw_max);
    tx_puts("  ns per seed      engine idle: cpu stall  done | engine busy: cpu stall  done\r\n");
    for (p = 0; p < 5; p++)
        tx_printf("  %s %5d       %5u %5u %5u |              %5u %5u %5u\r\n", name[p], n[p],
                  ns_per(cpu[0][p], n[p]), ns_per(stl[0][p], n[p]), ns_per(done[0][p], n[p]),
                  ns_per(cpu[1][p], n[p]), ns_per(stl[1][p], n[p]), ns_per(done[1][p], n[p]));
    tx_printf("  full loop: idle %u us, busy %u us (cpu, without stalls)\r\n",
              cpu[0][SEED_PART_FULL] / TICKS_PER_US, cpu[1][SEED_PART_FULL] / TICKS_PER_US);
    if (band_was) blit_set_band(1);
}

/*
 * '8': engine time per part of the real frame, with the CPU idle while the engine runs.
 * hwlist mode only: the commands of one part are first only collected in the DDR
 * list (blit_st.lhold, the engine stays idle, "cpu" = submit time), then published
 * at once while the CPU just polls STATUS (APB, no DDR traffic). Parts:
 *   bg    = one full-screen background copy
 *   seeds = the hostile seed loop (TAB when on)
 *   frame = the whole render_game (contains one bg and the seeds)
 *   rest  = frame - bg - seeds (enemies, gems, bullets, particles, HUD)
 * "live" = the same part submitted normally (engine runs while the CPU submits).
 * The busy split comes from the stall counters (CAPS[29], see blit.h): write +
 * the four PIXEL columns + "wide" (clocks on the 128-bit path, CAPS[30]) add up
 * to 100 % of the engine busy clocks (the "ld" column = commands / busy, only
 * informational since the 2026-10-07 PIXEL). clk/wd and MB/s count wide beats
 * as 4 words / 16 bytes.
 */
enum { EP_BG, EP_SEEDS, EP_FRAME, EP_N };
static uint32_t pct_of(uint32_t part, uint32_t total);

static void ep_row(const char *name, const BlitPerf *d, uint32_t eng, uint32_t cpu, uint32_t live, int held)
{
    uint32_t b = d->busy ? d->busy : 1u;
    uint32_t wd = d->wbeat + 4u * d->xwbeat;   /* 32-bit words written (wide beat = 4 words) */
    tx_printf("  %s %5u %6u %6u %5u.%02u | %3u %3u %3u %3u %3u %3u %3u | %3u %3u %3u %3u | %4u %4u | %6u %6u%s\r\n",
              name, d->cmds, eng / TICKS_PER_US, d->busy / BLIT_CLK_MHZ,
              wd ? d->busy / wd : 0u, wd ? (d->busy % wd) * 100u / wd : 0u,
              pct_of(d->wbeat, b), pct_of(d->cmds, b), pct_of(d->st[BLIT_ST_PXWAIT], b),
              pct_of(d->st[BLIT_ST_PXFULL], b), pct_of(d->st[BLIT_ST_PXBUB], b), pct_of(d->st[BLIT_ST_PXIDLE], b),
              pct_of(d->wclk, b),
              pct_of(d->st[BLIT_ST_WSTALL], b), pct_of(d->st[BLIT_ST_AWWAIT], b),
              pct_of(d->st[BLIT_ST_AWSTALL], b), pct_of(d->st[BLIT_ST_ARSTALL], b),
              (uint32_t)((uint64_t)(d->rbeat + 4u * d->xrbeat) * 4u * BLIT_CLK_MHZ / b),
              (uint32_t)((uint64_t)wd * 4u * BLIT_CLK_MHZ / b),
              cpu / TICKS_PER_US, live / TICKS_PER_US, held ? "" : "  (list wrapped: not isolated)");
}

static uint32_t ep_sub(uint32_t a, uint32_t b, uint32_t c) { return a > b + c ? a - b - c : 0u; }

static void engine_profile(Gfx *gfx, GfxHwTarget *tgt, uint32_t fb, const Game *g, RenderInfo info)
{
    static const char *const name[EP_N] = { "bg   ", "seeds", "frame" };
    BlitPerf a, b, d[EP_N], r, bd;
    uint32_t eng[EP_N], cpu[EP_N], live[EP_N], t0, t1;
    uint32_t beng = 0, bcpu = 0, blive = 0;
    int p, live_run, held[EP_N], k, bheld = 0;
    const int band_was = blit_st.band;

    if (blit_st.mode != BLIT_MODE_LIST) { tx_puts("[engine profile needs hwlist seeds: press 'k' until HW command list]\r\n"); return; }
    if (band_was) blit_set_band(0);              /* bg / seeds / frame：直接画（分带关），和以前的数可比 */
    for (p = 0; p < EP_N; p++)
        for (live_run = 0; live_run < 2; live_run++) {
            blit_wait();
            view_begin(gfx, tgt, 1, fb);          /* rewinds the list (idle) */
            if (!live_run) blit_st.lhold = 1;
            t0 = now();
            if (p == EP_BG)         render_seed_part(gfx, g, SEED_PART_BG);
            else if (p == EP_SEEDS) render_seed_part(gfx, g, SEED_PART_FULL);
            else                    render_game(g, gfx, info);
            gfx_end(gfx);                         /* held: no LIST_END yet */
            if (live_run) { blit_wait(); live[p] = now() - t0; continue; }
            cpu[p] = now() - t0;
            held[p] = blit_st.lhold;
            blit_perf_snap(&a);
            t1 = now();
            blit_list_release();
            blit_wait();
            eng[p] = now() - t1;
            blit_perf_snap(&b);
            blit_perf_diff(&d[p], &a, &b);
        }
    /* 整帧再按分带合成画一次（BANDS..ENDB 先攒着，放开后引擎单独跑：第 1 阶段分桶 + 逐带画 + 写回） */
    memset(&bd, 0, sizeof(bd));
    if (blit_st.has_band && blit_set_band(1)) {
        for (live_run = 0; live_run < 2; live_run++) {
            blit_wait();
            view_begin(gfx, tgt, 1, fb);
            if (!live_run) blit_st.lhold = 1;
            t0 = now();
            render_game(g, gfx, info);
            gfx_end(gfx);                         /* ENDB written; held: not published yet */
            if (live_run) { blit_wait(); blive = now() - t0; continue; }
            bcpu = now() - t0;
            bheld = blit_st.lhold;
            blit_perf_snap(&a);
            t1 = now();
            blit_list_release();
            blit_wait();
            beng = now() - t1;
            blit_perf_snap(&b);
            blit_perf_diff(&bd, &a, &b);
        }
    }
    blit_set_band(band_was);

    /* rest = frame - bg - seeds */
    r.busy  = ep_sub(d[EP_FRAME].busy,  d[EP_BG].busy,  d[EP_SEEDS].busy);
    r.cmds  = ep_sub(d[EP_FRAME].cmds,  d[EP_BG].cmds,  d[EP_SEEDS].cmds);
    r.rbeat = ep_sub(d[EP_FRAME].rbeat, d[EP_BG].rbeat, d[EP_SEEDS].rbeat);
    r.wbeat = ep_sub(d[EP_FRAME].wbeat, d[EP_BG].wbeat, d[EP_SEEDS].wbeat);
    r.chit = r.cmiss = 0;
    for (k = 0; k < BLIT_ST_N; k++) r.st[k] = ep_sub(d[EP_FRAME].st[k], d[EP_BG].st[k], d[EP_SEEDS].st[k]);
    r.wclk   = ep_sub(d[EP_FRAME].wclk,   d[EP_BG].wclk,   d[EP_SEEDS].wclk);
    r.xwbeat = ep_sub(d[EP_FRAME].xwbeat, d[EP_BG].xwbeat, d[EP_SEEDS].xwbeat);
    r.xrbeat = ep_sub(d[EP_FRAME].xrbeat, d[EP_BG].xrbeat, d[EP_SEEDS].xrbeat);

    tx_printf("engine profile (CPU idle while the engine runs): %s seeds, cache %s, LIMIT rd %u wr %u words %u, wide %s%s\r\n",
              blit_st.has_table ? "TAB" : "word", blit_st.cflag ? "on" : "off",
              blit_st.rd_max, blit_st.wr_max, blit_st.rw_max,
              !blit_st.has_wide ? "n/a" : (blit_st.wide & BLIT_WIDE_OFF) ? "off" : "on",
              blit_st.has_stall ? "" : " | no stall counters (CAPS[29]) in this bitstream");
    tx_puts("  part   cmds eng_us busyus clk/wd | wr  ld  wait full bub idle wide % | Wst AWw AWs ARs % | rd/wr MB/s (busy) | cpu_us live_us\r\n");
    for (p = 0; p < EP_N; p++) { ep_row(name[p], &d[p], eng[p], cpu[p], live[p], held[p]); tx_flush(); }
    ep_row("rest ", &r, ep_sub(eng[EP_FRAME], eng[EP_BG], eng[EP_SEEDS]),
           ep_sub(cpu[EP_FRAME], cpu[EP_BG], cpu[EP_SEEDS]), ep_sub(live[EP_FRAME], live[EP_BG], live[EP_SEEDS]), 1);
    tx_flush();
    if (blit_st.has_band) {
        const uint32_t bb = bd.busy ? bd.busy : 1u;
        tx_printf("  band frame%s: engine %u us = phase1 %u + bands %u (write-back %u) us | K busy %u us, %u K beats, %u bin records\r\n",
                  blit_st.band_koff ? " (K path off)" : "", beng / TICKS_PER_US,
                  bd.band[BLIT_BP_P1] / BLIT_CLK_MHZ, bd.band[BLIT_BP_BAND] / BLIT_CLK_MHZ,
                  bd.band[BLIT_BP_FLUSH] / BLIT_CLK_MHZ, bd.band[BLIT_BP_KCLK] / BLIT_CLK_MHZ,
                  bd.band[BLIT_BP_KBEAT], bd.band[BLIT_BP_RECS]);
        tx_printf("              rd/wr %u %u MB/s (busy), cache hit %u miss %u | cpu %u us, live %u us%s"
                  " | direct frame above: %u us -> x%u.%02u\r\n",
                  (uint32_t)((uint64_t)(bd.rbeat + 4u * bd.xrbeat) * 4u * BLIT_CLK_MHZ / bb),
                  (uint32_t)((uint64_t)(bd.wbeat + 4u * bd.xwbeat) * 4u * BLIT_CLK_MHZ / bb),
                  bd.chit, bd.cmiss, bcpu / TICKS_PER_US, blive / TICKS_PER_US,
                  bheld ? "" : " (list wrapped: not isolated)", eng[EP_FRAME] / TICKS_PER_US,
                  beng ? eng[EP_FRAME] / beng : 0u, beng ? (eng[EP_FRAME] % beng) * 100u / beng : 0u);
        tx_flush();
    }
}

#define SEED_BENCH_N 2000
static void seed_bench(uint32_t fb)
{
    static Pixel px[9 * 9];
    static GfxItem items[SEED_BENCH_N];
    Sprite spr[1];
    Gfx gfx;
    GfxHwTarget tgt;
    /* pass: submission mode, draw list or one gfx_blit per sprite */
    static const struct { int mode, list; const char *name; } pass_def[5] = {
        { BLIT_MODE_APB,  0, "APB  gfx_blit calls" }, { BLIT_MODE_APB,  1, "APB  gfx_blit_list " },
        { BLIT_MODE_LIST, 0, "DDR  gfx_blit calls" }, { BLIT_MODE_LIST, 1, "DDR  gfx_blit_list " },
        { BLIT_MODE_WORD, 1, "WORD gfx_blit_list " } };
    uint32_t t0, sub, all, w0, k0;
    int i, pass, npass = blit_st.has_list ? 5 : 2;

    for (i = 0; i < 9 * 9; i++) px[i] = ((i % 9) + (i / 9)) & 1 ? RGB565(238, 112, 112) : COLOR_KEY;
    spr[0].width = 9; spr[0].height = 9; spr[0].pixels = px; spr[0].alpha = 0;
    for (i = 0; i < SEED_BENCH_N; i++) {
        items[i].x = (int16_t)((i * 37) % (SCREEN_W - 9));
        items[i].y = (int16_t)((i * 53) % (SCREEN_H - 9));
        items[i].sprite = 0;
    }
    tx_printf("  %d x 9x9 keyed sprites (HW): queued = CPU time, done = engine finished too\r\n",
              SEED_BENCH_N);
    for (pass = 0; pass < npass; pass++) {
#if BOOT_DIAG
        bsp_printf("[diag] seed pass %d (%s) start\r\n", pass, pass_def[pass].name);
#endif
        blit_set_mode(pass_def[pass].mode);
        view_begin(&gfx, &tgt, 1, fb);            /* fence: px[] -> DDR */
        w0 = blit_st.lwords; k0 = blit_st.lkicks;
        t0 = now();
        if (!pass_def[pass].list)
            for (i = 0; i < SEED_BENCH_N; i++) gfx_blit(&gfx, items[i].x, items[i].y, &spr[0], 1);
        else
            gfx_blit_list(&gfx, spr, items, SEED_BENCH_N, 1);
        sub = now() - t0;
#if BOOT_DIAG
        bsp_printf("[diag]   submitted, STATUS=%x\r\n", (int)blit_rd(BLIT_R_STATUS));
#endif
        gfx_end(&gfx);                        /* LIST: publishes the rest */
        blit_wait();
        all = now() - t0;
        tx_printf("    %s queued %6u us = %5u ns/cmd, done %6u us = %5u ns/cmd | list words %u kicks %u\r\n",
                  pass_def[pass].name,
                  sub / TICKS_PER_US, (uint32_t)((uint64_t)sub * 1000u / TICKS_PER_US / SEED_BENCH_N),
                  all / TICKS_PER_US, (uint32_t)((uint64_t)all * 1000u / TICKS_PER_US / SEED_BENCH_N),
                  blit_st.lwords - w0, blit_st.lkicks - k0);
        tx_flush();
    }
    blit_set_mode(BLIT_MODE_APB);
    blit_dcache_invalidate();
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
#endif /* SPROUT_STATS */


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
    uint32_t seeds, hud, scene;  /* submit + qstall split by render_game (render_split) */
    uint32_t seeds_n;            /* hostile seeds drawn */
    uint32_t sc[6];              /* scene split: bg+gems sort enemies pbullets particles */
    uint32_t lg[4];              /* logic split (game_split): enemies pbullets hostile rest */
    uint32_t logic_peak;
    int      ticked;
} Frame;

/* R6: wait only at presentation. Keep queued and in-flight framebuffers owned. */
static void present(Frame *f, uint32_t fb)
{
    if (vsync && hdmi_flip_pending()) {
        const uint32_t t0 = now();
        while (hdmi_flip_pending()) {
            tx_pump(); poll_uart();
            /* 卡死诊断：翻页等了 1 s 还没完成 = HDMI 扫描 DMA 停了（DDR 读口卡住？） */
            if (now() - t0 > TICKS_PER_US * 1000000u && !flip_stuck) {
                flip_stuck = 1;
                tx_flush();
                bsp_printf("\r\n!! HDMI flip pending > 1 s: STATUS=%x FRAME_CNT=%x FB_ADDR=%x FB_CUR=%x UF_CNT=%x\r\n",
                           (unsigned)HDMI_STATUS, (unsigned)HDMI_FRAME_CNT, (unsigned)HDMI_FB_ADDR,
                           (unsigned)HDMI_FB_CUR, (unsigned)HDMI_UF_CNT);
                if (hw_avail) blit_stuck("vsync wait (HDMI flip)");
            }
        }
        f->vsync += now() - t0;
    }
    hdmi_flip(fb);
}


typedef struct {                 /* sums over one window */
    uint32_t t0, frames, ticks;
    uint32_t present0,repeat0,scan0,video0,audio0,prs;
    uint32_t work_max,over_budget;
    uint32_t vsync, logic, logic_max, submit, qstall, tail, draw_max;
    uint32_t qend, qpeak;
    uint32_t seeds, hud, scene, seeds_n;
    uint32_t sc[6], lg[4];
    uint32_t hdmi0, busy0, rb0, wb0, cmds0, stalls0;
    uint32_t lwords0, lkicks0, lwraps0;
    uint32_t chit0, cmiss0;      /* engine source cache PERF_CHIT / PERF_CMISS */
    uint32_t hcmds0, st0[BLIT_ST_N]; /* engine PERF_CMDS and stall counters (CAPS[29]) */
    uint32_t wclk0, xrb0, xwb0;  /* wide path PERF_WCLK / XRBEAT / XWBEAT (CAPS[30]) */
    uint32_t bp0[BLIT_BP_N], bsplits0;   /* band mode counters 0xA4..0xB8 (CAPS[31]), split frames */
} Win;

typedef struct {                 /* results: us per frame (logic: per tick) */
    uint32_t el_us, frames, fps10, hdmi_fps;
    uint32_t logic10,present10,repeat10,scan10,video_delta,audio_delta,prs;
    uint32_t loop_work_us,work_max_us,over_budget;
    uint32_t vsync, logic, logic_max, submit, qstall, tail, other;
    uint32_t draw, draw_max, work, limit10;
    uint32_t pct[6];             /* share of the window: logic submit qstall tail vsync other */
    uint32_t qend, qpeak, stalls, cmds;
    uint32_t blt;                /* engine busy, % of the window */
    uint32_t rd_mbs, wr_mbs;     /* engine AXI bytes / window time */
    uint32_t rd_busy, wr_busy;   /* engine AXI bytes / engine busy time */
    uint32_t seeds, hud, scene;  /* submit split, us per frame */
    uint32_t seeds_n, seed_ns;   /* seeds per frame, ns per seed */
    uint32_t sc[6], lg[4];       /* scene split us per frame, logic split us per tick */
    uint32_t lwords, lkicks, lwraps; /* command list: words / LIST_END writes per frame, wraps */
    uint32_t chit, cmiss, chit_pct;  /* source cache line requests per frame, hit % */
    uint32_t wb_pct, ld_pct, st_pct[BLIT_ST_N]; /* share of engine busy: W beats, command loads, stalls */
    uint32_t wd_pct;             /* share of engine busy on the 128-bit wide path (CAPS[30]) */
    uint32_t bp[BLIT_BP_N];      /* band mode, per frame: clocks (P1 FLUSH BAND KCLK) / counts (KBEAT RECS) */
    uint32_t bsplits;            /* band frames ended early (list wrap) in the window */
} WinStats;

static Win hud_w, uart_w;

static void win_begin(Win *w, uint32_t t)
{
    memset(w, 0, sizeof(*w));
    w->t0 = t;
    w->hdmi0 = hdmi_frame_cnt();
    w->prs=HDMI_REG(0x4c)==0x50525332u;
    if(w->prs){w->present0=HDMI_REG(0x40);w->repeat0=HDMI_REG(0x44);w->scan0=HDMI_REG(0x48);}
    w->video0=hdmi_uf_cnt();w->audio0=HDMI_REG(0x2c);
#if SPROUT_STATS                          /* 发行版只要帧数和时间，不读引擎计数器 */
    if (hw_avail) {
        w->busy0 = blit_rd(BLIT_R_PERF_BUSY);
        w->rb0   = blit_rd(BLIT_R_PERF_RB);
        w->wb0   = blit_rd(BLIT_R_PERF_WB);
        if (blit_st.has_cache) {
            w->chit0  = blit_rd(BLIT_R_PERF_CHIT);
            w->cmiss0 = blit_rd(BLIT_R_PERF_CMISS);
        }
        if (blit_st.has_stall) {
            int k;
            w->hcmds0 = blit_rd(BLIT_R_PERF_CMDS);
            for (k = 0; k < BLIT_ST_N; k++) w->st0[k] = blit_rd(BLIT_R_PERF_WSTALL + 4u * (uint32_t)k);
        }
        if (blit_st.has_wide) {
            w->wclk0 = blit_rd(BLIT_R_PERF_WCLK);
            w->xrb0  = blit_rd(BLIT_R_PERF_XRBEAT);
            w->xwb0  = blit_rd(BLIT_R_PERF_XWBEAT);
        }
        if (blit_st.has_band) {
            int k;
            for (k = 0; k < BLIT_BP_N; k++) w->bp0[k] = blit_rd(BLIT_R_PERF_P1 + 4u * (uint32_t)k);
        }
    }
    w->bsplits0 = blit_st.bsplits;
    w->cmds0   = blit_st.cmds;
    w->stalls0 = blit_st.stalls;
    w->lwords0 = blit_st.lwords;
    w->lkicks0 = blit_st.lkicks;
    w->lwraps0 = blit_st.lwraps;
#endif
}

static void win_add(Win *w, const Frame *f)
{
    uint32_t draw = f->submit + f->qstall + f->tail;
    ++w->frames;
    {uint32_t work=f->logic+draw;if(work>w->work_max)w->work_max=work;
     if(work>(hdmi_mode_1080()?2u*TICK_PERIOD:TICK_PERIOD))++w->over_budget;}
    w->vsync  += f->vsync;
    w->submit += f->submit;
    w->qstall += f->qstall;
    w->tail   += f->tail;
    w->qend   += f->qend;
    w->seeds  += f->seeds;
    w->hud    += f->hud;
    w->scene  += f->scene;
    w->seeds_n += f->seeds_n;
    { int k; for (k = 0; k < 6; k++) w->sc[k] += f->sc[k]; }
    if (f->qpeak > w->qpeak) w->qpeak = f->qpeak;
    if (draw > w->draw_max)  w->draw_max = draw;
    if (f->ticked) {
        int k;
        w->ticks += (uint32_t)f->ticked;
        w->logic += f->logic;
        for (k = 0; k < 4; k++) w->lg[k] += f->lg[k];
        if (f->logic_peak > w->logic_max) w->logic_max = f->logic_peak;
    }
}

static uint32_t pct_of(uint32_t part, uint32_t total)
{
    return total ? (uint32_t)(((uint64_t)part * 100u + total / 2u) / total) : 0u;
}

static uint32_t per_us(uint32_t ticks, uint32_t n)
{
    return n ? (uint32_t)((uint64_t)ticks*1000000u/((uint64_t)n*BSP_CLINT_HZ)) : 0u;
}

static void win_end(const Win *w, uint32_t t, WinStats *s)
{
    uint32_t el = t - w->t0, n = w->frames, used, other;
#if SPROUT_STATS
    uint32_t busy = 0, rb = 0, wb = 0;
    uint64_t rbytes, wbytes;
#endif

    memset(s, 0, sizeof(*s));
    if (!el) el = 1u;
    s->el_us  = per_us(el,1) ? per_us(el,1) : 1u;
    s->frames = n;
    s->logic10=(uint32_t)(((uint64_t)w->ticks*BSP_CLINT_HZ*10u+el/2u)/el);
    s->prs=w->prs&&HDMI_REG(0x4c)==0x50525332u;
    if(s->prs){
        s->present10=(uint32_t)(((uint64_t)(HDMI_REG(0x40)-w->present0)*BSP_CLINT_HZ*10u+el/2u)/el);
        s->repeat10=(uint32_t)(((uint64_t)(HDMI_REG(0x44)-w->repeat0)*BSP_CLINT_HZ*10u+el/2u)/el);
        s->scan10=(uint32_t)(((uint64_t)(HDMI_REG(0x48)-w->scan0)*BSP_CLINT_HZ*10u+el/2u)/el);
    }
    s->video_delta=hdmi_uf_cnt()-w->video0;s->audio_delta=HDMI_REG(0x2c)-w->audio0;
    s->loop_work_us=per_us(w->logic+w->submit+w->qstall+w->tail,n);
    s->work_max_us=per_us(w->work_max,1);s->over_budget=w->over_budget;
    s->fps10  = (uint32_t)(((uint64_t)n * BSP_CLINT_HZ * 10u + el / 2u) / el);
    s->hdmi_fps = (uint32_t)(((uint64_t)(hdmi_frame_cnt() - w->hdmi0) * BSP_CLINT_HZ + el / 2u) / el);

    s->vsync     = per_us(w->vsync, n);
    s->logic     = per_us(w->logic, w->ticks);
    s->logic_max = per_us(w->logic_max,1);
    s->submit    = per_us(w->submit, n);
    s->qstall    = per_us(w->qstall, n);
    s->tail      = per_us(w->tail, n);
    s->draw      = per_us(w->submit + w->qstall + w->tail, n);
    s->draw_max  = per_us(w->draw_max,1);
    s->work      = per_us(w->logic,w->frames) + s->draw;          /* a frame that also ticks */
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

#if SPROUT_STATS
    s->seeds   = per_us(w->seeds, n);
    s->hud     = per_us(w->hud, n);
    s->scene   = per_us(w->scene, n);
    s->seeds_n = n ? (w->seeds_n + n / 2u) / n : 0u;
    { int k; for (k = 0; k < 6; k++) s->sc[k] = per_us(w->sc[k], n);
             for (k = 0; k < 4; k++) s->lg[k] = per_us(w->lg[k], w->ticks); }
    s->seed_ns = w->seeds_n ? (uint32_t)((uint64_t)w->seeds * 1000000000u / ((uint64_t)BSP_CLINT_HZ*w->seeds_n)) : 0u;
    s->qend   = n ? (w->qend + n / 2u) / n : 0u;
    s->qpeak  = w->qpeak;
    s->stalls = blit_st.stalls - w->stalls0;
    s->cmds   = n ? (blit_st.cmds - w->cmds0 + n / 2u) / n : 0u;
    s->lwords = n ? (blit_st.lwords - w->lwords0 + n / 2u) / n : 0u;
    s->lkicks = n ? (blit_st.lkicks - w->lkicks0 + n / 2u) / n : 0u;
    s->lwraps = blit_st.lwraps - w->lwraps0;

    if (hw_avail) {
        busy = blit_rd(BLIT_R_PERF_BUSY) - w->busy0;
        rb   = blit_rd(BLIT_R_PERF_RB)   - w->rb0;
        wb   = blit_rd(BLIT_R_PERF_WB)   - w->wb0;
        if (blit_st.has_cache) {
            uint32_t h = blit_rd(BLIT_R_PERF_CHIT) - w->chit0, m = blit_rd(BLIT_R_PERF_CMISS) - w->cmiss0;
            s->chit  = n ? (h + n / 2u) / n : 0u;
            s->cmiss = n ? (m + n / 2u) / n : 0u;
            s->chit_pct = pct_of(h, h + m);
        }
        if (blit_st.has_stall) {
            int k;
            s->wb_pct = pct_of(wb, busy);
            s->ld_pct = pct_of(blit_rd(BLIT_R_PERF_CMDS) - w->hcmds0, busy);
            for (k = 0; k < BLIT_ST_N; k++)
                s->st_pct[k] = pct_of(blit_rd(BLIT_R_PERF_WSTALL + 4u * (uint32_t)k) - w->st0[k], busy);
        }
    }
    rbytes = (uint64_t)rb * 4u;                 /* 32-bit AXI beats */
    wbytes = (uint64_t)wb * 4u;
    if (hw_avail && blit_st.has_band) {         /* band mode: per frame */
        int k;
        for (k = 0; k < BLIT_BP_N; k++)
            s->bp[k] = n ? (blit_rd(BLIT_R_PERF_P1 + 4u * (uint32_t)k) - w->bp0[k]) / n : 0u;
    }
    s->bsplits = blit_st.bsplits - w->bsplits0;
    if (hw_avail && blit_st.has_wide) {         /* + 128-bit wide path beats (16 B) */
        s->wd_pct = pct_of(blit_rd(BLIT_R_PERF_WCLK) - w->wclk0, busy);
        rbytes += (uint64_t)(blit_rd(BLIT_R_PERF_XRBEAT) - w->xrb0) * 16u;
        wbytes += (uint64_t)(blit_rd(BLIT_R_PERF_XWBEAT) - w->xwb0) * 16u;
    }
    s->blt     = pct_of(busy,el);
    s->rd_mbs  = (uint32_t)(rbytes / s->el_us);            /* bytes per us = MB/s */
    s->wr_mbs  = (uint32_t)(wbytes / s->el_us);
    s->rd_busy = busy ? (uint32_t)(rbytes * BSP_CLINT_HZ / busy / 1000000u) : 0u;
    s->wr_busy = busy ? (uint32_t)(wbytes * BSP_CLINT_HZ / busy / 1000000u) : 0u;
#endif /* SPROUT_STATS */
}

/* 弹幕场景：每秒多一行状态 */
static void th_uart_line(void)
{
    static const char *const mname[5] = { "title", "play", "pause", "clear", "over" };
    static const char *const dname[TH_DIFFS] = { "normal", "lunatic", "stress" };
    int th_np;
    th_phase_list(&th, &th_np);
    tx_printf("  danmaku: %s %s%s | stage %d tick %u %sboss %d/%d | bullets %d (peak %d, drawn %d, dropped %d)"
              " enemies %d items %d fx %d | score %u graze %d lives %d bombs %d power %d\r\n",
              mname[th.mode], dname[th.diff], th.bot ? " demo" : "", th.stage + 1, (unsigned)th.stage_tick,
              th.boss.active && th.boss.mid ? "mid-" : "",
              th.boss.active ? th.boss.phase + 1 : 0, th_np, th.nbullets, th.peak_bullets,
              th.drawn, th.dropped, th.enemies_alive, th.items_alive, th.fx_alive,
              (unsigned)th.score, th.graze, th.lives, th.bombs, th.power);
    if (th.diff == TH_STRESS)
        tx_printf("  stress: x%d %s | slowest frame of the last 0.5 s %u us of %u (%u%%) | cap %d | level down %d up %d"
                  " | tick us: fire+boss+enemies %u, shots+bullets %u (%u ns/bullet), items+fx %u\r\n",
                  th.dens, th.stress_auto ? "AUTO" : "manual", (unsigned)th.gov_last, (unsigned)(hdmi_mode_1080()?2u*TH_FRAME_BUDGET_US:TH_FRAME_BUDGET_US),
                  (unsigned)(th.gov_last * 100u / (hdmi_mode_1080()?2u*TH_FRAME_BUDGET_US:TH_FRAME_BUDGET_US)), th.stress_cap, th.gov_downs, th.gov_ups,
                  (unsigned)(th.prof[0] / TICKS_PER_US), (unsigned)(th.prof[1] / TICKS_PER_US),
                  th.nbullets ? (unsigned)(th.prof[1] / TICKS_PER_US * 1000u / (uint32_t)th.nbullets) : 0u,
                  (unsigned)(th.prof[2] / TICKS_PER_US));
}

/* Machine-readable reports use the exact same normalized window in every scene.
 * Render loop, logical ticks and physical new scan frames are distinct metrics. */
static unsigned telemetry_seq;
static int measurement_scene=-1,measurement_phase=-1;
static Input player_game_input(unsigned who,uint32_t time){
    uint32_t h=pad_source_held(who,time,TICKS_PER_MS),p=pad_source_pressed(who,time,TICKS_PER_MS);Input in={0};
    if(h&PAD_LEFT)in.held|=KEY_LEFT;if(h&PAD_RIGHT)in.held|=KEY_RIGHT;
    if(h&PAD_UP)in.held|=KEY_UP;if(h&PAD_DOWN)in.held|=KEY_DOWN;
    if(p&PAD_BOMB)in.pressed|=KEY_SKILL;if(p&PAD_PAUSE)in.pressed|=KEY_PAUSE;
    if(p&(PAD_START|PAD_SHOT))in.pressed|=KEY_START;
    if(player_pick[who]>=1&&player_pick[who]<=3)in.pressed|=KEY_PICK1<<(player_pick[who]-1);
    player_pick[who]=0;
    if(who==0&&!pad_active(time,TICKS_PER_MS)){
        if(dir_x<0)in.held|=KEY_LEFT;if(dir_x>0)in.held|=KEY_RIGHT;
        if(dir_y<0)in.held|=KEY_UP;if(dir_y>0)in.held|=KEY_DOWN;
    }return in;
}

static void print_telemetry(const WinStats *s){
    unsigned id=++telemetry_seq;int count=scene==SCENE_SQUARES?sq_count:scene==SCENE_GAME?game_hostile_count(&game):scene==SCENE_TH?th.nbullets:3;
    GfxFontStats fs=gfx_font_stats();
    control_party_line("seq",id);control_input_state(0);if(video_trial)control_video_state(0);
    tx_printf("M4 seq=%u epoch=%u scene=%d phase=%d count=%d target=%d step=%d bullet=%d square=%d alpha=%d vsync=%d hw=%d motion=%d table=%d bgm=%d submit_mode=%d band=%d cache=%d overlap=%d overlap_active=%d seed_table=%d seed_build=%d sort=%d kpath=%d rd_bursts=%u wr_bursts=%u rd_words=%u wide=%u vpipe=%d kmode=%d ftab=%d square_path=%d th_stage=%d\r\n",
      id,settings_epoch,scene,scene==SCENE_TH?th.mode:scene==SCENE_GAME?game.phase:0,count,control_limit(),count_step,
      scene==SCENE_TH?th_pressure_scale():game_seed_size(),demo_square_size(),alpha_fx,vsync,use_hw,demo_motion_enabled(),demo_table_enabled(),music_enabled(),
      seed_mode(),blit_st.band,!!blit_st.cflag,overlap,use_hw&&overlap&&(scene!=SCENE_SQUARES||demo_fill_table_active())&&!hdmi_mode_1080(),
      blit_st.has_table,render_seed_table,render_sort_keys,!(blit_st.band_koff & BLIT_BAND_KOFF),blit_st.rd_max,blit_st.wr_max,blit_st.rw_max,blit_st.wide,vpipe,blit_band_kmode(),blit_st.has_ftab,demo_square_path(),th.stage);
    tx_printf("F4 seq=%u elapsed_us=%u render10=%u logic10=%u present10=%u repeat10=%u scan10=%u valid=%u video_uf=%u audio_uf=%u drops=%u\r\n",
      id,s->el_us,s->fps10,s->logic10,s->present10,s->repeat10,s->scan10,s->prs,s->video_delta,s->audio_delta,tx_dropped);
    tx_printf("P4 seq=%u logic_us=%u logic_max_us=%u submit_us=%u wait_us=%u work_us=%u work_max_us=%u over=%u busy=%u phase1_us=%u bands_us=%u profile=%d\r\n",
      id,s->logic,s->logic_max,s->submit,s->qstall+s->tail,s->loop_work_us,s->work_max_us,s->over_budget,s->blt,per_us(s->bp[BLIT_BP_P1],1),per_us(s->bp[BLIT_BP_BAND],1),SPROUT_STATS);
    tx_printf("C4 seq=%u font_hit=%u font_miss=%u font_reset=%u label_hit=%u label_miss=%u font_pixels=%u track=%s w=%u h=%u auto=%d density=%d stress_auto=%d tone=%d volume=%d bank_sounds=%d mixer=%d audio_ctrl=%u stats=%d verbose=%d music_auto=%d music_index=%d music_total=10 pressure_supported=1 motion_min_px=%d player_shots=%d locator_us=%u locator_on=%d enemy_us=%u player_logic_us=%u hostile_logic_us=%u hud_us=%u band_splits=%u band_overflow=%u core_hz=%u square_band_bound=%u players=%d\r\n",
      id,fs.hits,fs.misses,fs.resets,fs.label_hits,fs.label_misses,fs.used_pixels,aud_name(aud_bgm_current()),disp_w,disp_h,autoplay,th_stress_dens(&th),th.stress_auto,aud.tone,aud.master,aud.nsnd,aud.mixer,HDMI_REG(0x20),show_stats,report_verbose,music_follow_scene(),music_index(),demo_motion_min_size(),scene==SCENE_GAME?game.player_active:scene==SCENE_TH?th.shots_alive:0,s->sc[5],scene==SCENE_GAME&&!game.limit_mode,s->sc[2],s->lg[1],s->lg[2],s->hud,s->bsplits,hw_avail?((blit_rd(BLIT_R_STATUS)>>6)&1u):0u,(unsigned)BSP_CLINT_HZ,demo_band_bound(),scene==SCENE_GAME?game.player_count:scene==SCENE_TH?th.player_count:0);
}

/* Physical scan statistics: independent of render-loop FPS, audio ABI untouched. */
static void square_present_line(void)
{
    static uint32_t pp,pr,ps,pu,pa;static int valid;
    volatile uint32_t *h=(volatile uint32_t *)(uintptr_t)0xF8100000u;
    if(h[0x4c/4]==0x50525332u){
        uint32_t p=h[0x40/4],r=h[0x44/4],scan=h[0x48/4],u=h[0x18/4],a=h[0x2c/4];
        tx_printf("  physical scan: new %u repeat %u active %u videoUF %u audioUF %u (since previous report%s)\r\n",
          valid?p-pp:0,valid?r-pr:0,valid?scan-ps:0,valid?u-pu:0,valid?a-pa:0,valid?"":"; baseline");
        pp=p;pr=r;ps=scan;pu=u;pa=a;valid=1;
    }
}

#if !SPROUT_STATS
/* 发行版：每秒一行 */
static void print_window(const WinStats *s)
{
    tx_printf("fps %u.%u | %s %s | hdmi %u/s uf %u | work %u us (logic %u draw %u)%s\r\n",
              s->fps10 / 10u, s->fps10 % 10u, use_hw ? (blit_st.band ? "HW band" : "HW") : "CPU", vsync ? "vsync" : "free",
              s->hdmi_fps, hdmi_uf_cnt(), s->work, s->logic, s->draw,
              tx_dropped ? " | uart drops" : "");
    if (scene == SCENE_TH) th_uart_line();
    if (scene == SCENE_SQUARES) square_present_line();
    if (scene == SCENE_SQUARES) tx_printf("  squares %d %ux%u: motion %s avail %d err %d tick %u cycles engine %u seq %u table_frames %u\r\n",
       sq_count,(unsigned)disp_w,(unsigned)disp_h,demo_motion_enabled()?"HW":"CPU",demo_motion_available(),demo_motion_error(),
       demo_tick_cycles(),demo_motion_cycles(),demo_motion_sequence(),demo_table_frames());
}
#else
static void print_window(const WinStats *s, const RenderInfo *info, const Gfx *gfx)
{
    tx_printf("fps %u.%u | %s%s scene %d | %s%s%s | hdmi %u/s uf %u | ph %d foes %d\r\n",
              s->fps10 / 10u, s->fps10 % 10u, use_hw ? "HW" : "CPU",
              use_hw ? (blit_st.band ? ((overlap && scene!=SCENE_SQUARES) ? " band+overlap" : " band") : (overlap && scene!=SCENE_SQUARES) ? " overlap" : " serial") : "", scene,
              vsync ? "vsync" : "free", autoplay ? " auto" : "", alpha_fx ? " fx" : "",
              s->hdmi_fps, hdmi_uf_cnt(), (int)game.phase, game.enemy_count);
    if (scene == SCENE_TH) th_uart_line();
    if (scene == SCENE_SQUARES) square_present_line();
    if (scene == SCENE_SQUARES) tx_printf("  squares %d %ux%u: motion %s avail %d err %d tick %u cycles engine %u seq %u table_frames %u\r\n",
       sq_count,(unsigned)disp_w,(unsigned)disp_h,demo_motion_enabled()?"HW":"CPU",demo_motion_available(),demo_motion_error(),
       demo_tick_cycles(),demo_motion_cycles(),demo_motion_sequence(),demo_table_frames());
    if (aud.hdmi) {
        char line[200];
        aud_status_line(line, sizeof line);
        tx_printf("  %s\r\n", line);
    }
    tx_printf("  us/frame: logic %u (max %u) submit %u qstall %u tail_wait %u vsync_wait %u other %u"
              " | draw %u (max %u) work %u -> limit %u.%u fps\r\n",
              s->logic, s->logic_max, s->submit, s->qstall, s->tail, s->vsync, s->other,
              s->draw, s->draw_max, s->work, s->limit10 / 10u, s->limit10 % 10u);
    tx_printf("  share %%: logic %u submit %u qstall %u tail_wait %u vsync_wait %u other %u\r\n",
              s->pct[0], s->pct[1], s->pct[2], s->pct[3], s->pct[4], s->pct[5]);
    tx_printf("  submit split: seeds %u (%u seeds, %u ns each, %s %s) hud %u scene %u\r\n",
              s->seeds, s->seeds_n, s->seed_ns, seed_mode_name[seed_mode()],
              render_seed_table ? "table" : "pool", s->hud, s->scene);
    tx_printf("  locator: %u us (separate from enemies); player shots %d; band splits %u\r\n",s->sc[5],game.player_active,s->bsplits);
    tx_printf("  scene split: bg+gems %u sort %u (%s) enemies %u pbullets %u particles %u"
              " | logic split: enemies %u pbullets %u hostile %u rest %u\r\n",
              s->sc[0], s->sc[1], render_sort_keys ? "keys" : "structs", s->sc[2], s->sc[3], s->sc[4],
              s->lg[0], s->lg[1], s->lg[2], s->lg[3]);
    if (hw_avail)
        tx_printf("  blit: busy %u%% cmds %u/frame | queue end %u peak %u/%u stalls %u"
                  " | axi rd %u wr %u MB/s (while busy rd %u wr %u) | limit rd %u wr %u words %u\r\n",
                  s->blt, s->cmds, s->qend, s->qpeak, blit_st.depth, s->stalls,
                  s->rd_mbs, s->wr_mbs, s->rd_busy, s->wr_busy, blit_st.rd_max, blit_st.wr_max, blit_st.rw_max);
    if (hw_avail && blit_st.has_cache)
        tx_printf("  src cache %s: hit %u miss %u line req/frame = %u%% hits | INV %u total\r\n",
                  blit_st.cflag ? "ON" : "OFF", s->chit, s->cmiss, s->chit_pct, blit_st.cinvs);
    if (hw_avail && blit_st.has_stall)          /* write + 后 4 项 + wide = 100%（引擎 BUSY 的每个时钟）；load 只是命令数 / busy，仅供参考 */
        tx_printf("  blit busy %%: write %u load %u | wait-read %u wr-full %u bubble %u no-cmd %u wide %u"
                  " | axi W-stall %u AW-wait %u AW-stall %u AR-stall %u\r\n",
                  s->wb_pct, s->ld_pct, s->st_pct[BLIT_ST_PXWAIT], s->st_pct[BLIT_ST_PXFULL],
                  s->st_pct[BLIT_ST_PXBUB], s->st_pct[BLIT_ST_PXIDLE], s->wd_pct, s->st_pct[BLIT_ST_WSTALL],
                  s->st_pct[BLIT_ST_AWWAIT], s->st_pct[BLIT_ST_AWSTALL], s->st_pct[BLIT_ST_ARSTALL]);
    if (hw_avail && blit_st.has_band) {         /* 分带：每帧第 1 阶段（分桶）/ 第 2 阶段（逐带画 + 写回）的引擎时间 */
        uint32_t st = blit_status();
        tx_printf("  band %s%s: phase1 %u us + bands %u us (write-back %u) /frame | K busy %u us, %u K beats, %u bin records /frame"
                  " | split frames %u | %s%s\r\n",
                  blit_st.band ? "ON" : "off", blit_st.band_koff ? " (K path off)" : "",
                  per_us(s->bp[BLIT_BP_P1],1), per_us(s->bp[BLIT_BP_BAND],1),
                  per_us(s->bp[BLIT_BP_FLUSH],1), per_us(s->bp[BLIT_BP_KCLK],1),
                  s->bp[BLIT_BP_KBEAT], s->bp[BLIT_BP_RECS], s->bsplits,
                  (st & BLIT_ST_BOVF) ? "BIN OVERFLOW " : "", (st & BLIT_ST_BERR) ? "BAD RECORD" : "ok");
    }
    if (hw_avail && list_mode != BLIT_MODE_APB) {
        uint32_t st = blit_status();
        tx_printf("  cmdlist: %s words %u/frame kicks %u/frame wraps %u | STATUS list %u lerr %u\r\n",
                  list_mode == BLIT_MODE_LIST ? "DDR" : "LIST_WORD", s->lwords, s->lkicks, s->lwraps,
                  (unsigned)((st >> 3) & 1u), (unsigned)((st >> 4) & 1u));
    }
    tx_printf("  gfx: fill %u blit %u alpha %u/%u px %u | CPU %u fps %u us / HW %u fps %u us",
              gfx->fills, gfx->blits, gfx->alphas, gfx->alpha_pixels, gfx->pixels_written,
              (unsigned)info->fps_mode[0], (unsigned)info->draw_mode[0],
              (unsigned)info->fps_mode[1], (unsigned)info->draw_mode[1]);
    if (tx_dropped) tx_printf(" | uart lines dropped %u", tx_dropped);
    tx_puts("\r\n");
}
#endif

/* ------------------------------------------------------------------------- */
/* logic: at most one 1/60 s step per rendered frame; fills f->ticked / logic / lg */
static void run_logic_tick(Frame *f, uint32_t frame, uint32_t *next_tick)
{
    uint32_t t0 = now(), t1;
    Input in;
    f->ticked = (vsync && !hdmi_mode_1080()) || (int32_t)(t0 - *next_tick) >= 0;
    if (f->ticked) {
        if (scene == SCENE_TH) {
            /* 输入：pc_pad.py 的真实按住状态；没有它时用串口键（wasd 锁定方向 + 自动射击，
             * Enter 开始/确定，p 暂停，1 Bomb，2 低速开关，r 重来，m 标题，l 压力演示） */
            uint32_t held = 0, pressed = th_edge;
            int bot;
            th_edge = 0;
            if (pad_has_state()) {
                held = pad_held(t0, TICKS_PER_MS);
                pressed |= !autoplay&&th.diff!=TH_STRESS&&th.mode==TH_PLAY?pad_source_pressed(0,t0,TICKS_PER_MS):pad_pressed(t0, TICKS_PER_MS);
                if (autoplay && (held & (PAD_UP | PAD_DOWN | PAD_LEFT | PAD_RIGHT | PAD_SHOT))) {
                    autoplay = 0;
                    tx_puts("[autoplay OFF: pad]\r\n");
                }
            } else {
                if (dir_x < 0) held |= PAD_LEFT;
                if (dir_x > 0) held |= PAD_RIGHT;
                if (dir_y < 0) held |= PAD_UP;
                if (dir_y > 0) held |= PAD_DOWN;
                if (th_slow) held |= PAD_SLOW;
                held |= PAD_SHOT;
            }
            if (key_pressed & KEY_START) pressed |= PAD_START;
            if (key_pressed & KEY_PAUSE) pressed |= PAD_PAUSE;
            if (key_pressed & KEY_PICK1) pressed |= PAD_BOMB;
            if (key_pressed & KEY_PICK2) th_slow = !th_slow;
            if (key_pressed & KEY_RESTART) th_start(&th, th.diff);
            if (key_pressed & KEY_LIMIT) th_start(&th, TH_STRESS);
            if (key_pressed & KEY_MENU) { th.mode = TH_TITLE; th.menu = 0; th.nbullets = 0; }
            key_pressed = 0;
            bot = autoplay;
            /* 刚从自动演示接手：回标题选难度（演示局是无敌的） */
            if (th_bot_last && !bot && th.mode != TH_TITLE) { th.mode = TH_TITLE; th.menu = 0; th.nbullets = 0; pressed = 0; }
            th_bot_last = bot;
            if(!bot&&th.diff!=TH_STRESS){
                if(th.mode==TH_PLAY&&th.player_count!=3)th_coop_enable(&th,3);
                uint32_t ph[TH_PLAYERS],pp[TH_PLAYERS];
                for(unsigned p=0;p<TH_PLAYERS;p++){
                    ph[p]=pad_source_held(p,t0,TICKS_PER_MS);pp[p]=pad_source_pressed(p,t0,TICKS_PER_MS);
                    if(player_pick[p]==1)pp[p]|=PAD_BOMB;
                    if(player_pick[p]==2)player_slow[p]=!player_slow[p];player_pick[p]=0;
                    if(player_slow[p])ph[p]|=PAD_SLOW;if(p==2&&matrix_autofire)ph[p]|=PAD_SHOT;
                }
                if(!pad_active(t0,TICKS_PER_MS)){ph[0]&=~(PAD_LEFT|PAD_RIGHT|PAD_UP|PAD_DOWN);if(dir_x<0)ph[0]|=PAD_LEFT;if(dir_x>0)ph[0]|=PAD_RIGHT;if(dir_y<0)ph[0]|=PAD_UP;if(dir_y>0)ph[0]|=PAD_DOWN;}
                /* Shared menus accept any controller. In play, legacy host
                 * actions apply only to P1; device skill edges stay separate. */
                pp[0]|=pressed;if(!pad_has_state())ph[0]|=PAD_SHOT;
                th_tick_coop(&th,ph,pp,0);
            }else th_tick(&th, held, pressed, bot);
            th_sound_tick(&th);                /* 事件位 -> 音效，状态 -> BGM */
            if (aud.bank_new) {                /* tools/load_sound.py 刚把声音库写进 DDR */
                aud.bank_new = 0;
                tx_printf("[audio: sound bank found at 0x02000000: %d sounds, %u KB, %d danmaku events have a sound]\r\n",
                          aud.nsnd, (unsigned)(aud.bank_bytes / 1024u), th_sound_found());
            }
        } else if (scene == SCENE_GAME) {
            {   /* pc_pad.py：方向 / 射击键一按就退出自动驾驶；Enter / Z = 开始，Esc = 暂停 */
                const uint32_t pe = !game.limit_mode&&game.phase==PLAYING?pad_source_pressed(0,t0,TICKS_PER_MS):pad_pressed(t0, TICKS_PER_MS);
                if (autoplay && (pad_held(t0, TICKS_PER_MS) & (PAD_UP | PAD_DOWN | PAD_LEFT | PAD_RIGHT | PAD_SHOT))) {
                    autoplay = 0;
                    tx_puts("[autoplay OFF: pad]\r\n");
                }
                if (pe & (PAD_START | PAD_SHOT)) key_pressed |= KEY_START;
                if (pe & PAD_PAUSE)              key_pressed |= KEY_PAUSE;
                if (pe & PAD_BOMB)               key_pressed |= KEY_SKILL;
            }
            in = autoplay ? bot_input(&game, frame) : manual_input();
            in.pressed |= key_pressed;
            Input party_input[GAME_PLAYERS]={in,{0},{0}};
            if(!game.limit_mode&&!autoplay){
                if(game.phase==PLAYING&&game.player_count!=3)game_coop_enable(&game,3);
                for(unsigned p=0;p<GAME_PLAYERS;p++)party_input[p]=player_game_input(p,t0);
                party_input[0].pressed|=in.pressed;
            }
            key_pressed = 0;
#if SPROUT_STATS
            memset(&game_split, 0, sizeof(game_split));
            game_tick_coop(&game, party_input);
            game_sound_tick(&game);
            f->lg[0] = game_split.enemies; f->lg[1] = game_split.pbullets;
            f->lg[2] = game_split.hostile; f->lg[3] = game_split.rest;
#else
            game_tick_coop(&game, party_input);
            game_sound_tick(&game);
#endif
        } else if (scene == SCENE_SQUARES) {
            demo_squares_tick(sq_count);
        }
        if (scene != SCENE_TH && scene != SCENE_GAME) music_idle_tick(scene);
        *next_tick += TICK_PERIOD;
        if (!hdmi_mode_1080() && (int32_t)(t0 - *next_tick) > 0) *next_tick = t0;   /* 考虑一下低帧率的问题（游戏逻辑频率小于显示频率） */
    }
    t1 = now();
    if (f->ticked) f->logic_peak = f->logic = t1 - t0;
}

/* Physical 1080p30 retains a 60 Hz logical target, with at most two catch-up
 * ticks per render. Fence before reusing the two logical draw-table buffers.
 * Under overload, drop backlog rather than accelerating the game indefinitely.
 * The measured logic_hz remains the actual number of ticks, never a nominal value. */
static void run_logic(Frame *f,uint32_t frame,uint32_t *next_tick){
    unsigned k,j,limit=hdmi_mode_1080()?2u:1u;
    if(limit==2 && hw_avail){uint32_t wait0=now();blit_wait();f->tail+=now()-wait0;}
    for(k=0;k<limit;k++){
        Frame step;memset(&step,0,sizeof(step));
        run_logic_tick(&step,frame+k,next_tick);
        f->ticked+=step.ticked;f->logic+=step.logic;
        if(step.logic_peak>f->logic_peak)f->logic_peak=step.logic_peak;
        for(j=0;j<4;j++)f->lg[j]+=step.lg[j];
        if(!step.ticked)break;
    }
    if(limit==2 && (int32_t)(now()-*next_tick)>(int32_t)(2u*TICK_PERIOD))
        *next_tick=now();
}

/* ------------------------------------------------------------------------- */
/* 主循环开头：弹幕画面大小换了就重画美术（1.5 倍约几百 ms），再清掉所有帧缓冲
 * （黑边 / 别的场景的残留）。引擎先停：它可能还在读旧精灵、写帧缓冲。 */
static void view_apply(void)
{
    const int hd = th_hd_want && th_hd_possible();
    int i;
    view_dirty = 0;
    if (hw_avail) blit_wait();
    if (hd != th_hd) {
        const uint32_t t0 = now();
        th_hd = hd;
        th_set_view(hd);
        th_art_init();
        memset(th.dcount, 0, sizeof th.dcount);    /* 上一 tick 的弹幕表还是旧尺寸的坐标 */
        tx_printf("[danmaku view: %s, art redrawn in %u ms, sprite pools %u KB]\r\n",
                  hd ? "1280x720 native (960x720 x1.5)" : "640x480",
                  (unsigned)((now() - t0) / (BSP_CLINT_HZ / 1000u)),
                  (unsigned)(((th_art_pool_used + th_art2_pool_used) * 2u + th_art_a8_used + th_art2_a8_used) / 1024u));
    }
    for (i = 0; i < FB_COUNT; i++) memset((void *)fb_addr[i], 0, disp_bytes);
}

int main(void)
{
    uint32_t last_req, frame = 0, back_i = 0, i, next_tick, compare_n = 0;
#if SPROUT_STATS
    uint32_t bench_cpu[BENCH_N], bench_hw[BENCH_N];
#endif
    int last_hw = 0;
    RenderInfo info;
    WinStats ws;
    Gfx gfx;
    GfxHwTarget hw_target;

    bsp_init();
#ifdef __riscv
    /* A versioned hardware clock register prevents mixing OC bit and ELF. */
    uint32_t clock_id=HDMI_REG(0x7c),actual_hz=HDMI_REG(0x78);
    if(clock_id==0x434c4b37u&&actual_hz){
        write_u32(actual_hz/(115200u*8u)-1u,BSP_UART_TERMINAL+UART_CLOCK_DIVIDER);
        bsp_printf("Clock actual=%u Hz firmware=%u Hz\r\n",actual_hz,(unsigned)BSP_CLINT_HZ);
        if(actual_hz!=BSP_CLINT_HZ){bsp_printf("CLOCK MISMATCH: load the matched bit and ELF.\r\n");for(;;);}
    }else if(BSP_CLINT_HZ!=200000000u){bsp_printf("CLOCK ABI missing: OC firmware requires its matched bit.\r\n");for(;;);}
#endif
    bsp_printf("\r\n=== TI60-R6-PIPE-KBLEND-FTAB-20261026 Sapphire RV32IM @ %d MHz ===\r\n",
               (int)(BSP_CLINT_HZ / 1000000u));

    if (hdmi_id() != HDMI_ID_VALUE)
        bsp_printf("WARNING: hdmi_fb ID = %x (expected 48444d49) - is the new bitstream loaded?\r\n",
                   (int)hdmi_id());
    else
        bsp_printf("hdmi_fb found, RES=%x, frames=%d\r\n", (int)HDMI_RES, (int)hdmi_frame_cnt());
    disp_init();
    bsp_printf("display %dx%d (pitch %d B); carrot, squares and images use the native full framebuffer\r\n", (int)disp_w, (int)disp_h,
               (int)disp_pitch);

    hw_avail = blit_init();
    if (hw_avail)
        bsp_printf("BitBlt engine found, CAPS=%x (command queue %d, command list %s)\r\n",
                   (int)blit_rd(BLIT_R_CAPS), (int)blit_st.depth,
                   blit_st.has_list ? "yes" : "NO - old bitstream");
    else
        bsp_printf("BitBlt engine NOT found (ID=%x, expected 424c5431) - CPU rendering only\r\n",
                   (int)blit_rd(BLIT_R_ID));

    bsp_printf("framebuffers: %x %x %x (%d bytes each, RGB565), demo pictures at %x\r\n",
               (int)fb_addr[0], (int)fb_addr[1], (int)fb_addr[2], (int)disp_bytes, (int)IMG_ADDR);
    bsp_printf("build: GFX_CPU_WORD_FILL=%d AUTOPLAY=%d VSYNC=%d HW=%d FONT_ATLAS=%d"
#ifdef __OPTIMIZE__
               " optimized"
#else
               " -O0/-Og (slow!)"
#endif
               "\r\n", GFX_CPU_WORD_FILL, AUTOPLAY_DEFAULT, VSYNC_DEFAULT, HW_DEFAULT, GFX_FONT_ATLAS);
    bsp_printf("build: %s, engine source cache %s\r\n",
               SPROUT_STATS ? "STATS=1 (measurement build)" : "release (make STATS=1 for the detailed report)",
               hw_avail ? (blit_st.cflag ? "ON" : "not in this bitstream") : "-");
#ifdef SYSTEM_CORES_0_ICACHE_SIZE
    bsp_printf("caches (BSP soc.h): I$ %d B %d-way, D$ %d B %d-way, on-chip RAM %d B\r\n",
               (int)SYSTEM_CORES_0_ICACHE_SIZE, (int)SYSTEM_CORES_0_ICACHE_WAYS,
               (int)SYSTEM_CORES_0_DCACHE_SIZE, (int)SYSTEM_CORES_0_DCACHE_WAYS, (int)SYSTEM_RAM_A_CTRL_SIZE);
#endif
#if SPROUT_STATS
    cache_probe(IMG_ADDR);
#endif

    for (i = 0; i < FB_COUNT; i++) memset((void *)fb_addr[i], 0, disp_bytes);   /* 黑边：只在开机清一次 */

#if SPROUT_STATS                          /* 启动测速约几秒；发行版直接进游戏 */
    bsp_printf("Benchmark, same gfx calls on both backends (DDR, write-through D$):\r\n");
    DIAG("boot (WIDE register as set by blit_init)");
    bandwidth_test(0, fb_addr[1], bench_cpu);
    DIAG("after CPU bandwidth_test");
    if (hw_avail) bandwidth_test(1, fb_addr[1], bench_hw);
    DIAG("after HW bandwidth_test (wide fill + wide copy)");
    print_bench(bench_cpu, hw_avail ? bench_hw : 0);

    if (hw_avail) seed_bench(fb_addr[1]);
    DIAG("after seed_bench");
    if (hw_avail) mem_bench(fb_addr[1], fb_addr[2]);
    DIAG("after mem_bench");
    if (hw_avail) limit_bench(fb_addr[1], fb_addr[2]);
    DIAG("after limit_bench");
#endif
    if (hw_avail) set_game_limit();
    /* default: DDR command list (hwlist) - fastest on the board since the
       32 KB I$/D$ (2026-10-03). Bitstream without command list -> APB list. */
    set_seed_mode(2);                        /* 也按 band_want 打开分带（blit_set_band） */
#if defined(KPATH_DEFAULT) && !KPATH_DEFAULT
    if (hw_avail && blit_st.has_band) {      /* makefile KPATH=0：分带里不用 K 通路（排查用） */
        blit_band_set_koff(1);
        bsp_printf("band mode K path OFF (makefile KPATH=0)\r\n");
    }
#endif
    if (hw_avail) bsp_printf("engine LIMIT read words in flight: %d (makefile LIMITW)\r\n", (int)GAME_LIMIT_WORDS);
    if (hw_avail)
        bsp_printf("band mode (engine CAPS[31]): %s\r\n",
                   !blit_st.has_band ? "not in this bitstream" :
                   blit_st.band ? "ON - frames composed on chip, pipelined flips (4 framebuffers)" : "off");

    render_init();
#if SPROUT_STATS
    render_clock = now;                       /* render_game measures seeds / hud */
    game_clock = now;                         /* game_tick measures its parts */
#endif
    demo_geometry((int)disp_w,(int)disp_h);
    demo_init(IMG_ADDR);
    tx_printf("[MOT2 available %d, packed square table %d, native square load max 99999]\r\n",
              demo_motion_available(),hw_avail && (blit_rd(BLIT_R_CAPS2)&8u)!=0);
    game_init(&game, 0xC0FFEEu);
    {
        uint32_t t0 = now();
        th_clock = now;                       /* 弹幕逻辑分段计时（串口 stress 行） */
        th_hd = th_hd_want && th_hd_possible();
        th_set_view(th_hd);
        th_art_init();                        /* 占位图全部由 CPU 现画（约几百 ms，1.5 倍更久） */
        th_init(&th, 0x5EEDu ^ t0);
        bsp_printf("danmaku art drawn in %d ms, view %s (Th state %d KB, sprite pools %d KB)\r\n",
                   (int)((now() - t0) / (BSP_CLINT_HZ / 1000u)),
                   th_hd ? "1280x720 native (x1.5, '#' = 640x480)" : th_hd_possible() ? "640x480 ('#' = 720p)" : "640x480",
                   (int)(sizeof th / 1024u), (int)(((th_art_pool_used + th_art2_pool_used) * 2u + th_art_a8_used + th_art2_a8_used) / 1024u));
    }
    use_hw = hw_avail && HW_DEFAULT;

    /* show a black buffer first, then start the loop */
    memset((void *)fb_addr[0], 0, disp_bytes);
    hdmi_fence();
    hdmi_enable(fb_addr[0]);
    {   /* HDMI 音频：位流里有就开 HDMI 模式 + 混音器，找声音库（0x02000000） */
        char line[200];
        aud_init();
        th_sound_init();
        aud_status_line(line, sizeof line);
        bsp_printf("%s | danmaku events with a sound: %d of %d\r\n", line, th_sound_found(), TS_COUNT);
    }
    last_req = fb_addr[0];
    print_help();
    tx_puts("Squares: @ native 720p 20000; ,/. -/+100 (max99999); ! CPU/HW motion; % table/calls; & exact A/B. Audio: ( tone, ) volume, / audition.\r\n");
#if SPROUT_STATS
    tx_puts("UART report every second:\r\n"
            "  us/frame: logic = game update (max = worst tick), submit = drawing / queueing\r\n"
            "  without qstall, qstall = waiting for a free queue slot, tail_wait = last command\r\n"
            "  -> engine idle, vsync_wait = waiting for the flip, other = UART + stats\r\n");
#else
    tx_puts("UART: one line per second (work = logic + draw per frame, us)\r\n");
#endif
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
        uint32_t ta, t1, t2, tl, t3, tend, cur, back, vs0;
#if SPROUT_STATS
        uint32_t stall0;
#endif
        int ahead;                             /* this frame: submit first, logic during the draw */
        int piped;                             /* this frame went to the engine as one band frame */
        uint32_t bseq0, bsplit0;
        Frame f;

        if (view_dirty) view_apply();          /* '#' 弹幕 720p / 480，或换了场景 */
        /* a backend / scene / load change starts fresh measurement windows */
        if (stats_reset) {
            uint32_t t = now();
            stats_reset = 0;
            win_begin(&hud_w, t);
            win_begin(&uart_w, t);
            compare_n = 0;
        }
        memset(&f, 0, sizeof(f));

        /* Build the next frame immediately; present() waits before a new flip. */
        ta = now();

        /* 流水关了（切到 CPU 后端 / 关了分带）但还挂着一帧：等它画完翻出来 */
        if (pend_fb && !(use_hw && blit_st.band)) {
            blit_wait();
            present(&f, pend_fb);
            last_req = pend_fb;
            pend_fb = 0;
        }

        /* pick a buffer that is neither on screen nor queued (nor the frame the
         * engine is still drawing: pend_fb) */
        cur = hdmi_cur();
        for (i = 1; i <= FB_COUNT; i++) {
            uint32_t k = (back_i + i) % FB_COUNT;
            if (fb_addr[k] != cur && fb_addr[k] != last_req && fb_addr[k] != pend_fb) { back_i = k; break; }
        }
        back = fb_addr[back_i];

        /* 2. input */
        poll_uart();
        {static unsigned video_previous=~0u;unsigned video=hdmi_dual_supported()?HDMI_MODE:0;
         if(video!=video_previous){video_previous=video;control_video_state(0);stats_reset=1;}}
        tx_pump();

        /* 3. logic (serial order). With the engine and 'q' overlap on, the
         *    logic runs after the submit instead (step 4b), while the engine
         *    draws: the frame shows the state before this tick, the next frame
         *    shows it -> same picture sequence, one tick later on screen. */
        demo_square_alpha(alpha_fx);
        ahead = use_hw && overlap && (scene != SCENE_SQUARES || demo_fill_table_active()) && !hdmi_mode_1080();
        if (!ahead) run_logic(&f, frame, &next_tick);
        t1 = now();
#if SPROUT_STATS
        if (seed_prof_req) {                   /* 'y': diagnostic, this frame is not representative */
            seed_prof_req = 0;
            if (use_hw && scene == SCENE_GAME) seed_profile(&gfx, &hw_target, back, &game);
            stats_reset = 1;
            t1 = now();
        }
#endif

        /* 4. submit: CPU draws the pixels itself, or queues BitBlt commands */
        info.quantity_step=count_step;
        info.show_stats = show_stats;
        info.alpha_fx = alpha_fx;
        info.hw = use_hw;
        info.batch = use_hw ? seed_mode() : batch_list;   /* 2/3 only mean something on HW */
        if (!use_hw && last_hw) blit_dcache_invalidate();   /* drop lines the engine overwrote */
        last_hw = use_hw;
#if SPROUT_STATS
        if (eng_prof_req) {                    /* '8': diagnostic, this frame is not representative */
            eng_prof_req = 0;
            if (use_hw && scene == SCENE_GAME) engine_profile(&gfx, &hw_target, back, &game, info);
            stats_reset = 1;
            t1 = now();
        }
#endif
#if SPROUT_STATS
        stall0 = blit_st.stall_ticks;
        blit_st.qpeak = 0;
#endif
        bseq0 = blit_st.bseq; bsplit0 = blit_st.bsplits;
        if ((scene == SCENE_TH && th_hd) || scene != SCENE_TH) view_begin_th_hd(&gfx, &hw_target, use_hw, back);
        else                            view_begin(&gfx, &hw_target, use_hw, back);
        if (scene == SCENE_TH) {
            ThRenderInfo ti;
            ti.fps = info.fps; ti.hw = use_hw; ti.stats = show_stats; ti.alpha = alpha_fx;   /* 'e'：半透明特效开 / 关 */
            th_render(&th, &gfx, ti);
        }
        else if (scene == SCENE_SQUARES) demo_squares_render(&gfx, info, sq_count);
        else if (scene == SCENE_IMAGES) demo_images_render(&gfx, info, frame, img_period);
        else                            render_game(&game, &gfx, info);
        gfx_end(&gfx);
        /* Only a complete, unsplit band frame can retain ownership across loops.
         * Square motion still writes its DMA source in place, so it drains first. */
        piped = (scene != SCENE_SQUARES || demo_fill_table_active()) && (!vsync || vpipe) && use_hw && blit_st.bseq - bseq0 == 1u && blit_st.bsplits == bsplit0 && !blit_st.band_open;
#if SPROUT_STATS
        if (use_hw) {
            f.qend  = blit_queue_used();      /* commands still waiting after the last push */
            f.qpeak = blit_st.qpeak;
        }
        t2 = now();
        f.qstall = blit_st.stall_ticks - stall0;
        f.submit = t2 - t1 > f.qstall ? t2 - t1 - f.qstall : 0u;
#else
        t2 = now();
        f.submit = t2 - t1;                   /* 发行版：qstall 不单独计，含在 submit 里 */
#endif
#if SPROUT_STATS
        if (scene == SCENE_GAME) {             /* seeds / hud measured inside render_game */
            f.seeds   = render_split.seeds;
            f.hud     = render_split.hud;
            f.seeds_n = render_split.seeds_n;
            f.sc[0] = render_split.bg;      f.sc[1] = render_split.sort;
            f.sc[2] = render_split.enemies; f.sc[3] = render_split.pbullets;
            f.sc[4] = render_split.particles; f.sc[5] = render_split.locator;
        }
        f.scene = t2 - t1 > f.seeds + f.hud ? t2 - t1 - f.seeds - f.hud : 0u;
#endif

        /* 4b. overlap: game tick while the engine works through the queue
         *     (all commands of this frame are already built: the game state
         *     may change now) */
        if (ahead) run_logic(&f, frame, &next_tick);
        tl = now();
        vs0 = f.vsync;

        /* 5. tail_wait: the frame must be complete in DDR before the flip
         *    (overlap: only what is left after the logic)
         * 6. present (latched by hardware at the next vsync)
         * 分带流水：这一帧（back）交给引擎后不等它；这里等的是上一帧（pend_fb），画完就翻页。
         * 引擎画上一帧的时间和 CPU 录这一帧 + 跑逻辑是重叠的，所以一般不用等。 */
        if (piped) {
            int flipped=0;
            if (pend_fb) {
                uint32_t spins = 0;
                while (!blit_band_done(pend_seq))
                    if ((++spins & (BLIT_STUCK_SPINS - 1u)) == 0) blit_stuck("frame wait (band pipeline)");
                present(&f, pend_fb);
                last_req = pend_fb;
                flipped=1;
            }
            pend_fb = back;
            pend_seq = blit_st.bseq;
            if ((!vsync || !flipped) && blit_band_done(pend_seq)) {    /* 这一帧也已经画完（比如跑了一个 tick）：马上翻，不多拖一帧 */
                present(&f, pend_fb);
                last_req = pend_fb;
                pend_fb = 0;
            }
            t3 = now();
        } else {
            /* 也把挂着的上一帧等完（刚切到 CPU 后端时引擎可能还在画它，那块缓冲不能马上再用）；
             * 这一帧更新，直接翻这一帧 */
            if (use_hw || pend_fb) blit_wait();
            pend_fb = 0;
            present(&f, back);
            t3 = now();
            last_req = back;
        }
        f.tail += t3 - tl - (f.vsync-vs0);
        /* 弹幕压力自动档：这一帧除了等 vsync 以外的全部时间 */
        if (scene == SCENE_TH) th_stress_frame(&th, per_us(t3-ta-f.vsync,1), hdmi_mode_1080()?2u*TH_FRAME_BUDGET_US:TH_FRAME_BUDGET_US);
        ++frame;
        /* Do not label a mixed transition window with the new configuration.
         * A phase or control change starts fresh counters after this frame. */
        {int phase=scene==SCENE_TH?th.mode:scene==SCENE_GAME?game.phase:0;
         int phase_changed=measurement_scene!=scene||measurement_phase!=phase;
         if(stats_reset||phase_changed){
             if(phase_changed)++settings_epoch;
             measurement_scene=scene;measurement_phase=phase;stats_reset=0;
             win_begin(&hud_w,t3);win_begin(&uart_w,t3);continue;
         }}
        win_add(&hud_w, &f);
        win_add(&uart_w, &f);

        /* 7. statistics: HUD every 0.5 s, UART every 1 s */
        tend = now();
        if (tend - hud_w.t0 >= STAT_PERIOD) {
            win_end(&hud_w, tend, &ws);
            info.fps          = (int)((ws.fps10 + 5u) / 10u);
            info.render_fps10=ws.fps10;info.present_fps10=ws.present10;info.present_valid=ws.prs;info.logic_hz10=ws.logic10;
            info.logic_us     = (int)ws.logic;
            info.logic_max_us = (int)ws.logic_max;
            info.render_us    = (int)ws.draw;
            info.work_us      = (int)ws.work;
            info.ms           = (int)((ws.work + 50u) / 100u);
            info.submit_us    = (int)ws.submit;
            info.qstall_us    = (int)ws.qstall;
            info.tail_us      = (int)ws.tail;
            info.vsync_us     = (int)ws.vsync;
            info.seeds_us     = (int)ws.seeds;
            info.hud_us       = (int)ws.hud;
            info.scene_us     = (int)ws.scene;
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
            print_telemetry(&ws);
#if SPROUT_STATS
            if(report_verbose)print_window(&ws, &info, &gfx);
#else
            if(report_verbose)print_window(&ws);
#endif
            win_begin(&uart_w, tend);
        }
    }
    return 0;
}
