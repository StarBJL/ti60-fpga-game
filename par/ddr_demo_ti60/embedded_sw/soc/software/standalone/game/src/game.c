#include "game.h"
#include "voice_timing.h"
#include "boss_voice_timing.h"
#include <stddef.h>
#include "weapon_pose.h"     /* 双枪位置：子弹从枪口射出（2026-10-26，队友版） */
#include <string.h>

/* Stress mode: 1 = enemy seeds that leave the arena re-enter from the opposite
 * edge (original SproutRun behaviour), 0 = they disappear at the edge like in
 * normal mode (board default, see makefile STRESS_WRAP). */
#ifndef STRESS_WRAP
#define STRESS_WRAP 0
#endif
/* Normal mode keeps the original 1024-slot enemy-seed pool so its frame cost is
 * unchanged; only stress mode uses the larger MAX_HOSTILE_BULLETS pool. */
#define NORMAL_HOSTILE_SLOTS (MAX_HOSTILE_BULLETS<1024 ? MAX_HOSTILE_BULLETS : 1024)

static int clamp(int n, int lo, int hi) { return n<lo ? lo : n>hi ? hi : n; }
/* Story motion only; round to the nearest fixed-point unit. */
static int story_speed(int speed) { return (speed*11+5)/10; }
/* Campaign balance only; stress tiers retain their original populations. */
#define CAMPAIGN_SWARM_MULTIPLIER 6
static int normal_shot_limit(int role){static const int n[3]={6,4,5};return n[clamp(role,0,2)];}
static int normal_attack_floor(int role){static const int n[3]={7,12,10};return n[clamp(role,0,2)];}
static int normal_skill_radius(const GamePlayer *p,int role){
    int rank=clamp(p->upgrades[ULT_AREA],0,4);
    return role==0?185+rank*12:role==1?120+rank*10:132+rank*12;
}
typedef struct {
    int enemies, shots, batch, spawn_ticks, volley, attack_ticks, shot_life;
} StressSettings;
/*
 * Board tuning (no wrap-around): each tier roughly doubles the seeds on screen.
 * Measured with the host model, player moving: ~470 / ~1100 / ~2700 seeds.
 * Original values: {24,256,2,30,1,60,360} {64,768,4,18,3,36,540} {128,2048,6,10,5,20,720}
 *   enemies, shots(pool cap), batch, spawn_ticks, volley, attack_ticks, shot_life
 */
static const StressSettings stress_settings[] = {
    {0,0,0,0,0,0,0},
    {32,1024,2,30,3,45,720},
    {64,2048,4,18,5,60,720},
    {128,4096,6,10,5,50,720},
    /* CUSTOM（2026-10-25，参照队友版）：128 个喷吐怪，每 10 tick 一轮扇形，
     * 每轮颗数随上限变（custom_target/1280），池子上限 = custom_target（串口设定） */
    {128,MAX_HOSTILE_BULLETS,6,10,5,10,720}
};
static const StressSettings *stress(const Game *g) {
    return g->stress_tier>=STRESS_LOW && g->stress_tier<=STRESS_CUSTOM
        ? &stress_settings[g->stress_tier] : 0;
}
/* 自定义压力的弹幕上限 / 压力弹幕的图大小：放在这里（不在 Game 里），重开一局也保留 */
static int custom_target=8192;
static void campaign_enter(Game *g,int scene);
static int campaign_can_level(const Game *g,int level);
static int pressure_seed_size=13;   /* 2026-10-26：队友版 1.5 倍美术的默认压力弹大小（9 x 1.5） */

/* 场地（原生 1280x720，game.h）。下面的数都是原来 640x480 里相对围栏的距离，没改。 */
#define AX0 SPROUT_ARENA_X0
#define AY0 SPROUT_ARENA_Y0
#define AX1 SPROUT_ARENA_X1
#define AY1 SPROUT_ARENA_Y1
/* ---- 多人（2026-10-26，见 game.h）：actor = 当前玩家，party[] = 其他玩家 */
const GamePlayer *game_player(const Game *g, int player) {
    return player==g->actor_index ? &g->actor : &g->party[player];
}
static void player_context(Game *g, int player) {
    if (player==g->actor_index) return;
    g->party[g->actor_index]=g->actor; g->actor=g->party[player]; g->actor_index=player;
}
int game_players_alive(const Game *g) {
    int i,n=0;
    for (i=0;i<g->player_count;i++) n+=game_player(g,i)->hp>0;
    return n;
}
/* 离 (x,y) 最近的活着的玩家（都倒下了返回 0） */
static int nearest_player(const Game *g, int x, int y) {
    int i,best=0; uint32_t distance=~0u;
    for (i=0;i<g->player_count;i++) {
        const GamePlayer *p=game_player(g,i); int dx,dy; uint32_t d;
        if (p->hp<=0) continue;
        dx=(p->x-x)/FIX; dy=(p->y-y)/FIX; d=(uint32_t)(dx*dx+dy*dy);
        if (d<distance) { distance=d; best=i; }
    }
    return best;
}
int game_combat_enabled(const Game *g) {
    return !g->limit_mode && g->phase!=STRESS_SELECT && g->player_count>=1 && g->player_count<=GAME_PLAYERS;
}
int game_combat_period(const GamePlayer *p) {
    return p->upgrades[POWER]>=2 ? 1 : p->upgrades[POWER] ? 2 : 3;
}
/* No RNG calls in visual state; gameplay RNG remains the existing game stream. */
static void combat_visual_tick(Game *g) {
    int i,dt=clamp(g->campaign.ticks-g->combat.clock_tick,1,3600);
    g->combat.clock_tick=g->campaign.ticks;
    for(i=0;i<CARROT_BLAST_SLOTS;i++) if(g->combat.blast[i].ticks)g->combat.blast[i].ticks=clamp(g->combat.blast[i].ticks-dt,0,CARROT_BLAST_TICKS);
    for(i=0;i<GAME_PLAYERS;i++) {
        CarrotFighter *f=&g->combat.player[i];
        if(f->dash_cooldown)f->dash_cooldown=clamp(f->dash_cooldown-dt,0,CARROT_DASH_COOLDOWN);
        if(f->skill_cooldown)f->skill_cooldown=clamp(f->skill_cooldown-dt,0,CARROT_SKILL_COOLDOWN);
        if(f->skill_total) {
            f->skill_age=clamp(g->campaign.ticks-f->skill_start,0,f->skill_total);
            f->skill_ticks=f->skill_total-f->skill_age;
        }
        const GamePlayer *a=game_player(g,i);
        f->reflect_window=clamp(f->reflect_window-dt,0,12);
        f->reflect_cooldown=clamp(f->reflect_cooldown-dt,0,180);
        f->reflect_flash=clamp(f->reflect_flash-dt,0,24);
        f->shield_cooldown=clamp(f->shield_cooldown-dt,0,480);
        f->shield_flash=clamp(f->shield_flash-dt,0,30);
        if(a->hp>0)f->shield_quiet=clamp(f->shield_quiet+dt,0,600);
        if(i==2 && a->hp>0 && !f->shield_charges && !f->shield_cooldown && f->shield_quiet>=240){
            f->shield_charges=2+clamp(a->upgrades[PASSIVE]/2,0,2);f->shield_flash=24;g->sfx|=GS_SHIELD;
        }
        if(f->trail_ticks)f->trail_ticks=clamp(f->trail_ticks-dt,0,18);
        else f->trail_count=0;
    }
}
static void combat_blast(Game *g,int x,int y,int owner) {
    unsigned slot;
    for(slot=0;slot<CARROT_BLAST_SLOTS;slot++) if(!g->combat.blast[slot].ticks) break;
    if(slot==CARROT_BLAST_SLOTS) { slot=g->combat.next_blast; ++g->combat.blast_replacements; }
    g->combat.next_blast=(slot+1)%CARROT_BLAST_SLOTS;
    g->combat.blast[slot]=(CarrotBlast){x/FIX,y/FIX,owner,CARROT_BLAST_TICKS};
    ++g->combat.blasts_spawned;
}
static uint32_t random_u32(Game *g) {
    uint32_t x=g->rng; x^=x<<13; x^=x>>17; x^=x<<5; return g->rng=x;
}
static int rand_n(Game *g, int n) { return (int)(random_u32(g)%(uint32_t)n); }

int game_isqrt(uint32_t n) {
    uint32_t root=0, bit=1u<<30;
    while (bit>n) bit>>=2;
    while (bit) {
        if (n>=root+bit) { n-=root+bit; root=(root>>1)+bit; }
        else root>>=1;
        bit>>=2;
    }
    return (int)root;
}

/* dx/dy are integer pixels, result is Q8 units per tick. */
static void direction(int dx, int dy, int speed, int *vx, int *vy) {
    int distance=game_isqrt((uint32_t)(dx*dx+dy*dy));
    if (distance<1) { *vx=0; *vy=0; return; }
    *vx=dx*speed/distance; *vy=dy*speed/distance;
}

/* 2026-10-26：怪物美术放大 1.5 倍（队友版），碰撞半径跟着 x1.5 */
int game_enemy_radius(int kind) { return (kind==3 ? 23 : kind==2 ? 15 : kind==4 ? 9 : kind==1 ? 8 : 10)*3/2; }

static void particles(Game *g, int x, int y, int color, int amount) {
    int i;
    for (i=0; i<MAX_PARTICLES && amount>0; ++i) {
        Particle *p=&g->particles[i];
        if (p->life) continue;
        p->x=x; p->y=y; p->vx=rand_n(g,700)-350; p->vy=rand_n(g,700)-400;
        p->total=p->life=12+rand_n(g,16); p->color=color; p->size=2+rand_n(g,2);
        --amount;
    }
}

static void drop_gem(Game *g, int x, int y, int value) {
    int i, nearest=0, best=0x7fffffff;
    for (i=0;i<MAX_GEMS;i++) {
        if (!g->gems[i].active) {
            Gem *gem=&g->gems[i];
            gem->active=1; gem->x=x; gem->y=y; gem->value=value; return;
        }
        {
            int dx=(g->gems[i].x-x)/FIX, dy=(g->gems[i].y-y)/FIX;
            int d=dx*dx+dy*dy;
            if (d<best) { best=d; nearest=i; }
        }
    }
    /* Merge instead of silently dropping XP when the fixed pool is full. */
    g->gems[nearest].value+=value;
}

static int spawn_enemy(Game *g, int kind) {
    int i, side, x, y, minutes=g->ticks/3600;
    for (i=0;i<MAX_ENEMIES;i++) if (!g->enemies[i].active) break;
    if (i==MAX_ENEMIES) return 0;
    side=rand_n(g,4);
    /* 640x480 时：x 30..609 y 80..423，四边 x=20/620 y=68/432 */
    x=AX0+16+rand_n(g,AX1-AX0-32); y=AY0+20+rand_n(g,AY1-AY0-39);
    if (side==0) x=AX0+6; else if (side==1) x=AX1-6;
    else if (side==2) y=AY0+8; else y=AY1-11;
    /* The warning ring also protects against unavoidable edge spawns. */
    {
        Enemy *e=&g->enemies[i];
        memset(e,0,sizeof(*e)); e->active=1; e->x=x*FIX; e->y=y*FIX;
        e->kind=kind; e->spawn=kind==3 ? 90 : g->limit_mode ? 8 : 24; e->id=++g->next_enemy_id;
        e->max_hp=e->hp=g->limit_mode && kind==4 ? 100000 : kind==3 ? 160+minutes*60 : (kind==4 ? 6 : kind==2 ? 11 : kind==1 ? 3 : 4)+minutes*2;
        e->attack_timer=kind==4 ? (g->limit_mode ? stress(g)->attack_ticks : 36) : 105;
        if(!g->limit_mode) {
            int scene=g->campaign.scene;
            int wave=g->campaign.endless_wave;
            e->family=scene==5?rand_n(g,6):scene;
            static const int boss_hp[6]={750,1150,1650,2150,2650,3300};
            e->max_hp=e->hp=kind==3?boss_hp[scene]*(100+85*(g->player_count>0?g->player_count-1:0))/100:(kind==2?18:kind==4?7:kind==1?4:5)+scene*2;
            if(scene==5) {
                int scale=100+clamp(wave,0,100000)*12;
                int64_t health=(int64_t)e->hp*scale/100;
                e->hp=health>10000000?10000000:(int)health;e->max_hp=e->hp;
            }
            e->attack_timer=70+rand_n(g,35);
            e->spawn_until=g->campaign.ticks+(kind==3?boss_voice_ticks[e->family]+12:24);
            if(kind==3){g->combat.boss_id=e->id;g->combat.boss_family=e->family;g->combat.boss_intro_start=g->campaign.ticks;g->combat.boss_intro_ticks=boss_voice_ticks[e->family]+12;g->sfx|=GS_BOSSVOICE;for(int p=0;p<g->player_count;p++){GamePlayer*a=p==g->actor_index?&g->actor:&g->party[p];if(a->hp>0&&a->invincible<2)a->invincible=2;}}
        }
        ++g->enemy_count;
    }
    return 1;
}

/* Re-evaluate while playing: both survival time and level raise the cost. */
static int required_xp_at(const Game *g, int lv) {
    int level=lv-1;
    if(!g->limit_mode) return 6+level*3;
    return 6+4*level+level*level/2+2*(g->ticks/1800);
}
static int required_xp(const Game *g) { return required_xp_at(g,g->level); }

static void spawn_director(Game *g) {
    int seconds=g->ticks/60, i, limit;
    /* Each stress tier has a fixed population and spawn rate. */
    if (g->limit_mode) {
        const StressSettings *s=stress(g);
        int cap=s->enemies<MAX_ENEMIES ? s->enemies : MAX_ENEMIES;
        if (--g->spawn_timer>0) return;
        for (i=0;i<s->batch && g->enemy_count<cap;i++) spawn_enemy(g,4);
        g->spawn_timer=s->spawn_ticks;
        return;
    }
    (void)seconds;
    if(g->campaign.scene==5) {
        int wave=g->campaign.ticks/GAME_ENDLESS_WAVE_TICKS;
        if(wave>g->campaign.endless_wave) {
            /* Scalar growth continues; entity / projectile pool costs stay bounded. */
            int steps=wave-g->campaign.endless_wave;
            for(int p=0;p<g->player_count;p++) {
                player_context(g,p);
                g->damage=clamp(g->damage+steps*(p==1?5:p==2?4:3),1,1000000);
                g->max_hp=clamp(g->max_hp+steps*6,1,1000000);
                if(g->hp>0)g->hp=clamp(g->hp+steps*18,1,g->max_hp);
            }
            player_context(g,0);g->campaign.endless_wave=wave;g->event=EVENT_HORDE;g->banner=90;
        }
        int bosses=0;for(i=0;i<MAX_ENEMIES;i++)if(g->enemies[i].active&&g->enemies[i].kind==3)++bosses;
        if(g->campaign.ticks%GAME_ENDLESS_WAVE_TICKS>=18*60 && wave>=g->campaign.endless_boss_wave && bosses<2 && (!g->combat.boss_id || g->campaign.ticks-g->combat.boss_intro_start>=g->combat.boss_intro_ticks)) {
            int id=g->next_enemy_id;
            if(spawn_enemy(g,3)) {
                int ultimate=(g->campaign.endless_boss_wave%3)==2;
                for(i=0;i<MAX_ENEMIES;i++)if(g->enemies[i].active&&g->enemies[i].id==id+1) {
                    Enemy *e=&g->enemies[i];
                    if(ultimate){e->family=6;e->hp*=2;e->max_hp=e->hp;++g->campaign.ultimate_spawned;
                        e->spawn_until=g->campaign.ticks+boss_voice_ticks[6]+12;g->combat.boss_family=6;g->combat.boss_intro_ticks=boss_voice_ticks[6]+12;}
                }
                ++g->campaign.endless_boss_wave;g->event=EVENT_BOSS;g->banner=120;
            }
        }
    }
    if(g->campaign.scene<5 && !g->campaign.boss_spawned && g->campaign.ticks>=24*60) {
        if(spawn_enemy(g,3)) {g->campaign.boss_spawned=1;g->event=EVENT_BOSS;g->banner=120;}
    }
    limit=(g->campaign.scene==5?clamp(26+g->campaign.endless_wave*2,26,60):14+g->campaign.scene*2)*CAMPAIGN_SWARM_MULTIPLIER;
    if(--g->spawn_timer<=0) {
        int minions=0;
        for(i=0;i<MAX_ENEMIES;i++)minions+=g->enemies[i].active&&g->enemies[i].kind!=3;
        int batch=(1+g->campaign.scene/3)*CAMPAIGN_SWARM_MULTIPLIER;
        for(i=0;i<batch && minions<limit;i++) {
            int pick=rand_n(g,10);
            if(spawn_enemy(g,pick<2?2:pick<5?4:pick<7?1:0))++minions;
        }
        int period=g->campaign.scene==5?clamp(44-g->campaign.endless_wave,18,44):66-g->campaign.scene*4;
        g->spawn_timer=(period+1)/2;
    }
    /* Visible local crystals prevent unlucky drop locations from stalling levels. */
    if(g->campaign.ticks%180==0) drop_gem(g,g->x+(rand_n(g,81)-40)*FIX,g->y+(rand_n(g,81)-40)*FIX,3);

}

int game_hostile_slots(const Game *g) {
    int slots=!g->limit_mode ? NORMAL_HOSTILE_SLOTS : g->limit_shots ? g->limit_shot_cap : MAX_HOSTILE_BULLETS;
    const StressSettings *s=stress(g);
    if (g->stress_tier==STRESS_CUSTOM && slots>custom_target) slots=custom_target;
    return s && slots>s->shots ? s->shots : slots;
}
int game_stress_bullets(void) { return custom_target; }
int game_seed_size(void) { return pressure_seed_size; }
int game_set_seed_size(int pixels) {
    if (pixels!=1 && pixels!=3 && pixels!=6 && pixels!=9 && pixels!=13 && pixels!=17 && pixels!=25) return 0;
    pressure_seed_size=pixels;
    return 1;
}
int game_seed_styles(const Game *g){return g->limit_mode?GAME_SEED_STYLES:GAME_CAMPAIGN_SEED_STYLES;}
int game_seed_half(const Game *g) { return g->limit_mode ? pressure_seed_size/2 : GAME_NORMAL_SEED_HALF; }
int game_seed_extent(const Game *g) { return g->limit_mode ? pressure_seed_size : 2*GAME_NORMAL_SEED_HALF+1; }
int game_player_level(const Game *g, const GamePlayer *p) { return g->limit_mode ? (g->stress_tier>=STRESS_HIGH ? 7 : 1) : clamp(p->level,1,9); }
int game_actor_level(const Game *g) { return game_player_level(g,&g->actor); }
const Seed *game_hostile_at(const Game *g, int index) {
    if (index<0 || index>=game_hostile_slots(g)) return 0;
    if (g->limit_mode && g->limit_shots) return &g->limit_shots[index];
    return &g->hostile[index];
}
static Seed *hostile_slot(Game *g, int index) {
    return (Seed *)game_hostile_at(g, index);
}
int game_hostile_count(const Game *g) { return g->hostile_active; }
int game_hostile_used(const Game *g) { return g->hostile_used; }
int game_seed_draw(const Game *g, const GfxItem **items) {
    *items=g->seed_draw[g->seed_draw_buf];
    return g->seed_draw_ok ? g->seed_draw_n : -1;
}
int game_seed_draw_clip(const Game *g) { return g->seed_draw_clip; }
int game_dropped_shots(const Game *g) { return g->dropped_shots; }
static int grow_limit_shots(Game *g) {
    const StressSettings *s=stress(g);
    int next=g->limit_shot_cap<256 ? 256 : g->limit_shot_cap*2;
    Seed *grown;
    const Seed *old=g->limit_shots ? g->limit_shots : g->hostile;
    int old_cap=g->limit_shots ? g->limit_shot_cap : MAX_HOSTILE_BULLETS;
    if (next<=old_cap) next=old_cap*2;
    if (s && next>s->shots) next=s->shots;
    if (next>GAME_HOSTILE_INDEX_SLOTS) next=GAME_HOSTILE_INDEX_SLOTS;
    if (next<=old_cap || !g->config.grow_shots) return 0;
    grown=g->config.grow_shots(g->config.user,old,old_cap,next);
    if (!grown) return 0;
    memset(grown+old_cap,0,(size_t)(next-old_cap)*sizeof(Seed));
    g->limit_shots=grown; g->limit_shot_cap=next; return 1;
}
/*
 * Free-slot index (see game.h). The old code looked for a free slot by scanning
 * the pool; with a full pool every seed an enemy tried to fire scanned all
 * slots (4096 x 32 bytes) just to find nothing. Now:
 *   take    : lowest set bit of the summary, then of that word  -> O(1)
 *   release : set the bit again                                  -> O(1)
 *   full    : all summary words are 0                            -> O(1)
 * The lowest free slot is taken, exactly like the scan did, so the seeds end
 * up in the same slots and the game plays the same.
 */
static int lowest_bit(uint32_t x) {             /* x != 0; RV32IM has no ctz */
    static const unsigned char pos[32]={0,1,28,2,29,14,24,3,30,22,20,15,25,17,4,8,
        31,27,13,23,21,19,16,7,26,12,18,6,11,5,10,9};
    return pos[((x&(0u-x))*0x077CB531u)>>27];
}
static void hostile_mark_free(Game *g, int from, int to) {
    int i;
    for (i=from;i<to;i++) {
        g->hostile_free[i>>5]|=1u<<(i&31);
        g->hostile_free_sum[i>>10]|=1u<<((i>>5)&31);
    }
    g->hostile_indexed=to;
}
static void release_hostile(Game *g, Seed *b, int index) {
    b->active=0;
    g->hostile_free[index>>5]|=1u<<(index&31);
    g->hostile_free_sum[index>>10]|=1u<<((index>>5)&31);
    --g->hostile_active;
}
static Seed *take_hostile(Game *g) {
    int s,k=-1;
    /* 空位只可能在 [0, hostile_indexed) 里：池子 32768 时不必每次扫全部 32 个汇总字 */
    const int nsum=(g->hostile_indexed+1023)>>10;
    for (s=0;s<nsum && s<GAME_HOSTILE_SUMS;s++) if (g->hostile_free_sum[s]) {
        int w=s*32+lowest_bit(g->hostile_free_sum[s]);
        k=w*32+lowest_bit(g->hostile_free[w]);
        break;
    }
    if (k<0) {                                  /* pool full */
        if (!g->limit_mode) return 0;
        if (!grow_limit_shots(g)) { ++g->dropped_shots; return 0; }
        k=g->hostile_indexed;                   /* first slot of the new part */
        hostile_mark_free(g,k,game_hostile_slots(g));
    }
    g->hostile_free[k>>5]&=~(1u<<(k&31));
    if (!g->hostile_free[k>>5]) g->hostile_free_sum[k>>10]&=~(1u<<((k>>5)&31));
    ++g->hostile_active;
    g->seed_draw_ok=0;                          /* pool changed: draw table stale until the seed loop */
    if (k>=g->hostile_used) g->hostile_used=k+1;
    return hostile_slot(g,k);
}
static void hostile_shot(Game *g, const Enemy *e, int vx, int vy, int damage, int style) {
    Seed *b=take_hostile(g);
    if (!b) return;
    /* active = 伤害（见 game.h Seed） */
    b->active=(uint8_t)(damage<1 ? 1 : damage>255 ? 255 : damage); b->x=e->x; b->y=e->y;
    b->vx=(int16_t)vx; b->vy=(int16_t)vy;
    b->life=(int16_t)(g->limit_mode ? stress(g)->shot_life : 300); b->style=(uint8_t)(g->limit_mode?style:clamp(e->family,0,6)*4+(style&3));
}

/* 自定义压力：改弹幕上限（参照队友版 game_set_stress_bullets）。不凭空造弹幕：
 * 调高时新空位等敌人开火填上；调低时只让超出上限的槽里的弹幕消失，然后重建空位索引。 */
int game_set_stress_bullets(Game *g, int count) {
    int i,n,used=0,active=0; Seed *pool;
    custom_target=clamp(count,1,MAX_HOSTILE_BULLETS);
    if (g->stress_tier!=STRESS_CUSTOM || !g->limit_mode) return custom_target;
    n=game_hostile_slots(g);                         /* = 新上限（宿主可增长池时可能更小） */
    pool=g->limit_shots ? g->limit_shots : g->hostile;
    for (i=n;i<g->hostile_used;i++) pool[i].active=0;
    memset(g->hostile_free,0,sizeof g->hostile_free);
    memset(g->hostile_free_sum,0,sizeof g->hostile_free_sum);
    hostile_mark_free(g,0,n);
    for (i=0;i<n;i++) if (pool[i].active) {
        ++active; used=i+1; g->hostile_free[i>>5]&=~(1u<<(i&31));
    }
    for (i=0;i<GAME_HOSTILE_WORDS;i++) if (!g->hostile_free[i])
        g->hostile_free_sum[i>>5]&=~(1u<<(i&31));
    g->hostile_active=active; g->hostile_used=used;
    g->seed_draw_ok=0;                               /* 池子变了：下一次 tick 前渲染改走池子 */
    return custom_target;
}

static void boss_attack(Game *g, Enemy *e) {
    static const int ring[12][2]={{256,0},{222,128},{128,222},{0,256},{-128,222},{-222,128},{-256,0},{-222,-128},{-128,-222},{0,-256},{128,-222},{222,-128}};
    if(--e->attack_timer>0)return;
    int family=clamp(e->family,0,6),mode=family,ang=e->volley%12,enrage=e->hp<e->max_hp/2;
    int vx,vy,speed=410+family*28+enrage*70;Enemy origin=*e;
    ++e->volley;e->attack_anim=18;
    direction((g->x-e->x)/FIX,(g->y-e->y)/FIX,speed,&vx,&vy);
    if(!vx&&!vy)vx=speed;
    if(family==6)mode=(e->volley+enrage)%6;
    if(mode==0||mode==3){
        int gap=rand_n(g,12);
        for(int i=0;i<12;i++)if(i!=gap && i!=(gap+1)%12){
            int k=(i+ang)%12;hostile_shot(g,e,ring[k][0]*speed/256,ring[k][1]*speed/256,8+family,mode==0?0:3);
        }
        if(enrage)for(int i=-1;i<=1;i++)hostile_shot(g,e,vx-vy*i/5,vy+vx*i/5,10+family,2);
    }else if(mode==1){
        for(int i=-2;i<=2;i++)hostile_shot(g,e,vx-vy*i/4,vy+vx*i/4,9,1);
        if(e->volley&1){e->x+=vx*8;e->y+=vy*8;}
    }else if(mode==2){
        for(int i=0;i<12;i+=enrage?2:3){int k=(i+ang)%12;hostile_shot(g,e,ring[k][0]*(speed+140)/256,ring[k][1]*(speed+140)/256,10,2);}
    }else if(mode==4){
        origin.y=(AY0+20)*FIX;
        for(int i=0;i<(enrage?9:6);i++){origin.x=(AX0+32+rand_n(g,AX1-AX0-64))*FIX;hostile_shot(g,&origin,(rand_n(g,3)-1)*80,440,9,3);}
        hostile_shot(g,e,vx,vy,10,2);
    }else{
        for(int side=-1;side<=1;side+=2){
            origin.x=e->x+side*28*FIX;
            for(int i=-1;i<=1;i++)hostile_shot(g,&origin,vx-vy*(i*42+side*20)/256,vy+vx*(i*42+side*20)/256,10,side<0?0:2);
        }
        if(enrage)for(int i=0;i<12;i+=3)hostile_shot(g,e,ring[(i+ang)%12][0]*400/256,ring[(i+ang)%12][1]*400/256,11,3);
    }
    e->attack_timer=clamp(112-family*5-enrage*20-g->campaign.endless_wave,42,112)+rand_n(g,18);
}

/* Keeps a gap and fires one aimed seed. Contact is backup, not the main threat. */
static void spitter_attack(Game *g, Enemy *e) {
    int vx,vy,minutes=g->ticks/3600,i,count;
    int speed=g->limit_mode ? 300 : clamp(420+minutes*12,420,640);
    if (--e->attack_timer>0) return;
    if(!g->limit_mode) {
        int family=clamp(e->family,0,5),wave=g->campaign.endless_wave;
        speed=story_speed(clamp(speed+wave*8,420,800));
        direction((g->x-e->x)/FIX,(g->y-e->y)/FIX,speed,&vx,&vy);
        if(!vx&&!vy)vx=speed;
        ++e->volley;e->attack_anim=12;count=e->kind==2?3:family==0?1:family==2?4:3;
        for(i=0;i<count;i++) {
            int dx=vx,dy=vy,spread=(i-count/2)*50;
            if(family==2){static const int d[4][2]={{1,0},{0,1},{-1,0},{0,-1}};dx=d[(i+e->volley)%4][0]*speed;dy=d[(i+e->volley)%4][1]*speed;}
            else if(family==4){int jitter=rand_n(g,101)-50;dx=vx-vy*jitter/256;dy=vy+vx*jitter/256;}
            else {dx=vx-vy*spread/256;dy=vy+vx*spread/256;}
            hostile_shot(g,e,dx,dy,clamp(6+family+wave/3,6,40),(i+e->volley)&3);
        }
        e->attack_timer=clamp(82-family*4-wave,30,82)+rand_n(g,16);return;
    }
    direction((g->x-e->x)/FIX,(g->y-e->y)/FIX,speed,&vx,&vy);
    if (!vx && !vy) vx=speed;
    ++e->volley;
    count=g->limit_mode ? stress(g)->volley : (e->kind==2?3:1+g->campaign.scene/3);
    if (g->stress_tier==STRESS_CUSTOM) {
        /* 每轮颗数跟上限走（每 1280 个上限多 1 颗），扇形宽度固定 */
        int free_slots=game_hostile_slots(g)-g->hostile_active;
        count=(custom_target+1279)/1280;
        if (count>free_slots) count=free_slots;
    }
    for (i=0;i<count;i++) {
        int spread=(i-count/2)*36;
        int damage=g->limit_mode ? 1 : clamp(7+minutes,7,16);
        int style=g->limit_mode ? 1+(i+e->volley)%(g->stress_tier>STRESS_HIGH ? STRESS_HIGH : g->stress_tier) : 1;
        if (g->stress_tier==STRESS_CUSTOM) spread=count>1 ? ((2*i-count+1)*180)/(count-1) : 0;
        hostile_shot(g,e,vx-vy*spread/256,vy+vx*spread/256,damage,style);
    }
    e->attack_timer=g->limit_mode ? stress(g)->attack_ticks : clamp(75-minutes*2,36,75);
}

static void begin_run(Game *g, uint32_t seed, int tier) {
    int i;
    const int party=g->party_size>1 ? g->party_size : 1, solo=g->solo;   /* 人数设置保留（压力模式 / 演示单人） */
    const int players=solo ? 1 : party;
    Seed *extended=g->limit_shots;
    int extended_cap=g->limit_shot_cap;
    GameConfig config=g->config;
    {
        const int ring=g->seed_draw_buf;
        const size_t tail=offsetof(Game,seed_draw)+sizeof g->seed_draw;
        memset(g,0,offsetof(Game,seed_draw));
        memset((char*)g+tail,0,sizeof(*g)-tail);
        g->seed_draw_buf=ring;
    }
    g->limit_shots=extended; g->limit_shot_cap=extended_cap; g->config=config;
    g->party_size=party; g->solo=solo;
    if (extended && extended_cap>0) memset(extended,0,(size_t)extended_cap*sizeof(Seed));
    g->rng=seed ? seed : 0x12345678u; g->phase=PLAYING;
    g->stress_tier=tier; g->limit_mode=tier!=0;
    hostile_mark_free(g,0,game_hostile_slots(g));   /* index: every slot of this mode is free */
    g->x=(SPROUT_W/2)*FIX; g->y=((AY0+AY1+1)/2)*FIX; g->max_hp=g->hp=g->limit_mode ? 999 : 100;   /* 场地中心（640x480 时 320,252） */
    g->speed=560; g->shots=1; g->damage=2; g->fire_period=17;
    g->pickup_radius=62; g->next_xp=6; g->level=1;
    g->facing=1; g->aim_x=1; g->banner=g->limit_mode ? 180 : 150;
    g->weapon_idle_ticks=1000;                         /* 开局收着枪 */
    if (g->limit_mode) {
        int target=stress(g)->enemies<MAX_ENEMIES ? stress(g)->enemies : MAX_ENEMIES;
        for (i=0;i<target;i++) spawn_enemy(g,4);
        g->spawn_timer=stress(g)->spawn_ticks;
        if (tier==STRESS_CUSTOM)                     /* 开火时刻错开，不让 128 个怪同一 tick 齐射 */
            for (i=0;i<target;i++) g->enemies[i].attack_timer=1+i%stress(g)->attack_ticks;
    } else {
        for (i=0;i<5;i++) spawn_enemy(g,0);
        g->spawn_timer=30;
    }
    game_coop_enable(g,tier ? 1 : players);
    if(!tier) campaign_enter(g,0);
}

void game_coop_enable(Game *g, int n) {
    int i;
    if (n<1) n=1;
    if (n>GAME_PLAYERS) n=GAME_PLAYERS;
    if (g->limit_mode) n=1;
    player_context(g,0);
    g->player_count=n;
    g->party[0]=g->actor;
    for (i=1;i<n;i++) {                                /* P2 左边、P3 右边 96 px，往下 32 px，状态抄 P1 */
        g->party[i]=g->actor;
        g->party[i].x=clamp(g->x+(i==1 ? -96 : 96)*FIX,(AX0+31)*FIX,(AX1-31)*FIX);
        g->party[i].y=clamp(g->y+32*FIX,(AY0+39)*FIX,(AY1-33)*FIX);
    }
    if(!g->limit_mode) {
        static const int health[3]={100,145,85},damage[3]={3,10,5},period[3]={12,25,24},speed[3]={740,610,550};
        for(i=0;i<n;i++) {
            GamePlayer *p=i==0?&g->actor:&g->party[i];
            p->hp=p->max_hp=health[i];p->damage=damage[i];p->fire_period=period[i];p->speed=speed[i];p->pickup_radius=170;p->shots=i==1?2:3;
        }
    }
    g->upgrade_mask=0; g->upgrade_actor=0; g->upgrade_hold=0;
    g->team_xp=g->xp; g->team_level=g->level; g->team_next_xp=g->next_xp;
    for (i=0;i<GAME_PLAYERS;i++) g->pick_slot[i]=-1;
}
int game_set_party(Game *g, int n) {
    g->party_size=n<1 ? 1 : n>GAME_PLAYERS ? GAME_PLAYERS : n;
    return g->party_size;
}


static const int scene_first[6]={1,4,6,7,8,9};
static const int scene_last[6]={3,5,6,7,8,9};
int game_scene_for_level(int level) { return level<=3?0:level<=5?1:clamp(level-4,2,5); }
const char *game_scene_name(int scene) {
    static const char *name[6]={"MOSS COURT","JADE PALACE","THUNDER CITY","EMBER PALACE","ASTRAL SANCTUM","TWIN DRAGONS"};
    return name[clamp(scene,0,5)];
}
void game_campaign_clock(Game *g,uint64_t cycles,uint32_t hz,int visible) {
    if(!hz)return;
    const int active=visible&&!g->limit_mode&&(g->phase==PLAYING||g->phase==UPGRADE);
    g->campaign.clock_external=1;
    if(g->campaign.clock_valid && active && g->campaign.clock_was_active) {
        uint64_t delta=cycles-g->campaign.clock_previous;
        uint64_t budget=(uint64_t)hz*(g->campaign.scene==5?100000000u:60u);
        uint64_t remaining=budget-g->campaign.clock_active_cycles;
        g->campaign.clock_active_cycles+=delta>remaining?remaining:delta;
        if(g->phase==UPGRADE && !g->upgrade_hold) {
            uint64_t choice_cap=(uint64_t)hz*5u;
            uint64_t choice_left=choice_cap-g->campaign.choice_cycles;
            g->campaign.choice_cycles+=delta>choice_left?choice_left:delta;
            g->campaign.choice_ticks=(int)(g->campaign.choice_cycles*60u/hz);
        }
        {uint64_t t=g->campaign.clock_active_cycles*60u/hz;g->campaign.ticks=t>180000000u?180000000:(int)t;}
        if(g->campaign.scene<5 && g->campaign.clock_active_cycles>=budget) {
            g->campaign.timed_out=1;g->phase=DEFEAT;
        }
    }
    g->campaign.clock_previous=cycles;
    g->campaign.clock_valid=1;g->campaign.clock_was_active=active;
}
int game_scene_remaining(const Game *g){if(g->campaign.scene==5)return -1;return clamp((GAME_SCENE_TICKS-g->campaign.ticks+59)/60,0,60);}
static int campaign_can_level(const Game *g,int level){
    return g->limit_mode || (g->campaign.scene==5 && level<1000000) || (level<9 && (level<scene_last[g->campaign.scene] || (g->bosses_killed>g->campaign.boss_baseline && g->campaign.ticks>=GAME_SCENE_MIN_TICKS)));
}
static void campaign_enter(Game *g,int scene) {
    int i;g->campaign.scene=clamp(scene,0,5);g->campaign.ticks=0;g->campaign.boss_spawned=0;
    g->campaign.boss_baseline=g->bosses_killed;g->campaign.timed_out=0;
    g->campaign.endless_wave=g->campaign.endless_boss_wave=g->campaign.ultimate_spawned=0;
    memset(&g->combat,0,sizeof g->combat);
    g->campaign.clock_active_cycles=0;g->campaign.clock_valid=0;g->campaign.clock_was_active=0;
    memset(g->enemies,0,sizeof g->enemies);memset(g->gems,0,sizeof g->gems);memset(g->bullets,0,sizeof g->bullets);
    memset(g->hostile,0,sizeof g->hostile);memset(g->particles,0,sizeof g->particles);
    memset(g->hostile_free,0,sizeof g->hostile_free);memset(g->hostile_free_sum,0,sizeof g->hostile_free_sum);
    g->hostile_active=g->hostile_used=g->hostile_indexed=0;hostile_mark_free(g,0,game_hostile_slots(g));
    g->enemy_count=0;g->seed_draw_ok=0;g->spawn_timer=15;g->event=EVENT_START;g->banner=90;
    for(i=0;i<5*CAMPAIGN_SWARM_MULTIPLIER;i++)spawn_enemy(g,i%2);
    for(i=0;i<g->player_count;i++){player_context(g,i);g->invincible=90;int recovery=g->max_hp*(g->hp>0?2:1)/3;g->hp=clamp(g->hp+20,0,g->max_hp);if(g->hp<recovery)g->hp=recovery;}
    player_context(g,0);
}
int game_start_scene(Game *g,int scene) {
    int i,lv;
    if(scene<1||scene>6)return 0;
    begin_run(g,0xC0FFEEu+(unsigned)scene,0);lv=scene_first[scene-1];
    for(i=0;i<g->player_count;i++){
        player_context(g,i);g->level=lv;g->xp=0;g->next_xp=required_xp_at(g,lv);
        g->upgrades[POWER]=lv-1;g->damage+=(lv-1)*(i==1?5:i==2?4:3);g->max_hp+=(lv-1)*4;g->hp=g->max_hp;
    }
    player_context(g,0);g->team_level=lv;g->team_xp=0;g->team_next_xp=required_xp_at(g,lv);
    campaign_enter(g,scene-1);return 1;
}

void game_start(Game *g, uint32_t seed) { begin_run(g,seed,0); }
void game_start_stress(Game *g, uint32_t seed, int tier) {
    begin_run(g,seed,clamp(tier,STRESS_LOW,STRESS_CUSTOM));
}
void game_start_limit(Game *g, uint32_t seed) { game_start_stress(g,seed,STRESS_HIGH); }

static void restart_run(Game *g) {
    if (g->limit_mode) game_start_stress(g,0xC0FFEEu,g->stress_tier);
    else game_start(g,0xC0FFEEu);
}

static void return_to_title(Game *g) {
    if (g->limit_shots && g->config.release_shots)
        g->config.release_shots(g->config.user,g->limit_shots);
    g->limit_shots=0; g->limit_shot_cap=0;
    game_start(g,0xC0FFEEu);
    g->phase=TITLE;
}

void game_init(Game *g, uint32_t seed) {
    GameConfig config;
    memset(&config,0,sizeof(config));
    game_init_config(g,seed,config);
}

void game_init_config(Game *g, uint32_t seed, GameConfig config) {
    memset(g,0,sizeof(*g));
    g->config=config;
    game_start(g,seed); g->phase=TITLE;
}

void game_shutdown(Game *g) {
    if (!g) return;
    if (g->limit_shots && g->config.release_shots)
        g->config.release_shots(g->config.user,g->limit_shots);
    g->limit_shots=0; g->limit_shot_cap=0;
    memset(&g->config,0,sizeof(g->config));
}

static int upgrade_allowed(Game *g, int id) {
    if(id<0||id>=UPGRADE_COUNT)return 0;
    if(g->limit_mode)return id<6 && (id!=MULTISHOT || g->shots<4) && (id!=RAPID || g->fire_period>7)
        && (id!=BOOTS || g->speed<950) && (id!=MAGNET || g->pickup_radius<190);
    const int role=g->actor_index;
    switch(id){
        case RAPID:return g->fire_period>normal_attack_floor(role);
        case POWER:return g->damage<1000000;
        case BOOTS:return g->speed<(role==0?1100:950)||g->upgrades[BOOTS]<8;
        case VITALITY:return g->max_hp<1000000||g->hp<g->max_hp;
        case MAGNET:return g->pickup_radius<320;
        case MULTISHOT:return g->shots<normal_shot_limit(role);
        case ULT_POWER:return g->upgrades[ULT_POWER]<100;
        case ULT_AREA:return g->upgrades[ULT_AREA]<4;
        case ULT_HASTE:return game_skill_cooldown(&g->actor,role)>120;
        case PASSIVE:return g->upgrades[PASSIVE]<(role==1?6:8);
        default:return 0;
    }
}

static void offer_upgrades(Game *g) {
    if(!g->limit_mode){
        /* Upgrade selection interrupts the spoken cast. Cancel its remaining
         * pulses too: no invisible delayed damage when the menu closes. */
        for(int p=0;p<GAME_PLAYERS;p++){
            g->combat.player[p].skill_total=0;g->combat.player[p].skill_ticks=0;
        }
        int pool[UPGRADE_COUNT],n=0,ult[3],nu=0;
        for(int id=0;id<UPGRADE_COUNT;id++)if(upgrade_allowed(g,id)){
            pool[n++]=id;if(id>=ULT_POWER&&id<=ULT_HASTE)ult[nu++]=id;
        }
        /* One useful ultimate choice while any remain; all other cards are
         * sampled without replacement. Capped ranks never enter the pool. */
        int chosen=nu?ult[rand_n(g,nu)]:n?pool[rand_n(g,n)]:VITALITY;
        g->choices[0]=chosen;
        for(int j=0;j<n;j++)if(pool[j]==chosen){pool[j]=pool[--n];break;}
        for(int slot=1;slot<3;slot++){
            int j=n?rand_n(g,n):0;
            g->choices[slot]=n?pool[j]:chosen;
            if(n)pool[j]=pool[--n];
        }
        for(int k=2;k>0;k--){int j=rand_n(g,k+1),v=g->choices[k];g->choices[k]=g->choices[j];g->choices[j]=v;}
        g->phase=UPGRADE;g->campaign.choice_ticks=0;g->campaign.choice_cycles=0;return;
    }
    int i,j,id,valid[UPGRADE_COUNT],count=0;
    for (i=0;i<UPGRADE_COUNT;i++) if (upgrade_allowed(g,i)) valid[count++]=i;
    for (i=count-1;i>0;i--) {
        j=rand_n(g,i+1); id=valid[i]; valid[i]=valid[j]; valid[j]=id;
    }
    for (i=0;i<3;i++) g->choices[i]=valid[i%count];
    g->phase=UPGRADE;
}

/* 单人升级（和以前一样） */
static void check_level(Game *g) {
    if(!g->limit_mode&&g->combat.boss_id&&g->campaign.ticks-g->combat.boss_intro_start<g->combat.boss_intro_ticks)return;
    if (g->xp>=g->next_xp && campaign_can_level(g,g->level)) {
        g->xp-=g->next_xp; ++g->level; g->next_xp=required_xp(g);
        if(!g->limit_mode){g->damage=clamp(g->damage+1,1,1000000);g->max_hp=clamp(g->max_hp+4,1,1000000);} g->hp=clamp(g->hp+10,0,g->max_hp); offer_upgrades(g);
        g->upgrade_mask|=1<<g->actor_index;
        g->pick_slot[g->actor_index]=-1;
    }
}
/* 多人：共享经验涨够了 -> 全队升一级，每个活着的玩家各抽 3 张卡（各自的 rng 顺序：P1 P2 P3） */
static void sync_team(Game *g) {
    int i;
    const int prev=g->actor_index;
    for (i=0;i<g->player_count;i++) {
        player_context(g,i);
        g->xp=g->team_xp; g->level=g->team_level; g->next_xp=g->team_next_xp;
    }
    player_context(g,prev);
}
static void check_team_level(Game *g) {
    int i;
    if(!g->limit_mode&&g->combat.boss_id&&g->campaign.ticks-g->combat.boss_intro_start<g->combat.boss_intro_ticks)return;
    if (g->upgrade_mask || !game_players_alive(g)) return;
    g->team_next_xp=required_xp_at(g,g->team_level);
    if (g->team_xp>=g->team_next_xp && campaign_can_level(g,g->team_level)) {
        g->team_xp-=g->team_next_xp; ++g->team_level; g->team_next_xp=required_xp_at(g,g->team_level);
        for (i=0;i<g->player_count;i++) {
            player_context(g,i);
            g->pick_slot[i]=-1;
            if (g->hp<=0) continue;                    /* 倒下的玩家不升级 */
            if(!g->limit_mode){g->damage=clamp(g->damage+1,1,1000000);g->max_hp=clamp(g->max_hp+4,1,1000000);} g->hp=clamp(g->hp+10,0,g->max_hp); offer_upgrades(g);
            g->upgrade_mask|=1<<i;
        }
        player_context(g,0);
        if (g->upgrade_mask) { g->upgrade_actor=__builtin_ctz((unsigned)g->upgrade_mask); g->phase=UPGRADE; }
    }
    sync_team(g);
}
/* 选完 / 高亮结束：回到游戏，再看一次经验（一次拿够两级） */
static void finish_upgrade(Game *g) {
    g->phase=PLAYING; g->event=EVENT_UPGRADE;
    if (g->player_count>1) check_team_level(g);
    else {
        check_level(g);
        if (g->upgrade_mask) { g->upgrade_actor=__builtin_ctz((unsigned)g->upgrade_mask); g->phase=UPGRADE; }
    }
    player_context(g,0);
}
static void apply_upgrade(Game *g, int id) {
    ++g->upgrades[id];
    switch (id) {
        case RAPID: g->fire_period=clamp(g->fire_period-3,g->limit_mode?7:normal_attack_floor(g->actor_index),60); if(!g->limit_mode)++g->damage; break;
        case POWER: g->damage+=g->actor_index==1?4:g->actor_index==2?3:2; break;
        case BOOTS: g->speed=clamp(g->speed+80,0,!g->limit_mode&&g->actor_index==0?1100:950); break;
        case VITALITY: g->max_hp+=20; g->hp=clamp(g->hp+40,0,g->max_hp); break;
        case MAGNET: g->pickup_radius=clamp(g->pickup_radius+40,0,g->limit_mode?190:320); break;
        case MULTISHOT: g->shots=clamp(g->shots+1,1,g->limit_mode?4:normal_shot_limit(g->actor_index)); if(!g->limit_mode)g->damage+=2; break;
        case ULT_POWER: break; /* cast scales from upgrade ranks */
        case ULT_AREA: break;
        case ULT_HASTE: break;
        case PASSIVE: if(g->actor_index==0)g->speed=clamp(g->speed+45,0,1100);if(g->actor_index==2)g->max_hp+=4;else ++g->damage;break;
        default: break;
    }
    if(!g->limit_mode){
        CarrotFighter *f=&g->combat.player[g->actor_index];
        if(id==ULT_HASTE||id==RAPID)f->skill_cooldown=clamp(f->skill_cooldown-(id==RAPID?12:36),0,game_skill_cooldown(&g->actor,g->actor_index));
        if(id==BOOTS)f->dash_cooldown=clamp(f->dash_cooldown-8,0,CARROT_DASH_COOLDOWN);
        if(id==PASSIVE&&g->actor_index==1)f->reflect_cooldown=clamp(f->reflect_cooldown-15,0,180);
        if(id==PASSIVE&&g->actor_index==2){
            f->shield_cooldown=clamp(f->shield_cooldown-30,0,480);
            if(f->shield_charges&&g->upgrades[PASSIVE]<=4&&!(g->upgrades[PASSIVE]&1))++f->shield_charges;
        }
        g->damage=clamp(g->damage,1,1000000);g->max_hp=clamp(g->max_hp,1,1000000);g->hp=clamp(g->hp,0,g->max_hp);
    }
    g->invincible=60; g->banner=90;
}
int game_choose_upgrade_for(Game *g, int player, int slot) {
    if (g->phase!=UPGRADE || g->upgrade_hold || player<0 || player>=g->player_count || slot<0 || slot>2 ||
        !(g->upgrade_mask&(1<<player))) return 0;
    player_context(g,player);
    apply_upgrade(g,g->choices[slot]);
    g->pick_slot[player]=slot;
    g->upgrade_mask&=~(1<<player);
    player_context(g,0);
    if (g->upgrade_mask) g->upgrade_actor=__builtin_ctz((unsigned)g->upgrade_mask);
    else if (GAME_PICK_HOLD>0) g->upgrade_hold=GAME_PICK_HOLD;   /* 都选完：高亮一会儿 */
    else finish_upgrade(g);
    return 1;
}
void game_choose_upgrade(Game *g, int slot) {
    game_choose_upgrade_for(g,g->upgrade_actor,slot);
}

static int nearest_enemy(Game *g, int x, int y) {
    int i,index=-1,best=450*450;                      /* 瞄准范围 300 x 1.5 */
    for (i=0;i<MAX_ENEMIES;i++) {
        Enemy *e=&g->enemies[i]; int dx,dy,d;
        if (!e->active || e->spawn) continue;
        dx=e->x/FIX-x; dy=e->y/FIX-y; d=dx*dx+dy*dy;
        if(d>450*450)continue;
        if(!g->limit_mode&&e->kind==3)d/=5;
        if (d<best) { best=d; index=i; }
    }
    return index;
}

/* 玩家子弹的激光图方向 0..7（东 = 0，顺时针），开火时定一次（队友版） */
static int bullet_heading(int vx,int vy) {
    int ax=vx<0?-vx:vx,ay=vy<0?-vy:vy;
    if(ay*2<=ax) return vx>=0?0:4;
    if(ax*2<=ay) return vy>=0?2:6;
    if(vx>=0) return vy>=0?1:7;
    return vy>=0?3:5;
}

int game_skill_cooldown(const GamePlayer *p,int role){
    static const int base[3]={360,300,420};
    return clamp(base[clamp(role,0,2)]-p->upgrades[RAPID]*12-p->upgrades[ULT_HASTE]*36,120,420);
}
int game_melee_radius(const GamePlayer *p){return 58+clamp(p->upgrades[POWER],0,5)*3;}
int game_passive_damage(Game *g,int role,int damage){
    if(g->limit_mode)return damage;
    CarrotFighter *f=&g->combat.player[role];
    f->shield_quiet=0;
    if(role==2 && f->shield_charges){
        --f->shield_charges;f->shield_flash=24;++f->shield_blocked;g->sfx|=GS_SHIELD;
        if(!f->shield_charges)f->shield_cooldown=clamp(480-game_player(g,role)->upgrades[PASSIVE]*30,240,480);
        return 0;
    }
    return damage;
}
int game_try_reflect(Game *g,Seed *seed,int index){
    if(g->limit_mode || g->player_count<2)return 0;
    const GamePlayer *a=game_player(g,1);CarrotFighter *f=&g->combat.player[1];
    if(a->hp<=0 || !f->reflect_window || !f->reflect_left)return 0;
    int dx=(seed->x-a->x)/FIX,dy=(seed->y-a->y)/FIX,r=game_melee_radius(a)+12;
    if(dx*dx+dy*dy>r*r)return 0;
    int first=GAME_PLAYER_BULLETS,last=first+GAME_PLAYER_BULLETS,i;
    for(i=first;i<last;i++)if(!g->bullets[i].active)break;
    if(i==last)return 0;
    Bullet *b=&g->bullets[i];int vx=-seed->vx*2,vy=-seed->vy*2;
    if(!vx&&!vy)vx=6*FIX;
    *b=(Bullet){1,seed->x,seed->y,vx,vy,75,a->damage*2+6,bullet_heading(vx,vy),1};
    g->combat.explosive[i]=1;release_hostile(g,seed,index);
    if(!f->reflect_cooldown)f->reflect_cooldown=clamp(180-a->upgrades[PASSIVE]*15,90,180);
    --f->reflect_left;f->reflect_flash=18;++f->reflected;g->sfx|=GS_REFLECT;return 1;
}
const char *game_boss_name(int f){static const char *s[7]={"ANCIENT ROOT KING","JADE QILIN","THUNDER ENGINE","EMBER PHOENIX","ASTRAL ORACLE","TWIN DRAGON KING","ECLIPSE EMPEROR"};return s[clamp(f,0,6)];}
const char *game_boss_call(int f){static const char *s[7]={"MY DOMAIN. YOUR FINAL REST.","LEAVE THE JADE PALACE!","THUNDER LEAVES NO ESCAPE!","LET THE FLAMES CONSUME ALL!","THE STARS FORETELL YOUR END!","BOW BEFORE ICE AND FIRE!","SIX CRYSTALS. ONE FINAL ECLIPSE!"};return s[clamp(f,0,6)];}
static void combat_damage(Game *g,Enemy *e,int damage);
static void shoot(Game *g) {
    if(!g->limit_mode && g->actor_index==1){
        int n=0,j,range=game_melee_radius(&g->actor);
        CarrotFighter *f=&g->combat.player[1];
        for(j=0;j<MAX_ENEMIES;j++){
            Enemy *e=&g->enemies[j];int dx=(e->x-g->x)/FIX,dy=(e->y-g->y)/FIX;
            int reach=range+game_enemy_radius(e->kind);
            if(e->active&&!e->spawn&&dx*dx+dy*dy<reach*reach){combat_damage(g,e,g->damage);n++;}
        }
        if(n){if(!f->reflect_cooldown){f->reflect_window=6;f->reflect_left=6+clamp(g->upgrades[PASSIVE],0,6);}
            g->sfx|=GS_SHOT;g->muzzle=12;g->weapon_idle_ticks=0;g->fire_timer=g->fire_period;}
        else g->fire_timer=4;
        /* Retain the circular melee/parry, then launch the new ranged blades. */
    }
    int target=nearest_enemy(g,g->x/FIX,g->y/FIX), i,shot;
    const int first=g->actor_index*GAME_PLAYER_BULLETS,last=first+GAME_PLAYER_BULLETS;   /* 每个玩家自己的子弹槽 */
    int dx,dy,vx,vy;
    const int speed=g->limit_mode ? 7*FIX : story_speed(7*FIX);
    if (target<0) return;
    dx=(g->enemies[target].x-g->x)/FIX; dy=(g->enemies[target].y-g->y)/FIX;
    direction(dx,dy,speed,&vx,&vy);
    if (!vx && !vy) vx=speed;
    g->aim_x=dx; g->aim_y=dy; g->facing=dx<0 ? -1 : 1;
    g->weapon_heading=weapon_heading_for(dx,dy);       /* 双枪转向目标（队友版） */
    g->weapon_flash_mask=0;
    int chosen[6]={target,-1,-1,-1,-1,-1};
    for (shot=0;shot<g->shots;shot++) {
        int starget=target;
        if(!g->limit_mode&&shot>0&&shot<6&&g->enemies[target].kind!=3){
            int best=450*450;
            for(int j=0;j<MAX_ENEMIES;j++){
                Enemy *e=&g->enemies[j];int used=0;
                for(int k=0;k<shot;k++)used|=j==chosen[k];
                if(!e->active||e->spawn||used)continue;
                int dx=(e->x-g->x)/FIX,dy=(e->y-g->y)/FIX,d=dx*dx+dy*dy;
                if(d<best){best=d;starget=j;}
            }
            chosen[shot]=starget;
        }
        int spread=g->limit_mode?(shot*2-(g->shots-1))*14:0;
        for (i=first;i<last;i++) if (!g->bullets[i].active) break;
        if (i==last) break;
        {
            Bullet *b=&g->bullets[i];
            const int gun=g->shots==1 ? (g->weapon_burst&1) : (shot&1);   /* 单发时左右枪轮流 */
            const WeaponPose p=weapon_pose(g,gun,0);
            b->active=1; b->x=p.mx*FIX; b->y=p.my*FIX;  /* 从枪口射出 */
            b->vx=vx-vy*spread/256; b->vy=vy+vx*spread/256;
            if(!g->limit_mode)direction((g->enemies[starget].x-b->x)/FIX,(g->enemies[starget].y-b->y)/FIX,speed,&b->vx,&b->vy);
            if(!g->limit_mode){
                int offset=(shot*2-(g->shots-1))*3;
                b->x-=vy*offset/(speed/FIX);b->y+=vx*offset/(speed/FIX);
            }
            b->damage=!g->limit_mode&&g->actor_index==1?clamp(g->damage*3/4,1,1000000):g->damage;
            b->life=!g->limit_mode&&g->actor_index==1?42:60;
            b->style=bullet_heading(b->vx,b->vy);      /* 8 个方向的激光图 */
            b->owner=g->actor_index;
            if(game_combat_enabled(g)) {
                CarrotFighter *f=&g->combat.player[g->actor_index];
                const int period=game_combat_period(&g->actor);
                g->combat.explosive[i]=(uint8_t)(++f->shot_cycle>=period);
                if(g->combat.explosive[i]) f->shot_cycle=0;
            }
            g->weapon_flash_mask|=1<<gun;
        }
    }
    g->fire_timer=g->fire_period; g->muzzle=g->limit_mode?4:g->actor_index==2?9:6;
    g->sfx|=GS_SHOT; ++g->weapon_burst; g->weapon_idle_ticks=0;
}

/* Actor is the attack owner. Ultimate kill attribution follows existing rules.
 * Bounded by MAX_ENEMIES (384), never scans the hostile stress pool. */
static void combat_damage(Game *g,Enemy *e,int damage) {
    if(!e->active || e->spawn) return;
    e->hp-=damage; e->hit=5; g->sfx|=GS_HIT;
    if(e->hp>0) return;
    e->active=0; --g->enemy_count; ++g->kills; g->sfx|=GS_KILL;
    drop_gem(g,e->x,e->y,e->kind==3 ? 25+5*(g->ticks/3600) : e->kind==4 ? 2 : e->kind==2 ? 3 : 1);
    /* Explosion sprite replaces per-kill particle clouds for these attacks. */
    if(e->kind==3) { ++g->bosses_killed; if(g->hp>0) g->hp=clamp(g->hp+12,0,g->max_hp); }
    else if(g->kills%15==0 && g->hp>0) g->hp=clamp(g->hp+5,0,g->max_hp);
}
/* One immutable polyline per cast is shared by rendering and collision.
 * It is generated once, never scans hostile stress bullets, and has <=24 segments. */
static void skill_segment(CarrotFighter *f,int u0,int v0,int u1,int v1) {
    if(f->skill_segments>=CARROT_SKILL_SEGMENTS)return;
    CarrotSegment *p=&f->path[f->skill_segments++];
    p->x0=(int16_t)(f->skill_x+(u0*f->skill_dx-v0*f->skill_dy)/256);
    p->y0=(int16_t)(f->skill_y+(u0*f->skill_dy+v0*f->skill_dx)/256);
    p->x1=(int16_t)(f->skill_x+(u1*f->skill_dx-v1*f->skill_dy)/256);
    p->y1=(int16_t)(f->skill_y+(u1*f->skill_dy+v1*f->skill_dx)/256);
}
void game_build_skill_path(CarrotFighter *f,int owner) {
    static const int sn[24]={0,66,128,181,222,247,256,247,222,181,128,66,0,-66,-128,-181,-222,-247,-256,-247,-222,-181,-128,-66};
    int i;f->skill_segments=0;if(f->skill_radius<=0)f->skill_radius=185;
    if(owner==0){int length=576+f->skill_radius-185;for(i=0;i<24;i++)skill_segment(f,i*length/24,0,(i+1)*length/24,0);}
    else if(owner==1)for(i=0;i<24;i++){
        int j=(i+1)%24;skill_segment(f,sn[(i+6)%24]*f->skill_radius/256,sn[i]*f->skill_radius/256,sn[(j+6)%24]*f->skill_radius/256,sn[j]*f->skill_radius/256);
    }else{
        /* Twelve local blossoms: one centre, three inner, eight outer sites. */
        unsigned r=(unsigned)(f->skill_x*131+f->skill_y*313+f->shot_cycle*71+1);
        for(i=0;i<12;i++){
            r=r*1664525u+1013904223u;
            int angle=(i*24/12+(int)(r>>28))%24;
            int radius=i==0?0:i<4?f->skill_radius/3:f->skill_radius-34;
            int x=clamp(f->skill_x+sn[(angle+6)%24]*radius/256,AX0+25,AX1-25);
            int y=clamp(f->skill_y+sn[angle]*radius/256,AY0+35,AY1-35);
            f->path[f->skill_segments++]=(CarrotSegment){x,y-18,x,y+18};
        }
    }
}

int game_skill_hits(const CarrotFighter *f,int x,int y,int radius) {
    int i;radius+=7;
    for(i=0;i<f->skill_segments;i++) {
        const CarrotSegment *s=&f->path[i];
        int minx=s->x0<s->x1?s->x0:s->x1,maxx=s->x0>s->x1?s->x0:s->x1;
        int miny=s->y0<s->y1?s->y0:s->y1,maxy=s->y0>s->y1?s->y0:s->y1;
        if(x<minx-radius||x>maxx+radius||y<miny-radius||y>maxy+radius)continue;
        int dx=s->x1-s->x0,dy=s->y1-s->y0,px=x-s->x0,py=y-s->y0;
        int len=dx*dx+dy*dy,dot=px*dx+py*dy;
        if(dot<=0) {if(px*px+py*py<=radius*radius)return 1;}
        else if(dot>=len) {px=x-s->x1;py=y-s->y1;if(px*px+py*py<=radius*radius)return 1;}
        else {int cross=px*dy-py*dx;if((int64_t)cross*cross<=(int64_t)radius*radius*len)return 1;}
    }
    return 0;
}
static void combat_beam(Game *g) {
    const CarrotFighter *f=&g->combat.player[g->actor_index];
    int rank=clamp(g->upgrades[ULT_POWER],0,100),area=clamp(g->upgrades[ULT_AREA],0,4);
    int power=(int)((int64_t)(g->damage*3+16)*(100+rank*25)/100);
    for(int j=0;j<MAX_ENEMIES;j++){
        Enemy *e=&g->enemies[j];if(!e->active||e->spawn)continue;
        int dx=e->x/FIX-f->skill_x,dy=e->y/FIX-f->skill_y;
        int hit=g->actor_index==1?dx*dx+dy*dy<=(f->skill_radius+game_enemy_radius(e->kind))*(f->skill_radius+game_enemy_radius(e->kind)):
            game_skill_hits(f,e->x/FIX,e->y/FIX,game_enemy_radius(e->kind)+10+area*3);
        if(g->actor_index==2){int reach=f->skill_radius+game_enemy_radius(e->kind);hit=hit&&dx*dx+dy*dy<=reach*reach;}
        if(hit)combat_damage(g,e,e->kind==3?power:power*2);
    }
    /* One bounded clearing pass per cast, following the visible attack region. */
    if(!f->skill_hits)for(int j=0;j<g->hostile_used;j++){
        Seed *b=hostile_slot(g,j);if(!b->active)continue;
        int dx=b->x/FIX-f->skill_x,dy=b->y/FIX-f->skill_y,r=f->skill_radius;
        int hit=g->actor_index==0?game_skill_hits(f,b->x/FIX,b->y/FIX,24+area*4):dx*dx+dy*dy<=r*r;
        if(hit)release_hostile(g,b,j);
    }
}
static void combat_player_move(Game *g,Input in,int *mx,int *my,int *speed) {
    CarrotFighter *f=&g->combat.player[g->actor_index];(void)speed;
    if((in.pressed&KEY_DASH) && !f->dash_cooldown) {
        int dx=*mx,dy=*my,ox=g->x,oy=g->y;
        if(!dx&&!dy)dx=g->facing<0?-1:1;
        int step=(dx&&dy?68:96)*FIX;
        g->x=clamp(g->x+dx*step,(AX0+31)*FIX,(AX1-31)*FIX);
        g->y=clamp(g->y+dy*step,(AY0+39)*FIX,(AY1-33)*FIX);
        for(int k=0;k<3;k++){f->trail_x[k]=(ox+(g->x-ox)*k/3)/FIX;f->trail_y[k]=(oy+(g->y-oy)*k/3)/FIX;}
        f->trail_count=3;f->trail_ticks=18;f->dash_ticks=0;
        f->dash_cooldown=clamp(CARROT_DASH_COOLDOWN-g->upgrades[BOOTS]*8,30,CARROT_DASH_COOLDOWN);
        if(g->invincible<12)g->invincible=12;
    }
}
static void combat_skill(Game *g,Input in) {
    CarrotFighter *f=&g->combat.player[g->actor_index];
    if(f->skill_total) {
        int wind=f->skill_total/3;
        for(int k=0;k<3;k++)if(f->skill_age>=wind+k*12 && !(f->skill_hits&(1<<k))) {
            combat_beam(g);f->skill_hits|=1<<k;
        }
        if(!f->skill_ticks)f->skill_total=0;
    }
    if(!(in.pressed&KEY_SKILL) || f->skill_cooldown || f->skill_ticks) return;
    f->skill_cooldown=game_skill_cooldown(&g->actor,g->actor_index);
    f->skill_total=voice_ticks[g->actor_index]+24;
    f->skill_radius=normal_skill_radius(&g->actor,g->actor_index);
    f->skill_ticks=f->skill_total;f->skill_age=0;f->skill_start=g->campaign.ticks;f->skill_hits=0;
    ++f->shot_cycle;f->skill_x=g->x/FIX;f->skill_y=g->y/FIX;
    if(g->invincible<18)g->invincible=18;
    static const int vec[8][2]={{256,0},{181,181},{0,256},{-181,181},{-256,0},{-181,-181},{0,-256},{181,-181}};
    int heading=bullet_heading(g->aim_x?g->aim_x:g->facing,g->aim_y);
    f->skill_dx=vec[heading][0];f->skill_dy=vec[heading][1];game_build_skill_path(f,g->actor_index);
    if(g->actor_index==2){
        int n=0;for(int j=0;j<MAX_ENEMIES && n<6;j++)if(g->enemies[j].active&&!g->enemies[j].spawn){
            int x=g->enemies[j].x/FIX,y=g->enemies[j].y/FIX;
            int dx=x-f->skill_x,dy=y-f->skill_y,reach=f->skill_radius-24;
            if(dx*dx+dy*dy>reach*reach)continue;
            f->path[n++]=(CarrotSegment){x,y-18,x,y+18};
        }
    }
    g->sfx|=(GS_VOICE0<<g->actor_index);
}

/* 分段计时只在调试版（SPROUT_STATS=1）编译；发行版 gclock() = 0，GSPLIT 什么也不存，编译器删掉整段计时。 */
#if SPROUT_STATS
uint32_t (*game_clock)(void);
GameSplit game_split;
static uint32_t gclock(void) { return game_clock ? game_clock() : 0u; }
#define GSPLIT(f, v) (game_split.f = (v))
#else
#define gclock() 0u
#define GSPLIT(f, v) ((void)(v))
#endif

static int incoming_damage(const Game *g,int base) {
    if(g->limit_mode || g->campaign.scene!=5)return base;
    int64_t n=(int64_t)base*(100+g->campaign.endless_wave*10)/100;
    return n>1000000?1000000:(int)n;
}
static void game_tick_core(Game *g, Input in) {
    int i,j,pi,mx,my,vx,vy,seconds,n_hostile,hostile_used,n_draw=0,n_clip=0,n_player[GAME_PLAYERS]={0,0,0};
    Seed *hostile_pool;
    const int seed_buf=g->seed_draw_buf+1<GAME_SEED_DRAW_BUFS ? g->seed_draw_buf+1 : 0;
    GfxItem *seed_draw=g->seed_draw[seed_buf];   /* 写下一张，前面的表引擎可能还在读 */
    const int seed_half=game_seed_half(g),seed_ext=game_seed_extent(g);   /* 压力模式可换图大小 */
    uint32_t t,t1;
    ++g->visual_ticks;
    if (in.pressed&KEY_MENU) { return_to_title(g); return; }
    if (in.pressed&KEY_RESTART) { restart_run(g); return; }
    if (g->phase==TITLE) {
        if (in.pressed&KEY_LIMIT) g->phase=STRESS_SELECT;
        else if (in.pressed&KEY_START) game_start(g,0xC0FFEEu);
        else if (in.pressed&KEY_PICK1) game_set_party(g,1);     /* 1 2 3：下一局的人数 */
        else if (in.pressed&KEY_PICK2) game_set_party(g,2);
        else if (in.pressed&KEY_PICK3) game_set_party(g,3);
        return;
    }
    if (g->phase==STRESS_SELECT) {
        if (in.pressed&KEY_PICK1 || in.pressed&KEY_START)
            game_start_stress(g,0xC0FFEEu,STRESS_LOW);
        else if (in.pressed&KEY_PICK2) game_start_stress(g,0xC0FFEEu,STRESS_MEDIUM);
        else if (in.pressed&KEY_PICK3) game_start_stress(g,0xC0FFEEu,STRESS_HIGH);
        else if (in.pressed&KEY_PICK4) game_start_stress(g,0xC0FFEEu,STRESS_CUSTOM);
        else if (in.pressed&KEY_PAUSE) return_to_title(g);
        return;
    }
    if (g->phase==DEFEAT || g->phase==VICTORY) {
        if (in.pressed&KEY_START) restart_run(g);
        return;
    }
    if(!g->limit_mode && (g->phase==PLAYING || g->phase==UPGRADE)) {
        if(!g->campaign.clock_external && g->campaign.ticks<180000000)++g->campaign.ticks;
        if(g->campaign.scene<5 && g->campaign.ticks>=GAME_SCENE_TICKS){g->campaign.timed_out=1;g->phase=DEFEAT;return;}
    }
    if (g->phase==UPGRADE) {
        if(!g->campaign.clock_external)++g->campaign.choice_ticks;
        if(!g->limit_mode && g->campaign.choice_ticks>=300 && !g->upgrade_hold){
            int p;for(p=0;p<g->player_count;p++)if(g->upgrade_mask&(1<<p))game_choose_upgrade_for(g,p,0);
        }
#define PICK_OF(k) ((k)&KEY_PICK1 ? 0 : (k)&KEY_PICK2 ? 1 : (k)&KEY_PICK3 ? 2 : -1)
        if (g->upgrade_hold) {                         /* 高亮选中的卡，然后继续 */
            if (--g->upgrade_hold==0) finish_upgrade(g);
            return;
        }
        if (g->player_count<=1) {
            const int k=PICK_OF(in.pressed);
            if (k>=0) game_choose_upgrade(g,k);
        } else {
            /* 每个人用自己的设备选自己的卡；P1（串口 / 上位机）自己选完以后，再按 1 2 3 就替还没选的人选
             * （没接设备的玩家不会卡住）。同一次按键只用一次。 */
            const int mask=g->upgrade_mask,p1_pending=mask&1;
            const int k1=PICK_OF(in.pressed);
            int p;
            for (p=0;p<g->player_count;p++) if (mask&(1<<p)) {
                const int k=p==0 ? k1 : PICK_OF(g->player_input[p].pressed);
                if (k>=0) game_choose_upgrade_for(g,p,k);
            }
            if (!p1_pending && k1>=0 && g->upgrade_mask && !g->upgrade_hold)
                game_choose_upgrade_for(g,g->upgrade_actor,k1);
        }
#undef PICK_OF
        return;
    }
    if (g->phase==PAUSED && !g->limit_mode && (in.pressed&(KEY_PICK1|KEY_PICK2|KEY_PICK3))) {
        /* 暂停里按 1 2 3：换人数并重开一局 */
        game_set_party(g,in.pressed&KEY_PICK1 ? 1 : in.pressed&KEY_PICK2 ? 2 : 3);
        restart_run(g);
        return;
    }
    if (in.pressed&KEY_PAUSE) { g->phase=g->phase==PAUSED ? PLAYING : PAUSED; return; }
    if (g->phase!=PLAYING) return;

    t=gclock();
    ++g->ticks; seconds=g->ticks/60;
    if (g->banner) --g->banner;
    if(game_combat_enabled(g)) combat_visual_tick(g);
    for (pi=0;pi<g->player_count;pi++) {               /* 每个玩家：计时器 + 移动（单人 = 原来的顺序） */
        const Input mv=g->player_count>1 ? g->player_input[pi] : in;
        int move_speed;
        player_context(g,pi);
        g->next_xp=required_xp(g);
        if (g->hp<=0) continue;                        /* 倒下的玩家不动（单人时这里不会发生） */
        if (g->invincible) --g->invincible;
        if(!g->limit_mode && g->combat.boss_id && g->campaign.ticks-g->combat.boss_intro_start<g->combat.boss_intro_ticks && g->invincible<2)g->invincible=2;
        if (g->hurt_flash) --g->hurt_flash;
        if (g->muzzle) --g->muzzle;
        if (g->weapon_idle_ticks<1000) ++g->weapon_idle_ticks;
        mx=((mv.held&KEY_RIGHT)!=0)-((mv.held&KEY_LEFT)!=0);
        my=((mv.held&KEY_DOWN)!=0)-((mv.held&KEY_UP)!=0);
        g->moving=mx||my;
        /* Keep movement upgrades monotonic; the story multiplier applies at every rank. */
        move_speed=g->limit_mode?g->speed:story_speed(g->speed);
        if(game_combat_enabled(g)) combat_player_move(g,mv,&mx,&my,&move_speed);
        vx=mx*move_speed; vy=my*move_speed;
        if (mx && my) { vx=vx*181/256; vy=vy*181/256; }
        /* 角色 60x60（队友版 1.5 倍）：离围栏的边距照队友版（左右 31，上 39，下 33） */
        g->x=clamp(g->x+vx,(AX0+31)*FIX,(AX1-31)*FIX);
        g->y=clamp(g->y+vy,(AY0+39)*FIX,(AY1-33)*FIX);
        if (mx) g->facing=mx;
    }
    player_context(g,0);

    spawn_director(g);
    for (i=0;i<MAX_ENEMIES;i++) {
        Enemy *e=&g->enemies[i]; int dx,dy,r,speed;
        if (!e->active) continue;
        if (g->player_count>1) player_context(g,nearest_player(g,e->x,e->y));   /* 追最近的玩家，接触伤害也给他 */
        if (e->hit) --e->hit;
        if(!g->limit_mode && e->attack_anim)--e->attack_anim;
        if (e->spawn) {
            if(g->limit_mode){--e->spawn;continue;}
            e->spawn=clamp(e->spawn_until-g->campaign.ticks,0,600);
            if(e->spawn)continue;
        }
        dx=(g->x-e->x)/FIX; dy=(g->y-e->y)/FIX;
        speed=(e->kind==3 ? 160 : e->kind==4 ? 120 : e->kind==2 ? 115 : e->kind==1 ? 290 : 160)+clamp(seconds/3,0,180);
        if (!g->limit_mode) speed=story_speed(speed);
        if (e->kind==3) {
            int distance2=dx*dx+dy*dy;
            if (distance2<130*130) direction(-dx,-dy,speed,&vx,&vy);
            else if (distance2>210*210) direction(dx,dy,speed,&vx,&vy);
            else direction(-dy,dx,speed/2,&vx,&vy);
            boss_attack(g,e);
        } else if (e->kind==4) {
            int distance2=dx*dx+dy*dy;
            if (distance2<100*100) direction(-dx,-dy,speed,&vx,&vy);
            else if (distance2>170*170) direction(dx,dy,speed,&vx,&vy);
            else direction(-dy,dx,speed/3,&vx,&vy);
            spitter_attack(g,e);
        } else {
            if(!g->limit_mode && e->kind==2)spitter_attack(g,e);
            if(!g->limit_mode && e->kind==1 && (g->ticks+e->id*11)%90<15)speed*=2;
            direction(dx,dy,speed,&vx,&vy);
        }
        e->x+=vx; e->y+=vy;
        /* Mild local separation keeps enemies individually readable.
           Limit mode skips it so a full field of spitters stays a bullet test. */
        if (!g->limit_mode && (g->ticks+i)%3==0) for (j=i+1;j<MAX_ENEMIES && j<i+5;j++) {
            Enemy *other=&g->enemies[j]; int ox,oy,d,sep;
            if (!other->active || other->spawn) continue;
            ox=(e->x-other->x)/FIX; oy=(e->y-other->y)/FIX;
            d=ox*ox+oy*oy; sep=game_enemy_radius(e->kind)+game_enemy_radius(other->kind)-4;
            if (d<sep*sep) {
                if (!ox && !oy) ox=1;
                direction(ox,oy,80,&vx,&vy);
                e->x+=vx; e->y+=vy; other->x-=vx; other->y-=vy;
            }
        }
        e->x=clamp(e->x,(AX0+13)*FIX,(AX1-13)*FIX); e->y=clamp(e->y,(AY0+30)*FIX,(AY1-30)*FIX);   /* 1.5 倍怪物（队友版边距） */
        r=12+game_enemy_radius(e->kind);              /* 角色半径 8 x 1.5 */
        dx=(g->x-e->x)/FIX; dy=(g->y-e->y)/FIX;
        if (!g->invincible && dx*dx+dy*dy<r*r) {
            int touch=g->limit_mode ? 1 : (e->kind==2 || e->kind==3) ? 16 : 10;
            int scaled=g->limit_mode ? 0 : clamp(seconds/120,0,20);
            /* 20 ticks is one-third of a second: a graze is survivable, standing in the crowd is not. */
            if(!g->limit_mode)e->attack_anim=12;
            int hurt=game_passive_damage(g,g->actor_index,incoming_damage(g,touch+scaled));
            g->hp-=hurt;g->invincible=g->limit_mode?20:hurt?36:12;g->hurt_flash=hurt?8:0;if(hurt)g->sfx|=GS_HURT;
            particles(g,g->x,g->y,1,12);
            direction(-dx,-dy,12*FIX,&vx,&vy); e->x+=vx; e->y+=vy;
        }
    }

    player_context(g,0);
    t1=gclock(); GSPLIT(enemies,t1-t); t=t1;
    for (pi=0;pi<g->player_count;pi++) {               /* 每个活着的玩家自动开火 */
        player_context(g,pi);
        if (g->hp<=0) continue;
        if(game_combat_enabled(g)) combat_skill(g,g->player_input[pi]);
        if (g->fire_timer) --g->fire_timer;
        if (!g->limit_mode && !g->fire_timer) shoot(g);
    }
    player_context(g,0);
    for (i=0;i<MAX_BULLETS;i++) {
        Bullet *b=&g->bullets[i];
        if (!b->active) continue;
        if (g->player_count>1) player_context(g,(unsigned)b->owner<(unsigned)g->player_count ? b->owner : 0);   /* 击杀 / 回血归发射者 */
        b->x+=b->vx; b->y+=b->vy;
        if (--b->life<=0 || b->x<(AX0-6)*FIX || b->x>(AX1+6)*FIX || b->y<(AY0-2)*FIX || b->y>(AY1+1)*FIX) {
            b->active=0; continue;                       /* 640x480 时 8..632, 58..444 */
        }
        for (j=0;j<MAX_ENEMIES;j++) {
            Enemy *e=&g->enemies[j]; int dx,dy,r;
            if (!e->active || e->spawn) continue;
            dx=(b->x-e->x)/FIX; dy=(b->y-e->y)/FIX; r=game_enemy_radius(e->kind)+3;
            if (dx*dx+dy*dy<r*r) {
                b->active=0; e->hp-=b->damage; e->hit=5; g->sfx|=GS_HIT;
                e->x+=b->vx/3; e->y+=b->vy/3;
                particles(g,b->x,b->y,0,4);
                if (e->hp<=0) {
                    e->active=0; --g->enemy_count; ++g->kills; g->sfx|=GS_KILL;
                    drop_gem(g,e->x,e->y,e->kind==3 ? 25+5*(seconds/60) : e->kind==4 ? 2 : e->kind==2 ? 3 : 1);
                    particles(g,e->x,e->y,2,9);
                    if (e->kind==3) {
                        ++g->bosses_killed;
                        if (g->hp>0) g->hp=clamp(g->hp+12,0,g->max_hp);
                    } else if (g->kills%15==0 && g->hp>0) g->hp=clamp(g->hp+5,0,g->max_hp);
                }
                if(game_combat_enabled(g) && g->combat.explosive[i]) {
                    combat_blast(g,b->x,b->y,g->actor_index);
                }
                break;
            }
        }
        if (b->active) ++n_player[g->actor_index];
    }
    for (pi=0;pi<g->player_count;pi++) { player_context(g,pi); g->player_active=n_player[pi]; }
    player_context(g,0);
    t1=gclock(); GSPLIT(pbullets,t1-t); t=t1;
    n_hostile=g->hostile_used;                        /* slots above are all free */
    hostile_used=0;
    hostile_pool=n_hostile ? hostile_slot(g,0) : 0;   /* pool is contiguous */
    for (i=0;i<n_hostile;i++) {
        Seed *b=&hostile_pool[i]; int dx,dy;
        if (!b->active) continue;
        b->x+=b->vx; b->y+=b->vy;
        if (g->limit_mode && STRESS_WRAP) {
            /* Stay inside the garden and pile up, so a fast CPU still fills the frame.
               (640x480: x 20/620 +-580, y 72/430 +-350) */
            if (b->x<(AX0+6)*FIX) b->x+=(AX1-AX0-32)*FIX; else if (b->x>(AX1-6)*FIX) b->x-=(AX1-AX0-32)*FIX;
            if (b->y<(AY0+12)*FIX) b->y+=(AY1-AY0-33)*FIX; else if (b->y>(AY1-13)*FIX) b->y-=(AY1-AY0-33)*FIX;
        }
        if (--b->life<=0 || ((!g->limit_mode || !STRESS_WRAP) && (b->x<(AX0-6)*FIX || b->x>(AX1+6)*FIX || b->y<(AY0-2)*FIX || b->y>(AY1+1)*FIX))) {
            release_hostile(g,b,i);
            continue;
        }
        if(!g->limit_mode && game_try_reflect(g,b,i))continue;
        if (g->player_count>1) {                       /* 多人（只在普通模式）：打中最先碰到的活着的玩家 */
            const int hr=((b->style&3)==2 ? 15 : (b->style&3)==3 ? 13 : 11)*3/2;
            int hit=0;
            for (pi=0;pi<g->player_count && !hit;pi++) {
                GamePlayer *p=pi==g->actor_index ? &g->actor : &g->party[pi];
                if (p->hp<=0) continue;
                if ((unsigned)(p->x-b->x+24*FIX)>=48u*FIX || (unsigned)(p->y-b->y+24*FIX)>=48u*FIX) continue;
                dx=(p->x-b->x)/FIX; dy=(p->y-b->y)/FIX;
                if (dx*dx+dy*dy<hr*hr) {
                    const int damage=incoming_damage(g,b->active);           /* active = 伤害 */
                    release_hostile(g,b,i); hit=1;
                    if (!p->invincible) {
                        int hurt=game_passive_damage(g,pi,damage);p->hp-=hurt;p->invincible=hurt?45:12;p->hurt_flash=hurt?10:0;if(hurt)g->sfx|=GS_HURT;
                        particles(g,p->x,p->y,1,10);
                    }
                }
            }
            if (hit) continue;
        } else {
        dx=(g->x-b->x)/FIX; dy=(g->y-b->y)/FIX;
        {   /* 命中半径跟着 1.5 倍美术（队友版：普通模式按弹型 11/13/15，压力 12，再 x1.5） */
            const int hr=(g->limit_mode ? 12 : (b->style&3)==2 ? 15 : (b->style&3)==3 ? 13 : 11)*3/2;
            if (dx*dx+dy*dy<hr*hr) {
                const int damage=incoming_damage(g,b->active);               /* active = 伤害（release 会清 0，先取出来） */
                release_hostile(g,b,i);
                if (!g->invincible) {
                    int hurt=game_passive_damage(g,g->actor_index,damage);g->hp-=hurt;g->invincible=hurt?45:12;g->hurt_flash=hurt?10:0;if(hurt)g->sfx|=GS_HURT;
                    particles(g,g->x,g->y,1,10);
                }
                continue;
            }
        }
        }
        hostile_used=i+1;                             /* highest slot still in flight */
        {   /* draw table: the items a pool walk would draw, in the same order */
            int sx=b->x/FIX-seed_half,sy=b->y/FIX-seed_half;
            /* 完全在屏内（无符号比较同时判 >=0）：常见情况，两次比较；
             * 否则部分可见的也写进表，并计数 n_clip（渲染据此决定能否跳过边界检查） */
            if (((unsigned)sx<=(unsigned)(SPROUT_W-seed_ext) && (unsigned)sy<=(unsigned)(SPROUT_H-seed_ext)) ||
                (sx>-seed_ext && sy>-seed_ext && sx<SPROUT_W && sy<SPROUT_H && ++n_clip)) {
                GfxItem *d=&seed_draw[n_draw++];
                d->x=(int16_t)sx; d->y=(int16_t)sy;
                d->sprite=(unsigned)b->style<(unsigned)game_seed_styles(g) ? b->style : 0;
            }
        }
    }
    g->hostile_used=hostile_used;
    g->seed_draw_n=n_draw; g->seed_draw_clip=n_clip; g->seed_draw_ok=1;
    g->seed_draw_buf=seed_buf;                        /* 新表生效 */
    t1=gclock(); GSPLIT(hostile,t1-t); t=t1;
    for (i=0;i<MAX_GEMS;i++) {
        Gem *gem=&g->gems[i]; int dx,dy,d;
        if (!gem->active) continue;
        if (g->player_count>1) player_context(g,nearest_player(g,gem->x,gem->y));   /* 最近的玩家吸过去 */
        dx=(g->x-gem->x)/FIX; dy=(g->y-gem->y)/FIX; d=dx*dx+dy*dy;
        if (d<14*14) {                                 /* 多人：进全队的经验 */
            if (g->player_count>1) g->team_xp+=gem->value; else g->xp+=gem->value;
            gem->active=0;
        }
        else if (d<g->pickup_radius*g->pickup_radius) {
            direction(dx,dy,4*FIX,&vx,&vy); gem->x+=vx; gem->y+=vy;
        }
    }
    for (i=0;i<MAX_PARTICLES;i++) {
        Particle *p=&g->particles[i];
        if (p->life) { --p->life; p->x+=p->vx; p->y+=p->vy; p->vx=p->vx*235/256; p->vy=p->vy*235/256; }
    }
    player_context(g,0);
    if (g->player_count>1) {                           /* 全队一起升级；全部倒下才失败 */
        for (pi=0;pi<g->player_count;pi++) { player_context(g,pi); if (g->hp<=0) g->hp=0; }
        player_context(g,0);
        if (!game_players_alive(g)) g->phase=DEFEAT;
        else check_team_level(g);
    } else {
        if (g->hp<=0) { g->hp=0; g->phase=DEFEAT; }
        else check_level(g);
    }
    GSPLIT(rest,gclock()-t);
}

void game_tick_coop(Game *g, const Input input[GAME_PLAYERS]) {
    Input shared=input[0];
    int i;
    /* 暂停 / 菜单 / 重来 / 开始：谁按都行；标题 / 暂停里选人数的 1 2 3 也是 */
    const uint32_t any=KEY_PAUSE|KEY_MENU|KEY_RESTART|KEY_START|
        (g->phase==TITLE||g->phase==PAUSED ? KEY_PICK1|KEY_PICK2|KEY_PICK3 : 0u);
    for (i=0;i<GAME_PLAYERS;i++) g->player_input[i]=input[i];
    for (i=1;i<g->player_count;i++) shared.pressed|=input[i].pressed&any;
    player_context(g,0);
    game_tick_core(g,shared);
    if(!g->limit_mode && (g->phase==PLAYING || g->phase==UPGRADE)){
        int lv=g->player_count>1?g->team_level:g->level;
        int scene=game_scene_for_level(lv);
        if(scene>g->campaign.scene)campaign_enter(g,scene);

    }
    player_context(g,0);
}
void game_tick(Game *g, Input in) {
    Input all[GAME_PLAYERS];
    memset(all,0,sizeof all);
    all[0]=in;
    game_tick_coop(g,all);
}

const char *game_upgrade_name(int id) {
    static const char *names[]={"QUICK TRIGGER","HEAVY SEEDS","RUNNING ROOTS","HEARTY HARVEST","GREEN MAGNET","DOUBLE SPROUT"};
    return id>=0&&id<6?names[id]:id>=6&&id<UPGRADE_COUNT?game_class_upgrade_name(0,id):"";
}
const char *game_upgrade_line(int id, int line) {
    static const char *lines[][3]={
        {"SHOOT FASTER","-3 TICKS PER SHOT","MORE SEEDS. LESS WAIT."},
        {"DAMAGE +1","EVERY SEED HITS HARDER","SMALL SEED. BIG IMPACT."},
        {"MOVE SPEED UP","DODGE THE SWARM","KEEP THOSE ROOTS MOVING."},
        {"MAX HEALTH +20","RESTORE 40 HEALTH","A LITTLE EXTRA STAYING POWER."},
        {"PICKUP RANGE +40","COLLECT XP FROM AFAR","BRING THE HARVEST TO YOU."},
        {"PROJECTILES +1","FIRE A WIDER VOLLEY","A LITTLE HELP FROM A FRIEND."}
    };
    return id>=0&&id<6&&line>=0&&line<3?lines[id][line]:id>=6&&id<UPGRADE_COUNT&&line>=0&&line<2?game_class_upgrade_line(0,id,line):"";
}

const char *game_class_upgrade_name(int p,int id){
    static const char *names[3][UPGRADE_COUNT]={
      {"RAPID CANNON","LASER CORE","SWIFT BOOTS","SURVIVAL KIT","CRYSTAL RADAR","FOCUSED VOLLEY","SOLAR OVERLOAD","BEAM EXPANSION","QUICK CHARGE","WIND RUNNER"},
      {"QUICK SLASH","FROST BLADE","BATTLE STEP","IRON HEART","BATTLE MAGNET","BLADE FORCE","GLACIAL WRATH","FROST DOMAIN","BLADE RECHARGE","PERFECT PARRY"},
      {"RUNE FLOW","HOLY LANCE","ASTRAL STEP","LIFE RUNE","STAR MAGNET","STAR SPLIT","STAR COLLAPSE","CONSTELLATION","MANA RECOVERY","ARCANE BARRIER"}};
    return p>=0&&p<3&&id>=0&&id<UPGRADE_COUNT?names[p][id]:"";
}
const char *game_class_upgrade_line(int p,int id,int line){
    if(id==ULT_POWER)return line?"STRONGER BOSS DAMAGE":"ULTIMATE DAMAGE +25%";
    if(id==ULT_AREA)return line?"LOCAL AREA / MAX 4 RANKS":p==1?"FROST RADIUS +10 (MAX 160)":p==2?"BLOOM RADIUS +12 (MAX 180)":"ULTIMATE REACH +12";
    if(id==ULT_HASTE)return line?"MORE ULTIMATE CASTS":"ULTIMATE COOLDOWN -0.6S";
    if(id==PASSIVE)return p==0?(line?"ALWAYS ACTIVE":"MOVE SPEED +45"):p==1?(line?"MORE REFLECTED SHOTS":"PARRY COOLDOWN -0.25S"):(line?"EXTRA SHIELD EVERY 2 RANKS":"SHIELD COOLDOWN -0.5S");
    if(id==POWER)return p==1?(line?"SHORT CIRCULAR STRIKE":"MELEE DAMAGE +4"):p==2?(line?"STRONGER MAGIC":"MAGIC DAMAGE +3"):(line?"STRONGER BULLETS":"SHOT DAMAGE +2");
    if(id==RAPID)return line?"ULTIMATE COOLDOWN -0.2S":"ATTACK FASTER / DMG +1";
    if(id==VITALITY)return line?"RESTORE 40 HEALTH":"MAX HEALTH +20";
    if(id==MULTISHOT)return line?"CONVERGING SHOTS / DMG +2":p==1?"BLADE SHOTS +1 (MAX 4)":p==2?"PROJECTILES +1 (MAX 5)":"PROJECTILES +1 (MAX 6)";
    if(id==BOOTS)return line?"DASH RECHARGE -0.13S":"SPEED UP TO +80 / DASH UP";
    if(id==MAGNET)return line?"PICKUP MAX 320":"PICKUP RANGE +40";
    return "";
}
