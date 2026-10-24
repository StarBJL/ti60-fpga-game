#include "game.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
static Game g;
static uint32_t mix(uint32_t h,uint32_t x){return (h^x)*16777619u;}
int main(void){
 const int sizes[]={1,3,6,9,13,17,25};
 for(int k=0;k<7;k++){
  game_init(&g,417);game_start_stress(&g,417,STRESS_CUSTOM);game_set_seed_size(&g,sizes[k]);memset(g.enemies,0,sizeof g.enemies);g.enemy_count=0;g.spawn_timer=100000;g.invincible=0;g.hp=g.max_hp=100000;
  int n=16000;g.hostile_active=g.hostile_used=n;
  for(int i=0;i<n;i++)g.hostile[i]=(Bullet){1,(16+(i*73)%1249)*FIX,(87+(i*31)%580)*FIX,(i%65-32)*70,(i%47-23)*80,1+i%100,1+i%7,i%4};
  for(int frame=0;frame<64;frame++){
   game_tick(&g,(Input){0});uint32_t h=2166136261u;
   h=mix(h,g.hp);h=mix(h,g.invincible);h=mix(h,g.hostile_active);h=mix(h,g.hostile_used);h=mix(h,g.seed_draw_n);h=mix(h,g.seed_draw_clip);h=mix(h,g.rng);
   for(int i=0;i<n;i++){const Bullet *b=&g.hostile[i];h=mix(h,b->active);h=mix(h,b->x);h=mix(h,b->y);h=mix(h,(int)b->vx);h=mix(h,(int)b->vy);h=mix(h,b->life);h=mix(h,b->damage);h=mix(h,b->style);}
   const GfxItem *items;int count=game_seed_draw(&g,&items);for(int i=0;i<count;i++){h=mix(h,(uint16_t)items[i].x);h=mix(h,(uint16_t)items[i].y);h=mix(h,items[i].sprite);}
   printf("%d %d %08x\n",sizes[k],frame,h);
  }
 }
}
