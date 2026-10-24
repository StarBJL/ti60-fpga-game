/*
 * th_game.c - Touhou-style game logic (see th.h). Portable C99, integer only.
 *
 * Hot loop: th_tick -> bullets_update walks the dense bullet array once: move
 * (with optional acceleration / curving / delayed redirect), cull, hit + graze
 * test against the player, and write one GfxItem per visible bullet into a
 * scratch list tagged with its draw bucket. A counting pass then copies the
 * items bucket by bucket into this tick's draw table (one of TH_DRAW_BUFS), so
 * the renderer submits every bucket with ONE gfx_blit_table (the BitBlt engine
 * reads the table itself) and the CPU never walks the bullets again.
 */
#include "th.h"
#include "pad.h"
#include <string.h>
#include <stddef.h>

#define PX(v)      ((int32_t)(v) * TH_ONE)
#define PLAYER_R   2                        /* hit radius, px */
#define GRAZE_R    20
#define POC_Y      128                      /* item auto-collect line, px */

/* collision radius of each bullet kind, in 1/16 px (compared in Q4 units) */
static const uint16_t brad16[TB_KINDS] = { 2 * 16 + 8, 4 * 16, 2 * 16 + 8, 2 * 16 + 8, 4 * 16, 10 * 16, 24 * 16 };

uint32_t (*th_clock)(void);

uint32_t th_rand(Th *t)
{
    uint32_t x = t->rng;
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    return t->rng = x ? x : 0x1234567u;
}

uint16_t th_atan2(int32_t dy, int32_t dx)
{
    uint32_t ax = (uint32_t)(dx < 0 ? -dx : dx), ay = (uint32_t)(dy < 0 ? -dy : dy);
    uint32_t a;
    if (!ax && !ay) return 0;
    while ((ax | ay) >= (1u << 21)) { ax >>= 1; ay >>= 1; }   /* keep (v << 10) in 32 bits: no 64-bit divide on RV32 */
    if (ax >= ay) a = th_atan_tab[(ay << 10) / (ax ? ax : 1u)];
    else          a = 16384u - th_atan_tab[(ax << 10) / ay];
    if (dx < 0) a = 32768u - a;
    if (dy < 0) a = 65536u - a;
    return (uint16_t)a;
}

uint16_t th_aim(const Th *t, int32_t x, int32_t y) { return th_atan2(t->py - y, t->px - x); }

int th_bullet_count(const Th *t) { return t->nbullets; }

/* ---------------------------------------------------------------- spawning */
/* draw bucket + sprite index in the bucket (stars: + animation frame in the loop).
 * Must be called again whenever kind, colour or (rice / kunai) the angle change. */
static const uint8_t kind_bucket[TB_KINDS] = { TBK_PELLET, TBK_ORB, TBK_RICE0, TBK_KUNAI0, TBK_STAR, TBK_BIG, TBK_BUBBLE };
void th_bullet_sprite(ThBullet *b)
{
    switch (b->kind) {
    case TB_RICE: case TB_KUNAI:
        b->bk = (uint8_t)(kind_bucket[b->kind] + (b->color >> 2));
        b->sp = (uint8_t)((b->color & 3) * TH_ROT_STEPS + (((b->ang + 2048u) >> 12) & 15u));
        break;
    case TB_STAR:
        b->bk = TBK_STAR; b->sp = (uint8_t)(b->color * TH_STAR_FRAMES);
        break;
    default:
        b->bk = kind_bucket[b->kind]; b->sp = b->color;
        break;
    }
}

ThBullet *th_fire(Th *t, int32_t x, int32_t y, uint16_t ang, int spd, int kind, int color)
{
    ThBullet *b;
    if (t->nbullets >= (t->diff == TH_STRESS ? t->stress_cap : 16384)) { ++t->dropped; return 0; }
    b = &t->bullets[t->nbullets++];
    t->sfx |= TS_BIT(TS_FIRE);
    memset(b, 0, sizeof *b);
    b->x = x; b->y = y; b->ang = ang; b->spd = (int16_t)spd;
    b->vx = (int16_t)((spd * th_cos(ang)) >> 14);
    b->vy = (int16_t)((spd * th_sin(ang)) >> 14);
    b->kind = (uint8_t)kind; b->color = (uint8_t)color;
    b->spin = (uint8_t)th_rand(t);
    th_bullet_sprite(b);
    return b;
}

void th_ring(Th *t, int32_t x, int32_t y, int n, uint16_t ang0, int spd, int kind, int color)
{
    int i;
    /* big glowing kinds (32 / 64 px, additive) at most x4: at x32 they would only
     * white out the screen (and cost the most pixels) */
    n *= (kind == TB_BIG || kind == TB_BUBBLE) && t->dens > 4 ? 4 : t->dens;
    for (i = 0; i < n; i++) th_fire(t, x, y, (uint16_t)(ang0 + (uint32_t)i * 65536u / (uint32_t)n), spd, kind, color);
}

void th_fan(Th *t, int32_t x, int32_t y, int n, uint16_t ang, uint16_t spread, int spd, int kind, int color)
{
    int i;
    if (t->dens > 1) {
        uint32_t sp = (uint32_t)spread + (uint32_t)spread * (uint32_t)(t->dens - 1) / 4u;
        n = n * t->dens;
        spread = (uint16_t)(sp > 57344u ? 57344u : sp);           /* wider fan, at most 315 degrees */
    }
    if (n == 1) { th_fire(t, x, y, ang, spd, kind, color); return; }
    for (i = 0; i < n; i++)
        th_fire(t, x, y, (uint16_t)(ang - spread / 2 + (uint32_t)spread * (uint32_t)i / (uint32_t)(n - 1)), spd, kind, color);
}

void th_fx(Th *t, int kind, int color, int32_t x, int32_t y, int32_t vx, int32_t vy, int life)
{
    ThFx *f;
    if (t->nfx >= TH_MAX_FX) return;
    f = &t->fx[t->nfx++];
    f->active = 1; f->kind = (uint8_t)kind; f->color = (uint8_t)color;
    f->x = x; f->y = y; f->vx = vx; f->vy = vy; f->life = f->total = (uint16_t)life;
}

void th_explode(Th *t, int32_t x, int32_t y, int color, int big)
{
    int i, n = big ? 24 : 8;
    th_fx(t, big ? TF_GLOW64 : TF_GLOW32, color, x, y, 0, 0, big ? 40 : 24);
    th_fx(t, TF_RING, color, x, y, 0, 0, big ? 36 : 20);
    for (i = 0; i < n; i++) {
        uint16_t a = (uint16_t)th_rand(t);
        int s = 256 + (int)(th_rand(t) % (big ? 1024u : 512u));
        th_fx(t, TF_SPARK, color, x, y, (s * th_cos(a)) >> 14, (s * th_sin(a)) >> 14, 20 + (int)(th_rand(t) % 20u));
    }
}

void th_drop(Th *t, int kind, int32_t x, int32_t y)
{
    ThItem *it;
    if (t->nitems >= TH_MAX_ITEMS) return;
    it = &t->items[t->nitems++];
    it->active = 1; it->kind = (uint8_t)kind; it->autoc = 0;
    it->x = x + (int32_t)(th_rand(t) % PX(24)) - PX(12);
    it->y = y + (int32_t)(th_rand(t) % PX(16)) - PX(8);
    it->vy = -PX(2) - (int32_t)(th_rand(t) % 256u);
    if (kind == TI_STAR) { it->vy = 0; it->autoc = 1; }
}

ThEnemy *th_spawn_enemy(Th *t, int kind, int color, int32_t x, int32_t y, int hp, ThEnemyFn fn)
{
    int i;
    for (i = 0; i < TH_MAX_ENEMIES; i++) {
        ThEnemy *e = &t->enemies[i];
        if (e->active) continue;
        memset(e, 0, sizeof *e);
        e->active = 1; e->kind = (uint8_t)kind; e->color = (uint8_t)color;
        e->x = x; e->y = y; e->hp = e->maxhp = t->diff == TH_LUNATIC ? hp * 3 / 2 : hp; e->fn = fn;
        e->drop_p = kind == TE_BIGFAIRY ? 8 : 1;
        e->drop_pt = kind == TE_BIGFAIRY ? 8 : 1;
        return e;
    }
    return 0;
}

/* bullets -> star items (bomb, death, end of a boss phase) */
void th_ghost(Th *t, const Sprite *s, int32_t x, int32_t y, int32_t vy, int life)
{
    ThGhost *gh;
    if (t->nghosts >= TH_MAX_GHOSTS || !s->pixels) return;
    gh = &t->ghosts[t->nghosts++];
    gh->s = s; gh->x = x; gh->y = y; gh->vy = vy; gh->life = gh->total = (uint16_t)life;
}

/* fading copy of a bullet sprite: draw_ghosts centres it at width / 2, a tight-box sprite
 * has its bullet centre at th_bspr_cx / cy - move the ghost by the difference (view px -> Q8) */
static void bullet_ghost(Th *t, int bk, int sp, int32_t x, int32_t y)
{
    const Sprite *s = &th_bspr[bk][sp];
    const int32_t ox = s->width / 2 - th_bspr_cx[bk][sp], oy = s->height / 2 - th_bspr_cy[bk][sp];
    th_ghost(t, s, x + ox * 512 / th_view.s2, y + oy * 512 / th_view.s2, -64, 24);
}

/* ---------------------------------------------------------------- phantoms / lasers (stage 2) */
int th_phantom_alpha(const ThPhantom *p) { return 40 + ((190 * (th_sin(p->ph) + 16384)) >> 15); }

ThPhantom *th_phantom(Th *t, int32_t x, int32_t y, uint16_t ang, int spd, int kind, int color, uint16_t ph0, uint16_t dph)
{
    ThPhantom *p;
    ThBullet tmp;
    if (t->nphantoms >= TH_MAX_PHANTOMS) { ++t->dropped; return 0; }
    p = &t->phantoms[t->nphantoms++];
    p->x = x; p->y = y;
    p->vx = (int16_t)((spd * th_cos(ang)) >> 14);
    p->vy = (int16_t)((spd * th_sin(ang)) >> 14);
    p->ph = ph0; p->dph = dph; p->grazed = 0;
    memset(&tmp, 0, sizeof tmp);
    tmp.kind = (uint8_t)kind; tmp.color = (uint8_t)color; tmp.ang = ang;
    th_bullet_sprite(&tmp);
    p->bk = tmp.bk; p->sp = tmp.sp;
    return p;
}

ThLaser *th_laser(Th *t, int kind, int32_t x, int32_t y, uint16_t ang, int len, int wmax, int warn, int on, int color)
{
    ThLaser *l;
    if (t->nlasers >= TH_MAX_LASERS) return 0;
    l = &t->lasers[t->nlasers++];
    t->sfx |= TS_BIT(TS_LASER);
    memset(l, 0, sizeof *l);
    l->kind = (uint8_t)kind; l->x = x; l->y = y; l->ang = ang;
    l->len = (int16_t)len; l->wmax = (int16_t)wmax; l->warn = (int16_t)warn; l->on = (int16_t)on;
    l->color = (uint8_t)color;
    if (kind == TL_COMET) l->w = (int16_t)wmax;
    return l;
}

/* bullets -> star items (bomb, end of a boss phase) or nothing (death); a part of
 * them (at most TH_MAX_GHOSTS, spread over the whole array) fades out as ghosts */
void th_clear_bullets(Th *t, int to_items)
{
    int i, made = 0;
    const int step = t->nbullets / (TH_MAX_GHOSTS - 32) + 1;
    for (i = 0; i < t->nbullets; i++) {
        const ThBullet *b = &t->bullets[i];
        if (to_items && made < 600 && (i & 3) == 0) { th_drop(t, TI_STAR, b->x, b->y); ++made; }
        if (to_items) t->score += 10;
        if (i % step == 0) bullet_ghost(t, b->bk, b->sp, b->x, b->y);
    }
    t->nbullets = 0;
    for (i = 0; i < t->nphantoms; i++) {
        const ThPhantom *p = &t->phantoms[i];
        if (to_items && (i & 3) == 0) th_drop(t, TI_STAR, p->x, p->y);
        if ((i & 1) == 0) bullet_ghost(t, p->bk, p->sp, p->x, p->y);
    }
    t->nphantoms = 0;
    for (i = 0; i < t->nlasers; i++) {                 /* lasers: shrink away (comets just vanish) */
        ThLaser *l = &t->lasers[i];
        if (l->kind == TL_COMET) { *l = t->lasers[--t->nlasers]; --i; continue; }
        if (l->t < l->warn + l->on) l->t = (int16_t)(l->warn + l->on);
    }
}

/* ---------------------------------------------------------------- start */
void th_init(Th *t, uint32_t seed)
{
    memset(t, 0, sizeof *t);
    t->rng = seed ? seed : 1u;
    t->mode = TH_TITLE;
    t->dens = 1;
    t->hiscore = 1000000u;
    t->stress_level = TH_STRESS_LEVEL_INIT;
    t->stress_auto = 1;
    t->stress_cap = TH_MAX_BULLETS;
}

/* ---------------------------------------------------------------- stress level / governor */
const uint8_t th_stress_levels[TH_STRESS_LEVELS] = { 1, 2, 3, 4, 6, 8, 10, 12, 16, 20, 24, 32, 40, 48 };

int th_stress_dens(const Th *t) { return th_stress_levels[t->stress_level]; }

static void stress_apply(Th *t)
{
    if (t->stress_level < 0) t->stress_level = 0;
    if (t->stress_level >= TH_STRESS_LEVELS) t->stress_level = TH_STRESS_LEVELS - 1;
    if (t->diff == TH_STRESS) t->dens = th_stress_dens(t);       /* new patterns use it at once */
}

void th_stress_step(Th *t, int dir)
{
    t->stress_auto = 0;
    t->stress_level += dir;
    t->stress_cap = TH_MAX_BULLETS;
    stress_apply(t);
}

void th_stress_set_auto(Th *t, int on)
{
    t->stress_auto = on;
    t->gov_n = 0; t->gov_max = 0; t->gov_ok = 0; t->gov_cool = 0;
    memset(t->gov_block, 0, sizeof t->gov_block);
    memset(t->gov_fail, 0, sizeof t->gov_fail);
    if (!on) t->stress_cap = TH_MAX_BULLETS;
}

/*
 * 60 fps governor. A level change only affects bullets fired from now on (the
 * ones on screen live another 2-4 s), so the reaction is two-fold:
 *   - one frame over 97 % of the budget: the bullet cap is set to the bullets
 *     alive now (no growth; takes effect on the next th_fire); over 125 %, or a
 *     0.5 s window whose slowest frame is over 92 %: cap = 90 % of them, and the
 *     window case also goes one level down; then 3 s cool-down before a level-up;
 *   - a window in which the cap refused bullets (patterns look cut) also goes one
 *     level down, at most every 3 s;
 *   - six windows in a row (3 s) under 70 % and the cap back at the maximum: one
 *     level up, unless that level was given up recently (blocked 20 s after its
 *     first failure, then 40 / 80 / 160 s), so a calm part of the stage does not
 *     push the level up into a dense spell card again and again; every window
 *     under 70 % also raises the cap by 1/8 + 256.
 * The slowest frame of the window (not the average) is what decides, since one
 * long frame is already a visible hitch at 60 fps.
 */
void th_stress_frame(Th *t, uint32_t work_us, uint32_t budget_us)
{
    int capped, i;
    /* windows / cool-downs count game ticks (1/60 s), not frames: with vsync off
     * there are several frames per tick */
    const int dt = (int)(t->tick - t->gov_tick) > 8 ? 8 : (int)(t->tick - t->gov_tick);
    t->gov_tick = t->tick;
    t->gov_budget = budget_us;
    if (t->diff != TH_STRESS || t->mode != TH_PLAY) { t->gov_n = 0; t->gov_max = 0; return; }
    if (work_us > t->gov_max) t->gov_max = work_us;
    t->gov_n += dt;
    if (!t->stress_auto) {                     /* manual: only measure (HUD / UART show the load) */
        if (t->gov_n >= 30) { t->gov_last = t->gov_max; t->gov_n = 0; t->gov_max = 0; }
        return;
    }
    t->gov_cool = t->gov_cool > dt ? t->gov_cool - dt : 0;
    for (i = 0; i < TH_STRESS_LEVELS; i++) t->gov_block[i] = t->gov_block[i] > dt ? t->gov_block[i] - dt : 0;
    if (work_us > budget_us + budget_us / 4u && t->nbullets * 9 / 10 < t->stress_cap) {
        t->stress_cap = t->nbullets * 9 / 10 > 256 ? t->nbullets * 9 / 10 : 256;   /* clearly late: cut */
        t->gov_ok = 0;
    } else if (work_us > budget_us / 100u * 97u && t->nbullets < t->stress_cap) {
        t->stress_cap = t->nbullets > 256 ? t->nbullets : 256;                      /* nearly late: stop growing */
        t->gov_ok = 0;
    }
    if (t->gov_n < 30) return;
    t->gov_last = t->gov_max;
    capped = t->dropped - t->gov_drop0 > 32;
    t->gov_drop0 = t->dropped;
    /* too slow, or kept in time only because the cap refused bullets (the patterns
     * look broken then): one level down. The cap case at most every 3 s, since
     * the bullets of the old level stay on screen for a while. */
    if (t->gov_max > budget_us / 100u * 92u || (capped && !t->gov_cool)) {
        if (t->stress_level > 0) {
            const int l = t->stress_level, f = t->gov_fail[l] < 3 ? t->gov_fail[l] : 3;
            t->gov_block[l] = 1200 << f;                 /* ticks: 20 s, 40 s, 80 s, 160 s before trying it again */
            if (t->gov_fail[l] < 255) ++t->gov_fail[l];
            --t->stress_level; ++t->gov_downs; stress_apply(t);
        }
        if (t->gov_max > budget_us / 100u * 92u && t->nbullets * 9 / 10 < t->stress_cap)
            t->stress_cap = t->nbullets * 9 / 10 > 256 ? t->nbullets * 9 / 10 : 256;
        t->gov_ok = 0;
        t->gov_cool = 180;
    } else if (capped) {
        t->gov_ok = 0;
    } else if (t->gov_max < budget_us / 100u * 70u) {
        t->stress_cap += t->stress_cap / 8 + 256;
        if (t->stress_cap > TH_MAX_BULLETS) t->stress_cap = TH_MAX_BULLETS;
        if (++t->gov_ok >= 6 && !t->gov_cool && t->stress_cap >= TH_MAX_BULLETS) {
            if (t->stress_level < TH_STRESS_LEVELS - 1 && !t->gov_block[t->stress_level + 1]) {
                ++t->stress_level; ++t->gov_ups; stress_apply(t);
            }
            t->gov_ok = 0;
            t->gov_cool = 180;
        }
    } else {
        t->gov_ok = 0;
    }
    t->gov_n = 0;
    t->gov_max = 0;
}

const ThStageDef th_stages[TH_STAGES] = {
    { "第一关", "萌芽花田",  th_stage1_tick, th_s1_phases, &th_s1_nphases, 0,         0 },
    { "第二关", "月下云海",   th_stage2_tick, th_s2_phases, &th_s2_nphases, th_s2_mid, &th_s2_nmid },
};

const ThPhase *th_phase_list(const Th *t, int *n)
{
    const ThStageDef *sd = &th_stages[t->stage];
    if (t->boss.mid) { *n = sd->nmid ? *sd->nmid : 0; return sd->mid_phases; }
    *n = *sd->nphases;
    return sd->phases;
}
static const ThPhase *cur_phase(const Th *t)
{
    int n;
    return &th_phase_list(t, &n)[t->boss.phase];
}

/* boss (mid = 0) or mid-boss (mid = 1) of the current stage flies in */
void th_spawn_boss(Th *t, int mid)
{
    ThBoss *b = &t->boss;
    memset(b, 0, sizeof *b);
    b->active = 1; b->entering = mid ? 70 : 100; b->phase = 0; b->mid = mid;
    b->art = t->stage;
    b->x = PX(TH_PF_W / 2); b->y = -PX(60);
    if (!mid) t->boss_seen = 1;
    t->banner = mid ? 0 : "首领来袭"; t->banner_t = mid ? 0 : 120;
    if (!mid) t->sfx |= TS_BIT(TS_WARNING);
}

/* stage state (everything before the draw tables: the engine may still be reading
 * the current table with pipelined frames, so dbuf / dcount / draw[] stay) */
static void stage_reset(Th *t, int keep_player)
{
    /* the fields kept (Th is megabytes: never copy it whole - the stack is 16 KB) */
    const uint32_t rng = t->rng, hi = t->hiscore, tk = t->tick, score = t->score;
    const int diff = t->diff, dens = t->dens, ss = t->start_stage, lives = t->lives, bombs = t->bombs;
    const int power = t->power, graze = t->graze, pts = t->points_got;
    memset(t, 0, offsetof(Th, dbuf));
    t->rng = rng; t->hiscore = hi; t->tick = tk;
    if (keep_player) {
        t->diff = diff; t->dens = dens; t->start_stage = ss;
        t->lives = lives; t->bombs = bombs; t->power = power; t->score = score;
        t->graze = graze; t->points_got = pts;
    }
}

void th_start_stage(Th *t, int diff, int stage)
{
    stage_reset(t, 0);
    t->mode = TH_PLAY;
    t->diff = diff;
    t->stage = t->start_stage = stage;
    t->dens = diff == TH_STRESS ? th_stress_dens(t) : diff == TH_LUNATIC ? 2 : 1;
    t->lives = 3; t->bombs = 3;
    t->power = diff == TH_STRESS ? 400 : stage ? 250 : 100;   /* practice from stage 2: some power */
    t->px = PX(TH_PF_W / 2); t->py = PX(400);
    t->banner = th_stages[stage].banner; t->banner_t = 180;
    {
        int i;
        for (i = 0; i < 4; i++) { t->opt_x[i] = t->px; t->opt_y[i] = t->py; }
    }
}

void th_start(Th *t, int diff) { th_start_stage(t, diff, 0); }

/* stage cleared: on to the next one with lives / bombs / power / score */
static void next_stage(Th *t)
{
    const int st = t->stage + 1;
    stage_reset(t, 1);
    t->mode = TH_PLAY;
    t->stage = st;
    t->px = PX(TH_PF_W / 2); t->py = PX(400);
    t->banner = th_stages[st].banner; t->banner_t = 180;
    t->invuln = 60;
    {
        int i;
        for (i = 0; i < 4; i++) { t->opt_x[i] = t->px; t->opt_y[i] = t->py; }
    }
}

/* ---------------------------------------------------------------- player */
static void player_die(Th *t)
{
    int i;
    th_explode(t, t->px, t->py, TC_RED, 1);
    th_ghost(t, &th_spr_player[1], t->px, t->py, -PX(1), 45);
    t->sfx |= TS_BIT(TS_DEATH);
    if (--t->lives < 0) { t->lives = 0; t->mode = TH_OVER; t->respawn_t = 0; t->sfx |= TS_BIT(TS_OVER); return; }
    for (i = 0; i < 5; i++) th_drop(t, i == 0 && t->power >= 100 ? TI_BIGP : TI_POWER, t->px, t->py - PX(32));
    t->power = t->power > 100 ? t->power - 100 : 0;
    t->bombs = 3;
    t->invuln = 180;
    t->boss.spell_fail = 1;
    th_clear_bullets(t, 0);
    t->px = PX(TH_PF_W / 2); t->py = PX(400);
}

static void player_bomb(Th *t)
{
    --t->bombs;
    t->bomb_t = 150;
    t->sfx |= TS_BIT(TS_BOMB);
    if (t->invuln < 150) t->invuln = 150;
    t->dying = 0;
    t->boss.spell_fail = 1;
    t->flash = 200;
    t->shake = 30;
    th_clear_bullets(t, 1);
}

static void player_hit(Th *t)
{
    if (t->invuln || t->bomb_t || t->dying || t->invincible) return;
    t->dying = 8;                                         /* deathbomb window */
}

static const int8_t opt_off[5][4][2] = {
    { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
    { { 0, -32 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
    { { -24, -8 }, { 24, -8 }, { 0, 0 }, { 0, 0 } },
    { { -28, -4 }, { 0, -34 }, { 28, -4 }, { 0, 0 } },
    { { -34, 0 }, { -14, -26 }, { 14, -26 }, { 34, 0 } } };
static const int8_t opt_off_f[5][4][2] = {
    { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
    { { 0, -24 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
    { { -10, -22 }, { 10, -22 }, { 0, 0 }, { 0, 0 } },
    { { -14, -18 }, { 0, -28 }, { 14, -18 }, { 0, 0 } },
    { { -18, -16 }, { -7, -28 }, { 7, -28 }, { 18, -16 } } };

static void shot_spawn(Th *t, int32_t x, int32_t y, int32_t vx, int32_t vy, int dmg, int kind, int target)
{
    int i;
    for (i = 0; i < TH_MAX_SHOTS; i++) {
        ThShot *s = &t->shots[i];
        if (s->active) continue;
        s->active = 1; s->x = x; s->y = y; s->vx = vx; s->vy = vy;
        s->dmg = (int16_t)dmg; s->kind = (uint8_t)kind; s->target = (int16_t)target;
        return;
    }
}

/* nearest enemy (index) or the boss (TH_MAX_ENEMIES), -1 = none */
static int nearest_target(const Th *t, int32_t x, int32_t y)
{
    int i, best = -1;
    int64_t bd = (int64_t)PX(600) * PX(600);
    for (i = 0; i < TH_MAX_ENEMIES; i++) {
        const ThEnemy *e = &t->enemies[i];
        int64_t dx, dy, d;
        if (!e->active || e->y < 0) continue;
        dx = e->x - x; dy = e->y - y; d = dx * dx + dy * dy;
        if (d < bd) { bd = d; best = i; }
    }
    if (t->boss.active && !t->boss.entering && !t->boss.dying) {
        int64_t dx = t->boss.x - x, dy = t->boss.y - y;
        if (dx * dx + dy * dy < bd) best = TH_MAX_ENEMIES;
    }
    return best;
}

static void player_update(Th *t)
{
    const uint32_t h = t->held;
    int32_t sp = (h & PAD_SLOW) ? PX(2) : PX(4) + 128;
    int32_t dx = 0, dy = 0, n, i;
    t->focus = (h & PAD_SLOW) != 0;
    if (h & PAD_LEFT) dx -= 1;
    if (h & PAD_RIGHT) dx += 1;
    if (h & PAD_UP) dy -= 1;
    if (h & PAD_DOWN) dy += 1;
    if (dx && dy) sp = sp * 181 / 256;
    if (!t->dying) { t->px += dx * sp; t->py += dy * sp; }
    if (t->px < PX(8)) t->px = PX(8);
    if (t->px > PX(TH_PF_W - 8)) t->px = PX(TH_PF_W - 8);
    if (t->py < PX(16)) t->py = PX(16);
    if (t->py > PX(TH_PF_H - 16)) t->py = PX(TH_PF_H - 16);
    if (t->invuln) --t->invuln;
    if (t->bomb_t) --t->bomb_t;
    if ((t->pressed & PAD_BOMB) && t->bombs > 0 && !t->bomb_t) player_bomb(t);
    if (t->dying && --t->dying == 0) player_die(t);
    if (t->mode != TH_PLAY) return;

    /* options follow their (focused / unfocused) offsets */
    n = t->power / 100;
    if (t->opt_focus < 16 && t->focus) ++t->opt_focus;
    if (t->opt_focus > 0 && !t->focus) --t->opt_focus;
    for (i = 0; i < n; i++) {
        int32_t ox = (opt_off[n][i][0] * (16 - t->opt_focus) + opt_off_f[n][i][0] * t->opt_focus) * TH_ONE / 16;
        int32_t oy = (opt_off[n][i][1] * (16 - t->opt_focus) + opt_off_f[n][i][1] * t->opt_focus) * TH_ONE / 16;
        t->opt_x[i] += (t->px + ox - t->opt_x[i]) / 3;
        t->opt_y[i] += (t->py + oy - t->opt_y[i]) / 3;
    }

    /* shooting */
    if (t->shot_t) --t->shot_t;
    if ((h & PAD_SHOT) && !t->shot_t && !t->dying) {
        t->shot_t = 4;
        t->sfx |= TS_BIT(TS_SHOT);
        shot_spawn(t, t->px - PX(7), t->py - PX(12), 0, -PX(24), 6, 0, -1);
        shot_spawn(t, t->px + PX(7), t->py - PX(12), 0, -PX(24), 6, 0, -1);
        for (i = 0; i < n; i++) {
            if (t->focus) shot_spawn(t, t->opt_x[i], t->opt_y[i] - PX(8), 0, -PX(24), 4, 1, -1);
            else shot_spawn(t, t->opt_x[i], t->opt_y[i] - PX(8), 0, -PX(14), 3, 1,
                            nearest_target(t, t->opt_x[i], t->opt_y[i]));     /* homing amulet */
        }
    }
}

static void enemy_damage(Th *t, ThEnemy *e, int dmg)
{
    int i;
    e->hp -= dmg;
    e->flash = 3;
    if (e->hp > 0) { t->sfx |= TS_BIT(TS_HIT); return; }
    e->active = 0;
    t->sfx |= TS_BIT(TS_KILL);
    t->score += e->kind == TE_BIGFAIRY ? 30000u : 1000u;
    th_explode(t, e->x, e->y, e->color, e->kind == TE_BIGFAIRY);
    th_ghost(t, &th_spr_enemy[e->kind][e->color & 3][0], e->x, e->y, -PX(1), 30);
    for (i = 0; i < e->drop_p; i++) th_drop(t, e->kind == TE_BIGFAIRY && i == 0 ? TI_BIGP : TI_POWER, e->x, e->y);
    for (i = 0; i < e->drop_pt; i++) th_drop(t, TI_POINT, e->x, e->y);
    if (e->kind == TE_BIGFAIRY) th_drop(t, TI_BOMB, e->x, e->y);
}

static void boss_damage(Th *t, int dmg)
{
    ThBoss *b = &t->boss;
    if (!b->active || b->entering || b->dying || b->phase_tick < 60) return;
    if (cur_phase(t)->name) dmg = dmg * 3 / 4;   /* spell cards are tougher */
    b->hp -= dmg;
    t->sfx |= TS_BIT(TS_HIT);
    if ((t->tick & 3) == 0) t->score += 10;
}

static void shots_update(Th *t)
{
    int i, j;
    t->shots_alive = 0;
    for (i = 0; i < TH_MAX_SHOTS; i++) {
        ThShot *s = &t->shots[i];
        int hit = 0;
        if (!s->active) continue;
        if (s->target >= 0) {                              /* homing amulet: steer to the target */
            int32_t tx = 0, ty = 0, ok = 0;
            if (s->target == TH_MAX_ENEMIES) { if (t->boss.active) { tx = t->boss.x; ty = t->boss.y; ok = 1; } }
            else if (t->enemies[s->target].active) { tx = t->enemies[s->target].x; ty = t->enemies[s->target].y; ok = 1; }
            if (ok) {
                uint16_t a = th_atan2(ty - s->y, tx - s->x);
                s->vx += (((PX(14) * th_cos(a)) >> 14) - s->vx) >> 2;
                s->vy += (((PX(14) * th_sin(a)) >> 14) - s->vy) >> 2;
            }
        }
        s->x += s->vx; s->y += s->vy;
        if (s->y < -PX(32) || s->y > PX(TH_PF_H + 32) || s->x < -PX(32) || s->x > PX(TH_PF_W + 32)) { s->active = 0; continue; }
        for (j = 0; j < TH_MAX_ENEMIES && !hit; j++) {
            ThEnemy *e = &t->enemies[j];
            int32_t r = e->kind == TE_BIGFAIRY ? PX(22) : PX(14);
            if (!e->active) continue;
            if (s->x > e->x - r && s->x < e->x + r && s->y > e->y - r && s->y < e->y + r) {
                enemy_damage(t, e, s->dmg);
                hit = 1;
            }
        }
        if (!hit && t->boss.active && s->x > t->boss.x - PX(28) && s->x < t->boss.x + PX(28) &&
            s->y > t->boss.y - PX(36) && s->y < t->boss.y + PX(36)) {
            boss_damage(t, s->dmg);
            hit = 1;
        }
        if (hit) {
            s->active = 0;
            if ((i & 1) == 0) th_fx(t, TF_SPARK, s->kind ? TC_BLUE : TC_WHITE, s->x, s->y, 0, -PX(2), 8);
        } else {
            ++t->shots_alive;
        }
    }
}

static void enemies_update(Th *t)
{
    int i;
    t->enemies_alive = 0;
    for (i = 0; i < TH_MAX_ENEMIES; i++) {
        ThEnemy *e = &t->enemies[i];
        if (!e->active) continue;
        ++e->timer;
        if (e->fn) e->fn(t, e);
        if (!e->active) continue;
        e->x += e->vx; e->y += e->vy;
        if (e->flash) --e->flash;
        if (e->timer > 60 && (e->x < -PX(64) || e->x > PX(TH_PF_W + 64) || e->y < -PX(64) || e->y > PX(TH_PF_H + 64))) {
            e->active = 0;
            continue;
        }
        if (t->bomb_t && e->y > 0) enemy_damage(t, e, 8);
        if (!e->active) continue;
        {
            int32_t dx = e->x - t->px, dy = e->y - t->py;
            if (dx > -PX(10) && dx < PX(10) && dy > -PX(10) && dy < PX(10)) player_hit(t);
        }
        ++t->enemies_alive;
    }
}

static void items_update(Th *t)
{
    int i;
    const int all = t->py < PX(POC_Y) || t->bomb_t > 100;
    for (i = 0; i < t->nitems; i++) {
        ThItem *it = &t->items[i];
        int32_t dx, dy;
        if (all && t->mode == TH_PLAY && !t->dying) it->autoc = 1;
        dx = t->px - it->x; dy = t->py - it->y;
        if (it->autoc && !t->dying) {                      /* fly to the player */
            uint16_t a = th_atan2(dy, dx);
            int sp = it->kind == TI_STAR ? PX(10) : PX(8);
            it->x += (sp * th_cos(a)) >> 14;
            it->y += (sp * th_sin(a)) >> 14;
        } else {
            it->vy += 16;
            if (it->vy > PX(2) + 128) it->vy = PX(2) + 128;
            it->y += it->vy;
            if (it->y > PX(TH_PF_H + 16)) { *it = t->items[--t->nitems]; --i; continue; }
        }
        if (dx > -PX(20) && dx < PX(20) && dy > -PX(20) && dy < PX(20) && !t->dying) {
            const int kind = it->kind, autoc = it->autoc, p0 = t->power;
            const int32_t iy = it->y;
            t->sfx |= TS_BIT(kind == TI_LIFE ? TS_EXTEND : kind == TI_BOMB ? TS_BOMBGET : TS_ITEM);
            *it = t->items[--t->nitems]; --i;            /* remove: the last one takes the slot */
            switch (kind) {
            case TI_POWER: if (t->power < 400) t->power += 5; else t->score += 100; t->score += 10; break;
            case TI_BIGP:  t->power = t->power + 100 > 400 ? 400 : t->power + 100; break;
            case TI_POINT: {
                int32_t y = iy / TH_ONE;
                uint32_t v = (autoc || y <= POC_Y) ? 10000u : (uint32_t)(10000 - (y - POC_Y) * 20);
                t->score += v < 2000u ? 2000u : v;
                ++t->points_got;
                break;
            }
            case TI_BOMB:  if (t->bombs < 8) ++t->bombs; break;
            case TI_LIFE:  if (t->lives < 8) ++t->lives; break;
            default:       t->score += 50u + (uint32_t)t->graze; break;
            }
            if (t->power > 400) t->power = 400;
            if (t->power < 0) t->power = 0;
            if (t->power / 100 > p0 / 100) t->sfx |= TS_BIT(TS_POWERUP);
            continue;
        }
    }
    t->items_alive = t->nitems;
}

static void fx_update(Th *t)
{
    int i;
    for (i = 0; i < t->nghosts; i++) {
        ThGhost *gh = &t->ghosts[i];
        if (--gh->life == 0) { *gh = t->ghosts[--t->nghosts]; --i; continue; }
        gh->y += gh->vy;
    }
    if (t->cutin_t) --t->cutin_t;
    for (i = 0; i < t->nfx; i++) {
        ThFx *f = &t->fx[i];
        if (--f->life == 0) { *f = t->fx[--t->nfx]; --i; continue; }
        f->x += f->vx; f->y += f->vy;
        f->vx -= f->vx >> 4; f->vy -= f->vy >> 4;
    }
    t->fx_alive = t->nfx;
}

static void phantoms_update(Th *t)
{
    int i, hit = 0;
    const int32_t px = t->px, py = t->py;
    const int can_hit = !(t->invuln || t->bomb_t || t->dying || t->invincible) && t->mode == TH_PLAY;
    for (i = 0; i < t->nphantoms; i++) {
        ThPhantom *p = &t->phantoms[i];
        int32_t dx, dy;
        p->x += p->vx; p->y += p->vy; p->ph = (uint16_t)(p->ph + p->dph);
        if ((uint32_t)(p->x + PX(40)) > (uint32_t)PX(TH_PF_W + 80) || (uint32_t)(p->y + PX(40)) > (uint32_t)PX(TH_PF_H + 80)) {
            *p = t->phantoms[--t->nphantoms]; --i; continue;
        }
        dx = p->x - px; dy = p->y - py;
        if (th_phantom_alpha(p) >= 150 && (uint32_t)(dx + PX(30)) < (uint32_t)PX(60) && (uint32_t)(dy + PX(30)) < (uint32_t)PX(60)) {
            const int32_t ex = dx >> 4, ey = dy >> 4, d2 = ex * ex + ey * ey;
            const int32_t r = (int32_t)th_bk_half[p->bk] * 8 + PLAYER_R * 16, g = r + GRAZE_R * 16;
            if (d2 < r * r && can_hit) hit = 1;
            else if (d2 < g * g && !p->grazed && t->mode == TH_PLAY && !t->dying) {
                p->grazed = 1; ++t->graze; t->score += 500; t->sfx |= TS_BIT(TS_GRAZE);
                th_fx(t, TF_SPARK, TC_WHITE, p->x, p->y, -dx >> 3, -dy >> 3, 12);
            }
        }
    }
    if (hit) player_hit(t);
}

static void lasers_update(Th *t)
{
    int i, hit = 0;
    const int can_hit = !(t->invuln || t->bomb_t || t->dying || t->invincible) && t->mode == TH_PLAY;
    for (i = 0; i < t->nlasers; i++) {
        ThLaser *l = &t->lasers[i];
        const int act = l->t - l->warn;               /* < 0 warning, 0 .. on firing, then shrinking */
        int32_t dx, dy;
        ++l->t;
        if (l->follow && t->boss.active) { l->x = t->boss.x; l->y = t->boss.y; }
        l->x += l->vx; l->y += l->vy;
        l->ang = (uint16_t)(l->ang + l->angv);
        if (l->kind == TL_PILLAR && ((l->x < PX(16) && l->vx < 0) || (l->x > PX(TH_PF_W - 16) && l->vx > 0)))
            l->vx = -l->vx;                            /* sweeping pillars turn at the sides */
        if (l->kind == TL_COMET) {
            if ((uint32_t)(l->x + PX(80)) > (uint32_t)PX(TH_PF_W + 160) || (uint32_t)(l->y + PX(80)) > (uint32_t)PX(TH_PF_H + 160)) {
                *l = t->lasers[--t->nlasers]; --i; continue;
            }
            if ((l->t & 3) == 0) th_fx(t, TF_SPARK, l->color, l->x, l->y, -l->vx / 4, -l->vy / 4, 16);
        } else {
            if (act >= 0 && act < l->on) l->w = (int16_t)(l->w + (l->wmax + 7) / 8 > l->wmax ? l->wmax : l->w + (l->wmax + 7) / 8);
            else if (act >= l->on) {
                l->w = (int16_t)(l->w - (l->wmax + 9) / 10);
                if (l->w <= 0) { *l = t->lasers[--t->nlasers]; --i; continue; }
            }
        }
        /* collision with the player (the core only: ~60 % of the width) */
        dx = t->px - l->x; dy = t->py - l->y;
        if (l->kind == TL_COMET) {
            if (dx > -PX(6) && dx < PX(6) && dy > -PX(6) && dy < PX(6) && can_hit) hit = 1;
        } else if (act >= 0 && l->w * 2 >= l->wmax) {
            const int32_t half = (int32_t)l->w * TH_ONE * 3 / 10;
            if (l->kind == TL_PILLAR) {
                if (dx > -half && dx < half && can_hit) hit = 1;
            } else {
                const int32_t along = (int32_t)(((int64_t)dx * th_cos(l->ang) + (int64_t)dy * th_sin(l->ang)) >> 14);
                const int32_t perp  = (int32_t)(((int64_t)-dx * th_sin(l->ang) + (int64_t)dy * th_cos(l->ang)) >> 14);
                if (along >= 0 && along <= (int32_t)l->len * TH_ONE) {
                    if (perp > -half && perp < half && can_hit) hit = 1;
                    else if (perp > -half - PX(16) && perp < half + PX(16) && (l->t & 7) == 0 && t->mode == TH_PLAY) {
                        ++t->graze; t->score += 200; t->sfx |= TS_BIT(TS_GRAZE);   /* grazing a laser: every 8 ticks */
                    }
                }
            }
        }
    }
    if (hit) player_hit(t);
}

/* ---------------------------------------------------------------- bullets (hot loop) */
static void bullets_update(Th *t)
{
    ThBullet *b = t->bullets;
    const int d = (t->dbuf + 1) % TH_DRAW_BUFS;       /* this tick's tables (the engine may read the other two) */
    GfxItem *dst[TBK_COUNT];
    int n = t->nbullets, i, j, cnt[TBK_COUNT], hit = 0;
    int32_t bfx = 0, bfy = 0;
    const int32_t px = t->px, py = t->py;
    const int can_hit = !(t->invuln || t->bomb_t || t->dying || t->invincible) && t->mode == TH_PLAY;
    const int can_graze = t->mode == TH_PLAY && !t->dying, bot = t->bot;
    const uint32_t star_t = t->tick;
    /* screen mapping of the current view (see TH_SX / TH_SY), hoisted out of the loop */
    const int v_s2 = th_view.s2, v_ox = th_view.ox, v_oy = th_view.oy;
    const int v_w = 640 * v_s2 / 2, v_h = 480 * v_s2 / 2;
    for (j = 0; j < TBK_COUNT; j++) { dst[j] = t->draw[d][j]; cnt[j] = 0; }
    for (i = 0; i < n; i++) {
        ThBullet *q = &b[i];
        int32_t x, y, dx, dy;
        int bucket, half, sx, sy;
        if (q->timer) {
            --q->timer;
            if (q->acc || q->angv) {
                int s = q->spd + q->acc;
                q->spd = (int16_t)(s < 0 ? 0 : s);
                q->ang = (uint16_t)(q->ang + q->angv);
                q->vx = (int16_t)((q->spd * th_cos(q->ang)) >> 14);
                q->vy = (int16_t)((q->spd * th_sin(q->ang)) >> 14);
                if (q->angv) th_bullet_sprite(q);
            }
            if (!q->timer && (q->flags & TBF_REDIR)) {
                q->ang = (uint16_t)(((q->flags & TBF_AIM) ? th_atan2(py - q->y, px - q->x) : q->ang) + q->aux_ang);
                q->spd = q->aux_spd;
                q->acc = 0; q->angv = 0;
                q->vx = (int16_t)((q->spd * th_cos(q->ang)) >> 14);
                q->vy = (int16_t)((q->spd * th_sin(q->ang)) >> 14);
                q->flags &= (uint8_t)~TBF_REDIR;
                th_bullet_sprite(q);
            }
        }
        x = q->x + q->vx; y = q->y + q->vy;
        q->x = x; q->y = y;
        if ((q->flags & TBF_BOUNCE) && (x < 0 || x > PX(TH_PF_W))) {
            q->vx = (int16_t)-q->vx; q->ang = (uint16_t)(32768u - q->ang);
            q->flags &= (uint8_t)~TBF_BOUNCE;
            th_bullet_sprite(q);
        }
        /* outside the playfield + 40 px: remove (the last one takes the slot) */
        if ((uint32_t)(x + PX(40)) > (uint32_t)PX(TH_PF_W + 80) || (uint32_t)(y + PX(40)) > (uint32_t)PX(TH_PF_H + 80)) {
            b[i] = b[--n];
            --i;
            continue;
        }
        /* player: hit + graze (Q4 units, only when close); the bot's dodge force too */
        dx = x - px; dy = y - py;
        if ((uint32_t)(dx + PX(40)) < (uint32_t)PX(80) && (uint32_t)(dy + PX(40)) < (uint32_t)PX(80)) {
            int32_t ex = dx >> 4, ey = dy >> 4, d2 = ex * ex + ey * ey;
            int32_t r = brad16[q->kind] + PLAYER_R * 16, g = r + GRAZE_R * 16;
            if (d2 < r * r && can_hit) hit = 1;
            else if (d2 < g * g && !(q->flags & TBF_GRAZED) && can_graze) {
                q->flags |= TBF_GRAZED;
                ++t->graze;
                t->sfx |= TS_BIT(TS_GRAZE);
                t->score += 500;
                th_fx(t, TF_SPARK, TC_WHITE, x, y, -dx >> 3, -dy >> 3, 12);
            }
            if (bot) {
                const int32_t pdx = -dx >> 8, pdy = -dy >> 8, pd2 = pdx * pdx + pdy * pdy;
                if (pd2 && pd2 < 40 * 40) { bfx += pdx * 2048 / pd2; bfy += pdy * 2048 / pd2; }
            }
        }
        /* draw item, only if fully on screen: straight into its bucket's table */
        bucket = q->bk;
        half = t->diff==TH_STRESS?th_stress_half[bucket]:th_bk_hv[bucket];
        sx = ((x + (TH_PF_X << 8)) * v_s2) >> 9; sy = ((y + (TH_PF_Y << 8)) * v_s2) >> 9;   /* centre */
        /* cull with the full (square) box; the sprite may be a tight box inside it */
        if ((uint32_t)(sx - half) > (uint32_t)(v_w - 2 * half) || (uint32_t)(sy - half) > (uint32_t)(v_h - 2 * half)) continue;
        {
            GfxItem *it = &dst[bucket][cnt[bucket]++];
            const int spr = bucket == TBK_STAR ? q->sp + (int)(((star_t + q->spin) >> 2) & (TH_STAR_FRAMES - 1)) : q->sp;
            it->x = (int16_t)(sx + v_ox - (t->diff==TH_STRESS?th_stress_cx[bucket][spr]:th_bspr_cx[bucket][spr]));
            it->y = (int16_t)(sy + v_oy - (t->diff==TH_STRESS?th_stress_cy[bucket][spr]:th_bspr_cy[bucket][spr]));
            it->sprite = spr;
        }
    }
    t->nbullets = n;
    if (n > t->peak_bullets) t->peak_bullets = n;
    if (hit) player_hit(t);
    t->bot_fx = bfx; t->bot_fy = bfy;
    t->drawn = 0;
    for (j = 0; j < TBK_COUNT; j++) { t->dcount[d][j] = cnt[j]; t->drawn += cnt[j]; }
    t->dbuf = d;
}

/* ---------------------------------------------------------------- boss */
void th_boss_move_default(Th *t, int tick)
{
    ThBoss *b = &t->boss;
    if (tick % 200 == 100) {
        b->tx = PX(96 + (int)(th_rand(t) % 192u));
        b->ty = PX(80 + (int)(th_rand(t) % 70u));
    }
    b->x += (b->tx - b->x) / 24;
    b->y += (b->ty - b->y) / 24;
}

static void boss_phase_start(Th *t)
{
    ThBoss *b = &t->boss;
    const ThPhase *ph = cur_phase(t);
    b->phase_tick = 0;
    b->hp = b->maxhp = t->diff == TH_LUNATIC ? ph->hp * 3 / 2 : ph->hp;
    b->bonus = ph->bonus;
    b->spell_fail = 0;
    if (ph->name) { t->cutin_t = 110; t->sfx |= TS_BIT(TS_SPELL); }   /* spell card declaration: portrait cut-in */
    memset(&b->em, 0, sizeof b->em);
    b->tx = PX(TH_PF_W / 2); b->ty = PX(110);
}

static void boss_update(Th *t)
{
    ThBoss *b = &t->boss;
    const ThPhase *ph;
    int i;
    if (b->captured_show) --b->captured_show;          /* also after a mid-boss has left */
    if (!b->active) return;
    if (b->entering) {
        b->y += (PX(110) - b->y) / 20;
        b->x += (PX(TH_PF_W / 2) - b->x) / 20;
        if (--b->entering == 0) boss_phase_start(t);
        return;
    }
    if (b->leaving) {                                  /* mid-boss done: flies away, the stage goes on */
        b->y -= PX(4); b->x += (b->x < PX(TH_PF_W / 2) ? -PX(1) : PX(1));
        t->spell_bg = t->spell_bg > 8 ? t->spell_bg - 8 : 0;
        if (--b->leaving == 0) { b->active = 0; b->mid = 0; }
        return;
    }
    if (b->dying) {
        if (b->dying % 10 == 0)
            th_explode(t, b->x + (int32_t)(th_rand(t) % PX(80)) - PX(40), b->y + (int32_t)(th_rand(t) % PX(80)) - PX(40),
                       (int)(th_rand(t) % TC_COLORS), 1);
        if (b->dying % 10 == 0) t->sfx |= TS_BIT(TS_KILL);
        if (--b->dying == 0) {
            b->active = 0;
            t->sfx |= TS_BIT(TS_BOSSDEAD);
            t->flash = 255;
            t->shake = 40;
            th_explode(t, b->x, b->y, TC_WHITE, 1);
            for (i = 0; i < 16; i++) th_drop(t, i < 4 ? TI_BIGP : TI_POINT, b->x, b->y);
            th_drop(t, TI_LIFE, b->x, b->y);
            t->cleared_t = 1;
        }
        return;
    }
    ph = cur_phase(t);
    ++b->phase_tick;
    {
        const int left = ph->time_s * 60 + 60 - b->phase_tick;
        if (ph->name && left > 0 && left <= 600 && left % 60 == 0) t->sfx |= TS_BIT(TS_TIMEOUT);
    }
    if (ph->name && t->spell_bg < 255) t->spell_bg += 8;
    if (!ph->name && t->spell_bg > 0) t->spell_bg -= 8;
    if (t->spell_bg > 255) t->spell_bg = 255;
    if (t->spell_bg < 0) t->spell_bg = 0;
    if (ph->name && b->phase_tick > 60) {                 /* bonus falls to 1/3 over the time limit */
        uint32_t lim = (uint32_t)ph->time_s * 60u;
        uint32_t e = (uint32_t)b->phase_tick > lim ? lim : (uint32_t)b->phase_tick;
        b->bonus = ph->bonus - ph->bonus / (3u * lim) * 2u * e;      /* 32-bit only (no 64-bit divide on RV32) */
    }
    if (b->phase_tick > 60) ph->fn(t, &b->em, b->phase_tick - 60);
    {
        int32_t dx = b->x - t->px, dy = b->y - t->py;
        if (dx > -PX(16) && dx < PX(16) && dy > -PX(20) && dy < PX(20)) player_hit(t);
    }
    if (t->bomb_t) boss_damage(t, 2);
    if (b->hp <= 0 || b->phase_tick > ph->time_s * 60 + 60) {
        const int beaten = b->hp <= 0;
        if (ph->name && beaten && !b->spell_fail) {
            t->score += b->bonus;
            b->captured_show = 180;
            t->sfx |= TS_BIT(TS_CAPTURE);
            b->captured_bonus = (int)b->bonus;
        }
        th_clear_bullets(t, 1);
        for (i = 0; i < (beaten ? 6 : 2); i++) th_drop(t, i & 1 ? TI_POINT : TI_POWER, b->x, b->y);
        th_explode(t, b->x, b->y, TC_PURPLE, 0);
        t->sfx |= TS_BIT(TS_PHASE);
        {
            int np;
            th_phase_list(t, &np);
            if (++b->phase >= np && b->mid) {          /* mid-boss: leave */
                b->phase = np - 1;
                b->leaving = 60;
            } else if (b->phase >= np) {
                b->dying = 120;
                b->phase = np - 1;
            } else {
                boss_phase_start(t);
            }
        }
    }
}

/* ---------------------------------------------------------------- bot */
/* simple autoplay: always shoots, slides away from close bullets, follows the
 * target horizontally, focuses near the boss. Demo / stress only (invincible). */
static uint32_t bot_input(Th *t)
{
    /* dodge force of the bullets within 40 px: summed by bullets_update last tick
     * (no second walk over thousands of bullets here) */
    int32_t fx = t->bot_fx, fy = t->bot_fy, i, gx;
    uint32_t h = PAD_SHOT;
    gx = t->boss.active ? t->boss.x : PX(TH_PF_W / 2);
    for (i = 0; i < TH_MAX_ENEMIES; i++)
        if (t->enemies[i].active && t->enemies[i].y > 0) { gx = t->enemies[i].x; break; }
    fx += ((gx - t->px) >> 8) * 2;                     /* stay under the target (shots hit) */
    fy += (PX(380) - t->py) >> 8;
    if (fx > 6) h |= PAD_RIGHT;
    if (fx < -6) h |= PAD_LEFT;
    if (fy > 6) h |= PAD_DOWN;
    if (fy < -6) h |= PAD_UP;
    if (t->boss.active) h |= PAD_SLOW;
    return h;
}

/* ---------------------------------------------------------------- tick */
void th_tick(Th *t, uint32_t held, uint32_t pressed, int bot)
{
    ++t->tick;
    t->bot = bot;
    if (t->flash > 0) t->flash -= 8;
    if (t->flash < 0) t->flash = 0;
    if (t->shake) --t->shake;
    switch (t->mode) {
    case TH_TITLE:
        if (pressed & PAD_UP) t->menu = (t->menu + TH_MENU_N - 1) % TH_MENU_N;
        if (pressed & PAD_DOWN) t->menu = (t->menu + 1) % TH_MENU_N;
        if (pressed & (PAD_UP | PAD_DOWN)) t->sfx |= TS_BIT(TS_SELECT);
        if (bot && (t->tick % 240) == 0) {
            /* demo rotation: stress 1, normal 2, stress 2, normal 1 */
            static const uint8_t dd[4] = { TH_STRESS, TH_NORMAL, TH_STRESS, TH_NORMAL }, ds[4] = { 0, 1, 1, 0 };
            const int k = (int)(t->tick / 240u % 4u);
            th_start_stage(t, dd[k], ds[k]);
        } else if (pressed & (PAD_SHOT | PAD_START)) {
            static const uint8_t md[TH_MENU_N] = { TH_NORMAL, TH_LUNATIC, TH_NORMAL, TH_STRESS };
            th_start_stage(t, md[t->menu], t->menu == 2);
            t->sfx |= TS_BIT(TS_OK);
        }
        t->bg_y = (t->bg_y + 64) % PX(TH_PF_H);
        bullets_update(t);
        return;
    case TH_PAUSE:
        if (pressed & PAD_UP) t->menu = (t->menu + 2) % 3;
        if (pressed & PAD_DOWN) t->menu = (t->menu + 1) % 3;
        if (pressed & (PAD_UP | PAD_DOWN)) t->sfx |= TS_BIT(TS_SELECT);
        if (pressed & PAD_PAUSE) { t->mode = TH_PLAY; t->sfx |= TS_BIT(TS_OK); return; }
        if (pressed & (PAD_SHOT | PAD_START)) {
            t->sfx |= TS_BIT(TS_OK);
            if (t->menu == 0) t->mode = TH_PLAY;
            else if (t->menu == 1) th_start_stage(t, t->diff, t->start_stage);
            else { t->mode = TH_TITLE; t->menu = 0; t->nbullets = 0; }
        }
        return;
    case TH_CLEAR:
    case TH_OVER:
        ++t->respawn_t;
        if (t->score > t->hiscore) t->hiscore = t->score;
        if ((t->respawn_t > 60 && (pressed & (PAD_SHOT | PAD_START))) || (bot && t->respawn_t > 300) ||
            (t->mode == TH_CLEAR && t->respawn_t > 480)) {
            if (t->mode == TH_CLEAR && t->stage + 1 < TH_STAGES) { next_stage(t); return; }
            t->mode = TH_TITLE; t->menu = 0; t->nbullets = 0;
        }
        fx_update(t);
        bullets_update(t);
        return;
    default:
        break;
    }
    if (pressed & PAD_PAUSE) { t->mode = TH_PAUSE; t->menu = 0; t->sfx |= TS_BIT(TS_PAUSE); return; }
    t->invincible = bot || t->diff == TH_STRESS;
    t->held = bot ? bot_input(t) : held;
    t->pressed = bot ? 0u : pressed;
    if (!(t->boss.active && t->boss.mid)) ++t->stage_tick;    /* the stage clock stops for a mid-boss */
    t->bg_y = (t->bg_y + (t->boss.active ? 96 : 160)) % PX(TH_PF_H);
    if (t->banner_t) --t->banner_t;
    player_update(t);
    if (t->mode != TH_PLAY) return;
    {
        /* optional split of the tick (th_clock set by main.c): patterns / bullets / rest */
        const uint32_t c0 = th_clock ? th_clock() : 0u;
        uint32_t c1, c2;
        if (!t->boss_seen && !(t->boss.active && t->boss.mid)) th_stages[t->stage].timeline(t);
        boss_update(t);
        enemies_update(t);
        c1 = th_clock ? th_clock() : 0u;
        shots_update(t);
        bullets_update(t);
        if (t->nphantoms) phantoms_update(t);
        if (t->nlasers) lasers_update(t);
        if (t->moon_t) ++t->moon_t;
        c2 = th_clock ? th_clock() : 0u;
        items_update(t);
        fx_update(t);
        if (th_clock) {
            const uint32_t c3 = th_clock();
            t->prof[0] = c1 - c0; t->prof[1] = c2 - c1; t->prof[2] = c3 - c2;
        }
    }
    if (t->score > t->hiscore) t->hiscore = t->score;
    if (t->cleared_t && ++t->cleared_t > 240) {
        t->clear_bonus = (int)(t->lives * 3000000 + t->bombs * 1000000 + t->power * 1000 + t->graze * 1000);
        t->score += (uint32_t)t->clear_bonus;
        t->mode = TH_CLEAR;
        t->sfx |= TS_BIT(TS_CLEAR);
        t->respawn_t = 0;
    }
}

/* Rebuild current geometry without advancing time, collision or RNG; caller fenced GPU. */
void th_rebuild_draw(Th *t){
    int d=t->dbuf;memset(t->dcount[d],0,sizeof t->dcount[d]);
    for(int i=0;i<t->nbullets;i++){
        const ThBullet *b=&t->bullets[i];int k=b->bk;
        int sp=k==TBK_STAR?b->sp+(int)(((t->tick+b->spin)>>2)&(TH_STAR_FRAMES-1)):b->sp;
        int h=t->diff==TH_STRESS?th_stress_half[k]:th_bk_hv[k];
        int x=((b->x+(TH_PF_X<<8))*th_view.s2)>>9,y=((b->y+(TH_PF_Y<<8))*th_view.s2)>>9;
        int w=640*th_view.s2/2,vh=480*th_view.s2/2;
        if((uint32_t)(x-h)>(uint32_t)(w-2*h)||(uint32_t)(y-h)>(uint32_t)(vh-2*h))continue;
        GfxItem *it=&t->draw[d][k][t->dcount[d][k]++];
        it->x=x+th_view.ox-(t->diff==TH_STRESS?th_stress_cx[k][sp]:th_bspr_cx[k][sp]);
        it->y=y+th_view.oy-(t->diff==TH_STRESS?th_stress_cy[k][sp]:th_bspr_cy[k][sp]);it->sprite=sp;
    }
}
