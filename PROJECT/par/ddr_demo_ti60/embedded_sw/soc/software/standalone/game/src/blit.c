#include "blit.h"

#ifdef BLIT_HOST_MODEL
static uint32_t blit_time(void) { return 0; }       /* the host model never stalls */
#else
#include "soc.h"                               /* SYSTEM_CLINT_CTRL (100 MHz mtime) */
#include "clint.h"
static inline uint32_t blit_time(void) { return clint_getTimeLow(SYSTEM_CLINT_CTRL); }
#endif

/* blit_stuck() 的输出：板上用阻塞的 bsp_printf（主循环的 tx 环形缓冲此时不会被抽走） */
#ifdef BLIT_HOST_MODEL
#include <stdio.h>
#define BLIT_LOG printf
#define BLIT_HDMI_FRAMES() 0u
#else
#include "bsp.h"
#include "hdmi_fb.h"
#define BLIT_LOG bsp_printf
#define BLIT_HDMI_FRAMES() hdmi_frame_cnt()
#endif

BlitState blit_st;
uint32_t blit_slot_src[BLIT_L_SLOTS];
uint8_t blit_slot_ga[BLIT_L_SLOTS];
uint32_t blit_slot_size[BLIT_L_SLOTS];

/* DDR command list (the engine DMA-reads it; the CPU only writes it) */
#ifdef __riscv
#define DDR_LIST __attribute__((section(".artbss"),aligned(256)))
#else
#define DDR_LIST __attribute__((aligned(256)))
#endif
static uint32_t blit_list_buf[BLIT_LIST_WORDS] DDR_LIST;

static inline uint32_t lst_addr(const uint32_t *p) { return (uint32_t)(uintptr_t)p; }

int blit_present(void)
{
    return blit_rd(BLIT_R_ID) == BLIT_ID_VALUE;
}

/* 引擎卡死诊断：忙等约 1 s 还没等到时由 blit_wait 等调用，直接（阻塞）打串口。
 * 看法：两份快照之间 PERF_BUSY 在涨、CMDS/RB/WB/CHIT/CMISS 不动 = 引擎内部死锁；
 * LADDR != LEND 且不动 = 列表取数停了；HDMI 帧计数不动 = DDR 读口整个卡住。 */
void blit_stuck(const char *where)
{
    static uint32_t shots;
    if (shots >= 4u) return;
    ++shots;
    BLIT_LOG("\r\n!! BitBlt engine stuck in %s (dump %d/4)\r\n", where, (int)shots);
    BLIT_LOG("   STATUS=%x PUSHED=%x CMDS=%x BUSY=%x RB=%x WB=%x\r\n",
             (unsigned)blit_rd(BLIT_R_STATUS), (unsigned)blit_rd(BLIT_R_PUSHED),
             (unsigned)blit_rd(BLIT_R_PERF_CMDS), (unsigned)blit_rd(BLIT_R_PERF_BUSY),
             (unsigned)blit_rd(BLIT_R_PERF_RB), (unsigned)blit_rd(BLIT_R_PERF_WB));
    BLIT_LOG("   CHIT=%x CMISS=%x LADDR=%x LEND=%x CAPS=%x HDMI frames=%x\r\n",
             (unsigned)blit_rd(BLIT_R_PERF_CHIT), (unsigned)blit_rd(BLIT_R_PERF_CMISS),
             (unsigned)blit_rd(BLIT_R_LADDR), (unsigned)blit_rd(BLIT_R_LEND),
             (unsigned)blit_rd(BLIT_R_CAPS), (unsigned)BLIT_HDMI_FRAMES());
    BLIT_LOG("   cpu: mode=%d cflag=%x list buf=%x lwp=%x lkick=%x\r\n",
             blit_st.mode, (unsigned)blit_st.cflag, (unsigned)lst_addr(blit_list_buf),
             (unsigned)lst_addr(blit_st.lwp), (unsigned)lst_addr(blit_st.lkick));
    if (blit_st.has_band)                       /* 分带：FRAMES 不动且 STATUS[5]=1 = 卡在某一带 / 等 ENDB */
        BLIT_LOG("   band: on=%d open=%d seq=%x FRAMES=%x GEO=%x P1=%x BAND=%x FLUSH=%x RECS=%x\r\n",
                 blit_st.band, blit_st.band_open, (unsigned)blit_st.bseq,
                 (unsigned)blit_rd(BLIT_R_FRAMES), (unsigned)blit_rd(BLIT_R_BAND_GEO),
                 (unsigned)blit_rd(BLIT_R_PERF_P1), (unsigned)blit_rd(BLIT_R_PERF_BAND),
                 (unsigned)blit_rd(BLIT_R_PERF_FLUSH), (unsigned)blit_rd(BLIT_R_PERF_RECS));
    if (blit_rd(BLIT_R_CAPS2) & 2u)             /* CAPS2[1]：调试寄存器 DBG0..3（0xC0..0xCC，2026-10-20），tools/board_diag.py 有逐位解码 */
        BLIT_LOG("   dbg: KBEAT=%x DBG0=%x DBG1=%x DBG2=%x DBG3=%x\r\n",
                 (unsigned)blit_rd(0xACu), (unsigned)blit_rd(0xC0u), (unsigned)blit_rd(0xC4u),
                 (unsigned)blit_rd(0xC8u), (unsigned)blit_rd(0xCCu));
}

void blit_refill(void)
{
    uint32_t free = blit_rd(BLIT_R_STATUS) >> 16;
#if BLIT_STATS
    {
        uint32_t used = blit_st.depth > free ? blit_st.depth - free : 0u;
        ++blit_st.refills;
        if (used > blit_st.qpeak) blit_st.qpeak = used;
    }
#endif
    if (!free) {
        /* queue full: the CPU is ahead of the engine by a whole queue */
#if BLIT_STATS
        uint32_t t0 = blit_time();
#endif
        uint32_t n = 0;
        do {
            free = blit_rd(BLIT_R_STATUS) >> 16;
            if ((++n & (BLIT_STUCK_SPINS - 1u)) == 0) blit_stuck("blit_refill (queue full)");
        } while (!free);
#if BLIT_STATS
        blit_st.stall_ticks += blit_time() - t0;
        ++blit_st.stalls;
#endif
    }
    blit_st.credits = free < BLIT_CREDIT_MAX ? free : BLIT_CREDIT_MAX;
}

uint32_t blit_queue_used(void)
{
    uint32_t free = blit_rd(BLIT_R_STATUS) >> 16;
    uint32_t used = blit_st.depth > free ? blit_st.depth - free : 0u;
    BLIT_STAT(if (used > blit_st.qpeak) blit_st.qpeak = used);
    /* the engine only frees slots, so the credits stay valid; refresh them */
    blit_st.credits = free < BLIT_CREDIT_MAX ? free : BLIT_CREDIT_MAX;
    return used;
}

/* ------------------------------------------------------------------------- */
/* command list                                                               */
/* ------------------------------------------------------------------------- */
void blit_list_kick(void)
{
    if (blit_st.lhold) return;                 /* 测量：先攒着，blit_list_release() 一次发布 */
    if (blit_st.lwp != blit_st.lkick) {
        blit_fence();                          /* list words -> DDR before the engine reads */
        BLIT_STAT(blit_st.lwords += (uint32_t)(blit_st.lwp - blit_st.lkick));
        blit_st.lkick = blit_st.lwp;
        blit_wr(BLIT_R_LEND, lst_addr(blit_st.lwp));
        BLIT_STAT(++blit_st.lkicks);
    }
}

static void list_restart(void)
{
    blit_st.lwp = blit_st.lkick = blit_list_buf;
    blit_wr(BLIT_R_LADDR, lst_addr(blit_list_buf));
}

void blit_list_frame(void)
{
    if (blit_st.mode != BLIT_MODE_LIST) return;
    blit_list_kick();
    if (blit_st.lhold) return;                 /* 有攒着没发布的字：不能回到开头 */
    if (blit_st.lwp == blit_list_buf) return;
    if (blit_rd(BLIT_R_STATUS) & BLIT_ST_LIST) return;   /* still running: keep appending */
    list_restart();
}

/* 分带帧提前 / 正常结束：写 ENDB（缓冲末尾始终留着 1 个字给它，见 blit_init 的 llim） */
static void band_close(void)
{
    *blit_st.lwp++ = BLIT_L_ENDB;
    blit_st.band_open = 0;
    ++blit_st.bseq;
}

uint32_t *blit_list_wrap(uint32_t need)
{
    (void)need;                                /* need << BLIT_LIST_WORDS */
    blit_st.lhold = 0;                         /* 缓冲满：攒不下了，必须发布 */
    if (blit_st.band_open) {                   /* 帧中途回绕：这一帧到此为止先画出来，剩下的命令直接画 */
        band_close();
        ++blit_st.bsplits;                     /* 主循环靠它知道这一帧被拆开了（不能流水），所以发行版也计 */
    }
    blit_list_kick();
    if (blit_rd(BLIT_R_STATUS) & BLIT_ST_LIST) {
        /* the CPU is a whole buffer ahead of the engine */
#if BLIT_STATS
        uint32_t t0 = blit_time();
#endif
        uint32_t n = 0;
        while (blit_rd(BLIT_R_STATUS) & BLIT_ST_LIST)
            if ((++n & (BLIT_STUCK_SPINS - 1u)) == 0) blit_stuck("blit_list_wrap (list buffer full)");
#if BLIT_STATS
        blit_st.stall_ticks += blit_time() - t0;
        ++blit_st.stalls;
#endif
    }
    BLIT_STAT(++blit_st.lwraps);
    list_restart();
    return blit_st.lwp;
}

#define LSET(i, bit, v) do { uint32_t v_ = (v); \
    if (c[i] != v_) { c[i] = v_; *p++ = v_; m |= (bit); } } while (0)

/* Budget assumes every record could touch the same scan band. Once full,
 * finish this band pass and append ordinary draws, preserving blend order.
 * Do not start another BANDS pass: the hardware does not reload prior pixels. */
void blit_band_admit(uint32_t records)
{
    if(!blit_st.band_open)return;
    const uint32_t capacity=1u<<(BLIT_BAND_BIN_SHIFT-5u);
    if(records>capacity-blit_st.band_records){
        band_close();++blit_st.bsplits;blit_list_kick();
    }else blit_st.band_records+=records;
}

void blit_list_fill(uint32_t dst, uint32_t dstr, uint32_t size, uint32_t color, uint32_t cmd)
{
    blit_band_admit(1);
    uint32_t *c = blit_st.lctx, *p = blit_list_reserve(5), *h = p++, m = 0;
    LSET(0, BLIT_LF_DST,   dst);
    LSET(1, BLIT_LF_DSTR,  dstr);
    LSET(6, BLIT_LF_SIZE,  size);
    LSET(7, BLIT_LF_COLOR, color);
    *h = BLIT_L_REC(m, cmd);
    BLIT_STAT(++blit_st.cmds);
    blit_list_commit(p);
}

void blit_list_copy(uint32_t dst, uint32_t dstr, uint32_t src, uint32_t sstr,
                    uint32_t size, uint32_t cmd, uint32_t a, uint32_t astr)
{
    blit_band_admit(1);
    uint32_t *c = blit_st.lctx, *p = blit_list_reserve(10), *h = p++, m = 0;
    LSET(0, BLIT_LF_DST,  dst);
    LSET(1, BLIT_LF_DSTR, dstr);
    LSET(2, BLIT_LF_SRC,  src);
    LSET(3, BLIT_LF_SSTR, sstr);
    if (cmd & BLIT_APLANE) {
        LSET(4, BLIT_LF_A,    a);
        LSET(5, BLIT_LF_ASTR, astr);
    }
    LSET(6, BLIT_LF_SIZE, size);
    if (cmd & BLIT_KEY) LSET(8, BLIT_LF_KEY, blit_st.key);
    *h = BLIT_L_REC(m, cmd);
    BLIT_STAT(++blit_st.cmds);
    blit_list_commit(p);
}

/* words that do not push a command: DDR list or LIST_WORD */
static void list_put(const uint32_t *w, uint32_t n)
{
    uint32_t i;
    if (blit_st.mode == BLIT_MODE_LIST) {
        uint32_t *p = blit_list_reserve(n);
        for (i = 0; i < n; i++) *p++ = w[i];
        blit_st.lwp = p;                       /* the caller's records follow: no kick here */
    } else {
        for (i = 0; i < n; i++) blit_wr(BLIT_R_LWORD, w[i]);
        BLIT_STAT(blit_st.lwords += n);
    }
}

void blit_list_draw_ctx(uint32_t base, uint32_t dstr, int key)
{
    uint32_t *c = blit_st.lctx, w[4], *p = w + 1, m = 0;
    LSET(1, BLIT_LF_DSTR, dstr);
    if (key) LSET(8, BLIT_LF_KEY, blit_st.key);
    LSET(9, BLIT_LF_BASE, base);
    if (!m) return;
    w[0] = BLIT_L_REC(m | BLIT_L_NOPUSH, 0u);
    list_put(w, (uint32_t)(p - w));
}

void blit_list_slot(uint32_t slot, uint32_t src, uint32_t sstr, uint32_t size, uint32_t cmd)
{
    uint32_t *c = blit_st.lctx, w[5], *p = w + 1, m = 0;
    LSET(2, BLIT_LF_SRC,  src);
    LSET(3, BLIT_LF_SSTR, sstr);
    LSET(6, BLIT_LF_SIZE, size);
    if (m) w[0] = BLIT_L_REC(m | BLIT_L_NOPUSH, 0u);
    else   p = w;                              /* context already right: SLOT word only */
    *p++ = BLIT_L_SLOT(slot, cmd);
    list_put(w, (uint32_t)(p - w));
}

/* source cache: a size-0 command with INV (no pixels, only clears the valid bits) */
void blit_cache_inv(void)
{
    if (!blit_st.cflag) return;                /* nothing is read through the cache */
    BLIT_STAT(++blit_st.cinvs);
    if (blit_st.mode == BLIT_MODE_LIST) {
        uint32_t *c = blit_st.lctx, *p = blit_list_reserve(2), *h = p++, m = 0;
        LSET(6, BLIT_LF_SIZE, 0u);
        *h = BLIT_L_REC(m, BLIT_INV);
        BLIT_STAT(++blit_st.cmds);
        blit_list_commit(p);
    } else {
        BLIT_SET(size, BLIT_R_SIZE, 0u);
        blit_push(BLIT_INV);
    }
}

int blit_set_cache(int on)
{
    blit_st.cflag = on && blit_st.has_cache ? BLIT_CACHE : 0u;
    blit_cache_inv();                          /* lines from before may be stale */
    return blit_st.cflag != 0;
}

void blit_list_release(void)
{
    blit_st.lhold = 0;
    if (blit_st.mode == BLIT_MODE_LIST) blit_list_kick();
}

/* ------------------------------------------------------------------------- */
/* band mode (D): a frame = BANDS .. ENDB in the command list                 */
/* ------------------------------------------------------------------------- */
int blit_set_band(int on)
{
    blit_wait();                               /* 结束打开的帧，等引擎空闲 */
    blit_st.band = on && blit_st.has_band && blit_st.mode == BLIT_MODE_LIST;
    return blit_st.band;
}

void blit_band_set_koff(int off)
{
    if (!blit_st.has_band) return;
    blit_wait();
    blit_st.band_koff = off ? BLIT_BAND_KOFF : 0u;
    blit_st.band_geo = 0u;                     /* 下一帧重写 BAND_GEO */
}

int blit_band_set_kmode(int mode)
{
    if (!blit_st.has_band) return 2;
    if (mode == 1 && !blit_st.has_kblend) mode = 0;   /* 旧位流：半透明本来就走窄通路 */
    blit_wait();
    blit_st.band_koff = mode >= 2 ? BLIT_BAND_KOFF : mode == 1 ? BLIT_BAND_KBOFF : 0u;
    blit_st.band_geo = 0u;
    return blit_band_kmode();
}

int blit_band_kmode(void)
{
    return (blit_st.band_koff & BLIT_BAND_KOFF) ? 2 :
           ((blit_st.band_koff & BLIT_BAND_KBOFF) || !blit_st.has_kblend) ? 1 : 0;
}

int blit_band_begin(uint32_t fb, uint32_t pitch, uint32_t rows)
{
    uint32_t geo, *p;
    if (!blit_st.band || blit_st.mode != BLIT_MODE_LIST || blit_st.band_open) return 0;
    if (pitch < 256u || pitch > blit_st.band_max_pitch || (pitch & 15u) || (fb & 255u) ||
        rows == 0u || (rows % BLIT_BAND_ROWS) || rows / BLIT_BAND_ROWS > blit_st.band_max_bands)
        return 0;                              /* 放不进带缓冲的目标：直接画 */
    geo = pitch | ((rows / BLIT_BAND_ROWS) << 16) | blit_st.band_koff;
    if (geo != blit_st.band_geo) {             /* 几何变了：只能在引擎空闲时改（平时每帧都一样，不会进来） */
        blit_wait();
        blit_st.band_geo = geo;
        blit_wr(BLIT_R_BAND_GEO, geo);
        blit_wr(BLIT_R_BAND_RECIP, (1u << 24) / (pitch >> 4) + 1u);
    }
    /* 一帧要连续放在缓冲里：剩得不多就先回到开头（等引擎读完，流水时偶尔发生） */
    if ((uint32_t)(blit_st.llim - blit_st.lwp) < BLIT_BAND_ROOM + 2u) blit_list_wrap(0u);
    p = blit_st.lwp;
    p[0] = BLIT_L_BANDS;
    p[1] = fb;
    blit_st.lwp = p + 2;
    blit_st.band_records = 0;
    blit_st.band_open = 1;
    return 1;
}

void blit_band_end(void)
{
    if (!blit_st.band_open) return;
    band_close();
    blit_list_kick();
}

void blit_perf_snap(BlitPerf *p)
{
    uint32_t i;
    p->busy  = blit_rd(BLIT_R_PERF_BUSY);
    p->cmds  = blit_rd(BLIT_R_PERF_CMDS);
    p->rbeat = blit_rd(BLIT_R_PERF_RB);
    p->wbeat = blit_rd(BLIT_R_PERF_WB);
    p->chit  = blit_st.has_cache ? blit_rd(BLIT_R_PERF_CHIT)  : 0u;
    p->cmiss = blit_st.has_cache ? blit_rd(BLIT_R_PERF_CMISS) : 0u;
    for (i = 0; i < BLIT_ST_N; i++)
        p->st[i] = blit_st.has_stall ? blit_rd(BLIT_R_PERF_WSTALL + 4u * i) : 0u;
    p->wclk   = blit_st.has_wide ? blit_rd(BLIT_R_PERF_WCLK)   : 0u;
    p->xwbeat = blit_st.has_wide ? blit_rd(BLIT_R_PERF_XWBEAT) : 0u;
    p->xrbeat = blit_st.has_wide ? blit_rd(BLIT_R_PERF_XRBEAT) : 0u;
    for (i = 0; i < BLIT_BP_N; i++)
        p->band[i] = blit_st.has_band ? blit_rd(BLIT_R_PERF_P1 + 4u * i) : 0u;
}

void blit_perf_diff(BlitPerf *d, const BlitPerf *a, const BlitPerf *b)
{
    uint32_t i;
    d->busy  = b->busy  - a->busy;
    d->cmds  = b->cmds  - a->cmds;
    d->rbeat = b->rbeat - a->rbeat;
    d->wbeat = b->wbeat - a->wbeat;
    d->chit  = b->chit  - a->chit;
    d->cmiss = b->cmiss - a->cmiss;
    for (i = 0; i < BLIT_ST_N; i++) d->st[i] = b->st[i] - a->st[i];
    d->wclk   = b->wclk   - a->wclk;
    d->xwbeat = b->xwbeat - a->xwbeat;
    d->xrbeat = b->xrbeat - a->xrbeat;
    for (i = 0; i < BLIT_BP_N; i++) d->band[i] = b->band[i] - a->band[i];
}

static void slots_forget(void)
{
    uint32_t i;
    for (i = 0; i < BLIT_L_SLOTS; i++) { blit_slot_src[i] = 0; blit_slot_size[i] = 0xFFFFFFFFu; }
}

int blit_set_mode(int mode)
{
    if (mode != BLIT_MODE_APB && !blit_st.has_list) mode = BLIT_MODE_APB;
    blit_wait();                               /* publishes the list in LIST mode */
    blit_st.mode = mode;
    blit_st.credits = 0;
    if (mode == BLIT_MODE_LIST) list_restart();
    else blit_st.band = 0;                     /* 分带只在 LIST 模式下有（回到 LIST 后再 blit_set_band） */
    return mode;
}

int blit_init(void)
{
    uint32_t caps;
    if (!blit_present()) return 0;
    blit_wait();
    blit_wr(BLIT_R_CTRL, 3u);                  /* clear perf counters + error */
    blit_st.dst   = 0; blit_wr(BLIT_R_DST,   0);
    blit_st.dstr  = 0; blit_wr(BLIT_R_DSTR,  0);
    blit_st.src   = 0; blit_wr(BLIT_R_SRC,   0);
    blit_st.sstr  = 0; blit_wr(BLIT_R_SSTR,  0);
    blit_st.a     = 0; blit_wr(BLIT_R_AADDR, 0);
    blit_st.astr  = 0; blit_wr(BLIT_R_ASTR,  0);
    blit_st.size  = 0; blit_wr(BLIT_R_SIZE,  0);
    blit_st.color = 0; blit_wr(BLIT_R_COLOR, 0);
    blit_st.key   = 0xF81Fu; blit_wr(BLIT_R_KEY, 0xF81Fu);
    caps = blit_rd(BLIT_R_CAPS) & 0xFFu;       /* CQ_AW */
    blit_st.depth = caps >= 1u && caps <= 15u ? 1u << caps : 256u;
    blit_st.credits = 0;
    blit_st.cmds = 0;
    blit_st.stall_ticks = blit_st.stalls = blit_st.refills = blit_st.qpeak = 0;

    /* command list: put the list context in a known state (all 10 fields, no push) */
    blit_st.mode = BLIT_MODE_APB;
    blit_st.has_list = (blit_rd(BLIT_R_CAPS) & BLIT_CAPS_LIST) != 0;
    blit_st.has_limit = (blit_rd(BLIT_R_CAPS) & BLIT_CAPS_LIMIT) != 0;
    blit_st.has_limit_words = (blit_rd(BLIT_R_CAPS) & BLIT_CAPS_LIMIT_WORDS) != 0;
    blit_st.rd_max = BLIT_LIMIT_RD_MAX; blit_st.wr_max = BLIT_LIMIT_WR_MAX;
    blit_st.rw_max = BLIT_LIMIT_RW_MAX;
    blit_set_limit(BLIT_LIMIT_RD_MAX, BLIT_LIMIT_WR_MAX);
    blit_st.has_cache = (blit_rd(BLIT_R_CAPS) & BLIT_CAPS_CACHE) != 0;
    blit_st.cinvs = 0;
    blit_st.has_table = blit_st.has_list && (blit_rd(BLIT_R_CAPS) & BLIT_CAPS_TABLE) != 0;
    blit_st.ltabs = 0;
    blit_st.has_stall = (blit_rd(BLIT_R_CAPS) & BLIT_CAPS_STALL) != 0;
    blit_st.has_wide = (blit_rd(BLIT_R_CAPS) & BLIT_CAPS_WIDE) != 0;
    blit_st.wide = BLIT_WIDE_OFF;
    //blit_set_wide(BLIT_WIDE_OFF);
    blit_set_wide(BLIT_WIDE_RESET);            /* 宽通路：复位值（>= 128 像素才走） */
    blit_st.lhold = 0;
    blit_st.lwp = blit_st.lkick = blit_list_buf;
    blit_st.llim = blit_list_buf + BLIT_LIST_WORDS - 1u;   /* 末尾留 1 个字：分带帧随时能写 ENDB */
    blit_st.lwords = blit_st.lkicks = blit_st.lwraps = 0;
    /* 分带合成（CAPS[31]）：默认关，由应用 blit_set_band(1) 打开（要先进 LIST 模式） */
    blit_st.has_band = blit_st.has_list && (blit_rd(BLIT_R_CAPS) & BLIT_CAPS_BAND) != 0;
    /* 加法混合（CAPS2，0xBC）：旧位流这个地址读出 0 */
    blit_st.has_kblend=(blit_rd(BLIT_R_CAPS2)&BLIT_CAPS2_KBLEND)!=0;
    blit_st.has_ftab=(blit_rd(BLIT_R_CAPS2)&BLIT_CAPS2_FTAB)!=0;
    blit_st.has_add = (blit_rd(BLIT_R_CAPS2) & BLIT_CAPS2_ADD) != 0;
    {   /* 分带窗口大小（2026-10-21）：720p 位流 2560 字节 / 行、最多 45 带；旧位流 1280 / 32 */
        uint32_t c2 = blit_rd(BLIT_R_CAPS2);
        blit_st.band_max_pitch = (c2 & BLIT_CAPS2_WIN2560) ? 2560u : BLIT_BAND_MAX_PITCH;
        blit_st.band_max_bands = BLIT_CAPS2_MAXB(c2) ? BLIT_CAPS2_MAXB(c2) : BLIT_BAND_MAX_BANDS;
    }
    blit_st.band = 0; blit_st.band_open = 0;
    blit_st.band_geo = 0; blit_st.band_koff = 0; blit_st.bsplits = 0;
    blit_st.bseq = blit_st.has_band ? blit_rd(BLIT_R_FRAMES) : 0u;
    if (blit_st.has_band) blit_wr(BLIT_R_BAND_BIN, BLIT_BAND_BIN_BASE | BLIT_BAND_BIN_SHIFT);
    slots_forget();
    if (blit_st.has_list) {
        uint32_t i;
        blit_wr(BLIT_R_LADDR, lst_addr(blit_list_buf));
        blit_wr(BLIT_R_LWORD, BLIT_L_REC(0x3FFu | BLIT_L_NOPUSH, 0u));
        for (i = 0; i < 10; i++) {
            blit_st.lctx[i] = i == 8 ? 0xF81Fu : 0u;
            blit_wr(BLIT_R_LWORD, blit_st.lctx[i]);
        }
        blit_wait();
    }
    blit_set_cache(BLIT_CACHE_DEFAULT);        /* default on (CAPS[27]); INV once; make CACHE=0 关 */
    blit_wait();
    return 1;
}
