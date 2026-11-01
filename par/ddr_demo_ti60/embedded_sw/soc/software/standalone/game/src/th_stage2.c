/*
 * th_stage2.c - stage 2 "MOONLIT SEA OF CLOUDS": the moon princess.
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
 * Ring / fan bullet counts scale with t->dens like stage 1; phantoms, beams and
 * pillars do not (stress mode would only multiply alpha commands). Comets do in
 * stress mode (2026-10-26, comets() below; additive glows on the K path).
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

/* 流星（TL_COMET）：压力模式按倍率 t->dens 多发（2026-10-26），普通 / Lunatic 不变（随机数序列也不变）。
 * 第 1 颗照原样；其余的出发点在 +-spread 内、往回 0..30 px，角度 +-10 度，速度 80..120 %，随机散开，
 * 标成副本（pad = 1：不冒火花）。激光池最后 TH_LASER_RESERVE 个留给光束 / 光柱。 */
static void comets(Th *t, int32_t x, int32_t y, uint16_t a, int32_t spd, int color, int32_t spread)
{
    const int n = t->diff == TH_STRESS ? t->dens : 1;
    int i;
    for (i = 0; i < n; i++) {
        int32_t cx = x, cy = y, cs = spd;
        uint16_t ca = a;
        ThLaser *l;
        if (i) {
            if (t->nlasers >= TH_MAX_LASERS - TH_LASER_RESERVE) return;
            cx += (int32_t)(th_rand(t) % (uint32_t)(2 * spread + 1)) - spread;
            cy -= (int32_t)(th_rand(t) % (uint32_t)PX(30));
            ca = (uint16_t)(a + (uint16_t)DEG((int)(th_rand(t) % 21u) - 10));
            cs = spd * (80 + (int32_t)(th_rand(t) % 41u)) / 100;
        }
        l = th_laser(t, TL_COMET, cx, cy, ca, 0, 10, 0, 0, color);
        if (!l) return;
        l->pad = (uint8_t)(i != 0);
        l->vx = (int32_t)((cs * th_cos(ca)) >> 14); l->vy = (int32_t)((cs * th_sin(ca)) >> 14);
    }
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
        comets(t, e->x, e->y, DEG(90 - e->em.dir * 25), PX(4), (e->timer / 22) & 1 ? TC_YELLOW : TC_CYAN, PX(40));
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

/* "MOON SIGN: SILVER PILLARS": columns of moonlight + a rain of pellets */
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

/* "PHANTOM SIGN: LUNAR MIRAGE": rings of phantoms whose visibility runs round,
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

/* "STAR SIGN: METEOR SHOWER": comets from the upper left / right, star rings, big orbs */
static void b_spell2(Th *t, ThEmitter *m, int tick)
{
    th_boss_move_default(t, tick);
    if (tick % 6 == 0) {
        const int from_left = (tick / 120) & 1;
        const int32_t x = from_left ? -PX(20) + PX((int)(th_rand(t) % 200u)) : PX(TH_PF_W + 20) - PX((int)(th_rand(t) % 200u));
        const uint16_t a = from_left ? DEG(60 + (int)(th_rand(t) % 15u)) : DEG(120 - (int)(th_rand(t) % 15u));
        comets(t, x, -PX(20), a, PX(5), (tick / 6) % 3 == 0 ? TC_WHITE : TC_YELLOW, PX(100));
    }
    if (tick % 40 == 20) {
        th_ring(t, BX, BY, 15, m->ang, PX(2), TB_STAR, TC_YELLOW);
        m->ang = (uint16_t)(m->ang + DEG(9));
    }
    if (tick % 120 == 60) th_ring(t, BX, BY, 6, (uint16_t)th_rand(t), PX(1) + 128, TB_BIG, TC_BLUE);
}

/* ---------------------------------------------------------------- 最后两段（2026-10-28 重做）
 * 以前：两根横扫的月光柱 + 环 / 满月 + 环 + 六根慢转光束，用户觉得太简单。现在用"月之种子"（th_orb）：
 * 种子从 Boss 飞到几个点停住发光 -> 激光预警线把它们连起来 -> 连线发射 -> 种子爆开成弹幕环。 */
static uint32_t isqrt32(uint32_t v)
{
    uint32_t r = 0, b = 1u << 30;
    while (b > v) b >>= 2;
    while (b) {
        if (v >= r + b) { v -= r + b; r = (r >> 1) + b; } else r >>= 1;
        b >>= 2;
    }
    return r;
}

/* 种子 a -> 种子 b 的一条激光（按目标点连，种子此时已经停住） */
static void orb_link(Th *t, const ThOrb *a, const ThOrb *b, int w, int warn, int on, int color)
{
    const int32_t dx = b->tx - a->tx, dy = b->ty - a->ty;
    const int32_t ex = dx / TH_ONE, ey = dy / TH_ONE;
    const int len = (int)isqrt32((uint32_t)(ex * ex + ey * ey));
    if (len > 8) th_laser(t, TL_BEAM, a->tx, a->ty, th_atan2(dy, dx), len, w, warn, on, color);
}

/* 一颗种子爆开：光环 + 火花，两层米弹（一层顺时针弯、一层逆时针弯 = 花瓣），一圈星弹 */
static void orb_burst(Th *t, const ThOrb *o, int n, uint16_t a0, int kind2, int n2)
{
    int k;
    const int m = D(n);
    th_fx(t, TF_RING, o->color, o->x, o->y, 0, 0, 22);
    th_fx(t, TF_GLOW64, TC_WHITE, o->x, o->y, 0, 0, 14);
    for (k = 0; k < 6; k++) {
        const uint16_t a = (uint16_t)(a0 + k * 10923);
        th_fx(t, TF_SPARK, o->color, o->x, o->y, (PX(4) * th_cos(a)) >> 14, (PX(4) * th_sin(a)) >> 14, 18);
    }
    for (k = 0; k < m; k++) {
        const uint16_t a = (uint16_t)(a0 + (uint32_t)k * 65536u / (uint32_t)m);
        ThBullet *b = th_fire(t, o->x, o->y, a, PX(1) + 160, TB_RICE, (k & 1) ? o->color : TC_WHITE);
        if (b) { b->angv = (int16_t)((k & 1) ? 70 : -70); b->timer = 70; }
    }
    th_ring(t, o->x, o->y, n2, (uint16_t)(a0 + 32768u / (uint32_t)n), kind2 == TB_BIG ? PX(1) + 64 : PX(2), kind2, o->color);
}

/* non-spell 3 "月之碎片"（每 4 s 一轮）：3 颗种子（Lunatic / 压力 4 颗）落在自机周围的三角 / 四边形顶点上，
 * 激光把相邻的种子连成一个笼子把自机围住（预警 0.8 s），连线收掉后种子同时爆开成花瓣环。
 * Boss 一直在上面放慢速瞄准苦无。 */
static void b_non3(Th *t, ThEmitter *m, int tick)
{
    const int c = tick % 240, n = t->dens > 1 ? 4 : 3;
    int k;
    th_boss_move_default(t, tick);
    if (c == 1) {
        int32_t cx = t->px, cy = t->py - PX(40);
        const int32_t r = n == 3 ? PX(118) : PX(108);
        if (cx < PX(110)) cx = PX(110);
        if (cx > PX(TH_PF_W - 110)) cx = PX(TH_PF_W - 110);
        if (cy < PX(170)) cy = PX(170);
        if (cy > PX(330)) cy = PX(330);
        t->norbs = 0;
        for (k = 0; k < n; k++) {
            const uint16_t a = (uint16_t)(m->ang + (uint32_t)k * 65536u / (uint32_t)n);
            th_orb(t, BX, BY, cx + ((r >> 8) * th_cos(a) >> 6), cy + ((r >> 8) * th_sin(a) >> 6), 1,
                   k & 1 ? TC_CYAN : TC_PURPLE);
        }
        m->ang = (uint16_t)(m->ang + DEG(37));
    }
    if (c == 40 && t->norbs >= 3)
        for (k = 0; k < t->norbs; k++)
            orb_link(t, &t->orbs[k], &t->orbs[(k + 1) % t->norbs], 12, 50, 55, TC_CYAN);
    if (c == 155 && t->norbs) {
        for (k = 0; k < t->norbs; k++) orb_burst(t, &t->orbs[k], 16, (uint16_t)(m->ang + k * 5000), TB_STAR, 8);
        t->norbs = 0;
        t->shake = 8;
    }
    if (tick % 50 == 25) th_fan(t, BX, BY, 3, th_aim(t, BX, BY), DEG(24), PX(3), TB_KUNAI, TC_WHITE);
    if (tick % 80 == 0) th_ring(t, BX, BY, 10, (uint16_t)th_rand(t), PX(1) + 128, TB_ORB, TC_BLUE);
}

/* "FINAL: FULL MOON NIGHT"（每 7 s 一轮）：满月从下面升起（背景）。
 *   0..70   Boss 身前凝聚一轮小满月（月亮图淡入，四周火花往里收）；
 *   70      满月爆开：白光 + 震屏，十二道放射月光（短预警），两层反向弯曲的米弹花瓣 + 大玉环；
 *   130     六颗月之碎片飞到场地中间的六边形顶点上；
 *   190     激光把隔一个的碎片连起来 = 六芒星（预警 1 s）；
 *   310     碎片同时爆开成星弹花瓣。
 * 幻影星弹环照旧（每 0.75 s）。 */
static void b_spell3(Th *t, ThEmitter *m, int tick)
{
    const int c = (tick - 1) % 420;                              /* tick 从 1 开始：第一轮也从凝聚开始 */
    int k;
    if (!t->moon_t) t->moon_t = 1;
    t->boss.tx = PX(TH_PF_W / 2); t->boss.ty = PX(120);
    t->boss.x += (t->boss.tx - t->boss.x) / 16; t->boss.y += (t->boss.ty - t->boss.y) / 16;
    if (c == 0) { t->norbs = 0; th_orb(t, BX, BY + PX(30), BX, BY + PX(30), 2, TC_YELLOW); }
    if (c < 70 && t->norbs && t->orbs[0].size == 2) {           /* 凝聚：月亮跟着 Boss，火花往里收 */
        ThOrb *o = &t->orbs[0];
        o->tx = BX; o->ty = BY + PX(30);
        if ((c & 3) == 0) {
            const uint16_t a = (uint16_t)(th_rand(t));
            const int32_t r = PX(90);
            th_fx(t, TF_SPARK, (c & 4) ? TC_YELLOW : TC_WHITE, o->x + ((r >> 8) * th_cos(a) >> 6), o->y + ((r >> 8) * th_sin(a) >> 6),
                  -(PX(5) * th_cos(a)) >> 14, -(PX(5) * th_sin(a)) >> 14, 18);
        }
    }
    if (c == 70 && t->norbs) {                                   /* 满月爆开 */
        const ThOrb o = t->orbs[0];
        t->norbs = 0;
        t->flash = 170; t->shake = 24;
        t->sfx |= TS_BIT(TS_BOMB);
        for (k = 0; k < 12; k++)
            th_laser(t, TL_BEAM, o.x, o.y, (uint16_t)(m->ang + k * 5461 + 2730), 560, 10, 26, 34, k & 1 ? TC_YELLOW : TC_WHITE);
        orb_burst(t, &o, 28, m->ang, TB_BIG, 8);
        th_fx(t, TF_RING, TC_WHITE, o.x, o.y, 0, 0, 30);
        m->ang = (uint16_t)(m->ang + DEG(15));
    }
    if (c == 130) {                                              /* 六颗碎片 */
        const int32_t cx = PX(TH_PF_W / 2), cy = PX(236), r = PX(150);
        for (k = 0; k < 6; k++) {
            const uint16_t a = (uint16_t)(m->ang + k * 10923);
            th_orb(t, BX, BY, cx + ((r >> 8) * th_cos(a) >> 6), cy + ((r >> 8) * th_sin(a) >> 6), 0, k & 1 ? TC_YELLOW : TC_CYAN);
        }
    }
    if (c == 190 && t->norbs == 6)                               /* 六芒星：0-2-4 和 1-3-5 */
        for (k = 0; k < 6; k++)
            orb_link(t, &t->orbs[k], &t->orbs[(k + 2) % 6], 12, 60, 60, k & 1 ? TC_YELLOW : TC_CYAN);
    if (c == 310 && t->norbs) {
        for (k = 0; k < t->norbs; k++) orb_burst(t, &t->orbs[k], 12, (uint16_t)(m->ang + k * 7000), TB_STAR, 6);
        t->norbs = 0;
        t->shake = 10;
    }
    if (tick % 45 == 20) phantom_ring(t, BX, BY, 10, (uint16_t)th_rand(t), PX(2) + 128, TB_STAR, TC_WHITE, 0);
}

const ThPhase th_s2_mid[] = {
    { 0,                             2500, 30, m_non,   0 },
    { "MOON SIGN: SILVER PILLARS",   3500, 35, m_spell, 1500000u },
};
const int th_s2_nmid = (int)(sizeof th_s2_mid / sizeof th_s2_mid[0]);

const ThPhase th_s2_phases[] = {
    { 0,                             4500, 40, b_non1,   0 },
    { "PHANTOM SIGN: LUNAR MIRAGE",  6500, 50, b_spell1, 3000000u },
    { 0,                             4500, 40, b_non2,   0 },
    { "STAR SIGN: METEOR SHOWER",    6500, 50, b_spell2, 4000000u },
    { 0,                             5000, 45, b_non3,   0 },
    { "FINAL: FULL MOON NIGHT",      8000, 60, b_spell3, 6000000u },
};
const int th_s2_nphases = (int)(sizeof th_s2_phases / sizeof th_s2_phases[0]);
