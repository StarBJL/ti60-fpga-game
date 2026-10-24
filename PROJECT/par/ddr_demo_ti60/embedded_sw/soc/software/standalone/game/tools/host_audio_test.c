/*
 * host_audio_test.c - host run of the audio driver (src/audio.c) and the danmaku
 * sound hooks (src/th_sound.c) against a C model of the hardware:
 *   hdmi_fb audio registers + audio_mixer (16 voices, same arithmetic as
 *   rtl/audio/audio_mixer.v; the Python twin of this model is bit-exact against
 *   the RTL in sim/audio/tb_mixer.py).
 *
 * The real game logic runs (title, then a NORMAL game played by the bot), the
 * driver programs the "mixer", the model mixes 800 samples per 1/60 s tick and
 * the result is written as a WAV file = what the board should play over HDMI.
 * Checks: bank found, every sample read inside the bank, sound effects never on
 * the BGM voices, BGM follows title -> stage 1 -> boss 1, the main events made a
 * sound, tone / master volume registers.
 *
 *   gcc -O2 -w -no-pie -fno-pie -DBLIT_HOST_MODEL -DAUD_HOST_REGS -Isrc -Itools -o host_audio_test \
 *       tools/host_audio_test.c src/audio.c src/th_sound.c src/th_tab.c src/th_game.c src/th_pattern.c \
 *       src/th_stage2.c src/th_art.c src/th_pix.c src/gfx.c src/mini_snprintf.c
 *   ./host_audio_test [sound/sound_bank.bin] [preview.wav] [seconds]     (PASS / FAIL)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include "audio.h"
#include "th.h"
#include "th_sound.h"

static int errors;
static void fail(const char *m, long a) { if (++errors <= 20) printf("ERROR: %s (%ld)\n", m, a); }

/* ------------------------------------------------------------------ hardware model */
static uint32_t h_ctrl = 0x33, h_sent, h_uf;
static uint32_t vw[16][6];
static uint64_t vpos[16];
static uint32_t m_active, m_kon, m_en, m_mvol = 256, m_target = 64, m_samples, m_clips;
static uint32_t bank_size;
static long bad_reads;
static uint32_t keyon_cnt[16], keyon_snd_cnt[256];
static int bgm_voice_used_by_se;

static int snd_of_addr(uint32_t a)
{
    const uint32_t *h = (const uint32_t *)(uintptr_t)AUD_BANK_ADDR;
    const AudEntry *e = (const AudEntry *)(uintptr_t)(AUD_BANK_ADDR + 16u);
    uint32_t i;
    for (i = 0; i < h[1]; i++) if (AUD_BANK_ADDR + e[i].off == a) return (int)i;
    return -1;
}

uint32_t aud_host_rd(int mixer, uint32_t off)
{
    if (!mixer) {
        switch (off) {
        case AUD_R_CTRL: return h_ctrl;
        case AUD_R_SENT: return h_sent;
        case AUD_R_UF:   return h_uf;
        case AUD_R_CAPS: return 0x0801u;
        default: return 0;
        }
    }
    if (off & 0x200u) return 0;
    switch (off) {
    case MIX_R_ID:      return MIX_ID_VALUE;
    case MIX_R_CTRL:    return m_en;
    case MIX_R_MVOL:    return m_mvol;
    case MIX_R_TARGET:  return m_target;
    case MIX_R_ACTIVE:  return m_active;
    case MIX_R_SAMPLES: return m_samples;
    case MIX_R_CLIPS:   return m_clips;
    case MIX_R_CAPS:    return 0x00012010u;
    default: return 0;
    }
}

void aud_host_wr(int mixer, uint32_t off, uint32_t v)
{
    if (!mixer) { if (off == AUD_R_CTRL) h_ctrl = v & 0x3Fu; return; }
    if (off & 0x200u) {
        const uint32_t k = (off >> 5) & 15u, w = (off >> 2) & 7u;
        if (w < 6) vw[k][w] = v;
        return;
    }
    switch (off) {
    case MIX_R_CTRL:   m_en = v & 1u; break;
    case MIX_R_MVOL:   m_mvol = v & 0x1FFu; break;
    case MIX_R_TARGET: m_target = v & 0x1FFu; break;
    case MIX_R_KEYON: {
        int k;
        m_kon |= v & 0xFFFFu;
        for (k = 0; k < 16; k++) if (v >> k & 1u) {
            const int s = snd_of_addr(vw[k][0]);
            ++keyon_cnt[k];
            if (s >= 0) ++keyon_snd_cnt[s];
            if (k < AUD_BGM_VOICES && s >= 0 && strncmp(aud_name(s), "bgm", 3) != 0) ++bgm_voice_used_by_se;
        }
        break;
    }
    case MIX_R_KEYOFF: m_active &= ~v; m_kon &= ~v; break;
    default: break;
    }
}

static int16_t s16(uint32_t x) { return (int16_t)(x & 0xFFFFu); }

static uint8_t mem8(uint32_t a)
{
    a &= 0x0FFFFFFFu;
    if (a < AUD_BANK_ADDR || a >= AUD_BANK_ADDR + bank_size) { ++bad_reads; return 0; }
    return *(const uint8_t *)(uintptr_t)a;
}

static void sample(int k, uint32_t i, int32_t *l, int32_t *r)
{
    uint32_t fmt = (vw[k][3] >> 16) & 3u, a;
    if (fmt == 3) fmt = 1;
    if (fmt == 0) { int8_t b = (int8_t)mem8(vw[k][0] + i); *l = *r = b * 256; return; }
    if (fmt == 1) { a = vw[k][0] + 2u * i; *l = *r = s16(mem8(a) | mem8(a + 1) << 8); return; }
    a = vw[k][0] + 4u * i;
    *l = s16(mem8(a) | mem8(a + 1) << 8);
    *r = s16(mem8(a + 2) | mem8(a + 3) << 8);
}

/* one 48 kHz output sample, exactly as audio_mixer.v */
static uint32_t mix(void)
{
    int64_t al = 0, ar = 0;
    int k;
    for (k = 0; k < 16; k++) {
        const uint32_t bit = 1u << k;
        uint32_t len, lst, lpen, step, vl, vr, i0, i1, ni, fr;
        int has1;
        int32_t s0l, s0r, s1l, s1r, il, ir;
        uint64_t np;
        if (!((m_active | m_kon) & bit)) continue;
        len = vw[k][1] & 0xFFFFFFu; lst = vw[k][2] & 0xFFFFFFu; lpen = vw[k][2] >> 31;
        step = vw[k][3] & 0xFFFFu; vl = vw[k][4] & 0x1FFu; vr = (vw[k][4] >> 16) & 0x1FFu;
        if (m_kon & bit) { vpos[k] = (uint64_t)(vw[k][5] & 0xFFFFFFu) << 12; m_active |= bit; m_kon &= ~bit; }
        i0 = (uint32_t)(vpos[k] >> 12);
        if (i0 >= len) { m_active &= ~bit; continue; }
        i1 = i0 + 1u;
        has1 = i1 < len || lpen;
        if (i1 >= len) i1 = lst;
        sample(k, i0, &s0l, &s0r);
        if (has1) sample(k, i1, &s1l, &s1r); else { s1l = s0l; s1r = s0r; }
        fr = (uint32_t)(vpos[k] & 0xFFFu);
        il = s0l + (int32_t)(((int64_t)(s1l - s0l) * fr) >> 12);
        ir = s0r + (int32_t)(((int64_t)(s1r - s0r) * fr) >> 12);
        al += (int64_t)il * vl;
        ar += (int64_t)ir * vr;
        np = vpos[k] + step;
        ni = (uint32_t)(np >> 12);
        if (ni >= len) {
            if (lpen) {
                const uint32_t nwr = ni - (len - lst);
                vpos[k] = ((uint64_t)(nwr >= len ? lst : nwr) << 12) | (np & 0xFFFu);
            } else { vpos[k] = np; m_active &= ~bit; }
        } else vpos[k] = np;
    }
    {
        int64_t xl = ((al >> 8) * (int64_t)m_mvol) >> 8, xr = ((ar >> 8) * (int64_t)m_mvol) >> 8;
        if (xl > 32767 || xl < -32768 || xr > 32767 || xr < -32768) ++m_clips;
        if (xl > 32767) xl = 32767; if (xl < -32768) xl = -32768;
        if (xr > 32767) xr = 32767; if (xr < -32768) xr = -32768;
        ++m_samples;
        return (uint32_t)(uint16_t)xl | (uint32_t)(uint16_t)xr << 16;
    }
}

/* ------------------------------------------------------------------ WAV out */
static FILE *wav;
static uint32_t wav_n;
static void wav_open(const char *p)
{
    static const uint8_t h[44] = { 0 };
    wav = fopen(p, "wb");
    if (wav) fwrite(h, 1, 44, wav);
}
static void wav_close(void)
{
    uint32_t h[11];
    if (!wav) return;
    memcpy(&h[0], "RIFF", 4); h[1] = 36u + wav_n * 4u; memcpy(&h[2], "WAVE", 4); memcpy(&h[3], "fmt ", 4);
    h[4] = 16; h[5] = 1u | (2u << 16); h[6] = 48000; h[7] = 48000u * 4u; h[8] = 4u | (16u << 16);
    memcpy(&h[9], "data", 4); h[10] = wav_n * 4u;
    fseek(wav, 0, SEEK_SET);
    fwrite(h, 4, 11, wav);
    fclose(wav);
}

/* ------------------------------------------------------------------ main */
static Th th;

int main(int argc, char **argv)
{
    const char *bank = argc > 1 ? argv[1] : "sound/sound_bank.bin";
    const char *out = argc > 2 ? argv[2] : "sound_preview.wav";
    const int secs = argc > 3 ? atoi(argv[3]) : 150;
    FILE *f = fopen(bank, "rb");
    void *m;
    uint32_t ev_cnt[TS_COUNT] = { 0 }, tick, smp;
    int bgm_seq[16], nbgm = 0, i;

    m = mmap((void *)(uintptr_t)AUD_BANK_ADDR, 16u << 20, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
    if (m == MAP_FAILED || !f) { printf("mmap / open %s failed\n", bank); return 1; }
    bank_size = (uint32_t)fread(m, 1, 16u << 20, f);
    fclose(f);

    if (aud_init() != 0) fail("aud_init", -1);
    printf("bank %s: %d sounds, %u KB\n", bank, aud.nsnd, (unsigned)(aud.bank_bytes / 1024u));
    if (aud.nsnd <= 0) fail("no sounds in the bank", 0);
    if (m_en != 1 || h_ctrl != 0x33u) fail("init registers", (long)h_ctrl);
    aud_tone(1);
    if (h_ctrl != 0x37u) fail("tone on", (long)h_ctrl);
    aud_tone(0);
    aud_master(128);
    if (m_mvol != 128) fail("master", (long)m_mvol);
    aud_master(256);
    th_sound_init();
    printf("danmaku events with a sound: %d of %d\n", th_sound_found(), TS_COUNT);

    th_init(&th, 12345u);
    wav_open(out);
    for (tick = 0; tick < (uint32_t)secs * 60u; tick++) {
        uint32_t ev;
        int bot = 1;
        if (tick == 150) th_start(&th, TH_NORMAL);                    /* 2.5 s title, then a game */
        if (th.mode == TH_TITLE && tick > 150) bot = 1;
        th_tick(&th, 0, 0, bot);
        ev = th.sfx;
        for (i = 0; i < TS_COUNT; i++) if (ev >> i & 1u) ++ev_cnt[i];
        th_sound_tick(&th);
        if (th.sfx) fail("sfx not cleared", (long)th.sfx);
        {
            const int b = aud_bgm_current();
            if (!nbgm || bgm_seq[nbgm - 1] != b) { if (nbgm < 16) bgm_seq[nbgm++] = b; }
        }
        for (smp = 0; smp < 800u; smp++) {
            const uint32_t s = mix();
            if (wav) { fwrite(&s, 4, 1, wav); ++wav_n; }
        }
        ++h_sent;
        if (th.mode == TH_CLEAR) break;
    }
    wav_close();

    printf("%u ticks (%.1f s game time), mode %d stage %d boss %d, score %u\n", tick, tick / 60.0, th.mode,
           th.stage + 1, th.boss.active, (unsigned)th.score);
    printf("events (ticks with the event):");
    for (i = 0; i < TS_COUNT; i++) printf(" %d:%u", i, (unsigned)ev_cnt[i]);
    printf("\nbgm sequence:");
    for (i = 0; i < nbgm; i++) printf(" %s", bgm_seq[i] < 0 ? "-" : aud_name(bgm_seq[i]));
    printf("\nkey-ons per voice:");
    for (i = 0; i < 16; i++) printf(" %u", (unsigned)keyon_cnt[i]);
    printf("\nplays per sound:");
    for (i = 0; i < aud.nsnd; i++) if (keyon_snd_cnt[i]) printf(" %s:%u", aud_name(i), (unsigned)keyon_snd_cnt[i]);
    printf("\nmixed %u samples, clipped %u, reads outside the bank %ld -> %s\n", (unsigned)m_samples,
           (unsigned)m_clips, bad_reads, wav ? out : "(no wav)");

    if (bad_reads) fail("sample reads outside the bank", bad_reads);
    if (bgm_voice_used_by_se) fail("sound effect on a BGM voice", bgm_voice_used_by_se);
    {
        static const char *const need[] = { "shot", "graze", "kill", "item", "enemy_fire", "warning", "spell", "phase" };
        for (i = 0; i < (int)(sizeof need / sizeof need[0]); i++) {
            const int s = aud_find(need[i]);
            if (s >= 0 && !keyon_snd_cnt[s]) { printf("  (no %s played)\n", need[i]); fail("event never sounded", i); }
        }
    }
    if (aud_find("bgm_title") >= 0 && (nbgm < 1 || bgm_seq[0] != aud_find("bgm_title"))) fail("title music", 0);
    if (aud_find("bgm_stage1") >= 0) {
        int seen = 0;
        for (i = 0; i < nbgm; i++) seen |= bgm_seq[i] == aud_find("bgm_stage1") ? 1 : bgm_seq[i] == aud_find("bgm_boss1") ? 2 : 0;
        if (seen != 3) fail("stage 1 / boss 1 music", seen);
    }
    printf(errors ? "FAIL (%d errors)\n" : "PASS\n", errors);
    return errors ? 1 : 0;
}
