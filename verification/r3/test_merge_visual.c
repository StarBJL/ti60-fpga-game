#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/mman.h>
#define BLIT_MODEL_PTR(a) ((uint8_t *)(uintptr_t)(a))
#include "blit_model.h"
#include "gfx_hw.h"
#include "blit.h"
#include "render.c"
#define FB 0x01000000u
#define NPIX (1280*720)
static Game g,saved;
static Pixel gold[NPIX],cpu[NPIX+64];
static void fixture(int pressure,int level,int n){
    game_start(&g,417);memset(g.enemies,0,sizeof g.enemies);memset(g.particles,0,sizeof g.particles);
    memset(g.bullets,0,sizeof g.bullets);g.enemy_count=g.player_active=0;
    g.banner=g.hurt_flash=g.muzzle=g.invincible=0;g.fire_timer=100000;g.spawn_timer=100000;
    g.level=level;g.limit_mode=pressure;g.stress_tier=pressure?STRESS_CUSTOM:0;
    g.ticks=g.visual_ticks=100;g.seed_draw_ok=0;g.weapon_idle_ticks=0;g.weapon_heading=0;
    for(int k=0;k<n;k++){int cell=(k*97)%400;
        g.hostile[k]=(Seed){(602+cell%20*4)*FIX,(338+cell/20*4)*FIX,0,0,1000,0,k%4};}
    g.hostile_active=g.hostile_used=n;
}
static void actor(unsigned char *a){
    memset(a,0,LOC_W*LOC_H);const Sprite *s=render_hero_asset(game_appearance_level(&g),0);
    for(int y=0;y<s->height;y++)for(int x=0;x<s->width;x++)if(s->pixels[y*s->width+x]!=COLOR_KEY)
        a[(y+LOC_Y-CARROT_HERO_ANCHOR_Y)*LOC_W+x+LOC_X-s->width/2]=1;
    for(int k=0;k<2;k++){
        WeaponPose p=weapon_pose(&g,k,g.weapon_idle_ticks>30);s=render_gun_asset(game_appearance_level(&g),p.heading);
        for(int y=0;y<s->height;y++)for(int x=0;x<s->width;x++)if(s->pixels[y*s->width+x]!=COLOR_KEY){
            int xx=p.x-g.x/FIX-s->width/2+LOC_X+x,yy=p.y-g.y/FIX-s->height/2+LOC_Y+y;
            assert(xx>1&&xx<LOC_W-2&&yy>1&&yy<LOC_H-2);a[yy*LOC_W+xx]=1;
        }
    }
}
static int projectile_hit(int wx,int wy){
    for(int pool=0;pool<2;pool++)for(int k=0;k<(pool?MAX_BULLETS:g.hostile_used);k++){
        const Bullet *b=pool?&g.bullets[k]:&g.hostile[k];if(!b->active)continue;
        const Sprite *s=pool?render_laser_asset(b->style):render_projectile_asset(g.limit_mode,b->style);
        int x=wx-b->x/FIX+s->width/2,y=wy-b->y/FIX+s->height/2;
        if(x>=0&&x<s->width&&y>=0&&y<s->height&&s->pixels[y*s->width+x]!=COLOR_KEY)return 1;
    }return 0;
}
static void reference(void){
    unsigned char a[LOC_W*LOC_H];actor(a);int total=0,cover=0,bob=g.moving?(g.ticks/7)%2:0;
    for(int y=0;y<LOC_H;y++)for(int x=0;x<LOC_W;x++)if(a[y*LOC_W+x]){
        total++;cover+=projectile_hit(g.x/FIX-LOC_X+x,g.y/FIX-LOC_Y-bob+y);
    }
    prepare_player_locator(&g);RenderLocatorInfo info=render_locator_info();
    assert(info.total==total&&info.covered==cover);
}
static void frame(int mode,int alpha){
    Gfx gfx;GfxHwTarget hw;RenderInfo info={0};info.hw=1;info.alpha_fx=alpha;info.batch=mode;
    blit_set_mode(mode==2?BLIT_MODE_LIST:mode==3?BLIT_MODE_WORD:BLIT_MODE_APB);
    gfx_begin_hw(&gfx,&hw,FB,1280,720);render_game(&g,&gfx,info);gfx_end(&gfx);blit_wait();
}
static void dump(const char *name){FILE *f=fopen(name,"wb");assert(f);assert(fwrite((void *)(uintptr_t)FB,2,NPIX,f)==NPIX);fclose(f);}
static void compare_modes(void){
    frame(0,1);memcpy(gold,(void *)(uintptr_t)FB,sizeof gold);
    for(int mode=1;mode<4;mode++){frame(mode,1);assert(!memcmp(gold,(void *)(uintptr_t)FB,sizeof gold));}
    Gfx gfx;RenderInfo info={0};info.alpha_fx=1;info.batch=2;
    for(int k=NPIX;k<NPIX+64;k++)cpu[k]=0x5aa5;
    gfx_begin(&gfx,cpu,1280,720);render_game(&g,&gfx,info);gfx_end(&gfx);
    for(int k=NPIX;k<NPIX+64;k++)assert(cpu[k]==0x5aa5);
    /* HW uses 8-bit alpha and CPU 5-bit; compare the game viewport, avoiding
       the intentionally different backend label in the bottom HUD. */
    int maxr=0,maxg=0,maxb=0;
    for(int y=90;y<670;y++)for(int x=0;x<1280;x++){
        int k=y*1280+x,a=cpu[k],b=gold[k],d;
        d=abs((a>>11)-(b>>11));if(d>maxr)maxr=d;
        d=abs(((a>>5)&63)-((b>>5)&63));if(d>maxg)maxg=d;
        d=abs((a&31)-(b&31));if(d>maxb)maxb=d;
    }
    assert(maxr<=2&&maxg<=3&&maxb<=2);
}
int main(void){
    assert(mmap((void*)(uintptr_t)FB,0x00200000,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED,-1,0)==(void*)(uintptr_t)FB);
    blit_model_reset();assert(blit_init());render_init();assert(native_used<48000);
    for(int pressure=0;pressure<2;pressure++)for(int level=1;level<=7;level++)for(int h=0;h<32;h++)for(int bob=0;bob<2;bob++){
        fixture(pressure,level,38+(h%9));g.weapon_heading=h;g.moving=bob;g.ticks=105;reference();
        unsigned char a[LOC_W*LOC_H];actor(a);const Sprite *edge=render_locator_outline_asset();
        for(int k=0;k<LOC_W*LOC_H;k++)if(a[k])assert(edge->pixels[k]==COLOR_KEY);
    }
    for(int level=1;level<=7;level++){fixture(0,level,0);g.weapon_idle_ticks=1000;reference();}
    puts("PASS independent opaque-pixel reference: 896 level/heading/bob/mode combinations, idle poses and hollow union outlines");
    fixture(0,7,40);g.invincible=10000;game_tick(&g,(Input){0});reference();assert(render_locator_info().used_near_cache);
    int cov=render_locator_info().covered;g.occlusion_near_overflow=1;reference();assert(render_locator_info().covered==cov);
    g.seed_draw_ok=0;reference();assert(render_locator_info().covered==cov);
    fixture(1,7,400);g.invincible=10000;game_tick(&g,(Input){0});assert(g.occlusion_near_overflow);reference();
    fixture(1,7,256);for(int k=0;k<256;k++)g.hostile[k]=g.hostile[0];reference();cov=render_locator_info().covered;
    g.hostile_used=1;reference();assert(render_locator_info().covered==cov);
    fixture(0,7,0);for(int k=0;k<MAX_BULLETS;k++)g.bullets[k]=(Bullet){1,(612+k%10*5)*FIX,(345+k/10*4)*FIX,0,0,100,0,k%8};reference();
    puts("PASS near-index cache, overflow/table/pool fallbacks, duplicate union and player-laser coverage");
    fixture(0,7,400);int all=g.hostile_used,mid=0,low=0;
    for(int n=1;n<=all;n++){g.hostile_used=n;prepare_player_locator(&g);int p=render_locator_info().percent;if(!low&&p>=55)low=n;if(!mid&&p>=70)mid=n;}
    assert(mid&&low);render_locator_reset();g.hostile_used=mid;prepare_player_locator(&g);assert(!loc_info.active&&!loc_info.fade);
    g.hostile_used=all;g.visual_ticks++;prepare_player_locator(&g);assert(loc_info.fade==64&&loc_info.active);
    prepare_player_locator(&g);assert(loc_info.fade==64);compare_modes();dump("locator_enter.rgb565");
    for(int i=0;i<3;i++){g.visual_ticks++;prepare_player_locator(&g);}assert(loc_info.fade==255&&!loc_info.colored);
    g.hostile_used=mid;g.visual_ticks++;prepare_player_locator(&g);assert(loc_info.active&&loc_info.fade==255);
    {Gfx gfx;memset(cpu,0,sizeof cpu);gfx_begin(&gfx,cpu,1280,720);draw_player(&gfx,&g);for(int k=0;k<NPIX;k++)assert(!cpu[k]);}
    compare_modes();dump("locator_active.rgb565");
    g.hostile_used=low;g.visual_ticks++;prepare_player_locator(&g);assert(!loc_info.active&&loc_info.fade==191);compare_modes();dump("locator_exit.rgb565");
    g.hostile_used=0;g.visual_ticks++;prepare_player_locator(&g);assert(!loc_info.fade&&!loc_info.active);
    compare_modes();dump("hero_level7.rgb565");
    for(int i=1;i<=7;i++){fixture(0,i,0);frame(2,1);char p[80];sprintf(p,"hero_level%d.rgb565",i);dump(p);}
    fixture(0,7,all);for(int i=0;i<4;i++){g.visual_ticks++;prepare_player_locator(&g);}
    const Pixel *pointers[3]={0};Pixel outlines[3][LOC_W*LOC_H];
    for(int h=0;h<32;h++){
        g.weapon_heading=h;g.visual_ticks++;memcpy(&saved,&g,sizeof g);frame(2,1);assert(!memcmp(&saved,&g,sizeof g));
        if(h<3){pointers[h]=render_locator_outline_asset()->pixels;memcpy(outlines[h],pointers[h],sizeof outlines[h]);
            for(int j=0;j<h;j++)assert(pointers[h]!=pointers[j]&&!memcmp(outlines[j],pointers[j],sizeof outlines[j]));}
    }
    puts("PASS 80/65 hysteresis, four-step fade, replay stability, hidden body at full fade, 3 distinct outline buffers and gameplay immutability");
    fixture(1,7,0);g.hostile_used=g.hostile_active=40000;
    for(int i=0;i<40000;i++)g.hostile[i]=(Seed){(60+(i*137)%1160)*FIX,(140+(i*53)%480)*FIX,0,0,1000,0,i%4};
    g.invincible=10000;game_tick(&g,(Input){0});compare_modes();dump("stress40k.rgb565");
    fixture(0,1,0);g.phase=TITLE;frame(2,1);dump("title.rgb565");g.phase=STRESS_SELECT;frame(2,1);dump("stress_menu.rgb565");
    g.phase=UPGRADE;g.level=7;g.choices[0]=NOVA;g.choices[1]=MULTISHOT;g.choices[2]=POWER;frame(2,1);dump("upgrade.rgb565");
    g.phase=PAUSED;frame(2,1);dump("pause.rgb565");
    puts("PASS native 720p render and framebuffer guards; APB/list/DDR-table/LIST_WORD exact frame equality, CPU/HW alpha tolerance, 40K full frame, title/menus/upgrade/pause exports");return 0;
}
