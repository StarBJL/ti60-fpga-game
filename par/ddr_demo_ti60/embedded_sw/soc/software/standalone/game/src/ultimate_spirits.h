#ifndef ULTIMATE_SPIRITS_H
#define ULTIMATE_SPIRITS_H
#include "gfx.h"
#include "carrot_combat.h"
int ultimate_spirits_init(void);
void ultimate_spirit_draw(Gfx *gfx, const CarrotFighter *fighter, int role, int alpha);
#endif
