#ifndef GFX_HW_H
#define GFX_HW_H
/*
 * BitBlt backend for gfx.c: every gfx_* primitive becomes one blitter command
 * (fill / copy / colour-key copy / alpha fill / alpha copy with A8 plane).
 * gfx.c does the clipping, this file only turns rectangles into addresses.
 *
 * gfx_end() does NOT wait for the engine; call blit_wait() before the frame
 * is flipped to the screen.
 */
#include <stdint.h>
#include "gfx.h"

typedef struct {
    uint32_t fb;        /* byte address of pixel (0,0) of the target */
    uint32_t pitch;     /* bytes per row */
    uint32_t rows;      /* 目标行数（分带合成 blit_band_begin 用） */
    uint32_t band_fb;   /* 分带帧起点（256 字节对齐的行首；= fb，或 fb 所在行的开头） */
} GfxHwTarget;

extern const GfxOps gfx_hw_ops;

/* Draw into the framebuffer at address fb (width x height, packed rows). */
void gfx_begin_hw(Gfx *gfx, GfxHwTarget *target, uint32_t fb, int width, int height);
/* Draw a width x height view inside a wider framebuffer (rows pitch bytes apart);
 * band_fb = start of the view's first framebuffer row (band frame base, 256-byte
 * aligned). 2026-10-21: the 640x480 game centred in the 1280x720 framebuffer. */
void gfx_begin_hw_view(Gfx *gfx, GfxHwTarget *target, uint32_t fb, uint32_t pitch,
                       uint32_t band_fb, int width, int height);

#endif
