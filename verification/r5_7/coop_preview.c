#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#define BLIT_MODEL_PTR(a) ((uint8_t *)(uintptr_t)(a))
#include "blit_model.h"
#include "blit.h"
#include "gfx_hw.h"
#include "game.h"
#include "render.h"
#include "th.h"
#include "pad.h"
static Game game;static Th th;
static void dump(const char *file){FILE *f=fopen(file,"wb");assert(f);fwrite((void*)0x1000000,2,1280*720,f);fclose(f);}
int main(void){
 assert(mmap((void*)0x1000000,0x200000,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED,-1,0)==(void*)0x1000000);
 blit_model_reset();blit_init();blit_set_mode(BLIT_MODE_LIST);blit_set_band(1);render_init();
 game_init(&game,417);game_start(&game,417);game_coop_enable(&game,3);
 for(int k=0;k<450;k++){
  Input in[3]={{0},{0},{0}};game.invincible=100000;for(int p=1;p<3;p++)game.party[p].invincible=100000;
  if(k<90){in[0].held=KEY_LEFT;in[1].held=KEY_UP;in[2].held=KEY_RIGHT;}
  if(game.phase==UPGRADE)in[game.upgrade_actor].pressed=KEY_PICK2;
  if(k==440)for(int p=0;p<3;p++)in[p].pressed|=KEY_SKILL;
  game_tick_coop(&game,in);
 }
 RenderInfo ri={0};ri.hw=ri.alpha_fx=1;ri.batch=2;Gfx g;GfxHwTarget target;
 gfx_begin_hw(&g,&target,0x1000000,1280,720);render_game(&game,&g,ri);gfx_end(&g);blit_wait();assert(!bm_fault&&!bm_lerr&&!bm_berr);dump("carrot_coop.rgb565");
 th_set_view(1);th_art_init();th_init(&th,417);th_start(&th,TH_NORMAL);th_coop_enable(&th,3);
 for(int k=0;k<500;k++){
  uint32_t h[3]={PAD_SHOT,PAD_SHOT|PAD_SLOW,PAD_SHOT},p[3]={0};
  th.invuln=100000;for(int i=1;i<3;i++)th.party[i].invuln=100000;
  if(k<30){h[0]|=PAD_LEFT;h[2]|=PAD_RIGHT;}if(k==420)p[1]=PAD_BOMB;
  th_tick_coop(&th,h,p,0);
 }
 th.invuln=0;th.party[1].invuln=th.party[2].invuln=0;
 gfx_begin_hw(&g,&target,0x1000000,1280,720);th_render(&th,&g,(ThRenderInfo){.fps=0,.hw=1,.alpha=1});gfx_end(&g);blit_wait();assert(!bm_fault&&!bm_lerr&&!bm_berr);dump("touhou_coop.rgb565");
 puts("PASS 3-player game render models and framebuffer bounds; FPS values are placeholders, not board measurements");
}
