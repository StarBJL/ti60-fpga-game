#ifndef SPROUT_DEMO_H
#define SPROUT_DEMO_H
/*
 * BitBlt benchmark scenes (赛题二 basic demos), drawn only through gfx_* so they
 * run on both backends:
 *   squares : N randomly moving solid squares (every 2nd one translucent
 *             when alpha effects are on) on a cleared screen
 *   images  : a full-screen 640x480 image copy every frame, switching between
 *             3 pictures (large-image switching)
 */
#include <stdint.h>
#include "gfx.h"
#include "render.h"

#define DEMO_MAX_SQUARES 99999
void demo_geometry(int width,int height);
int demo_motion_toggle(void);
int demo_motion_available(void);
int demo_motion_min_size(void);
int demo_motion_enabled(void);
int demo_motion_error(void);
unsigned demo_motion_cycles(void);
unsigned demo_motion_sequence(void);
unsigned demo_tick_cycles(void);
unsigned demo_table_frames(void);
int demo_table_toggle(void);
int demo_table_enabled(void);
void demo_reset_squares(void);
/* 0 = deterministic mixed 6..40px; fixed sizes use existing MOT2/SQTAB ABI. */
int demo_set_square_size(int pixels);
int demo_square_size(void);
int demo_motion_benchmark(int count,unsigned *cpu,unsigned *hardware);

/* Generates the 3 pictures at img_base, img_base + 2 MB, img_base + 4 MB. */
void demo_init(uint32_t img_base);

void demo_squares_tick(int count);
void demo_squares_render(Gfx *gfx, RenderInfo info, int count);

/* period = frames each picture stays on screen (1 = new picture every frame) */
void demo_images_render(Gfx *gfx, RenderInfo info, uint32_t frame, int period);

#endif
