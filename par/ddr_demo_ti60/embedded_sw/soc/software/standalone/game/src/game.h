#ifndef SPROUT_GAME_H
#define SPROUT_GAME_H

#include <stdint.h>
#include "sprout_config.h"
#include "carrot_combat.h"
#include "story_rules.h"
#include "gfx.h"            /* GfxItem: seed draw table (game_seed_draw) */
#include "carrot_geometry.h"  /* 队友版 1.5 倍角色几何（2026-10-26）：角色 60x60、枪 25x25、激光 29x29 */
#define GAME_SEED_DRAW_BUFS 3     /* 弹幕绘制表的张数（轮流写），见 Game.seed_draw */

/* Portable C99. All positions are Q8 fixed-point; one update is 1/60 second. */
#define FIX 256

enum { KEY_LEFT=1, KEY_RIGHT=2, KEY_UP=4, KEY_DOWN=8,
       KEY_START=16, KEY_PAUSE=32, KEY_RESTART=64,
       KEY_PICK1=128, KEY_PICK2=256, KEY_PICK3=512, KEY_LIMIT=1024,
       KEY_MENU=2048, KEY_PICK4=4096, KEY_DASH=8192, KEY_SKILL=16384 };
typedef struct { uint32_t held, pressed; } Input;
//标题、压力测试、正常游戏、暂停、升级、失败
typedef enum { TITLE, STRESS_SELECT, PLAYING, PAUSED, UPGRADE, DEFEAT, VICTORY } Phase;
//压力测试等级；CUSTOM（2026-10-25）= 弹幕数量上限由串口设定（game_set_stress_bullets）
typedef enum { STRESS_LOW=1, STRESS_MEDIUM=2, STRESS_HIGH=3, STRESS_CUSTOM=4 } StressTier;

/* 原生 1280x720 场地（2026-10-25）：围栏矩形，和 640x480 时的边距一样
 * （左右 14，上 60 = HUD 下面，下边离屏幕底 37 = 底栏上面）。游戏逻辑和渲染都按它算。 */
#define SPROUT_ARENA_X0 14
#define SPROUT_ARENA_Y0 60
#define SPROUT_ARENA_X1 (SPROUT_W-14)
#define SPROUT_ARENA_Y1 (SPROUT_H-37)
/* 声音事件位（Game.sfx，game_sound.c） */
enum { GS_SHOT=1, GS_HIT=2, GS_KILL=4, GS_HURT=8, GS_LEVEL=16, GS_VOICE0=32, GS_VOICE1=64, GS_VOICE2=128, GS_BOSSVOICE=256, GS_REFLECT=512, GS_SHIELD=1024 };
//事件的枚举
typedef enum { EVENT_START, EVENT_HORDE, EVENT_BOSS, EVENT_UPGRADE } Event;
typedef enum { RAPID, POWER, BOOTS, VITALITY, MAGNET, MULTISHOT, ULT_POWER, ULT_AREA, ULT_HASTE, PASSIVE, UPGRADE_COUNT } Upgrade;

/* kind: 0 slime, 1 runner, 2 brute, 3 root king, 4 spitter (ranged). */
typedef struct { int active, x, y, hp, max_hp, kind, spawn, hit, id, attack_timer, volley, family, attack_anim, spawn_until; } Enemy;
typedef struct { int active, x, y, vx, vy, life, damage, style, owner; } Bullet;   /* owner = 发射的玩家 0..2 */
/* 敌方弹（2026-10-26 热 / 冷拆分，和东方弹幕同一个思路）：16 B，一条 64 B 缓存行装 4 颗（32 B 的 Bullet 只装 2 颗；
 * 引擎忙时一次 D$ 缺失约 850 ns，压力模式每 tick 要扫一遍整个池子）。
 * active = 伤害值（0 = 空槽；敌方弹伤害 1..40），vx / vy（速度 <= 1000）和 life（<= 720 tick）都在 int16 范围内。 */
typedef struct { int32_t x, y; int16_t vx, vy; int16_t life; uint8_t active, style; } Seed;
typedef struct { int active, x, y, value; } Gem;
typedef struct { int life, total, x, y, vx, vy, color, size; } Particle;

/*
 * Optional host allocator used only when limit mode exhausts the fixed pool.
 * Bare-metal ports leave these callbacks NULL and count dropped shots instead.
 */
typedef Seed *(*GameGrowShots)(void *user, const Seed *old_pool,
    int old_capacity, int new_capacity);
typedef void (*GameReleaseShots)(void *user, Seed *pool);
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
/*
 * 多人（2026-10-26，移植自队友版，只在普通模式；压力模式永远单人）：
 * 每个玩家一套状态（位置、血量、经验、等级、升级、击杀、双枪……），GamePlayer。
 * Game 里的 x / y / hp ... 是"当前玩家"（actor，和 GamePlayer 同一块内存，原来的单人代码不用改），
 * party[] 存其他玩家；game.c 的 player_context() 把某个玩家换进 actor 再跑原来的代码。
 * 规则：怪物追最近的活着的玩家（接触伤害也给他），敌弹打所有活着的玩家，玩家子弹记在发射者名下
 * （击杀、回血归他），经验宝石被最近的玩家吸走，全部倒下才算失败。
 * 2026-10-28：人数只在标题 / 暂停里选（party_size，1 2 3 键，暂停里选了就重开一局），不会自己变；
 * 多人时经验和等级全队共享（team_xp / team_level / team_next_xp，玩家身上的 xp / level 只是副本），
 * 升级时每个活着的玩家各自从自己的 3 张卡里选一张（pick_slot），都选完再继续；生命、升级效果、击杀各自独立。
 * 选中的卡高亮 GAME_PICK_HOLD 个 tick（单人也是）再回到游戏。
 * 双枪（队友版美术）：朝向 0..31（东 = 0，南 = 8），连发计数（单发时左右枪交替），
 * 本次开火的枪口闪光（bit0 左 bit1 右），距上次开火的 tick（> 30 = 收枪待机姿势）。
 * player_active = 本 tick 结束时这个玩家在飞的子弹数。
 */
#define GAME_PLAYERS 3
#define GAME_PLAYER_FIELDS \
    int x, y, hp, max_hp, invincible; \
    int facing, moving, aim_x, aim_y, fire_timer, fire_period, damage; \
    int speed, shots, pickup_radius, xp, next_xp, level, kills; \
    int hurt_flash, muzzle, upgrades[UPGRADE_COUNT], choices[3]; \
    int weapon_heading, weapon_burst, weapon_flash_mask, weapon_idle_ticks, player_active;
typedef struct { GAME_PLAYER_FIELDS } GamePlayer;
typedef struct {
    union { GamePlayer actor; struct { GAME_PLAYER_FIELDS }; };
    GamePlayer party[GAME_PLAYERS];   /* party[actor_index] 是旧副本，以 actor 为准（用 game_player()） */
    Input player_input[GAME_PLAYERS];
    int player_count, actor_index, upgrade_mask, upgrade_actor;
    int party_size;         /* 下一局的人数 1..3（标题 / 暂停里选，begin_run 用；重开保留） */
    int solo;               /* 1 = 强制单人（自动演示，main.c 每 tick 设） */
    int team_xp, team_level, team_next_xp;   /* 多人：共享经验 / 等级 */
    int pick_slot[GAME_PLAYERS];             /* 这次升级每人选的卡 0..2，-1 = 还没选 */
    int upgrade_hold;       /* > 0：都选完了，高亮选中的卡，倒数到 0 回到游戏 */
    Phase phase;
    uint32_t rng;
    int ticks, visual_ticks;
    int spawn_timer, next_enemy_id, enemy_count, banner;
    int horde_left, horde_timer, horde_ticks, boss_pending, bosses_killed, limit_mode, stress_tier;
    uint32_t sfx;           /* 本 tick 的声音事件 GS_*（game_sound_tick 取走并清 0） */
    Event event;
    Enemy enemies[MAX_ENEMIES];
    Bullet bullets[MAX_BULLETS];
    Seed hostile[MAX_HOSTILE_BULLETS];
    /* Limit-mode extension supplied by the host. NULL means fixed-pool mode. */
    Seed *limit_shots;
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
    /* 几张表轮流写（seed_draw_buf = 最新的那张）：主循环里 game_tick 和引擎画上一帧是重叠的，
     * 引擎可能正在读上一帧的表（gfx_blit_table），这一帧的表写到另一张里。
     * 2026-10-10 两张 -> 三张：分带 + 流水帧时，用某张表的帧要到"下一次循环等上一帧"时才保证画完，
     * 那之前可能已经又过了一个 tick；三张表保证一张表至少隔两个 tick 才被重写。 */
    int seed_draw_buf;
    GfxItem seed_draw[GAME_SEED_DRAW_BUFS][GAME_HOSTILE_INDEX_SLOTS];
    GameConfig config;
    Gem gems[MAX_GEMS];
    Particle particles[MAX_PARTICLES];
    struct { int scene, ticks, boss_spawned, boss_baseline, choice_ticks, timed_out; int endless_wave, endless_boss_wave, ultimate_spawned; int clock_external,clock_valid,clock_was_active; uint64_t clock_previous,clock_active_cycles,choice_cycles; } campaign;
    CarrotCombat combat; /* V3 three-player story only; legacy offsets stay fixed. */
} Game;

#define GAME_SCENES 6
#define GAME_SCENE_TICKS (60*60)
#define GAME_SCENE_MIN_TICKS (40*60)
#define GAME_ENDLESS_WAVE_TICKS (50*60)
void game_campaign_clock(Game *game,uint64_t cycles,uint32_t hz,int visible);
int game_scene_for_level(int level);
int game_start_scene(Game *game,int scene); /* 1..6; preserves selected party */
const char *game_scene_name(int scene);
const char *game_class_upgrade_name(int player,int id);
const char *game_class_upgrade_line(int player,int id,int line);
int game_scene_remaining(const Game *game);
#define GAME_CAMPAIGN_SEED_STYLES 28
int game_seed_styles(const Game *game);
int game_combat_enabled(const Game *game);
void game_build_skill_path(CarrotFighter *fighter,int owner);
int game_skill_hits(const CarrotFighter *fighter,int x,int y,int radius);
int game_combat_period(const GamePlayer *player);
int game_skill_cooldown(const GamePlayer *player,int role);
int game_melee_radius(const GamePlayer *player);
int game_passive_damage(Game *game,int role,int damage);
int game_try_reflect(Game *game,Seed *seed,int index);
const char *game_boss_name(int family);
const char *game_boss_call(int family);
void game_init(Game *game, uint32_t seed);
void game_init_config(Game *game, uint32_t seed, GameConfig config);
void game_shutdown(Game *game);
void game_start(Game *game, uint32_t seed);
void game_start_stress(Game *game, uint32_t seed, int tier);
void game_start_limit(Game *game, uint32_t seed);
/* 自定义压力（STRESS_CUSTOM）的弹幕数量上限，1..MAX_HOSTILE_BULLETS。
 * 记在 game.c 里，重开 / 回标题后保留；正在自定义压力时立即生效（多出来的弹幕直接消失，
 * 新的空位等敌人开火填上）。返回实际值。只改游戏池，不碰引擎正在读的绘制表。 */
int game_set_stress_bullets(Game *game, int count);
int game_stress_bullets(void);
/* 压力模式弹幕的美术大小（像素，1 3 6 9 13 17 25，默认 9 = 原来的 9x9）：只换图，
 * 碰撞半径、轨迹、数量都不变。普通模式一直是 9x9。返回 0 = 不支持的大小。 */
int game_set_seed_size(int pixels);
int game_seed_size(void);
int game_seed_half(const Game *game);     /* 本局弹幕图左上角 = 中心 - half */
int game_seed_extent(const Game *game);   /* 本局弹幕图边长 */
/* 角色外观等级 1..7（队友版 7 种形态）：普通 = 当前等级；压力测试 = HIGH 及以上 7 级，其余 1 级 */
int game_actor_level(const Game *game);
int game_player_level(const Game *game, const GamePlayer *player);
void game_tick(Game *game, Input input);                       /* 单人 = game_tick_coop 只给 P1 输入 */
/* 多人：每个玩家一份输入。暂停 / 菜单 / 重来 / 开始任何人都能按；升级时轮到的玩家（或 P1）选。 */
void game_tick_coop(Game *game, const Input input[GAME_PLAYERS]);
/* 1..3 人（压力模式只能 1）；新加的玩家站在 P1 左右两边，状态抄 P1。重开一局保留人数。 */
void game_coop_enable(Game *game, int players);
/* 下一局的人数（1..3，返回实际值）；压力模式、solo 时仍是单人。标题里按 1 2 3 也是它。 */
int game_set_party(Game *game, int players);
/* 多人升级：玩家 player 选第 slot 张卡（0..2）；不在等他选返回 0 */
int game_choose_upgrade_for(Game *game, int player, int slot);
#ifndef GAME_PICK_HOLD
#define GAME_PICK_HOLD 24       /* 选完后高亮的 tick 数（0.4 s） */
#endif
const GamePlayer *game_player(const Game *game, int player);   /* 0..player_count-1 */
int game_players_alive(const Game *game);
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
const Seed *game_hostile_at(const Game *game, int index);
int game_hostile_count(const Game *game);
/* Loop bound: slots [0, used) may hold seeds, everything above is free. */
int game_hostile_used(const Game *game);
/*
 * Seed draw table: while game_tick updates the enemy seeds it also writes one
 * GfxItem per surviving seed, in slot order (= the order of a pool walk):
 * x,y = top-left of the seed sprite (centre - game_seed_half(), 9x9 = GAME_SEED_HALF 4), sprite =
 * style 0..GAME_SEED_STYLES-1; seeds completely off screen are left out. So
 * the renderer draws the seeds without walking the 32 B Bullet pool again.
 * Returns the item count and sets *items, or -1 if the table is not up to date
 * (pool changed since the last PLAYING tick, e.g. a new run) - walk the pool then.
 */
#define GAME_SEED_HALF 4          /* 压力测试 9x9 时的半宽（大小可换，见 game_seed_half） */
#define GAME_NORMAL_SEED_HALF 12  /* 普通模式敌弹 25x25（队友版 1.5 倍美术） */
#define GAME_SEED_STYLES 4
/* 返回的表在接下来的一次 game_tick 里不会被改（那次写另一张），第二次才会被覆盖。
 * 所以"提交（引擎读表）→ 一次 game_tick → blit_wait"这种重叠顺序是安全的。 */
int game_seed_draw(const Game *game, const GfxItem **items);
/* 表中部分出屏、需要裁剪的项数。为 0 时整张表都完全在屏内，
 * 渲染可以用 gfx_blit_list_inside（不再逐项检查边界）。 */
int game_seed_draw_clip(const Game *game);
int game_dropped_shots(const Game *game);
int game_isqrt(uint32_t n);

#endif
