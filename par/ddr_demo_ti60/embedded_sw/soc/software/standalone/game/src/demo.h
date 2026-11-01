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

/* 225 MHz integration: permit the requested 1px preset (32000 * 1.1).
 * Square objects, all three fill tables and sorting scratch share this bound. */
#define DEMO_MAX_SQUARES 35200

/* 方块场景画面大小（开机按显示分辨率设，要在 demo_init 之前调用）：1280x720 = 原生 720p */
void demo_geometry(int width, int height);
int demo_width(void);
int demo_height(void);
/* 方块大小：0 = 混合 6..40 px（默认），或固定 1 3 6..40；返回 0 = 不支持 */
int demo_set_square_size(int pixels);
int demo_square_size(void);
/* 方块画法：1 = 填充表（tick 写表，BitBlt 有 FTAB 时整张表一条命令，默认），0 = 每个方块一条 fill（旧） */
int demo_square_table(void);
int demo_square_table_toggle(void);
/* 实验（2026-10-28）：把方块按 y 排一次序（按 16 行一带分桶），看分带第 1 段（phase1）是不是被 DDR 换行拖慢。返回带数 */
int demo_squares_sort_y(void);

/* Generates the 3 pictures at img_base, img_base + 2 MB, img_base + 4 MB. */
void demo_init(uint32_t img_base);
void demo_relabel(void);          /* 换界面语言后重画图片上的字（先等引擎空闲） */

void demo_squares_tick(int count);
void demo_squares_render(Gfx *gfx, RenderInfo info, int count);

/* period = frames each picture stays on screen (1 = new picture every frame) */
void demo_images_render(Gfx *gfx, RenderInfo info, uint32_t frame, int period);

#endif
