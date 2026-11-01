#include "demo.h"
#include "scene_assets.h"
#include "mini_snprintf.h"
#include <string.h>

#define INK   RGB565(15,24,31)
#define CREAM RGB565(242,237,213)
#define MUTED RGB565(145,163,152)
#define GREEN RGB565(181,225,111)
#define GOLD  RGB565(242,190,93)
#define RED   RGB565(238,112,112)
#define CYAN  RGB565(118,208,198)

/* 方块场景的画面大小（2026-10-25）：开机由 demo_geometry() 按显示分辨率设定，
 * 720p 位流 = 原生 1280x720（参照队友版），否则 640x480。上下两条信息栏，方块在中间运动。 */
static int sq_w = SCREEN_W, sq_h = SCREEN_H;
#define TOP_H  40            /* info bars; the scene lives in between */
#define BOT_Y  (sq_h - 44)   /* 640x480 时 436 */
#define FIXS   4             /* square positions in 1/16 px */

typedef struct { int32_t x, y; int16_t vx, vy; int16_t size; Pixel color; } Square;
static Square squares[DEMO_MAX_SQUARES];
static Sprite images[DEMO_PICTURE_COUNT];
static uint32_t rng = 0x2545F491u;
static int fixed_size;       /* 0 = 混合 6..40（原来的随机大小），否则全部这个边长 */

/* 填充表（2026-10-26，引擎 FTAB）：每次 tick 顺手把方块写成 GfxFillItem 表，渲染时整张表交给引擎
 * （CPU 不再一个方块写一条命令）。三张轮流写：主循环里 tick 和引擎画上一帧（甚至上上帧，流水时）重叠，
 * 引擎读的表在它画完前不能改（同萝卜的弹幕表 Game.seed_draw）。 */
#define SQ_TABS 3
/* 大缓冲放到 DDR 0x0400_0000 的 .artbss（game.ld，不下载、启动不清零，用之前一定先写）：
 * 程序区（.bss）只有 15 MB，2026-10-26 加了萝卜新美术后放不下了 */
#ifdef __riscv
#define DDR_BIG(al) __attribute__((section(".artbss"),aligned(al)))
#else
#define DDR_BIG(al) __attribute__((aligned(al)))
#endif
static GfxFillItem sq_tab[SQ_TABS][DEMO_MAX_SQUARES] DDR_BIG(64);   /* 768 KB；tick 先写再用（sq_cha_n） */
static int      sq_tab_n[SQ_TABS], sq_cha_n[SQ_TABS], sq_tab_cur = -1;
static uint32_t sq_tab_px[SQ_TABS], sq_tab_apx[SQ_TABS];
static int      sq_tab_fx[SQ_TABS];
static unsigned sq_cha_dirty = (1u << SQ_TABS) - 1u;   /* 第二个字（颜色 / 高 / alpha）要重写的表 */
static int      sq_fx = -1;                            /* 表里 alpha 按的 'e' 状态 */
static int      sq_table_on = 1, sq_tab_used;
int demo_square_table(void) { return sq_table_on; }
int demo_square_table_toggle(void) { sq_table_on = !sq_table_on; return sq_table_on; }

/* 实验：按带（16 行）排序方块（计数排序，稳定）。排完表里相邻的项落在同一带，phase1 写桶就是顺序写 DDR；
 * 方块会慢慢散开（每 tick 最多 3 px），所以效果几秒后变弱，可以再按。只改画的先后（谁压谁），不改位置 / 速度。 */
static Square sq_tmp[DEMO_MAX_SQUARES] DDR_BIG(64);
int demo_squares_sort_y(void)
{
    static int cnt[64];
    const int nb = (sq_h + 15) / 16 < 64 ? (sq_h + 15) / 16 : 64;
    int i, b, sum = 0;
    memset(cnt, 0, sizeof cnt);
    for (i = 0; i < DEMO_MAX_SQUARES; i++) { b = (int)(squares[i].y >> FIXS) / 16; cnt[b < nb ? b : nb - 1]++; }
    for (b = 0; b < nb; b++) { const int c = cnt[b]; cnt[b] = sum; sum += c; }
    for (i = 0; i < DEMO_MAX_SQUARES; i++) { b = (int)(squares[i].y >> FIXS) / 16; sq_tmp[cnt[b < nb ? b : nb - 1]++] = squares[i]; }
    memcpy(squares, sq_tmp, sizeof squares);
    sq_cha_dirty = (1u << SQ_TABS) - 1u;                  /* 颜色 / 大小跟着换了位置：表的第二个字全重写 */
    return nb;
}

static uint32_t rnd(void) { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; }

/* 混合大小按编号算（不随 rnd），所以换成固定大小再换回来还是同一组大小 */
static int mixed_size(int i) { return 6 + (int)(((uint32_t)i * 2654435761u >> 8) % 35u); }

static void new_square(Square *s, int i)
{
    static const Pixel pal[8] = {
        RGB565(240,103,84), RGB565(63,181,217), RGB565(181,225,111), RGB565(242,190,93),
        RGB565(158,120,224), RGB565(239,106,135), RGB565(118,208,198), RGB565(242,237,213) };
    s->size  = (int16_t)(fixed_size ? fixed_size : mixed_size(i));
    s->x     = (int32_t)(rnd() % (uint32_t)(sq_w - s->size)) << FIXS;
    s->y     = (int32_t)(TOP_H + rnd() % (uint32_t)(BOT_Y - TOP_H - s->size)) << FIXS;
    s->vx    = (int16_t)((int)(rnd() % 97) - 48);          /* up to 3 px per tick */
    s->vy    = (int16_t)((int)(rnd() % 97) - 48);
    s->color = pal[rnd() & 7u];
}

void demo_geometry(int w, int h)
{
    if (w < 64 || h < 128) return;
    sq_w = w; sq_h = h;
}
int demo_width(void) { return sq_w; }
int demo_height(void) { return sq_h; }
int demo_square_size(void) { return fixed_size; }

/* 改方块大小（0 = 混合 6..40）：位置夹回场地里，速度 / 颜色不变 */
int demo_set_square_size(int px)
{
    int i;
    if (px != 0 && px != 1 && px != 3 && (px < 6 || px > 40)) return 0;
    fixed_size = px;
    sq_cha_dirty = (1u << SQ_TABS) - 1u;              /* 填充表里的大小也要重写 */
    for (i = 0; i < DEMO_MAX_SQUARES; i++) {
        Square *s = &squares[i];
        const int size = px ? px : mixed_size(i);
        const int32_t maxx = (int32_t)(sq_w - size) << FIXS, maxy = (int32_t)(BOT_Y - size) << FIXS;
        s->size = (int16_t)size;
        if (s->x > maxx) s->x = maxx;
        if (s->y > maxy) s->y = maxy;
    }
    return 1;
}

/* ------------------------------------------------------------------------- */
/* Pictures for the switching demo (integer only, generated once at boot).    */
/* ------------------------------------------------------------------------- */
/* 2026-10-28：图片和方块同一个画面大小（720p 位流 = 原生 1280x720，每张 1.8 MB，仍是 2 MB 一张） */
static void gen_image(Pixel *p, int kind, int W, int H)
{
    int x, y;
    for (y = 0; y < H; y++) for (x = 0; x < W; x++) {
        int r, g, b;
        if (kind == 0) {                      /* colour gradient */
            r = x * 255 / (W - 1); g = y * 255 / (H - 1);
            b = 255 - (x + y) * 255 / (W + H);
        } else if (kind == 1) {               /* rings over a checkerboard */
            int dx = x - W / 2, dy = y - H / 2;
            int ring = ((dx * dx + dy * dy) >> 10) & 1, chk = ((x >> 5) ^ (y >> 5)) & 1;
            r = ring ^ chk ? 240 : 30; g = ring ? 190 : 60; b = chk ? 90 : 200;
        } else {                              /* "munching squares" xor texture */
            r = (x ^ y) & 255; g = ((x * 2) ^ y) & 255; b = (y * 3) & 255;
        }
        p[y * W + x] = RGB565(r, g, b);
    }
}

static uint32_t img_base_addr;
static int gallery_ready;
/* First three test pictures are relabelled on language changes; the fourth
 * supplied poster is immutable after its startup decode. */
static void draw_images(uint32_t img_base)
{
    static const char *names[DEMO_PICTURE_COUNT] = { "IMAGE 1", "IMAGE 2", "IMAGE 3", "IMAGE 4" };
    int i;
    for (i = 0; i < DEMO_PICTURE_COUNT; i++) {
        Gfx g;
        Pixel *p = (Pixel *)(uintptr_t)(img_base + (i<3 ? (uint32_t)i*0x200000u : DEMO_PICTURE4_OFFSET));
        const int W = sq_w, H = sq_h;              /* demo_geometry() 先调：720p = 1280x720 */
        if(i==3) {
            if(!gallery_ready || images[i].pixels!=p || images[i].width!=W || images[i].height!=H)
                gallery_ready=scene_gallery_decode(p,W,H);
            if(gallery_ready) {images[i]=(Sprite){W,H,p,0};continue;}
        }
        gen_image(p, i, W, H);
        gfx_begin(&g, p, W, H);
        gfx_fill_rect(&g, W / 2 - 150, H / 2 - 50, 300, 100, INK);
        gfx_text(&g, W / 2 - gfx_text_width(TR(names[i]), 7) / 2, H / 2 - 25, TR(names[i]), 7, CREAM);
        gfx_end(&g);
        images[i].width = W; images[i].height = H;
        images[i].pixels = p; images[i].alpha = 0;
    }
}
void demo_relabel(void) { if (img_base_addr) draw_images(img_base_addr); }
void demo_init(uint32_t img_base)
{
    int i;
    img_base_addr = img_base;
    draw_images(img_base);
    for (i = 0; i < DEMO_MAX_SQUARES; i++) new_square(&squares[i], i);
}

/* ------------------------------------------------------------------------- */
/* Info bars: scene name + load on top, CPU vs HW comparison at the bottom.   */
/* ------------------------------------------------------------------------- */
static void overlay(Gfx *g, RenderInfo info, const char *title, const char *detail)
{
    char text[96];
    int w;
    const int W = g->width, H = g->height, by = H - 44;   /* 方块 720p / 图片 640x480 共用 */
    gfx_fill_rect(g, 0, 0, W, TOP_H, INK);
    gfx_text(g, 12, 6, title, 2, CREAM);
    gfx_text(g, 12, 27, detail, 1, MUTED);
    /* 2026-10-28：显示帧率（上屏的新画面 / 秒，大字）和渲染帧率（主循环，小字）分开 */
    snprintf(text, sizeof(text), TR("SHOW %d FPS"), info.show_fps);
    w = gfx_text_width(text, 2);
    gfx_text(g, W - 12 - w, 6, text, 2, info.show_fps >= 58 ? GREEN : info.show_fps >= 30 ? GOLD : RED);
    snprintf(text, sizeof(text), TR("RENDER %d FPS"), info.fps);
    {
        const int rw = gfx_text_width(text, 1);
        gfx_text(g, W - 12 - w - 14 - rw, 4, text, 1, info.fps >= 58 ? GREEN : info.fps >= 30 ? GOLD : RED);
        gfx_text(g, W - 12 - w - 14 - rw, 20, info.hw ? TR("HW BLIT") : TR("CPU DRAW"), 1, info.hw ? GREEN : GOLD);
    }
    snprintf(text, sizeof(text), TR("DRAW %d US"), info.render_us);
    gfx_text(g, W - 12 - gfx_text_width(text, 1), 27, text, 1, CREAM);

    gfx_fill_rect(g, 0, by, W, H - by, INK);
    if (info.fps_mode[0] > 0)
        snprintf(text, sizeof(text), TR("CPU %d FPS %d.%d MS"), info.fps_mode[0],
                 info.draw_mode[0] / 1000, (info.draw_mode[0] % 1000) / 100);
    else snprintf(text, sizeof(text), TR("CPU -- FPS"));
    gfx_text(g, 12, by + 8, text, 2, info.hw ? MUTED : GREEN);
    if (info.fps_mode[1] > 0)
        snprintf(text, sizeof(text), TR("HW %d FPS %d.%d MS"), info.fps_mode[1],
                 info.draw_mode[1] / 1000, (info.draw_mode[1] % 1000) / 100);
    else snprintf(text, sizeof(text), TR("HW -- FPS"));
    gfx_text(g, 330, by + 8, text, 2, info.hw ? GREEN : MUTED);
    if (info.hw)
        snprintf(text, sizeof(text), TR("SUB %d QST %d TAIL %d VS %d US  BLT %d%% RD %d WR %d MB/S  N G - + V"),
                 info.submit_us, info.qstall_us, info.tail_us, info.vsync_us, info.blt,
                 info.rd_mbs, info.wr_mbs);
    else
        snprintf(text, sizeof(text), TR("LOGIC %d DRAW %d VSYNC %d US   N NEXT  G CPU/HW  -/+ LOAD  V VSYNC  E ALPHA"),
                 info.logic_us, info.submit_us, info.vsync_us);
    gfx_text(g, 12, by + 30, text, 1, MUTED);
}

/* ------------------------------------------------------------------------- */
void demo_squares_tick(int count)
{
    int i;
    const int b = sq_tab_cur + 1 < SQ_TABS ? sq_tab_cur + 1 : 0;   /* 写下一张，前两张引擎可能还在读 */
    GfxFillItem *t = sq_tab[b];
    const int all = (sq_cha_dirty >> b) & 1;          /* 颜色 / 大小 / alpha 变了：这张表的第二个字全写 */
    const int cvalid = all ? 0 : sq_cha_n[b];
    uint32_t px = 0, apx = 0;
    if (count < 0) count = 0;
    if (count > DEMO_MAX_SQUARES) count = DEMO_MAX_SQUARES;
    for (i = 0; i < count; i++) {
        Square *s = &squares[i];
        int32_t maxx = (int32_t)(sq_w - s->size) << FIXS;
        int32_t miny = (int32_t)TOP_H << FIXS, maxy = (int32_t)(BOT_Y - s->size) << FIXS;
        s->x += s->vx; s->y += s->vy;
        if (s->x < 0)    { s->x = -s->x;           s->vx = (int16_t)-s->vx; }
        if (s->x > maxx) { s->x = 2 * maxx - s->x; s->vx = (int16_t)-s->vx; }
        if (s->y < miny) { s->y = 2 * miny - s->y; s->vy = (int16_t)-s->vy; }
        if (s->y > maxy) { s->y = 2 * maxy - s->y; s->vy = (int16_t)-s->vy; }
        /* 填充表（FTAB）：第一个字每 tick 写（位置），第二个字（颜色 / 高 / alpha）只在变了时写 */
        t[i].xyw = GFX_FILL_XYW(s->x >> FIXS, s->y >> FIXS, s->size);
        if (i >= cvalid)
            t[i].cha = GFX_FILL_CHA(s->color, s->size, (sq_fx && (i & 1)) ? 150u : 255u);
        {   /* 统计用的像素数（HUD），不影响画面 */
            const uint32_t a = (uint32_t)s->size * (uint32_t)s->size;
            px += a;
            if (sq_fx && (i & 1)) apx += a;
        }
    }
    sq_tab_px[b] = px; sq_tab_apx[b] = apx; sq_tab_fx[b] = sq_fx != 0;
    if (all || count > sq_cha_n[b]) sq_cha_n[b] = count;
    sq_cha_dirty &= ~(1u << b);
    sq_tab_n[b] = count;
    sq_tab_cur = b;
}

void demo_squares_render(Gfx *g, RenderInfo info, int count)
{
    char detail[96];
    int i;
    if (count > DEMO_MAX_SQUARES) count = DEMO_MAX_SQUARES;
    if (info.alpha_fx != sq_fx) { sq_fx = info.alpha_fx; sq_cha_dirty = (1u << SQ_TABS) - 1u; }
    gfx_fill_rect(g, 0, TOP_H, sq_w, BOT_Y - TOP_H, RGB565(24, 38, 36));
    sq_tab_used = 0;
    /* 表里的 alpha 是写表时的 'e' 状态：刚按过 'e'（最多 3 个 tick）就先逐个画，画面和统计都对 */
    if (sq_table_on && sq_tab_cur >= 0 && sq_tab_n[sq_tab_cur] > 0 &&
        sq_tab_fx[sq_tab_cur] == (info.alpha_fx != 0) && sq_cha_n[sq_tab_cur] >= sq_tab_n[sq_tab_cur] &&
        g->width >= sq_w && g->height >= sq_h) {
        /* 最近一次 tick 写的填充表（位置、颜色、alpha 都在表里）：BitBlt 有 FTAB 时整张表一条记录，
         * 引擎自己读；否则 gfx_fill_table 逐项画（和下面逐个调用同一画面） */
        const int b = sq_tab_cur, n = sq_tab_n[b] < count ? sq_tab_n[b] : count;
        if (gfx_fill_table(g, sq_tab[b], n, 0)) {
            const uint32_t na = sq_tab_fx[b] ? (uint32_t)n / 2u : 0u;   /* 表里的 alpha 按写表时的 'e' */
            g->fills += (uint32_t)n - na; g->alphas += na;
            g->pixels_written += sq_tab_px[b]; g->alpha_pixels += sq_tab_apx[b];
            sq_tab_used = 1;
        }
    } else {
        for (i = 0; i < count; i++) {
            const Square *s = &squares[i];
            int x = (int)(s->x >> FIXS), y = (int)(s->y >> FIXS);
            if (info.alpha_fx && (i & 1)) gfx_fill_alpha(g, x, y, s->size, s->size, s->color, 150);
            else gfx_fill_rect(g, x, y, s->size, s->size, s->color);
        }
    }
    if (fixed_size) snprintf(detail, sizeof(detail), TR("%d SQUARES %dX%d PX%s  %dX%d  FILL %u ALPHA %u PIXELS %u"), count,
             fixed_size, fixed_size, info.alpha_fx ? TR(" HALF TRANSLUCENT") : "", sq_w, sq_h, g->fills, g->alphas, g->pixels_written);
    else snprintf(detail, sizeof(detail), TR("%d SQUARES 6-40 PX%s  %dX%d  FILL %u ALPHA %u PIXELS %u"), count,
             info.alpha_fx ? TR(" HALF TRANSLUCENT") : "", sq_w, sq_h, g->fills, g->alphas, g->pixels_written);
    overlay(g, info, TR("BITBLT DEMO: MOVING SQUARES"), detail);   /* 画法（表 / 逐条）见串口 load 行 */
    if (sq_w >= 1000) gfx_text(g, sq_w - 330, sq_h - 14, TR("- + COUNT  BRACKETS SIZE  AT N ENTER SET"), 1, CREAM);
}

void demo_images_render(Gfx *g, RenderInfo info, uint32_t frame, int period)
{
    char detail[96];
    int k = (int)((frame / (uint32_t)(period < 1 ? 1 : period)) % DEMO_PICTURE_COUNT);
    const int display_k=k;
    static const int order[4]={0,1,2,3};
    k=order[k];
    const int W = images[k].width, H = images[k].height;
    int bar = (int)((frame * 8u) % (uint32_t)W);
    const int BOT_IMG = H - 44;
    gfx_blit(g, 0, 0, &images[k], 0);                       /* 720p: 1.8 MB per frame (640x480: 614 KB) */
    gfx_fill_rect(g, bar, TOP_H, 8, BOT_IMG - TOP_H, CREAM);  /* moves 8 px / frame */
    snprintf(detail, sizeof(detail), TR("%dX%d COPY EVERY FRAME, PICTURE %d/4, SWITCH EVERY %d FRAME%s"),
             W, H, display_k + 1, period, period == 1 ? "" : "S");
    overlay(g, info, TR("BITBLT DEMO: IMAGE SWITCHING"), detail);
}
