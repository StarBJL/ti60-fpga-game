#ifndef V5_ART_H
#define V5_ART_H
#include "gfx.h"
int v5_art_init(void);
const Sprite *v5_enemy(int family,int kind,int attack,int enraged);
const Sprite *v5_hero(int role,int pose);
const Sprite *v5_seeds(void);
#endif
