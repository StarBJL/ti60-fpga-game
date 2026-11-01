/*
 * audio.c - HDMI 音频驱动，见 audio.h。
 */
#include <string.h>
#include "audio.h"
#include "mini_snprintf.h"

AudState aud;

#if defined(BLIT_HOST_MODEL) && !defined(AUD_HOST_REGS)
/* 主机测试（host_main_test 等）：没有音频硬件，全部空操作 */
#define AUD_NO_HW 1
#endif

#ifdef AUD_HOST_REGS
/* tools/host_audio_test.c: registers go to a C model of hdmi_fb audio + audio_mixer */
uint32_t aud_host_rd(int mixer, uint32_t off);
void     aud_host_wr(int mixer, uint32_t off, uint32_t v);
static inline uint32_t hrd(uint32_t off) { return aud_host_rd(0, off); }
static inline void hwr(uint32_t off, uint32_t v) { aud_host_wr(0, off, v); }
static inline uint32_t mrd(uint32_t off) { return aud_host_rd(1, off); }
static inline void mwr(uint32_t off, uint32_t v) { aud_host_wr(1, off, v); }
#else
static inline uint32_t hrd(uint32_t off) { return *(volatile uint32_t *)(uintptr_t)(AUD_HDMI_BASE + off); }
static inline void hwr(uint32_t off, uint32_t v) { *(volatile uint32_t *)(uintptr_t)(AUD_HDMI_BASE + off) = v; }
static inline uint32_t mrd(uint32_t off) { return *(volatile uint32_t *)(uintptr_t)(AUD_MIX_BASE + off); }
static inline void mwr(uint32_t off, uint32_t v) { *(volatile uint32_t *)(uintptr_t)(AUD_MIX_BASE + off) = v; }
#endif

static const AudEntry *ent;          /* bank table (in DDR) */
static uint32_t bank_base;

/* voice bookkeeping */
static int16_t  snd_voice[256];      /* sound -> SE voice (0 = none yet) */
static int16_t  voice_snd[AUD_VOICES];
static uint32_t voice_age[AUD_VOICES], age_now;
/* BGM */
static int bgm_v = 0, bgm_snd = -1, bgm_vol, bgm_scale = 256;
static int fade_v = -1, fade_vol, fade_step;

static void vol_pan(int vol, int pan, uint32_t *reg)
{
    int l, r;
    if (vol < 0) vol = 0;
    if (vol > 256) vol = 256;
    if (pan < -128) pan = -128;
    if (pan > 127) pan = 127;
    l = pan > 0 ? vol * (128 - pan) / 128 : vol;      /* pan right: left quieter */
    r = pan < 0 ? vol * (128 + pan) / 128 : vol;
    *reg = (uint32_t)l | ((uint32_t)r << 16);
}

int aud_init(void)
{
    const uint32_t *h;
    memset(&aud, 0, sizeof aud);
    memset(snd_voice, 0, sizeof snd_voice);
    memset(voice_snd, 0xFF, sizeof voice_snd);
    memset(voice_age, 0, sizeof voice_age);
    bgm_v = 0; bgm_snd = -1; fade_v = -1; bgm_scale = 256;
    aud.master = 256;
#ifdef AUD_NO_HW
    return -1;
#else
    aud.hdmi = (hrd(AUD_R_CAPS) & 1u) != 0;
    aud.mixer = aud.hdmi && mrd(MIX_R_ID) == MIX_ID_VALUE;
    if (!aud.hdmi) return -1;
    hwr(AUD_R_CTRL, 0x33u);                       /* HDMI + audio packets, tone off */
    if (!aud.mixer) return -1;
    mwr(MIX_R_CTRL, 0);
    mwr(MIX_R_KEYOFF, 0xFFFFu);
    mwr(MIX_R_MVOL, 256);
    mwr(MIX_R_TARGET, 64);                        /* 1.3 ms of samples queued */
    mwr(MIX_R_CTRL, 1);
    (void)h;
    aud_check_bank();
    aud.bank_new = 0;
    return 0;
#endif
}

int aud_check_bank(void)
{
#ifdef AUD_NO_HW
    return 0;
#else
    const volatile uint32_t *h = (const volatile uint32_t *)(uintptr_t)AUD_BANK_ADDR;
    if (!aud.mixer || aud.nsnd) return 0;
#ifdef __riscv
    __asm__ volatile (".word(0x500F)" ::: "memory");   /* D$ invalidate all: JTAG may have written DDR */
#endif
    if (h[0] == SBK_MAGIC && h[1] > 0 && h[1] <= 256 && h[2] <= 0x01000000u) {
        bank_base = AUD_BANK_ADDR;
        ent = (const AudEntry *)(uintptr_t)(AUD_BANK_ADDR + 16u);
        aud.bank_bytes = h[2];
        aud.nsnd = (int)h[1];
        aud.bank_new = 1;
        return 1;
    }
    return 0;
#endif
}

void aud_tone(int on)
{
    aud.tone = on;
#ifndef AUD_NO_HW
    if (aud.hdmi) hwr(AUD_R_CTRL, on ? 0x37u : 0x33u);
#endif
}

void aud_master(int vol)
{
    if (vol < 0) vol = 0;
    if (vol > 256) vol = 256;
    aud.master = vol;
#ifndef AUD_NO_HW
    if (aud.mixer) mwr(MIX_R_MVOL, (uint32_t)vol);
#endif
}

int aud_find(const char *name)
{
    int i;
    for (i = 0; i < aud.nsnd; i++)
        if (strncmp(ent[i].name, name, 16) == 0) return i;
    return -1;
}

const char *aud_name(int snd) { return snd >= 0 && snd < aud.nsnd ? ent[snd].name : "?"; }
uint32_t aud_frames(int snd)  { return snd >= 0 && snd < aud.nsnd ? ent[snd].frames : 0u; }
uint32_t aud_rate(int snd)    { return snd >= 0 && snd < aud.nsnd ? (ent[snd].rate_fmt & 0x1FFFFu) : 0u; }

void aud_stop(int voice)
{
    if (voice < 0 || voice >= AUD_VOICES) return;
#ifndef AUD_NO_HW
    if (aud.mixer) mwr(MIX_R_KEYOFF, 1u << voice);
#endif
}

void aud_voice_vol(int voice, int vol, int pan)
{
    uint32_t r;
    if (voice < 0 || voice >= AUD_VOICES || !aud.mixer) return;
    vol_pan(vol, pan, &r);
#ifndef AUD_NO_HW
    mwr(MIX_V(voice, MIX_V_VOL), r);
#endif
}

void aud_voice(int voice, int snd, int vol, int pan, int pitch_q12, int loop)
{
    const AudEntry *e;
    uint32_t rate, fmt, step, vr;
    if (!aud.mixer || snd < 0 || snd >= aud.nsnd || voice < 0 || voice >= AUD_VOICES) return;
    e = &ent[snd];
    rate = e->rate_fmt & 0x1FFFFu;
    fmt = (e->rate_fmt >> 24) & 3u;
    step = (rate * 4096u + AUD_RATE / 2u) / AUD_RATE;          /* 4.12, rate <= 96 kHz -> fits */
    if (pitch_q12 > 0 && pitch_q12 != 4096) step = (step * (uint32_t)pitch_q12 + 2048u) >> 12;
    if (step > 0xFFFFu) step = 0xFFFFu;
    if (step == 0) step = 1;
    vol_pan(vol, pan, &vr);
#ifndef AUD_NO_HW
    mwr(MIX_R_KEYOFF, 1u << voice);                            /* stop first: no half-new parameters */
    mwr(MIX_V(voice, MIX_V_ADDR), bank_base + e->off);
    mwr(MIX_V(voice, MIX_V_LEN), e->frames);
    mwr(MIX_V(voice, MIX_V_LOOP), (loop && e->loop < e->frames) ? (e->loop | 0x80000000u) : 0u);
    mwr(MIX_V(voice, MIX_V_PITCH), step | (fmt << 16));
    mwr(MIX_V(voice, MIX_V_VOL), vr);
    mwr(MIX_V(voice, MIX_V_START), 0);
    mwr(MIX_R_KEYON, 1u << voice);
#endif
    voice_snd[voice] = (int16_t)snd;
    voice_age[voice] = ++age_now;
}

int aud_play(int snd, int vol, int pan)
{
    int v, i;
    if (!aud.mixer || snd < 0 || snd >= aud.nsnd || snd >= 256) return -1;
    v = snd_voice[snd];
    if (v < AUD_BGM_VOICES || voice_snd[v] != snd) {            /* no voice yet / taken by another sound */
        uint32_t oldest = 0xFFFFFFFFu;
        v = AUD_BGM_VOICES;
        for (i = AUD_BGM_VOICES; i < AUD_VOICES; i++) {
            if (voice_snd[i] < 0) { v = i; break; }
            if (voice_age[i] < oldest) { oldest = voice_age[i]; v = i; }
        }
        snd_voice[snd] = (int16_t)v;
    }
    aud_voice(v, snd, vol, pan, 4096, 0);
    return v;
}

static int bgm_muted;                     /* 控制面板"背景音乐"关（2026-10-26）：所有场景的 BGM 都当 -1 */
void aud_bgm_mute(int on) { bgm_muted = on != 0; if (bgm_muted) aud_bgm(-1, 0, 20); }
int  aud_bgm_muted(void) { return bgm_muted; }
void aud_bgm(int snd, int vol, int fade_ticks)
{
    if (!aud.mixer) return;
    if (bgm_muted) snd = -1;
    if (snd == bgm_snd) { bgm_vol = vol; aud_voice_vol(bgm_v, vol * bgm_scale / 256, 0); return; }
    if (bgm_snd >= 0) {                                        /* old song fades out */
        if (fade_v >= 0) aud_stop(fade_v);
        fade_v = bgm_v;
        fade_vol = bgm_vol * bgm_scale / 256;
        fade_step = fade_ticks > 0 ? (fade_vol + fade_ticks - 1) / fade_ticks : fade_vol;
        if (fade_step <= 0) fade_step = 1;
        bgm_v ^= 1;
    }
    bgm_snd = snd;
    bgm_vol = vol;
    if (snd >= 0) aud_voice(bgm_v, snd, vol * bgm_scale / 256, 0, 4096, 1);
    else aud_stop(bgm_v);
}

void aud_bgm_volume(int scale)
{
    bgm_scale = scale < 0 ? 0 : scale > 256 ? 256 : scale;
    if (bgm_snd >= 0) aud_voice_vol(bgm_v, bgm_vol * bgm_scale / 256, 0);
}

int aud_bgm_current(void) { return bgm_snd; }

void aud_tick(void)
{
    if (fade_v < 0) return;
    fade_vol -= fade_step;
    if (fade_vol <= 0) { aud_stop(fade_v); fade_v = -1; return; }
    aud_voice_vol(fade_v, fade_vol, 0);
}

uint32_t aud_active(void)
{
#ifndef AUD_NO_HW
    if (aud.mixer) return mrd(MIX_R_ACTIVE);
#endif
    return 0;
}

void aud_status_line(char *buf, int n)
{
    if (!aud.hdmi) { mini_snprintf(buf, n, "audio: no HDMI audio in this bitstream"); return; }
#ifndef AUD_NO_HW
    mini_snprintf(buf, n, "audio: %s%s | voices %04x | sent %u underflow %u | mixed %u miss %u fetch %u clip %u"
                  " | bank %d sounds %u KB | vol %d%s",
                  aud.mixer ? "mixer" : "no mixer", aud.tone ? " TONE" : "",
                  (unsigned)aud_active(), (unsigned)hrd(AUD_R_SENT), (unsigned)hrd(AUD_R_UF),
                  aud.mixer ? (unsigned)mrd(MIX_R_SAMPLES) : 0u, aud.mixer ? (unsigned)mrd(MIX_R_MISSES) : 0u,
                  aud.mixer ? (unsigned)mrd(MIX_R_FETCHES) : 0u, aud.mixer ? (unsigned)mrd(MIX_R_CLIPS) : 0u,
                  aud.nsnd, (unsigned)(aud.bank_bytes / 1024u), aud.master, bgm_snd >= 0 ? " bgm" : "");
#endif
}
