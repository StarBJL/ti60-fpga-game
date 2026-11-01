#ifndef SPROUT_CONFIG_H
#define SPROUT_CONFIG_H

/* Override with compiler -D options or a board-specific wrapper header. */
#ifndef SCREEN_W
#define SCREEN_W 640
#endif
#ifndef SCREEN_H
#define SCREEN_H 480
#endif
/* SproutRun（萝卜场景）的画面 = 原生 1280x720（2026-10-25）：场地、HUD 按这个大小布局，
 * 角色 / 种子美术大小不变（只是场地变大）。SCREEN_W/H 仍是 640x480 的老画面（弹幕 / 图片 / 测速用）。 */
#ifndef SPROUT_W
#define SPROUT_W 1280
#endif
#ifndef SPROUT_H
#define SPROUT_H 720
#endif
#ifndef MAX_ENEMIES
/* Sixfold campaign population: up to 360 minions plus two endless bosses. */
#define MAX_ENEMIES 384
#endif
/* 玩家子弹：每个玩家 GAME_PLAYER_BULLETS 个槽（多人 2026-10-26：3 x 160，单人只用前 160 个，和以前一样） */
#ifndef GAME_PLAYER_BULLETS
#define GAME_PLAYER_BULLETS 160
#endif
#ifndef MAX_BULLETS
#define MAX_BULLETS (GAME_PLAYER_BULLETS*3)
#endif
#ifndef MAX_HOSTILE_BULLETS
/* 4096 so stress tier 3 has room on the board (no heap growth there);
 * normal mode still only uses the first 1024 slots (see game.c).
 * 2026-10-25: 32768 for the custom stress tier (count set from the UART);
 * tiers 1-3 still cap at 1024 / 2048 / 4096. Game grows to ~1.8 MB (.bss). */
#define MAX_HOSTILE_BULLETS 32768
#endif
#ifndef MAX_GEMS
#define MAX_GEMS 192
#endif
#ifndef MAX_PARTICLES
#define MAX_PARTICLES 128
#endif

#endif
