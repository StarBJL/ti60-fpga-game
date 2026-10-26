/*
 * th_stage2.c - stage 2 "月下云海": the moon princess.
 *
 * New here (see th_game.c / th_render.c):
 *   - phantom bullets (th_phantom): their transparency pulses, they only hit /
 *     graze while visible; one alpha copy each, so a few hundred at most;
 *   - lasers (th_laser): BEAM (aimed or turning, translucent dotted warning line,
 *     then additive glow), PILLAR (vertical moonlight column, alpha body +
 *     additive core, may sweep sideways), COMET (falling star with a tail);
 *   - wisps (TE_WISP): translucent ghost flames (A8 sprites) that fade in;
 *   - a mid-boss (the stage clock stops while it is up), the full moon rising
 *     behind the last spell card (t->moon_t).
 * Ring / fan bullet counts scale with t->dens like stage 1; phantoms and lasers
 * do not (stress mode would only multiply alpha commands).
 */
#include "th.h"

#define PX(v)   ((int32_t)(v) * TH_ONE)
#define DEG(d)  ((uint16_t)((int32_t)(d) * 65536 / 360))
#define D(n)    ((n) * t->dens)
#define BX (t->boss.x)
#define BY (t->boss.y)

/* phantom ring: n phantoms (x2 on Lunatic / Stress), visibility phase shifted
 * along the ring so the ring "flickers" around */
static void phantom_ring(Th *t, int32_t x, int32_t y, int n, uint16_t a0, int spd, int kind, int color, uint16_t ph0)
{
    int i;
    if (t->dens > 1) n *= 2;
    for (i = 0; i < n; i++)
        th_phantom(t, x, y, (uint16_t)(a0 + (uint32_t)i * 65536u / (uint32_t)n), spd, kind, color,
                   (uint16_t)(ph0 + (uint32_t)i * 4096u), 900);
}

/* ---------------------------------------------------------------- enemies */
/* wisp: fades in at the side, drifts across on a wave, phantom shots at the player */
static void w_float(Th *t, ThEnemy *e)
{
    e->vx = e->em.dir * (PX(1) + 128);
    e->vy = (int32_t)((th_sin((uint16_t)(e->timer * 600)) * 200) >> 14);
    if (e->timer % 50 == 25 && e->x > 0 && e->x < PX(TH_PF_W))
        th_phantom(t, e->x, e->y, th_aim(t, e->x, e->y), PX(2) + 64, TB_ORB, (e->color & 1) ? TC_CYAN : TC_BLUE,
                   (uint16_t)(e->timer * 997u), 1100);
}

/* wisp that stops and blows a phantom ring, then rises away */
static void w_ring(Th *t, ThEnemy *e)
{
    if (e->timer < 40) { e->vx = 0; e->vy = (e->em.a - e->y) / 10; return; }
    e->vy = 0;
    if (e->timer < 160) {
        if (e->timer % 60 == 45) phantom_ring(t, e->x, e->y, 12, (uint16_t)th_rand(t), PX(2), TB_ORB, TC_PURPLE, 0);
        return;
    }
    e->vy = -PX(1);
}

/* laser fairy: comes down, aimed beam after a warning, leaves */
static void f_laser(Th *t, ThEnemy *e)
{
    if (e->timer < 50) { e->vx = 0; e->vy = (e->em.a - e->y) / 10; return; }
    e->vy = 0;
    if (e->timer == 60) th_laser(t, TL_BEAM, e->x, e->y, th_aim(t, e->x, e->y), 520, 14, 45, 40, e->em.b);
    if (e->timer == 60 && t->dens > 1) {
        const uint16_t a = th_aim(t, e->x, e->y);
        th_laser(t, TL_BEAM, e->x, e->y, (uint16_t)(a + DEG(25)), 520, 10, 45, 40, e->em.b);
        th_laser(t, TL_BEAM, e->x, e->y, (uint16_t)(a - DEG(25)), 520, 10, 45, 40, e->em.b);
    }
    if (e->timer % 30 == 0 && e->timer > 60 && e->timer < 150)
        th_fan(t, e->x, e->y, 3, th_aim(t, e->x, e->y), DEG(30), PX(3), TB_PELLET, TC_WHITE);
    if (e->timer > 170) e->vy = -PX(2);
}

/* small fairy crossing the screen, pellets */
static void f_cross(Th *t, ThEnemy *e)
{
    e->vx = e->em.dir * PX(2); e->vy = PX(1) / 2;
    if (e->timer % 36 == 18 && e->y > 0) th_fan(t, e->x, e->y, 2, th_aim(t, e->x, e->y), DEG(14), PX(3), TB_RICE, TC_CYAN);
}

/* meteor fairy: flies along the top, drops comets diagonally + star bullets */
static void f_meteor(Th *t, ThEnemy *e)
{
    e->vx = e->em.dir * (PX(1) + 64); e->vy = 0;
    if (e->timer % 22 == 0 && e->x > 0 && e->x < PX(TH_PF_W)) {
        ThLaser *l = th_laser(t, TL_COMET, e->x, e->y, DEG(90 - e->em.dir * 25), 0, 10, 0, 0,
                              (e->timer / 22) & 1 ? TC_YELLOW : TC_CYAN);
        if (l) { l->vx = (int32_t)((PX(4) * th_cos(l->ang)) >> 14); l->vy = (int32_t)((PX(4) * th_sin(l->ang)) >> 14); }
    }
    if (e->timer % 70 == 35) th_ring(t, e->x, e->y, 8, (uint16_t)th_rand(t), PX(2), TB_STAR, TC_YELLOW);
}

/* big fairy with two beams turning around it, rings of orbs */
static void f_bigl(Th *t, ThEnemy *e)
{
    if (e->timer < 70) { e->vy = (PX(130) - e->y) / 14; e->vx = 0; return; }
    e->vy = 0; e->vx = 0;
    if (e->timer == 80) {
        int k;
        for (k = 0; k < 2; k++) {
            ThLaser *l = th_laser(t, TL_BEAM, e->x, e->y, (uint16_t)(DEG(60) + k * 32768), 460, 18, 50, 380, TC_PURPLE);
            if (l) l->angv = e->em.dir * 110;
        }
    }
    if (e->timer % 50 == 0) th_ring(t, e->x, e->y, 14, (uint16_t)th_rand(t), PX(2), TB_ORB, TC_BLUE);
    if (e->timer > 520) e->vy = -PX(2);
}

static ThEnemy *spawn(Th *t, int kind, int color, int x, int y, int hp, ThEnemyFn fn)
{
    return th_spawn_enemy(t, kind, color, PX(x), PX(y), hp, fn);
}

/* ---------------------------------------------------------------- timeline */
void th_stage2_tick(Th *t)
{
    const int s = (int)t->stage_tick;
    ThEnemy *e;
    /* wisps from both sides */
    if (s >= 120 && s < 600 && s % 30 == 0) {
        const int left = (s / 30) & 1;
        e = spawn(t, TE_WISP, (s / 60) & 3, left ? -12 : TH_PF_W + 12, 60 + (s * 7) % 90, 12, w_float);
        if (e) e->em.dir = left ? 1 : -1;
    }
    /* laser fairies in pairs + crossing fairies */
    if (s >= 640 && s < 1100 && s % 110 == 0) {
        e = spawn(t, TE_FAIRY2, 1, 96, -20, 40, f_laser);
        if (e) { e->em.a = PX(90); e->em.b = TC_CYAN; }
        e = spawn(t, TE_FAIRY2, 1, TH_PF_W - 96, -20, 40, f_laser);
        if (e) { e->em.a = PX(90); e->em.b = TC_CYAN; }
    }
    if (s >= 660 && s < 1100 && s % 40 == 20) {
        const int left = (s / 40) & 1;
        e = spawn(t, TE_FAIRY, 0, left ? -16 : TH_PF_W + 16, 40, 16, f_cross);
        if (e) e->em.dir = left ? 1 : -1;
    }
    if (s == 1150) th_spawn_boss(t, 1);                         /* mid-boss (the clock stops) */
    /* meteor fairies + wisps */
    if (s >= 1200 && s < 1750 && s % 120 == 0) {
        const int left = (s / 120) & 1;
        e = spawn(t, TE_FAIRY, 3, left ? -16 : TH_PF_W + 16, 30, 60, f_meteor);
        if (e) e->em.dir = left ? 1 : -1;
    }
    if (s >= 1230 && s < 1750 && s % 45 == 0) {
        const int left = (s / 45) & 1;
        e = spawn(t, TE_WISP, 2 + ((s / 45) & 1), left ? -12 : TH_PF_W + 12, 120 + (s * 13) % 80, 12, w_float);
        if (e) e->em.dir = left ? 1 : -1;
    }
    /* big laser fairies, ring wisps */
    if (s == 1800 || s == 2100) {
        e = spawn(t, TE_BIGFAIRY, s == 1800 ? 2 : 0, s == 1800 ? 120 : TH_PF_W - 120, -30, 1500, f_bigl);
        if (e) e->em.dir = s == 1800 ? 1 : -1;
    }
    if (s >= 1820 && s < 2400 && s % 60 == 30) {
        e = spawn(t, TE_WISP, 2, 48 + (int)(th_rand(t) % (TH_PF_W - 96)), -16, 24, w_ring);
        if (e) e->em.a = PX(60 + (int)(th_rand(t) % 80u));
    }
    /* final rush */
    if (s >= 2450 && s < 2950) {
        if (s % 80 == 0) {
            e = spawn(t, TE_FAIRY2, 1, 64 + (int)(th_rand(t) % (TH_PF_W - 128)), -20, 40, f_laser);
            if (e) { e->em.a = PX(70 + (int)(th_rand(t) % 60u)); e->em.b = (s / 80) & 1 ? TC_PURPLE : TC_CYAN; }
        }
        if (s % 25 == 0) {
            const int left = (s / 25) & 1;
            e = spawn(t, TE_WISP, (s / 25) & 3, left ? -12 : TH_PF_W + 12, 50 + (s * 7) % 120, 12, w_float);
            if (e) e->em.dir = left ? 1 : -1;
        }
    }
    if (s == 3050) th_spawn_boss(t, 0);
}

/* ---------------------------------------------------------------- mid-boss */
/* crescent arcs of rice + phantom rings */
static void m_non(Th *t, ThEmitter *m, int tick)
{
    th_boss_move_default(t, tick);
    if (tick % 50 == 0) {
        th_fan(t, BX, BY, 9, (uint16_t)(th_aim(t, BX, BY) + m->dir * DEG(20)), DEG(100), PX(2) + 128, TB_RICE, TC_CYAN);
        m->dir = (int16_t)(m->dir ? -m->dir : 1);
    }
    if (tick % 90 == 45) phantom_ring(t, BX, BY, 12, (uint16_t)th_rand(t), PX(2), TB_ORB, TC_PURPLE, 0);
}

/* "月符：银色光柱": columns of moonlight + a rain of pellets */
static void m_spell(Th *t, ThEmitter *m, int tick)
{
    int k;
    (void)m;
    t->boss.tx = PX(TH_PF_W / 2); t->boss.ty = PX(90);
    t->boss.x += (t->boss.tx - t->boss.x) / 16; t->boss.y += (t->boss.ty - t->boss.y) / 16;
    if (tick % 70 == 0)
        for (k = 0; k < 3; k++)
            th_laser(t, TL_PILLAR, PX(32 + (int)(th_rand(t) % (TH_PF_W - 64))), 0, 0, TH_PF_H, 26, 50, 40, TC_CYAN);
    if (tick % 6 == 0)
        for (k = 0; k < D(1); k++)
            th_fire(t, PX(8 + (int)(th_rand(t) % (TH_PF_W - 16))), -PX(8), DEG(85 + (int)(th_rand(t) % 10u)), PX(2) + 64,
                    TB_PELLET, TC_WHITE);
}

/* ---------------------------------------------------------------- boss */
/* non-spell 1: crescents + denser phantom rings */
static void b_non1(Th *t, ThEmitter *m, int tick)
{
    th_boss_move_default(t, tick);
    if (tick % 40 == 0) {
        th_fan(t, BX, BY, 11, (uint16_t)(th_aim(t, BX, BY) + m->dir * DEG(25)), DEG(120), PX(3), TB_RICE, TC_BLUE);
        m->dir = (int16_t)(m->dir ? -m->dir : 1);
    }
    if (tick % 60 == 30) phantom_ring(t, BX, BY, 16, m->ang, PX(2) + 64, TB_ORB, TC_CYAN, (uint16_t)(tick * 300));
    m->ang = (uint16_t)(m->ang + 300);
}

/* "幻符：月之幻影": rings of phantoms whose visibility runs round,
 * aimed kunai walls between them */
static void b_spell1(Th *t, ThEmitter *m, int tick)
{
    th_boss_move_default(t, tick);
    if (tick % 14 == 0) {
        phantom_ring(t, BX, BY, 10, m->ang, PX(2), TB_ORB, (tick / 14) & 1 ? TC_PURPLE : TC_BLUE, (uint16_t)(tick * 1300));
        m->ang = (uint16_t)(m->ang + DEG(7));
    }
    if (tick % 70 == 35) th_fan(t, BX, BY, 7, th_aim(t, BX, BY), DEG(40), PX(3) + 128, TB_KUNAI, TC_WHITE);
}

/* non-spell 2: four beams turning around the boss, pellet spiral */
static void b_non2(Th *t, ThEmitter *m, int tick)
{
    int k;
    th_boss_move_default(t, tick);
    if (tick % 420 == 1) {
        m->dir = (int16_t)(m->dir > 0 ? -1 : 1);
        for (k = 0; k < 4; k++) {
            ThLaser *l = th_laser(t, TL_BEAM, BX, BY, (uint16_t)(m->ang + k * 16384), 520, 16, 40, 360, TC_PURPLE);
            if (l) { l->follow = 1; l->angv = m->dir * 90; }
        }
    }
    if (tick % 5 == 0) {
        th_ring(t, BX, BY, 3, (uint16_t)(m->ang * 3u), PX(2) + 64, TB_PELLET, TC_YELLOW);
        m->ang = (uint16_t)(m->ang + DEG(4));
    }
}

/* "星符：流星雨": comets from the upper left / right, star rings, big orbs */
static void b_spell2(Th *t, ThEmitter *m, int tick)
{
    th_boss_move_default(t, tick);
    if (tick % 6 == 0) {
        const int from_left = (tick / 120) & 1;
        const int32_t x = from_left ? -PX(20) + PX((int)(th_rand(t) % 200u)) : PX(TH_PF_W + 20) - PX((int)(th_rand(t) % 200u));
        const uint16_t a = from_left ? DEG(60 + (int)(th_rand(t) % 15u)) : DEG(120 - (int)(th_rand(t) % 15u));
        ThLaser *l = th_laser(t, TL_COMET, x, -PX(20), a, 0, 10, 0, 0, (tick / 6) % 3 == 0 ? TC_WHITE : TC_YELLOW);
        if (l) { l->vx = (int32_t)((PX(5) * th_cos(a)) >> 14); l->vy = (int32_t)((PX(5) * th_sin(a)) >> 14); }
    }
    if (tick % 40 == 20) {
        th_ring(t, BX, BY, 15, m->ang, PX(2), TB_STAR, TC_YELLOW);
        m->ang = (uint16_t)(m->ang + DEG(9));
    }
    if (tick % 120 == 60) th_ring(t, BX, BY, 6, (uint16_t)th_rand(t), PX(1) + 128, TB_BIG, TC_BLUE);
}

/* non-spell 3: two moonlight pillars sweeping sideways, orb rings */
static void b_non3(Th *t, ThEmitter *m, int tick)
{
    th_boss_move_default(t, tick);
    if (tick % 600 == 1) {
        ThLaser *l = th_laser(t, TL_PILLAR, PX(80), 0, 0, TH_PF_H, 22, 60, 520, TC_BLUE);
        if (l) l->vx = PX(1);
        l = th_laser(t, TL_PILLAR, PX(TH_PF_W - 80), 0, 0, TH_PF_H, 22, 60, 520, TC_BLUE);
        if (l) l->vx = -PX(1);
    }
    if (tick % 30 == 0) {
        th_ring(t, BX, BY, 18, m->ang, PX(2) + 64, TB_ORB, (tick / 30) & 1 ? TC_BLUE : TC_CYAN);
        m->ang = (uint16_t)(m->ang + DEG(5));
    }
}

/* "终符：满月之夜": the full moon rises; rings start on a circle around
 * the boss (the moon's rim), six slow beams, phantom stars */
static void b_spell3(Th *t, ThEmitter *m, int tick)
{
    int k;
    if (!t->moon_t) t->moon_t = 1;
    t->boss.tx = PX(TH_PF_W / 2); t->boss.ty = PX(130);
    t->boss.x += (t->boss.tx - t->boss.x) / 16; t->boss.y += (t->boss.ty - t->boss.y) / 16;
    if (tick % 10 == 0) {
        const int n = D(12);
        for (k = 0; k < n; k++) {
            const uint16_t a = (uint16_t)(m->ang + (uint32_t)k * 65536u / (uint32_t)n);
            const int32_t rx = BX + ((PX(80) >> 8) * th_cos(a) >> 6), ry = BY + ((PX(80) >> 8) * th_sin(a) >> 6);
            ThBullet *b = th_fire(t, rx, ry, (uint16_t)(a + DEG(30)), PX(1) + 128, TB_ORB, (tick / 10) % 7);
            if (b) { b->acc = 4; b->timer = 60; }
        }
        m->ang = (uint16_t)(m->ang + DEG(6));
    }
    if (tick == 120 || tick % 900 == 120)
        for (k = 0; k < 6; k++) {
            ThLaser *l = th_laser(t, TL_BEAM, BX, BY, (uint16_t)(k * 10923 + tick * 7), 520, 12, 60, 760, TC_YELLOW);
            if (l) { l->follow = 1; l->angv = 60; }
        }
    if (tick % 45 == 20) phantom_ring(t, BX, BY, 10, (uint16_t)th_rand(t), PX(2) + 128, TB_STAR, TC_WHITE, 0);
}

const ThPhase th_s2_mid[] = {
    { 0,                             2500, 30, m_non,   0 },
    { "月符：银色光柱",   3500, 35, m_spell, 1500000u },
};
const int th_s2_nmid = (int)(sizeof th_s2_mid / sizeof th_s2_mid[0]);

const ThPhase th_s2_phases[] = {
    { 0,                             4500, 40, b_non1,   0 },
    { "幻符：月之幻影",  6500, 50, b_spell1, 3000000u },
    { 0,                             4500, 40, b_non2,   0 },
    { "星符：流星雨",    6500, 50, b_spell2, 4000000u },
    { 0,                             5000, 45, b_non3,   0 },
    { "终符：满月之夜",      8000, 60, b_spell3, 6000000u },
};
const int th_s2_nphases = (int)(sizeof th_s2_phases / sizeof th_s2_phases[0]);
