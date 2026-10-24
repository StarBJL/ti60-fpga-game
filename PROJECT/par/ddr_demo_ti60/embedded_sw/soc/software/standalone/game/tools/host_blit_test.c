/*
 * Host test of the BitBlt backend: renders the same game states and demo
 * scenes once with the CPU backend (gfx.c) and once through gfx_hw.c + blit.h
 * into the C model of the engine (blit_model.h), and compares the pixels.
 *
 *   gcc -O2 -no-pie -fno-pie -DBLIT_HOST_MODEL -DGFX_FONT_ATLAS_CPU_MAX_SCALE=3 \
 *       -Isrc -Itools -o host_blit_test \
 *       tools/host_blit_test.c src/game.c src/gfx.c src/render.c src/demo.c \
 *       src/gfx_hw.c src/blit.c src/mini_snprintf.c
 *   ./host_blit_test [outdir]      (writes <outdir>/<name>_{cpu,hw}.raw RGB565)
 *
 * GFX_FONT_ATLAS_CPU_MAX_SCALE=3 makes the CPU backend take the same text path
 * as the engine, so the FILL/BLIT counters printed in the stats panel are equal
 * (by default the CPU draws scale 2/3 text with fills: same glyph pixels, but
 * other counter values, and that one HUD line would differ).
 *
 * Every engine frame is also drawn twice without the stats panel (it names the
 * mode), with the hostile seeds as a batched draw list (gfx_blit_list ->
 * hw_copy_list, info.batch = 1) and one gfx_blit per seed (info.batch = 0):
 * pixels and command counts must be identical. The draw-list frame is also
 * drawn through the engine's command list (BLIT_MODE_LIST: every command a
 * DDR list record, seeds DRAW words; BLIT_MODE_WORD: seeds via LIST_WORD):
 * same pixels, same command count, and the model must have executed exactly
 * the commands the driver counted (no list word lost or decoded wrong).
 * Build once more with -DBLIT_LIST_WORDS=300 -DBLIT_LIST_KICK=7 to exercise
 * buffer wrap-around and odd kick points.
 *
 * Without alpha effects both backends must match exactly. With alpha the CPU
 * uses 5-bit alpha and the engine 8-bit, so small differences are expected
 * (reported as max difference per channel in 5/6-bit units).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>

#define BLIT_MODEL_PTR(a) ((uint8_t *)(uintptr_t)(a))
#include "blit_model.h"

#include "game.h"
#include "gfx.h"
#include "gfx_hw.h"
#include "blit.h"
#include "render.h"
#include "demo.h"

#define DDR_LO  0x01000000u
#define DDR_SZ  0x01000000u
#define FB_CPU  0x01000000u
#define FB_HW   0x01200000u
#define FB_SRC  0x01400000u
#define IMG     0x01600000u
#define FB_HW1  0x01C00000u          /* engine, no stats panel, seeds one gfx_blit each */
#define FB_HW2  0x01D00000u          /* engine, no stats panel, seeds as a draw list */
#define FB_HW3  0x01E00000u          /* same through the DDR command list */
#define FB_HW4  0x01F00000u          /* same, seeds through LIST_WORD */
#define NPIX    (SCREEN_W * SCREEN_H)

static Game game;
static const char *outdir;
static int failures;
static int held_frames, held_isolated;  /* '8' held-list frames tested / not broken by a wrap */

static void dump(const char *name, const char *tag, uint32_t fb)
{
    char path[512];
    FILE *f;
    if (!outdir) return;
    snprintf(path, sizeof(path), "%s/%s_%s.raw", outdir, name, tag);
    f = fopen(path, "wb");
    if (!f) return;
    fwrite((void *)(uintptr_t)fb, 2, NPIX, f);
    fclose(f);
}

/* scene: 0 game, 1 squares, 2 images */
static void draw(int hw, uint32_t fb, int scene, RenderInfo info, uint32_t frame)
{
    Gfx g;
    GfxHwTarget t;
    /* garbage in the target first: every pixel must be drawn by the frame */
    memset((void *)(uintptr_t)fb, 0xA5, NPIX * 2);
    if (hw) gfx_begin_hw(&g, &t, fb, SCREEN_W, SCREEN_H);
    else    gfx_begin(&g, (Pixel *)(uintptr_t)fb, SCREEN_W, SCREEN_H);
    if (scene == 1)      demo_squares_render(&g, info, 512);
    else if (scene == 2) demo_images_render(&g, info, frame, 1);
    else                 render_game(&game, &g, info);
    gfx_end(&g);
    if (hw) blit_wait();
}

/* draw-list frame in another submission mode: returns pixel differences to ref.
 * band = 1: the frame as a band frame (BANDS .. ENDB, engine band mode D): same
 * pixels and commands, exactly one frame counted, nothing that would hang the
 * engine (bm_fault), every drawing command inside the frame. */
static int band_frames, band_splits;
static int draw_mode(int mode, int band, uint32_t fb, uint32_t ref, int scene, RenderInfo info, uint32_t frame,
                     uint32_t cmds_ref, char *why, size_t whylen)
{
    const uint16_t *c = (const uint16_t *)(uintptr_t)ref, *d = (const uint16_t *)(uintptr_t)fb;
    uint32_t cmds0, inv0, push0, cmds, pushed;
    uint32_t fr0 = bm_frames, bc0 = bm_band_cmds, bo0 = bm_band_other, sp0 = blit_st.bsplits, seq0;
    int i, diff = 0, bad = 0;
    if (blit_set_mode(mode) != mode) { snprintf(why, whylen, " mode %d refused", mode); return 1; }
    if (band && !blit_set_band(1)) { snprintf(why, whylen, " band mode refused"); return 1; }
    cmds0 = blit_st.cmds; inv0 = blit_st.cinvs; push0 = bm_pushed; seq0 = blit_st.bseq;
    info.batch = 1 + mode;
    draw(1, fb, scene, info, frame);
    cmds = blit_st.cmds - cmds0; pushed = bm_pushed - push0;
    if (band) {
        const uint32_t sp = blit_st.bsplits - sp0;
        /* one frame; a split (small list buffer) ends it early, the rest is drawn directly */
        if (bm_frames - fr0 != 1u || blit_st.bseq - seq0 != 1u || !blit_band_done(blit_st.bseq) ||
            bm_fault || bm_berr || blit_st.band_open || bm_band_on ||
            (!sp && (bm_band_other != bo0 || bm_band_cmds - bc0 != cmds - (blit_st.cinvs - inv0))))
            bad = 1;
        ++band_frames;
        band_splits += (int)sp;
    }
    blit_set_mode(BLIT_MODE_APB);
    for (i = 0; i < NPIX; i++) diff += c[i] != d[i];
    why[0] = 0;
    if (pushed != cmds) cmds = ~0u;                     /* every driver command reached the model */
    else cmds -= blit_st.cinvs - inv0;                  /* drawing commands only */
    if (bad || diff || cmds != cmds_ref || bm_lptr != bm_lend || bm_lmask || bm_lerr) {
        snprintf(why, whylen, "  mode %d%s: diff px %d cmds %u (want %u) model pushed %u ptr %x end %x mask %x lerr %u"
                 " | frames %u fault %u berr %u in-frame %u other %u",
                 mode, band ? " band" : "", diff, cmds, cmds_ref, pushed, bm_lptr, bm_lend, bm_lmask, bm_lerr,
                 bm_frames - fr0, bm_fault, bm_berr, bm_band_cmds - bc0, bm_band_other - bo0);
        return 1;
    }
    return 0;
}

/* drawing commands so far: the source-cache INVs (size-0 commands) are left out,
 * their number depends on when font atlas strips happen to be built */
static uint32_t ncmds(void) { return blit_st.cmds - blit_st.cinvs; }

static void compare(const char *name, int scene, int alpha_fx, uint32_t frame)
{
    RenderInfo info;
    const uint16_t *a = (const uint16_t *)(uintptr_t)FB_CPU, *b = (const uint16_t *)(uintptr_t)FB_HW;
    int i, diff = 0, maxd = 0, first = -1, ldiff = 0;
    uint32_t cmds0, cmds1, cmds2;

    memset(&info, 0, sizeof(info));
    info.fps = 60; info.ms = 51; info.logic_us = 800; info.render_us = 4300;
    info.work_us = 5100; info.show_stats = 1; info.mouse_x = -1; info.mouse_y = -1;
    info.alpha_fx = alpha_fx; info.hw_avail = 1; info.hw = 1;
    info.fps_mode[0] = 37; info.draw_mode[0] = 25123; info.fps_mode[1] = 60; info.draw_mode[1] = 3210;
    info.submit_us = 1200; info.qstall_us = 0; info.tail_us = 2000; info.vsync_us = 11500;
    info.logic_max_us = 2100; info.blt = 35; info.q_end = 34; info.q_peak = 120; info.q_depth = 256;
    info.rd_mbs = 61; info.wr_mbs = 83;
    info.seeds_us = 900; info.hud_us = 250; info.scene_us = 50;

    info.show_stats = 0;                /* list vs calls, same picture otherwise */
    cmds0 = ncmds();
    info.batch = 0;
    draw(1, FB_HW1, scene, info, frame);
    cmds1 = ncmds() - cmds0;
    cmds0 = ncmds();
    info.batch = 1;
    draw(1, FB_HW2, scene, info, frame);
    cmds2 = ncmds() - cmds0;
    {
        const uint16_t *c = (const uint16_t *)(uintptr_t)FB_HW1, *d = (const uint16_t *)(uintptr_t)FB_HW2;
        for (i = 0; i < NPIX; i++) ldiff += c[i] != d[i];
    }
#if SPROUT_STATS                    /* the A/B switch only exists in the STATS build */
    {   /* seeds from the game's draw table (default) vs walking the pool again */
        const uint16_t *c = (const uint16_t *)(uintptr_t)FB_HW2, *d = (const uint16_t *)(uintptr_t)FB_HW3;
        uint32_t cmds3;
        int tdiff = 0;
        render_seed_table = 0;
        cmds0 = ncmds();
        draw(1, FB_HW3, scene, info, frame);
        cmds3 = ncmds() - cmds0;
        render_seed_table = 1;
        for (i = 0; i < NPIX; i++) tdiff += c[i] != d[i];
        if (tdiff || cmds3 != cmds2) {
            printf("%-22s fx=%d seed table vs pool walk FAIL: diff px %d cmds %u / %u\n",
                   name, alpha_fx, tdiff, cmds2, cmds3);
            ++failures;
        }
    }
#endif
    {
        char why[2][200];
        int bad = draw_mode(BLIT_MODE_LIST, 0, FB_HW3, FB_HW2, scene, info, frame, cmds2, why[0], sizeof why[0]);
        bad |= draw_mode(BLIT_MODE_WORD, 0, FB_HW4, FB_HW2, scene, info, frame, cmds2, why[1], sizeof why[1]);
        if (bad) { printf("%-22s fx=%d command list FAIL%s%s\n", name, alpha_fx, why[0], why[1]); ++failures; }
        /* band mode (D): the same frame as BANDS .. ENDB */
        if (draw_mode(BLIT_MODE_LIST, 1, FB_HW3, FB_HW2, scene, info, frame, cmds2, why[0], sizeof why[0])) {
            printf("%-22s fx=%d band frame FAIL%s\n", name, alpha_fx, why[0]);
            ++failures;
        }
    }
    if (scene == 0) {   /* '8' engine profile: a held list runs nothing until blit_list_release() */
        const uint16_t *c = (const uint16_t *)(uintptr_t)FB_HW2, *d = (const uint16_t *)(uintptr_t)FB_HW3;
        uint32_t lend0, lend1;
        int hdiff = 0, touched = 0, held;
        Gfx g;
        GfxHwTarget t;
        blit_set_mode(BLIT_MODE_LIST);
        info.batch = 1 + BLIT_MODE_LIST;
        memset((void *)(uintptr_t)FB_HW3, 0xA5, NPIX * 2);
        gfx_begin_hw(&g, &t, FB_HW3, SCREEN_W, SCREEN_H);
        lend0 = bm_lend;
        blit_st.lhold = 1;
        render_game(&game, &g, info);
        gfx_end(&g);
        held = blit_st.lhold;               /* 0: the buffer wrapped (small-list build) */
        lend1 = bm_lend;
        for (i = 0; i < NPIX; i++) touched += d[i] != 0xA5A5u;
        blit_list_release();
        blit_wait();
        for (i = 0; i < NPIX; i++) hdiff += c[i] != d[i];
        blit_set_mode(BLIT_MODE_APB);
        info.batch = 1;
        if (hdiff || blit_st.lhold || bm_lptr != bm_lend || (held && (touched || lend1 != lend0))) {
            printf("%-22s fx=%d held list FAIL: held %d, before release %d px drawn, LIST_END %s; after: diff px %d\n",
                   name, alpha_fx, held, touched, lend1 != lend0 ? "moved" : "kept", hdiff);
            ++failures;
        }
        ++held_frames;
        held_isolated += held;
    }
    info.show_stats = 1;
    cmds0 = blit_st.cmds;
    draw(0, FB_CPU, scene, info, frame);
    draw(1, FB_HW, scene, info, frame);
    for (i = 0; i < NPIX; i++) if (a[i] != b[i]) {
        int d, c;
        static const int sh[3] = { 11, 5, 0 }, mk[3] = { 31, 63, 31 };
        ++diff;
        if (first < 0) first = i;
        for (c = 0; c < 3; c++) {
            d = ((a[i] >> sh[c]) & mk[c]) - ((b[i] >> sh[c]) & mk[c]);
            if (d < 0) d = -d;
            if (c == 1) d = (d + 1) / 2;                  /* green in 5-bit units */
            if (d > maxd) maxd = d;
        }
    }
    printf("%-22s fx=%d  cmds %6u  diff px %6d  max %d", name, alpha_fx,
           blit_st.cmds - cmds0, diff, maxd);
    if (first >= 0) printf("  first (%d,%d) cpu %04x hw %04x", first % SCREEN_W, first / SCREEN_W, a[first], b[first]);
    if (ldiff || cmds1 != cmds2)
        printf("  list/calls: diff px %d cmds %u / %u", ldiff, cmds2, cmds1);
    if ((!alpha_fx && diff) || maxd > 2 || ldiff || cmds1 != cmds2) { printf("  <-- FAIL"); ++failures; }
    printf("\n");
    {
        char n[64];
        snprintf(n, sizeof(n), "%s_fx%d", name, alpha_fx);
        dump(n, "cpu", FB_CPU);
        dump(n, "hw", FB_HW);
    }
}

static void tick(uint32_t held, uint32_t pressed, int n)
{
    Input in;
    while (n-- > 0) {
        in.held = held; in.pressed = pressed; pressed = 0;
        game_tick(&game, in);
    }
}

#if SPROUT_STATS
/* render_seed_part ('y' profile): every part handles the same seeds, each
 * submitted seed is one engine command, FULL draws exactly what render_game does */
static void seed_parts(int mode)
{
    Gfx g;
    GfxHwTarget t;
    int n[5], p, slots;
    uint32_t c0, cm[5];
    const uint16_t *a = (const uint16_t *)(uintptr_t)FB_HW, *b = (const uint16_t *)(uintptr_t)FB_HW1;
    int i, diff = 0;
    RenderInfo info;
    memset(&info, 0, sizeof(info));
    if (blit_set_mode(mode) != mode) { printf("seed parts: mode %d refused\n", mode); ++failures; return; }
    gfx_begin_hw(&g, &t, FB_HW1, SCREEN_W, SCREEN_H);
    slots = render_seed_part(&g, &game, SEED_PART_PREP);
    for (p = 0; p < 5; p++) {
        if (p == SEED_PART_FULL) {          /* background first, so FULL = the seed layer of a frame */
            render_seed_part(&g, &game, SEED_PART_BG);
        }
        c0 = blit_st.cmds;
        n[p] = render_seed_part(&g, &game, p);
        gfx_end(&g);
        blit_wait();
        cm[p] = blit_st.cmds - c0;
    }
    /* reference: the same frame up to the seeds is not separable, so compare
     * FULL against background + gfx_blit per seed through the CPU backend */
    gfx_begin(&g, (Pixel *)(uintptr_t)FB_HW, SCREEN_W, SCREEN_H);
    render_seed_part(&g, &game, SEED_PART_BG);
    render_seed_part(&g, &game, SEED_PART_FULL);
    gfx_end(&g);
    for (i = 0; i < NPIX; i++) diff += a[i] != b[i];
    blit_set_mode(BLIT_MODE_APB);
    printf("seed parts (mode %d): slots %d, n walk %d build %d submit %d tiny %d full %d, cmds %u %u %u %u %u, diff px %d\n",
           mode, slots, n[0], n[1], n[2], n[3], n[4], cm[0], cm[1], cm[2], cm[3], cm[4], diff);
    if (n[0] <= 0 || n[1] != n[0] || n[2] != n[0] || n[3] != n[0] || n[4] != n[0] ||
        cm[0] || cm[1] || cm[2] != (uint32_t)n[0] || cm[3] != (uint32_t)n[0] || cm[4] != (uint32_t)n[0] || diff) {
        printf("  FAIL seed parts\n");
        ++failures;
    }
}
#endif

static void both(const char *name, int scene, uint32_t frame)
{
    compare(name, scene, 0, frame);
    compare(name, scene, 1, frame);
}

int main(int argc, char **argv)
{
    void *m;
    int i;
    outdir = argc > 1 ? argv[1] : 0;

    m = mmap((void *)(uintptr_t)DDR_LO, DDR_SZ, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    if (m != (void *)(uintptr_t)DDR_LO) { perror("mmap"); return 2; }
    if ((uintptr_t)&game > 0xFFFFFFFFu) { printf("need -no-pie: statics above 4 GB\n"); return 2; }

    blit_model_reset();
    if (!blit_init()) { printf("model ID wrong\n"); return 2; }

    render_init();
    demo_init(IMG);
    game_init(&game, 0xC0FFEEu);

    both("title", 0, 0);
    tick(0, KEY_START, 1);
    tick(KEY_RIGHT, 0, 90);
    tick(KEY_DOWN | KEY_LEFT, 0, 200);
    both("playing", 0, 0);
    tick(0, KEY_PAUSE, 1);
    both("paused", 0, 0);
    tick(0, KEY_PAUSE, 1);
    for (i = 0; i < 6 && game.phase == PLAYING; i++) tick(i & 1 ? KEY_UP : KEY_RIGHT | KEY_DOWN, 0, 150);
    both("later", 0, 0);
    tick(0, KEY_MENU, 1); tick(0, 0, 2);
    tick(0, KEY_LIMIT, 1); tick(0, 0, 2);
    both("stress_menu", 0, 0);
    tick(0, KEY_PICK3, 1);
    tick(KEY_LEFT, 0, 240);
    both("stress_high", 0, 0);
#if SPROUT_STATS
    seed_parts(BLIT_MODE_APB);
    seed_parts(BLIT_MODE_LIST);
    seed_parts(BLIT_MODE_WORD);
#endif

    for (i = 0; i < 30; i++) demo_squares_tick(512);
    both("squares", 1, 0);
    both("images", 2, 7);
    /* the CPU rewrites sources the engine has cached: font strips rebuilt in another
     * order land on addresses (and source-cache lines) that held other colours */
    gfx_font_reset(0);
    both("images atlas rebuilt", 2, 7);
    both("game atlas rebuilt", 0, 0);

    printf("model: %u commands, %u write beats; driver: %u list words, %u kicks, %u wraps\n",
           bm_cmds, bm_wbeat, blit_st.lwords, blit_st.lkicks, blit_st.lwraps);
    printf("source cache: %u INVs, %u pixel reads hit, %u line fills\n", blit_st.cinvs, bm_chit, bm_cmiss);
    printf("seed tables (TAB): %u\n", blit_st.ltabs);
    printf("held list frames ('8' profile): %d, %d isolated (rest wrapped the list buffer)\n",
           held_frames, held_isolated);
    printf("band frames (BANDS..ENDB): %d, %d ended early by a list wrap; model: %u frames, fault %u\n",
           band_frames, band_splits, bm_frames, bm_fault);
    if (blit_st.has_band && !band_frames) { printf("band mode never used  <-- FAIL\n"); ++failures; }
    if (bm_fault) { printf("driver would hang the engine (fault %u)  <-- FAIL\n", bm_fault); ++failures; }
    if (blit_st.has_table && !blit_st.ltabs) { printf("TAB never used  <-- FAIL\n"); ++failures; }
    if (blit_st.has_cache && !bm_chit) { printf("source cache never used  <-- FAIL\n"); ++failures; }
    printf(failures ? "FAIL (%d)\n" : "PASS\n", failures);
    return failures ? 1 : 0;
}
