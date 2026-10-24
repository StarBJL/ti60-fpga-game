#include "game.h"
#include "weapon_pose.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static Game g;
static void empty(void){
    game_start(&g,417);memset(g.enemies,0,sizeof g.enemies);g.enemy_count=0;
    g.spawn_timer=100000;g.fire_timer=100000;g.invincible=100000;
}
static int active_players(void){int n=0;for(int i=0;i<MAX_BULLETS;i++)n+=g.bullets[i].active!=0;return n;}
static void guns(void){
    for(int h=0;h<32;h++)for(int shots=1;shots<=6;shots++){
        empty();g.fire_timer=0;g.shots=shots;
        Enemy *e=&g.enemies[0];e->active=1;e->kind=4;e->id=1;e->attack_timer=100000;e->hp=e->max_hp=100000;
        e->x=g.x+weapon_unit[h][0]*200;e->y=g.y+weapon_unit[h][1]*200;g.enemy_count=1;
        game_tick(&g,(Input){0});assert(game_player_count(&g)==shots&&g.weapon_burst==1);
        assert(g.weapon_flash_mask==(shots==1?1:3));assert(g.sfx&GS_SHOT);
        for(int k=0;k<shots;k++){
            WeaponPose p=weapon_pose(&g,shots==1?0:k&1,0);Bullet *b=&g.bullets[k];
            assert(b->active&&b->x-b->vx==p.mx*FIX&&b->y-b->vy==p.my*FIX);
            assert(b->damage==2&&b->life==59&&(unsigned)b->style<8);
        }
        if(shots==1){g.fire_timer=0;game_tick(&g,(Input){0});WeaponPose p=weapon_pose(&g,1,0);
            assert(g.weapon_flash_mask==2&&g.bullets[1].x-g.bullets[1].vx==p.mx*FIX&&g.bullets[1].y-g.bullets[1].vy==p.my*FIX);}
    }
}
static void skills(void){
    for(int level=1;level<=7;level++)for(int rank=0;rank<8;rank++){
        empty();g.level=level;g.upgrades[NOVA]=rank;int n=level>=6?32:16;
        game_tick(&g,(Input){0,KEY_SKILL});assert(g.skill_casts==1&&active_players()==n&&game_player_count(&g)==n);
        assert(g.skill_cooldown==(480-rank*45<240?240:480-rank*45));assert(g.sfx&GS_SKILL);
        for(int i=0;i<n;i++)assert(g.bullets[i].damage==2*(4+rank)+level&&g.bullets[i].life==54);
        game_tick(&g,(Input){0,KEY_SKILL});assert(g.skill_casts==1);
    }
    empty();for(int i=0;i<MAX_BULLETS-15;i++)g.bullets[i]=(Bullet){1,g.x,g.y,0,0,1000,1,0};
    game_tick(&g,(Input){0,KEY_SKILL});assert(!g.skill_casts&&!g.skill_cooldown);
    empty();g.phase=PAUSED;game_tick(&g,(Input){0,KEY_SKILL});assert(!g.skill_casts);
    empty();
    for(int level=2;level<=10;level++){
        g.xp=g.next_xp;game_tick(&g,(Input){0});assert(g.level==level&&g.phase==UPGRADE&&(g.sfx&GS_LEVEL));
        assert(game_appearance_level(&g)==(level>7?7:level));
        assert(g.shots==(level>=7?4:level>=5?3:level>=3?2:1));
        g.choices[0]=POWER;game_choose_upgrade(&g,0);assert(g.phase==PLAYING);
    }
    empty();for(int i=0;i<8;i++){g.phase=UPGRADE;g.choices[0]=MULTISHOT;game_choose_upgrade(&g,0);}assert(g.shots==6);
    assert(UPGRADE_COUNT==7&&strlen(game_upgrade_line(NOVA,2))>0);
}
static void tiers_and_geometry(void){
    const int cap[]={0,1024,2048,4096,40000};
    for(int tier=1;tier<=4;tier++){
        game_start_stress(&g,417,tier);assert(game_hostile_slots(&g)==cap[tier]);
        assert(game_appearance_level(&g)==(tier>=3?7:1)&&g.shots==(tier>=3?4:1));
        int seen[3]={0};
        for(int tick=0;tick<600;tick++){
            g.invincible=10000;game_tick(&g,(Input){(tick/150)&1?KEY_LEFT|KEY_UP:KEY_RIGHT|KEY_DOWN,0});
            assert(g.x>=GAME_PLAYER_MIN_X*FIX&&g.x<=GAME_PLAYER_MAX_X*FIX&&g.y>=GAME_PLAYER_MIN_Y*FIX&&g.y<=GAME_PLAYER_MAX_Y*FIX);
            assert(g.hostile_active<=cap[tier]);assert(game_total_bullets(&g)==g.hostile_active+active_players());seen[g.seed_draw_buf]=1;
            const GfxItem *items;int n=game_seed_draw(&g,&items);assert(n>=0&&n<=cap[tier]);
            for(int j=0;j<n;j++)assert(items[j].x>-13&&items[j].x<1280&&items[j].y>-13&&items[j].y<720);
            for(int j=0;j<MAX_ENEMIES;j++)if(g.enemies[j].active&&!g.enemies[j].spawn)assert(g.enemies[j].x>=21*FIX&&g.enemies[j].x<=1260*FIX);
        }
        assert(seen[0]&&seen[1]&&seen[2]);
    }
    game_set_stress_bullets(&g,14000);assert(game_hostile_slots(&g)==14000);
    game_set_stress_bullets(&g,1);assert(game_hostile_slots(&g)==1000&&g.hostile_active<=1000);
    game_set_stress_bullets(&g,99999);assert(game_hostile_slots(&g)==40000);
    /* The two previous DMA tables cannot be overwritten by one more tick. */
    const GfxItem *old;game_seed_draw(&g,&old);size_t bytes=(size_t)g.seed_draw_n*sizeof *old;
    GfxItem *copy=malloc(bytes?bytes:1);memcpy(copy,old,bytes);
    game_tick(&g,(Input){0});assert(memcmp(copy,old,bytes)==0);
    game_tick(&g,(Input){0});assert(memcmp(copy,old,bytes)==0);free(copy);
    game_start(&g,417);assert(game_seed_half(&g)==12);assert(game_hostile_slots(&g)==1024);
    /* Native formation still occupies a visible ring/fan at all four corners. */
    for(int c=0;c<4;c++){
        game_start_stress(&g,417,STRESS_CUSTOM);g.x=(c&1?GAME_PLAYER_MAX_X:GAME_PLAYER_MIN_X)*FIX;g.y=(c&2?GAME_PLAYER_MAX_Y:GAME_PLAYER_MIN_Y)*FIX;
        for(int t=0;t<300;t++){g.invincible=10000;game_tick(&g,(Input){0});}
        assert(g.formation_span>1&&g.formation_span<128&&g.formation_count>0);
    }
}
static void collisions_and_events(void){
    empty();g.hostile[0]=(Bullet){1,g.x,g.y,0,0,1000,3,2};g.hostile_active=g.hostile_used=1;
    for(int i=0;i<12;i++)game_tick(&g,(Input){0});assert(g.hostile[0].active&&g.hp==100);
    g.invincible=0;game_tick(&g,(Input){0});assert(g.hp==97&&g.hostile[0].active&&(g.sfx&GS_HURT));
    empty();Enemy *e=&g.enemies[0];*e=(Enemy){0};e->active=1;e->x=g.x+100*FIX;e->y=g.y;e->hp=e->max_hp=1;e->attack_timer=1000;g.enemy_count=1;
    g.bullets[0]=(Bullet){1,e->x,e->y,0,0,100,2,0};game_tick(&g,(Input){0});assert((g.sfx&(GS_HIT|GS_KILL))==(GS_HIT|GS_KILL)&&g.kills==1);
}
int main(void){guns();skills();tiers_and_geometry();collisions_and_events();puts("PASS R3 gameplay: 32 headings x 1..6 shots; muzzle alignment; 16/32-shot skill and cooldown; seven upgrades and appearances; four tiers and 1K..40K; native corners; triple DMA tables; invulnerability preserves bullets; audio events");return 0;}
