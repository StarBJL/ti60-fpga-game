#ifndef STORY_RULES_H
#define STORY_RULES_H
#include <stdint.h>
#define STORY_SCENES 6
#define STORY_MAX_LEVEL 9 /* visual tiers; endless gameplay continues beyond level 9 */
#define STORY_DEADLINE_TICKS (60*60)
#define STORY_BOSS_TICKS (24*60)
#define STORY_PICK_TICKS 300
typedef struct {
    int scene, elapsed_ticks, boss_spawned, boss_defeated, timed_out;
    int upgrade_ticks, external_clock, clock_fraction;
    unsigned cleared_mask, crystals_absorbed[STORY_SCENES];
} StoryState;
#endif
