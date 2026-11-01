#ifndef CARROT_FX_H
#define CARROT_FX_H
#include "game.h"
void carrot_fx_init(void);
void carrot_fx_projectile(Gfx *r,const Game *g,int index,int alpha);
void carrot_fx_under(Gfx *r,const Game *g,int alpha);
void carrot_fx_ui(Gfx *r,const Game *g,int alpha);
void carrot_fx_over(Gfx *r,const Game *g,int alpha);
#endif
