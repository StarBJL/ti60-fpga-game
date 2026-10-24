/* Random test of gfx_blit_list / gfx_blit_list_inside through the engine model
 * (LIST + WORD + APB) vs the CPU backend: sprite indices collide on slots
 * (i, i+64, i+128), sprites change size / key between calls.
 *
 *   gcc -O1 -g -fsanitize=address,undefined -no-pie -fno-pie -DBLIT_HOST_MODEL \
 *       -Isrc -Itools -o host_list_test tools/host_list_test.c src/gfx.c src/gfx_hw.c src/blit.c
 *   ./host_list_test        (PASS / FAIL)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#define BLIT_MODEL_PTR(a) ((uint8_t *)(uintptr_t)(a))
#include "blit_model.h"
#include "gfx.h"
#include "gfx_hw.h"
#include "blit.h"

#define DDR_LO 0x01000000u
#define W 640
#define H 480
#define FB_CPU 0x01000000u
#define FB_HW  0x01200000u
#define SPR    0x01400000u
#define NS 200

static Sprite spr[NS];
static GfxItem items[6 * 3000];     /* 每次调用用自己的一段：表在引擎画完前不能改 */
static uint32_t rs = 12345;
static uint32_t rnd(void) { rs = rs * 1103515245u + 12345u; return rs >> 8; }

static void mk_sprites(void)
{
    uint32_t a = SPR;
    for (int i = 0; i < NS; i++) {
        int w = 1 + rnd() % 24, h = 1 + rnd() % 24;
        Pixel *p = (Pixel *)(uintptr_t)a;
        for (int k = 0; k < w * h; k++) p[k] = (rnd() % 5 == 0) ? 0xF81F : (Pixel)rnd();
        spr[i].width = w; spr[i].height = h; spr[i].pixels = p; spr[i].alpha = 0;
        a += (uint32_t)(w * h * 2 + 64) & ~63u;
        if (i % 7 == 3) {                                 /* some sprites with an A8 plane */
            uint8_t *pa = (uint8_t *)(uintptr_t)a;
            for (int k = 0; k < w * h; k++) pa[k] = (uint8_t)rnd();
            spr[i].alpha = pa;
            a += (uint32_t)(w * h + 64) & ~63u;
        }
    }
}

int main(void)
{
    void *m = mmap((void *)(uintptr_t)DDR_LO, 0x01000000u, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    int fails = 0, tables = 0, bands = 0, adds = 0;
    if (m != (void *)(uintptr_t)DDR_LO) { perror("mmap"); return 2; }
    blit_model_reset();
    if (!blit_init()) { printf("model\n"); return 2; }
    mk_sprites();
    for (int round = 0; round < 600; round++) {
        int mode = round % 3 == 0 ? BLIT_MODE_LIST : round % 3 == 1 ? BLIT_MODE_WORD : BLIT_MODE_APB;
        Gfx gc, gh; GfxHwTarget t;
        int calls = 1 + rnd() % 6;
        int pool[8], np = 1 + rnd() % 8;
        if (round % 50 == 0) mk_sprites();               /* sprites move: slots must be redefined */
        for (int k = 0; k < np; k++) {
            int base = rnd() % 20;                        /* i, i+64, i+128 share a slot */
            pool[k] = base + 64 * (rnd() % 3);
            if (rnd() % 4 == 0) pool[k] = rnd() % NS;
        }
        /* 每隔一轮 LIST：整帧走分带合成（BANDS..ENDB）。分带帧从黑色开始，所以两边都先清成 0 */
        int band = mode == BLIT_MODE_LIST && round % 6 == 0;
        blit_set_mode(mode);
        if (band && !blit_set_band(1)) { printf("round %d: band mode refused\n", round); ++fails; }
        memset((void *)(uintptr_t)FB_CPU, band ? 0 : 0x5A, W * H * 2);
        memset((void *)(uintptr_t)FB_HW, 0x5A, W * H * 2);
        gfx_begin(&gc, (Pixel *)(uintptr_t)FB_CPU, W, H);
        gfx_begin_hw(&gh, &t, FB_HW, W, H);
        for (int c = 0; c < calls; c++) {
            int n = 1 + rnd() % 3000, inside = rnd() % 2, key = rnd() % 2;
            if (rnd() % 3 == 0) key |= GFX_BLIT_ADD;          /* additive batches (CAPS2[0]) */
            int table = inside && rnd() % 2;              /* gfx_blit_table：精灵号 0..nsp-1，相对 spr + sb */
            int nsp = 1 + rnd() % 64, sb = rnd() % (NS - 64);
            GfxItem *it = items + c * 3000;
            uint32_t px = 0;
            for (int i = 0; i < n; i++) {
                int s = pool[rnd() % np];
                if (rnd() % 3 == 0) s = pool[0];          /* runs of the same sprite */
                if (table) s = rnd() % 4 ? sb + (int)(rnd() % (uint32_t)nsp) : sb;
                it[i].sprite = table ? s - sb : s;
                if (inside) {
                    it[i].x = (int16_t)(rnd() % (W - spr[s].width + 1));
                    it[i].y = (int16_t)(rnd() % (H - spr[s].height + 1));
                    px += (uint32_t)(spr[s].width * spr[s].height);
                } else {
                    it[i].x = (int16_t)((int)(rnd() % (W + 60)) - 30);
                    it[i].y = (int16_t)((int)(rnd() % (H + 60)) - 30);
                }
            }
            if (table) {
                gfx_blit_table(&gc, spr + sb, nsp, it, n, key, px);
                gfx_blit_table(&gh, spr + sb, nsp, it, n, key, px);
                ++tables;
            } else if (inside) {
                gfx_blit_list_inside(&gc, spr, it, n, key, px);
                gfx_blit_list_inside(&gh, spr, it, n, key, px);
            } else {
                gfx_blit_list(&gc, spr, it, n, key);
                gfx_blit_list(&gh, spr, it, n, key);
            }
        }
        for (int c = 0; c < 12; c++) {                    /* single additive draws, any alpha, clipped */
            int x = (int)(rnd() % (W + 40)) - 20, y = (int)(rnd() % (H + 40)) - 20;
            int al = rnd() % 4 ? 255 : (int)(rnd() % 256), k = rnd() % 2;
            const Sprite *sp = &spr[rnd() % NS];
            if (rnd() % 3 == 0) {
                int w = 1 + rnd() % 200, h = 1 + rnd() % 60; Pixel col = (Pixel)rnd();
                gfx_fill_add(&gc, x, y, w, h, col, al); gfx_fill_add(&gh, x, y, w, h, col, al);
            } else {
                gfx_blit_add(&gc, x, y, sp, k, al); gfx_blit_add(&gh, x, y, sp, k, al);
            }
            ++adds;
        }
        gfx_end(&gc); gfx_end(&gh);
        blit_wait();
        if (memcmp((void *)(uintptr_t)FB_CPU, (void *)(uintptr_t)FB_HW, W * H * 2) || bm_lerr || bm_fault || bm_berr) {
            printf("round %d mode %d%s: DIFF lerr %u fault %u berr %u\n", round, mode, band ? " band" : "",
                   bm_lerr, bm_fault, bm_berr);
            if (++fails > 5) break;
        }
        bands += band;
    }
    printf("band frames %d, model counted %u (splits %u)\n", bands, bm_frames, blit_st.bsplits);
    if (bm_frames != (uint32_t)bands) { printf("band frame count\n"); ++fails; }
    printf("gfx_blit_table calls %d, engine TAB records %u, single additive draws %d (engine add %d)\n",
           tables, blit_st.ltabs, adds, blit_st.has_add);
    if (!blit_st.has_add) { printf("model has no CAPS2 add\n"); ++fails; }
    if (!blit_st.ltabs) { printf("TAB never used\n"); ++fails; }
    printf(fails ? "FAIL\n" : "PASS (600 rounds)\n");
    return fails != 0;
}
