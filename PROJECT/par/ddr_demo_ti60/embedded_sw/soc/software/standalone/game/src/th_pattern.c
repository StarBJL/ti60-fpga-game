/*
 * th_pattern.c - stage 1 of the Touhou-style game: the wave timeline, the
 * fairies' behaviour and the boss's six phases (3 non-spells + 3 spell cards).
 *
 * Every pattern is plain C called once per tick. Bullet counts of rings / fans
 * are multiplied by t->dens (Lunatic 2, Stress th_stress_levels[], x1..x48) inside th_ring / th_fan or
 * explicitly with D(n), so the same patterns scale from playable to "as many as
 * the hardware can draw".
 */
#include "th.h"

#define PX(v)   ((int32_t)(v) * TH_ONE)
#define DEG(d)  ((uint16_t)((int32_t)(d) * 65536 / 360))
#define D(n)    ((n) * t->dens)

/* ---------------------------------------------------------------- fairies */
/* crosses the screen sideways (em.dir = +1 from the left, -1 from the right),
 * one aimed pellet every 40 ticks */
static void f_line(Th *t, ThEnemy *e)
{
    e->vx = e->em.dir * (PX(2) + 64);
    e->vy = (int32_t)((th_sin((uint16_t)(e->timer * 700)) * 160) >> 14);
    if (e->timer % 40 == 20 && e->y > 0)
        th_fan(t, e->x, e->y, 1, th_aim(t, e->x, e->y), DEG(6), PX(3), TB_PELLET, e->color == 0 ? TC_BLUE : TC_RED);
}

/* comes down to em.a, three aimed 3-way orb shots, then leaves */
static void f_dive(Th *t, ThEnemy *e)
{
    if (e->timer < 60) { e->vx = 0; e->vy = (e->em.a - e->y) / 12; return; }
    e->vy = 0;
    if (e->timer < 200) {
        if (e->timer % 50 == 10) th_fan(t, e->x, e->y, 3, th_aim(t, e->x, e->y), DEG(24), PX(3) + 64, TB_ORB, TC_PURPLE);
        return;
    }
    e->vy = -PX(1); e->vx = e->x < PX(TH_PF_W / 2) ? -PX(2) : PX(2);
}

/* comes down, three rings of orbs, leaves */
static void f_ring(Th *t, ThEnemy *e)
{
    if (e->timer < 50) { e->vy = (e->em.a - e->y) / 10; e->vx = 0; return; }
    e->vy = 0;
    if (e->timer < 190) {
        if (e->timer % 45 == 5)
            th_ring(t, e->x, e->y, 16, (uint16_t)th_rand(t), PX(2), TB_ORB, e->em.b);
        return;
    }
    e->vy = PX(2);
}

/* side streams: diagonal flight, rice aimed downwards */
static void f_stream(Th *t, ThEnemy *e)
{
    e->vx = e->em.dir * PX(1); e->vy = PX(2);
    if (e->timer % 30 == 15 && e->y > 0)
        th_fan(t, e->x, e->y, 2, th_aim(t, e->x, e->y), DEG(18), PX(3) + 128, TB_RICE, TC_GREEN);
}

/* big fairy (mid-boss lite): spiral of rice + kunai fans, leaves after 900 ticks */
static void f_big(Th *t, ThEnemy *e)
{
    ThEmitter *m = &e->em;
    int k;
    if (e->timer < 80) { e->vy = (PX(120) - e->y) / 16; e->vx = (PX(TH_PF_W / 2) - e->x) / 16; return; }
    e->vx = (int32_t)((th_sin((uint16_t)(e->timer * 180)) * 256) >> 14);
    e->vy = 0;
    if (e->timer > 900) { e->vy = -PX(2); return; }
    if (e->timer % 4 == 0) {
        for (k = 0; k < D(2); k++) {
            th_fire(t, e->x, e->y, (uint16_t)(m->ang + (uint32_t)k * 65536u / (uint32_t)D(2)), PX(2) + 96, TB_RICE, TC_CYAN);
        }
        m->ang = (uint16_t)(m->ang + DEG(13));
    }
    if (e->timer % 70 == 0)
        th_fan(t, e->x, e->y, 5, th_aim(t, e->x, e->y), DEG(50), PX(3), TB_KUNAI, TC_RED);
}

static ThEnemy *spawn(Th *t, int kind, int color, int x, int y, int hp, ThEnemyFn fn)
{
    return th_spawn_enemy(t, kind, color, PX(x), PX(y), hp, fn);
}

/* ---------------------------------------------------------------- timeline */
void th_stage1_tick(Th *t)
{
    const int s = (int)t->stage_tick;
    ThEnemy *e;
    /* wave A: lines from alternating sides */
    if (s >= 120 && s < 540 && s % 24 == 0) {
        const int left = (s / 24) & 1;
        e = spawn(t, TE_FAIRY, left ? 0 : 1, left ? -16 : TH_PF_W + 16, 50 + (s % 96) / 2, 20, f_line);
        if (e) e->em.dir = left ? 1 : -1;
    }
    /* wave B: diving pairs */
    if (s >= 600 && s < 1000 && s % 60 == 0) {
        e = spawn(t, TE_FAIRY2, 2, 80 + (s % 120), -20, 60, f_dive);
        if (e) e->em.a = PX(90 + (s % 50));
        e = spawn(t, TE_FAIRY2, 2, TH_PF_W - 80 - (s % 120), -20, 60, f_dive);
        if (e) e->em.a = PX(90 + (s % 50));
    }
    /* wave C: ring fairies + side streams */
    if (s >= 1060 && s < 1500 && s % 110 == 0) {
        e = spawn(t, TE_FAIRY2, 3, 64 + (int)(th_rand(t) % 256u), -20, 120, f_ring);
        if (e) { e->em.a = PX(80 + (int)(th_rand(t) % 60u)); e->em.b = (int)(th_rand(t) % TC_COLORS); }
    }
    if (s >= 1100 && s < 1500 && s % 30 == 0) {
        const int left = (s / 30) & 1;
        e = spawn(t, TE_FAIRY, 2, left ? 16 : TH_PF_W - 16, -16, 16, f_stream);
        if (e) e->em.dir = left ? 1 : -1;
    }
    /* big fairy */
    if (s == 1620) {
        e = spawn(t, TE_BIGFAIRY, 0, TH_PF_W / 2, -40, 2500, f_big);
        t->banner = 0;
    }
    if (s > 1700 && s < 2500 && s % 90 == 0) {
        const int left = (s / 90) & 1;
        e = spawn(t, TE_FAIRY, 1, left ? 16 : TH_PF_W - 16, -16, 16, f_stream);
        if (e) e->em.dir = left ? 1 : -1;
    }
    /* wave D: dense lines + dives */
    if (s >= 2550 && s < 3100) {
        if (s % 16 == 0) {
            const int left = (s / 16) & 1;
            e = spawn(t, TE_FAIRY, left ? 0 : 1, left ? -16 : TH_PF_W + 16, 40 + (s % 128) / 2, 20, f_line);
            if (e) e->em.dir = left ? 1 : -1;
        }
        if (s % 70 == 0) {
            e = spawn(t, TE_FAIRY2, 2, 96 + (int)(th_rand(t) % 192u), -20, 60, f_dive);
            if (e) e->em.a = PX(70 + (int)(th_rand(t) % 60u));
        }
    }
    /* boss */
    if (s == 3250) th_spawn_boss(t, 0);
}

/* ---------------------------------------------------------------- boss phases */
#define BX (t->boss.x)
#define BY (t->boss.y)

/* non-spell 1: rotating rings of orbs + aimed rice fans */
static void p_non1(Th *t, ThEmitter *m, int tick)
{
    th_boss_move_default(t, tick);
    if (tick % 40 == 0) {
        th_ring(t, BX, BY, 20, m->ang, PX(2) + 64, TB_ORB, TC_BLUE);
        m->ang = (uint16_t)(m->ang + DEG(7));
    }
    if (tick % 90 == 45) th_fan(t, BX, BY, 5, th_aim(t, BX, BY), DEG(40), PX(3), TB_RICE, TC_RED);
}

/* spell 1: two counter-rotating five-arm star spirals + slow big orbs */
static void p_spell1(Th *t, ThEmitter *m, int tick)
{
    t->boss.tx = PX(TH_PF_W / 2); t->boss.ty = PX(110);
    t->boss.x += (t->boss.tx - t->boss.x) / 16; t->boss.y += (t->boss.ty - t->boss.y) / 16;
    if (tick % 4 == 0) {
        th_ring(t, BX, BY, 5, m->ang, PX(2) + 64, TB_STAR, TC_YELLOW);
        th_ring(t, BX, BY, 5, (uint16_t)(0u - m->ang), PX(2), TB_STAR, TC_CYAN);
        m->ang = (uint16_t)(m->ang + 900);
    }
    if (tick % 120 == 60) th_ring(t, BX, BY, 8, (uint16_t)th_rand(t), PX(1) + 128, TB_BIG, TC_RED);
}

/* non-spell 2: aimed kunai walls at three speeds + random pellet rings */
static void p_non2(Th *t, ThEmitter *m, int tick)
{
    uint16_t a;
    (void)m;
    th_boss_move_default(t, tick);
    if (tick % 50 == 0) {
        a = th_aim(t, BX, BY);
        th_fan(t, BX, BY, 7, a, DEG(50), PX(3), TB_KUNAI, TC_PURPLE);
        th_fan(t, BX, BY, 7, a, DEG(50), PX(3) + 160, TB_KUNAI, TC_PURPLE);
        th_fan(t, BX, BY, 7, a, DEG(50), PX(4) + 64, TB_KUNAI, TC_PURPLE);
    }
    if (tick % 25 == 12) th_ring(t, BX, BY, 12, (uint16_t)th_rand(t), PX(2), TB_PELLET, TC_WHITE);
}

/* spell 2: rings of curving rice (alternating direction) + accelerating rain of big orbs */
static void p_spell2(Th *t, ThEmitter *m, int tick)
{
    int k, n = D(10);
    th_boss_move_default(t, tick);
    if (tick % 8 == 0) {
        const int16_t av = (tick / 8) & 1 ? 150 : -150;
        for (k = 0; k < n; k++) {
            ThBullet *b = th_fire(t, BX, BY, (uint16_t)(m->ang + (uint32_t)k * 65536u / (uint32_t)n), PX(2) + 128,
                                  TB_RICE, (tick / 8) & 1 ? TC_BLUE : TC_CYAN);
            if (b) { b->angv = av; b->timer = 70; }
        }
        m->ang = (uint16_t)(m->ang + DEG(9));
    }
    if (tick % 50 == 25)
        for (k = 0; k < (t->dens > 4 ? 4 : t->dens); k++) {
            ThBullet *b = th_fire(t, PX(16 + (int)(th_rand(t) % (TH_PF_W - 32))), -PX(20), DEG(90), PX(1),
                                  TB_BIG, TC_BLUE);
            if (b) { b->acc = 3; b->timer = 80; }
        }
}

/* non-spell 3: two pellet rings at two speeds + aimed orbs */
static void p_non3(Th *t, ThEmitter *m, int tick)
{
    th_boss_move_default(t, tick);
    if (tick % 24 == 0) {
        th_ring(t, BX, BY, 24, m->ang, PX(2), TB_PELLET, TC_ORANGE);
        th_ring(t, BX, BY, 24, (uint16_t)(m->ang + DEG(7)), PX(3), TB_PELLET, TC_YELLOW);
        m->ang = (uint16_t)(m->ang + DEG(5));
    }
    if (tick % 12 == 6) th_fan(t, BX, BY, 1, th_aim(t, BX, BY), DEG(10), PX(4), TB_ORB, TC_RED);
}

/* spell 3: flower rings that stop, then re-aim at the player as a fan; bubble rings */
static void p_spell3(Th *t, ThEmitter *m, int tick)
{
    int k, n = D(8);
    t->boss.tx = PX(TH_PF_W / 2); t->boss.ty = PX(120);
    t->boss.x += (t->boss.tx - t->boss.x) / 16; t->boss.y += (t->boss.ty - t->boss.y) / 16;
    if (tick % 6 == 0) {
        const int col = (tick / 6) % 7;
        for (k = 0; k < n; k++) {
            ThBullet *b = th_fire(t, BX, BY, (uint16_t)(m->ang + (uint32_t)k * 65536u / (uint32_t)n), PX(3) + 128,
                                  TB_ORB, col);
            if (b) {
                b->acc = -16; b->timer = 50;
                b->flags |= TBF_REDIR | TBF_AIM;
                b->aux_spd = PX(2) + 192;
                b->aux_ang = (uint16_t)((k % 8 - 4) * DEG(6));
            }
        }
        m->ang = (uint16_t)(m->ang + DEG(11));
    }
    if (tick % 180 == 90) th_ring(t, BX, BY, 6, (uint16_t)th_rand(t), PX(1) + 64, TB_BUBBLE, TC_PURPLE);
}

const ThPhase th_s1_phases[] = {
    { 0,                           4000, 40, p_non1,   0 },
    { "星符：双子银河",    6000, 50, p_spell1, 2000000u },
    { 0,                           4000, 40, p_non2,   0 },
    { "雨符：曲折骤雨", 6000, 50, p_spell2, 3000000u },
    { 0,                           4500, 45, p_non3,   0 },
    { "终符：棱镜之花",       7500, 60, p_spell3, 5000000u },
};
const int th_s1_nphases = (int)(sizeof th_s1_phases / sizeof th_s1_phases[0]);
