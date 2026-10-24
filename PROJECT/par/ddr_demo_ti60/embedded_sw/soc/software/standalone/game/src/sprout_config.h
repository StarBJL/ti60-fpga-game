#ifndef SPROUT_CONFIG_H
#define SPROUT_CONFIG_H

/* Override with compiler -D options or a board-specific wrapper header. */
#ifndef SCREEN_W
#define SCREEN_W 640
#endif
#ifndef SCREEN_H
#define SCREEN_H 480
#endif
#ifndef MAX_ENEMIES
#define MAX_ENEMIES 192
#endif
#ifndef MAX_BULLETS
#define MAX_BULLETS 160
#endif
#ifndef MAX_HOSTILE_BULLETS
/* 4096 so stress tier 3 has room on the board (no heap growth there);
 * normal mode still only uses the first 1024 slots (see game.c). */
#define MAX_HOSTILE_BULLETS 40000
#endif
#ifndef MAX_GEMS
#define MAX_GEMS 192
#endif
#ifndef MAX_PARTICLES
#define MAX_PARTICLES 128
#endif

#endif
