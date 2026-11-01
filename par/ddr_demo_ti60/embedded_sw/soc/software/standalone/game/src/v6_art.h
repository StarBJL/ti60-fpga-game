#ifndef V6_ART_H
#define V6_ART_H
#include "gfx.h"
int v6_art_init(void);
const Sprite *v6_portrait(int role,int release);
const Sprite *v6_outline(int role,int pose);
const Sprite *v6_ring(int role);
#endif
