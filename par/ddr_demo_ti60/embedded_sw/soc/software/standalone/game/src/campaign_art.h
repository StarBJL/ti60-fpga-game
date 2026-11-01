#ifndef CAMPAIGN_ART_H
#define CAMPAIGN_ART_H
#include "gfx.h"
int campaign_art_init(void);
const Sprite *campaign_background(int scene);
const Sprite *campaign_hero(int player,int level);
const Sprite *campaign_burst_hero(int player);
const Sprite *campaign_crystal(int scene);
#endif
