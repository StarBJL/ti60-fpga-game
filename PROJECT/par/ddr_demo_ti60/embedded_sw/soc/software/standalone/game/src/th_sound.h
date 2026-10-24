#ifndef TH_SOUND_H
#define TH_SOUND_H
/*
 * 弹幕场景的声音（2026-10-19）：th_game.c 只在 t->sfx 里置事件位（TS_*，见 th.h），
 * 这里每 tick 把事件位换成音效、按游戏状态选 BGM。音效 / BGM 按名字在声音库里找
 * （sound/sounds.txt 里的名字），库里没有的就不响。
 *
 * 音效名：shot enemy_fire hit kill graze item powerup extend bomb_get bomb death spell
 *        capture phase boss_dead warning timeout select ok pause clear over laser
 * BGM 名：bgm_title bgm_stage1 bgm_boss1 bgm_stage2 bgm_boss2（bgm_mid2 = 第 2 关道中 Boss，没有就用 bgm_stage2）
 */
#include "th.h"

void th_sound_init(void);           /* after aud_init(): look the names up in the bank */
void th_sound_tick(Th *t);          /* once per game tick, after th_tick() */
void th_sound_idle(void);           /* once per tick in the other scenes: music off, fades */
int  th_sound_found(void);          /* events with a sound in the bank */

#endif
