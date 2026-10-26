#include "th.h"
#include "pad.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
static Th t;
static uint32_t digest;
static void word(uint32_t v){digest=(digest^v)*16777619u;}
static void state(void){
 word(t.rng);word(t.mode);word(t.stage);word(t.stage_tick);word(t.nbullets);word(t.nphantoms);word(t.nlasers);word(t.nitems);word(t.boss.hp);word(t.boss.phase);
 for(int p=0;p<t.player_count;p++){
  const ThPlayer *a=th_player(&t,p);word(a->px);word(a->py);word(a->score);word(a->lives);word(a->bombs);word(a->graze);word(a->power);word(a->out);
 }
 for(int i=0;i<t.nbullets;i++){
  const ThBullet *b=&t.bullets[i];
#ifdef HOT
  word(t.hot[i].x);word(t.hot[i].y);word(t.hot[i].vx);word(t.hot[i].vy);
  word(b->flags&31u);word(b->flags>>5);
#else
  word(b->x);word(b->y);word(b->vx);word(b->vy);word(b->flags&31u);word(b->pad2);
#endif
  word(b->ang);word(b->timer);word(b->kind);word(b->color);word(b->bk);word(b->sp);
 }
 for(int i=0;i<TH_MAX_SHOTS;i++)if(t.shots[i].active){word(i);word(t.shots[i].x);word(t.shots[i].y);word(t.shots[i].owner);}
 for(int i=0;i<t.nlasers;i++){const ThLaser *l=&t.lasers[i];word(l->kind);word(l->x);word(l->y);word(l->t);word(l->w);}
}
int main(int argc,char **argv){
 const int stage=argc>1?atoi(argv[1]):0,players=argc>2?atoi(argv[2]):1,stress=argc>3?atoi(argv[3]):0;
 th_set_view(1);th_art_init();th_init(&t,0x456789);th_start_stage(&t,stress?TH_STRESS:TH_NORMAL,stage);th_coop_enable(&t,players);digest=2166136261u;
 for(int k=0;k<4800;k++){
  uint32_t held[3]={PAD_SHOT,PAD_SHOT,PAD_SHOT},pressed[3]={0};
  for(int p=0;p<players;p++)held[p]|=((k/90+p)%4==0?PAD_LEFT:(k/90+p)%4==1?PAD_RIGHT:(k/90+p)%4==2?PAD_UP:PAD_DOWN);
  if(k==350||k==1250)pressed[(k/350)%players]=PAD_BOMB;
  if(k%173==0){ThBullet *b=th_fire(&t,50*TH_ONE,70*TH_ONE,40000,650,TB_RICE,TC_RED);if(b){b->timer=30;b->acc=2;b->angv=70;b->flags=TBF_REDIR|TBF_AIM|TBF_BOUNCE;b->aux_spd=730;b->aux_ang=1000;}}
  th_tick_coop(&t,held,pressed,0);state();
  if(k%60==59)printf("%d %08x %d\n",k,digest,t.nbullets);
 }
 return 0;
}
