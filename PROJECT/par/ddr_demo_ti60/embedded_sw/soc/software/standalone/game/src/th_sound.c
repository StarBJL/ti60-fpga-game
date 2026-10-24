/*
 * th_sound.c - 弹幕场景：事件位 -> 音效，游戏状态 -> BGM（见 th_sound.h）。
 */
#include "th_sound.h"
#include "audio.h"
#include "music.h"

/* 每个事件：库里的名字、音量（0..256）、最短间隔（tick，同一音效不会比这更密地重放） */
static const struct { const char *name; uint16_t vol; uint8_t gap; } se_def[TS_COUNT] = {
    [TS_SHOT]      = { "shot",        70, 4 },
    [TS_FIRE]      = { "enemy_fire",  90, 5 },
    [TS_HIT]       = { "hit",         80, 4 },
    [TS_KILL]      = { "kill",       180, 2 },
    [TS_GRAZE]     = { "graze",      140, 3 },
    [TS_ITEM]      = { "item",       110, 3 },
    [TS_POWERUP]   = { "powerup",    220, 0 },
    [TS_EXTEND]    = { "extend",     256, 0 },
    [TS_BOMBGET]   = { "bomb_get",   220, 0 },
    [TS_BOMB]      = { "bomb",       256, 0 },
    [TS_DEATH]     = { "death",      256, 0 },
    [TS_SPELL]     = { "spell",      230, 0 },
    [TS_CAPTURE]   = { "capture",    230, 0 },
    [TS_PHASE]     = { "phase",      230, 0 },
    [TS_BOSSDEAD]  = { "boss_dead",  256, 0 },
    [TS_WARNING]   = { "warning",    220, 0 },
    [TS_TIMEOUT]   = { "timeout",    200, 0 },
    [TS_SELECT]    = { "select",     180, 0 },
    [TS_OK]        = { "ok",         200, 0 },
    [TS_PAUSE]     = { "pause",      200, 0 },
    [TS_CLEAR]     = { "clear",      220, 0 },
    [TS_OVER]      = { "over",       220, 0 },
    [TS_LASER]     = { "laser",      150, 6 },
};

static int16_t  se_snd[TS_COUNT];
static uint32_t se_next[TS_COUNT];
static uint32_t ticks;
static int bgm_title, bgm_stage[TH_STAGES], bgm_boss[TH_STAGES], bgm_mid2;
static int found;

#define BGM_VOL  200
#define BGM_FADE 40

void th_sound_init(void)
{
    int i;
    found = 0;
    for (i = 0; i < TS_COUNT; i++) {
        se_snd[i] = (int16_t)(se_def[i].name ? aud_find(se_def[i].name) : -1);
        se_next[i] = 0;
        if (se_snd[i] >= 0) ++found;
    }
    bgm_title = aud_find("bgm_title");
    bgm_stage[0] = aud_find("bgm_stage1");
    bgm_boss[0] = aud_find("bgm_boss1");
    bgm_stage[1] = aud_find("bgm_stage2");
    bgm_boss[1] = aud_find("bgm_boss2");
    bgm_mid2 = aud_find("bgm_mid2");
}

int th_sound_found(void) { return found; }

static int want_bgm(const Th *t)
{
    const int st = t->stage < TH_STAGES ? t->stage : 0;
    switch (t->mode) {
    case TH_TITLE: return bgm_title;
    case TH_OVER:  return -1;
    default: break;
    }
    if (t->boss.active && !t->boss.mid && bgm_boss[st] >= 0) return bgm_boss[st];
    if (t->boss.active && t->boss.mid && st == 1 && bgm_mid2 >= 0) return bgm_mid2;
    return bgm_stage[st];
}

void th_sound_tick(Th *t)
{
    uint32_t ev = t->sfx;
    int i;
    t->sfx = 0;
    ++ticks;
    if (aud.mixer && !aud.nsnd && ticks % 60u == 0 && aud_check_bank())
        th_sound_init();                                   /* bank loaded by tools/load_sound.py */
    if (aud.mixer) {
        for (i = 0; ev && i < TS_COUNT; i++, ev >>= 1) {
            if (!(ev & 1u) || se_snd[i] < 0 || (int32_t)(ticks - se_next[i]) < 0) continue;
            aud_play(se_snd[i], se_def[i].vol, 0);
            se_next[i] = ticks + se_def[i].gap;
        }
        aud_bgm(t->mode==TH_OVER?-1:music_track(t->mode==TH_TITLE?MUSIC_MENU:t->boss.active?MUSIC_BOSS:t->diff==TH_STRESS?MUSIC_STRESS:t->stage==1?MUSIC_TH_STAGE2:MUSIC_TH_STAGE), BGM_VOL, BGM_FADE);
        aud_bgm_volume(t->mode == TH_PAUSE ? 64 : 256);      /* quieter while paused */
    }
    aud_tick();
}

void th_sound_idle(void)
{
    if (aud.mixer && aud_bgm_current() >= 0) aud_bgm(-1, 0, BGM_FADE);
    aud_tick();
}
