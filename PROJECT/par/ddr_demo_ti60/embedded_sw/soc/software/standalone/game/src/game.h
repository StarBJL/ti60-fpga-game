#ifndef SPROUT_GAME_H
#define SPROUT_GAME_H

#include <stdint.h>
#include "sprout_config.h"
#include "carrot_geometry.h"
#include "gfx.h"            /* GfxItem: seed draw table (game_seed_draw) */

/* Portable C99. All positions are Q8 fixed-point; one update is 1/60 second. */
#define FIX 256

enum { KEY_LEFT=1, KEY_RIGHT=2, KEY_UP=4, KEY_DOWN=8,
       KEY_START=16, KEY_PAUSE=32, KEY_RESTART=64,
       KEY_PICK1=128, KEY_PICK2=256, KEY_PICK3=512, KEY_LIMIT=1024,
       KEY_MENU=2048, KEY_PICK4=4096, KEY_SKILL=8192 };
typedef struct { uint32_t held, pressed; } Input;
//标题、压力测试、正常游戏、暂停、升级、失败
typedef enum { TITLE, STRESS_SELECT, PLAYING, PAUSED, UPGRADE, DEFEAT } Phase;
//压力测试等级
typedef enum { STRESS_LOW=1, STRESS_MEDIUM=2, STRESS_HIGH=3, STRESS_CUSTOM=4 } StressTier;
#define GAME_STRESS_BULLET_STEP 1000
#define GAME_OCCLUSION_NEAR_MAX 256
//事件的枚举
typedef enum { EVENT_START, EVENT_HORDE, EVENT_BOSS, EVENT_UPGRADE } Event;
typedef enum { RAPID, POWER, BOOTS, VITALITY, MAGNET, MULTISHOT, NOVA, UPGRADE_COUNT } Upgrade;

/* kind: 0 slime, 1 runner, 2 brute, 3 root king, 4 spitter (ranged). */
typedef struct { int active, x, y, hp, max_hp, kind, spawn, hit, id, attack_timer, volley; } Enemy;
typedef struct { int active, x, y, vx, vy, life, damage, style; } Bullet;
typedef struct { int active, x, y, value; } Gem;
typedef struct { int life, total, x, y, vx, vy, color, size; } Particle;

/*
 * Optional host allocator used only when limit mode exhausts the fixed pool.
 * Bare-metal ports leave these callbacks NULL and count dropped shots instead.
 */
typedef Bullet *(*GameGrowShots)(void *user, const Bullet *old_pool,
    int old_capacity, int new_capacity);
typedef void (*GameReleaseShots)(void *user, Bullet *pool);
/*
 * Free-slot index of the enemy-seed pool: one bit per slot (1 = free) and one
 * summary bit per 32 slots (1 = that word still has a free slot). Taking the
 * lowest free slot is two lookups instead of a scan over the pool, and a full
 * pool is detected at once (all summary words are 0).
 */
#define GAME_HOSTILE_INDEX_SLOTS (MAX_HOSTILE_BULLETS>4096 ? MAX_HOSTILE_BULLETS : 4096)
#define GAME_HOSTILE_WORDS ((GAME_HOSTILE_INDEX_SLOTS+31)/32)
#define GAME_HOSTILE_SUMS  ((GAME_HOSTILE_WORDS+31)/32)
typedef struct {
    GameGrowShots grow_shots;
    GameReleaseShots release_shots;
    void *user;
} GameConfig;
/*
@int ticks, visual_ticks, x, y, hp, max_hp, invincible：
时间、视觉时间，玩家信息，无敌时间
@int facing, moving, aim_x, aim_y, fire_timer, fire_period, damage：
攻击和动作
@int speed, shots, pickup_radius, xp, next_xp, level, kills：玩家状态
@int spawn_timer, next_enemy_id, enemy_count, banner, hurt_flash：敌人和事件ui
@int muzzle, horde_left, horde_timer, horde_ticks, boss_pending, bosses_killed, limit_mode, stress_tier：特殊事件状态
@Bullet bullets[MAX_BULLETS]:玩家弹幕
@Bullet hostile[MAX_HOSTILE_BULLETS]：敌方弹幕
@gems：经验宝石
@Particle：特效
*/
enum { GS_SHOT=1,GS_HIT=2,GS_KILL=4,GS_HURT=8,GS_LEVEL=16,GS_SKILL=32 };
typedef struct {
    uint32_t sfx;
    Phase phase;
    uint32_t rng;
    int ticks, visual_ticks, x, y, hp, max_hp, invincible;
    int facing, moving, aim_x, aim_y, fire_timer, fire_period, damage;
    int speed, shots, pickup_radius, xp, next_xp, level, kills;
    int spawn_timer, next_enemy_id, enemy_count, banner, hurt_flash;
    int muzzle, horde_left, horde_timer, horde_ticks, boss_pending, bosses_killed, limit_mode, stress_tier;
    int stress_bullet_target; /* tier 4: cap and firing load; all shots originate at enemies */
    Event event;
    int upgrades[UPGRADE_COUNT], choices[3];
    Enemy enemies[MAX_ENEMIES];
    Bullet bullets[MAX_BULLETS];
    Bullet hostile[MAX_HOSTILE_BULLETS];
    /* Limit-mode extension supplied by the host. NULL means fixed-pool mode. */
    Bullet *limit_shots;
    int limit_shot_cap, dropped_shots;
    /* Free-slot index of the active enemy-seed pool (maintained by game.c). */
    uint32_t hostile_free[GAME_HOSTILE_WORDS];
    uint32_t hostile_free_sum[GAME_HOSTILE_SUMS];
    int hostile_indexed;    /* slots covered by the index = game_hostile_slots() */
    int hostile_active;     /* seeds in flight */
    int hostile_used;       /* every slot >= this is free: loop bound for update / draw */
    /* Seed draw table, written by game_tick's seed loop (see game_seed_draw). */
    int seed_draw_n, seed_draw_ok;
    int seed_draw_clip;     /* 表中需要裁剪（部分出屏）的项数，见 game_seed_draw_clip */
    /* 三张表轮流写（seed_draw_buf = 最新的那张）：主循环里 game_tick 和引擎画上一帧是重叠的，
     * 引擎可能正在读上一帧的表（gfx_blit_table），这一帧的表写到另一张里 */
    int seed_draw_buf;
    GfxItem seed_draw[3][GAME_HOSTILE_INDEX_SLOTS];
    GameConfig config;
    Gem gems[MAX_GEMS];
    Particle particles[MAX_PARTICLES];
    /* Per-enemy cached steering: no additional state or work per hostile shot. */
    int16_t walk_dx[MAX_ENEMIES], walk_dy[MAX_ENEMIES];
    uint8_t orbit_slot[MAX_ENEMIES];
    int skill_cooldown, skill_flash, skill_casts;
    int player_active; /* counted within the existing player update, no extra pool scan */
    int formation_x,formation_y,formation_count,formation_start,formation_span,formation_lanes;
    int weapon_heading,weapon_burst,weapon_flash_mask,weapon_idle_ticks;
    /* Visual-only indices into the current seed draw table. Collected during
     * its existing generation, never a second full-pool scan in normal play. */
    uint32_t occlusion_near[GAME_OCCLUSION_NEAR_MAX];
    int occlusion_near_n,occlusion_near_overflow;
    int occlusion_x,occlusion_y,occlusion_half;
} Game;

void game_init(Game *game, uint32_t seed);
void game_init_config(Game *game, uint32_t seed, GameConfig config);
void game_shutdown(Game *game);
void game_start(Game *game, uint32_t seed);
void game_start_stress(Game *game, uint32_t seed, int tier);
/* Tier 4 only. Clamp firing cap to 1..available capacity; restart retains it.
 * New bullets are emitted by enemies, never by this setter. DMA tables untouched. */
void game_set_stress_bullets(Game *game, int count);
void game_start_limit(Game *game, uint32_t seed);
void game_tick(Game *game, Input input);
/*
 * Optional timing of the last PLAYING game_tick(), in game_clock() ticks
 * (game_clock = 0: nothing measured). enemies: spawn director + enemy update,
 * pbullets: auto-fire + player bullets vs enemies, hostile: hostile seed pool,
 * rest: gems, particles, level check.
 */
typedef struct { uint32_t enemies, pbullets, hostile, rest; } GameSplit;
#if SPROUT_STATS                 /* 只在调试版（make STATS=1）里有 */
extern uint32_t (*game_clock)(void);
extern GameSplit game_split;
#endif
void game_choose_upgrade(Game *game, int slot);
const char *game_upgrade_name(int id);
const char *game_upgrade_line(int id, int line);
int game_enemy_radius(int kind);
int game_hostile_slots(const Game *game);
const Bullet *game_hostile_at(const Game *game, int index);
int game_hostile_count(const Game *game);
int game_player_count(const Game *game);
int game_total_bullets(const Game *game);
int game_appearance_level(const Game *game);
int game_seed_half(const Game *game);
/* Stress artwork size only: collision radii, trajectories and counts unchanged.
 * Caller must fence pending GPU work before changing any draw-table geometry. */
int game_set_seed_size(Game *game,int pixels);
int game_seed_size(void);
/* Loop bound: slots [0, used) may hold seeds, everything above is free. */
int game_hostile_used(const Game *game);
/*
 * Seed draw table: while game_tick updates the enemy seeds it also writes one
 * GfxItem per surviving seed, in slot order (= the order of a pool walk):
 * x,y = top-left of the seed sprite (centre - game_seed_half(game)), sprite =
 * style 0..GAME_SEED_STYLES-1; seeds completely off screen are left out. So
 * the renderer draws the seeds without walking the 32 B Bullet pool again.
 * Returns the item count and sets *items, or -1 if the table is not up to date
 * (pool changed since the last PLAYING tick, e.g. a new run) - walk the pool then.
 */
#define GAME_PLAYER_MIN_X 51
#define GAME_PLAYER_MAX_X 1229
#define GAME_PLAYER_MIN_Y 129
#define GAME_PLAYER_MAX_Y 633
#define GAME_SEED_HALF 6 /* pressure: 13x13, R2 native size */
#define GAME_NORMAL_SEED_HALF 12 /* normal: 25x25 canvas; R19 shapes scaled for 720p */
#define GAME_SEED_STYLES 4
/* 返回的表在接下来的一次 game_tick 里不会被改（那次写另一张），第三次才会被覆盖。
 * 所以"提交（引擎读表）→ 一次 game_tick → blit_wait"这种重叠顺序是安全的。 */
int game_seed_draw(const Game *game, const GfxItem **items);
/* 表中部分出屏、需要裁剪的项数。为 0 时整张表都完全在屏内，
 * 渲染可以用 gfx_blit_list_inside（不再逐项检查边界）。 */
int game_seed_draw_clip(const Game *game);
int game_dropped_shots(const Game *game);
int game_isqrt(uint32_t n);

#endif
