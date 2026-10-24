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
/*
 * Alpha blending, alpha = 0..255 (global). Destination is read back, so this
 * costs read + write per pixel (twice the bus traffic of fill/blit).
 * gfx_fill_alpha : translucent rectangle (dim overlays, UI panels)
 * gfx_blit_alpha : sprite * global alpha; with key_enabled 0xF81F pixels are
 *                  skipped; if sprite->alpha is set, per-pixel alpha * global.
 */
void gfx_fill_alpha(Gfx *gfx,int x,int y,int w,int h,Pixel color,int alpha);
void gfx_blit_part_alpha(Gfx *gfx,int x,int y,const Sprite *sprite,int sx,int sy,int w,int h,int key_enabled,int alpha);
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
void gfx_blit_add(Gfx *gfx,int x,int y,const Sprite *sprite,int key_enabled,int alpha);
void gfx_border(Gfx *gfx,int x,int y,int w,int h,int thickness,Pixel color);
/*
 * Text, 5x7 font, advance 6*scale. With GFX_FONT_ATLAS (default 1) each glyph
 * is ONE colour-keyed block copy from a pre-drawn atlas (one atlas per colour
 * and scale, built on first use): BitBlt backend up to GFX_FONT_ATLAS_MAX_SCALE
 * (3), CPU backend up to GFX_FONT_ATLAS_CPU_MAX_SCALE (1). Other scales draw
 * merged runs (one fill per horizontal run of equal rows) instead of pixels.
 * The picture is the same in every case.
 */
void gfx_text(Gfx *gfx,int x,int y,const char *text,int scale,Pixel color);
/* Cached immutable static text; falls back safely if the label pool is full. */
void gfx_label(Gfx *gfx,int x,int y,const char *text,int scale,Pixel color);
typedef struct {uint32_t hits,misses,resets,label_hits,label_misses,used_pixels;} GfxFontStats;
GfxFontStats gfx_font_stats(void);
int gfx_text_height(int scale);
/* Drop all cached font atlases (waits for the backend first). */
void gfx_font_reset(Gfx *gfx);
int gfx_text_width(const char *text,int scale);

#endif
