/*
 * th_render.c - draws the Touhou-style game with the gfx API (CPU or BitBlt).
 *
 * Order: scrolling background (one opaque 384x448 copy, 128-bit wide path on
 * the engine) -> boss aura / boss / enemies -> items -> player shots (additive)
 * -> player + options -> enemy bullets (one gfx_blit_table per draw bucket:
 * keyed shapes first, glowing ones additive) -> hitbox -> effects (additive) ->
 * bomb / flash -> boss bar, spell name, banners -> frame + info panel (four
 * opaque copies of a pre-drawn 640x480 picture, covering whatever spilled out
 * of the playfield) -> numbers.
 */
#include "th.h"
#include "pad.h"
#include "mini_snprintf.h"
#include <string.h>

/* View (2026-10-21, th.h): everything is laid out in 640x480 units and scaled by
 * th_view (1x, or 1.5x for native 1280x720). SX / SY: playfield position (Q8) ->
 * screen; V(n): a size; PX0 / PY0 / PW / PH: the playfield on screen; txt / txt_r /
 * text_c / fa / fr / fadd: text and fills at 640x480 positions. */
#define SX(x) TH_SX(x)
#define SY(y) TH_SY(y)
#define V(n)  TV(n)
#define PX0   TVX(TH_PF_X)
#define PY0   TVY(TH_PF_Y)
#define PW    TV(TH_PF_W)
#define PH    TV(TH_PF_H)

static const Pixel C_WHITE = RGB565(240, 240, 255);
static const Pixel C_GOLD  = RGB565(255, 210, 110);

/* scratch lists for the per-object draws (copied into the command list, so
 * they may be reused right after the call) */
static GfxItem lst[2048];

static void text_c(Gfx *g, int cx, int y, const char *s, int scale, Pixel c)
{
    int sc = TTS(scale);                    /* font scales are whole numbers: 1 -> 2 at 1.5x */
    while (sc > 1 && gfx_text_width(s, sc) > PW - V(8)) --sc;   /* long line: keep it inside the playfield */
    gfx_text(g, TVX(cx) - gfx_text_width(s, sc) / 2, TVY(y), s, sc, c);
}
static void label_c(Gfx *g,int cx,int y,const char *s,int scale,Pixel c){
    int sc=TTS(scale);while(sc>1&&gfx_text_width(s,sc)>PW-V(8))--sc;
    gfx_label(g,TVX(cx)-gfx_text_width(s,sc)/2,TVY(y),s,sc,c);
}
static void txt(Gfx *g, int x, int y, const char *s, int scale, Pixel c)
{
    gfx_text(g, TVX(x), TVY(y), s, TTS(scale), c);
}
static void txt_r(Gfx *g, int xr, int y, const char *s, int scale, Pixel c)     /* right edge at xr */
{
    gfx_text(g, TVX(xr) - gfx_text_width(s, TTS(scale)), TVY(y), s, TTS(scale), c);
}
static void fa(Gfx *g, int x, int y, int w, int h, Pixel c, int a)
{
    gfx_fill_alpha(g, TVX(x), TVY(y), TV(w), TV(h), c, a);
}
static void fr(Gfx *g, int x, int y, int w, int h, Pixel c)
{
    gfx_fill_rect(g, TVX(x), TVY(y), TV(w), TV(h), c);
}
static void fadd(Gfx *g, int x, int y, int w, int h, Pixel c, int a)
{
    gfx_fill_add(g, TVX(x), TVY(y), TV(w), TV(h), c, a);
}

/* the background picture is 2 x 448 rows: a 448-row window as a Sprite of its own */
static Sprite bg_window(const Sprite *bg, int sy)
{
    Sprite w = *bg;
    w.pixels = bg->pixels + (uint32_t)sy * (uint32_t)bg->width;
    w.height = PH;
    return w;
}

static const Pixel lcol[TC_COLORS] = {
    RGB565(255, 60, 70), RGB565(255, 150, 50), RGB565(255, 230, 80), RGB565(80, 240, 110),
    RGB565(80, 230, 255), RGB565(90, 130, 255), RGB565(200, 100, 255), RGB565(240, 240, 255) };

/* stage 2: sky + moon, the sea of clouds one row per copy (pseudo-3D), moonlight
 * reflection, fog at the horizon, red tint for spell cards, the rising full moon */
static void draw_bg2(const Th *t, Gfx *g, int alpha)
{
    const int scroll = (int)((t->tick * 3u) >> 3);                       /* texture rows */
    int y, k;
    gfx_blit(g, PX0, PY0, &th_bg2_sky, 0);
    if (alpha) gfx_blit_alpha(g, TVX(TH_PF_X + 236), TVY(TH_PF_Y + 14), &th_spr_moon, 1, 255);
    else       gfx_blit_add(g, TVX(TH_PF_X + 236), TVY(TH_PF_Y + 14), &th_spr_moon, 1, 255);
    for (y = V(TH_BG2_HZ); y < PH; y++)                                  /* view rows */
        gfx_blit_part(g, PX0, PY0 + y, &th_bg2_level[th_bg2_row_level[y]], 0,
                      (th_bg2_row_z[y] + scroll) & (TH_BG2_TEX_H - 1), PW, 1, 0);
    for (k = 0; k < 10; k++) {                                           /* moonlight on the clouds */
        const int yy = TH_BG2_HZ + 4 + k * k * 3;
        const int xx = 236 + 48 + (int)((th_sin((uint16_t)(t->tick * 700u + (uint32_t)k * 9000u)) * (2 + k)) >> 14);
        const Sprite *gl = &th_spr_fx[k < 4 ? TF_GLOW16 : TF_GLOW32][TC_WHITE];
        gfx_blit_add(g, TVX(TH_PF_X + xx) - gl->width / 2, TVY(TH_PF_Y + yy) - gl->height / 2, gl, 0, 150 - k * 8);
    }
    if (alpha) {
        for (k = 0; k < 8; k++)                                          /* haze at the horizon */
            fa(g, TH_PF_X, TH_PF_Y + TH_BG2_HZ - 10 + k * 5, TH_PF_W, 5, RGB565(110, 120, 190), 150 - k * 18);
        if (t->spell_bg) fa(g, TH_PF_X, TH_PF_Y, TH_PF_W, TH_PF_H, RGB565(90, 0, 50), t->spell_bg * 90 / 255);
    }
    if (t->moon_t) {                                                     /* final spell: the full moon rises */
        const int up = t->moon_t < 200 ? t->moon_t : 200;
        const int cy = TH_PF_Y + 470 - up * 2, a = t->moon_t * 2 < 200 ? t->moon_t * 2 : 200;
        if (alpha) gfx_blit_alpha(g, TVX(TH_PF_X + TH_PF_W / 2 - 96), TVY(cy - 96), &th_spr_bigmoon, 1, a);
        else       gfx_blit_add(g, TVX(TH_PF_X + TH_PF_W / 2 - 96), TVY(cy - 96), &th_spr_bigmoon, 1, a);
    }
}

static void draw_background(const Th *t, Gfx *g, int alpha)
{
    if (t->stage == 1 && t->mode != TH_TITLE) { draw_bg2(t, g, alpha); return; }
    {
    int sy = (TH_PF_H - (t->bg_y >> 8)) % TH_PF_H;
    if (t->shake) sy = (sy + (int)((t->tick * 7u) % 5u)) % TH_PF_H;
    sy = V(sy);                                          /* texture row in view pixels */
    if (alpha && t->spell_bg > 0 && t->spell_bg < 255) {
        /* spell card background cross-fades over the normal one (full-playfield alpha copy) */
        const Sprite w = bg_window(&th_spr_bg_spell, sy);
        gfx_blit_part(g, PX0, PY0, &th_spr_bg, 0, sy, PW, PH, 0);
        gfx_blit_alpha(g, PX0, PY0, &w, 0, t->spell_bg);
    } else if (t->spell_bg < 128) {
        gfx_blit_part(g, PX0, PY0, &th_spr_bg, 0, sy, PW, PH, 0);
        if (t->spell_bg) fa(g, TH_PF_X, TH_PF_Y, TH_PF_W, TH_PF_H, 0, t->spell_bg * 2);
    } else {
        gfx_blit_part(g, PX0, PY0, &th_spr_bg_spell, 0, sy, PW, PH, 0);
        if (t->spell_bg < 255) fa(g, TH_PF_X, TH_PF_Y, TH_PF_W, TH_PF_H, 0, (255 - t->spell_bg) * 2);
    }
    }
}

static void draw_enemies(const Th *t, Gfx *g, int alpha)
{
    int i;
    const ThBoss *b = &t->boss;
    if (b->active) {
        const Sprite *a = &th_spr_boss_aura;
        int k = (int)(t->tick % 40u) < 20 ? 0 : 2;
        /* arms raised while a spell card runs */
        int np;
        const int cast = !b->entering && !b->dying && !b->leaving && th_phase_list(t, &np)[b->phase].name != 0;
        if (alpha && cast) {                  /* translucent magic circle turning behind the boss */
            const Sprite *m = &th_spr_magic[(t->tick >> 3) % TH_MAGIC_FRAMES];
            gfx_blit_alpha(g, SX(b->x) - V(64), SY(b->y) - V(60), m, 1, t->spell_bg);
        }
        gfx_blit_add(g, SX(b->x) - V(32 + k), SY(b->y) - V(32 + k), a, 0, 255);
        gfx_blit(g, SX(b->x) - V(32), SY(b->y) - V(40), &th_spr_boss[b->art][cast], 1);
    }
    for (i = 0; i < TH_MAX_ENEMIES; i++) {
        const ThEnemy *e = &t->enemies[i];
        const Sprite *s;
        if (!e->active) continue;
        s = &th_spr_enemy[e->kind][e->color & 3][((t->tick + (uint32_t)i * 3u) >> 3) & 1u];   /* wings flap */
        if (e->kind == TE_WISP) {                       /* ghost flame: A8 sprite, fades in, flickers */
            const int a = e->timer * 8 < 200 ? e->timer * 8 : 200 + (int)((t->tick + (uint32_t)i) & 7u) * 6;
            if (alpha) gfx_blit_alpha(g, SX(e->x) - s->width / 2, SY(e->y) - s->height / 2, s, 1, a);
            else       gfx_blit_add(g, SX(e->x) - s->width / 2, SY(e->y) - s->height / 2, s, 1, 255);
        } else {
            gfx_blit(g, SX(e->x) - s->width / 2, SY(e->y) - s->height / 2, s, 1);
        }
        if (e->flash) gfx_blit_add(g, SX(e->x) - V(16), SY(e->y) - V(16), &th_spr_fx[TF_GLOW32][TC_WHITE], 0, 128);
    }
}

static void draw_items(const Th *t, Gfx *g)
{
    int i, n = 0, ns = 0;
    static GfxItem stars[TH_MAX_ITEMS];
    for (i = 0; i < t->nitems; i++) {
        const ThItem *it = &t->items[i];
        if (it->kind == TI_STAR) {
            stars[ns].x = (int16_t)(SX(it->x) - V(4)); stars[ns].y = (int16_t)(SY(it->y) - V(4)); stars[ns].sprite = 0;
            ++ns;
        } else if (n < (int)(sizeof lst / sizeof lst[0])) {
            const int h = th_spr_item[it->kind].width / 2;
            lst[n].x = (int16_t)(SX(it->x) - h); lst[n].y = (int16_t)(SY(it->y) - h); lst[n].sprite = it->kind;
            ++n;
        }
    }
    gfx_blit_list(g, th_spr_item, lst, n, GFX_BLIT_KEY);
    gfx_blit_list(g, &th_spr_item[TI_STAR], stars, ns, GFX_BLIT_ADD);
}

static void draw_player(const Th *t, Gfx *g, int alpha)
{
    int i, n = 0;
    Sprite sh[2];
    sh[0] = th_spr_shot_main; sh[1] = th_spr_shot_opt;
    for (i = 0; i < TH_MAX_SHOTS; i++) {
        const ThShot *s = &t->shots[i];
        if (!s->active) continue;
        lst[n].x = (int16_t)(SX(s->x) - V(6));
        lst[n].y = (int16_t)(SY(s->y) - V(s->kind ? 6 : 12));
        lst[n].sprite = s->kind;
        ++n;
    }
    gfx_blit_list(g, sh, lst, n, GFX_BLIT_ADD);
    if (t->mode == TH_TITLE) return;
    if (t->mode == TH_OVER && t->lives == 0) return;
    if (alpha && t->focus && !t->dying)     /* focus: magic circle turning around the player */
        gfx_blit_alpha(g, SX(t->px) - V(32), SY(t->py) - V(32), &th_spr_focus[(t->tick >> 2) % TH_FOCUS_FRAMES], 1, 255);
    if (!t->dying) {
        const uint32_t h = t->held;
        const int f = (h & PAD_LEFT) ? 0 : (h & PAD_RIGHT) ? 2 : (t->tick & 16u) ? 3 : 1;   /* lean / idle */
        if (!t->invuln)  gfx_blit(g, SX(t->px) - V(16), SY(t->py) - V(24), &th_spr_player[f], 1);
        else if (alpha)  gfx_blit_alpha(g, SX(t->px) - V(16), SY(t->py) - V(24), &th_spr_player[f], 1,
                                        (t->tick & 8) ? 96 : 160);   /* invulnerable: translucent, pulsing */
        else if (!(t->tick & 4)) gfx_blit(g, SX(t->px) - V(16), SY(t->py) - V(24), &th_spr_player[f], 1);
    }
    for (i = 0; i < t->power / 100 && i < 4; i++)
        gfx_blit(g, SX(t->opt_x[i]) - V(8), SY(t->opt_y[i]) - V(8), &th_spr_option, 1);
}

static void draw_bullets(const Th *t, Gfx *g)
{
    static const uint8_t order[TBK_COUNT] = { TBK_RICE0, TBK_RICE1, TBK_KUNAI0, TBK_KUNAI1, TBK_STAR,
                                              TBK_PELLET, TBK_ORB, TBK_BIG, TBK_BUBBLE };
    const int d = t->dbuf;
    int j;
    for (j = 0; j < TBK_COUNT; j++) {
        const int b = order[j], n = t->dcount[d][b], h = t->diff==TH_STRESS?th_stress_half[b]:th_bk_hv[b];
        if (!n) continue;
        gfx_blit_table(g, t->diff==TH_STRESS?th_stress_bspr[b]:th_bspr[b], th_bspr_n[b], t->draw[d][b], n,
                       b == TBK_BIG || b == TBK_BUBBLE ? GFX_BLIT_ADD : GFX_BLIT_KEY,
                       (uint32_t)n * (uint32_t)(4 * h * h));
    }
}

/* phantom bullets: one alpha copy each (the visibility is the gameplay) */
static void draw_phantoms(const Th *t, Gfx *g, int alpha)
{
    int i;
    for (i = 0; i < t->nphantoms; i++) {
        const ThPhantom *p = &t->phantoms[i];
        const int a = th_phantom_alpha(p), hx = t->diff==TH_STRESS?th_stress_cx[p->bk][p->sp]:th_bspr_cx[p->bk][p->sp], hy = t->diff==TH_STRESS?th_stress_cy[p->bk][p->sp]:th_bspr_cy[p->bk][p->sp];
        const Sprite *s = t->diff==TH_STRESS?&th_stress_bspr[p->bk][p->sp]:&th_bspr[p->bk][p->sp];
        if (alpha)        gfx_blit_alpha(g, SX(p->x) - hx, SY(p->y) - hy, s, 1, a);
        else if (a >= 150) gfx_blit(g, SX(p->x) - hx, SY(p->y) - hy, s, 1);
    }
}

/* lasers: beads of additive glow along the beam (any angle, batched), translucent
 * warning dots, moonlight pillars as alpha + additive fills, comets with tails */
static void draw_lasers(const Th *t, Gfx *g, int alpha)
{
    static GfxItem lz[4][1536];
    static const uint8_t kinds[3] = { TF_GLOW16, TF_GLOW32, TF_GLOW64 }, hf[3] = { 8, 16, 32 };
    int n[4] = { 0, 0, 0, 0 }, i, k;
    for (i = 0; i < t->nlasers; i++) {
        const ThLaser *l = &t->lasers[i];
        const int warn = l->t < l->warn && l->kind != TL_COMET;
        if (l->kind == TL_PILLAR) {
            const int x = SX(l->x), lw = V(l->w);
            if (warn) {
                if (alpha) gfx_fill_alpha(g, x - V(1), PY0, V(3), PH, lcol[l->color], (l->t & 4) ? 150 : 90);
                else       gfx_fill_add(g, x - V(1), PY0, V(2), PH, lcol[l->color], 90);
            } else if (l->w > 0) {
                if (alpha) gfx_fill_alpha(g, x - lw / 2, PY0, lw, PH, lcol[l->color], 120);
                gfx_fill_add(g, x - lw / 4, PY0, lw / 2 > 1 ? lw / 2 : 1, PH, RGB565(200, 210, 255), 170);
            }
            continue;
        }
        {
            const int32_t cs = th_cos(l->ang), sn = th_sin(l->ang);
            if (l->kind == TL_COMET) {                       /* head + tail backwards */
                if (n[1] < 1500) { lz[1][n[1]].x = (int16_t)(SX(l->x) - V(16)); lz[1][n[1]].y = (int16_t)(SY(l->y) - V(16)); lz[1][n[1]++].sprite = l->color; }
                for (k = 0; k < 8 && n[0] < 1500; k++) {
                    const int d = k * 6;                     /* playfield px; (d * cs) >> 6 = Q8 */
                    lz[0][n[0]].x = (int16_t)(SX(l->x - ((d * cs) >> 6)) - V(8));
                    lz[0][n[0]].y = (int16_t)(SY(l->y - ((d * sn) >> 6)) - V(8));
                    lz[0][n[0]++].sprite = k < 2 ? TC_WHITE : l->color;
                }
                continue;
            }
            if (warn) {                                      /* translucent dotted line */
                const Sprite *dot = &th_bspr[TBK_PELLET][l->color];
                for (k = 14; k < l->len; k += 14) {
                    const int x = SX(l->x + ((k * cs) >> 6)), y = SY(l->y + ((k * sn) >> 6));
                    if (x < PX0 - V(8) || x > PX0 + PW + V(8) || y < PY0 - V(8) || y > PY0 + PH + V(8)) break;
                    if (alpha) gfx_blit_alpha(g, x - V(4), y - V(4), dot, 1, (l->t & 4) ? 160 : 90);
                    else if (n[0] < 1500) { lz[0][n[0]].x = (int16_t)(x - V(8)); lz[0][n[0]].y = (int16_t)(y - V(8)); lz[0][n[0]++].sprite = l->color; }
                }
                continue;
            }
            if (l->w <= 0) continue;
            {
                const int sz = l->w < 10 ? 0 : l->w < 28 ? 1 : 2, step = sz == 2 ? 14 : sz == 1 ? 8 : 6;
                for (k = 0; k < l->len; k += step) {
                    const int x = SX(l->x + ((k * cs) >> 6)), y = SY(l->y + ((k * sn) >> 6));
                    if (x < PX0 - V(16) || x > PX0 + PW + V(16) || y < PY0 - V(16) || y > PY0 + PH + V(16)) break;
                    if (n[sz] < 1500) { lz[sz][n[sz]].x = (int16_t)(x - V(hf[sz])); lz[sz][n[sz]].y = (int16_t)(y - V(hf[sz])); lz[sz][n[sz]++].sprite = l->color; }
                    if (l->w >= 6 && (k % 8) < step && n[3] < 1500) {
                        lz[3][n[3]].x = (int16_t)(x - V(8)); lz[3][n[3]].y = (int16_t)(y - V(8)); lz[3][n[3]++].sprite = TC_WHITE;
                    }
                }
            }
        }
    }
    for (k = 0; k < 3; k++) gfx_blit_list(g, th_spr_fx[kinds[k]], lz[k], n[k], GFX_BLIT_ADD);
    gfx_blit_list(g, th_spr_fx[TF_GLOW16], lz[3], n[3], GFX_BLIT_ADD);
}

static void draw_fx(const Th *t, Gfx *g)
{
    /* one pass over the (dense) effect array, sorted into one list per kind */
    static GfxItem fl[TF_KINDS][TH_MAX_FX];
    static const uint8_t half[TF_KINDS] = { 4, 8, 16, 32, 32 };
    int k, i, n[TF_KINDS] = { 0, 0, 0, 0, 0 };
    for (i = 0; i < t->nfx; i++) {
        const ThFx *f = &t->fx[i];
        GfxItem *it;
        if (f->life < 8 && (f->life & 1)) continue;          /* fade out by blinking */
        k = f->kind;
        it = &fl[k][n[k]++];
        it->x = (int16_t)(SX(f->x) - V(half[k])); it->y = (int16_t)(SY(f->y) - V(half[k]));
        it->sprite = f->color;
    }
    for (k = 0; k < TF_KINDS; k++) gfx_blit_list(g, th_spr_fx[k], fl[k], n[k], GFX_BLIT_ADD);
}

/* fading ghosts of cancelled bullets / shot-down fairies / the player (alpha only) */
static void draw_ghosts(const Th *t, Gfx *g)
{
    int i;
    for (i = 0; i < t->nghosts; i++) {
        const ThGhost *gh = &t->ghosts[i];
        gfx_blit_alpha(g, SX(gh->x) - gh->s->width / 2, SY(gh->y) - gh->s->height / 2, gh->s, 1,
                       200 * gh->life / gh->total);
    }
}

/* spell card declaration: the boss portrait slides in from the right, holds, then
 * drifts up and fades (A8 plane x global alpha); a dark translucent bar under the name */
static void draw_cutin(const Th *t, Gfx *g)
{
    const Sprite *p = &th_spr_portrait[t->boss.art];
    const int e = 110 - t->cutin_t;                          /* ticks since the declaration */
    int x = TH_PF_X + TH_PF_W - 150, y = TH_PF_Y + 120, a;
    if (e < 16) x += (16 - e) * (16 - e);                    /* ease in */
    if (t->cutin_t < 30) y -= (30 - t->cutin_t) * 2;
    a = e * 16 < 210 ? e * 16 : 210;
    if (t->cutin_t * 7 < a) a = t->cutin_t * 7;
    gfx_blit_alpha(g, TVX(x), TVY(y), p, 1, a);
}

static void draw_bomb(const Th *t, Gfx *g)
{
    int k;
    if (t->bomb_t) {
        const int r = (150 - t->bomb_t) * 3;
        for (k = 0; k < 8; k++) {
            const uint16_t a = (uint16_t)(k * 8192 + t->tick * 600u);
            const int x = SX(t->px + (int32_t)((r * th_cos(a)) >> 6)) - V(32);
            const int y = SY(t->py + (int32_t)((r * th_sin(a)) >> 6)) - V(32);
            gfx_blit_add(g, x, y, &th_spr_bomb_ring, 0, 255);
        }
    }
    if (t->flash) fadd(g, TH_PF_X, TH_PF_Y, TH_PF_W, TH_PF_H, C_WHITE, t->flash / 2);
}

static void draw_boss_hud(const Th *t, Gfx *g, int alpha)
{
    const ThBoss *b = &t->boss;
    char s[160];
    int i;
    int np;
    const ThPhase *phs = th_phase_list(t, &np);
    if (b->active && !b->entering && !b->dying && !b->leaving && b->maxhp) {
        const ThPhase *ph = &phs[b->phase];
        const int w = (TH_PF_W - 64) * (b->hp > 0 ? b->hp : 0) / b->maxhp;
        const int left = (ph->time_s * 60 + 60 - b->phase_tick) / 60;
        if (alpha) fa(g, TH_PF_X + 6, TH_PF_Y + 4, TH_PF_W - 60, 8, 0, 120);   /* bar backing */
        fr(g, TH_PF_X + 8, TH_PF_Y + 6, w, 4, ph->name ? RGB565(255, 90, 90) : RGB565(240, 240, 255));
        for (i = b->phase + 1; i < np; i++)
            if (phs[i].name) gfx_blit(g, TVX(TH_PF_X + 8 + (i - b->phase - 1) * 12), TVY(TH_PF_Y + 12), &th_spr_life, 1);
        snprintf(s, sizeof s, "%02d", left < 0 ? 0 : left);
        txt(g, TH_PF_X + TH_PF_W - 40, TH_PF_Y + 2, s, 2, left < 10 ? RGB565(255, 90, 90) : C_WHITE);
        if (ph->name) {
            const int nw = gfx_text_width(ph->name, TTS(1));          /* view pixels */
            if (alpha) gfx_fill_alpha(g, TVX(TH_PF_X + TH_PF_W - 8) - nw - V(6), TVY(TH_PF_Y + 19),
                                      nw + V(12), V(23), RGB565(40, 0, 30), 150);
            txt_r(g, TH_PF_X + TH_PF_W - 8, TH_PF_Y + 22, ph->name, 1, C_GOLD);
            snprintf(s, sizeof s, "奖励 %u", b->spell_fail ? 0u : (unsigned)b->bonus);
            txt_r(g, TH_PF_X + TH_PF_W - 8, TH_PF_Y + 32, s, 1, C_WHITE);
        }
    }
    if (b->captured_show) {
        label_c(g, TH_PF_X + TH_PF_W / 2, TH_PF_Y + 100, "获得符卡奖励", 2, C_GOLD);
        snprintf(s, sizeof s, "%u", (unsigned)b->captured_bonus);
        text_c(g, TH_PF_X + TH_PF_W / 2, TH_PF_Y + 122, s, 2, C_WHITE);
    }
    if (t->banner && t->banner_t) {
        text_c(g, TH_PF_X + TH_PF_W / 2, TH_PF_Y + 180, t->banner, 3, C_GOLD);
        if (t->banner == th_stages[t->stage].banner)           /* stage title: subtitle under it */
            text_c(g, TH_PF_X + TH_PF_W / 2, TH_PF_Y + 212, th_stages[t->stage].subtitle, 2, RGB565(220, 225, 255));
    }
}

static void draw_frame(const Th *t, Gfx *g, ThRenderInfo info)
{
    const Sprite *f = &th_spr_frame;
    char s[160];
    int i;
    /* the frame picture covers the whole target (720p: with the side strips): the four
     * parts around the playfield, at the same position in the picture and on screen */
    {
        const int fw = f->width, fh = f->height;
        gfx_blit_part(g, 0, 0, f, 0, 0, fw, PY0, 0);
        gfx_blit_part(g, 0, PY0 + PH, f, 0, PY0 + PH, fw, fh - PY0 - PH, 0);
        gfx_blit_part(g, 0, PY0, f, 0, PY0, PX0, PH, 0);
        gfx_blit_part(g, PX0 + PW, PY0, f, PX0 + PW, PY0, fw - PX0 - PW, PH, 0);
    }
    snprintf(s, sizeof s, "%010u", (unsigned)t->hiscore);
    txt_r(g, 616, 53, s, 2, RGB565(200, 200, 220));
    snprintf(s, sizeof s, "%010u", (unsigned)t->score);
    txt_r(g, 616, 89, s, 2, C_WHITE);
    for (i = 0; i < t->lives && i < 8; i++) gfx_blit(g, TVX(530 + i * 12), TVY(109), &th_spr_life, 1);
    for (i = 0; i < t->bombs && i < 8; i++) gfx_blit(g, TVX(530 + i * 12), TVY(139), &th_spr_bombicon, 1);
    snprintf(s, sizeof s, "%d.%02d/4.00", t->power / 100, t->power % 100);
    txt_r(g, 616, 181, s, 2, RGB565(255, 160, 140));
    snprintf(s, sizeof s, "%d", t->graze);
    txt_r(g, 616, 211, s, 2, C_WHITE);
    snprintf(s, sizeof s, "%d", t->points_got);
    txt_r(g, 616, 241, s, 2, RGB565(140, 190, 255));
    snprintf(s, sizeof s, "弹幕 %d", t->nbullets);
    txt(g, 440, 280, s, 2, RGB565(255, 200, 200));
    if (t->diff == TH_STRESS) {
        snprintf(s, sizeof s, "压力 X%d %s", t->dens, t->stress_auto ? "自动" : "手动 - +");
        txt(g, 440, 300, s, 1, RGB565(255, 120, 120));
        snprintf(s, sizeof s, t->stress_auto ? "负载 %d%% 上限 %d" : "负载 %d%%",
                 t->gov_budget ? (int)(t->gov_last * 100u / t->gov_budget) : 0, t->stress_cap);
        txt(g, 440, 311, s, 1, RGB565(255, 170, 150));
    }
    else if (t->bot) txt(g, 440, 302, "自动演示", 1, RGB565(180, 180, 200));
    else if (t->diff == TH_LUNATIC) txt(g, 440, 302, "极难模式", 1, RGB565(255, 120, 200));
    snprintf(s, sizeof s, "渲染%d帧/秒 %s", info.fps, info.hw ? "硬件" : "处理器");
    txt_r(g, 640 - 8, 480 - 30, s, 2, C_WHITE);
}

static void draw_menus(const Th *t, Gfx *g)
{
    const char *diffs[TH_MENU_N] = { "开始 普通", "开始 极难", "第二关练习", 0 };
    char sl[32];
    static const char *const pause[3] = { "继续", "重新开始", "标题" };
    const int cx = TH_PF_X + TH_PF_W / 2;
    char s[160];
    int i;
    snprintf(sl, sizeof sl, "压力 X%d %s", th_stress_levels[t->stress_level], t->stress_auto ? "自动" : "");
    diffs[3] = sl;
    if (t->mode == TH_TITLE) {
        fa(g, TH_PF_X, TH_PF_Y, TH_PF_W, TH_PF_H, 0, 96);
        label_c(g, cx, 90, "东方", 6, C_GOLD);
        label_c(g, cx, 140, "弹幕游戏", 5, RGB565(255, 230, 200));
        label_c(g, cx, 186, "TI60 硬件加速", 1, RGB565(200, 200, 220));
        for (i = 0; i < TH_MENU_N; i++)
            (i<3?label_c:text_c)(g, cx, 240 + i * 30, diffs[i], 2, i == t->menu ? C_GOLD : RGB565(150, 150, 170));
        label_c(g, cx, 400, "方向移动 Z射击 X符卡 SHIFT低速", 1, RGB565(180, 180, 200));
        label_c(g, cx, 414, "WASD移动 1符卡 2低速", 1, RGB565(140, 140, 160));
    } else if (t->mode == TH_PAUSE) {
        fa(g, TH_PF_X, TH_PF_Y, TH_PF_W, TH_PF_H, 0, 140);
        label_c(g, cx, 170, "暂停", 4, C_GOLD);
        for (i = 0; i < 3; i++) label_c(g, cx, 230 + i * 30, pause[i], 2, i == t->menu ? C_GOLD : RGB565(150, 150, 170));
    } else if (t->mode == TH_OVER) {
        fa(g, TH_PF_X, TH_PF_Y, TH_PF_W, TH_PF_H, 0, 140);
        label_c(g, cx, 200, "游戏结束", 4, RGB565(255, 90, 90));
        snprintf(s, sizeof s, "得分 %u", (unsigned)t->score);
        text_c(g, cx, 250, s, 2, C_WHITE);
    } else if (t->mode == TH_CLEAR) {
        fa(g, TH_PF_X, TH_PF_Y, TH_PF_W, TH_PF_H, 0, 120);
        text_c(g, cx, 170, t->stage + 1 < TH_STAGES ? "关卡通过" : "全部通关", 4, C_GOLD);
        snprintf(s, sizeof s, "通关奖励 %d", t->clear_bonus);
        text_c(g, cx, 230, s, 2, C_WHITE);
        snprintf(s, sizeof s, "得分 %u", (unsigned)t->score);
        text_c(g, cx, 260, s, 2, C_WHITE);
        snprintf(s, sizeof s, "擦弹 %d 收集 %d", t->graze, t->points_got);
        text_c(g, cx, 290, s, 1, RGB565(200, 200, 220));
    }
}

void th_render(const Th *t, Gfx *g, ThRenderInfo info)
{
    const int alpha = info.alpha;
    draw_background(t, g, alpha);
    if (t->mode != TH_TITLE) {
        if (alpha && t->cutin_t) draw_cutin(t, g);
        draw_enemies(t, g, alpha);
        draw_items(t, g);
        draw_player(t, g, alpha);
    }
    draw_bullets(t, g);
    if (t->nphantoms) draw_phantoms(t, g, alpha);
    if (t->nlasers) draw_lasers(t, g, alpha);
    if (t->mode == TH_PLAY || t->mode == TH_PAUSE) {
        if (t->focus && !t->dying) gfx_blit(g, SX(t->px) - V(8), SY(t->py) - V(8), &th_spr_hitbox, 1);
    }
    if (alpha && t->mode != TH_TITLE) draw_ghosts(t, g);
    draw_fx(t, g);
    draw_bomb(t, g);
    if (t->mode != TH_TITLE) draw_boss_hud(t, g, alpha);
    draw_menus(t, g);
    draw_frame(t, g, info);
}
