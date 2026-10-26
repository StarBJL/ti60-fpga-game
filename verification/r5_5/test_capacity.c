#include "game.h"
#include "th.h"
#include <assert.h>
#include <stdio.h>
static Game g;static Th t;
int main(void){
 game_init(&g,3);game_start_stress(&g,3,STRESS_CUSTOM);game_set_stress_bullets(&g,1000000);
 assert(game_hostile_slots(&g)==99999);
 for(int i=0;i<180;i++){g.invincible=100000;game_tick(&g,(Input){0,KEY_SKILL});assert(!g.player_active&&!g.muzzle&&!g.weapon_burst&&!g.skill_casts);}
 /* All indices, including the partial final bitmap word, must be reachable. */
 for(int i=0;i<99999;i++)g.hostile[i]=(Seed){640*FIX,360*FIX,0,0,2,0,0};
 g.hostile_active=g.hostile_used=99999;g.invincible=100000;
 game_tick(&g,(Input){0});assert(g.seed_draw_n==99999&&g.occlusion_near_n==0&&!g.occlusion_near_overflow);
 game_set_stress_bullets(&g,65536);assert(g.hostile_active==65536&&game_hostile_slots(&g)==65536);
 game_set_stress_bullets(&g,1);assert(g.hostile_active==1);game_set_stress_bullets(&g,99999);
 game_tick(&g,(Input){0});assert(g.hostile_active<99999);assert(g.hostile_free[GAME_HOSTILE_WORDS-1]>>31==0);
 th_init(&t,7);th_start(&t,TH_STRESS);th_stress_set_auto(&t,0);
 for(int i=0;i<99999;i++)assert(th_fire(&t,100*256,100*256,0,0,TB_PELLET,TC_RED));
 assert(!th_fire(&t,0,0,0,0,TB_PELLET,TC_RED));assert(t.nbullets==99999);
 th_rebuild_draw(&t);int n=0;for(int i=0;i<TBK_COUNT;i++)n+=t.dcount[t.dbuf][i];assert(n==99999);
 puts("PASS 99999 carrot/TH capacity, bitmap tail, >65535 indices, pressure no gun/skill volleys, shrink/regrow");
}
