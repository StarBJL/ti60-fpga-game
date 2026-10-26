/*
 * Functional C model of rtl/blit/blit_engine.v for host tests (rvemu.c and
 * host_blit_test.c). Same arithmetic as the RTL / sim/blit/tb_blit.py golden
 * model; commands execute immediately when CMD is written, so the model is
 * never busy. The command list (LIST_ADDR / LIST_END / LIST_WORD) is decoded
 * as soon as words are published, with its own context and 64 sprite slots.
 *
 * Before including, define  BLIT_MODEL_PTR(addr)  ->  uint8_t * for a DDR
 * byte address.
 */
#ifndef BLIT_MODEL_H
#define BLIT_MODEL_H
#include <stdint.h>
#include <string.h>

static uint32_t bm_reg[32];              /* word index = offset / 4 */
static uint32_t bm_cmds, bm_rbeat, bm_wbeat, bm_pushed;

/* command list: context in mask-bit order DST DSTR SRC SSTR A ASTR SIZE COLOR KEY BASE */
enum { BM_L_DST, BM_L_DSTR, BM_L_SRC, BM_L_SSTR, BM_L_A, BM_L_ASTR, BM_L_SIZE,
       BM_L_COLOR, BM_L_KEY, BM_L_BASE };
static uint32_t bm_lctx[10];
static struct { uint32_t src, sstr, size, cmd; } bm_slot[64];
static uint32_t bm_lptr, bm_lend, bm_lerr;
static uint32_t bm_limit = (1024u << 16) | (128u << 8) | 16u;   /* 0x54 LIMIT (no effect on the model) */
static uint32_t bm_lhdr, bm_lmask;        /* record being decoded (mask = fields still due) */
static uint32_t bm_lwords;                /* words decoded (host statistics only) */
#ifndef BLIT_MODEL_SQTAB
#define BLIT_MODEL_SQTAB 0
#endif
static uint32_t bm_sq_count,bm_sq_alpha,bm_sq_items,bm_sq_commands;
static uint32_t bm_ltab;                  /* TAB 记录：等地址字时 = 项数 + 1，否则 0 */
static uint32_t bm_lftab, bm_lfadd;       /* FTAB 记录（2026-10-26）：同上；ADD 位 */

/* band mode (D, CAPS[31]). The model draws every command at once, so a band frame
 * is modelled by its visible rules only: BANDS clears the frame to black (the
 * window starts empty), commands inside the frame are clipped to its rows and
 * must use the frame's row stride (else dropped + STATUS[7]), commands for other
 * targets draw normally, ENDB counts the frame. Driver mistakes that would hang
 * the real engine are caught: CMD / LIST_ADDR while a frame is open and waiting
 * for BUSY = 0 with an open frame set bm_fault (host tests check it). */
/* band window of the modelled bitstream (2026-10-21): 2560-byte rows / 45 bands (720p,
 * CAPS2[2] + CAPS2[15:8]); -DBLIT_MODEL_WIN=1280 models the old 640 px window / 32 bands */
#ifndef BLIT_MODEL_WIN
#define BLIT_MODEL_WIN 2560u
#endif
#define BM_MAXB (BLIT_MODEL_WIN > 1280u ? 45u : 32u)
static uint32_t bm_bgeo = (30u << 16) | 1280u, bm_brecip = 209716u, bm_bbin = 0x03000000u | 18u;
static uint32_t bm_band_on, bm_band_fb, bm_band_w8, bm_frames, bm_berr, bm_fault, bm_spin;
static uint32_t bm_band_cmds, bm_band_other;   /* commands inside the frame / other targets */
/* Optional hooks for a host harness (tools/host_main_test.c): time passing on every
 * register read, band frames that finish later than their ENDB, and checks. */
#ifndef BLIT_MODEL_TICK
#define BLIT_MODEL_TICK() ((void)0)
#endif
#ifndef BLIT_MODEL_FRAMES_DONE              /* of the n band frames closed, how many are finished */
#define BLIT_MODEL_FRAMES_DONE(n) (n)
#endif
#ifndef BLIT_MODEL_ON_BANDS
#define BLIT_MODEL_ON_BANDS(fb) ((void)0)
#endif
#ifndef BLIT_MODEL_ON_ENDB
#define BLIT_MODEL_ON_ENDB(seq) ((void)0)
#endif
#ifndef BLIT_MODEL_ON_CMD                   /* a drawing command with destination dst (not in a band frame) */
#define BLIT_MODEL_ON_CMD(dst) ((void)0)
#endif

static uint16_t bm_rd16(uint32_t a) { uint8_t *p = BLIT_MODEL_PTR(a); return (uint16_t)(p[0] | (p[1] << 8)); }
static void bm_wr16(uint32_t a, uint16_t v) { uint8_t *p = BLIT_MODEL_PTR(a); p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }

/* source cache (CAPS[27]) modelled like the RTL: 512 direct-mapped lines of 32 B,
 * tag = addr[31:14], filled from DDR on a miss, never updated by engine writes,
 * cleared only by INV (CMD[5]). A missing INV after the CPU rewrote a cached
 * source therefore shows up as stale pixels in the host test, as on the board. */
static uint8_t  bm_cdata[512][32];
static uint32_t bm_ctag[512];
static uint8_t  bm_cvalid[512];
static uint16_t bm_chalf[512];   /* 主机模型专用：行内哪些半字已经从内存取过 */
static uint32_t bm_chit, bm_cmiss;
static uint16_t bm_crd16(uint32_t a)
{
    uint32_t line = (a >> 5) & 511u, tag = a >> 14, o = a & 31u, hb = 1u << (o >> 1);
    if (!bm_cvalid[line] || bm_ctag[line] != tag) {
        bm_cvalid[line] = 1; bm_ctag[line] = tag; bm_chalf[line] = 0;
        ++bm_cmiss;
    } else {
        ++bm_chit;
    }
    /* 硬件缺失时整行 32 B 一次读入；主机上小图是独立的数组，整行读会越界（ASan 报错），
     * 所以按半字首次用到时再取。被画过的半字之后都从缓存返回，旧数据（缺 INV）照样会暴露。 */
    if (!(bm_chalf[line] & hb)) {
        const uint8_t *p = BLIT_MODEL_PTR(a);
        bm_cdata[line][o] = p[0]; bm_cdata[line][o + 1u] = p[1];
        bm_chalf[line] = (uint16_t)(bm_chalf[line] | hb);
    }
    return (uint16_t)(bm_cdata[line][o] | (bm_cdata[line][o + 1u] << 8));
}

/* f = DST DSTR SRC SSTR A ASTR SIZE COLOR KEY (same order as the registers) */
static void bm_exec_f(const uint32_t *f, uint32_t cmd)
{
    uint32_t dst = f[0], dstr = f[1] & 0xFFFFu, src = f[2], sstr = f[3] & 0xFFFFu;
    uint32_t aad = f[4], astr = f[5] & 0xFFFFu;
    uint32_t w = f[6] & 0x7FFu, h = (f[6] >> 16) & 0x7FFu;
    uint32_t color = f[7] & 0xFFFFu, key = f[8] & 0xFFFFu;
    uint32_t op = cmd & 1u, key_en = (cmd >> 1) & 1u, apl = ((cmd >> 2) & 1u) & op;
    uint32_t ga = (cmd >> 8) & 0xFFu, ga9 = ga + (ga >> 7);
    uint32_t add = (cmd >> 6) & 1u;                   /* CMD[6] ADD: dst + src * a, saturated */
    int blend = ga != 255u || apl || add, cached = op && (cmd & 0x10u);
    uint32_t r, p;
    ++bm_pushed;
    if (cmd & 0x20u) memset(bm_cvalid, 0, sizeof bm_cvalid);   /* INV, also on a size-0 command */
    if (!w || !h) return;
    if (bm_band_on) {                                 /* band frame: rows of the frame only */
        const uint32_t s = bm_bgeo & 0xFFFFu, rows = ((bm_bgeo >> 16) & 63u) * 16u;
        const uint32_t off = dst - bm_band_fb;
        if (s < 256u || bm_brecip != (1u << 24) / (s >> 4) + 1u) bm_fault |= 8u;   /* BAND_RECIP wrong */
        if (off < rows * s) {
            const uint32_t y = off / s;
            if (h > 1u && dstr != s) { bm_berr = 1; return; }   /* rows off the frame stride: dropped */
            if (y + h > rows) h = rows - y;
            ++bm_band_cmds;
        } else {
            ++bm_band_other;                          /* another target: drawn right away */
            BLIT_MODEL_ON_CMD(dst);
        }
    } else {
        BLIT_MODEL_ON_CMD(dst);
    }
    ++bm_cmds;
    for (r = 0; r < h; r++) {
        for (p = 0; p < w; p++) {
            uint32_t da = dst + r * dstr + 2u * p;
            uint32_t s = !op ? color : cached ? bm_crd16(src + r * sstr + 2u * p)
                                              : bm_rd16(src + r * sstr + 2u * p);
            if (op && key_en && s == key) continue;
            if (blend) {
                uint32_t a9 = ga9, d = bm_rd16(da), out = 0;
                static const uint32_t sh[3] = { 11, 5, 0 }, mk[3] = { 31, 63, 31 };
                int c;
                if (apl) {
                    uint32_t e = (*BLIT_MODEL_PTR(aad + r * astr + p) * ga9) >> 8;
                    a9 = e + (e >> 7);
                }
                for (c = 0; c < 3; c++) {
                    uint32_t sc = (s >> sh[c]) & mk[c], dc = (d >> sh[c]) & mk[c];
                    if (add) {
                        uint32_t v = dc + ((sc * a9 + 128u) >> 8);
                        out |= (v > mk[c] ? mk[c] : v) << sh[c];
                    } else {
                        out |= ((sc * a9 + dc * (256u - a9) + 128u) >> 8) << sh[c];
                    }
                }
                s = out;
            }
            bm_wr16(da, (uint16_t)s);
        }
    }
    /* rough beat counts for the perf registers */
    bm_wbeat += h * ((w + 2u) / 2u);
    bm_rbeat += h * (((op ? 1u : 0u) + (blend ? 1u : 0u)) * ((w + 2u) / 2u) + (apl ? (w + 4u) / 4u : 0u));
}

static void bm_exec(uint32_t cmd) { bm_exec_f(&bm_reg[4], cmd); }

/* list command word = {inv, cache, alpha, flags} -> CMD register layout */
static uint32_t bm_lcmd(uint32_t hdr)
{
    return ((hdr >> 12) & 0xFu) | (((hdr >> 24) & 7u) << 4) | (((hdr >> 16) & 0xFFu) << 8);   /* [26:24] -> CMD[6:4] */
}

static void bm_draw_slot(uint32_t n, uint32_t off)
{
    uint32_t f[9];
    memcpy(f, bm_lctx, sizeof f);
    f[0] = bm_lctx[BM_L_BASE] + (off & 0x1FFFFFFu);
    f[2] = bm_slot[n].src; f[3] = bm_slot[n].sstr; f[6] = bm_slot[n].size;
    bm_exec_f(f, bm_slot[n].cmd);
}

static uint32_t bm_rd32(uint32_t a)
{
    const uint8_t *p = BLIT_MODEL_PTR(a);
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void bm_list_word(uint32_t w)
{
    ++bm_lwords;
    if (bm_band_w8) {                                 /* BANDS: frame base address word */
        const uint32_t n = ((bm_bgeo >> 16) & 63u) * 16u * (bm_bgeo & 0xFFFFu);
        bm_band_w8 = 0;
        bm_band_on = 1;
        bm_band_fb = w;
        BLIT_MODEL_ON_BANDS(w);
        memset(BLIT_MODEL_PTR(w), 0, n);              /* the band window starts black */
        return;
    }
    if(bm_sq_count){
        static const uint32_t colors[]={0xf32a,0x3dbb,0xb70d,0xf5eb,0x9bdc,0xeb50,0x7698,0xf77a};
        uint32_t n=bm_sq_count-1u;bm_sq_count=0;bm_sq_items+=n;++bm_sq_commands;
        for(uint32_t i=0;i<n;i++){
            uint32_t xy=bm_rd32(w+8*i),m=bm_rd32(w+8*i+4),sz=(m>>16)&255u;
            uint32_t f[10]={0};f[0]=bm_lctx[BM_L_BASE]+(xy>>20)*bm_lctx[BM_L_DSTR]+((xy&65535u)>>4)*2;
            f[1]=bm_lctx[BM_L_DSTR];f[6]=sz|(sz<<16);f[7]=colors[(m>>24)&7];
            bm_exec_f(f,(bm_sq_alpha&&(i&1)?150u:255u)<<8);
        }return;
    }
    if (bm_lftab) {                                   /* FTAB 的地址字：逐项填充 */
        uint32_t k, n = bm_lftab - 1u, a = w & ~3u;
        bm_lftab = 0;
        for (k = 0; k < n; k++, a += 8u) {
            const uint32_t w0 = bm_rd32(a), w1 = bm_rd32(a + 4u);
            const uint32_t x = w0 & 0xFFFu, y = (w0 >> 12) & 0xFFFu, fw = w0 >> 24, fh = (w1 >> 16) & 0xFFu;
            uint32_t f[9];
            if (!fw || !fh) continue;
            memcpy(f, bm_lctx, sizeof f);
            f[0] = bm_lctx[BM_L_BASE] + ((y * (bm_lctx[BM_L_DSTR] & 0xFFFFu) + 2u * x) & 0x1FFFFFFu);
            f[6] = (fh << 16) | fw;
            f[7] = w1 & 0xFFFFu;
            bm_exec_f(f, ((w1 >> 24) << 8) | (bm_lfadd ? 0x40u : 0u));   /* FILL, global alpha, ADD */
        }
        return;
    }
    if (bm_ltab) {                                    /* TAB 的地址字：逐项画 8 B 的表项 */
        uint32_t k, n = bm_ltab - 1u, a = w & ~3u;
        bm_ltab = 0;
        for (k = 0; k < n; k++, a += 8u) {
            const uint32_t xy = bm_rd32(a), sp = bm_rd32(a + 4u);
            const uint32_t x = xy & 0xFFFu, y = (xy >> 16) & 0xFFFu;
            bm_draw_slot(sp & 63u, y * (bm_lctx[BM_L_DSTR] & 0xFFFFu) + 2u * x);
        }
        return;
    }
    if (bm_lmask) {                                   /* argument of a REC */
        uint32_t i = 0;
        while (!((bm_lmask >> i) & 1u)) ++i;
        bm_lctx[i] = w;
        bm_lmask &= bm_lmask - 1u;
    } else {
        bm_lhdr = w;
        if (w & 0x80000000u) {                        /* DRAW */
            bm_draw_slot((w >> 25) & 63u, w);
            return;
        }
        if(BLIT_MODEL_SQTAB && (w>>28)==6u){
            if((w&0x0ffe0000u)||(w&65535u)>30000u){bm_lerr=1;return;}
            bm_sq_count=(w&65535u)+1u;bm_sq_alpha=(w>>16)&1u;return;
    }
        if ((w >> 28) == 7u) {                        /* FTAB：下一个字是填充表地址 */
            bm_lftab = (w & 0xFFFFu) + 1u;
            bm_lfadd = (w >> 26) & 1u;
            return;
        }
        if ((w >> 28) == 2u) {                        /* TAB：下一个字是表地址 */
            bm_ltab = (w & 0xFFFFu) + 1u;
            return;
        }
        if ((w >> 28) == 1u) {                        /* SLOT */
            uint32_t n = w & 63u;
            bm_slot[n].src = bm_lctx[BM_L_SRC];
            bm_slot[n].sstr = bm_lctx[BM_L_SSTR] & 0xFFFFu;
            bm_slot[n].size = bm_lctx[BM_L_SIZE] & 0x07FF07FFu;
            bm_slot[n].cmd = bm_lcmd(w) & ~0x20u;     /* a slot keeps the cache bit, not INV */
            return;
        }
        if ((w >> 28) == 3u) {                        /* BANDS (not nested) */
            if (bm_band_on) bm_lerr = 1; else bm_band_w8 = 1;
            return;
        }
        if ((w >> 28) == 4u) {                        /* ENDB */
            if (!bm_band_on) bm_lerr = 1; else { bm_band_on = 0; ++bm_frames; BLIT_MODEL_ON_ENDB(bm_frames); }
            return;
        }
        if ((w >> 28) != 0u) { bm_lerr = 1; return; } /* reserved: skipped */
        bm_lmask = w & 0x3FFu;
    }
    if (!bm_lmask && !(bm_lhdr & 0x400u))             /* REC complete -> push */
        bm_exec_f(bm_lctx, bm_lcmd(bm_lhdr));
}

static void bm_list_run(void)
{
    while (bm_lptr != bm_lend) {
        uint8_t *p = BLIT_MODEL_PTR(bm_lptr);
        bm_list_word((uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24));
        bm_lptr += 4u;
    }
}

uint32_t blit_model_rd(uint32_t off)
{
    BLIT_MODEL_TICK();
    switch (off & 0xFFCu) {
    case 0x00: return 0x424C5431u;
    case 0x04:                                        /* idle, queue empty; busy while a band frame is open */
        if (bm_band_on && ++bm_spin > 2000000u) {     /* a wait that can never end on the board */
            bm_fault |= 1u; bm_band_on = 0; ++bm_frames;
        }
        {   /* busy / list active / band active until every closed band frame is finished */
            const uint32_t act = bm_band_on || BLIT_MODEL_FRAMES_DONE(bm_frames) != bm_frames;
            return (256u << 16) | (bm_berr << 7) | (act << 5) | (bm_lerr << 4) | (act ? 9u : 0u);
        }
    case 0x0C: return 0xBF0A0908u;                    /* [24] list [25] LIMIT [26] LIMIT words [27] cache [28] TAB [29] stall counters [31] band mode */
    case 0x94: return bm_bgeo;
    case 0x98: return bm_brecip;
    case 0x9C: return bm_bbin;
    case 0xA0: return BLIT_MODEL_FRAMES_DONE(bm_frames);
    case 0xBC: return BLIT_MODEL_WIN > 1280u ? (3u | 4u | 16u | 32u | (BLIT_MODEL_SQTAB?8u:0u) | (45u << 8)) : 1u;   /* CAPS2: [0] add [2] 2560 window [15:8] bands */
    case 0x38: return bm_lptr;
    case 0x3C: return bm_lend;
    case 0x40: return 0;
    case 0x44: return bm_cmds;
    case 0x48: return bm_rbeat;
    case 0x4C: return bm_wbeat;
    case 0x50: return bm_pushed;
    case 0x54: return bm_limit;
    case 0x5C: return bm_chit;                        /* per pixel here, per line request in the RTL */
    case 0x60: return bm_cmiss;
    case 0x64: case 0x68: case 0x6C: case 0x70:       /* stall counters: the model never waits */
    case 0x74: case 0x78: case 0x7C: case 0x80: return 0;
    default:  return (off & 0xFFCu) < 0x80u ? bm_reg[(off >> 2) & 31u] : 0u;   /* 0xA4.. band counters: 0 */
    }
}

void blit_model_wr(uint32_t off, uint32_t v)
{
    off &= 0xFFCu;
    if (off == 0x08) { if (v & 1u) bm_cmds = bm_rbeat = bm_wbeat = bm_chit = bm_cmiss = 0; if (v & 2u) bm_lerr = bm_berr = 0; return; }
    if (off == 0x34) { if (bm_band_on) bm_fault |= 2u; bm_exec(v); return; }   /* the engine would stall this write */
    if (off == 0x38) { if (bm_band_on) bm_fault |= 4u; bm_lptr = bm_lend = v & ~3u; return; }
    if (off == 0x3C) { bm_spin = 0; bm_lend = v & ~3u; bm_list_run(); return; }
    if (off == 0x94) {
        uint32_t s = v & 0xFFF0u, nb = (v >> 16) & 63u;
        if (s > BLIT_MODEL_WIN) s = BLIT_MODEL_WIN;
        nb = nb == 0 ? 1u : nb > BM_MAXB ? BM_MAXB : nb;
        bm_bgeo = (v & 0x80000000u) | (nb << 16) | s;
        return;
    }
    if (off == 0x98) { bm_brecip = v & 0x1FFFFFu; return; }
    if (off == 0x9C) {
        uint32_t sh = v & 31u;
        sh = sh < 12u ? 12u : sh > 20u ? 20u : sh;
        bm_bbin = (v & 0xFFFFF000u) | sh;
        return;
    }
    if (off == 0x58) { bm_list_word(v); return; }
    if (off == 0x54) {
        uint32_t rd = v & 31u, wr = (v >> 8) & 255u, rw = (v >> 16) & 2047u;
        rd = rd == 0 ? 1u : rd > 16u ? 16u : rd;
        wr = wr == 0 ? 1u : wr > 128u ? 128u : wr;
        rw = rw == 0 ? 1u : rw > 1024u ? 1024u : rw;
        bm_limit = (rw << 16) | (wr << 8) | rd;
        return;
    }
    if (off >= 0x10 && off <= 0x30) bm_reg[off >> 2] = v;
}

static void blit_model_reset(void)
{
    uint32_t i;
    for (i = 0; i < 32; i++) bm_reg[i] = 0;
    bm_reg[12] = 0xF81Fu;
    bm_cmds = bm_rbeat = bm_wbeat = bm_pushed = 0;
    memset(bm_lctx, 0, sizeof bm_lctx);
    memset(bm_slot, 0, sizeof bm_slot);
    bm_lctx[BM_L_DSTR] = 1280u;
    bm_lctx[BM_L_KEY] = 0xF81Fu;
    bm_lptr = bm_lend = bm_lerr = bm_lhdr = bm_lmask = bm_lwords = bm_ltab = 0;
    bm_lftab = bm_lfadd = 0;
    memset(bm_cvalid, 0, sizeof bm_cvalid);
    bm_chit = bm_cmiss = 0;bm_sq_count=bm_sq_items=bm_sq_commands=0;
    bm_bgeo = (30u << 16) | 1280u; bm_brecip = 209716u; bm_bbin = 0x03000000u | 18u;
    bm_band_on = bm_band_fb = bm_band_w8 = bm_frames = bm_berr = bm_fault = bm_spin = 0;
    bm_band_cmds = bm_band_other = 0;
}

#endif
