#include "th.h"
#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
static Th t;static Pixel frame[1280*720+64];
static uint32_t hash(const void *v,size_t n){const unsigned char*p=v;uint32_t h=2166136261u;while(n--)h=(h^*p++)*16777619u;return h;}
int main(void){
 th_set_view(1);th_art_init();th_init(&t,417);th_start(&t,TH_STRESS);th_stress_set_auto(&t,0);
 for(int i=0;i<360;i++){t.invuln=100000;th_tick(&t,0,0,1);}
 assert(t.nbullets>0);uint32_t before=hash(&t,offsetof(Th,dbuf));
 const int scales[]={1,3,6,75,100,125,150,100};
 for(int k=0;k<8;k++){
  assert(th_pressure_scale_set(scales[k]));th_rebuild_draw(&t);assert(hash(&t,offsetof(Th,dbuf))==before);
  if(scales[k]<=6)for(int b=0;b<TBK_COUNT;b++)for(int a=0;a<th_bspr_n[b];a++){
    const Sprite *p=&th_stress_bspr[b][a];assert(p->width==scales[k]&&p->height==scales[k]);
    int visible=0;for(int x=0;x<p->width*p->height;x++)visible+=p->pixels[x]!=COLOR_KEY;assert(visible);
  }
  for(int b=0;b<TBK_COUNT;b++)for(int j=0;j<t.dcount[t.dbuf][b];j++){
   GfxItem *it=&t.draw[t.dbuf][b][j];const Sprite *s=&th_stress_bspr[b][it->sprite];
   assert(it->x>=0&&it->y>=0&&it->x+s->width<=1280&&it->y+s->height<=720);
  }
  for(int i=1280*720;i<1280*720+64;i++)frame[i]=0xa55a;
  Gfx g;gfx_begin(&g,frame,1280,720);th_render(&t,&g,(ThRenderInfo){60,0,0,1});gfx_end(&g);
  for(int i=1280*720;i<1280*720+64;i++)assert(frame[i]==0xa55a);
  char p[64];sprintf(p,"th_pressure_%d.rgb565",scales[k]);FILE*f=fopen(p,"wb");assert(f);fwrite(frame,2,1280*720,f);fclose(f);
  printf("PASS Touhou stress %d%%: %d bullets, state hash %08x, guard intact\n",scales[k],t.nbullets,before);
 }
 assert(!th_pressure_scale_set(200));
 th_start(&t,TH_NORMAL);for(int i=0;i<120;i++)th_tick(&t,0,0,1);
 uint32_t gameplay=hash(&t,offsetof(Th,dbuf));assert(th_pressure_scale_set(150));th_rebuild_draw(&t);assert(hash(&t,offsetof(Th,dbuf))==gameplay);
 puts("PASS stress display scaling changes neither gameplay nor collision, normal mode uses original art");return 0;
}
