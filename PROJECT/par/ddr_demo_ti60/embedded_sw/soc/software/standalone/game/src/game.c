#include "game.h"
#include "weapon_pose.h"
#include <string.h>
/* R2 native arena; other scenes retain their own geometry. */
#undef SCREEN_W
#undef SCREEN_H
#define SCREEN_W 1280
#define SCREEN_H 720

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
    {128,MAX_HOSTILE_BULLETS,6,10,5,10,720}
};
static const StressSettings *stress(const Game *g) {
    return g->stress_tier>=STRESS_LOW && g->stress_tier<=STRESS_CUSTOM
        ? &stress_settings[g->stress_tier] : 0;
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

/* Q8 direction table: generated offline, never runtime sin/cos. */
static const int16_t orbit_unit[128][2]={
    {256,0},
    {256,13},
    {255,25},
    {253,38},
    {251,50},
    {248,62},
    {245,74},
    {241,86},
    {237,98},
    {231,109},
    {226,121},
    {220,132},
    {213,142},
    {206,152},
    {198,162},
    {190,172},
    {181,181},
    {172,190},
    {162,198},
    {152,206},
    {142,213},
    {132,220},
    {121,226},
    {109,231},
    {98,237},
    {86,241},
    {74,245},
    {62,248},
    {50,251},
    {38,253},
    {25,255},
    {13,256},
    {0,256},
    {-13,256},
    {-25,255},
    {-38,253},
    {-50,251},
    {-62,248},
    {-74,245},
    {-86,241},
    {-98,237},
    {-109,231},
    {-121,226},
    {-132,220},
    {-142,213},
    {-152,206},
    {-162,198},
    {-172,190},
    {-181,181},
    {-190,172},
    {-198,162},
    {-206,152},
    {-213,142},
    {-220,132},
    {-226,121},
    {-231,109},
    {-237,98},
    {-241,86},
    {-245,74},
    {-248,62},
    {-251,50},
    {-253,38},
    {-255,25},
    {-256,13},
    {-256,0},
    {-256,-13},
    {-255,-25},
    {-253,-38},
    {-251,-50},
    {-248,-62},
    {-245,-74},
    {-241,-86},
    {-237,-98},
    {-231,-109},
    {-226,-121},
    {-220,-132},
    {-213,-142},
    {-206,-152},
    {-198,-162},
    {-190,-172},
    {-181,-181},
    {-172,-190},
    {-162,-198},
    {-152,-206},
    {-142,-213},
    {-132,-220},
    {-121,-226},
    {-109,-231},
    {-98,-237},
    {-86,-241},
    {-74,-245},
    {-62,-248},
    {-50,-251},
    {-38,-253},
    {-25,-255},
    {-13,-256},
    {0,-256},
    {13,-256},
    {25,-255},
    {38,-253},
    {50,-251},
    {62,-248},
    {74,-245},
    {86,-241},
    {98,-237},
    {109,-231},
    {121,-226},
    {132,-220},
    {142,-213},
    {152,-206},
    {162,-198},
    {172,-190},
    {181,-181},
    {190,-172},
    {198,-162},
    {206,-152},
    {213,-142},
    {220,-132},
    {226,-121},
    {231,-109},
    {237,-98},
    {241,-86},
    {245,-74},
    {248,-62},
    {251,-50},
    {253,-38},
    {255,-25},
    {256,-13}
};
/* Recompute the visible arc only after a population/position change.
 * Each enemy's moving waypoint is then staggered across eight updates. */
static void refresh_formation(Game *g){
    unsigned char valid[128];
    int px=g->x/FIX,py=g->y/FIX,dx=px-g->formation_x,dy=py-g->formation_y;
    int i,n=0,run=0,best=0,first=0,rx=g->limit_mode?369:240,ry=g->limit_mode?213:180;
    if(g->formation_count==g->enemy_count&&dx>-12&&dx<12&&dy>-12&&dy<12)return;
    for(i=0;i<MAX_ENEMIES;i++)if(g->enemies[i].active)g->orbit_slot[i]=(uint8_t)n++;
    g->formation_x=px;g->formation_y=py;g->formation_count=n;
    for(i=0;i<128;i++){
        int x=px+orbit_unit[i][0]*rx/256,y=py+orbit_unit[i][1]*ry/256;
        valid[i]=(unsigned)(x-42)<=1196u&&(unsigned)(y-132)<=492u;
    }
    for(i=0;i<256;i++){
        if(valid[i&127]){
            if(run<128)++run;
            if(run>best&&i-run+1<128){best=run;first=i-run+1;}
        }else run=0;
    }
    g->formation_start=first&127;g->formation_span=best==128?128:best>1?best-1:1;
    {int row=g->formation_span>=112?64:g->formation_span>=60?40:24;
     g->formation_lanes=clamp((n+row-1)/row,1,8);}
}
static void set_walk_target(Game *g,int i,int unused){
    Enemy *e=&g->enemies[i];
    int lanes=g->formation_lanes?g->formation_lanes:1,rank=g->orbit_slot[i];
    int lane=rank%lanes,place=rank/lanes,slots=(g->formation_count+lanes-1-lane)/lanes;
    int phase,whole,f,u,v,rx,ry,factor=256-lane*24;
    (void)unused;if(slots<1)slots=1;
    if(g->formation_span==128)phase=place*32768/slots+(g->ticks&4095)*8;
    else{
        int span=g->formation_span,pad=span/8;
        int wave=g->ticks&1023;if(wave>512)wave=1024-wave;
        if(pad>4)pad=4;
        phase=(g->formation_start+pad)*256;
        if(slots>1)phase+=place*(span-2*pad)*256/(slots-1);
        else phase+=(span-2*pad)*128;
        phase+=(wave-256)*pad; /* coherent slow breathing of the entire fan */
    }
    whole=(phase>>8)&127;f=phase&255;
    u=(orbit_unit[whole][0]*(256-f)+orbit_unit[(whole+1)&127][0]*f)/256;
    v=(orbit_unit[whole][1]*(256-f)+orbit_unit[(whole+1)&127][1]*f)/256;
    rx=(g->limit_mode?246:e->kind==3?160:e->kind>=2?135:96)*3/2;
    ry=g->limit_mode?213:rx*3/4;
    rx=rx*factor/256;ry=ry*factor/256;
    g->walk_dx[i]=(int16_t)(u*rx/256);g->walk_dy[i]=(int16_t)(v*ry/256);
}
static void place_on_ring(Game *g,int i,int angle) {
    Enemy *e=&g->enemies[i];
    g->orbit_slot[i]=(uint8_t)(angle&127);
    e->x=(640+orbit_unit[angle&127][0]*405/256)*FIX;
    e->y=(378+orbit_unit[angle&127][1]*222/256)*FIX;
    g->walk_dx[i]=(int16_t)(orbit_unit[angle&127][0]*369/256);
    g->walk_dy[i]=(int16_t)(orbit_unit[angle&127][1]*213/256);
}

static int spawn_enemy(Game *g, int kind) {
    int i, minutes=g->ticks/3600;
    for (i=0;i<MAX_ENEMIES;i++) if (!g->enemies[i].active) break;
    if (i==MAX_ENEMIES) return 0;
    /* The warning ring also protects against unavoidable edge spawns. */
    {
        Enemy *e=&g->enemies[i];
        memset(e,0,sizeof(*e)); e->active=1;
        e->kind=kind; e->spawn=kind==3 ? 90 : g->limit_mode ? 8 : 45; e->id=++g->next_enemy_id;
        e->max_hp=e->hp=g->limit_mode && kind==4 ? 100000 : kind==3 ? 160+minutes*60 : (kind==4 ? 6 : kind==2 ? 11 : kind==1 ? 3 : 4)+minutes*2;
        e->attack_timer=kind==4 ? (g->limit_mode ? stress(g)->attack_ticks : 36) : 105;
        place_on_ring(g,i,(e->id-1)*49);
        ++g->enemy_count;
    }
    return 1;
}

/* Re-evaluate while playing: both survival time and level raise the cost. */
static int required_xp(const Game *g) {
    int level=g->level-1;
    return 6+4*level+level*level/2+2*(g->ticks/1800);
}

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
    if (g->ticks%3600==1800) {
        g->horde_left+=clamp(36+6*(seconds/60),36,120);
        g->horde_timer=0; g->horde_ticks=600;
        g->event=EVENT_HORDE; g->banner=180;
    }
    if (g->ticks%3600==0) ++g->boss_pending;
    /* Retry a pending boss when space opens; regular enemies reserve four slots. */
    if (g->boss_pending && spawn_enemy(g,3)) {
        --g->boss_pending; g->event=EVENT_BOSS; g->banner=210;
    }
    if (g->horde_ticks) --g->horde_ticks;
    limit=clamp(24+12*(seconds/30)+(g->horde_ticks || g->horde_left ? 48 : 0),24,MAX_ENEMIES-4);
    if (g->horde_left && --g->horde_timer<=0) {
        for (i=0;i<4 && g->horde_left && g->enemy_count<limit;i++) {
            int pick=rand_n(g,10);
            if (spawn_enemy(g,pick==0 ? 4 : pick<3 ? 1 : 0)) --g->horde_left;
        }
        g->horde_timer=12;
    }
    if (--g->spawn_timer<=0) {
        int batch=clamp(1+seconds/60,1,6);
        for (i=0;i<batch && g->enemy_count<limit;i++) {
            int kind=seconds>=40 && rand_n(g,5)==0 ? 4 : seconds>=30 && rand_n(g,5)==0 ? 2 : seconds>=15 && rand_n(g,3)==0 ? 1 : 0;
            spawn_enemy(g,kind);
        }
        g->spawn_timer=clamp(42-seconds/4,8,42);
    }
}

int game_hostile_slots(const Game *g) {
    int slots=!g->limit_mode ? NORMAL_HOSTILE_SLOTS : g->limit_shots ? g->limit_shot_cap : MAX_HOSTILE_BULLETS;
    const StressSettings *s=stress(g);
    if(g->stress_tier==STRESS_CUSTOM && slots>g->stress_bullet_target)
        slots=g->stress_bullet_target;
    return s && slots>s->shots ? s->shots : slots;
}
const Bullet *game_hostile_at(const Game *g, int index) {
    if (index<0 || index>=game_hostile_slots(g)) return 0;
    if (g->limit_mode && g->limit_shots) return &g->limit_shots[index];
    return &g->hostile[index];
}
static Bullet *hostile_slot(Game *g, int index) {
    return (Bullet *)game_hostile_at(g, index);
}
int game_hostile_count(const Game *g) { return g->hostile_active; }
int game_player_count(const Game *g) { return g->player_active; }
int game_total_bullets(const Game *g) { return g->hostile_active+g->player_active; }
int game_appearance_level(const Game *g) {
    if(g->limit_mode)return g->stress_tier>=STRESS_HIGH?7:1;
    return clamp(g->level,1,7);
}
static int pressure_seed_size=13;
int game_seed_size(void){return pressure_seed_size;}
int game_set_seed_size(Game *g,int pixels){
    if(pixels!=1&&pixels!=3&&pixels!=6&&pixels!=9&&pixels!=13&&pixels!=17&&pixels!=25)return 0;
    pressure_seed_size=pixels;g->seed_draw_ok=0;g->occlusion_near_n=0;
    g->occlusion_near_overflow=1;return 1;
}
int game_seed_half(const Game *g) { return g->limit_mode?pressure_seed_size/2:GAME_NORMAL_SEED_HALF; }
int game_seed_extent(const Game *g) { return g->limit_mode?pressure_seed_size:2*GAME_NORMAL_SEED_HALF+1; }
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
    Bullet *grown;
    const Bullet *old=g->limit_shots ? g->limit_shots : g->hostile;
    int old_cap=g->limit_shots ? g->limit_shot_cap : MAX_HOSTILE_BULLETS;
    if (next<=old_cap) next=old_cap*2;
    if (s && next>s->shots) next=s->shots;
    if (next>GAME_HOSTILE_INDEX_SLOTS) next=GAME_HOSTILE_INDEX_SLOTS;
    if (next<=old_cap || !g->config.grow_shots) return 0;
    grown=g->config.grow_shots(g->config.user,old,old_cap,next);
    if (!grown) return 0;
    memset(grown+old_cap,0,(size_t)(next-old_cap)*sizeof(Bullet));
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
static void release_hostile(Game *g, Bullet *b, int index) {
    b->active=0;
    g->hostile_free[index>>5]|=1u<<(index&31);
    g->hostile_free_sum[index>>10]|=1u<<((index>>5)&31);
    --g->hostile_active;
}
static Bullet *take_hostile(Game *g) {
    int s,k=-1;
    for (s=0;s<GAME_HOSTILE_SUMS;s++) if (g->hostile_free_sum[s]) {
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
    Bullet *b=take_hostile(g);
    if (!b) return;
    b->active=1; b->x=e->x; b->y=e->y; b->vx=vx; b->vy=vy;
    b->life=g->limit_mode ? stress(g)->shot_life : 300; b->damage=damage; b->style=style;
}

/* Change the firing limit without inventing projectiles. New slots remain
 * free until an enemy fires; lowering the limit retires only excess slots.
 * The current/previous DMA draw tables remain untouched. */
void game_set_stress_bullets(Game *g, int count) {
    int i,cap,used=0,active=0;Bullet *pool;
    if(g->stress_tier!=STRESS_CUSTOM)return;
    cap=g->limit_shots?g->limit_shot_cap:MAX_HOSTILE_BULLETS;
    if(cap>MAX_HOSTILE_BULLETS)cap=MAX_HOSTILE_BULLETS;
    count=clamp(count,1,cap);
    pool=g->limit_shots?g->limit_shots:g->hostile;
    for(i=count;i<g->hostile_used;i++)pool[i].active=0;
    memset(g->hostile_free,0,sizeof g->hostile_free);
    memset(g->hostile_free_sum,0,sizeof g->hostile_free_sum);
    g->stress_bullet_target=count;
    hostile_mark_free(g,0,count);
    for(i=0;i<count;i++)if(pool[i].active){
        ++active;used=i+1;g->hostile_free[i>>5]&=~(1u<<(i&31));
    }
    for(i=0;i<GAME_HOSTILE_WORDS;i++)if(!g->hostile_free[i])
        g->hostile_free_sum[i>>5]&=~(1u<<(i&31));
    g->hostile_active=active;g->hostile_used=used;
    g->seed_draw_ok=0;
}

static void boss_attack(Game *g, Enemy *e) {
    static const int ring[12][2]={{256,0},{222,128},{128,222},{0,256},{-128,222},{-222,128},
        {-256,0},{-222,-128},{-128,-222},{0,-256},{128,-222},{222,-128}};
    int i,vx,vy,minutes=g->ticks/3600,speed=clamp(600+minutes*20,600,1000);
    int shot=clamp(12+2*minutes,12,40);
    if (--e->attack_timer>0) return;
    ++e->volley;
    if (e->volley%3==0) {
        for (i=0;i<12;i++) hostile_shot(g,e,ring[i][0]*speed/256,ring[i][1]*speed/256,shot,2);
    } else {
        int count=clamp(3+2*(minutes/3),3,7);
        direction((g->x-e->x)/FIX,(g->y-e->y)/FIX,speed,&vx,&vy);
        if (!vx && !vy) vx=speed;
        for (i=0;i<count;i++) {
            int spread=(i-count/2)*65;
            hostile_shot(g,e,vx-vy*spread/256,vy+vx*spread/256,shot,2);
        }
    }
    e->attack_timer=clamp(110-minutes*5,50,110);
}

/* Keeps a gap and fires one aimed seed. Contact is backup, not the main threat. */
static void spitter_attack(Game *g, Enemy *e) {
    int vx,vy,minutes=g->ticks/3600,i,count;
    int speed=g->limit_mode ? 300 : clamp(420+minutes*12,420,640);
    if (--e->attack_timer>0) return;
    direction((g->x-e->x)/FIX,(g->y-e->y)/FIX,speed,&vx,&vy);
    if (!vx && !vy) vx=speed;
    ++e->volley;
    count=g->limit_mode ? stress(g)->volley : 1;
    if(g->stress_tier==STRESS_CUSTOM){
        /* 128 emitters, ten-tick volleys. Scale the fan with the requested
         * pool limit; bounded spread keeps higher loads from firing faster
         * sideways. Every bullet still starts at this enemy's position. */
        count=(g->stress_bullet_target+1279)/1280;
        int free_slots=game_hostile_slots(g)-g->hostile_active;
        if(count>free_slots)count=free_slots;
    }
    for (i=0;i<count;i++) {
        int spread=(i-count/2)*36;
        if(g->stress_tier==STRESS_CUSTOM)
            spread=count>1?((2*i-count+1)*180)/(count-1):0;
        int damage=g->limit_mode ? 1 : clamp(7+minutes,7,16);
        int style=g->limit_mode ? 1+(i+e->volley)%(g->stress_tier==STRESS_CUSTOM?3:g->stress_tier) : 1;
        hostile_shot(g,e,vx-vy*spread/256,vy+vx*spread/256,damage,style);
    }
    e->attack_timer=g->limit_mode ? stress(g)->attack_ticks : clamp(75-minutes*2,36,75);
}

/* Ordinary enemies fire one blue shot; brutes use a purple three-shot fan. */
static void small_attack(Game *g,Enemy *e) {
    int vx,vy,k,n=e->kind==2?3:1;
    if(--e->attack_timer>0)return;
    direction((g->x-e->x)/FIX,(g->y-e->y)/FIX,e->kind==2?450:370,&vx,&vy);
    if(!vx&&!vy)vx=370;
    for(k=0;k<n;k++){
        int spread=(k-n/2)*95;
        hostile_shot(g,e,vx-vy*spread/256,vy+vx*spread/256,e->kind==2?12:6,e->kind==2?3:1);
    }
    e->attack_timer=e->kind==2?100:160+(e->id&31);
}

static void begin_run(Game *g, uint32_t seed, int tier) {
    int i;
    int previous_target=g->stress_tier==STRESS_CUSTOM?g->stress_bullet_target:0;
    Bullet *extended=g->limit_shots;
    int extended_cap=g->limit_shot_cap;
    GameConfig config=g->config;
    memset(g,0,sizeof(*g));
    g->limit_shots=extended; g->limit_shot_cap=extended_cap; g->config=config;
    if (extended && extended_cap>0) memset(extended,0,(size_t)extended_cap*sizeof(Bullet));
    g->rng=seed ? seed : 0x12345678u; g->phase=PLAYING;
    g->stress_tier=tier; g->limit_mode=tier!=0;
    g->stress_bullet_target=tier==STRESS_CUSTOM && previous_target>0?
        previous_target:MAX_HOSTILE_BULLETS;
    hostile_mark_free(g,0,game_hostile_slots(g));   /* index: every slot of this mode is free */
    g->x=640*FIX; g->y=378*FIX; g->max_hp=g->hp=g->limit_mode ? 999 : 100;
    g->speed=560; g->shots=1; g->damage=2; g->fire_period=17;
    g->pickup_radius=62; g->next_xp=6; g->level=1;
    if(tier>=STRESS_HIGH){g->shots=4;g->fire_period=9;g->level=7;g->next_xp=required_xp(g);}
    g->facing=1; g->aim_x=1; g->banner=g->limit_mode ? 180 : 150;
    g->weapon_idle_ticks=1000;
    if (g->limit_mode) {
        int target=stress(g)->enemies<MAX_ENEMIES ? stress(g)->enemies : MAX_ENEMIES;
        for (i=0;i<target;i++) { spawn_enemy(g,4); place_on_ring(g,i,i*128/target); }
        g->spawn_timer=stress(g)->spawn_ticks;
        if(tier==STRESS_CUSTOM)for(i=0;i<target;i++)
            g->enemies[i].attack_timer=1+i%stress(g)->attack_ticks;
    } else {
        for (i=0;i<5;i++) { spawn_enemy(g,0); place_on_ring(g,i,i*128/5); }
        g->spawn_timer=30;
    }
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
    return (id!=MULTISHOT || g->shots<6) && (id!=RAPID || g->fire_period>7)
        && (id!=BOOTS || g->speed<950) && (id!=MAGNET || g->pickup_radius<190);
}

static void offer_upgrades(Game *g) {
    int i,j,id,valid[UPGRADE_COUNT],count=0;
    for (i=0;i<UPGRADE_COUNT;i++) if (upgrade_allowed(g,i)) valid[count++]=i;
    for (i=count-1;i>0;i--) {
        j=rand_n(g,i+1); id=valid[i]; valid[i]=valid[j]; valid[j]=id;
    }
    for (i=0;i<3;i++) g->choices[i]=valid[i%count];
    g->phase=UPGRADE;
}

static void check_level(Game *g) {
    if (g->xp>=g->next_xp) {
        g->xp-=g->next_xp; ++g->level; g->sfx|=GS_LEVEL; g->next_xp=required_xp(g);
        /* Milestones increase the normal volley; the active skill is separate. */
        if((g->level==3||g->level==5||g->level==7)&&g->shots<6)++g->shots;
        g->hp=clamp(g->hp+7,0,g->max_hp); offer_upgrades(g);
    }
}

void game_choose_upgrade(Game *g, int slot) {
    int id;
    if (g->phase!=UPGRADE || slot<0 || slot>2) return;
    id=g->choices[slot]; ++g->upgrades[id];
    switch (id) {
        case RAPID: g->fire_period=clamp(g->fire_period-3,7,60); break;
        case POWER: ++g->damage; break;
        case BOOTS: g->speed=clamp(g->speed+80,0,950); break;
        case VITALITY: g->max_hp+=20; g->hp=clamp(g->hp+40,0,g->max_hp); break;
        case MAGNET: g->pickup_radius=clamp(g->pickup_radius+40,0,190); break;
        case MULTISHOT: g->shots=clamp(g->shots+1,1,6); break;
        case NOVA: break; /* skill damage/cooldown read the upgrade rank */
        default: break;
    }
    g->phase=PLAYING; g->invincible=60; g->banner=90; g->event=EVENT_UPGRADE;
    check_level(g);
}

static int nearest_enemy(Game *g, int x, int y) {
    int i,index=-1,best=450*450;
    for (i=0;i<MAX_ENEMIES;i++) {
        Enemy *e=&g->enemies[i]; int dx,dy,d;
        if (!e->active || e->spawn) continue;
        dx=e->x/FIX-x; dy=e->y/FIX-y; d=dx*dx+dy*dy;
        if (d<best) { best=d; index=i; }
    }
    return index;
}

/* One of eight cached laser drawings. Decide once when the shot is fired;
 * the render loop does not need a per-bullet direction calculation. */
static int bullet_heading(int vx,int vy) {
    int ax=vx<0?-vx:vx,ay=vy<0?-vy:vy;
    if(ay*2<=ax) return vx>=0?0:4;
    if(ax*2<=ay) return vy>=0?2:6;
    if(vx>=0) return vy>=0?1:7;
    return vy>=0?3:5;
}

static void shoot(Game *g) {
    int target=nearest_enemy(g,g->x/FIX,g->y/FIX), i,shot,emitted=0;
    int dx,dy,vx,vy;
    if(target<0){
        if(!g->limit_mode)return;
        /* Keep stress player shots visible even when the ring is beyond aim range. */
        dx=g->aim_x;dy=g->aim_y;
    }else{dx=(g->enemies[target].x-g->x)/FIX;dy=(g->enemies[target].y-g->y)/FIX;}
    direction(dx,dy,7*FIX,&vx,&vy);
    if (!vx && !vy) vx=7*FIX;
    g->aim_x=dx; g->aim_y=dy; g->facing=dx<0 ? -1 : 1;
    g->weapon_heading=weapon_heading_for(dx,dy);
    g->weapon_flash_mask=0;
    for (shot=0;shot<g->shots;shot++) {
        int spread=(shot*2-(g->shots-1))*14;
        for (i=0;i<MAX_BULLETS;i++) if (!g->bullets[i].active) break;
        if (i==MAX_BULLETS) break;
        {
            Bullet *b=&g->bullets[i];
            int gun=g->shots==1?(g->weapon_burst&1):(shot&1);
            WeaponPose p=weapon_pose(g,gun,0);
            b->active=1; b->x=p.mx*FIX; b->y=p.my*FIX;
            b->vx=vx-vy*spread/256; b->vy=vy+vx*spread/256;
            b->damage=g->damage; b->life=60;
            b->style=bullet_heading(b->vx,b->vy);
            g->weapon_flash_mask|=1<<gun;++emitted;
        }
    }
    g->fire_timer=g->fire_period; g->muzzle=4;
    if(emitted){g->sfx|=GS_SHOT;++g->weapon_burst;g->weapon_idle_ticks=0;}
}

static void cast_nova(Game *g) {
    int i,k,free_slots=0,count=g->level>=6?32:16;
    if(g->skill_cooldown)return;
    for(i=0;i<MAX_BULLETS;i++)if(!g->bullets[i].active)++free_slots;
    /* Never consume cooldown for a truncated ring when the existing pool is full. */
    if(free_slots<count)return;
    for(k=0,i=0;k<count;k++){
        int a=k*128/count;
        Bullet *b;
        while(g->bullets[i].active)++i;
        b=&g->bullets[i++];
        b->active=1;b->x=g->x;b->y=g->y;
        b->vx=orbit_unit[a][0]*8;b->vy=orbit_unit[a][1]*8;
        b->damage=g->damage*(4+g->upgrades[NOVA])+g->level;
        b->life=55;b->style=bullet_heading(b->vx,b->vy);
    }
    g->skill_cooldown=clamp(480-g->upgrades[NOVA]*45,240,480);
    g->skill_flash=24;++g->skill_casts;g->sfx|=GS_SKILL;
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

void game_tick(Game *g, Input in) {
    int i,j,mx,my,vx,vy,seconds,n_hostile,hostile_used,n_draw=0,n_clip=0,n_player=0;
    const int seed_half=game_seed_half(g),seed_size=game_seed_extent(g);
    Bullet *hostile_pool;
    const int seed_buf=(g->seed_draw_buf+1)%3;
    GfxItem *seed_draw=g->seed_draw[seed_buf];   /* 写另一张，上一帧的表引擎可能还在读 */
    uint32_t t,t1;
    ++g->visual_ticks;
    if (in.pressed&KEY_MENU) { return_to_title(g); return; }
    if (in.pressed&KEY_RESTART) { restart_run(g); return; }
    if (g->phase==TITLE) {
        if (in.pressed&KEY_LIMIT) g->phase=STRESS_SELECT;
        else if (in.pressed&KEY_START) game_start(g,0xC0FFEEu);
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
    if (g->phase==DEFEAT) {
        if (in.pressed&KEY_START) restart_run(g);
        return;
    }
    if (g->phase==UPGRADE) {
        if (in.pressed&KEY_PICK1) game_choose_upgrade(g,0);
        else if (in.pressed&KEY_PICK2) game_choose_upgrade(g,1);
        else if (in.pressed&KEY_PICK3) game_choose_upgrade(g,2);
        return;
    }
    if (in.pressed&KEY_PAUSE) { g->phase=g->phase==PAUSED ? PLAYING : PAUSED; return; }
    if (g->phase!=PLAYING) return;

    t=gclock();
    ++g->ticks; seconds=g->ticks/60; g->next_xp=required_xp(g);
    if (g->invincible) --g->invincible;
    if (g->banner) --g->banner;
    if (g->hurt_flash) --g->hurt_flash;
    if (g->muzzle) --g->muzzle;
    if(g->weapon_idle_ticks<1000)++g->weapon_idle_ticks;
    if(g->skill_cooldown)--g->skill_cooldown;
    if(g->skill_flash)--g->skill_flash;
    mx=((in.held&KEY_RIGHT)!=0)-((in.held&KEY_LEFT)!=0);
    my=((in.held&KEY_DOWN)!=0)-((in.held&KEY_UP)!=0);
    g->moving=mx||my;
    vx=mx*g->speed; vy=my*g->speed;
    if (mx && my) { vx=vx*181/256; vy=vy*181/256; }
    g->x=clamp(g->x+vx,GAME_PLAYER_MIN_X*FIX,GAME_PLAYER_MAX_X*FIX);
    /* One common 60px canvas; the body centre is the gameplay/collision point. */
    g->y=clamp(g->y+vy,GAME_PLAYER_MIN_Y*FIX,GAME_PLAYER_MAX_Y*FIX);
    if (mx) g->facing=mx;

    spawn_director(g);
    refresh_formation(g);
    for (i=0;i<MAX_ENEMIES;i++) {
        Enemy *e=&g->enemies[i]; int dx,dy,r,speed;
        if (!e->active) continue;
        if (e->hit) --e->hit;
        if (e->spawn) { if(e->spawn==1)set_walk_target(g,i,0);--e->spawn;continue; }
        dx=(g->x-e->x)/FIX; dy=(g->y-e->y)/FIX;
        speed=(e->kind==3 ? 160 : e->kind==4 ? 120 : e->kind==2 ? 115 : e->kind==1 ? 290 : 160)+clamp(seconds/3,0,180);
        /* Follow a player-centred ring, or the visible inward-facing fan near edges. */
        if(((g->ticks+i)&7)==0) set_walk_target(g,i,0);
        {
            int cx=g->x/FIX;
            int cy=g->y/FIX;
            int tx=clamp(cx+g->walk_dx[i],42,1238),ty=clamp(cy+g->walk_dy[i],132,627);
            /* Runners occasionally charge; ranged enemies keep their own sector. */
            if(!g->limit_mode&&e->kind==1&&((g->ticks+e->id*19)&255)<48){tx=g->x/FIX;ty=g->y/FIX;}
            direction(tx-e->x/FIX,ty-e->y/FIX,speed,&vx,&vy);
        }
        if(e->kind==3)boss_attack(g,e);
        else if(e->kind==4)spitter_attack(g,e);
        else if(!g->limit_mode)small_attack(g,e);
        e->x+=vx; e->y+=vy;
        /* Multiple radial lanes allow limited overlap without O(N^2) repulsion. */
        e->x=clamp(e->x,33*FIX,1247*FIX); e->y=clamp(e->y,120*FIX,636*FIX);
        r=12+game_enemy_radius(e->kind);
        dx=(g->x-e->x)/FIX; dy=(g->y-e->y)/FIX;
        if (!g->invincible && dx*dx+dy*dy<r*r) {
            int touch=g->limit_mode ? 1 : (e->kind==2 || e->kind==3) ? 16 : 10;
            int scaled=g->limit_mode ? 0 : clamp(seconds/120,0,20);
            /* 20 ticks is one-third of a second: a graze is survivable, standing in the crowd is not. */
            g->hp-=touch+scaled; g->invincible=20; g->hurt_flash=8; g->sfx|=GS_HURT;
            particles(g,g->x,g->y,1,12);
            direction(-dx,-dy,12*FIX,&vx,&vy); e->x+=vx; e->y+=vy;
        }
    }

    t1=gclock(); GSPLIT(enemies,t1-t); t=t1;
    /* Pressure measures hostile objects only: no player shots, damage to
     * emitters, kill particles, gems or skill volleys. Normal gameplay retains
     * automatic dual guns and the active skill. Starting stress resets the pool. */
    if (!g->limit_mode) {
    if (g->fire_timer) --g->fire_timer;
    if (!g->fire_timer) shoot(g);
    if(in.pressed&KEY_SKILL)cast_nova(g);
    for (i=0;i<MAX_BULLETS;i++) {
        Bullet *b=&g->bullets[i];
        if (!b->active) continue;
        b->x+=b->vx; b->y+=b->vy;
        if (--b->life<=0 || b->x<16*FIX || b->x>1264*FIX || b->y<87*FIX || b->y>666*FIX) {
            b->active=0; continue;
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
                break;
            }
        }
        if(b->active)++n_player;
    }
    }
    g->player_active=n_player;
    t1=gclock(); GSPLIT(pbullets,t1-t); t=t1;
    n_hostile=g->hostile_used;                        /* slots above are all free */
    hostile_used=0;
    const int player_x=g->x,player_y=g->y,pressure=g->limit_mode;
    const int near_left=player_x/FIX-CARROT_LOC_X-seed_size+1;
    const int near_top=player_y/FIX-CARROT_LOC_Y-1-seed_size+1;
    const unsigned near_width=CARROT_LOC_W+seed_size-1,near_height=CARROT_LOC_H+1+seed_size-1;
    g->occlusion_near_n=g->occlusion_near_overflow=0;
    int can_hit=!g->invincible;
    hostile_pool=n_hostile ? hostile_slot(g,0) : 0;   /* pool is contiguous */
    for (i=0;i<n_hostile;i++) {
        Bullet *b=&hostile_pool[i]; int dx,dy;
        if (!b->active) continue;
        b->x+=b->vx; b->y+=b->vy;
        if (pressure && STRESS_WRAP) {
            /* Stay inside the garden and pile up, so a fast CPU still fills the frame. */
            if (b->x<30*FIX) b->x+=1220*FIX; else if (b->x>1250*FIX) b->x-=1220*FIX;
            if (b->y<108*FIX) b->y+=537*FIX; else if (b->y>645*FIX) b->y-=537*FIX;
        }
        if (--b->life<=0 || ((!pressure || !STRESS_WRAP) && (b->x<16*FIX || b->x>1264*FIX || b->y<87*FIX || b->y>666*FIX))) {
            release_hostile(g,b,i);
            continue;
        }
        /* Touhou semantics: invulnerability suppresses damage, not the bullet.
         * Reject distant bullets in fixed point before division/squared distance.
         * 24px covers every scaled hit radius incl. truncation toward zero;
         * damage radii and real projectile counts are unchanged. */
        if (can_hit &&
            (unsigned)(player_x-b->x+24*FIX)<48u*FIX &&
            (unsigned)(player_y-b->y+24*FIX)<48u*FIX) {
            int hit_radius=(pressure?12:b->style==2?15:b->style==3?13:11)*3/2;
            dx=(player_x-b->x)/FIX; dy=(player_y-b->y)/FIX;
            if (dx*dx+dy*dy<hit_radius*hit_radius) {
                g->hp-=b->damage; g->invincible=45; g->hurt_flash=10; g->sfx|=GS_HURT;
                particles(g,player_x,player_y,1,10);
                can_hit=0;
            }
        }
        hostile_used=i+1;                             /* highest slot still in flight */
        {   /* draw table: the items a pool walk would draw, in the same order */
            int sx=b->x/FIX-seed_half,sy=b->y/FIX-seed_half;
            /* 完全在屏内（无符号比较同时判 >=0）：常见情况，两次比较；
             * 否则部分可见的也写进表，并计数 n_clip（渲染据此决定能否跳过边界检查） */
            if (((unsigned)sx<=(unsigned)(SCREEN_W-seed_size) && (unsigned)sy<=(unsigned)(SCREEN_H-seed_size)) ||
                (sx>-seed_size && sy>-seed_size && sx<SCREEN_W && sy<SCREEN_H && ++n_clip)) {
                GfxItem *d=&seed_draw[n_draw++];
                d->x=(int16_t)sx; d->y=(int16_t)sy;
                d->sprite=(unsigned)b->style<GAME_SEED_STYLES ? b->style : 0;
                if((unsigned)(sx-near_left)<near_width&&(unsigned)(sy-near_top)<near_height){
                    if(g->occlusion_near_n<GAME_OCCLUSION_NEAR_MAX)
                        g->occlusion_near[g->occlusion_near_n++]=(uint32_t)(n_draw-1);
                    else g->occlusion_near_overflow=1;
                }
            }
        }
    }
    g->hostile_used=hostile_used;
    g->seed_draw_n=n_draw; g->seed_draw_clip=n_clip; g->seed_draw_ok=1;
    g->seed_draw_buf=seed_buf;          /* 新表生效 */
    g->occlusion_x=player_x/FIX;g->occlusion_y=player_y/FIX;g->occlusion_half=seed_half;
    t1=gclock(); GSPLIT(hostile,t1-t); t=t1;
    for (i=0;i<MAX_GEMS;i++) {
        Gem *gem=&g->gems[i]; int dx,dy,d;
        if (!gem->active) continue;
        dx=(g->x-gem->x)/FIX; dy=(g->y-gem->y)/FIX; d=dx*dx+dy*dy;
        if (d<14*14) { g->xp+=gem->value; gem->active=0; }
        else if (d<g->pickup_radius*g->pickup_radius) {
            direction(dx,dy,4*FIX,&vx,&vy); gem->x+=vx; gem->y+=vy;
        }
    }
    for (i=0;i<MAX_PARTICLES;i++) {
        Particle *p=&g->particles[i];
        if (p->life) { --p->life; p->x+=p->vx; p->y+=p->vy; p->vx=p->vx*235/256; p->vy=p->vy*235/256; }
    }
    if (g->hp<=0) { g->hp=0; g->phase=DEFEAT; }
    else check_level(g);
    GSPLIT(rest,gclock()-t);
}

const char *game_upgrade_name(int id) {
    static const char *names[]={"快速射击","重型弹头","轻盈步伐","生命成长","经验磁力","多重射击","环形爆发"};
    return id>=0 && id<UPGRADE_COUNT ? names[id] : "";
}
const char *game_upgrade_line(int id, int line) {
    static const char *lines[][3]={
        {"提升射速","射击间隔减少","更快出手"},
        {"伤害 +1","每发伤害提升","小弹头大威力"},
        {"提升移动速度","灵活躲避敌群","保持走位"},
        {"生命上限 +20","恢复生命 40","顽强生存"},
        {"拾取范围 +40","远处也能收集经验","自动吸引经验"},
        {"普通弹数 +1","自动射击更宽","升级获得更多弹头"},
        {"技能威力提升","冷却时间缩短","按 J 主动释放"}
    };
    return id>=0 && id<UPGRADE_COUNT && line>=0 && line<3 ? lines[id][line] : "";
}
