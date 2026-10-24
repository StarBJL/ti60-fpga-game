extern unsigned long long probe_slot_checks, probe_alloc_calls;
#include "game.h"
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
    {128,4096,6,10,5,50,720}
};
static const StressSettings *stress(const Game *g) {
    return g->stress_tier>=STRESS_LOW && g->stress_tier<=STRESS_HIGH
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

int game_enemy_radius(int kind) { return kind==3 ? 23 : kind==2 ? 15 : kind==4 ? 9 : kind==1 ? 8 : 10; }

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
    x=30+rand_n(g,580); y=80+rand_n(g,344);
    if (side==0) x=20; else if (side==1) x=620;
    else if (side==2) y=68; else y=432;
    /* The warning ring also protects against unavoidable edge spawns. */
    {
        Enemy *e=&g->enemies[i];
        memset(e,0,sizeof(*e)); e->active=1; e->x=x*FIX; e->y=y*FIX;
        e->kind=kind; e->spawn=kind==3 ? 90 : g->limit_mode ? 8 : 45; e->id=++g->next_enemy_id;
        e->max_hp=e->hp=g->limit_mode && kind==4 ? 100000 : kind==3 ? 160+minutes*60 : (kind==4 ? 6 : kind==2 ? 11 : kind==1 ? 3 : 4)+minutes*2;
        e->attack_timer=kind==4 ? (g->limit_mode ? stress(g)->attack_ticks : 36) : 105;
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
int game_hostile_count(const Game *g) {
    int i,count=0,n=game_hostile_slots(g);
    const Bullet *pool=n ? game_hostile_at(g,0) : 0;
    for (i=0;i<n;i++) count+=pool[i].active!=0;
    return count;
}
int game_dropped_shots(const Game *g) { return g->dropped_shots; }
static int grow_limit_shots(Game *g) {
    const StressSettings *s=stress(g);
    int next=g->limit_shot_cap<256 ? 256 : g->limit_shot_cap*2;
    Bullet *grown;
    const Bullet *old=g->limit_shots ? g->limit_shots : g->hostile;
    int old_cap=g->limit_shots ? g->limit_shot_cap : MAX_HOSTILE_BULLETS;
    if (next<=old_cap) next=old_cap*2;
    if (s && next>s->shots) next=s->shots;
    if (next<=old_cap || !g->config.grow_shots) return 0;
    grown=g->config.grow_shots(g->config.user,old,old_cap,next);
    if (!grown) return 0;
    memset(grown+old_cap,0,(size_t)(next-old_cap)*sizeof(Bullet));
    g->limit_shots=grown; g->limit_shot_cap=next; return 1;
}
static Bullet *take_limit_shot(Game *g) {
    int i,n,start;
    ++probe_alloc_calls;
    Bullet *pool=g->limit_shots ? g->limit_shots : g->hostile;
    n=game_hostile_slots(g);
    if (g->limit_shot_cursor<0 || g->limit_shot_cursor>=n) g->limit_shot_cursor=0;
    start=g->limit_shot_cursor;
    for (i=0;i<n;i++) {
        ++probe_slot_checks;
        int k=start+i; if (k>=n) k-=n;
        if (!pool[k].active) {
            g->limit_shot_cursor=k+1; if (g->limit_shot_cursor>=n) g->limit_shot_cursor=0;
            return &pool[k];
        }
    }
    if (!grow_limit_shots(g)) { ++g->dropped_shots; return 0; }
    g->limit_shot_cursor=n+1; return &g->limit_shots[n];
}
static void hostile_shot(Game *g, const Enemy *e, int vx, int vy, int damage, int style) {
    int i; Bullet *b=0;
    if (g->limit_mode) b=take_limit_shot(g);
    else for (i=0;i<NORMAL_HOSTILE_SLOTS;i++) if (!g->hostile[i].active) { b=&g->hostile[i]; break; }
    if (!b) return;
    b->active=1; b->x=e->x; b->y=e->y; b->vx=vx; b->vy=vy;
    b->life=g->limit_mode ? stress(g)->shot_life : 300; b->damage=damage; b->style=style;
}

static void boss_attack(Game *g, Enemy *e) {
    static const int ring[12][2]={{256,0},{222,128},{128,222},{0,256},{-128,222},{-222,128},
        {-256,0},{-222,-128},{-128,-222},{0,-256},{128,-222},{222,-128}};
    int i,vx,vy,minutes=g->ticks/3600,speed=clamp(600+minutes*20,600,1000);
    int shot=clamp(12+2*minutes,12,40);
    if (--e->attack_timer>0) return;
    ++e->volley;
    if (e->volley%3==0) {
        for (i=0;i<12;i++) hostile_shot(g,e,ring[i][0]*speed/256,ring[i][1]*speed/256,shot,0);
    } else {
        int count=clamp(3+2*(minutes/3),3,7);
        direction((g->x-e->x)/FIX,(g->y-e->y)/FIX,speed,&vx,&vy);
        if (!vx && !vy) vx=speed;
        for (i=0;i<count;i++) {
            int spread=(i-count/2)*65;
            hostile_shot(g,e,vx-vy*spread/256,vy+vx*spread/256,shot,0);
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
    for (i=0;i<count;i++) {
        int spread=(i-count/2)*36;
        int damage=g->limit_mode ? 1 : clamp(7+minutes,7,16);
        int style=g->limit_mode ? 1+(i+e->volley)%g->stress_tier : 1;
        hostile_shot(g,e,vx-vy*spread/256,vy+vx*spread/256,damage,style);
    }
    e->attack_timer=g->limit_mode ? stress(g)->attack_ticks : clamp(75-minutes*2,36,75);
}

static void begin_run(Game *g, uint32_t seed, int tier) {
    int i;
    Bullet *extended=g->limit_shots;
    int extended_cap=g->limit_shot_cap;
    GameConfig config=g->config;
    memset(g,0,sizeof(*g));
    g->limit_shots=extended; g->limit_shot_cap=extended_cap; g->config=config;
    if (extended && extended_cap>0) memset(extended,0,(size_t)extended_cap*sizeof(Bullet));
    g->rng=seed ? seed : 0x12345678u; g->phase=PLAYING;
    g->stress_tier=tier; g->limit_mode=tier!=0;
    g->x=320*FIX; g->y=252*FIX; g->max_hp=g->hp=g->limit_mode ? 999 : 100;
    g->speed=560; g->shots=1; g->damage=2; g->fire_period=17;
    g->pickup_radius=62; g->next_xp=6; g->level=1;
    g->facing=1; g->aim_x=1; g->banner=g->limit_mode ? 180 : 150;
    if (g->limit_mode) {
        int target=stress(g)->enemies<MAX_ENEMIES ? stress(g)->enemies : MAX_ENEMIES;
        for (i=0;i<target;i++) spawn_enemy(g,4);
        g->spawn_timer=stress(g)->spawn_ticks;
    } else {
        for (i=0;i<5;i++) spawn_enemy(g,0);
        g->spawn_timer=30;
    }
}

void game_start(Game *g, uint32_t seed) { begin_run(g,seed,0); }
void game_start_stress(Game *g, uint32_t seed, int tier) {
    begin_run(g,seed,clamp(tier,STRESS_LOW,STRESS_HIGH));
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
    return (id!=MULTISHOT || g->shots<4) && (id!=RAPID || g->fire_period>7)
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
        g->xp-=g->next_xp; ++g->level; g->next_xp=required_xp(g);
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
        case MULTISHOT: g->shots=clamp(g->shots+1,1,4); break;
        default: break;
    }
    g->phase=PLAYING; g->invincible=60; g->banner=90; g->event=EVENT_UPGRADE;
    check_level(g);
}

static int nearest_enemy(Game *g, int x, int y) {
    int i,index=-1,best=300*300;
    for (i=0;i<MAX_ENEMIES;i++) {
        Enemy *e=&g->enemies[i]; int dx,dy,d;
        if (!e->active || e->spawn) continue;
        dx=e->x/FIX-x; dy=e->y/FIX-y; d=dx*dx+dy*dy;
        if (d<best) { best=d; index=i; }
    }
    return index;
}

static void shoot(Game *g) {
    int target=nearest_enemy(g,g->x/FIX,g->y/FIX), i,shot;
    int dx,dy,vx,vy;
    if (target<0) return;
    dx=(g->enemies[target].x-g->x)/FIX; dy=(g->enemies[target].y-g->y)/FIX;
    direction(dx,dy,7*FIX,&vx,&vy);
    if (!vx && !vy) vx=7*FIX;
    g->aim_x=dx; g->aim_y=dy; g->facing=dx<0 ? -1 : 1;
    for (shot=0;shot<g->shots;shot++) {
        int spread=(shot*2-(g->shots-1))*14;
        for (i=0;i<MAX_BULLETS;i++) if (!g->bullets[i].active) break;
        if (i==MAX_BULLETS) break;
        {
            Bullet *b=&g->bullets[i];
            b->active=1; b->x=g->x+vx; b->y=g->y+vy;
            b->vx=vx-vy*spread/256; b->vy=vy+vx*spread/256;
            b->damage=g->damage; b->life=60;
        }
    }
    g->fire_timer=g->fire_period; g->muzzle=4;
}

void game_tick(Game *g, Input in) {
    int i,j,mx,my,vx,vy,seconds,n_hostile;
    Bullet *hostile_pool;
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

    ++g->ticks; seconds=g->ticks/60; g->next_xp=required_xp(g);
    if (g->invincible) --g->invincible;
    if (g->banner) --g->banner;
    if (g->hurt_flash) --g->hurt_flash;
    if (g->muzzle) --g->muzzle;
    mx=((in.held&KEY_RIGHT)!=0)-((in.held&KEY_LEFT)!=0);
    my=((in.held&KEY_DOWN)!=0)-((in.held&KEY_UP)!=0);
    g->moving=mx||my;
    vx=mx*g->speed; vy=my*g->speed;
    if (mx && my) { vx=vx*181/256; vy=vy*181/256; }
    g->x=clamp(g->x+vx,28*FIX,612*FIX);
    g->y=clamp(g->y+vy,82*FIX,422*FIX);
    if (mx) g->facing=mx;

    spawn_director(g);
    for (i=0;i<MAX_ENEMIES;i++) {
        Enemy *e=&g->enemies[i]; int dx,dy,r,speed;
        if (!e->active) continue;
        if (e->hit) --e->hit;
        if (e->spawn) { --e->spawn; continue; }
        dx=(g->x-e->x)/FIX; dy=(g->y-e->y)/FIX;
        speed=(e->kind==3 ? 160 : e->kind==4 ? 120 : e->kind==2 ? 115 : e->kind==1 ? 290 : 160)+clamp(seconds/3,0,180);
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
        } else direction(dx,dy,speed,&vx,&vy);
        e->x+=vx; e->y+=vy;
        /* Mild local separation keeps enemies individually readable.
           Limit mode skips it so a full field of spitters stays a bullet test. */
        if (!g->limit_mode && (g->ticks+i)%3==0) for (j=i+1;j<MAX_ENEMIES;j++) {
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
        e->x=clamp(e->x,22*FIX,618*FIX); e->y=clamp(e->y,80*FIX,424*FIX);
        r=8+game_enemy_radius(e->kind);
        dx=(g->x-e->x)/FIX; dy=(g->y-e->y)/FIX;
        if (!g->invincible && dx*dx+dy*dy<r*r) {
            int touch=g->limit_mode ? 1 : (e->kind==2 || e->kind==3) ? 16 : 10;
            int scaled=g->limit_mode ? 0 : clamp(seconds/120,0,20);
            /* 20 ticks is one-third of a second: a graze is survivable, standing in the crowd is not. */
            g->hp-=touch+scaled; g->invincible=20; g->hurt_flash=8;
            particles(g,g->x,g->y,1,12);
            direction(-dx,-dy,12*FIX,&vx,&vy); e->x+=vx; e->y+=vy;
        }
    }

    if (g->fire_timer) --g->fire_timer;
    if (!g->fire_timer) shoot(g);
    for (i=0;i<MAX_BULLETS;i++) {
        Bullet *b=&g->bullets[i];
        if (!b->active) continue;
        b->x+=b->vx; b->y+=b->vy;
        if (--b->life<=0 || b->x<8*FIX || b->x>632*FIX || b->y<58*FIX || b->y>444*FIX) {
            b->active=0; continue;
        }
        for (j=0;j<MAX_ENEMIES;j++) {
            Enemy *e=&g->enemies[j]; int dx,dy,r;
            if (!e->active || e->spawn) continue;
            dx=(b->x-e->x)/FIX; dy=(b->y-e->y)/FIX; r=game_enemy_radius(e->kind)+3;
            if (dx*dx+dy*dy<r*r) {
                b->active=0; e->hp-=b->damage; e->hit=5;
                e->x+=b->vx/3; e->y+=b->vy/3;
                particles(g,b->x,b->y,0,4);
                if (e->hp<=0) {
                    e->active=0; --g->enemy_count; ++g->kills;
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
    }
    n_hostile=game_hostile_slots(g);
    hostile_pool=n_hostile ? hostile_slot(g,0) : 0;   /* pool is contiguous */
    for (i=0;i<n_hostile;i++) {
        Bullet *b=&hostile_pool[i]; int dx,dy;
        if (!b->active) continue;
        b->x+=b->vx; b->y+=b->vy;
        if (g->limit_mode && STRESS_WRAP) {
            /* Stay inside the garden and pile up, so a fast CPU still fills the frame. */
            if (b->x<20*FIX) b->x+=580*FIX; else if (b->x>620*FIX) b->x-=580*FIX;
            if (b->y<72*FIX) b->y+=350*FIX; else if (b->y>430*FIX) b->y-=350*FIX;
        }
        if (--b->life<=0 || ((!g->limit_mode || !STRESS_WRAP) && (b->x<8*FIX || b->x>632*FIX || b->y<58*FIX || b->y>444*FIX))) {
            b->active=0;
            if (g->limit_mode && i<g->limit_shot_cursor) g->limit_shot_cursor=i;
            continue;
        }
        dx=(g->x-b->x)/FIX; dy=(g->y-b->y)/FIX;
        if (dx*dx+dy*dy<12*12) {
            b->active=0;
            if (g->limit_mode && i<g->limit_shot_cursor) g->limit_shot_cursor=i;
            if (!g->invincible) {
                g->hp-=b->damage; g->invincible=45; g->hurt_flash=10;
                particles(g,g->x,g->y,1,10);
            }
        }
    }
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
}

const char *game_upgrade_name(int id) {
    static const char *names[]={"QUICK TRIGGER","HEAVY SEEDS","RUNNING ROOTS","HEARTY HARVEST","GREEN MAGNET","DOUBLE SPROUT"};
    return id>=0 && id<UPGRADE_COUNT ? names[id] : "";
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
    return id>=0 && id<UPGRADE_COUNT && line>=0 && line<3 ? lines[id][line] : "";
}
