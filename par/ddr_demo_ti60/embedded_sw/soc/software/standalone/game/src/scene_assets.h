#ifndef SCENE_ASSETS_H
#define SCENE_ASSETS_H
#include "gfx.h"
#include <stddef.h>
enum { SCENE_ASSET_W=1280, SCENE_ASSET_H=720 };
/* The fourth image skips FB3 at 0x01c00000 and occupies the free 2 MiB slot. */
#define DEMO_PICTURE_COUNT 4
#define DEMO_PICTURE4_OFFSET 0x00800000u
int scene_lz4_decode(const uint8_t *src,size_t bytes,uint8_t *dst,size_t capacity);
int scene_assets_init(void);
const Sprite *carrot_story_background(void);
int scene_gallery_decode(Pixel *dst,int width,int height);
#endif
