#include "game.h"
#include "th.h"
#include "pad.h"
#include "input_devices.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static Game g;static Th t;
static void quiet_game(void){
 game_init(&g,7);game_start(&g,7);game_coop_enable(&g,3);
 memset(g.enemies,0,sizeof g.enemies);g.enemy_count=0;g.spawn_timer=100000;g.invincible=10000;g.fire_timer=100000;
 for(int i=1;i<3;i++){g.party[i].invincible=10000;g.party[i].fire_timer=100000;}
}
static void game_checks(void){
 quiet_game();int x0=g.x,x1=g.party[1].x,y2=g.party[2].y;Input in[3]={{KEY_LEFT,0},{KEY_RIGHT,0},{KEY_UP,0}};
 game_tick_coop(&g,in);assert(g.x<x0&&g.party[1].x>x1&&g.party[2].y<y2&&g.actor_index==0);
 quiet_game();memset(in,0,sizeof in);in[1].pressed=KEY_SKILL;game_tick_coop(&g,in);
 assert(g.skill_casts==0&&g.party[1].skill_casts==1&&g.party[2].skill_casts==0);
 int shots[3]={0};for(int i=0;i<MAX_BULLETS;i++)if(g.bullets[i].active){assert(g.bullets[i].owner<3);++shots[g.bullets[i].owner];}
 assert(shots[0]==0&&shots[1]==16&&shots[2]==0);
 quiet_game();for(int i=0;i<3;i++)in[i]=(Input){0,KEY_SKILL};game_tick_coop(&g,in);
 memset(shots,0,sizeof shots);for(int i=0;i<MAX_BULLETS;i++)if(g.bullets[i].active)++shots[g.bullets[i].owner];
 assert(shots[0]==16&&shots[1]==16&&shots[2]==16);
 quiet_game();memset(in,0,sizeof in);g.party[1].invincible=0;
 g.hostile[0]=(Seed){g.party[1].x,g.party[1].y,0,0,1000,5,0};g.hostile_active=g.hostile_used=1;
 game_tick_coop(&g,in);assert(g.hp==100&&g.party[1].hp==95&&g.party[2].hp==100&&g.hostile[0].active);
 quiet_game();g.gems[0]=(Gem){1,g.party[2].x,g.party[2].y,2};game_tick_coop(&g,in);
 assert(g.xp==0&&g.party[1].xp==0&&g.party[2].xp==2);
 quiet_game();g.party[1].xp=6;game_tick_coop(&g,in);assert(g.phase==UPGRADE&&g.upgrade_actor==1);
 int before[UPGRADE_COUNT];memcpy(before,g.party[1].upgrades,sizeof before);in[0].pressed=KEY_PICK1;game_tick_coop(&g,in);assert(g.phase==UPGRADE&&!memcmp(before,g.party[1].upgrades,sizeof before));
 in[0].pressed=0;in[1].pressed=KEY_PICK1;game_tick_coop(&g,in);assert(g.phase==PLAYING&&memcmp(before,g.party[1].upgrades,sizeof before));
 quiet_game();memset(in,0,sizeof in);g.hp=g.party[1].hp=0;game_tick_coop(&g,in);assert(g.phase==PLAYING);
 g.party[2].hp=0;game_tick_coop(&g,in);assert(g.phase==DEFEAT);
 game_start_stress(&g,7,STRESS_CUSTOM);assert(g.player_count==1);game_tick_coop(&g,in);assert(!g.occlusion_near_n&&!game_player_count(&g));
 puts("PASS carrot: three independent movement/skill/shot quotas, per-owner hits/items/upgrades, whole-team defeat, pressure remains one player");
}
static void quiet_th(void){
 th_init(&t,7);th_start(&t,TH_NORMAL);th_coop_enable(&t,3);t.boss_seen=1;t.nbullets=t.nitems=t.nfx=0;t.invuln=10000;
 for(int i=1;i<3;i++)t.party[i].invuln=10000;
}
static void th_checks(void){
 th_set_view(1);th_art_init();quiet_th();uint32_t h[3]={PAD_LEFT,PAD_RIGHT,PAD_UP},p[3]={0};int x=t.px,x1=t.party[1].px,y2=t.party[2].py;
 th_tick_coop(&t,h,p,0);assert(t.px<x&&t.party[1].px>x1&&t.party[2].py<y2&&t.actor_index==0);
 quiet_th();h[0]=h[1]=0;h[2]=PAD_SHOT;p[1]=PAD_BOMB;th_tick_coop(&t,h,p,0);
 assert(t.bombs==3&&t.party[1].bombs==2&&t.party[2].bombs==3);int n[3]={0};for(int i=0;i<TH_MAX_SHOTS;i++)if(t.shots[i].active)++n[t.shots[i].owner];assert(!n[0]&&!n[1]&&n[2]>0);
 quiet_th();memset(h,0,sizeof h);memset(p,0,sizeof p);t.party[2].invuln=0;
 assert(th_fire(&t,t.party[2].px,t.party[2].py,0,0,TB_PELLET,TC_RED));th_tick_coop(&t,h,p,0);assert(!t.dying&&!t.party[1].dying&&t.party[2].dying==8);
 for(int i=0;i<8;i++)th_tick_coop(&t,h,p,0);assert(t.lives==3&&t.party[1].lives==3&&t.party[2].lives==2&&t.mode==TH_PLAY);
 quiet_th();for(int i=1;i<3;i++){t.party[i].px=t.px;t.party[i].py=t.py;}
 assert(th_fire(&t,t.px+10*256,t.py,0,0,TB_PELLET,TC_RED));th_tick_coop(&t,h,p,0);assert(t.graze==1&&t.party[1].graze==1&&t.party[2].graze==1);
 th_tick_coop(&t,h,p,0);assert(t.graze==1&&t.party[1].graze==1&&t.party[2].graze==1);
 quiet_th();t.lives=0;t.dying=1;t.invuln=0;th_tick_coop(&t,h,p,0);assert(t.out&&t.mode==TH_PLAY);
 for(int i=1;i<3;i++){t.party[i].lives=0;t.party[i].dying=1;t.party[i].invuln=0;}
 th_tick_coop(&t,h,p,0);assert(t.mode==TH_OVER);
 th_start(&t,TH_STRESS);assert(t.player_count==1);
 puts("PASS Touhou: independent movement/shooting/bombs/lives/graze, shared world and whole-team defeat; pressure remains one player");
}
static void input_cb(void *unused,InputAction action,int value){(void)unused;if(action==INPUT_SOURCE_HELD)pad_set_device((unsigned)value>>16,(unsigned)value&65535);}
static void input_checks(void){
 InputDevices d;input_devices_init(&d,input_cb,0);uint32_t time=1000;
 pad_set_state(PAD_LEFT,time);input_matrix_snapshot(&d,1u<<6);
 unsigned char report[]={0x57,0xab,1,0,0,0x52,0,0,0,0,0};for(unsigned i=0;i<sizeof report;i++)input_ch9350_feed(&d,report[i],0);
 assert(pad_source_held(0,time,1)==PAD_LEFT&&pad_source_held(1,time,1)==PAD_UP&&pad_source_held(2,time,1)==PAD_RIGHT);
 input_matrix_snapshot(&d,0);assert(pad_source_held(1,time,1)==PAD_UP&&pad_source_held(0,time,1)==PAD_LEFT);
 input_keyboard_release(&d);assert(!pad_source_held(1,time,1)&&pad_source_held(0,time,1)==PAD_LEFT);
 input_matrix_snapshot(&d,1u<<1);for(unsigned i=0;i<sizeof report;i++)input_ch9350_feed(&d,report[i],0);
 assert(pad_source_held(1,time,1)==PAD_UP&&pad_source_held(2,time,1)==PAD_UP);
 input_keyboard_release(&d);assert(pad_source_held(2,time,1)==PAD_UP);
 input_matrix_snapshot(&d,0x33);assert(!pad_source_held(2,time,1));assert(!pad_source_held(0,time+301,1));
 puts("PASS independent UART/USB/matrix input ownership, same-key overlap, release, ghost isolation and UART timeout");
}
int main(void){input_checks();game_checks();th_checks();return 0;}
