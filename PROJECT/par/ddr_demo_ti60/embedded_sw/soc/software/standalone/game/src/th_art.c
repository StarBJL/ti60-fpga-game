/*
 * th_art.c - placeholder art for the Touhou-style game, drawn by code at boot.
 *
 * Everything is integer math (no FPU on the RV32IM): shapes are tested per pixel
 * in half-pixel units, rotations use the Q14 sine table. Keyed sprites use
 * COLOR_KEY (magenta) as transparent; additive sprites (glows, shots, effects)
 * are drawn on black (black adds nothing). Every sprite starts 64-byte aligned
 * in one static pool, so the engine's source cache lines never straddle two.
 *
 * To use real art later, replace the functions here by loading a PNG atlas into
 * the same Sprite structs (sizes / centres in th_bk_half must stay consistent).
 */
#include "th.h"
#include <string.h>

/* ---------------------------------------------------------------- view */
/* 2026-10-21 native 720p: see th.h. The game logic is unchanged; th_render scales the
 * 640x480 layout by s2 / 2, and th_art_init draws the art for it: backgrounds, frame
 * and stage-2 textures natively at the view size, sprites at 1x and then scaled up
 * (bilinear; keyed sprites by coverage; pixel-map characters nearest). */
ThView th_view = { 2, 0, 0, 640, 480 };
uint8_t th_bk_hv[TBK_COUNT] = { 4, 8, 8, 8, 12, 12, 8, 16, 32 };

void th_set_view(int hd)
{
    int b;
    th_view.s2 = hd ? 3 : 2;
    th_view.ox = hd ? 160 : 0;            /* 960x720 view centred in the 1280x720 target */
    th_view.oy = 0;
    th_view.w  = hd ? 1280 : 640;
    th_view.h  = hd ? 720 : 480;
    for (b = 0; b < TBK_COUNT; b++) th_bk_hv[b] = (uint8_t)TV(th_bk_half[b]);
}

/* the big buffers (1.5x art, ~10 MB) do not fit below the framebuffers: on the board
 * they live in the DDR art area 0x0400_0000 (game.ld section .artbss, not loaded) */
#ifdef __riscv
#define ART_BIG __attribute__((section(".artbss"), aligned(64)))
#else
#define ART_BIG __attribute__((aligned(64)))
#endif
#define PF_W_MAX (TH_PF_W * 3 / 2)
#define PF_H_MAX (TH_PF_H * 3 / 2)

/* ---------------------------------------------------------------- pools */
#define ART_POOL_PIXELS (512u * 1024u)
static Pixel art_pool[ART_POOL_PIXELS] __attribute__((aligned(64)));
static uint32_t art_used, art_last;
uint32_t th_art_pool_used, th_art_a8_used;
uint32_t th_art2_pool_used, th_art2_a8_used;              /* the 1.5x copies (0 in the 640x480 view) */
#define ART2_POOL_PIXELS (1280u * 1024u)                 /* the 1.5x copies (x 2.25) */
static Pixel art2_pool[ART2_POOL_PIXELS] ART_BIG;
static uint32_t art2_used, art2_last;
static Pixel bg_px[PF_W_MAX * PF_H_MAX * 2] ART_BIG;
static Pixel bgs_px[PF_W_MAX * PF_H_MAX * 2] ART_BIG;
static Pixel frame_px[1280 * 720] ART_BIG;              /* 720p: the whole screen (side strips) */
#define A82_POOL_BYTES (640u * 1024u)
static uint8_t a82_pool[A82_POOL_BYTES] ART_BIG;
static uint32_t a82_used;
static Pixel *art2_alloc(int w, int h)
{
    uint32_t n = ((uint32_t)(w * h) + 31u) & ~31u;
    Pixel *p;
    if (art2_used + n > ART2_POOL_PIXELS) return 0;
    p = &art2_pool[art2_used];
    art2_last = art2_used;
    art2_used += n;
    return p;
}

Sprite th_bspr[TBK_COUNT][64];
int    th_bspr_n[TBK_COUNT];
uint8_t th_bspr_cx[TBK_COUNT][64], th_bspr_cy[TBK_COUNT][64];

/* Tight box (2026-10-21): rice / kunai are drawn rotated inside a square box, most of it
 * colour key. The engine still walks every pixel of the box (K beats, band records, source
 * cache lines), so the sprite is cut down to its opaque pixels; th_bspr_cx / cy = bullet
 * centre inside the cut sprite (view px), used instead of the bucket half size. The picture
 * is the same. The sprite is the last allocation of its pool, so the pool shrinks too
 * (the sprites of one pattern then sit closer together in the 16 KB source cache). */
static void crop_tight(Sprite *s, int half, uint8_t *cx, uint8_t *cy)
{
    const int w = s->width, h = s->height;
    Pixel *p = (Pixel *)s->pixels;
    int x, y, x0 = w, y0 = h, x1 = -1, y1 = -1;
    *cx = (uint8_t)half; *cy = (uint8_t)half;
    if (!p) return;
    for (y = 0; y < h; y++) for (x = 0; x < w; x++)
        if (p[y * w + x] != COLOR_KEY) {
            if (x < x0) x0 = x;
            if (x > x1) x1 = x;
            if (y < y0) y0 = y;
            if (y > y1) y1 = y;
        }
    if (x1 < 0) return;
    {
        const int nw = x1 - x0 + 1, nh = y1 - y0 + 1;
        const uint32_t n = ((uint32_t)(nw * nh) + 31u) & ~31u;
        for (y = 0; y < nh; y++)                      /* rows move down in memory: in place is safe */
            for (x = 0; x < nw; x++) p[y * nw + x] = p[(y + y0) * w + x + x0];
        s->width = nw; s->height = nh;
        *cx = (uint8_t)(half - x0); *cy = (uint8_t)(half - y0);
        if (p == &art_pool[art_last] && art_used == art_last + (((uint32_t)(w * h) + 31u) & ~31u)) art_used = art_last + n;
        if (p == &art2_pool[art2_last] && art2_used == art2_last + (((uint32_t)(w * h) + 31u) & ~31u)) art2_used = art2_last + n;
    }
}
/* half size of the sprites of each bucket: top-left = centre - half */
const uint8_t th_bk_half[TBK_COUNT] = { 4, 8, 8, 8, 12, 12, 8, 16, 32 };
Sprite th_spr_player[4], th_spr_option, th_spr_hitbox, th_spr_shot_main, th_spr_shot_opt;
Sprite th_spr_enemy[TE_KINDS][4][2], th_spr_boss[TH_STAGES][2], th_spr_boss_aura;
Sprite th_spr_item[TI_KINDS], th_spr_fx[TF_KINDS][TC_COLORS], th_spr_life, th_spr_bombicon;
Sprite th_spr_bg, th_spr_bg_spell, th_spr_frame, th_spr_bomb_ring;
Sprite th_spr_halo[TH_HALO_SIZES][TC_COLORS];
const uint8_t th_fade_alpha[TH_FADE_LV] = { 80, 150, 215 };   /* 淡入的三档透明度（年龄 0-2 / 3-5 / 6-8 tick） */
Sprite th_spr_portrait[TH_STAGES], th_spr_focus[TH_FOCUS_FRAMES], th_spr_magic[TH_MAGIC_FRAMES];

/* art scale of the sprites being drawn: 2 = 1x (640x480), 3 = 1.5x (720p view).
 * Generators take their sizes in 1x pixels and draw natively at AS(size). */
static int as2 = 2;
#define AS(n) ((n) * as2 / 2)
static int pool2;                                         /* 1: allocate in the 1.5x pools */
static Pixel *art2_alloc(int w, int h);

static Pixel *art_alloc(int w, int h)
{
    uint32_t n = ((uint32_t)(w * h) + 31u) & ~31u;      /* keep 64-byte alignment */
    Pixel *p;
    if (pool2) return art2_alloc(w, h);
    if (art_used + n > ART_POOL_PIXELS) return 0;        /* pool too small: sprite stays empty */
    p = &art_pool[art_used];
    art_last = art_used;
    art_used += n;
    return p;
}

static Sprite mk(int w, int h, Pixel fill)
{
    Sprite s;
    Pixel *p = art_alloc(w, h);
    int i;
    s.width = p ? w : 0; s.height = p ? h : 0; s.pixels = p; s.alpha = 0;
    if (p) for (i = 0; i < w * h; i++) p[i] = fill;
    return s;
}

/* ---------------------------------------------------------------- colours */
static const uint8_t pal[TC_COLORS][3] = {
    { 255,  40,  48 }, { 255, 140,  32 }, { 255, 230,  48 }, {  48, 230,  72 },
    {  48, 220, 240 }, {  64,  96, 255 }, { 200,  64, 255 }, { 235, 235, 250 } };

static Pixel rgb(int r, int g, int b)
{
    r = r < 0 ? 0 : r > 255 ? 255 : r;
    g = g < 0 ? 0 : g > 255 ? 255 : g;
    b = b < 0 ? 0 : b > 255 ? 255 : b;
    {
        Pixel p = RGB565(r, g, b);
        return p == COLOR_KEY ? (Pixel)(p ^ 0x0020u) : p;   /* never the key by accident */
    }
}
/* palette colour mixed towards white (w 0..256) and scaled (s 0..256) */
static Pixel pcol(int c, int w, int s)
{
    int r = pal[c][0] + ((255 - pal[c][0]) * w >> 8);
    int g = pal[c][1] + ((255 - pal[c][1]) * w >> 8);
    int b = pal[c][2] + ((255 - pal[c][2]) * w >> 8);
    return rgb(r * s >> 8, g * s >> 8, b * s >> 8);
}

static uint32_t isqrt(uint32_t v)
{
    uint32_t r = 0, b = 1u << 30;
    while (b > v) b >>= 2;
    while (b) {
        if (v >= r + b) { v -= r + b; r = (r >> 1) + b; } else r >>= 1;
        b >>= 2;
    }
    return r;
}

/* pixel centre relative to the sprite centre, in half pixels */
#define HX(x, w) (2 * (x) + 1 - (w))

/* ---------------------------------------------------------------- bullets */
/* round bullet: white core, coloured body, dark rim (keyed). Sizes in 1x pixels.
 * 1.5x (as2 3): drawn natively, plus light from the upper left (shaded body,
 * a specular spot) - the 1x picture is unchanged */
static Sprite round_bullet(int size, int c, int core, int body)
{
    const int N = AS(size), kc = core * as2, kb = body * as2, kr = body * as2 + as2;   /* half px */
    Sprite s = mk(N, N, COLOR_KEY);
    int x, y;
    if (!s.pixels) return s;
    for (y = 0; y < N; y++) for (x = 0; x < N; x++) {
        int X = HX(x, N), Y = HX(y, N), r2 = X * X + Y * Y;
        Pixel *p = &((Pixel *)s.pixels)[y * N + x];
        if (r2 > kr * kr) continue;
        if (r2 > kb * kb) { *p = pcol(c, 0, as2 > 2 ? 80 : 96); continue; }
        if (as2 <= 2) {
            if (r2 <= kc * kc) *p = pcol(c, 220, 256);
            else               *p = pcol(c, (kb * kb - r2) * 120 / (kb * kb), 256);
            continue;
        }
        {
            /* white centre fading smoothly into the colour (the core radius sets how far the
             * white reaches), shaded lower right, a small specular dot upper left */
            const int t = (kb * kb - r2) * 256 / (kb * kb);               /* 256 centre .. 0 edge */
            const int reach = kc * 256 / kb;                              /* core share of the radius */
            const int l = (-X - Y) * 100 / kb;                            /* lit side > 0 */
            const int hx = X + kb * 9 / 20, hy = Y + kb * 9 / 20, hr = kb * 2 / 9;
            int w = t > 256 - reach ? 200 + (t - 256 + reach) * 40 / (reach + 1) : t * 150 / 256, sc = 256;
            if (l > 0) w += l / 4; else sc += l / 2;
            if (kb >= 12 && hx * hx + hy * hy <= hr * hr) { w = 245; sc = 256; }
            *p = pcol(c, w > 250 ? 250 : w, sc < 140 ? 140 : sc);
        }
    }
    return s;
}

/* rotated ellipse (rice) / arrow head (kunai) inside a square box (1x pixels).
 * The shape is evaluated in 1x half pixels, Q4 (u, v x 16), at the native size. */
static Sprite rot_bullet(int box, int c, uint16_t ang, int kunai)
{
    const int N = AS(box);
    Sprite s = mk(N, N, COLOR_KEY);
    int32_t cs = th_cos(ang), sn = th_sin(ang);
    int x, y;
    if (!s.pixels) return s;
    for (y = 0; y < N; y++) for (x = 0; x < N; x++) {
        int32_t X = HX(x, N), Y = HX(y, N);
        /* along the flight direction / across, Q4 (1x: the old integer half px x 16, same picture) */
        int32_t u = as2 == 2 ? ((X * cs + Y * sn) >> 14) * 16 : ((X * cs + Y * sn) * 32 / as2) >> 14;
        int32_t v = as2 == 2 ? ((-X * sn + Y * cs) >> 14) * 16 : ((-X * sn + Y * cs) * 32 / as2) >> 14;
        Pixel *p = &((Pixel *)s.pixels)[y * N + x];
        if (!kunai) {                                     /* ellipse a = 7, b = 3 px */
            int32_t e = u * u * 9 + v * v * 49, R = 49 * 9 * 4 * 256;
            if (e <= R / 4)            *p = as2 > 2 ? pcol(c, 235 - (int)(e * 40 / (R / 4 + 1)), 256) : pcol(c, 230, 256);
            else if (e <= R * 3 / 4)   *p = as2 > 2 ? pcol(c, X + Y < 0 ? 90 : 40, X + Y < 0 ? 256 : 225) : pcol(c, 60, 256);   /* lit upper left */
            else if (e <= R)           *p = pcol(c, 0, as2 > 2 ? 105 : 120);
        } else {                                          /* kunai: tip at u = +10, tail u = -9 */
            int32_t au = u, av = v < 0 ? -v : v;
            int in, core;
            if (au > 320 || au < -288) continue;
            /* width: grows from the tip (u=+20 half px) to the guard (u=-2), thin tail */
            in = au >= -32 ? av * 22 <= (320 - au) * 5 : av <= 32;
            core = au >= -32 ? av * 22 <= (320 - au) * 2 : av <= 16;
            if (in) {
                if (core)          *p = pcol(c, as2 > 2 ? 225 : 210, 256);
                else if (as2 > 2)  *p = v < 0 ? pcol(c, 75, 256) : pcol(c, 15, 215);   /* lit / shaded edge */
                else               *p = pcol(c, 30, 256);
            }
            else if (au >= -64 && au <= 0 && av <= 112) {
                *p = pcol(c, 0, 110);                                    /* guard */
                if (as2 > 2 && (au >= -24 && au <= -40 + 32)) *p = pcol(c, 40, 150);   /* guard highlight */
            }
        }
    }
    return s;
}

/* five-pointed star, rotated by ang (16 px at 1x) */
static Sprite star_bullet(int c, uint16_t ang)
{
    const int N = AS(16);
    Sprite s = mk(N, N, COLOR_KEY);
    int x, y;
    if (!s.pixels) return s;
    for (y = 0; y < N; y++) for (x = 0; x < N; x++) {
        int32_t X = HX(x, N), Y = HX(y, N);
        uint16_t a = (uint16_t)(th_atan2(Y, X) - ang);
        uint32_t sect = a % 13107u;                       /* 72 degrees */
        uint32_t d = sect < 6554u ? sect : 13107u - sect; /* 0 at a point, 6554 between */
        /* radius and limit in 1x half px, Q4 (1x: the old integer values x 16, same picture) */
        uint32_t r = as2 == 2 ? isqrt((uint32_t)(X * X + Y * Y)) * 16u
                              : isqrt((uint32_t)(X * X + Y * Y) * 1024u / (uint32_t)(as2 * as2));
        uint32_t lim = as2 == 2 ? (15u - d * 8u / 6554u) * 16u : 240u - d * 128u / 6554u;   /* outer 15 -> inner 7 */
        Pixel *p = &((Pixel *)s.pixels)[y * N + x];
        if (as2 > 2 && r <= lim) {                        /* 1.5x: smooth white -> colour gradient */
            const int w = r <= lim / 2u ? 225 : 40 + (int)((lim - r) * 300u / lim);
            *p = pcol(c, w > 225 ? 225 : w, 256);
        }
        else if (r <= lim * 5u / 10u) *p = pcol(c, 220, 256);
        else if (r <= lim)            *p = pcol(c, 40, 256);
        else if (r <= lim + 32u)      *p = pcol(c, 0, 100);
    }
    return s;
}

/* additive glow: colour * falloff, white core (on black) */
static Sprite glow(int size, int c, int core_w, int scale)
{
    Sprite s = mk(AS(size), AS(size), 0);
    int x, y, R;
    size = AS(size); R = size;                            /* radius in half px */
    if (!s.pixels) return s;
    for (y = 0; y < size; y++) for (x = 0; x < size; x++) {
        int X = HX(x, size), Y = HX(y, size);
        int r = (int)isqrt((uint32_t)(X * X + Y * Y));
        int f;
        if (r >= R) continue;
        f = (R - r) * 256 / R;                            /* 0..256 */
        f = f * f >> 8;
        ((Pixel *)s.pixels)[y * size + x] = pcol(c, core_w * f >> 8, f * scale >> 8);
    }
    return s;
}

/* additive ring of radius rr (half px) and width ww */
static Sprite ring(int size, int c, int rr, int ww, int scale)
{
    Sprite s = mk(AS(size), AS(size), 0);
    int x, y;
    size = AS(size); rr = AS(rr); ww = AS(ww);
    if (!s.pixels) return s;
    for (y = 0; y < size; y++) for (x = 0; x < size; x++) {
        int X = HX(x, size), Y = HX(y, size);
        int r = (int)isqrt((uint32_t)(X * X + Y * Y)), d = r > rr ? r - rr : rr - r, f;
        if (d >= ww) continue;
        f = (ww - d) * 256 / ww;
        f = f * f >> 8;
        ((Pixel *)s.pixels)[y * size + x] = pcol(c, 160 * f >> 8, f * scale >> 8);
    }
    return s;
}

/* ---------------------------------------------------------------- simple pixel art */
static void px_rect(Sprite *s, int x, int y, int w, int h, Pixel c)
{
    int i, j;
    for (j = y; j < y + h; j++) for (i = x; i < x + w; i++)
        if (i >= 0 && j >= 0 && i < s->width && j < s->height) ((Pixel *)s->pixels)[j * s->width + i] = c;
}
static void px_disc(Sprite *s, int cx2, int cy2, int r2x, Pixel c)   /* centre / radius in half px */
{
    int x, y;
    for (y = 0; y < s->height; y++) for (x = 0; x < s->width; x++) {
        int X = 2 * x + 1 - cx2, Y = 2 * y + 1 - cy2;
        if (X * X + Y * Y <= r2x * r2x) ((Pixel *)s->pixels)[y * s->width + x] = c;
    }
}
static void px_text(Sprite *s, int x, int y, const char *t, int scale, Pixel c)
{
    Gfx g;
    gfx_begin(&g, (Pixel *)s->pixels, s->width, s->height);
    gfx_text(&g, x, y, t, scale, c);
    gfx_end(&g);
}

/* characters: pixel maps from th_pix.c (one char per pixel, '.' = COLOR_KEY).
 * lut[] maps the map characters to RGB565; fairies replace hair (g h H) and
 * dress (d D) with their own colours. */
static Pixel pix_lut[128];

static void pix_lut_reset(void)
{
    const ThPixPal *p;
    int i;
    for (i = 0; i < 128; i++) pix_lut[i] = COLOR_KEY;
    for (p = th_pix_pal; p->c; p++) pix_lut[(uint8_t)p->c & 127u] = rgb(p->r, p->g, p->b);
}

static Sprite from_map1(const char *m, int w, int h)
{
    Sprite s = mk(w, h, COLOR_KEY);
    int i;
    if (!s.pixels) return s;
    for (i = 0; i < w * h; i++) ((Pixel *)s.pixels)[i] = pix_lut[(uint8_t)m[i] & 127u];
    return s;
}

/* Scale3x (AdvMAME3x): 3x pixel-art upscale that rounds the diagonal staircases
 * without blurring (only copies neighbour pixels). Outside the picture = COLOR_KEY. */
static void scale3x(const Pixel *src, int w, int h, Pixel *dst)
{
    int x, y;
#define SP(xx, yy) (((xx) < 0 || (yy) < 0 || (xx) >= w || (yy) >= h) ? COLOR_KEY : src[(yy) * w + (xx)])
    for (y = 0; y < h; y++) for (x = 0; x < w; x++) {
        const Pixel A = SP(x - 1, y - 1), B = SP(x, y - 1), C = SP(x + 1, y - 1);
        const Pixel D = SP(x - 1, y),     E = SP(x, y),     F = SP(x + 1, y);
        const Pixel G = SP(x - 1, y + 1), H = SP(x, y + 1), I = SP(x + 1, y + 1);
        Pixel *o = &dst[(3 * y) * (3 * w) + 3 * x];
        const int W3 = 3 * w;
        if (B != H && D != F) {
            o[0]          = D == B ? D : E;
            o[1]          = (D == B && E != C) || (B == F && E != A) ? B : E;
            o[2]          = B == F ? F : E;
            o[W3]         = (D == B && E != G) || (D == H && E != A) ? D : E;
            o[W3 + 1]     = E;
            o[W3 + 2]     = (B == F && E != I) || (H == F && E != C) ? F : E;
            o[2 * W3]     = D == H ? D : E;
            o[2 * W3 + 1] = (D == H && E != I) || (H == F && E != G) ? H : E;
            o[2 * W3 + 2] = H == F ? F : E;
        } else {
            o[0] = o[1] = o[2] = o[W3] = o[W3 + 1] = o[W3 + 2] = o[2 * W3] = o[2 * W3 + 1] = o[2 * W3 + 2] = E;
        }
    }
#undef SP
}

/* scratch for the character upscale (largest map 64 x 80) */
#define MAP_MAXW 64
#define MAP_MAXH 80
static Pixel map1_px[MAP_MAXW * MAP_MAXH] ART_BIG;
static Pixel map3_px[MAP_MAXW * 3 * MAP_MAXH * 3] ART_BIG;

/* pixel map -> map1_px (1x) -> map3_px (Scale3x) */
static void map_to3(const char *m, int w, int h)
{
    int i;
    for (i = 0; i < w * h; i++) map1_px[i] = pix_lut[(uint8_t)m[i] & 127u];
    scale3x(map1_px, w, h, map3_px);
}

/* character sprite from a pixel map. 1.5x: Scale3x, then each 2 x 2 block of the 3x
 * picture -> one pixel (opaque if >= 2 of 4 samples are, colour = mean of those):
 * smooth outlines with a little anti-aliasing instead of uneven nearest-neighbour pixels */
static Sprite from_map(const char *m, int w, int h)
{
    Sprite s;
    int x, y;
    if (as2 == 2 || w > MAP_MAXW || h > MAP_MAXH) return from_map1(m, w, h);
    s = mk(AS(w), AS(h), COLOR_KEY);
    if (!s.pixels) return s;
    map_to3(m, w, h);
    for (y = 0; y < s.height; y++) for (x = 0; x < s.width; x++) {
        int k, n = 0, R = 0, G = 0, B = 0;
        for (k = 0; k < 4; k++) {
            const Pixel q = map3_px[(2 * y + (k >> 1)) * (3 * w) + 2 * x + (k & 1)];
            if (q == COLOR_KEY) continue;
            ++n; R += q >> 11; G += (q >> 5) & 63; B += q & 31;
        }
        if (n >= 2) {
            Pixel o = (Pixel)(((R / n) << 11) | ((G / n) << 5) | (B / n));
            ((Pixel *)s.pixels)[y * s.width + x] = o == COLOR_KEY ? (Pixel)(o ^ 0x0020u) : o;
        }
    }
    return s;
}

/* fairy colours: dress = c, hair = a lighter neighbour colour */
static Sprite fairy(int big, int c, int frame)
{
    static const uint8_t hair_of[TC_COLORS] = { TC_YELLOW, TC_YELLOW, TC_ORANGE, TC_YELLOW,
                                                TC_GREEN, TC_CYAN, TC_PURPLE, TC_YELLOW };
    Sprite s;
    const int hc = hair_of[c];
    pix_lut['h'] = pcol(hc, 70, 240); pix_lut['H'] = pcol(hc, 190, 255); pix_lut['g'] = pcol(hc, 0, 150);
    pix_lut['d'] = pcol(c, 20, 235);  pix_lut['D'] = pcol(c, 0, 150);
    s = big ? from_map(th_pix_bigfairy[frame], 48, 48) : from_map(th_pix_fairy[frame], 32, 32);
    pix_lut_reset();
    return s;
}

/* ---------------------------------------------------------------- alpha (A8) sprites */
#define A8_POOL_BYTES (256u * 1024u)
static uint8_t a8_pool[A8_POOL_BYTES] __attribute__((aligned(64)));
static uint32_t a8_used;

/* sprite with an A8 plane (same pitch as the pixels), all transparent */
static Sprite mk_a(int w, int h)
{
    Sprite s = mk(w, h, COLOR_KEY);
    const uint32_t n = ((uint32_t)(w * h) + 63u) & ~63u;
    if (!s.pixels) return s;
    if (pool2) {
        if (a82_used + n > A82_POOL_BYTES) { s.width = s.height = 0; s.pixels = 0; return s; }
        s.alpha = &a82_pool[a82_used];
        memset(&a82_pool[a82_used], 0, n);
        a82_used += n;
        return s;
    }
    if (a8_used + n > A8_POOL_BYTES) { s.width = s.height = 0; s.pixels = 0; return s; }   /* pool full: empty */
    s.alpha = &a8_pool[a8_used];
    memset(&a8_pool[a8_used], 0, n);
    a8_used += n;
    return s;
}
static void a8_set(Sprite *s, int x, int y, Pixel c, int a)
{
    ((Pixel *)s->pixels)[y * s->width + x] = c;
    ((uint8_t *)s->alpha)[y * s->width + x] = (uint8_t)a;
}

/* 第 1 关弹幕光晕（2026-10-26）：A8 径向渐变，中心不透明度 peak，外圈平滑到 0；
 * 颜色是弹的颜色往白色调一点，中心再亮一些（逐像素：颜色和透明度都随半径变） */
static Sprite halo(int size, int c, int peak)
{
    const int N = AS(size);
    Sprite s = mk_a(N, N);
    int x, y;
    if (!s.pixels) return s;
    for (y = 0; y < N; y++) for (x = 0; x < N; x++) {
        const int X = HX(x, N), Y = HX(y, N);
        const int r = (int)isqrt((uint32_t)(X * X + Y * Y));   /* half px */
        int f;
        if (r >= N) continue;
        f = (N - r) * 256 / N;                                 /* 0 edge .. 256 centre */
        f = f * f >> 8;                                         /* soft falloff */
        a8_set(&s, x, y, pcol(c, 60 + (f * 140 >> 8), 256), peak * f >> 8);
    }
    return s;
}

/* spell card cut-in: the casting boss frame (pixel map m, w x h) at 2x (1.5x view: 3x,
 * Scale3x), fading out towards the bottom */
static Sprite portrait(const char *m, int w, int h)
{
    const int k = as2;                                    /* 2x or 3x */
    Sprite s = mk_a(w * k, h * k);
    const int f0 = 50 * k;                                /* fade starts at 1x row 50 */
    int x, y;
    if (!s.pixels) return s;
    if (k == 3) map_to3(m, w, h);
    else { int i; for (i = 0; i < w * h; i++) map1_px[i] = pix_lut[(uint8_t)m[i] & 127u]; }
    for (y = 0; y < s.height; y++) for (x = 0; x < s.width; x++) {
        const Pixel p = k == 3 ? map3_px[y * s.width + x] : map1_px[(y / 2) * w + x / 2];
        const int fade = y < f0 ? 255 : 255 * (s.height - y) / (s.height - f0);
        if (p != COLOR_KEY) a8_set(&s, x, y, p, fade);
    }
    return s;
}

/* radius in 1x half px, Q4 (1x: the old integer half px x 16) */
static int rq16(int X, int Y)
{
    return as2 == 2 ? (int)isqrt((uint32_t)(X * X + Y * Y)) * 16
                    : (int)isqrt((uint32_t)(X * X + Y * Y) * 1024u / (uint32_t)(as2 * as2));
}

/* rotating magic circle around the focused player: two rings, 6 diamonds, 12 spokes
 * (64 px at 1x; radii below in 1x half px x 16) */
static Sprite focus_circle(uint16_t rot)
{
    const int N = AS(64);
    Sprite s = mk_a(N, N);
    const Pixel c = rgb(255, 150, 200), hi = rgb(255, 230, 240);
    int x, y;
    if (!s.pixels) return s;
    for (y = 0; y < N; y++) for (x = 0; x < N; x++) {
        const int X = HX(x, N), Y = HX(y, N);
        const int r = rq16(X, Y);
        const uint16_t a = (uint16_t)(th_atan2(Y, X) - rot);
        const int sec = (int)(a % 10923u), dang = sec > 5461 ? sec - 5461 : 5461 - sec;
        const int arc = dang * (r >> 4) / 10430 * 16 + (as2 > 2 ? dang * (r & 15) / 10430 : 0);   /* along the ring */
        const int dr = r > 912 ? r - 912 : 912 - r;
        if (dr + arc <= 112)                    a8_set(&s, x, y, hi, 230);
        else if (r >= 864 && r < 976)           a8_set(&s, x, y, c, r < 880 || r >= 960 ? 90 : 170);
        else if (r >= 576 && r < 640)           a8_set(&s, x, y, c, 130);
        else if (r >= 640 && r < 864 && a % 5461u < 360u) a8_set(&s, x, y, c, 100);
    }
    return s;
}

/* big magic circle behind the boss during spell cards: rings, an 8-point star
 * (two squares), a band of rune dashes (128 px at 1x) */
static Sprite magic_circle(uint16_t rot)
{
    const int N = AS(128);
    Sprite s = mk_a(N, N);
    const Pixel c = rgb(190, 120, 255), hi = rgb(235, 205, 255);
    int x, y, k;
    if (!s.pixels) return s;
    for (y = 0; y < N; y++) for (x = 0; x < N; x++) {
        const int X = HX(x, N), Y = HX(y, N);
        const int r = rq16(X, Y);
        const uint16_t a = (uint16_t)(th_atan2(Y, X) - rot);
        int on = 0;
        if (r >= 1888 && r < 2000)     { a8_set(&s, x, y, c, r < 1904 || r >= 1984 ? 80 : 150); continue; }
        if (r >= 1696 && r < 1744)     { a8_set(&s, x, y, hi, 120); continue; }
        if (r >= 1760 && r < 1872 && a % 2048u < 640u) { a8_set(&s, x, y, c, 110); continue; }
        if (r >= 640 && r < 704)       { a8_set(&s, x, y, hi, 130); continue; }
        if (as2 > 2 && r >= 1200 && r < 1232 && a % 4096u < 2400u) { a8_set(&s, x, y, hi, 70); continue; }   /* 1.5x: extra dashed ring */
        for (k = 0; k < 8 && !on; k++) {                                 /* square A (k < 4) and B */
            const uint16_t n = (uint16_t)(rot + (uint16_t)(k & 3) * 16384u + (k >= 4 ? 8192u : 0u));
            const int32_t dist = ((X * th_cos(n) + Y * th_sin(n)) >> 12) * 2 / as2;   /* quarter half-px (1x) */
            const int32_t along = ((-X * th_sin(n) + Y * th_cos(n)) >> 14) * 2 / as2;
            if (dist >= 288 && dist <= 300 && along >= -74 && along <= 74) on = 1;   /* 73.5 half px */
        }
        if (on) a8_set(&s, x, y, c, 140);
    }
    return s;
}

static Sprite item(int kind)
{
    static const char *lab[TI_KINDS] = { "P", "+", "P", "B", "1", "" };
    static const uint8_t col[TI_KINDS] = { TC_RED, TC_BLUE, TC_RED, TC_GREEN, TC_PURPLE, TC_GREEN };
    int sz = kind == TI_BIGP || kind == TI_LIFE ? 24 : kind == TI_STAR ? 8 : 16;
    Sprite s;
    if (kind == TI_STAR) return glow(8, TC_GREEN, 120, 256);
    if (as2 > 2) {                    /* 1.5x: rounded card, bevel (lit top-left edge), centred label */
        const int N = AS(sz), sc = sz > 16 ? 3 : 2;
        int x, y;
        Gfx g;
        s = mk(N, N, COLOR_KEY);
        if (!s.pixels) return s;
        for (y = 0; y < N; y++) for (x = 0; x < N; x++) {
            const int ex = x < N - 1 - x ? x : N - 1 - x, ey = y < N - 1 - y ? y : N - 1 - y;
            Pixel *p = &((Pixel *)s.pixels)[y * N + x];
            if (ex + ey < 2) continue;                                   /* rounded corners */
            if (ex == 0 || ey == 0 || ex + ey == 2) *p = pcol(col[kind], 0, 120);   /* dark rim */
            else if (x <= 2 || y <= 2)               *p = pcol(col[kind], 150, 255); /* lit edge */
            else if (x >= N - 3 || y >= N - 3)       *p = pcol(col[kind], 0, 190);   /* shaded edge */
            else *p = pcol(col[kind], 40 + (N - y) * 50 / N, 255);                   /* body, lighter at the top */
        }
        gfx_begin(&g, (Pixel *)s.pixels, N, N);
        gfx_text(&g, (N - gfx_text_width(lab[kind], sc)) / 2 + 1, (N - 7 * sc) / 2 + 1, lab[kind], sc, pcol(col[kind], 0, 90));
        gfx_text(&g, (N - gfx_text_width(lab[kind], sc)) / 2, (N - 7 * sc) / 2, lab[kind], sc, rgb(255, 255, 255));
        gfx_end(&g);
        return s;
    }
    s = mk(sz, sz, COLOR_KEY);
    if (!s.pixels) return s;
    px_rect(&s, 1, 1, sz - 2, sz - 2, pcol(col[kind], 0, 140));
    px_rect(&s, 2, 2, sz - 4, sz - 4, pcol(col[kind], 40, 255));
    px_text(&s, sz / 2 - (sz > 16 ? 5 : 2), sz / 2 - (sz > 16 ? 7 : 3), lab[kind], sz > 16 ? 2 : 1,
            rgb(255, 255, 255));
    return s;
}

/* ---------------------------------------------------------------- backgrounds */
static uint32_t hash2(uint32_t x, uint32_t y, uint32_t seed)
{
    uint32_t h = x * 0x8DA6B343u ^ y * 0xD8163841u ^ seed * 0xCB1AB31Fu;
    h ^= h >> 13; h *= 0x5BD1E995u; h ^= h >> 15;
    return h;
}
/* periodic value noise, cell size cs, period px (x) / py (y) pixels, 0..255 */
static int vnoise(int x, int y, int cs, int px_, int py_, uint32_t seed)
{
    int cx = x / cs, cy = y / cs, fx = (x % cs) * 256 / cs, fy = (y % cs) * 256 / cs;
    int nx = px_ / cs, ny = py_ / cs;
    int a = hash2((uint32_t)(cx % nx), (uint32_t)(cy % ny), seed) & 255;
    int b = hash2((uint32_t)((cx + 1) % nx), (uint32_t)(cy % ny), seed) & 255;
    int c = hash2((uint32_t)(cx % nx), (uint32_t)((cy + 1) % ny), seed) & 255;
    int d = hash2((uint32_t)((cx + 1) % nx), (uint32_t)((cy + 1) % ny), seed) & 255;
    int sx = fx * fx * (768 - 2 * fx) >> 16, sy = fy * fy * (768 - 2 * fy) >> 16;   /* smoothstep */
    int top = a + ((b - a) * sx >> 8), bot = c + ((d - c) * sx >> 8);
    return top + ((bot - top) * sy >> 8);
}

/* the texture is 2 x 448 rows of the same picture, so any 448-row window is contiguous
 * (view size: W x H = TV(384) x TV(448); the pattern is computed in 640x480 units) */
static void bg_make(Pixel *p, int spell)
{
    const int W = TV(TH_PF_W), H = TV(TH_PF_H);
    int x, y;
    for (y = 0; y < H; y++) for (x = 0; x < W; x++) {
        const int xo = x * 2 / th_view.s2, yo = y * 2 / th_view.s2;
        int n = vnoise(xo, yo, 64, TH_PF_W, TH_PF_H, spell ? 7u : 3u) * 5 / 8 +
                vnoise(xo, yo, 16, TH_PF_W, TH_PF_H, spell ? 9u : 5u) * 3 / 8;   /* clouds 0..255 */
        int r, g, b;
        if (th_view.s2 > 2)                                              /* 1.5x: a finer cloud octave */
            n = (n * 13 + vnoise(x, y, 8, W, H, spell ? 13u : 15u) * 3) / 16;
        if (!spell) {
            r = 10 + n * 40 / 256; g = 16 + n * 50 / 256; b = 40 + n * 90 / 256;
            if (th_view.s2 > 2) {                                        /* 1.5x: stars of varied brightness */
                const uint32_t h = hash2((uint32_t)x, (uint32_t)y, 11u);
                if ((h & 2047u) < 5u) { const int v = 120 + (int)((h >> 11) & 127u); r = v; g = v + 8; b = v + 40; }
            }
            else if ((hash2((uint32_t)x, (uint32_t)y, 11u) & 1023u) < 3u) { r = 200; g = 210; b = 255; }  /* stars */
        } else {
            int cx = xo - TH_PF_W / 2, cy = (yo % 224) - 112;
            int rr = (int)isqrt((uint32_t)(cx * cx + cy * cy));
            int band = ((rr / 6) & 1) ? 18 : 0;
            r = 40 + n * 80 / 256 + band; g = 6 + n * 20 / 256; b = 40 + n * 70 / 256 + band;
        }
        p[y * W + x] = rgb(r, g, b);
        p[(y + H) * W + x] = p[y * W + x];
    }
    if (th_view.s2 > 2 && !spell) {                                      /* 1.5x: a few big stars with a cross glint */
        int k;
        for (k = 0; k < 90; k++) {
            const uint32_t h = hash2((uint32_t)k, 77u, 19u);
            const int cx = (int)(h % (uint32_t)W), cy = (int)((h >> 12) % (uint32_t)H), arm = 2 + (int)((h >> 24) & 3u);
            int d;
            for (d = -arm; d <= arm; d++) {
                const int a = 255 - (d < 0 ? -d : d) * 200 / (arm + 1);
                const int xs[2] = { cx + d, cx }, ys[2] = { cy, cy + d };
                int q;
                for (q = 0; q < 2; q++) {
                    const int xx = (xs[q] + W) % W, yy = (ys[q] + H) % H;
                    Pixel *o = &p[yy * W + xx];
                    const int pr = (*o >> 11) << 3, pg = ((*o >> 5) & 63) << 2, pb = (*o & 31) << 3;
                    *o = rgb(pr + (230 - pr) * a / 255, pg + (235 - pg) * a / 255, pb + (255 - pb) * a / 255);
                    p[(yy + H) * W + xx] = *o;
                }
            }
        }
    }
}

/* frame + info panel, natively at the view size (TV(640) x TV(480)); the layout is in
 * 640x480 units (FR = rectangle, FT = text). 720p: the picture is the whole 1280x720
 * target with decorated side strips, the 960x720 view at th_frame_ox = 160. */
int th_frame_ox;
#define FR(x, y, w, h, c) px_rect(&s, FOX + TV(x), TV(y), TV(w), TV(h), c)
#define FT(x, y, t, sc, c) gfx_text(&g, FOX + TV(x), TV(y), t, TTS(sc), c)
static void gold_rect(Sprite *s, int x, int y, int w, int h)         /* 1.5x frame line: 3-tone bevel */
{
    px_rect(s, x, y, w, h, rgb(120, 84, 40));
    if (w > 2 && h > 2) px_rect(s, x + 1, y + 1, w - 2, h - 2, rgb(236, 196, 110));
    if (w > 2 && h > 2) px_rect(s, x + 1, y + 1, w - 2, 1, rgb(255, 238, 180));
}
/* 1.5x: a faint ornament (two rings, eight-point star, dots) centred at cx, cy (px) */
static void ornament(Sprite *s, int cx, int cy, int R)
{
    int x, y;
    for (y = cy - R - 2; y <= cy + R + 2; y++) for (x = cx - R - 2; x <= cx + R + 2; x++) {
        const int X = x - cx, Y = y - cy, r = (int)isqrt((uint32_t)(X * X + Y * Y));
        const uint16_t a = (uint16_t)th_atan2(Y, X);
        const int sec = (int)(a % 8192u), d = sec < 4096 ? sec : 8192 - sec;          /* 8 points */
        const int star = R * 7 / 10 - d * (R * 4 / 10) / 4096;
        Pixel *p;
        int k = 0;
        if (x < 0 || y < 0 || x >= s->width || y >= s->height) continue;
        if (r == R || r == R - 4) k = 70;
        else if (r < R - 6 && r > R - 12 && (a % 2731u) < 500u) k = 50;
        else if (r <= star && r >= star - 2) k = 60;
        else if (r <= 6) k = 40;
        if (!k) continue;
        p = &((Pixel *)s->pixels)[y * s->width + x];
        {
            const int pr = *p >> 11, pg = (*p >> 5) & 63, pb = *p & 31;
            *p = (Pixel)(((pr + (31 - pr) * k / 255) << 11) | ((pg + (60 - pg) * k / 400) << 5) | (pb + (20 - pb) * k / 400));
        }
    }
}
static void frame_make(void)
{
    const int hd = th_view.s2 > 2;
    const int FW = hd ? 1280 : TV(640), FH = TV(480), FOX = hd ? 160 : 0;
    Sprite s;
    int x, y;
    th_frame_ox = FOX;
    s.width = FW; s.height = FH; s.pixels = frame_px; s.alpha = 0;
    for (y = 0; y < FH; y++) for (x = 0; x < FW; x++) {
        int n = vnoise((x - FOX + 640) * 2 / th_view.s2, y * 2 / th_view.s2, 32, 1280, 480, 21u);
        if (hd) n = (n * 3 + vnoise(x, y, 8, 1280, 720, 23u)) / 4;          /* finer grain */
        if (!hd) n = vnoise(x * 2 / th_view.s2, y * 2 / th_view.s2, 32, 640, 480, 21u);
        frame_px[y * FW + x] = rgb(60 + n / 6, 18 + n / 12, 30 + n / 10);
    }
    if (!hd) {
        FR(TH_PF_X - 2, TH_PF_Y - 2, TH_PF_W + 4, 2, rgb(200, 160, 90));          /* gold edge */
        FR(TH_PF_X - 2, TH_PF_Y + TH_PF_H, TH_PF_W + 4, 2, rgb(200, 160, 90));
        FR(TH_PF_X - 2, TH_PF_Y, 2, TH_PF_H, rgb(200, 160, 90));
        FR(TH_PF_X + TH_PF_W, TH_PF_Y, 2, TH_PF_H, rgb(200, 160, 90));
        FR(432, 24, 192, 300, rgb(30, 10, 22));                                   /* info panel */
        FR(432, 24, 192, 2, rgb(150, 110, 60));
        FR(432, 322, 192, 2, rgb(150, 110, 60));
    } else {
        const int px = FOX + TV(TH_PF_X), py = TV(TH_PF_Y), pw = TV(TH_PF_W), ph = TV(TH_PF_H);
        const int ix = FOX + TV(432), iy = TV(24), iw = TV(192), ih = TV(300);
        /* playfield: 3-px bevelled gold edge, dark shadow outside */
        px_rect(&s, px - 6, py - 6, pw + 12, 2, rgb(30, 8, 18));
        gold_rect(&s, px - 3, py - 3, pw + 6, 3);
        gold_rect(&s, px - 3, py + ph, pw + 6, 3);
        gold_rect(&s, px - 3, py, 3, ph);
        gold_rect(&s, px + pw, py, 3, ph);
        /* info panel: vertical gradient, gold lines top / bottom, thin inner line */
        for (y = 0; y < ih; y++) px_rect(&s, ix, iy + y, iw, 1, rgb(34 - y * 14 / ih, 10, 26 - y * 8 / ih));
        gold_rect(&s, ix, iy, iw, 3);
        gold_rect(&s, ix, iy + ih - 3, iw, 3);
        px_rect(&s, ix + 4, iy + 6, 1, ih - 12, rgb(90, 60, 40));
        px_rect(&s, ix + iw - 5, iy + 6, 1, ih - 12, rgb(90, 60, 40));
        /* screen border and the side strips */
        gold_rect(&s, 6, 6, FW - 12, 3);
        gold_rect(&s, 6, FH - 9, FW - 12, 3);
        gold_rect(&s, 6, 6, 3, FH - 12);
        gold_rect(&s, FW - 9, 6, 3, FH - 12);
        ornament(&s, 80, 200, 56);
        ornament(&s, 80, 520, 40);
        ornament(&s, FW - 80, 200, 40);
        ornament(&s, FW - 80, 520, 56);
        ornament(&s, FOX + TV(530), TV(395), 70);                         /* behind the logo */
    }
    {
        Gfx g;
        gfx_begin(&g, frame_px, FW, FH);
        if (hd) {                                                           /* drop shadows */
            const Pixel sh = rgb(20, 4, 12);
            gfx_text(&g, FOX + TV(444) + 3, TV(360) + 3, "东方", TTS(4), sh);
            gfx_text(&g, FOX + TV(444) + 3, TV(396) + 3, "弹幕游戏", TTS(3), sh);
        }
        FT(440, 36,  "最高分", 2, rgb(200, 200, 220));
        FT(440, 72,  "得分",   2, rgb(240, 240, 255));
        FT(440, 108, "生命",  2, rgb(255, 150, 190));
        FT(440, 138, "符卡",    2, rgb(130, 240, 140));
        FT(440, 180, "火力",   2, rgb(255, 120, 100));
        FT(440, 210, "擦弹",   2, rgb(200, 200, 220));
        FT(440, 240, "收集",   2, rgb(120, 170, 255));
        FT(444, 360, "东方", 4, rgb(255, 200, 120));
        FT(444, 396, "弹幕游戏", 3, rgb(255, 230, 200));
        FT(444, 428, "TI60 硬件加速", 1, rgb(180, 160, 150));
        gfx_end(&g);
    }
    th_spr_frame.width = FW; th_spr_frame.height = FH; th_spr_frame.pixels = frame_px; th_spr_frame.alpha = 0;
}
#undef FR
#undef FT

/* ---------------------------------------------------------------- stage 2 art */
Sprite th_bg2_sky, th_bg2_level[TH_BG2_LEVELS], th_spr_moon, th_spr_bigmoon;
uint8_t  th_bg2_row_level[PF_H_MAX];
uint16_t th_bg2_row_z[PF_H_MAX];
static Pixel bg2_sky_px[PF_W_MAX * (TH_BG2_HZ * 3 / 2)] ART_BIG;
static Pixel bg2_lv_px[TH_BG2_LEVELS][TH_BG2_TEX_H * PF_W_MAX] ART_BIG;
static Pixel bg2_tex[TH_BG2_TEX_H][256];

/* ghost flame (A8): round body, flickering tip, two dark eyes; frame 0 / 1 */
static Sprite wisp(int c, int frame)
{
    Sprite s = mk_a(24, 32);
    int x, y;
    if (!s.pixels) return s;
    for (y = 0; y < 32; y++) for (x = 0; x < 24; x++) {
        const int X = 2 * x + 1 - 24;                                   /* half px from the centre line */
        int hw, a = 0, w;
        if (y >= 13) {                                                  /* body: circle r 8 at row 21 */
            const int Y = 2 * y + 1 - 42, r2 = X * X + Y * Y;
            if (r2 <= 256) { a = 255 - r2 * 160 / 256; }
        }
        if (!a && y >= 2 && y < 22) {                                   /* tip: narrows upwards, sways */
            const int sway = (th_sin((uint16_t)(y * 3000 + frame * 32768)) * 3) >> 14;
            hw = (y - 2) * 16 / 20;                                     /* half px */
            if (X - 2 * sway >= -hw && X - 2 * sway <= hw) a = 90 + (y - 2) * 5;
        }
        if (!a) continue;
        w = a > 200 ? 200 : a > 150 ? 110 : 30;                         /* whiter in the core */
        a8_set(&s, x, y, pcol(c, w, 256), a);
    }
    a8_set(&s, 9, 20, rgb(20, 20, 50), 255);  a8_set(&s, 9, 21, rgb(20, 20, 50), 255);
    a8_set(&s, 14, 20, rgb(20, 20, 50), 255); a8_set(&s, 14, 21, rgb(20, 20, 50), 255);
    return s;
}

/* moon (A8): disc of radius r px with craters and a darker limb, soft glow ring g px */
static Sprite moon(int size, int r, int gw, uint32_t seed)
{
    Sprite s;
    int x, y;
    size = AS(size); r = AS(r); gw = AS(gw);              /* 1x pixels -> native size */
    s = mk_a(size, size);
    if (!s.pixels) return s;
    for (y = 0; y < size; y++) for (x = 0; x < size; x++) {
        const int X = HX(x, size), Y = HX(y, size);
        const int d = (int)isqrt((uint32_t)(X * X + Y * Y));           /* half px */
        if (d <= 2 * r) {
            int n = vnoise(x + 512, y + 512, size / 6 > 4 ? size / 6 : 4, 1 << 20, 1 << 20, seed);
            if (as2 > 2)                                                /* 1.5x: small craters too */
                n = (n * 3 + vnoise(x + 900, y + 300, size / 20 > 3 ? size / 20 : 3, 1 << 20, 1 << 20, seed + 7u)) / 4;
            const int limb = 256 - d * d * 70 / (4 * r * r);            /* 256 .. 186 */
            const int crater = n > 150 ? (n - 150) / 3 : 0;
            a8_set(&s, x, y, rgb((250 - crater) * limb >> 8, (244 - crater) * limb >> 8, (214 - crater) * limb >> 8), 255);
        } else if (d <= 2 * (r + gw)) {
            const int f = 2 * (r + gw) - d;                             /* 0 .. 2 gw */
            a8_set(&s, x, y, rgb(214, 226, 255), f * f * 150 / (4 * gw * gw));
        }
    }
    return s;
}

/* drawn natively at the view size: PW x HZ sky, PW-wide texture levels, PH row tables
 * (in view pixels a level's scale is texels per VIEW pixel, so the level choice per row
 * is the same formula; the depth is 18944 / dy in 640x480 rows = 18944 * s2 / 2 / dy) */
static void bg2_make(void)
{
    const int PW = TV(TH_PF_W), PH = TV(TH_PF_H), HZ = TV(TH_BG2_HZ);
    int x, y, l;
    int32_t s_q8 = 64;                                                  /* 0.25 texels per pixel */
    int32_t level_s[TH_BG2_LEVELS];
    /* sky: gradient, stars, a faint band of light (band in 640x480 units) */
    for (y = 0; y < HZ; y++) for (x = 0; x < PW; x++) {
        const int xo = x * 2 / th_view.s2, yo = y * 2 / th_view.s2;
        int r = 4 + y * 42 / HZ, g = 6 + y * 50 / HZ, b = 26 + y * 96 / HZ;
        const uint32_t h = hash2((uint32_t)x, (uint32_t)y, 31u);
        const int band = vnoise(xo + yo, yo * 3, 32, 1024, 1024, 37u);
        const int dband = xo - 2 * yo - 40 < 0 ? 2 * yo + 40 - xo : xo - 2 * yo - 40;
        if (dband < 60) { const int k = (60 - dband) * band / 600; r += k; g += k; b += k * 2; }
        if ((h & 1023u) < 4u) { const int v = 120 + (int)((h >> 10) & 127u); r = v; g = v; b = v + 30; }
        bg2_sky_px[y * PW + x] = rgb(r, g, b);
    }
    th_bg2_sky.width = PW; th_bg2_sky.height = HZ; th_bg2_sky.pixels = bg2_sky_px; th_bg2_sky.alpha = 0;
    /* periodic cloud texture 256 x 64 */
    for (y = 0; y < TH_BG2_TEX_H; y++) for (x = 0; x < 256; x++) {
        const int n = (vnoise(x, y, 32, 256, TH_BG2_TEX_H, 41u) * 5 + vnoise(x, y, 8, 256, TH_BG2_TEX_H, 43u) * 3) / 8;
        const int t = n * n / 255;
        int r = 14 + t * 136 / 255, g = 20 + t * 146 / 255, b = 58 + t * 166 / 255;
        if (n > 190) { r += (n - 190) * 2; g += (n - 190) * 2; b += n - 190; }   /* moonlit tops */
        bg2_tex[y][x] = rgb(r * 3 / 4, g * 3 / 4, b * 4 / 5);         /* a bit dark: bullets must stand out */
    }
    /* scale levels: s_l = 0.25 * 1.2^l, centred on the middle column, wide levels averaged */
    for (l = 0; l < TH_BG2_LEVELS; l++) {
        Pixel *p = bg2_lv_px[l];
        const int ns = s_q8 > 256 ? (s_q8 > 768 ? 4 : 2) : 1;
        level_s[l] = s_q8;
        for (y = 0; y < TH_BG2_TEX_H; y++) for (x = 0; x < PW; x++) {
            int k, r = 0, g = 0, b = 0;
            for (k = 0; k < ns; k++) {
                const int32_t u = ((((int32_t)x - PW / 2) * s_q8 + k * s_q8 / ns) >> 8) + 128;
                const Pixel c = bg2_tex[y][u & 255];
                r += c >> 11; g += (c >> 5) & 63; b += c & 31;
            }
            p[y * PW + x] = (Pixel)(((r / ns) << 11) | ((g / ns) << 5) | (b / ns));
        }
        th_bg2_level[l].width = PW; th_bg2_level[l].height = TH_BG2_TEX_H;
        th_bg2_level[l].pixels = p; th_bg2_level[l].alpha = 0;
        s_q8 = s_q8 * 307 / 256;                                        /* x 1.2 */
    }
    /* per screen row: level whose scale is closest to 74 / dy, depth 18944 / dy */
    for (y = 0; y < PH; y++) {
        const int dy = y - HZ + 1;
        int32_t want;
        if (dy <= 0) { th_bg2_row_level[y] = 0; th_bg2_row_z[y] = 0; continue; }
        want = 74 * 256 / dy;
        for (l = 0; l < TH_BG2_LEVELS - 1 && level_s[l + 1] <= want; l++) ;
        if (l < TH_BG2_LEVELS - 1 && want - level_s[l] > level_s[l + 1] - want) ++l;
        th_bg2_row_level[y] = (uint8_t)l;
        th_bg2_row_z[y] = (uint16_t)(18944 * th_view.s2 / 2 / dy);
    }
}

/* ---------------------------------------------------------------- 1.5x copies */

static void unpack(Pixel p, int *r, int *g, int *b) { *r = p >> 11; *g = (p >> 5) & 63; *b = p & 31; }

/* replace s by a copy scaled to TV(w) x TV(h). A8 sprites: colour weighted by alpha,
 * alpha bilinear; keyed sprites: bilinear coverage of the non-key pixels (>= 1/2 =
 * opaque, colour from those pixels only); others (additive glows): plain bilinear.
 * nearest = 1: pixel-map characters and text (sharp, a little uneven). */
static void up_sprite(Sprite *s, int nearest)
{
    const int w = s->width, h = s->height, W = TV(w), H = TV(h);
    const Pixel *sp = s->pixels;
    const uint8_t *sa = s->alpha;
    uint32_t n = ((uint32_t)(W * H) + 31u) & ~31u, na = ((uint32_t)(W * H) + 63u) & ~63u;
    Pixel *d;
    uint8_t *da = 0;
    int keyed = 0, x, y, i;
    if (!sp || w <= 0 || h <= 0 || W <= 0 || H <= 0) return;
    if (art2_used + n > ART2_POOL_PIXELS) return;                 /* pool full: stays 1x */
    if (sa && a82_used + na > A82_POOL_BYTES) return;
    d = &art2_pool[art2_used]; art2_used += n;
    if (sa) { da = &a82_pool[a82_used]; a82_used += na; }
    else for (i = 0; i < w * h && !keyed; i++) keyed = sp[i] == COLOR_KEY;
    for (y = 0; y < H; y++) {
        int v = ((2 * y + 1) * h * 128) / H - 128, y0, y1, fy;
        if (v < 0) v = 0;
        y0 = v >> 8; fy = v & 255; y1 = y0 + 1 < h ? y0 + 1 : y0;
        for (x = 0; x < W; x++) {
            int u = ((2 * x + 1) * w * 128) / W - 128, x0, x1, fx, k;
            int idx[4], wt[4], R = 0, G = 0, B = 0, A = 0, cov = 0;
            Pixel out;
            if (nearest) {
                const int j = (y * h / H) * w + x * w / W;
                d[y * W + x] = sp[j];
                if (da) da[y * W + x] = sa[j];
                continue;
            }
            if (u < 0) u = 0;
            x0 = u >> 8; fx = u & 255; x1 = x0 + 1 < w ? x0 + 1 : x0;
            idx[0] = y0 * w + x0; idx[1] = y0 * w + x1; idx[2] = y1 * w + x0; idx[3] = y1 * w + x1;
            wt[0] = (256 - fx) * (256 - fy); wt[1] = fx * (256 - fy); wt[2] = (256 - fx) * fy; wt[3] = fx * fy;
            for (k = 0; k < 4; k++) {                            /* weights sum to 65536 */
                int r, g, b, ww = wt[k];
                if (sa) ww = (int)(((uint32_t)ww * sa[idx[k]]) >> 8);
                else if (keyed && sp[idx[k]] == COLOR_KEY) continue;
                unpack(sp[idx[k]], &r, &g, &b);
                R += r * ww; G += g * ww; B += b * ww; cov += ww;
            }
            if (sa) {
                A = cov >> 8;                                    /* 0..255 */
                da[y * W + x] = (uint8_t)(A > 255 ? 255 : A);
                if (!cov) { d[y * W + x] = COLOR_KEY; continue; }
            } else if (keyed && cov * 2 < 65536) {
                d[y * W + x] = COLOR_KEY;
                continue;
            }
            if (!cov) cov = 1;
            out = (Pixel)(((R / cov) << 11) | ((G / cov) << 5) | (B / cov));
            if (out == COLOR_KEY && (keyed || sa)) out ^= 0x0020u;      /* never the key by accident */
            d[y * W + x] = out;
        }
    }
    s->width = W; s->height = H; s->pixels = d; s->alpha = da;
}

/* ---------------------------------------------------------------- init */
/* every sprite, drawn natively at the art scale as2 (1.5x view: into the 1.5x pools) */
static void make_sprites(void)
{
    int c, a, f;
    memset(th_bspr_n, 0, sizeof th_bspr_n);
    for (c = 0; c < TBK_COUNT; c++)                   /* centre = half size (tight boxes below) */
        for (a = 0; a < 64; a++) th_bspr_cx[c][a] = th_bspr_cy[c][a] = (uint8_t)AS(th_bk_half[c]);
    /* Bucket by bucket, so the sprites of one bullet type sit together in memory:
     * the engine's source cache is 16 KB direct-mapped (address bits 13:5), and
     * a pattern usually uses one or two types in one or two colours. Interleaved
     * per colour, the yellow and cyan stars of "Twin Galaxy" shared cache lines
     * and evicted each other all the time (board: 30 % misses, engine 99 % busy). */
    for (c = 0; c < TC_COLORS; c++) th_bspr[TBK_PELLET][c] = round_bullet(8, c, 1, 3);
    for (c = 0; c < TC_COLORS; c++) th_bspr[TBK_ORB][c]    = round_bullet(16, c, 3, 6);
    for (c = 0; c < TC_COLORS; c++)
        for (f = 0; f < TH_STAR_FRAMES; f++) {
            /* 1.5x: 4 of the 8 spin frames (pairs share one picture). 2 colours x 8 frames at
             * 24x24 = 18 KB did not fit the engine's 16 KB source cache ("Twin Galaxy" x32:
             * 25 % misses in the host model, 1 % at 1x); 4 frames = 9 KB */
            if (as2 > 2 && (f & 1)) th_bspr[TBK_STAR][c * TH_STAR_FRAMES + f] = th_bspr[TBK_STAR][c * TH_STAR_FRAMES + f - 1];
            else th_bspr[TBK_STAR][c * TH_STAR_FRAMES + f] = star_bullet(c, (uint16_t)(f * (13107 / TH_STAR_FRAMES)));
        }
    for (c = 0; c < TC_COLORS; c++)
        for (a = 0; a < TH_ROT_STEPS; a++) {
            const int b = c < 4 ? TBK_RICE0 : TBK_RICE1, i = (c & 3) * TH_ROT_STEPS + a;
            /* 1.5x: the rice grain is the same picture turned by 180 degrees (symmetric shape,
             * light fixed on screen): 8 pictures per colour, half the source cache lines */
            if (as2 > 2 && a >= TH_ROT_STEPS / 2) {
                th_bspr[b][i] = th_bspr[b][i - TH_ROT_STEPS / 2];
                th_bspr_cx[b][i] = th_bspr_cx[b][i - TH_ROT_STEPS / 2];
                th_bspr_cy[b][i] = th_bspr_cy[b][i - TH_ROT_STEPS / 2];
                continue;
            }
            th_bspr[b][i] = rot_bullet(16, c, (uint16_t)(a * (65536 / TH_ROT_STEPS)), 0);
            crop_tight(&th_bspr[b][i], AS(8), &th_bspr_cx[b][i], &th_bspr_cy[b][i]);
        }
    for (c = 0; c < TC_COLORS; c++)
        for (a = 0; a < TH_ROT_STEPS; a++) {
            const int b = c < 4 ? TBK_KUNAI0 : TBK_KUNAI1, i = (c & 3) * TH_ROT_STEPS + a;
            th_bspr[b][i] = rot_bullet(24, c, (uint16_t)(a * (65536 / TH_ROT_STEPS)), 1);
            crop_tight(&th_bspr[b][i], AS(12), &th_bspr_cx[b][i], &th_bspr_cy[b][i]);
        }
    for (c = 0; c < TC_COLORS; c++) th_bspr[TBK_BIG][c]    = glow(32, c, 200, 256);
    for (c = 0; c < TC_COLORS; c++) th_bspr[TBK_BUBBLE][c] = ring(64, c, 48, 16, 256);
    th_bspr_n[TBK_PELLET] = th_bspr_n[TBK_ORB] = th_bspr_n[TBK_BIG] = th_bspr_n[TBK_BUBBLE] = TC_COLORS;
    th_bspr_n[TBK_RICE0] = th_bspr_n[TBK_RICE1] = th_bspr_n[TBK_KUNAI0] = th_bspr_n[TBK_KUNAI1] = 64;
    th_bspr_n[TBK_STAR] = 64;

    pix_lut_reset();
    for (f = 0; f < 4; f++) th_spr_player[f] = from_map(th_pix_player[f], 32, 48);
    th_spr_option = round_bullet(16, TC_RED, 4, 6);
    th_spr_hitbox = mk(AS(16), AS(16), COLOR_KEY);
    if (th_spr_hitbox.pixels) {
        px_disc(&th_spr_hitbox, AS(16), AS(16), AS(9), rgb(255, 40, 60));
        if (as2 > 2) px_disc(&th_spr_hitbox, AS(16), AS(16), AS(9) - 2, rgb(255, 120, 130));
        px_disc(&th_spr_hitbox, AS(16), AS(16), AS(6), rgb(255, 255, 255));
    }
    th_spr_shot_main = mk(AS(12), AS(24), 0);
    if (th_spr_shot_main.pixels) {
        px_rect(&th_spr_shot_main, AS(3), 0, AS(6), AS(24), rgb(110, 40, 50));
        px_rect(&th_spr_shot_main, AS(5), AS(2), AS(2), AS(20), rgb(180, 150, 150));
        if (as2 > 2) px_rect(&th_spr_shot_main, AS(5) + 1, AS(3), 1, AS(18), rgb(240, 220, 220));   /* bright core line */
    }
    th_spr_shot_opt = glow(12, TC_BLUE, 120, 200);
    for (c = 0; c < 4; c++) {
        static const uint8_t ec[4] = { TC_BLUE, TC_RED, TC_GREEN, TC_YELLOW };
        for (f = 0; f < 2; f++) {
            th_spr_enemy[TE_FAIRY][c][f]    = fairy(0, ec[c], f);
            th_spr_enemy[TE_FAIRY2][c][f]   = fairy(0, (ec[c] + 2) % TC_COLORS, f);
            th_spr_enemy[TE_BIGFAIRY][c][f] = fairy(1, ec[c], f);
        }
    }
    th_spr_boss[0][0] = from_map(th_pix_boss[0], 64, 80);
    th_spr_boss[0][1] = from_map(th_pix_boss[1], 64, 80);
    th_spr_boss[1][0] = from_map(th_pix_boss2[0], 64, 80);
    th_spr_boss[1][1] = from_map(th_pix_boss2[1], 64, 80);
    if (!pool2) a8_used = 0;
    th_spr_portrait[0] = portrait(th_pix_boss[1], 64, 80);
    th_spr_portrait[1] = portrait(th_pix_boss2[1], 64, 80);
    for (f = 0; f < TH_FOCUS_FRAMES; f++) th_spr_focus[f] = focus_circle((uint16_t)(f * (10923 / TH_FOCUS_FRAMES)));
    for (f = 0; f < TH_MAGIC_FRAMES; f++) th_spr_magic[f] = magic_circle((uint16_t)(f * (8192 / TH_MAGIC_FRAMES)));
    {
        /* ghost flames: soft A8 shapes, drawn at 1x and scaled (bilinear) for the 1.5x view */
        static const uint8_t wc[4] = { TC_BLUE, TC_CYAN, TC_PURPLE, TC_WHITE };
        const int keep_as2 = as2, keep_pool2 = pool2;
        as2 = 2; pool2 = 0;
        for (c = 0; c < 4; c++)
            for (f = 0; f < 2; f++) th_spr_enemy[TE_WISP][c][f] = wisp(wc[c], f);
        as2 = keep_as2; pool2 = keep_pool2;
        if (as2 > 2)
            for (c = 0; c < 4; c++)
                for (f = 0; f < 2; f++) up_sprite(&th_spr_enemy[TE_WISP][c][f], 0);
    }
    th_spr_moon = moon(96, 30, 16, 51u);
    th_spr_bigmoon = moon(192, 70, 24, 53u);
    th_spr_boss_aura = ring(64, TC_PURPLE, 40, 24, 200);
    for (a = 0; a < TI_KINDS; a++) th_spr_item[a] = item(a);
    for (c = 0; c < TC_COLORS; c++) {
        th_spr_fx[TF_SPARK][c]  = glow(8, c, 160, 256);
        th_spr_fx[TF_GLOW16][c] = glow(16, c, 140, 220);
        th_spr_fx[TF_GLOW32][c] = glow(32, c, 140, 220);
        th_spr_fx[TF_GLOW64][c] = glow(64, c, 120, 200);
        th_spr_fx[TF_RING][c]   = ring(64, c, 52, 10, 220);
    }
    th_spr_life = star_bullet(TC_PURPLE, 0);
    th_spr_bombicon = star_bullet(TC_GREEN, 0);
    th_spr_bomb_ring = glow(64, TC_WHITE, 200, 140);
    for (c = 0; c < TC_COLORS; c++) {                    /* 第 1 关弹幕：光晕 / 发射光（A8） */
        th_spr_halo[0][c] = halo(24, c, 190);
        th_spr_halo[1][c] = halo(40, c, 180);
        th_spr_halo[2][c] = halo(72, c, 170);
    }
}

void th_art_init(void)
{
    art_used = 0; a8_used = 0;
    art2_used = 0; a82_used = 0;
    /* 1.5x view: every sprite drawn natively at 1.5x into the 1.5x pools (the 1x pools
     * only hold the ghost flames, which are scaled up); 640x480: 1x as before */
    as2 = th_view.s2;
    pool2 = as2 != 2;
    make_sprites();
    th_pressure_scale_set(th_pressure_scale());
    as2 = 2; pool2 = 0;
    th_art_pool_used = art_used;
    th_art_a8_used = a8_used;             /* pool fill (pixels), for the boot message / host test */
    th_art2_pool_used = art2_used;
    th_art2_a8_used = a82_used;
    bg2_make();
    bg_make(bg_px, 0);
    bg_make(bgs_px, 1);
    th_spr_bg.width = TV(TH_PF_W); th_spr_bg.height = TV(TH_PF_H) * 2; th_spr_bg.pixels = bg_px; th_spr_bg.alpha = 0;
    th_spr_bg_spell = th_spr_bg; th_spr_bg_spell.pixels = bgs_px;
    frame_make();
}
