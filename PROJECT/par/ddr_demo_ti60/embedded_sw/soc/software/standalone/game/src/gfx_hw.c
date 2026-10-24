#include "gfx_hw.h"
#include "blit.h"

static inline uint32_t addr_of(const void *p) { return (uint32_t)(uintptr_t)p; }
/* TAB 记录要求表项 8 B：{int16 x, int16 y, int32 sprite} */
typedef char gfx_item_is_8_bytes[sizeof(GfxItem) == 8 ? 1 : -1];
static inline uint32_t clamp_alpha(int a) { return a <= 0 ? 0u : a >= 255 ? 255u : (uint32_t)a; }

static inline uint32_t dst_addr(const GfxHwTarget *t, int x, int y)
{
    return t->fb + (uint32_t)y * t->pitch + (uint32_t)x * 2u;
}

static void hw_begin(void *context)
{
    const GfxHwTarget *t = (const GfxHwTarget *)context;
    blit_fence();                               /* CPU-written sprites -> DDR */
    blit_list_frame();                          /* LIST mode: rewind the list if idle */
    blit_cache_inv();                           /* source cache: forget last frame's lines */
    /* 分带合成开着（blit_set_band）：这一帧用 BANDS..ENDB 包起来，引擎分带画；否则什么也不做 */
    blit_band_begin(t->band_fb, t->pitch, t->rows);
}

/* LIST mode: publish what is still pending, the engine starts on it now
 * (分带帧：写 ENDB，引擎开始逐带画) */
static void hw_end(void *context)
{
    (void)context;
    if (blit_st.mode != BLIT_MODE_LIST) return;
    if (blit_st.band_open) blit_band_end();
    else                   blit_list_kick();
}

static void hw_fill(void *context, int x, int y, int w, int h, Pixel c)
{
    const GfxHwTarget *t = (const GfxHwTarget *)context;
    blit_fill(dst_addr(t, x, y), t->pitch, w, h, c, BLIT_OPAQUE);
}

static void hw_fill_alpha(void *context, int x, int y, int w, int h, Pixel c, int alpha)
{
    const GfxHwTarget *t = (const GfxHwTarget *)context;
    blit_fill(dst_addr(t, x, y), t->pitch, w, h, c, clamp_alpha(alpha));
}

/* 批量调用的模式位（GFX_BLIT_KEY / GFX_BLIT_ADD）-> CMD 位。引擎没有加法混合（旧位流，
 * CAPS2[0] = 0）时退化成普通拷贝（画面不发光，但不会错位 / 卡死）。 */
static inline uint32_t mode_flags(int mode)
{
    return ((mode & GFX_BLIT_KEY) ? BLIT_KEY : 0u) |
           ((mode & GFX_BLIT_ADD) && blit_st.has_add ? BLIT_ADD : 0u);
}
/* 精灵槽标签里的模式位（size 只用到 bit 26）：色键 bit 31，缓存 bit 30，加法 bit 29 */
static inline uint32_t mode_tag(uint32_t cmd)
{
    return ((cmd & BLIT_KEY) ? 0x80000000u : 0u) | ((cmd & BLIT_CACHE) ? 0x40000000u : 0u) |
           ((cmd & BLIT_ADD) ? 0x20000000u : 0u);
}

static void hw_copy_common(const GfxHwTarget *t, int x, int y, const Sprite *s,
                           int sx, int sy, int w, int h, int key, uint32_t alpha, int plane, uint32_t add)
{
    uint32_t off = (uint32_t)(sy * s->width + sx);
    uint32_t a = plane && s->alpha ? addr_of(s->alpha) + off : 0u;  /* A8 plane, pitch = width */
    blit_copy(dst_addr(t, x, y), t->pitch,
              addr_of(s->pixels) + off * 2u, (uint32_t)s->width * 2u,
              w, h, (key ? BLIT_KEY : 0u) | blit_cache_flag(w) | add, alpha, a, (uint32_t)s->width);
}

static void hw_copy(void *context, int x, int y, const Sprite *s,
                    int sx, int sy, int w, int h, int key)
{
    hw_copy_common((const GfxHwTarget *)context, x, y, s, sx, sy, w, h, key, BLIT_OPAQUE, 0, 0u);  /* like the CPU: plane ignored */
}

static void hw_copy_alpha(void *context, int x, int y, const Sprite *s,
                          int sx, int sy, int w, int h, int key, int alpha)
{
    hw_copy_common((const GfxHwTarget *)context, x, y, s, sx, sy, w, h, key, clamp_alpha(alpha), 1, 0u);
}

/* 加法混合（gfx_fill_add / gfx_blit_add）：没有 CAPS2[0] 的旧位流退化成普通透明混合 */
static void hw_add_fill(void *context, int x, int y, int w, int h, Pixel c, int alpha)
{
    const GfxHwTarget *t = (const GfxHwTarget *)context;
    if (blit_st.has_add) blit_fill_add(dst_addr(t, x, y), t->pitch, w, h, c, clamp_alpha(alpha));
    else                 blit_fill(dst_addr(t, x, y), t->pitch, w, h, c, clamp_alpha(alpha));
}

static void hw_add_copy(void *context, int x, int y, const Sprite *s,
                        int sx, int sy, int w, int h, int key, int alpha)
{
    hw_copy_common((const GfxHwTarget *)context, x, y, s, sx, sy, w, h, key, clamp_alpha(alpha), 1,
                   blit_st.has_add ? BLIT_ADD : 0u);
}

/* new source data (font atlas): it may sit in lines the source cache already holds */
static void hw_publish(void *context)
{
    (void)context;
    blit_fence();
    blit_cache_inv();
    if (blit_st.mode == BLIT_MODE_LIST) blit_list_kick();
}
static void hw_wait_idle(void *context) { (void)context; blit_wait(); }

/*
 * Batched copies (gfx_blit_list): the same command stream as calling hw_copy
 * per item, but in one tight loop. The shadow registers and the credits live
 * in CPU registers for the whole list (no stores to blit_st per item - the
 * D$ is write-through), SRC/SSTR/SIZE are only looked at when the sprite
 * changes, so a typical item costs one DST write + one CMD write on APB.
 * All items are inside the target (gfx_blit_list checked), so no clipping.
 */
/*
 * Batched copies through the engine's command list (LIST / WORD mode): each
 * sprite is defined once in one of the 64 slots (slot = sprite index & 63,
 * tagged with its SRC and SIZE|key so a changed sprite is redefined), then
 * every item is ONE word: DRAW(slot, y * pitch + x * 2), relative to the
 * list context BASE = fb. LIST: the word is a DDR store, published with
 * LIST_END every BLIT_LIST_KICK words. WORD: the word is one APB write to
 * LIST_WORD (instead of DST + CMD).
 */
#define LIST_CHUNK 64
/*
 * 精灵前缀表（2026-10-04）：弹幕几种样式交错出现，原来"精灵变了"那段
 * （查 Sprite、算 src/size/缓存标志、比较 slot 表）几乎每项都要跑一遍。
 * 现在每次调用里，编号 < LIST_PRE 的精灵只在第一次出现时确认/定义 slot，
 * 把它的 DRAW 前缀记进 pre[]（0 = 本次还没确认）；之后同一精灵只查一次表。
 * 编号 >= LIST_PRE 的精灵仍按原来的 last 方式处理。
 * 编号 >= 64 的精灵和 < LIST_PRE 的精灵可能共用 slot：定义 slot 时把
 * pre[slot] 清掉，保证 pre[] 里的前缀对应的 slot 内容一直是对的。
 */
#define LIST_PRE 16
static void hw_copy_list_hw(const GfxHwTarget *t, const Sprite *sprites, const GfxItem *it, int n, int key)
{
    const uint32_t fb = t->fb, pitch = t->pitch;
    const uint32_t cmd = BLIT_COPY | mode_flags(key) | BLIT_GA(BLIT_OPAQUE);   /* key = GFX_BLIT_* 模式位 */
    const int dma = blit_st.mode == BLIT_MODE_LIST;
    uint32_t lsw = 0, credits = blit_st.credits;
    uint32_t pre[LIST_PRE];
    uint32_t last = 0xFFFFFFFFu;
    int i = 0;

    for (i = 0; i < (int)LIST_PRE; i++) pre[i] = 0u;
    i = 0;
    if(dma)blit_band_admit((uint32_t)n);
    blit_list_draw_ctx(fb, pitch, (cmd & BLIT_KEY) != 0u);
    while (i < n) {
        const int end = n - i > LIST_CHUNK ? i + LIST_CHUNK : n;
        uint32_t *p = dma ? blit_list_reserve(LIST_CHUNK) : 0;
        for (; i < end; i++) {
            const uint32_t sp = (uint32_t)it[i].sprite;
            uint32_t w, sw = sp < LIST_PRE ? pre[sp] : (sp == last ? lsw : 0u);
            if (!sw) {                              /* 本次第一次见到这个精灵：slot 命中，或定义它 */
                const Sprite *s = &sprites[sp];
                const uint32_t slot = sp & (BLIT_L_SLOTS - 1u);
                const uint32_t src = addr_of(s->pixels), size = blit_size(s->width, s->height);
                const uint32_t scmd = cmd | blit_cache_flag(s->width);
                const uint32_t tag = size | mode_tag(scmd);
                sw = BLIT_L_DRAW(slot, 0u);
                if (blit_slot_src[slot] != src || blit_slot_size[slot] != tag) {
                    blit_slot_src[slot] = src;
                    blit_slot_size[slot] = tag;
                    if (slot < LIST_PRE) pre[slot] = 0u;   /* 别的精灵占过这个 slot */
                    if (dma) blit_st.lwp = p;
                    blit_list_slot(slot, src, (uint32_t)s->width * 2u, size, scmd);
                    if (dma) p = blit_list_reserve((uint32_t)(end - i));
                }
                if (sp < LIST_PRE) pre[sp] = sw;
                last = sp; lsw = sw;
            }
            w = sw | ((uint32_t)it[i].y * pitch + (uint32_t)it[i].x * 2u);
            if (dma) {
                *p++ = w;
            } else {
                if (!credits) {                     /* every DRAW pushes a command */
                    blit_st.credits = 0;
                    blit_refill();
                    credits = blit_st.credits;
                }
                --credits;
                blit_wr(BLIT_R_LWORD, w);
            }
        }
        if (dma) blit_list_commit(p);
    }
    if (!dma) { blit_st.credits = credits; BLIT_STAT(blit_st.lwords += (uint32_t)n); }
    BLIT_STAT(blit_st.cmds += (uint32_t)n);
}

static void hw_copy_list(void *context, const Sprite *sprites, const GfxItem *it, int n, int key)
{
    const GfxHwTarget *t = (const GfxHwTarget *)context;
    const uint32_t fb = t->fb, pitch = t->pitch;
    const uint32_t cmd0 = BLIT_COPY | mode_flags(key) | BLIT_GA(BLIT_OPAQUE);
    uint32_t cmd = cmd0;
    uint32_t dst = blit_st.dst, src = blit_st.src, sstr = blit_st.sstr, size = blit_st.size;
    uint32_t credits = blit_st.credits;
    int32_t last = -1;
    int i;

    if (n <= 0) return;
    /* items are inside the target, so the DRAW offset fits when the whole
     * target (at most BLIT_MAX_H rows) does */
    if (blit_st.mode != BLIT_MODE_APB && pitch * (uint32_t)(BLIT_MAX_H + 1) <= BLIT_L_MAX_OFF + 1u) {
        hw_copy_list_hw(t, sprites, it, n, key);
        return;
    }
    BLIT_SET(dstr, BLIT_R_DSTR, pitch);
    for (i = 0; i < n; i++) {
        uint32_t d;
        if (it[i].sprite != last) {                 /* new sprite: SRC / SSTR / SIZE */
            const Sprite *s = &sprites[it[i].sprite];
            uint32_t v;
            last = it[i].sprite;
            cmd = cmd0 | blit_cache_flag(s->width);
            v = addr_of(s->pixels);
            if (v != src)  { src = v;  blit_wr(BLIT_R_SRC, v); }
            v = (uint32_t)s->width * 2u;
            if (v != sstr) { sstr = v; blit_wr(BLIT_R_SSTR, v); }
            v = blit_size(s->width, s->height);
            if (v != size) { size = v; blit_wr(BLIT_R_SIZE, v); }
        }
        d = fb + (uint32_t)it[i].y * pitch + (uint32_t)it[i].x * 2u;
        if (d != dst) { dst = d; blit_wr(BLIT_R_DST, d); }
        if (!credits) {                             /* slow path, every <= 64 items */
            blit_st.credits = 0;
            blit_refill();
            credits = blit_st.credits;
        }
        --credits;
        blit_wr(BLIT_R_CMD, cmd);
    }
    blit_st.dst = dst; blit_st.src = src; blit_st.sstr = sstr; blit_st.size = size;
    blit_st.credits = credits;
    BLIT_STAT(blit_st.cmds += (uint32_t)n);
}

/*
 * 精灵表交给引擎（TAB 记录，CAPS[28]，2026-10-04）：A 方案里每颗弹幕仍要 CPU
 * 算偏移、写 1 个 DDR 字（约 0.7 us/颗，3800 颗 ≈ 2.7 ms）。表项本身已经是
 * {x, y, 精灵号}（GfxItem 的布局正好是引擎要的格式），所以这里只做：
 *   1. 精灵 0..nsprites-1 放进槽 0..nsprites-1（槽号 = 精灵号，标签相同就不重定义）；
 *   2. 设上下文 BASE = fb、DSTR = pitch（与 hw_copy_list_hw 相同）；
 *   3. 写 TAB(n) + 表地址 2 个字，立即发布。
 * 引擎按表逐项画，每项等于 DRAW(槽, y*pitch + 2x)，命令顺序与逐项提交完全一样。
 * 只在 LIST 模式、引擎有 TAB、nsprites <= 64、DRAW 偏移放得下时使用，否则返回 0
 * （gfx_blit_table 改走 copy_list）。表在引擎读完前不能改（调用者保证）。
 */
static int hw_copy_table(void *context, const Sprite *sprites, int nsprites,
                         const GfxItem *it, int n, int key)
{
    const GfxHwTarget *t = (const GfxHwTarget *)context;
    const uint32_t cmd = BLIT_COPY | mode_flags(key) | BLIT_GA(BLIT_OPAQUE);
    uint32_t a = addr_of(it), *p;
    int s;

    if (blit_st.mode != BLIT_MODE_LIST || !blit_st.has_table ||
        nsprites <= 0 || nsprites > (int)BLIT_L_SLOTS || (a & 3u) ||
        t->pitch * (uint32_t)(BLIT_MAX_H + 1) > BLIT_L_MAX_OFF + 1u)
        return 0;
    blit_band_admit((uint32_t)n);
    blit_list_draw_ctx(t->fb, t->pitch, (cmd & BLIT_KEY) != 0u);
    for (s = 0; s < nsprites; s++) {               /* 槽号 = 精灵号 */
        const Sprite *sp = &sprites[s];
        const uint32_t src = addr_of(sp->pixels), size = blit_size(sp->width, sp->height);
        const uint32_t scmd = cmd | blit_cache_flag(sp->width);
        const uint32_t tag = size | mode_tag(scmd);
        if (blit_slot_src[s] != src || blit_slot_size[s] != tag) {
            blit_slot_src[s] = src;
            blit_slot_size[s] = tag;
            blit_list_slot((uint32_t)s, src, (uint32_t)sp->width * 2u, size, scmd);
        }
    }
    while (n > 0) {                                /* 一条 TAB 最多 65535 项 */
        const int k = n > (int)BLIT_L_TAB_MAX ? (int)BLIT_L_TAB_MAX : n;
        p = blit_list_reserve(2u);
        p[0] = BLIT_L_TAB(k);
        p[1] = a;
        blit_st.lwp = p + 2;
        BLIT_STAT(blit_st.cmds += (uint32_t)k);
        BLIT_STAT(++blit_st.ltabs);
        a += (uint32_t)k * (uint32_t)sizeof(GfxItem);
        n -= k;
    }
    blit_list_kick();                              /* 一张表就是一大段活，马上让引擎开始 */
    return 1;
}

const GfxOps gfx_hw_ops = {
    hw_begin, hw_fill, hw_copy, hw_end, hw_fill_alpha, hw_copy_alpha,
    hw_publish, hw_wait_idle, hw_copy_list, hw_copy_table, hw_add_fill, hw_add_copy
};

void gfx_begin_hw(Gfx *gfx, GfxHwTarget *t, uint32_t fb, int width, int height)
{
    gfx_begin_hw_view(gfx, t, fb, (uint32_t)width * 2u, fb, width, height);
}

void gfx_begin_hw_view(Gfx *gfx, GfxHwTarget *t, uint32_t fb, uint32_t pitch,
                       uint32_t band_fb, int width, int height)
{
    t->fb = fb;
    t->pitch = pitch;
    t->rows = (uint32_t)height;
    t->band_fb = band_fb;
    gfx_begin_backend(gfx, &gfx_hw_ops, t, width, height);
}
