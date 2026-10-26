#ifndef TH_H
#define TH_H
/*
 * 东方式竖版弹幕（2026-10-11 起，第一版：一关竖切片）。
 *
 *   th_game.c    逻辑：自机（低速 / 判定点 / 擦弹 / 决死 / Bomb）、火力与子机、道具与自动收点、
 *                弹幕池（定点 Q8，每颗 32 B，按"种类 + 颜色"分桶写绘制表）、杂兵、Boss 符卡、计分
 *   th_pattern.c 道中时间表、杂兵行为、Boss 6 个阶段（3 非符 + 3 符卡）的弹幕
 *   th_art.c     开机时用代码画出所有占位图（弹型 × 8 色 × 角度 / 帧、自机、子机、敌人、Boss、道具、
 *                特效、背景、边框）。以后换 PNG 图集只改这里
 *   th_render.c  画：背景卷动、实体、弹幕（每桶一次 gfx_blit_table，发光弹用加法混合）、判定点、
 *                特效、边框与右侧信息栏、Boss 血条 / 符卡名 / 计时
 *
 * 画面：640x480 里左边 384x448 的游戏区（屏幕 x 32..415，y 16..463），右边是信息栏。
 * 坐标：游戏区坐标，Q8 定点（TH_ONE = 1 像素），一个 tick = 1/60 秒；角度 16 位（65536 = 360°，
 * 0 = 向右，16384 = 向下）。
 */
#include <stdint.h>
#include "gfx.h"

#define TH_ONE       256
#define TH_PF_X      32
#define TH_PF_Y      16
#define TH_PF_W      384
#define TH_PF_H      448

#define TH_MAX_BULLETS 99999        /* 压力模式用得到；普通难度一般几百到两千 */
#define TH_MAX_SHOTS   768
#define TH_MAX_ENEMIES 64
#define TH_MAX_ITEMS   1024
#define TH_MAX_FX      1536
/* 压力模式：同样的弹幕，每个环 / 扇的子弹数乘 th_stress_levels[stress_level]（x1 .. x48，x32 峰值约 1 万颗）。
 * 档位可以手动调（'-' '+'），也可以交给自动档（'*'）：每 0.5 s 看一次这段时间里最慢一帧的耗时，
 * 超过预算的 92% 就降一档并立刻收紧弹数上限，连续 3 s 低于 70% 才升一档（见 th_stress_frame）。 */
#define TH_STRESS_LEVELS     14
#define TH_STRESS_LEVEL_INIT 8      /* x16 起步，自动档再往上 / 往下找 */
extern const uint8_t th_stress_levels[TH_STRESS_LEVELS];
#define TH_DRAW_BUFS   3            /* 绘制表轮流写（引擎可能还在读前一两张） */

/* 弹型（每种一个或两个绘制桶，每桶 <= 64 张图） */
enum { TB_PELLET, TB_ORB, TB_RICE, TB_KUNAI, TB_STAR, TB_BIG, TB_BUBBLE, TB_KINDS };
enum { TC_RED, TC_ORANGE, TC_YELLOW, TC_GREEN, TC_CYAN, TC_BLUE, TC_PURPLE, TC_WHITE, TC_COLORS };
/* 绘制桶：小弹 / 中弹 / 米弹 2 / 苦无 2 / 星弹 / 大玉（加法）/ 泡弹（加法） */
enum { TBK_PELLET, TBK_ORB, TBK_RICE0, TBK_RICE1, TBK_KUNAI0, TBK_KUNAI1, TBK_STAR, TBK_BIG, TBK_BUBBLE,
       TBK_COUNT };
#define TH_ROT_STEPS 16             /* 米弹 / 苦无预先转好的角度数 */
#define TH_STAR_FRAMES 8

/* bullet flags */
#define TBF_GRAZED   0x01           /* 已经擦过 */
#define TBF_REDIR    0x02           /* timer 到 0 时改向：aux_spd，aux_ang（TBF_AIM：重新瞄准自机 + aux_ang） */
#define TBF_AIM      0x04
#define TBF_BOUNCE   0x08           /* 碰左右边反弹一次 */
#define TBF_NOCLEAR  0x10           /* 不被 Bomb / 符卡结束消掉（暂未用） */

typedef struct {
    int32_t  x, y;                  /* Q8 */
    int16_t  vx, vy;                /* Q8 / tick */
    uint16_t ang;
    int16_t  spd;                   /* Q8 / tick */
    int16_t  acc;                   /* 速度变化 / tick（Q8），timer > 0 时生效 */
    int16_t  angv;                  /* 角速度 / tick，timer > 0 时生效 */
    int16_t  aux_spd;               /* TBF_REDIR：改向后的速度 */
    uint16_t aux_ang;               /* TBF_REDIR：改向角（TBF_AIM 时为相对瞄准角的偏移） */
    uint8_t  kind, color, flags, timer;
    uint8_t  spin, bk, sp, age;     /* age：出生后的 tick 数（到 TH_FADE_TICKS 停，第 1 关淡入用；之后不再写）；
                                       spin：星弹转动相位；bk / sp：绘制桶和桶内图号（th_fire / 改角度时算好，
                                       热循环里不再按种类分支；星弹再加转动帧） */
} ThBullet;                         /* 32 B */
/* 弹幕热循环用的紧凑副本（2026-10-26）：16 B，一条 64 B 缓存行装 4 颗（ThBullet 只装 2 颗）。
 * 每颗弹每 tick 只读这一份；位置以这里为准（ThBullet 的 x / y 只在发射时写，之后不再更新）。
 * vx / vy / bk / sp 和 ThBullet 一致：只在慢路径（timer、改向、反弹）里改，先改 ThBullet 再抄过来。
 * 新弹在 bullets_update 开头从 ThBullet 抄过来（th_fire 之后图案还会改 timer / acc / flags，所以晚一点抄）。 */
typedef struct {
    int32_t x, y;                   /* Q8 */
    int16_t vx, vy;
    uint8_t bk, sp, spin, slow;     /* slow：TH_S_*，非 0 才去读 ThBullet */
} ThHotBullet;                      /* 16 B */
#define TH_S_TIMER  0x01            /* timer > 0（变速 / 转向 / 到时改向） */
#define TH_S_BOUNCE 0x02            /* TBF_BOUNCE */
#define TH_S_YOUNG  0x04            /* 第 1 关刚出生：淡入表 */
#define TH_S_HALO   0x08            /* 第 1 关：垫光晕（发射时按 spin 和当时的弹数选定） */

typedef struct { int32_t x, y, vx, vy; int16_t dmg; uint8_t kind, active; int16_t target; uint8_t owner,pad; } ThShot;
typedef struct { int32_t x, y, vy; uint8_t kind, active, autoc, pad; } ThItem;
typedef struct { int32_t x, y, vx, vy; uint16_t life, total; uint8_t kind, color, active, pad; } ThFx;
/* 半透明"残影"：被消掉的子弹、被击落的妖精、中弹的自机，用 gfx_blit_alpha 淡出（alpha 特效开着时） */
typedef struct { const Sprite *s; int32_t x, y, vy; uint16_t life, total; } ThGhost;
/* 幻影弹（第 2 关）：透明度随相位周期变化，只有"显形"（alpha >= 150）时才能被打中 / 擦弹；每颗一条 alpha 命令，
 * 所以数量有上限（压力模式也不乘倍率）。bk / sp 同 ThBullet。 */
typedef struct { int32_t x, y; int16_t vx, vy; uint16_t ph, dph; uint8_t bk, sp, grazed, pad; } ThPhantom;
#define TH_MAX_PHANTOMS 384
/* 激光：BEAM 从原点沿 ang 射出（预警虚线 -> 变粗 -> 收细），PILLAR 竖直月光柱（整列），COMET 拖尾流星（只有头部判定）。
 * 压力模式流星也乘倍率（2026-10-26，th_stage2.c comets()）：池子 1024 个，最后 TH_LASER_RESERVE 个只给光束 / 光柱；
 * 倍率多出来的流星 pad = 1（不冒火花，免得占满特效池）。 */
enum { TL_BEAM, TL_PILLAR, TL_COMET };
typedef struct {
    int32_t x, y, vx, vy;           /* 原点（Q8）和移动速度；BEAM 可跟随 Boss（follow） */
    uint16_t ang; int16_t angv;     /* 方向 / 每 tick 转动 */
    int16_t len, wmax, w;           /* 长度 / 最大宽度 / 当前宽度（像素） */
    int16_t t, warn, on;            /* 已过 tick / 预警 tick / 发射 tick（之后 10 tick 收细消失） */
    uint8_t kind, color, follow, pad;
} ThLaser;
#define TH_MAX_LASERS 1024
#define TH_LASER_RESERVE 16
#define TH_COMET_TAIL 8             /* 每颗流星的拖尾段数（GLOW16） */
#define TH_MAX_GHOSTS 512

/* 第 1 关（压力测试也用它）的半透明弹幕（2026-10-26）：
 *   淡入（全局 alpha）：刚射出的子弹前 TH_FADE_TICKS 个 tick 按年龄分 TH_FADE_LV 档写进淡入表，
 *     每档一张表、整表一个透明度（gfx_blit_table_alpha：一条 TAB，分带时走 K 通路 8 像素/拍）；
 *     某档的表满了就照常画（不透明）。
 *   光晕（逐像素 A8）：子弹下面垫一圈柔和的彩色光晕（A8 径向渐变 x 全局 alpha 呼吸；大玉 / 泡弹用大号），
 *     每帧最多 TH_HALO_MAX 个（A8 拷贝走窄通路，每个一条命令，所以限量）；弹数 > 4096 时预算按
 *     256 x 4096 / 弹数 递减，按 spin 固定挑一部分子弹（不闪），16 px 网格去重。
 *   发射光（逐像素 A8）：th_ring / th_fan 出弹时在发射点亮一下（A8 光斑，TH_BLOOM_LIFE tick 淡出）；
 *     弹数 > 8192 时最多 TH_MAX_BLOOMS / 3 个。 */
#define TH_FADE_LV    3
#define TH_FADE_TICKS 9
#define TH_FADE_MAX   1024
#define TH_HALO_MAX   256
#define TH_HALO_SIZES 3
#define TH_MAX_BLOOMS 48
#define TH_BLOOM_LIFE 14
typedef struct { int16_t x, y; uint8_t size, color, alpha, pad; } ThHalo;      /* x,y = 屏幕上的中心 */
typedef struct { int32_t x, y; uint8_t color, size, life, pad; } ThBloom;      /* 游戏区坐标 Q8 */

enum { TI_POWER, TI_POINT, TI_BIGP, TI_BOMB, TI_LIFE, TI_STAR, TI_KINDS };
enum { TF_SPARK, TF_GLOW16, TF_GLOW32, TF_GLOW64, TF_RING, TF_KINDS };

typedef struct { int timer, a, b, c, d; uint16_t ang; int16_t dir; } ThEmitter;

struct Th;
struct ThEnemy;
typedef void (*ThEnemyFn)(struct Th *t, struct ThEnemy *e);
typedef struct ThEnemy {
    int32_t x, y, vx, vy;
    int hp, maxhp, timer, drop_p, drop_pt;
    uint8_t active, kind, color, flash;
    ThEnemyFn fn;
    ThEmitter em;
} ThEnemy;
enum { TE_FAIRY, TE_FAIRY2, TE_BIGFAIRY, TE_WISP, TE_KINDS };   /* TE_WISP：第 2 关的半透明鬼火（A8） */

typedef struct {
    const char *name;               /* 0 = 非符；否则符卡名（大写 ASCII） */
    int hp, time_s;
    void (*fn)(struct Th *t, ThEmitter *em, int tick);
    uint32_t bonus;                 /* 符卡初始奖励 */
} ThPhase;

typedef struct {
    int active, phase, phase_tick, hp, maxhp, entering, dying;
    int mid, leaving;               /* 道中 Boss（打完阶段后飞走、关卡继续）/ 飞走倒计时 */
    int art;                        /* 立绘 / 精灵组：0 花之精（第 1 关），1 月之公主（第 2 关） */
    int32_t x, y, tx, ty;           /* 位置 / 移动目标（Q8） */
    int move_t;
    uint32_t bonus;                 /* 当前符卡奖励（随时间下降） */
    int spell_fail;                 /* 本符卡被弹 / Bomb：收取失败 */
    int captured_show, captured_bonus;   /* "Get Spell Card Bonus" 显示 */
    ThEmitter em;
} ThBoss;

enum { TH_TITLE, TH_PLAY, TH_PAUSE, TH_CLEAR, TH_OVER };
/* 声音事件（2026-10-19）：逻辑里只置 t->sfx 的位，main.c 每 tick 交给 th_sound.c 换成音效再清零 */
enum { TS_SHOT, TS_FIRE, TS_HIT, TS_KILL, TS_GRAZE, TS_ITEM, TS_POWERUP, TS_EXTEND, TS_BOMBGET,
       TS_BOMB, TS_DEATH, TS_SPELL, TS_CAPTURE, TS_PHASE, TS_BOSSDEAD, TS_WARNING, TS_TIMEOUT,
       TS_SELECT, TS_OK, TS_PAUSE, TS_CLEAR, TS_OVER, TS_LASER, TS_COUNT };
#define TS_BIT(e) (1u << (e))
enum { TH_NORMAL, TH_LUNATIC, TH_STRESS, TH_DIFFS };
#define TH_STAGES  2
#define TH_MENU_N  5                /* 标题菜单：NORMAL / LUNATIC / 第 2 关练习 / STRESS 第 1 关 / STRESS 第 2 关
                                       （2026-10-26：压力模式按关分开，打完这一关回标题，不再接着进第 2 关） */

#define TH_PLAYER_FIELDS \
    int32_t px,py; \
    int focus,lives,bombs,power,graze,points_got,invuln,dying,bomb_t,shot_t,respawn_t; \
    uint32_t score,hiscore; \
    int32_t opt_x[4],opt_y[4]; \
    int opt_focus;uint32_t held,pressed;int out;
typedef struct {TH_PLAYER_FIELDS} ThPlayer;
#define TH_PLAYERS 3
typedef struct Th {
    int mode, menu, diff, dens;     /* dens：弹幕数量倍率（压力模式见 th_stress_levels） */
    int stage, start_stage;         /* 当前关（0 起）/ 这一局从哪关开始（重来用） */
    int invincible;                 /* 压力 / 演示模式：自机不死 */
    uint32_t rng, tick, stage_tick;
    union {ThPlayer actor;struct {TH_PLAYER_FIELDS};};
    ThPlayer party[TH_PLAYERS];
    uint32_t player_held[TH_PLAYERS],player_pressed[TH_PLAYERS];
    int player_count,actor_index;
    int bot;
    /* objects */
    ThBullet bullets[TH_MAX_BULLETS];
    int nbullets, peak_bullets, dropped;
    int hot_n;                      /* bullets[0 .. hot_n) 已经抄进 hot[]（见 ThHotBullet） */
    ThShot shots[TH_MAX_SHOTS];
    ThItem items[TH_MAX_ITEMS];     /* 道具、特效都是紧凑数组（前 nitems / nfx 个有效，删除 = 最后一个补位）：
                                       以前每 tick / 每帧把 1024 + 1536 个槽整个扫几遍，全是 DDR 缓存缺失 */
    ThFx fx[TH_MAX_FX];
    int nitems, nfx;
    ThEnemy enemies[TH_MAX_ENEMIES];
    ThBoss boss;
    int boss_seen, cleared_t, clear_bonus;
    int banner_t;                   /* 关卡标题 / 提示文字 */
    const char *banner;
    int shake;                      /* 画面震动（tick） */
    int cutin_t;                    /* 符卡宣言的立绘切入（tick，倒数） */
    ThGhost ghosts[TH_MAX_GHOSTS];
    int nghosts;
    ThPhantom phantoms[TH_MAX_PHANTOMS];
    int nphantoms;
    ThLaser lasers[TH_MAX_LASERS];
    int nlasers;
    int moon_t;                     /* 第 2 关终符：巨大满月升起（tick） */
    ThBloom blooms[TH_MAX_BLOOMS];  /* 发射光（第 1 关） */
    int nblooms;
    int flash;                      /* 白光 0..255 */
    int bg_y;                       /* 背景卷动（像素，Q8） */
    int spell_bg;                   /* 符卡背景淡入 0..255 */
    /* draw tables (written by th_tick, read by th_render / the engine's TAB) */
    int dbuf;
    int dcount[TH_DRAW_BUFS][TBK_COUNT];
    /* 每个桶一张表，容量都是 TH_MAX_BULLETS：弹幕循环直接写进去（不再先写临时表再按桶搬一遍）。
     * 3 x 9 x 99999 x 8 B = 21.6 MB，在独立 DDR .stressbss 中 */
    GfxItem draw[TH_DRAW_BUFS][TBK_COUNT][TH_MAX_BULLETS];
    /* 第 1 关：淡入表（年龄档 x 桶）和光晕表，和 draw[] 同一个 dbuf 轮流写 */
    int fcount[TH_DRAW_BUFS][TH_FADE_LV][TBK_COUNT];
    GfxItem fdraw[TH_DRAW_BUFS][TH_FADE_LV][TBK_COUNT][TH_FADE_MAX];
    int hcount[TH_DRAW_BUFS];
    ThHalo halo[TH_DRAW_BUFS][TH_HALO_MAX];
    ThHotBullet hot[TH_MAX_BULLETS];/* 16 * TH_MAX_BULLETS 字节，在 dbuf 之后：换关不清（hot_n 会清） */
    int32_t bot_fx, bot_fy;
    uint32_t prof[3];               /* 上一 tick（th_clock 时钟数）：出弹 + Boss + 杂兵 / 自机弹 + 弹幕循环 / 道具 + 特效 */         /* 自动驾驶：上一 tick 弹幕循环里顺便算出的近处子弹斥力 */
    /* statistics */
    int drawn, enemies_alive, items_alive, fx_alive, shots_alive;
    uint32_t sfx;                   /* 本 tick 的声音事件位 TS_BIT(TS_*)（在 dbuf 之后：换关的 stage_reset 不清它） */
    /* stress level / 60 fps governor (kept by th_start, reset by th_init) */
    int stress_level, stress_auto;  /* 档位下标、自动档开关 */
    int stress_cap;                 /* 压力模式的弹数上限（自动档超时立刻收紧，有余量再慢慢放开） */
    int gov_n, gov_ok, gov_cool;    /* 自动档：本窗口已过 tick 数、连续"有余量"的窗口数、调档后的冷却 tick 数 */
    uint32_t gov_max, gov_last;     /* 本窗口 / 上个窗口里最慢一帧（us） */
    uint32_t gov_budget;            /* 上个窗口用的预算（us） */
    int gov_downs, gov_ups;         /* 自动档降 / 升档次数 */
    int gov_block[TH_STRESS_LEVELS];/* 某档超时被降下来后，多少 tick 之内不再升回这一档（每失败一次翻倍） */
    uint8_t gov_fail[TH_STRESS_LEVELS];
    uint32_t gov_tick;              /* t->tick at the last call (windows count ticks, not frames) */
    int gov_drop0;                  /* dropped 计数在本窗口开始时的值（上限挡掉的子弹） */
} Th;

/* tables (th_tab.c) */
extern const int16_t  th_sin_tab[1024];
extern const uint16_t th_atan_tab[1025];
static inline int32_t th_sin(uint16_t a) { return th_sin_tab[a >> 6]; }          /* Q14 */
static inline int32_t th_cos(uint16_t a) { return th_sin_tab[((uint16_t)(a + 16384u)) >> 6]; }
uint16_t th_atan2(int32_t dy, int32_t dx);

/* game (th_game.c) */
void th_init(Th *t, uint32_t seed);
void th_start(Th *t, int diff);
/* held = PAD_* bits held this tick, pressed = new presses; bot = 1: autoplay (and demo invincibility) */
void th_tick(Th *t, uint32_t held, uint32_t pressed, int bot);
void th_tick_coop(Th *t,const uint32_t held[TH_PLAYERS],const uint32_t pressed[TH_PLAYERS],int bot);
void th_coop_enable(Th *t,int players);
const ThPlayer *th_player(const Th *t,int player);
extern uint32_t (*th_clock)(void);              /* optional (0 = off): free-running clock for t->prof */
uint32_t th_rand(Th *t);
ThBullet *th_fire(Th *t, int32_t x, int32_t y, uint16_t ang, int spd, int kind, int color);
uint16_t th_aim(const Th *t, int32_t x, int32_t y);
void th_ring(Th *t, int32_t x, int32_t y, int n, uint16_t ang0, int spd, int kind, int color);
void th_fan(Th *t, int32_t x, int32_t y, int n, uint16_t ang, uint16_t spread, int spd, int kind, int color);
ThEnemy *th_spawn_enemy(Th *t, int kind, int color, int32_t x, int32_t y, int hp, ThEnemyFn fn);
void th_fx(Th *t, int kind, int color, int32_t x, int32_t y, int32_t vx, int32_t vy, int life);
void th_explode(Th *t, int32_t x, int32_t y, int color, int big);
void th_drop(Th *t, int kind, int32_t x, int32_t y);
void th_clear_bullets(Th *t, int to_items);
void th_ghost(Th *t, const Sprite *s, int32_t x, int32_t y, int32_t vy, int life);
ThPhantom *th_phantom(Th *t, int32_t x, int32_t y, uint16_t ang, int spd, int kind, int color, uint16_t ph0, uint16_t dph);
ThLaser *th_laser(Th *t, int kind, int32_t x, int32_t y, uint16_t ang, int len, int wmax, int warn, int on, int color);
int th_phantom_alpha(const ThPhantom *p);       /* 40 .. 230 */
int  th_bullet_count(const Th *t);
void th_bullet_sprite(ThBullet *b);             /* recompute b->bk / b->sp after changing kind, colour or angle */
/* stress level: manual step (dir -1 / +1, switches the governor off), governor on / off,
 * and the governor itself: call once per rendered frame with that frame's work time
 * (everything except the vsync wait) and the budget (16667 us for 60 fps). */
void th_stress_step(Th *t, int dir);
void th_stress_set_auto(Th *t, int on);
void th_stress_frame(Th *t, uint32_t work_us, uint32_t budget_us);
int  th_stress_dens(const Th *t);

/* stage / patterns (th_pattern.c) */
/* stages: timeline (called every tick until the boss appears; frozen while a
 * mid-boss is up), boss phases, mid-boss phases (may be 0) */
typedef struct {
    const char *banner, *subtitle;
    void (*timeline)(Th *t);
    const ThPhase *phases;     const int *nphases;
    const ThPhase *mid_phases; const int *nmid;
} ThStageDef;
extern const ThStageDef th_stages[TH_STAGES];
const ThPhase *th_phase_list(const Th *t, int *n);   /* phases of the boss now up (mid-boss or boss) */
void th_start_stage(Th *t, int diff, int stage);
void th_spawn_boss(Th *t, int mid);
/* stage 1 (th_pattern.c) / stage 2 (th_stage2.c) */
void th_stage1_tick(Th *t);
extern const ThPhase th_s1_phases[];
extern const int th_s1_nphases;
void th_stage2_tick(Th *t);
extern const ThPhase th_s2_phases[], th_s2_mid[];
extern const int th_s2_nphases, th_s2_nmid;
void th_boss_move_default(Th *t, int tick);

/* view (2026-10-21, native 720p): the game logic always works on the 640x480 layout
 * (playfield TH_PF_* = 384x448, positions Q8). th_render draws it scaled by s2 / 2:
 *   s2 = 2: 640x480 (as before), ox = oy = 0
 *   s2 = 3: x1.5 = 960x720 centred in the 1280x720 target (ox = 160), all art drawn natively
 *           at 1.5x by th_art_init; the frame picture is the whole 1280x720 screen (decorated
 *           side strips). Band mode writes whole framebuffer rows, so the strips are drawn
 *           every frame like the rest of the frame (th_render draw_frame)
 * TV(n) scales a size, TVX / TVY a 640x480 screen position, TTS a text scale,
 * TH_SX / TH_SY a playfield position (Q8) -> screen pixel. */
typedef struct { int s2, ox, oy, w, h; } ThView;   /* w, h: drawing target (640x480 / 1280x720) */
extern ThView th_view;
#define TV(n)   ((n) * th_view.s2 / 2)
#define TVX(x)  (th_view.ox + TV(x))
#define TVY(y)  (th_view.oy + TV(y))
#define TTS(sc) (((sc) * th_view.s2 + 1) / 2)
#define TH_SX(x) (th_view.ox + (int)((((int32_t)(x) + (TH_PF_X << 8)) * th_view.s2) >> 9))
#define TH_SY(y) (th_view.oy + (int)((((int32_t)(y) + (TH_PF_Y << 8)) * th_view.s2) >> 9))
/* hd = 1: 1280x720 native (x1.5), 0: 640x480. Call th_art_init() afterwards (the art is
 * drawn for the view); the engine must be idle (it may still read the old sprites). */
void th_set_view(int hd);

extern uint8_t th_bk_hv[];                       /* th_bk_half scaled to the view */
extern int th_frame_ox;                          /* x of the view in th_spr_frame (= th_view.ox) */

/* art (th_art.c): call once at boot (CPU draws into static pools), again after th_set_view */
void th_art_init(void);
extern uint32_t th_art_pool_used, th_art_a8_used;   /* pixels used of ART_POOL_PIXELS (512 K) / bytes of the A8 pool (256 KB) */
extern uint32_t th_art2_pool_used, th_art2_a8_used; /* 1.5x copies: pixels of 1280 K / bytes of 640 KB (0 at 640x480) */
extern Sprite th_bspr[TBK_COUNT][64];
extern int    th_bspr_n[TBK_COUNT];
extern uint8_t th_bspr_cx[TBK_COUNT][64], th_bspr_cy[TBK_COUNT][64];   /* bullet centre in the sprite (view px; tight boxes) */
extern const uint8_t th_bk_half[TBK_COUNT];      /* sprite half size (centre -> top-left) */
/* 人物（th_pix.c 的像素图）：自机 0 左倾 / 1 站立 / 2 右倾 / 3 站立第二帧；妖精 [种类][颜色][翅膀帧]；
 * Boss 0 普通 / 1 举手（符卡中） */
extern Sprite th_spr_player[4], th_spr_option, th_spr_hitbox, th_spr_shot_main, th_spr_shot_opt;
extern Sprite th_spr_enemy[TE_KINDS][4][2], th_spr_boss[TH_STAGES][2], th_spr_boss_aura;
/* pixel maps (th_pix.c, generated by tools/th_pixgen.py) */
typedef struct { char c; uint8_t r, g, b; } ThPixPal;
extern const ThPixPal th_pix_pal[];
extern const char th_pix_player[4][32 * 48 + 1], th_pix_fairy[2][32 * 32 + 1];
extern const char th_pix_bigfairy[2][48 * 48 + 1], th_pix_boss[2][64 * 80 + 1];
extern Sprite th_spr_item[TI_KINDS], th_spr_fx[TF_KINDS][TC_COLORS], th_spr_life, th_spr_bombicon;
extern Sprite th_spr_bg, th_spr_bg_spell, th_spr_frame;
extern Sprite th_spr_bomb_ring;
/* 第 1 关半透明弹幕的 A8 图（2026-10-26）：光晕 3 种大小（1x 下 24 / 40 / 72 px）x 8 色，发射光 = 光晕的最大号 */
extern Sprite th_spr_halo[TH_HALO_SIZES][TC_COLORS];
extern const uint8_t th_fade_alpha[TH_FADE_LV];
/* alpha 特效用的图（都带 A8 平面）：符卡立绘（Boss 放大 2 倍，下沿渐隐）、低速时自机周围的魔法阵（8 帧转动）、
 * 符卡时 Boss 身后的大魔法阵（4 帧转动） */
#define TH_FOCUS_FRAMES 8
#define TH_MAGIC_FRAMES 4
extern Sprite th_spr_portrait[TH_STAGES], th_spr_focus[TH_FOCUS_FRAMES], th_spr_magic[TH_MAGIC_FRAMES];
extern const char th_pix_boss2[2][64 * 80 + 1];
/* stage 2 background: pseudo-3D sea of clouds under the moon.
 * Sky (rows 0 .. TH_BG2_HZ-1 of the playfield) is one picture; below the horizon
 * every screen row is ONE copy of a 384-pixel row from a pre-scaled cloud texture:
 * level l is the texture scaled to s_l = 0.25 * 1.2^l texels per pixel (64 rows each),
 * th_bg2_row_level[] / th_bg2_row_z[] give the level and the depth (texture row before
 * scrolling) of each screen row (perspective: depth = 18944 / rows below the horizon). */
#define TH_BG2_HZ     150
#define TH_BG2_LEVELS 20
#define TH_BG2_TEX_H  64
extern Sprite th_bg2_sky, th_bg2_level[TH_BG2_LEVELS], th_spr_moon, th_spr_bigmoon;
extern uint8_t  th_bg2_row_level[TH_PF_H * 3 / 2];   /* rows of the view (TV(TH_PF_H)) */
extern uint16_t th_bg2_row_z[TH_PF_H * 3 / 2];

/* render (th_render.c) */
typedef struct { int fps, hw, stats, alpha; } ThRenderInfo;   /* alpha：半透明特效开（main 的 'e'） */
void th_render(const Th *t, Gfx *g, ThRenderInfo info);

extern Sprite th_stress_bspr[TBK_COUNT][64];
extern uint8_t th_stress_cx[TBK_COUNT][64],th_stress_cy[TBK_COUNT][64],th_stress_half[TBK_COUNT];
int th_pressure_scale_set(int percent);
int th_pressure_scale(void);
void th_rebuild_draw(Th *t);
#endif
