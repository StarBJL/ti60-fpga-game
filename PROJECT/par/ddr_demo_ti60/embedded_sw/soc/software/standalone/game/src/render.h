#ifndef SPROUT_RENDER_H
#define SPROUT_RENDER_H
#include "game.h"
#include "gfx.h"

/* Immutable cached level 1..7 artwork, clamped above 7. */
const Sprite *render_hero_asset(int level,int pose);
const Sprite *render_gun_asset(int level,int heading);
/* Locator is visual state only. Coverage counts unique opaque actor pixels,
 * including both live weapon poses; projectile colour-key holes don't count. */
typedef struct {
    int covered,total,percent,active,fade,colored,near_items,used_near_cache;
} RenderLocatorInfo;
RenderLocatorInfo render_locator_info(void);
void render_locator_reset(void);
const Sprite *render_locator_outline_asset(void);
const Sprite *render_projectile_asset(int pressure,int style);
const Sprite *render_laser_asset(int style);

/* alpha_fx: 1 = translucent effects (shadows, dim, panels, ghost), 0 = original opaque look */
typedef struct {
    int fps, ms, logic_us, render_us, work_us, show_stats, mouse_x, mouse_y, alpha_fx;
    int render_fps10,present_fps10,present_valid,logic_hz10,quantity_step;
    /* frame time split, us per frame (logic: per tick) - see main.c */
    int logic_max_us;       /* worst game update in the window */
    int submit_us;          /* drawing (CPU) / building + queueing commands (HW), without qstall */
    int qstall_us;          /* HW: waiting for a free command-queue slot */
    int tail_us;            /* HW: last command queued -> engine idle */
    int vsync_us;           /* waiting until the previous flip is latched */
    /* BitBlt comparison (SoC build; all zero = CPU only) */
    int hw;                 /* this frame is drawn by the BitBlt engine */
    int hw_avail;           /* engine found at boot */
    int fps_mode[2];        /* last measured FPS per backend: [0] CPU, [1] HW (0 = not yet) */
    int draw_mode[2];       /* last measured draw time per backend, us */
    int blt;                /* HW: engine busy %, 0..100 */
    int q_end, q_peak, q_depth; /* HW: queue entries after submit (avg) / max seen / size */
    int rd_mbs, wr_mbs;     /* HW: engine AXI read / write throughput, MB/s */
    /* submit split, us per frame: seeds + hud + scene = submit_us + qstall_us */
    int seeds_us, hud_us, scene_us;
    int batch;              /* 0 = one gfx_blit per hostile seed, else gfx_blit_list (one call per 128);
                               HUD label: 1 LIST (APB), 2 HWLIST (DDR command list), 3 HWWORD (LIST_WORD) */
} RenderInfo;

/*
 * Optional timing of the last render_game() call, in render_clock() ticks.
 * render_clock = 0 (host build / default): nothing is measured, all zero.
 *   seeds : hostile seed loop (the "bullet hell"), seeds_n = seeds drawn
 *   hud   : top bar, menu / pause / upgrade panels and the stats overlay
 *   total : the whole render_game call; scene = total - seeds - hud
 *   scene parts (draw_world, game scene only):
 *   bg : background + gems, sort : enemy draw order, enemies : enemies + player,
 *   pbullets : player bullets, particles : particles + hurt border
 */
typedef struct { uint32_t total, seeds, hud, seeds_n, bg, sort, enemies, pbullets, particles; } RenderSplit;
/* 下面的计时、A/B 开关和 'y' 剖析只在调试版（make STATS=1 → SPROUT_STATS=1）里有；
 * 发行版固定 render_sort_keys = render_seed_table = 1，统计面板（'f'）也不编译。 */
#if SPROUT_STATS
extern uint32_t (*render_clock)(void);
extern RenderSplit render_split;
/* enemy draw order: 1 = y keys copied once + sort from last frame's order
 * (default), 0 = original sort reading the enemy structs (UART key 'j');
 * both give the same order */
extern int render_sort_keys;
/* seeds (batched modes): 1 = draw the table game_tick wrote (game_seed_draw,
 * default), 0 = walk the Bullet pool again (UART key 'z'); same picture */
extern int render_seed_table;
#else
#define render_sort_keys  1
#define render_seed_table 1
#endif

/*
 * Seed submit anatomy (diagnostic, UART key 'y'): the hostile seed loop of
 * render_game cut into parts that the caller times on the current game state.
 * Call PREP first (untimed); every other part returns the items it handled.
 *   PREP   : first 128 on-screen seeds -> private list, returns pool slots scanned
 *   WALK   : pool walk + on-screen test only, no stores
 *   BUILD  : walk + fill the 128-item draw list, nothing submitted
 *   SUBMIT : gfx_blit_list of the prebuilt list, same total count, no pool reads
 *   TINY   : SUBMIT with 1x1 sprites (engine nearly free -> pure CPU submit cost)
 *   FULL   : the real seed loop (walk + build + submit)
 *   BG     : queue one full-screen background copy (keeps the engine busy)
 */
#if SPROUT_STATS
enum { SEED_PART_WALK, SEED_PART_BUILD, SEED_PART_SUBMIT, SEED_PART_TINY, SEED_PART_FULL,
       SEED_PART_PREP, SEED_PART_BG };
int render_seed_part(Gfx *gfx, const Game *game, int part);
#endif

void render_init(void);
/* Select precomputed stress sprites after a GPU fence. */
int render_set_pressure_size(int pixels);
void render_game(const Game *game, Gfx *gfx, RenderInfo info);
int render_upgrade_hit(int x,int y);
int render_title_hit(int x,int y);
int render_stress_hit(int x,int y);

#endif
