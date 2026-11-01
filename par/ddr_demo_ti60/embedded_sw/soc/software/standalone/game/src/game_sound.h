#ifndef GAME_SOUND_H
#define GAME_SOUND_H
/*
 * 萝卜场景（SproutRun）的声音（2026-10-26，参照队友版 game_sound.c）：
 * game.c 只在 Game.sfx 里置事件位（GS_*，见 game.h），这里每 tick 换成音效，并按游戏阶段选 BGM：
 *   标题 / 压力选择 = bgm_title（和东方共用），游戏中（含暂停、升级、压力测试）= bgm_garden，失败 = 停。
 * 音效和东方共用同一套名字（shot hit kill death powerup pause over），库里没有的不响。
 */
#include "game.h"

void game_sound_init(void);         /* after aud_init(): look the names up in the bank */
void game_sound_tick(Game *g);      /* once per game tick, after game_tick() */

void game_sound_leave(void); /* stop spoken scene audio before switching away */

#endif
