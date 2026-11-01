#ifndef SPROUT_GFX_H
#define SPROUT_GFX_H
#include <stdint.h>

typedef uint16_t Pixel;
#define RGB565(r,g,b) ((Pixel)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
#define COLOR_KEY ((Pixel)0xf81f)
/*
 * alpha: optional per-pixel alpha plane (8 bit, 0 = transparent, 255 = opaque,
 * same pitch as pixels). NULL = opaque sprite (colour key may still apply).
 * Old initialisers {w,h,pixels} leave it NULL.
 */
typedef struct { int width,height; const Pixel *pixels; const uint8_t *alpha; } Sprite;
#define GFX_OPAQUE 255
/* Mode bits of the batched calls (gfx_blit_list / _inside / gfx_blit_table, the
 * old "key_enabled" argument; 1 still means colour key):
 *   GFX_BLIT_KEY  skip COLOR_KEY pixels
 *   GFX_BLIT_ADD  additive: dst += src per channel, saturated (glowing bullets)
 * Batched draws never use the sprites' A8 planes (also with GFX_BLIT_ADD). */
#define GFX_BLIT_KEY 1
#define GFX_BLIT_ADD 2
/* One entry of a batched draw list: sprites[sprite] at (x,y), see gfx_blit_list. */
typedef struct { int16_t x,y; int32_t sprite; } GfxItem;
/* 表项 x、y 用一次 32 位 store 写（热循环用，2026-10-26）。不能用 memcpy：RV32 的 GCC 不信指针类型的对齐，
 * 把 4 字节 memcpy 展开成 4 个 sb、还把临时变量放到栈上（板上反汇编看到的，每颗弹多了 3 次 DDR store）。
 * may_alias 让这次写不违反别名规则；GfxItem 4 字节对齐，所以就是一条 sw。小端：低半字 = x。 */
typedef uint32_t __attribute__((may_alias)) gfx_u32_alias;
static inline void gfx_item_xy(GfxItem *it, int x, int y)
{
    *(gfx_u32_alias *)(void *)it = (uint32_t)(uint16_t)x | (uint32_t)(uint16_t)y << 16;
}
/* 填充表的一项（2026-10-26，gfx_fill_table / 引擎 FTAB）：w x h 的纯色块画在 (x,y)，
 * 自己的颜色和 alpha（255 = 不透明）。8 字节，格式就是引擎读的格式，用下面两个宏写。 */
typedef struct { uint32_t xyw, cha; } GfxFillItem;
#define GFX_FILL_XYW(x,y,w) ((uint32_t)(x) | ((uint32_t)(y) << 12) | ((uint32_t)(w) << 24))
#define GFX_FILL_CHA(c,h,a) ((uint32_t)(c) | ((uint32_t)(h) << 16) | ((uint32_t)(a) << 24))
typedef struct {
    void (*begin_frame)(void *context);
    void (*solid_fill)(void *context,int x,int y,int w,int h,Pixel color);
    void (*block_copy)(void *context,int x,int y,const Sprite *sprite,
        int src_x,int src_y,int w,int h,int color_key);
    void (*end_frame)(void *context);
    /* Alpha hooks for the BitBlt engine (dst = src*a + dst*(1-a), a = 0..255). */
    void (*alpha_fill)(void *context,int x,int y,int w,int h,Pixel color,int alpha);
    void (*alpha_copy)(void *context,int x,int y,const Sprite *sprite,
        int src_x,int src_y,int w,int h,int color_key,int alpha);
    /* Optional (may be 0). publish: the CPU just wrote memory the backend will
     * read (e.g. a new font atlas) - make it visible (fence). wait_idle: every
     * queued command is finished, so memory it reads may be overwritten. */
    void (*publish)(void *context);
    void (*wait_idle)(void *context);
    /* Optional (may be 0). Batched block copies: item i = sprites[it[i].sprite]
     * at (it[i].x,it[i].y), whole sprite, in list order. gfx_blit_list only
     * passes items that lie completely inside the target (no clipping needed). */
    void (*copy_list)(void *context,const Sprite *sprites,const GfxItem *it,int n,int color_key);
    /* 可选（可为 0）。精灵表直接交给后端（BitBlt：引擎自己读表，CPU 不再逐项写命令）。
     * 条件同 copy_list，另外 it[].sprite < nsprites，且 it[] 在后端空闲前不能改。
     * 返回 1 = 已提交；0 = 这次做不了（模式 / 硬件不支持），调用者改走 copy_list。 */
    int (*copy_table)(void *context,const Sprite *sprites,int nsprites,const GfxItem *it,int n,int color_key);
    /* 可选（可为 0，2026-10-11）：加法混合 dst += src * alpha / 256（每通道饱和，见 gfx_fill_add /
     * gfx_blit_add）。上面 copy_list / copy_table 的 color_key 参数现在是 GFX_BLIT_* 模式位。 */
    void (*add_fill)(void *context,int x,int y,int w,int h,Pixel color,int alpha);
    void (*add_copy)(void *context,int x,int y,const Sprite *sprite,
        int src_x,int src_y,int w,int h,int color_key,int alpha);
    /* 可选（可为 0，2026-10-26）：填充表直接交给后端（BitBlt：引擎读表，FTAB）。
     * 项都在目标内；it[] 在后端画完前不能改。返回 1 = 已提交，0 = 做不了（调用者逐项画）。 */
    int (*fill_table)(void *context,const GfxFillItem *it,int n,int add);
    /* 可选（可为 0，2026-10-26）：同 copy_table，整张表带全局透明度 alpha（0..255；加法模式 = 加法强度） */
    int (*copy_table_ga)(void *context,const Sprite *sprites,int nsprites,const GfxItem *it,int n,int mode,int alpha);
} GfxOps;

/*
@width,height；宽、高
@pixels:起始位置
@fills,blits,pixels_written,alphas,alpha_pixels：pixels为像素点数，别的为操作数量
*/
typedef struct {
    int width,height;
    int stride;                 /* CPU backend: pixels per framebuffer row (>= width) */
    Pixel *pixels;
    const GfxOps *ops;
    void *context;
    uint32_t fills,blits,pixels_written;
    uint32_t alphas,alpha_pixels;   /* alpha operations / blended pixels this frame */
} Gfx;

/* CPU framebuffer backend. */
void gfx_begin(Gfx *gfx, Pixel *pixels, int width, int height);
/* CPU backend into a part of a wider framebuffer: rows are stride pixels apart
 * (2026-10-21: the 640x480 game view centred in a 1280x720 framebuffer). */
void gfx_begin_stride(Gfx *gfx, Pixel *pixels, int width, int height, int stride);
/* Command backend used by FPGA/BitBlt ports. */
void gfx_begin_backend(Gfx *gfx,const GfxOps *ops,void *context,int width,int height);
void gfx_end(Gfx *gfx);
void gfx_fill_rect(Gfx *gfx,int x,int y,int w,int h,Pixel color);
/* pitch is Sprite.width pixels. key_enabled provides optional color-key blit. */
void gfx_blit(Gfx *gfx,int x,int y,const Sprite *sprite,int key_enabled);
/* Copy only the sub-rectangle (sx,sy,w,h) of the sprite to (x,y), clipped. */
void gfx_blit_part(Gfx *gfx,int x,int y,const Sprite *sprite,int sx,int sy,int w,int h,int key_enabled);
/* 精灵的 (sx,sy,w,h) 部分以全局 alpha（0..255）画（2026-10-26） */
void gfx_blit_part_alpha(Gfx *gfx,int x,int y,const Sprite *sprite,int sx,int sy,int w,int h,int key_enabled,int alpha);
/*
 * Batched draw list: same pixels and counters as calling
 *     gfx_blit(gfx,it[i].x,it[i].y,&sprites[it[i].sprite],key_enabled)
 * for i = 0..n-1 in order, but items fully on screen go to ops->copy_list in
 * one call (the BitBlt backend then writes only DST + CMD per item). Items
 * that need clipping fall back to gfx_blit. CPU backend: plain gfx_blit loop.
 */
void gfx_blit_list(Gfx *gfx,const Sprite *sprites,const GfxItem *it,int n,int key_enabled);
/*
 * 同 gfx_blit_list，但由调用者保证每一项都完全在目标内（不再逐项检查、不裁剪），
 * 整张表一次交给 ops->copy_list，省掉 gfx_blit_list 的那一遍边界检查。
 * pixels = 这些项的总像素数（只用于 pixels_written 统计，例如 n * 9 * 9）。
 * 不能保证时请用 gfx_blit_list。CPU 后端：普通 gfx_blit 循环。
 */
void gfx_blit_list_inside(Gfx *gfx,const Sprite *sprites,const GfxItem *it,int n,int key_enabled,uint32_t pixels);
/*
 * 同 gfx_blit_list_inside（每项完全在目标内），另外要求：
 *   每项 it[i].sprite 在 0..nsprites-1 之间；
 *   it[] 这块内存在后端画完之前（下一次 wait_idle / blit_wait 之前）不能改写。
 * BitBlt 后端（LIST 模式且引擎有 CAPS[28]、nsprites <= 64）把整张表的地址交给引擎，
 * CPU 只写 2 个列表字；做不了时自动退回 gfx_blit_list_inside，画面完全相同。
 */
void gfx_blit_table(Gfx *gfx,const Sprite *sprites,int nsprites,const GfxItem *it,int n,int key_enabled,uint32_t pixels);
/* Ordered, clipped counterpart of gfx_blit_table. Valid sprite indices are
 * required. Consecutive fully-inside items use slices of the ORIGINAL table;
 * clipped/offscreen items use the ordinary single-item clipping path. The
 * table, sprite descriptors and pixel storage must remain valid and unchanged
 * until backend completion. Never pass a repeatedly filled scratch array.
 * Mode/alpha-plane semantics are identical to gfx_blit_list. */
void gfx_blit_table_clipped(Gfx *gfx,const Sprite *sprites,int nsprites,const GfxItem *it,int n,int mode);
/* 同 gfx_blit_table，整张表半透明（alpha 0..255，255 = gfx_blit_table）；mode 含 GFX_BLIT_ADD 时是加法强度。
 * BitBlt：一条 TAB（槽带全局透明度，分带时走 K 通路）；否则逐项 gfx_blit_alpha / gfx_blit_add（2026-10-26） */
void gfx_blit_table_alpha(Gfx *gfx,const Sprite *sprites,int nsprites,const GfxItem *it,int n,int mode,int alpha,uint32_t pixels);
/* Stable, ordered slices with global alpha; same lifetime contract as TAB. */
void gfx_blit_table_alpha_clipped(Gfx *gfx,const Sprite *sprites,int nsprites,const GfxItem *it,int n,int mode,int alpha);
/*
 * Alpha blending, alpha = 0..255 (global). Destination is read back, so this
 * costs read + write per pixel (twice the bus traffic of fill/blit).
 * gfx_fill_alpha : translucent rectangle (dim overlays, UI panels)
 * gfx_blit_alpha : sprite * global alpha; with key_enabled 0xF81F pixels are
 *                  skipped; if sprite->alpha is set, per-pixel alpha * global.
 */
void gfx_fill_alpha(Gfx *gfx,int x,int y,int w,int h,Pixel color,int alpha);
void gfx_blit_alpha(Gfx *gfx,int x,int y,const Sprite *sprite,int key_enabled,int alpha);
/*
 * Additive blending (2026-10-11), alpha = 0..255: per channel
 *     dst = min(dst + (src * a + 128) / 256, max),  a = alpha + (alpha >> 7)
 * (alpha 255 adds the source fully). Exactly the BitBlt engine's CMD[6] ADD, so
 * CPU and engine give the same pixels. gfx_blit_add: key_enabled skips
 * COLOR_KEY pixels; with sprite->alpha the per-pixel plane scales a too.
 * Counted as "alpha" operations (alphas / alpha_pixels).
 */
void gfx_fill_add(Gfx *gfx,int x,int y,int w,int h,Pixel color,int alpha);
/* 填充表（2026-10-26）：n 个 GfxFillItem，每项一个纯色块（alpha 255 = 不透明，否则半透明；
 * add = 1 时整张表加法混合）。项必须完全在目标内（x+w <= width, y+h <= height），w/h 不超过 255。
 * BitBlt 后端有 FTAB 时一条列表记录交给引擎，表在引擎画完前不能改；否则逐项 gfx_fill_*。
 * 返回 1 = 交给了引擎（统计计数 fills/alphas 不加，调用者自己算），0 = 逐项画的。 */
int  gfx_fill_table(Gfx *gfx,const GfxFillItem *it,int n,int add);
void gfx_blit_add(Gfx *gfx,int x,int y,const Sprite *sprite,int key_enabled,int alpha);
void gfx_border(Gfx *gfx,int x,int y,int w,int h,int thickness,Pixel color);
/*
 * Text, V6 antialiased Noto Sans CJK Medium, Latin advance 6*scale. With GFX_FONT_ATLAS (default 1) each glyph
 * is ONE colour-keyed block copy from a pre-drawn atlas (one atlas per colour
 * and scale, built on first use): BitBlt backend up to GFX_FONT_ATLAS_MAX_SCALE
 * (3), CPU backend up to GFX_FONT_ATLAS_CPU_MAX_SCALE (1). Other scales draw
 * merged runs (one fill per horizontal run of equal rows) instead of pixels.
 * The picture is the same in every case.
 */
void gfx_text(Gfx *gfx,int x,int y,const char *text,int scale,Pixel color);
/* 2026-10-31：on = 1 之后，BitBlt 后端的 gfx_text 用字符串缓存（一串一条拷贝，画面不变）。
 * 只给每帧大部分文字不变的地方开（萝卜 HUD）；东方压力模式的数字每帧都变，不开。 */
void gfx_text_cache(int on);
/* Drop all cached font atlases (waits for the backend first). */
void gfx_font_reset(Gfx *gfx);
int gfx_text_width(const char *text,int scale);
int gfx_text_height(int scale);

/*
 * UI language (2026-10-28): 0 English (default), 1 Chinese.
 * TR("ENGLISH") returns the Chinese text in Chinese mode (dictionary tools/lang/zh.txt -> ui_zh_data.h,
 * pointer cache: one hash probe per call), the literal itself in English mode (no cost). Only for
 * string literals / static strings, never for a buffer (the cache key is the pointer).
 * gfx_text / gfx_text_width take UTF-8: CJK characters are anti-aliased glyphs at the native size of
 * the scale (12 / 16 / 22 ... px, see tools/lang/gen_zh.py), one A8 alpha copy each, vertically centred
 * with matching metrics; Latin and Chinese both use antialiased Noto Medium.
 */
extern int gfx_lang;
void gfx_set_lang(int lang);
const char *gfx_tr(const char *english);
#define TR(s) (gfx_lang ? gfx_tr(s) : (s))
/* 同一个英文在不同地方要不同的中文时（"SPROUT"：东方 = 萌芽，萝卜标题 = 萝卜）：
 * TRX("@ctx ENGLISH", "ENGLISH")，词典里写 "@ctx ENGLISH"；没有这条时用英文 */
const char *gfx_trx(const char *key,const char *english);
#define TRX(k,s) (gfx_lang ? gfx_trx(k,s) : (s))
/* Drop the expanded CJK glyphs / colour planes (waits for the backend first). */
void gfx_cjk_reset(Gfx *gfx);

#endif
