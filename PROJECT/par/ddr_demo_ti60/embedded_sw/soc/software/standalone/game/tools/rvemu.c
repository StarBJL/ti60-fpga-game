/* Minimal RV32IM emulator for game.elf with Sapphire SoC MMIO mocks. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MEMSZ (32u<<20)
static uint8_t *mem;
static uint32_t x[32], pc;
static uint64_t cyc;
static uint32_t hdmi_ctrl, hdmi_addr, hdmi_cur, hdmi_frames_base;
static uint64_t max_cyc;
static int hdmi_enabled_reported;
static uint64_t hist[1<<16]; static uint32_t minsp=0xffffffff;

static void die(const char *m, uint32_t a);
/* BitBlt engine at 0xF8101000: functional C model (never busy); -DNO_BLIT = old behaviour (ID 0) */
#ifndef NO_BLIT
static uint8_t *bm_ptr(uint32_t a) { if (a >= MEMSZ - 4u || a < 0x1000u) die("BLIT DMA OUT OF RANGE", a); return mem + a; }
#define BLIT_MODEL_PTR(a) bm_ptr(a)
#include "blit_model.h"
#endif
static void die(const char *m, uint32_t a) {
    fprintf(stderr, "\n*** %s addr=%08x pc=%08x cyc=%llu (%.3f s)\n", m, a, pc,
            (unsigned long long)cyc, cyc / 1e8);
    exit(2);
}
static uint32_t hdmi_frames(void) { return (uint32_t)(cyc / 1666667u); }

static uint32_t ld(uint32_t a, int sz) {
    if (a & (sz - 1)) die("MISALIGNED LOAD", a);
    if (a < MEMSZ) {
        if (a < 0x1000) die("LOAD NULL PAGE", a);
        uint32_t v = 0; memcpy(&v, mem + a, sz); return v;
    }
    if (a >= 0xf8010000 && a < 0xf8010100) {       /* UART */
        if ((a & 0xff) == 4) return (0x80u << 16);   /* tx vacancy, rx empty */
        return 0;
    }
    if (a >= 0xf8b00000 && a < 0xf8b10000) {         /* CLINT */
        uint32_t o = a - 0xf8b00000;
        if (o == 0xbff8) return (uint32_t)cyc;
        if (o == 0xbffc) return (uint32_t)(cyc >> 32);
        return 0;
    }
#ifndef NO_BLIT
    if (a >= 0xf8101000 && a < 0xf8102000) return blit_model_rd(a & 0xfff);
#endif
    if (a >= 0xf8100000 && a < 0xf8110000) {
        switch (a & 0xff) {
        case 0x00: return hdmi_ctrl;
        case 0x08: return hdmi_cur;
        case 0x0c: return 0;
        case 0x10: return hdmi_frames();
        case 0x14: return (480u << 16) | 640u;
        case 0x18: return 0;
        case 0x1c: return 0x48444D49u;
        }
        return 0;
    }
    return 0;  /* other APB */
}
static void st(uint32_t a, uint32_t v, int sz) {
    if (a & (sz - 1)) die("MISALIGNED STORE", a);
    if (a < MEMSZ) {
        if (a < 0x1000) die("STORE NULL PAGE", a);
        memcpy(mem + a, &v, sz); return;
    }
    if (a == 0xf8010000) { putchar((int)(v & 0xff)); fflush(stdout); return; }
#ifndef NO_BLIT
    if (a >= 0xf8101000 && a < 0xf8102000) { blit_model_wr(a & 0xfff, v); return; }
#endif
    if (a >= 0xf8100000 && a < 0xf8110000) {
        if ((a & 0xff) == 0) {
            hdmi_ctrl = v;
            if ((v & 1) && !hdmi_enabled_reported) {
                hdmi_enabled_reported = 1;
                fprintf(stderr, "\n[emu] HDMI enabled at cyc %llu (%.3f s)\n",
                        (unsigned long long)cyc, cyc / 1e8);
            }
        }
        if ((a & 0xff) == 4) { hdmi_addr = v; hdmi_cur = v; }
        return;
    }
}

static int32_t sx(uint32_t v, int b) { return (int32_t)(v << (32 - b)) >> (32 - b); }

int main(int argc, char **argv) {
    FILE *f = fopen(argv[1], "rb");
    max_cyc = argc > 2 ? strtoull(argv[2], 0, 0) : 3000000000ull;
    mem = malloc(MEMSZ); { uint32_t r=12345; for (uint32_t i=0;i<MEMSZ;i++){ r=r*1103515245u+12345u; mem[i]=(uint8_t)(r>>16);} }
    uint8_t eh[52]; fread(eh, 1, 52, f);
    uint32_t entry, phoff; uint16_t phnum, phentsz;
    memcpy(&entry, eh + 24, 4); memcpy(&phoff, eh + 28, 4);
    memcpy(&phentsz, eh + 42, 2); memcpy(&phnum, eh + 44, 2);
    for (int i = 0; i < phnum; i++) {
        uint32_t ph[8];
        fseek(f, phoff + i * phentsz, SEEK_SET); fread(ph, 4, 8, f);
        if (ph[0] != 1) continue;
        fseek(f, ph[1], SEEK_SET); fread(mem + ph[3], 1, ph[4], f);
        fprintf(stderr, "[emu] load %08x filesz %x memsz %x\n", ph[3], ph[4], ph[5]);
    }
    /* garbage in bss region like real DDR would have */
#ifndef NO_BLIT
    blit_model_reset();
#endif
    pc = entry;
    for (;;) {
        if (cyc >= max_cyc) { fprintf(stderr, "\n[emu] cycle limit, pc=%08x\n", pc); break; }
        if (pc < 0x1000 || pc >= MEMSZ) die("PC OUT OF RANGE", pc);
        hist[(pc >> 2) & 0xffff]++; if (x[2] && x[2] < minsp) minsp = x[2];
        uint32_t in; memcpy(&in, mem + pc, 4);
        uint32_t op = in & 0x7f, rd = (in >> 7) & 31, f3 = (in >> 12) & 7,
                 r1 = (in >> 15) & 31, r2 = (in >> 20) & 31, f7 = in >> 25;
        uint32_t a = x[r1], b = x[r2], npc = pc + 4, res = 0; int wr = 1;
        cyc++;
        switch (op) {
        case 0x37: res = in & 0xfffff000; break;
        case 0x17: res = pc + (in & 0xfffff000); break;
        case 0x6f: res = npc; npc = pc + sx(((in >> 31) << 20) | (((in >> 12) & 0xff) << 12) |
                                      (((in >> 20) & 1) << 11) | (((in >> 21) & 0x3ff) << 1), 21); break;
        case 0x67: res = npc; npc = (a + sx(in >> 20, 12)) & ~1u; break;
        case 0x63: {
            int32_t off = sx(((in >> 31) << 12) | (((in >> 7) & 1) << 11) | (((in >> 25) & 0x3f) << 5) |
                             (((in >> 8) & 0xf) << 1), 13);
            int t = 0; wr = 0;
            switch (f3) { case 0: t = a == b; break; case 1: t = a != b; break;
                case 4: t = (int32_t)a < (int32_t)b; break; case 5: t = (int32_t)a >= (int32_t)b; break;
                case 6: t = a < b; break; case 7: t = a >= b; break; default: die("BAD BRANCH", in); }
            if (t) npc = pc + off; break; }
        case 0x03: { uint32_t ad = a + sx(in >> 20, 12);
            switch (f3) { case 0: res = sx(ld(ad, 1), 8); break; case 1: res = sx(ld(ad, 2), 16); break;
                case 2: res = ld(ad, 4); break; case 4: res = ld(ad, 1) & 0xff; break;
                case 5: res = ld(ad, 2) & 0xffff; break; default: die("BAD LOAD", in); } break; }
        case 0x23: { uint32_t ad = a + sx(((in >> 25) << 5) | ((in >> 7) & 31), 12); wr = 0;
            switch (f3) { case 0: st(ad, b, 1); break; case 1: st(ad, b, 2); break; case 2: st(ad, b, 4); break;
                default: die("BAD STORE", in); } break; }
        case 0x13: { int32_t im = sx(in >> 20, 12); uint32_t sh = r2;
            switch (f3) { case 0: res = a + im; break; case 2: res = (int32_t)a < im; break;
                case 3: res = a < (uint32_t)im; break; case 4: res = a ^ im; break; case 6: res = a | im; break;
                case 7: res = a & im; break; case 1: res = a << sh; break;
                case 5: res = (f7 & 0x20) ? (uint32_t)((int32_t)a >> sh) : a >> sh; break; } break; }
        case 0x33:
            if (f7 == 1) {
                switch (f3) {
                case 0: res = a * b; break;
                case 1: res = (uint32_t)(((int64_t)(int32_t)a * (int32_t)b) >> 32); break;
                case 2: res = (uint32_t)(((int64_t)(int32_t)a * (uint64_t)b) >> 32); break;
                case 3: res = (uint32_t)(((uint64_t)a * b) >> 32); break;
                case 4: res = b == 0 ? 0xffffffff : ((int32_t)a == INT32_MIN && (int32_t)b == -1) ? a : (uint32_t)((int32_t)a / (int32_t)b); break;
                case 5: res = b == 0 ? 0xffffffff : a / b; break;
                case 6: res = b == 0 ? a : ((int32_t)a == INT32_MIN && (int32_t)b == -1) ? 0 : (uint32_t)((int32_t)a % (int32_t)b); break;
                case 7: res = b == 0 ? a : a % b; break;
                }
            } else {
                switch (f3) {
                case 0: res = (f7 & 0x20) ? a - b : a + b; break; case 1: res = a << (b & 31); break;
                case 2: res = (int32_t)a < (int32_t)b; break; case 3: res = a < b; break;
                case 4: res = a ^ b; break;
                case 5: res = (f7 & 0x20) ? (uint32_t)((int32_t)a >> (b & 31)) : a >> (b & 31); break;
                case 6: res = a | b; break; case 7: res = a & b; break; }
            }
            break;
        case 0x0f: wr = 0; break;                 /* fence */
        case 0x73:                                 /* csr: read 0, ignore writes */
            if (f3 == 0) { die("ECALL/EBREAK/MRET", in); }
            res = 0; break;
        default: die("ILLEGAL INSTRUCTION", in);
        }
        if (wr && rd) x[rd] = res;
        pc = npc;
    }
    /* hot spots */
    FILE *h = fopen("/tmp/g/hot.txt", "w");
    for (int i = 0; i < 65536; i++) if (hist[i] > 1000000) fprintf(h, "%08x %llu\n", i * 4, (unsigned long long)hist[i]);
    fclose(h);
    /* dump fb0/1/2 current */
    FILE *o = fopen("/tmp/g/fb.raw", "wb"); fwrite(mem + hdmi_cur, 1, 640 * 480 * 2, o); fclose(o);
    fprintf(stderr, "[emu] min sp %08x\n", minsp);
    fprintf(stderr, "[emu] hdmi_cur=%08x frames=%u\n", hdmi_cur, hdmi_frames());
    return 0;
}
