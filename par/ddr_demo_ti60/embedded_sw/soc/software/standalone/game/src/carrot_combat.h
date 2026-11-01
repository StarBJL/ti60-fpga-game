#ifndef CARROT_COMBAT_H
#define CARROT_COMBAT_H
#include <stdint.h>
#include "sprout_config.h"
/* V3.1 normal 1-3 player state. Appended after all legacy Game members: stress pools,
 * hot data, draw tables, GamePlayer and Bullet retain their existing layout. */
#define CARROT_SKILL_SEGMENTS 24
typedef struct { int16_t x0,y0,x1,y1; } CarrotSegment;
#define CARROT_BLAST_SLOTS 12
#define CARROT_BLAST_TICKS 12
#define CARROT_DASH_TICKS 8
#define CARROT_DASH_COOLDOWN 90
#define CARROT_SKILL_TICKS 24
#define CARROT_SKILL_COOLDOWN 480
typedef struct {
    int shot_cycle, dash_ticks, dash_cooldown, dash_dx, dash_dy;
    int skill_ticks, skill_cooldown, skill_x, skill_y, skill_dx, skill_dy;
    int skill_total,skill_age,skill_start,skill_hits,trail_ticks;
    int reflect_window,reflect_left,reflect_cooldown,reflect_flash,reflected;
    int shield_charges,shield_cooldown,shield_quiet,shield_flash,shield_blocked;
    int skill_radius;
    int trail_x[3], trail_y[3], trail_count;
    int skill_segments;
    CarrotSegment path[CARROT_SKILL_SEGMENTS];
} CarrotFighter;
typedef struct { int x,y,owner,ticks; } CarrotBlast;
typedef struct {
    CarrotFighter player[3];
    CarrotBlast blast[CARROT_BLAST_SLOTS];
    uint8_t explosive[MAX_BULLETS];
    unsigned next_blast, blasts_spawned, blast_replacements;
    int clock_tick;
    int boss_id,boss_family,boss_intro_start,boss_intro_ticks;
} CarrotCombat;
#endif
