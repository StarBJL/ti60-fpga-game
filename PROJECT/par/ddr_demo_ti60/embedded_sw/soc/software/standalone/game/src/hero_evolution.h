/* R3: R19 approved RGB565 artwork, cached once at 1.5x for native 720p.
 * Source art is unchanged; scaled body/gun buffers remain immutable after init.
 * Pose requests share the approved picture; walking retains the existing bob. */
#include "hero_art_data.h"
static Pixel hero_native[7][CARROT_HERO_W*CARROT_HERO_H];
static Pixel gun_native[96][CARROT_GUN_SIZE*CARROT_GUN_SIZE];
static Sprite hero_sprites[7];
static Sprite gun_sprites[96];
static void init_hero_evolution(void){
    int k,x,y;
    for(k=0;k<7;k++){
        hero_sprites[k]=(Sprite){CARROT_HERO_W,CARROT_HERO_H,hero_native[k],0};
        for(y=0;y<CARROT_HERO_H;y++)for(x=0;x<CARROT_HERO_W;x++)
            hero_native[k][y*CARROT_HERO_W+x]=hero_art_pixels[k][y*HERO_H/CARROT_HERO_H*HERO_W+x*HERO_W/CARROT_HERO_W];
    }
    for(k=0;k<96;k++){
        gun_sprites[k]=(Sprite){CARROT_GUN_SIZE,CARROT_GUN_SIZE,gun_native[k],0};
        for(y=0;y<CARROT_GUN_SIZE;y++)for(x=0;x<CARROT_GUN_SIZE;x++)
            gun_native[k][y*CARROT_GUN_SIZE+x]=gun_art_pixels[k][y*17/CARROT_GUN_SIZE*17+x*17/CARROT_GUN_SIZE];
    }
    /* Reuse the title/material compatibility plane; square aspect is preserved. */
    for(y=0;y<32;y++)for(x=0;x<24;x++)
        player_pixels[y*24+x]=y<4||y>=28?COLOR_KEY:
            hero_art_pixels[0][((y-4)*HERO_H/24)*HERO_W+x*HERO_W/24];
}
const Sprite *render_hero_asset(int level,int pose){
    (void)pose;if(level<1)level=1;if(level>7)level=7;
    return &hero_sprites[level-1];
}
const Sprite *render_gun_asset(int level,int heading){
    int palette=level<4?0:level<6?1:2;
    return &gun_sprites[palette*32+(heading&31)];
}
