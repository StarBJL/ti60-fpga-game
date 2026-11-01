#ifndef COOP_FX_H
#define COOP_FX_H
#include "gfx.h"
Pixel coop_fx_color(int player);
const Sprite *coop_fx_tint(const Sprite *src,int player,Gfx *target);
void coop_fx_actor(Gfx *g,int x,int y,int player,unsigned tick,int moving,int skill,int hit,int enabled);
#endif
