/*
 * game_sound.c - 萝卜场景：事件位 -> 音效，游戏阶段 -> BGM（见 game_sound.h）。
 * 音量 / 最短间隔照队友版（R4）。
 */
#include "game_sound.h"
#include "audio.h"
#include "voice_timing.h"

/* 事件位顺序 = GS_SHOT GS_HIT GS_KILL GS_HURT GS_LEVEL（game.h） */
#define GS_N 5
static const struct { const char *name; uint16_t vol; uint8_t gap; } gs_def[GS_N] = {
    { "shot",     65,  4 },
    { "hit",     100,  4 },
    { "kill",    170,  2 },
    { "death",   200, 20 },            /* 被打中 */
    { "powerup", 210, 10 },            /* 升级 */
};
static int16_t  gs_snd[GS_N];
static uint32_t gs_next[GS_N];
static uint32_t ticks;
static int bgm_title, bgm_garden, snd_pause, snd_over, snd_up;
static int last_phase = -1;
static int boss_snd[7];
static int spoken[3],spoken_slot=-1,spoken_owner=-1,spoken_start,voice_round;

#define BGM_VOL  190
#define BGM_FADE 40

void game_sound_init(void)
{
    int i;
    for (i = 0; i < GS_N; i++) { gs_snd[i] = (int16_t)aud_find(gs_def[i].name); gs_next[i] = 0; }
    bgm_title  = aud_find("bgm_title");
    bgm_garden = aud_find("bgm_garden");
    if (bgm_garden < 0) bgm_garden = aud_find("bgm_stage1");   /* 旧声音库：没有萝卜曲时借东方第 1 关 */
    snd_pause = aud_find("pause");
    snd_over  = aud_find("over");
    snd_up    = aud_find("powerup");
    spoken[0]=aud_find("ult_shooter");spoken[1]=aud_find("ult_warrior");spoken[2]=aud_find("ult_mage");
    static const char *names[7]={"boss_moss","boss_jade","boss_thunder","boss_ember","boss_astral","boss_dragon","boss_eclipse"};
    for(i=0;i<7;i++)boss_snd[i]=aud_find(names[i]);
    spoken_slot=spoken_owner=-1;
}

void game_sound_tick(Game *g)
{
    uint32_t ev = g->sfx;
    int i, bgm;
    g->sfx = 0;
    ++ticks;
    if (aud.mixer && !aud.nsnd && ticks % 60u == 0 && aud_check_bank())
        game_sound_init();                                 /* bank loaded by tools/load_sound.py */
    if (aud.mixer) {
        for (i = 0; i < GS_N; i++) {
            if (!(ev & (1u << i)) || gs_snd[i] < 0 || (int32_t)(ticks - gs_next[i]) < 0) continue;
            aud_play(gs_snd[i], gs_def[i].vol, 0);
            gs_next[i] = ticks + gs_def[i].gap;
        }
        /* A single spoken call at a time. Simultaneous co-op calls rotate the
         * speaking role; all three animations/subtitles still run together.
         * Never queue a late line after its attack has already finished. */
        if(spoken_slot>=0){
            int stop=g->phase!=PLAYING || g->limit_mode || !(aud_active()&(1u<<spoken_slot));
            if(spoken_owner>=0)stop|=!g->combat.player[spoken_owner].skill_total || g->combat.player[spoken_owner].skill_start!=spoken_start || g->combat.player[spoken_owner].skill_age>=voice_ticks[spoken_owner];
            else stop|=g->combat.boss_id!=spoken_start || g->campaign.ticks-g->combat.boss_intro_start>=g->combat.boss_intro_ticks;
            if(stop){aud_stop(spoken_slot);spoken_slot=-1;}
        }
        if((ev&GS_BOSSVOICE) && g->phase==PLAYING && !g->limit_mode){
            if(spoken_slot>=0)aud_stop(spoken_slot);
            int family=g->combat.boss_family;
            spoken_slot=family>=0&&family<7?aud_play(boss_snd[family],245,0):-1;
            spoken_owner=-2;spoken_start=g->combat.boss_id;
        }
        if(spoken_slot<0 && g->phase==PLAYING && !g->limit_mode)for(int k=0;k<3;k++){
            int p=(voice_round+k)%3;
            if((ev&(GS_VOICE0<<p)) && spoken[p]>=0){
                spoken_slot=aud_play(spoken[p],235,(p-1)*42);
                if(spoken_slot>=0){spoken_owner=p;spoken_start=g->combat.player[p].skill_start;voice_round=(p+1)%3;}
                break;
            }
        }
        if(ev&GS_REFLECT)aud_play(aud_find("laser"),125,0);
        if(ev&GS_SHIELD)aud_play(aud_find("spell"),110,48);
        bgm = (g->phase == TITLE || g->phase == STRESS_SELECT) ? bgm_title :
              g->phase == DEFEAT ? -1 : bgm_garden;
        aud_bgm(bgm, BGM_VOL, BGM_FADE);
        aud_bgm_volume(g->phase == PAUSED ? 64 : spoken_slot>=0 ? 85 : 256);
        if (last_phase != (int)g->phase) {
            if (g->phase == DEFEAT && snd_over >= 0)       aud_play(snd_over, 220, 0);
            else if (g->phase == PAUSED && snd_pause >= 0) aud_play(snd_pause, 180, 0);
            else if (g->phase == UPGRADE && snd_up >= 0)   aud_play(snd_up, 210, 0);
        }
    }
    last_phase = (int)g->phase;
    aud_tick();
}

void game_sound_leave(void){if(spoken_slot>=0)aud_stop(spoken_slot);spoken_slot=-1;spoken_owner=-1;}
