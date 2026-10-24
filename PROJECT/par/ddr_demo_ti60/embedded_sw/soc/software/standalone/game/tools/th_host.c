/*
 * th_host.c - host run of the Touhou-style game (th_*.c) without a board:
 *   - plays whole games with the autoplay bot (Normal, Lunatic, Stress x32) and
 *     one scripted "human" game (mortal: deaths, bombs, game over);
 *   - every RENDER_EVERY ticks renders the frame twice, with the CPU backend and
 *     with the BitBlt backend running on the engine model (band mode on / off
 *     alternately), and compares the two framebuffers pixel for pixel;
 *   - writes PPM screenshots of interesting moments (title, waves, every boss
 *     phase, stress peak, clear / game over) to the directory given as argv[1];
 *   - prints bullet counts, draw-list words and the logic ops per tick;
 *   - runs the stress game with the 60 fps governor (th_stress_frame) against a
 *     made-up board cost model (frame us = base + k * bullets) for several k and
 *     counts the frames over the 16667 us budget.
 *
 *   gcc -O2 -no-pie -fno-pie -DBLIT_HOST_MODEL -Isrc -Itools -o th_host tools/th_host.c \
 *       src/th_tab.c src/th_game.c src/th_pattern.c src/th_stage2.c src/th_art.c src/th_render.c src/th_pix.c \
 *       src/gfx.c src/gfx_hw.c src/blit.c src/mini_snprintf.c
 *   ./th_host shots/        (PASS / FAIL)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/mman.h>
#define BLIT_MODEL_PTR(a) ((uint8_t *)(uintptr_t)(a))
#include "blit_model.h"
#include "gfx.h"
#include "gfx_hw.h"
#include "blit.h"
#include "th.h"
#include "pad.h"

#define DDR_LO 0x01000000u
#ifdef TH_HOST_HD                                   /* -DTH_HOST_HD: the 1.5x (720p) view, 1280x720 target */
#define W 1280
#define H 720
#else
#define W 640
#define H 480
#endif
#define FB_CPU 0x01000000u
#define FB_HW  0x01200000u
#define RENDER_EVERY 37

static Th th;
static const char *outdir;
static int fails, renders, band_renders, shots, tol_frames;
static uint32_t max_words, max_cmds;

static void ppm(const char *name, const Pixel *p)
{
    char path[512];
    FILE *f;
    int i;
    if (!outdir) return;
    snprintf(path, sizeof path, "%s/%s.ppm", outdir, name);
    f = fopen(path, "wb");
    if (!f) { perror(path); return; }
    fprintf(f, "P6\n%d %d\n255\n", W, H);
    for (i = 0; i < W * H; i++) {
        unsigned char c[3];
        c[0] = (unsigned char)(((p[i] >> 11) & 31) * 255 / 31);
        c[1] = (unsigned char)(((p[i] >> 5) & 63) * 255 / 63);
        c[2] = (unsigned char)((p[i] & 31) * 255 / 31);
        fwrite(c, 1, 3, f);
    }
    fclose(f);
    ++shots;
}

/* render the current state on both backends and compare */
static void render_both(const char *shot, int fps)
{
    Gfx gc, gh;
    GfxHwTarget t;
    ThRenderInfo ri;
    const int band = (renders & 1) == 0;
    uint32_t w0 = blit_st.lwords;
    blit_set_band(band);
    memset((void *)(uintptr_t)FB_CPU, 0, W * H * 2);
    memset((void *)(uintptr_t)FB_HW, 0x5A, W * H * 2);
    ri.fps = fps; ri.stats = 0;
    ri.alpha = shot || (renders % 4) != 3;    /* alpha effects on 3 of 4 renders (and every screenshot) */
    ri.hw = 0;
    gfx_begin(&gc, (Pixel *)(uintptr_t)FB_CPU, W, H);
    th_render(&th, &gc, ri);
    gfx_end(&gc);
    ri.hw = 0;                                   /* same text on both pictures */
    gfx_begin_hw(&gh, &t, FB_HW, W, H);
    th_render(&th, &gh, ri);
    gfx_end(&gh);
    blit_wait();
    if (blit_st.lwords - w0 > max_words) max_words = blit_st.lwords - w0;
    ++renders; band_renders += band;
    /* gfx_fill_alpha (spell background fade, menu overlays) is 5-bit alpha on the
     * CPU and 8-bit on the engine (gfx.h): those frames may differ by 1-2 LSB per
     * channel; everything else (keyed / additive / opaque) must be identical */
#ifdef TH_HOST_HD   /* 1.5x: more alpha layers overlap per pixel (moon + spell tint + aura): seen 3 LSB once */
    const int tol = (th.spell_bg > 0 && th.spell_bg < 255) || th.mode != TH_PLAY || ri.alpha ? 3 : 0;
#else
    const int tol = (th.spell_bg > 0 && th.spell_bg < 255) || th.mode != TH_PLAY || ri.alpha ? 2 : 0;
#endif
    int nd = 0, nexact = 0;
    {
        const Pixel *a = (const Pixel *)(uintptr_t)FB_CPU, *b = (const Pixel *)(uintptr_t)FB_HW;
        int i;
        for (i = 0; i < W * H; i++) if (a[i] != b[i]) {
            int dr = ((a[i] >> 11) & 31) - ((b[i] >> 11) & 31), dg = ((a[i] >> 5) & 63) - ((b[i] >> 5) & 63);
            int db = (a[i] & 31) - (b[i] & 31);
            ++nexact;
            if (dr < -tol || dr > tol || dg < -2 * tol || dg > 2 * tol || db < -tol || db > tol) ++nd;
        }
        if (nexact) ++tol_frames;
    }
    if (nd || bm_lerr || bm_fault || bm_berr) {
        const Pixel *a = (const Pixel *)(uintptr_t)FB_CPU, *b = (const Pixel *)(uintptr_t)FB_HW;
        int i, n = 0, first = -1;
        for (i = 0; i < W * H; i++) if (a[i] != b[i]) { if (first < 0) first = i; ++n; }
        printf("DIFF tick %u mode %d diff %d bullets %d band %d: %d pixels (first x %d y %d) lerr %u fault %u berr %u\n",
               th.tick, th.mode, th.diff, th.nbullets, band, n, first % W, first / W, bm_lerr, bm_fault, bm_berr);
        if (fails < 3) {
            char nm[64];
            snprintf(nm, sizeof nm, "diff%d_cpu", fails); ppm(nm, a);
            snprintf(nm, sizeof nm, "diff%d_hw", fails);  ppm(nm, b);
        }
        ++fails;
    }
    if (shot) ppm(shot, (const Pixel *)(uintptr_t)FB_HW);
}

typedef struct { const char *name; int diff; int bot; int max_ticks; int stage; } Run;

/* scripted player for the mortal run: zig-zags, focuses, bombs now and then */
static uint32_t human(uint32_t k, uint32_t *pressed)
{
    uint32_t h = PAD_SHOT;
    *pressed = 0;
    if ((k / 50) % 4 == 0) h |= PAD_LEFT;
    if ((k / 50) % 4 == 2) h |= PAD_RIGHT;
    if ((k / 120) % 3 == 1) h |= PAD_SLOW;
    if ((k / 30) % 5 == 0) h |= PAD_UP; else if ((k / 30) % 5 == 3) h |= PAD_DOWN;
    if (k % 700 == 350) *pressed |= PAD_BOMB;
    return h;
}

static void play(const Run *r)
{
    uint32_t k, peak = 0, pressed, held;
    int last_key = -1, boss_shots = 0, stress_shot = 0, wave_shot = 0, wave_stage = -1, stages_seen = 0, mids = 0;
    long logic_ns = 0, logic_max = 0;
    char nm[64];
    th_init(&th, 0xC0FFEEu);
    th.stress_auto = 0; th.stress_level = 11;          /* x32, no governor (no timing here) */
    /* title frame */
    th_tick(&th, 0, 0, 0);
    if (!strcmp(r->name, "normal")) render_both("00_title", 300);
    th_start_stage(&th, r->diff, r->stage);
    for (k = 0; k < (uint32_t)r->max_ticks; k++) {
        struct timespec t0, t1;
        long ns;
        int np;
        const ThPhase *phs;
        held = r->bot ? 0 : human(k, &pressed);
        if (r->bot) pressed = 0;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        th_tick(&th, held, pressed, r->bot);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        ns = (t1.tv_sec - t0.tv_sec) * 1000000000L + (t1.tv_nsec - t0.tv_nsec);
        logic_ns += ns; if (ns > logic_max) logic_max = ns;
        if ((uint32_t)th.nbullets > peak) peak = (uint32_t)th.nbullets;
        stages_seen |= 1 << th.stage;
        phs = th_phase_list(&th, &np);
        nm[0] = 0;
        if (wave_stage != th.stage) { wave_stage = th.stage; wave_shot = 0; }
        if (th.mode == TH_PLAY && !th.boss.active) {
            static const uint32_t at[2][4] = { { 400, 1250, 1900, 2800 }, { 400, 900, 1700, 2400 } };
            if (wave_shot < 4 && th.stage_tick == at[th.stage][wave_shot] && r->bot) {
                snprintf(nm, sizeof nm, "%s_s%d_1wave%d", r->name, th.stage + 1, wave_shot);
                ++wave_shot;
            }
        }
        if (th.boss.active && !th.boss.entering && !th.boss.leaving && th.boss.phase_tick == 300) {
            const int key = th.stage * 100 + th.boss.mid * 10 + th.boss.phase;
            if (key != last_key) {
                last_key = key;
                snprintf(nm, sizeof nm, "%s_s%d_2%s%d", r->name, th.stage + 1, th.boss.mid ? "mid" : "boss", th.boss.phase + 1);
                ++boss_shots;
                mids += th.boss.mid;
            }
        }
        if (th.boss.active && !th.boss.leaving && phs[th.boss.phase].name && th.boss.phase_tick == 22 && r->diff == TH_NORMAL)
            snprintf(nm, sizeof nm, "%s_s%d_cutin%d%s", r->name, th.stage + 1, th.boss.phase + 1, th.boss.mid ? "m" : "");
        if (th.boss.active && th.boss.phase_tick == 4 && th.boss.phase > 0 && r->diff == TH_STRESS)
            snprintf(nm, sizeof nm, "%s_s%d_cancel%d", r->name, th.stage + 1, th.boss.phase + 1);
        if (r->diff == TH_STRESS && !(stress_shot & (1 << th.stage)) && th.nbullets > 9000) {
            snprintf(nm, sizeof nm, "%s_s%d_peak", r->name, th.stage + 1);
            stress_shot |= 1 << th.stage;
        }
        if (th.mode == TH_CLEAR && th.respawn_t == 90) snprintf(nm, sizeof nm, "%s_s%d_3clear", r->name, th.stage + 1);
        if (th.mode == TH_OVER && th.respawn_t == 60) snprintf(nm, sizeof nm, "%s_3over", r->name);
        if (nm[0] || k % RENDER_EVERY == 0) render_both(nm[0] ? nm : 0, r->diff == TH_STRESS ? 140 : 300);
        if (th.mode == TH_CLEAR && th.stage == TH_STAGES - 1 && th.respawn_t > 100) break;
        if (th.mode == TH_OVER && th.respawn_t > 100) break;
        if (th.mode == TH_TITLE) break;
    }
    printf("%-8s ticks %5u mode %d stage %d | peak bullets %5u dropped %d | score %10u graze %5d lives %d bombs %d power %d"
           " | boss phase %d/%d (%d shots, %d mid) | host logic avg %ld us max %ld us\n",
           r->name, k, th.mode, th.stage + 1, peak, th.dropped, th.score, th.graze, th.lives, th.bombs, th.power,
           th.boss.phase + 1, *th_stages[th.stage].nphases, boss_shots, mids, logic_ns / 1000 / (long)(k ? k : 1), logic_max / 1000);
    if (r->bot && !(th.mode == TH_CLEAR && th.stage == TH_STAGES - 1)) { printf("  %s: bot run did not reach the last clear screen\n", r->name); ++fails; }
    if (r->bot && r->stage == 0 && stages_seen != 3) { printf("  %s: did not go on to stage 2\n", r->name); ++fails; }
    if (!r->bot && th.mode != TH_OVER && th.mode != TH_CLEAR) { printf("  %s: mortal run neither over nor clear\n", r->name); ++fails; }
}

/* engine source cache, band order (RTL: 512 direct-mapped lines of 32 B, INV
 * every frame; each 16-row band walks the items in list order): line requests
 * and misses of the bullet tables of the current frame. On the board one miss
 * costs ~90 engine clocks (30 k misses = 15 ms at 200 MHz before the sprite
 * layout was changed to bucket by bucket). */
static uint32_t cs_tag[512];
static uint8_t  cs_val[512];
static void cache_frame(uint32_t *req, uint32_t *miss)
{
    static const uint8_t order[TBK_COUNT] = { TBK_RICE0, TBK_RICE1, TBK_KUNAI0, TBK_KUNAI1, TBK_STAR,
                                              TBK_PELLET, TBK_ORB, TBK_BIG, TBK_BUBBLE };
    const int d = th.dbuf;
    int band, j, i, r;
    memset(cs_val, 0, sizeof cs_val);
    for (band = 0; band < 30; band++)
        for (j = 0; j < TBK_COUNT; j++) {
            const int bk = order[j];
            for (i = 0; i < th.dcount[d][bk]; i++) {
                const GfxItem *it = &th.draw[d][bk][i];
                const Sprite *s = &th_bspr[bk][it->sprite];
                if (s->width > 64) continue;                 /* not cached */
                for (r = 0; r < s->height; r++) {
                    const int y = it->y + r;
                    uintptr_t a, l;
                    if (y < band * 16 || y >= band * 16 + 16) continue;
                    a = (uintptr_t)s->pixels + (uintptr_t)r * (uintptr_t)s->width * 2u;
                    for (l = a >> 5; l <= (a + (uintptr_t)s->width * 2u - 1u) >> 5; l++) {
                        const unsigned idx = (unsigned)(l & 511u);
                        const uint32_t tg = (uint32_t)(l >> 9);
                        ++*req;
                        if (!cs_val[idx] || cs_tag[idx] != tg) { ++*miss; cs_val[idx] = 1; cs_tag[idx] = tg; }
                    }
                }
            }
        }
}

static void play_cache(int level)
{
    uint32_t k, req = 0, miss = 0, fr = 0, nb = 0;
    int ph = -1;
    th_init(&th, 0xBEEFu);
    th.stress_auto = 0; th.stress_level = level;
    th_start(&th, TH_STRESS);
    for (k = 0; k < 40000u && th.mode == TH_PLAY; k++) {
        const int p = th.boss.active ? th.boss.phase : -1;
        th_tick(&th, 0, 0, 1);
        if (p != ph || th.mode != TH_PLAY) {
            if (ph >= 0 && fr) {
                printf("  src cache x%d boss phase %d: bullets %5u, line requests %6u, misses %5u /frame (%u%%)\n",
                       th_stress_levels[level], ph + 1, nb / fr, req / fr, miss / fr, req ? miss * 100u / req : 0u);
                if (miss * 10u > req) { printf("  source cache: more than 10%% misses\n"); ++fails; }
            }
            ph = p; req = miss = fr = nb = 0;
        }
        if (p >= 0 && k % 10u == 0u) { cache_frame(&req, &miss); ++fr; nb += (uint32_t)th.nbullets; }
    }
}

/* stress governor against a cost model: one rendered frame per tick (vsync on) */
static void play_gov(int base_us, int ns_per_bullet, int start_level)
{
    uint32_t k, over = 0, over125 = 0, worst = 0, sum_b = 0, peak = 0, frames = 0;
    int lmin = 99, lmax = -1, lvl_hist[TH_STRESS_LEVELS];
    memset(lvl_hist, 0, sizeof lvl_hist);
    th_init(&th, 0xBEEFu);
    th.stress_level = start_level;
    th_start(&th, TH_STRESS);
    for (k = 0; k < 40000u && th.mode == TH_PLAY; k++) {
        uint32_t w;
        th_tick(&th, 0, 0, 1);
        /* cost of this frame: base + per bullet, +-4 % jitter */
        w = (uint32_t)base_us + (uint32_t)th.nbullets * (uint32_t)ns_per_bullet / 1000u;
        w = w * (96u + (th_rand(&th) % 9u)) / 100u;
        th_stress_frame(&th, w, 16667u);
        ++frames;
        if (w > 16667u) ++over;
        if (w > 16667u * 5u / 4u) ++over125;
        if (w > worst) worst = w;
        sum_b += (uint32_t)th.nbullets;
        if ((uint32_t)th.nbullets > peak) peak = (uint32_t)th.nbullets;
        if (th.stress_level < lmin) lmin = th.stress_level;
        if (th.stress_level > lmax) lmax = th.stress_level;
        ++lvl_hist[th.stress_level];
    }
    printf("governor  %4d us + %4d ns/bullet (60 fps limit ~%5d bullets): %u frames, over budget %u (%.2f%%), over 125%% %u,"
           " worst %u us | bullets avg %u peak %u | level x%d..x%d, downs %d ups %d, capped fires %d\n",
           base_us, ns_per_bullet, (16667 - base_us) * 1000 / ns_per_bullet, frames, over, over * 100.0 / frames, over125,
           worst, sum_b / frames, peak, th_stress_levels[lmin], th_stress_levels[lmax], th.gov_downs, th.gov_ups, th.dropped);
    if (over * 100u > frames) { printf("  governor: more than 1%% of the frames over budget\n"); ++fails; }
}

int main(int argc, char **argv)
{
    static const Run runs[] = {
        { "normal",  TH_NORMAL,  1, 80000, 0 },       /* stage 1 -> stage 2 -> all clear */
        { "lunatic", TH_LUNATIC, 1, 80000, 0 },
        { "stress",  TH_STRESS,  1, 80000, 0 },       /* manual x32 (set in play) */
        { "practice2", TH_NORMAL, 1, 40000, 1 },      /* title: stage 2 practice */
        { "mortal",  TH_NORMAL,  0, 40000, 0 },
    };
    void *m = mmap((void *)(uintptr_t)DDR_LO, 0x01000000u, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    unsigned i;
    if (m != (void *)(uintptr_t)DDR_LO) { perror("mmap"); return 2; }
    outdir = argc > 1 ? argv[1] : 0;
    blit_model_reset();
    if (!blit_init()) { printf("no engine model\n"); return 2; }
    blit_set_mode(BLIT_MODE_LIST);
#ifdef TH_HOST_HD
    th_set_view(1);
#endif
    th_art_init();
    printf("sprite pool %u of %u pixels, A8 pool %u of %u bytes\n", th_art_pool_used, 512u * 1024u, th_art_a8_used, 256u * 1024u);
    if (th_art_a8_used > 256u * 1024u - 4096u) { printf("A8 pool nearly full\n"); ++fails; }
    if (th_art_pool_used > 512u * 1024u - 4096u) { printf("sprite pool nearly full\n"); ++fails; }
    if (th_art2_a8_used > 640u * 1024u - 4096u) { printf("1.5x A8 pool nearly full\n"); ++fails; }
    if (th_art2_pool_used > 1280u * 1024u - 4096u) { printf("1.5x sprite pool nearly full\n"); ++fails; }
    printf("sprite pools: 1x %u px + A8 %u B, 1.5x %u px + A8 %u B\n", (unsigned)th_art_pool_used,
           (unsigned)th_art_a8_used, (unsigned)th_art2_pool_used, (unsigned)th_art2_a8_used);
    for (i = 0; i < sizeof runs / sizeof runs[0]; i++) play(&runs[i]);
    play_cache(11);                                    /* x32 */
    play_gov(3000, 1200, TH_STRESS_LEVEL_INIT);
    play_gov(3000, 2000, TH_STRESS_LEVEL_INIT);
    play_gov(6000, 3000, TH_STRESS_LEVELS - 1);
    play_gov(2000, 600, 0);
    printf("renders %d (band %d; %d with alpha-overlay LSB differences, within tolerance), max list words/frame %u,"
           " TAB records %u, screenshots %d\n", renders, band_renders, tol_frames, max_words, blit_st.ltabs, shots);
    (void)max_cmds;
    printf(fails ? "FAIL (%d)\n" : "PASS\n", fails);
    return fails != 0;
}
