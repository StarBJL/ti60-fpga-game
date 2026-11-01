/*
 * input_link.h - 多人输入（2026-10-26，移植自队友版 input_runtime.h，按本工程的命令集改写）。
 * 只被 main.c 包含一次（在 panel_link.h 之后），直接用 main.c / panel_link.h 的静态变量和函数。
 *
 * 硬件：rtl/input/input_hub.v，APB 0xF810_0080（HDMI 页里空着的一段）：
 *   0x80 ID 'INP5'   0x84 矩阵键 [15:0]（消抖后，bit = 行 x 4 + 列）
 *   0x88 状态 [0] RX 有数据 [1] FIFO 满 [2] 错误（停止位 / 溢出，粘住）[3] TX 忙 [14:8] FIFO 数
 *   0x8C RX 数据（读一次弹一个）  0x90 TX 写一个字节  0x94 写 1 清 FIFO 和错误
 * 旧位流读 0x80 不是 'INP5'：没有物理输入。
 *
 * 玩家（2026-10-28：人数由标题 / 暂停里的 1 2 3 或上位机 players 决定，不再自动变）：
 *   单人：P1 = 串口 / pc_pad.py / 上位机 + USB 键盘 + 矩阵键盘（合起来）
 *   双人：P1 = 串口 / pc_pad / 上位机，P2 = USB 键盘 + 矩阵键盘（哪个都行）
 *   三人：P1 = 串口 / pc_pad / 上位机，P2 = CH9350 USB 键盘（source 1），P3 = 4x4 矩阵键盘（source 2）
 * 设备上的功能键（暂停、菜单、换场景、音量……）照队友版映射成面板命令。
 */
#include "input_devices.h"

#ifdef INPUT_HUB_MOCK                  /* 主机测试自己实现（带 FIFO 语义） */
uint32_t input_hub_rd(unsigned off);
void     input_hub_wr(unsigned off, uint32_t v);
#else
#define INPUT_HUB_BASE (HDMI_FB_BASE + 0x80u)
static inline uint32_t input_hub_rd(unsigned off) { return *(volatile uint32_t *)(uintptr_t)(INPUT_HUB_BASE + off); }
static inline void     input_hub_wr(unsigned off, uint32_t v) { *(volatile uint32_t *)(uintptr_t)(INPUT_HUB_BASE + off) = v; }
#endif
#define INPUT_HUB_ID 0x494e5035u       /* "INP5" */

static InputDevices devices;
static int devices_ready, devices_present, keyboard_ack_pending;
static int player_pick[GAME_PLAYERS];  /* 设备 / 上位机 pick_for 给某个玩家按的 1/2/3（多人升级时各选各的） */
static uint32_t keyboard_faults, keyboard_acks, keyboard_tx_timeouts;

static void physical_action(void *opaque, InputAction action, int value);
static void input_ready(void)
{
    if (devices_ready) return;
    input_devices_init(&devices, physical_action, 0);
    devices_ready = 1;
    devices_present = input_hub_rd(0x00) == INPUT_HUB_ID;
}
/* 这一局里输入来源 src（0 串口 / 1 USB / 2 矩阵）是第几个玩家 */
static int player_of_source(unsigned src)
{
    const int n = game.player_count;
    if (n <= 1 || src == 0) return 0;
    if (n == 2) return 1;
    return src == 1 ? 1 : 2;
}

static int control_input_command(ControlEvent e)
{
    int ok = 0;
    input_ready();
    if (!devices_present) return 0;
    if (e.type == CT_INPUT_DIAG) ok = input_set_diagnostic(&devices, e.value);
    else if (e.type == CT_MATRIX_CAL) ok = input_matrix_calibration(&devices, e.value);
    else if (e.type == CT_MATRIX_BIND) ok = e.value >= 0 && input_matrix_bind(&devices, (unsigned)e.value);
    else if (e.type == CT_KEYBOARD_SYNC && devices.diagnostic) { keyboard_ack_pending = 1; ok = 1; }
    if (ok && devices.diagnostic) { pad_set_device(1, 0); pad_set_device(2, 0); }   /* 诊断时设备不控制游戏 */
    return ok;
}

static void input_hex(char *out, const uint8_t *bytes, unsigned n)
{
    static const char hex[] = "0123456789abcdef";
    unsigned i;
    for (i = 0; i < n && i < 16; i++) { out[2 * i] = hex[bytes[i] >> 4]; out[2 * i + 1] = hex[bytes[i] & 15u]; }
    out[2 * i] = 0;
}
/* 面板的输入诊断（队友版 I5 / I9 两行，字段名不变） */
static void control_input_state(unsigned id)
{
    static const char hex[] = "0123456789abcdef";
    char map[17], candidate[17], tail[33];
    uint8_t bytes[16];
    unsigned i, n;
    input_ready();
    tx_printf("I5 id=%u supported=%u held=%u matrix_keys=%u matrix_held=%u keyboard_held=%u keyboard_reports=%u malformed=%u rollover=%u ghosts=%u status_reports=%u uart_error=%u\r\n",
              id, (unsigned)devices_present, (unsigned)devices.published_held, (unsigned)devices.matrix_raw,
              (unsigned)devices.matrix_held, (unsigned)devices.keyboard_held, (unsigned)devices.keyboard_reports,
              (unsigned)devices.malformed_frames, (unsigned)devices.rollover_reports, (unsigned)devices.ghost_reports,
              (unsigned)devices.status_reports, devices_present ? (unsigned)((input_hub_rd(0x08) >> 2) & 1u) : 0u);
    n = devices.rx_history_used < 16 ? devices.rx_history_used : 16;
    for (i = 0; i < 16; i++) {
        map[i] = hex[devices.matrix_map[i] & 15u];
        candidate[i] = i < devices.cal_step ? hex[devices.cal_map[i] & 15u] : '-';
    }
    map[16] = candidate[16] = 0;
    for (i = 0; i < n; i++) bytes[i] = devices.rx_history[(devices.rx_history_head + 16u - devices.rx_history_used + i) & 15u];
    input_hex(tail, bytes, n);
    tx_printf("I9 id=%u input_diag_supported=%u input_diag=%u matrix_events=%u matrix_last=%u matrix_seen=%u matrix_logical=%u matrix_cal=%u matrix_step=%u matrix_cal_error=%u matrix_wait=%u matrix_mapped=%u matrix_map=x%s matrix_candidate=x%s keyboard_down=%u keyboard_bytes=%u keyboard_headers=%u keyboard_tail=x%s keyboard_faults=%u keyboard_acks=%u keyboard_tx_timeouts=%u mouse_reports=%u status_reports=%u\r\n",
              id, (unsigned)devices_present, (unsigned)devices.diagnostic, (unsigned)devices.matrix_events, (unsigned)devices.matrix_last,
              (unsigned)devices.matrix_seen, (unsigned)devices.matrix_previous, (unsigned)devices.calibrating, (unsigned)devices.cal_step,
              (unsigned)devices.cal_error, (unsigned)devices.cal_wait, (unsigned)devices.map_applied, map, candidate,
              (unsigned)devices.keyboard_down, (unsigned)devices.keyboard_bytes, (unsigned)devices.keyboard_headers, tail,
              (unsigned)keyboard_faults, (unsigned)keyboard_acks, (unsigned)keyboard_tx_timeouts,
              (unsigned)devices.mouse_reports, (unsigned)devices.status_reports);
}
/* 面板的多人状态（队友版 T7，字段名不变；skill = 大招就绪标志，score = 击杀） */
static void control_party_line(const char *key, unsigned id)
{
    const int count = scene == SCENE_GAME ? game.player_count : 0;
    int shield_wait=game.combat.player[2].shield_cooldown;
    if(!game.combat.player[2].shield_charges && 240-game.combat.player[2].shield_quiet>shield_wait)shield_wait=240-game.combat.player[2].shield_quiet;
    tx_printf("G4 %s=%u campaign_supported=1 campaign_scene=%d campaign_remaining=%d campaign_timeout=%d campaign_complete=%d crystal_type=%d campaign_endless=%d endless_wave=%d ultimate_spawned=%d p2_parry_cd=%d p2_reflected=%d p3_shield=%d p3_shield_cd=%d p3_blocked=%d\r\n",key,id,game.campaign.scene+1,game_scene_remaining(&game),game.campaign.timed_out,game.phase==VICTORY,game.campaign.scene+1,!game.limit_mode&&game.campaign.scene==5,game.campaign.endless_wave,game.campaign.ultimate_spawned,game.combat.player[1].reflect_cooldown,game.combat.player[1].reflected,game.combat.player[2].shield_charges,shield_wait,game.combat.player[2].shield_blocked);
    int life[3] = { 0, 0, 0 }, x[3] = { 0, 0, 0 }, y[3] = { 0, 0, 0 }, i;
    unsigned score[3] = { 0, 0, 0 }, held[3];
    int skill[3]={0,0,0};
    const uint32_t t = now();
    for (i = 0; i < 3; i++) {
        held[i] = pad_source_held((unsigned)i, t, TICKS_PER_MS);
        if (i >= count) continue;
        {
            const GamePlayer *p = game_player(&game, i);
            skill[i]=game_combat_enabled(&game) && p->hp>0 && !game.combat.player[i].skill_cooldown && !game.combat.player[i].skill_ticks;
            life[i] = p->hp; score[i] = (unsigned)p->kills; x[i] = p->x / FIX; y[i] = p->y / FIX;
        }
    }
    tx_printf("T7 %s=%u scene=%d players=%d selected_players=%d team_level=%d team_xp=%d team_next_xp=%d upgrade_wait=%d matrix_auto=0 p1_life=%d p1_skill=%d p1_score=%u p1_x=%d p1_y=%d p1_held=%u p2_life=%d p2_skill=%d p2_score=%u p2_x=%d p2_y=%d p2_held=%u p3_life=%d p3_skill=%d p3_score=%u p3_x=%d p3_y=%d p3_held=%u\r\n",
              key, id, scene, count, game.party_size < 1 ? 1 : game.party_size,
              count > 1 ? game.team_level : game.level, count > 1 ? game.team_xp : game.xp, count > 1 ? game.team_next_xp : game.next_xp,
              game.phase == UPGRADE ? game.upgrade_mask : 0,
              life[0], skill[0], score[0], x[0], y[0], held[0], life[1], skill[1], score[1], x[1], y[1], held[1],
              life[2], skill[2], score[2], x[2], y[2], held[2]);
}

/* Shared by S3, panel Start, USB Enter and legacy serial Enter. */
static int input_confirm_restart(unsigned start)
{
    if (!matrix_restart_confirm || !start) return 0;
    matrix_restart_confirm = 0;
    key_pressed = (key_pressed & ~KEY_START) | KEY_RESTART;
    autoplay = 0; th_bot_last = 0;
    return 1;
}
/* Labels and firmware actions share matrix_keys.def; calibration only remaps raw bits. */
static void matrix_key_help(void)
{
#define KEY(id,held,act,value,fnact,fnvalue,label,fnlabel) \
    tx_puts("S" #id " " label " | Fn: " fnlabel "\r\n");
#include "matrix_keys.def"
#undef KEY
    tx_puts("[V3.1 carrot 1-3P story: automatic fire; Shift/S16 dash; X/J/S8 owner energy pattern]\r\n"
            "[Fn+S12 P2 source / Fn+S15 next track: unsupported in this firmware]\r\n");
}
/* 设备上的功能键 -> 面板命令（同一个 control_apply，所以和上位机按钮行为一样） */
static void physical_action(void *opaque, InputAction action, int value)
{
    ControlEvent e = { CT_NONE, value, 0, 1 };
    (void)opaque;
    switch (action) {
    case INPUT_SOURCE_HELD: {
        const unsigned source = (unsigned)value >> 16;
        const uint32_t mask = (uint32_t)value & 0xFFFFu;
        pad_set_device(source, mask); return;
    }
    case INPUT_CH9350_STATE: keyboard_ack_pending = 1; return;
    case INPUT_HELD:
        if ((value & ~(int)PAD_SKIP) && autoplay) {   /* 只按 Ctrl（跳过）不退出自动演示 */ autoplay = 0; dir_x = dir_y = 0; tx_puts("[autoplay OFF: keyboard / keypad]\r\n"); }
        return;
    case INPUT_SCENE_NEXT: e.type = CT_SCENE; e.value = (scene + 1) % SCENE_COUNT; break;
    case INPUT_START_PAUSE: e.type = CT_START; break; /* S3 never substitutes pause. */
    case INPUT_STRESS_MENU: e.type = CT_STRESS; e.value = scene == SCENE_GAME ? (game.phase == STRESS_SELECT ? 4 : 0) : 1; break;
    case INPUT_COUNT_DELTA: e.type = CT_DELTA; break;
    case INPUT_STEP_NEXT: e.type = CT_STEP; e.value = count_step == 100 ? 10 : count_step == 10 ? 1 : 100; break;
    case INPUT_STEP_SET: e.type = CT_STEP; break;
    case INPUT_SIZE_NEXT:               /* Legacy action; matrix S12 uses INPUT_PLAYERS_NEXT. */
        if (scene == SCENE_GAME && !game.limit_mode && (game.phase == TITLE || game.phase == PAUSED)) {
            panel_players(game.party_size % GAME_PLAYERS + 1);
            control_state(0);
            return;
        }
        handle_char(scene == SCENE_SQUARES ? '}' : ']'); return;
    case INPUT_BULLET_DELTA: handle_char(value < 0 ? '[' : ']'); return;
    case INPUT_SQUARE_DELTA: handle_char(value < 0 ? '{' : '}'); return;
    case INPUT_MENU: e.type = CT_MENU; break;
    case INPUT_PICK:
        if (scene == SCENE_GAME && game.phase == UPGRADE && game.player_count > 1 && devices.source <= 2) {   /* 多人：记给这个设备的玩家 */
            player_pick[player_of_source(devices.source)] = value;
            return;
        }
        e.type = CT_PICK; break;
    case INPUT_RESTART:
        if (scene != SCENE_GAME && scene != SCENE_TH) {
            tx_puts("[restart: only available in a game]\r\n"); return;
        }
        matrix_restart_confirm = 1;
        tx_puts("[restart requested: S3 / Start confirms; S1 / Menu cancels]\r\n");
        control_state(0); return;
    case INPUT_PLAYERS_NEXT:
        if (scene != SCENE_GAME || game.limit_mode ||
            (game.phase != TITLE && game.phase != PAUSED)) {
            tx_puts("[players: select in carrot title / pause, outside stress]\r\n"); return;
        }
        autoplay=0;
        game_set_party(&game, (game.party_size < 1 ? 1 : game.party_size) % GAME_PLAYERS + 1);
        tx_puts("[players selected for next run: S3 starts; S13 resumes current run]\r\n");
        control_state(0); return;
    case INPUT_P2_SOURCE_NEXT:
        tx_puts("[Fn+S12 unsupported: this version combines USB and matrix for P2]\r\n"); return;
    case INPUT_TRACK_NEXT:
        tx_puts("[Fn+S15 unsupported: this version has no manual track selection]\r\n"); return;
    case INPUT_AUTO_TOGGLE: e.type = CT_AUTO; e.value = !autoplay; break;
    case INPUT_ALPHA_TOGGLE: e.type = CT_ALPHA; e.value = !alpha_fx; break;
    case INPUT_VOLUME_DELTA: e.type = CT_VOLUME_DELTA; break;
    case INPUT_STATS_TOGGLE: e.type = CT_STATS; e.value = !show_stats; break;
    case INPUT_BACKEND_TOGGLE: e.type = CT_BACKEND; e.value = !use_hw; break;
    case INPUT_VSYNC_TOGGLE: e.type = CT_VSYNC; e.value = !vsync; break;
    case INPUT_BGM_TOGGLE: e.type = CT_BGM; e.value = aud_bgm_muted(); break;
    case INPUT_STATUS:
        matrix_key_help(); e.type = CT_STATUS; break;
    case INPUT_PAUSE: e.type = CT_PAUSE; break;
    case INPUT_TONE_TOGGLE: e.type = CT_TONE; e.value = !aud.tone; break;
    case INPUT_LEGACY: handle_char((char)value); return;
    default: return;                    /* 换曲目、1080P / 视频恢复：本工程没有 */
    }
    if (control_apply(e)) control_state(0);
    else tx_puts("[keyboard / keypad: not available in this scene]\r\n");
}

static uint32_t input_milliseconds(void)
{
    static uint32_t previous, wraps;
    const uint32_t tick = now();
    if (tick < previous) ++wraps;
    previous = tick;
    return (uint32_t)((((uint64_t)wraps << 32) | tick) * 1000u / BSP_CLINT_HZ);
}

/* 每次 poll_uart 调一次：矩阵快照 + 把 CH9350 的 RX FIFO 读空 + 需要时回 CH9350 状态帧 */
static void poll_input_devices(void)
{
    unsigned limit = 64;
    uint32_t ms;
    input_ready();
    if (!devices_present) return;
    ms = input_milliseconds();
    input_matrix_snapshot(&devices, (uint16_t)input_hub_rd(0x04));
    if (input_hub_rd(0x08) & 4u) {
        ++keyboard_faults;
        input_keyboard_release(&devices);
        input_hub_wr(0x14, 1);
        tx_puts("[keyboard UART error: released and FIFO cleared]\r\n");
    }
    while (limit-- && (input_hub_rd(0x08) & 1u)) input_ch9350_feed(&devices, (uint8_t)input_hub_rd(0x0C), ms);
    input_devices_poll(&devices, ms);
    if (keyboard_ack_pending) {
        /* CH9350 DS v2.7 8.3.9 / 8.3.11：模式 2 的状态帧要回 11 字节（队友版原样）：
         * PID 槽 = 0，LED 灭，USB 状态 0xff，配置 0，协议版本 0x20。一次连续发完（约 0.96 ms） */
        static const uint8_t reply[11] = { 0x57, 0xab, 0x12, 0, 0, 0, 0, 0, 0xff, 0, 0x20 };
        unsigned i;
        const uint32_t start = now();
        keyboard_ack_pending = 0;
        for (i = 0; i < sizeof reply; i++) {
            while (input_hub_rd(0x08) & 8u)
                if ((uint32_t)(now() - start) > 20000u * TICKS_PER_US) {
                    ++keyboard_tx_timeouts;
                    tx_puts("[keyboard TX timeout]\r\n");
                    return;
                }
            input_hub_wr(0x10, reply[i]);
        }
        ++keyboard_acks;
    }
}

/* 多人萝卜：玩家 who 这一 tick 的输入（P1 没开 pc_pad 时用串口 wasd 锁定方向）。
 * 双人时 P2 = USB 和矩阵合起来。每个来源的"新按下"每 tick 只取一次。 */
static int carrot_manual_takeover(uint32_t held) {
    return (held & (PAD_UP|PAD_DOWN|PAD_LEFT|PAD_RIGHT|PAD_SHOT|PAD_START|PAD_BOMB|PAD_SLOW)) != 0;
}

static Input player_game_input(unsigned who, uint32_t t)
{
    uint32_t h = 0, p = 0;
    unsigned src;
    Input in = { 0, 0 };
    for (src = 0; src < 3; src++)
        if (player_of_source(src) == (int)who) {
            h |= pad_source_held(src, t, TICKS_PER_MS);
            p |= pad_source_pressed(src, t, TICKS_PER_MS);
        }
    if (h & PAD_LEFT)  in.held |= KEY_LEFT;
    if (h & PAD_RIGHT) in.held |= KEY_RIGHT;
    if (h & PAD_UP)    in.held |= KEY_UP;
    if (h & PAD_DOWN)  in.held |= KEY_DOWN;
    if (p & PAD_PAUSE) in.pressed |= KEY_PAUSE;
    if (p & PAD_SLOW) in.pressed |= KEY_DASH;
    if (p & PAD_BOMB) in.pressed |= KEY_SKILL;
    if (p & (PAD_START | PAD_SHOT)) in.pressed |= KEY_START;
    if (player_pick[who] >= 1 && player_pick[who] <= 3) in.pressed |= KEY_PICK1 << (player_pick[who] - 1);
    player_pick[who] = 0;
    if (who == 0 && !pad_active(t, TICKS_PER_MS)) {
        if (dir_x < 0) in.held |= KEY_LEFT;
        if (dir_x > 0) in.held |= KEY_RIGHT;
        if (dir_y < 0) in.held |= KEY_UP;
        if (dir_y > 0) in.held |= KEY_DOWN;
    }
    return in;
}

/* 上位机 "party_start"（和旧的 "coop 1" = 3 人）：切到萝卜、关自动演示、按选好的人数新开一局 */
static int party_start(int players)
{
    if (hw_avail) blit_wait();
    scene = SCENE_GAME; view_dirty = 1;
    autoplay = 0; dir_x = dir_y = 0; key_pressed = 0;
    pad_set_state(0, now());
    memset(player_pick, 0, sizeof player_pick);
    if (players) game_set_party(&game, players);
    game.solo = 0;
    game_start(&game, now() | 1u);
    return 1;
}
static int coop_start(int scene_value)
{
    if (scene_value != SCENE_GAME) return 0;
    return party_start(GAME_PLAYERS);
}
/* 上位机 "players N"：下一局人数（在标题 / 暂停里也能用 1 2 3 键）；"pick_for PS"：玩家 P 选第 S 张（11..33） */
static int panel_players(int v)
{
    if (v < 1 || v > GAME_PLAYERS) return 0;
    game_set_party(&game, v);
    if (scene == SCENE_GAME && game.phase == PAUSED && !game.limit_mode) {   /* 暂停里：和按数字键一样重开 */
        if (hw_avail) blit_wait();
        game_start(&game, 0xC0FFEEu);
    }
    return 1;
}
static int panel_pick_for(int v)
{
    const int who = v / 10 - 1, slot = v % 10;
    if (scene != SCENE_GAME || who < 0 || who >= game.player_count || slot < 1 || slot > 3) return 0;
    if (game.player_count <= 1) { key_pressed |= KEY_PICK1 << (slot - 1); return 1; }
    player_pick[who] = slot;
    return 1;
}
