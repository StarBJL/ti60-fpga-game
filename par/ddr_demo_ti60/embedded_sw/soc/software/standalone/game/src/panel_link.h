/*
 * panel_link.h - 队友版 PC 控制面板（tools/control_panel，"Ti60 实验台"）的串口协议（2026-10-26）。
 * 只被 main.c 包含一次（在 poll_uart 之前），直接用 main.c 里的静态变量 / 函数。
 *
 * 面板发 ":<id> <命令> [整数]\n"（control.c 解析），固件回
 *   ACK id=.. ok=0|1 code=..                         每条命令
 *   S4 / A4 / V5 / E5 id=..                          设备状态（命令之后、status）
 *   M4 / F4 / P4 / C4 seq=..                         每个 1 s 统计窗口一组（print_telemetry，main.c）
 * 普通字符（不是 ':' 开头）照旧走 handle_char（串口按键全部保留）。
 *
 * 支持的命令：status scene start pause restart menu stress（东方：值 2 = 第 2 关，其他 = 第 1 关）skill pick move stop step delta count
 *   bullet square period density auto vsync alpha backend motion(只能 0) table bgm volume
 *   volume_delta tone stats verbose key fast bars
 * 多人（2026-10-26，input_link.h）：coop <场景>（只支持萝卜 = 1：新开一局 3 人）、
 *   input_diag / matrix_cal / matrix_bind / keyboard_sync（矩阵键盘校准、CH9350 诊断，需要输入页硬件），
 *   状态行 T7（三名玩家）和 I5 / I9（输入设备），每个统计窗口也发一次。
 * 不支持（回 ok=0，面板上显示不可用）：1080P / 视频链路、东方的多人、
 *   硬件运动、曲目选择（next / prev / music）、bench。东方场景不改数量 / 弹幕大小（只有密度 ± 档）。
 */
#include "control.h"

static ControlParser control_rx;
static unsigned settings_epoch = 1;     /* 设置改了就加 1：面板据此丢掉改设置前的测量窗口 */
static int      report_verbose = 1;     /* 0 = 只发 M4..C4 和一行 fps，不发详细报告 */
static unsigned telemetry_seq;
static int matrix_restart_confirm; /* S14 requests; S3 confirms; S1/scene cancels. */
static int input_confirm_restart(unsigned start);
static void set_game_limit(void);
/* input_link.h（在这个文件之后包含）里的多人 / 输入设备部分 */
static void control_input_state(unsigned id);
static void control_party_line(const char *key, unsigned id);
static int  control_input_command(ControlEvent e);
static int  coop_start(int scene_value);
static int  party_start(int players);
static int  panel_players(int v);
static int  panel_pick_for(int v);
/* 界面语言（2026-10-28）：0 英文，1 中文。边框 / 图片上画好的字要重画，先等引擎画完 */
static void set_language(int v)
{
    if (v == gfx_lang) return;
    if (hw_avail) blit_wait();
    gfx_font_reset(0);
    gfx_set_lang(v);
    th_art_relabel();
    demo_relabel();
    view_dirty = 1;
    tx_puts(v ? "[UI language: Chinese]\r\n" : "[UI language: English]\r\n");
}

static int panel_count(void)            /* 当前在场数量 */
{
    return scene == SCENE_SQUARES ? sq_count : scene == SCENE_GAME ? game_hostile_count(&game) :
           scene == SCENE_TH ? th.nbullets : 0;
}
static int panel_limit(void)            /* 目标 / 上限 */
{
    return scene == SCENE_SQUARES ? sq_count : scene == SCENE_GAME ? game_hostile_slots(&game) :
           scene == SCENE_TH ? th.stress_cap : 0;
}
static int panel_phase(void)
{
    return scene == SCENE_TH ? (int)th.mode : scene == SCENE_GAME ? (int)game.phase : 0;
}
static int panel_kpath(void) { return hw_avail && blit_st.has_band ? blit_band_kmode() != 2 : 0; }

static void control_state(unsigned id)
{
    tx_printf("S4 id=%u epoch=%u scene=%d phase=%d step=%d count=%d cap=%d bullet=%d square=%d alpha=%d vsync=%d hw=%d auto=%d stats=%d\r\n",
              id, settings_epoch, scene, panel_phase(), count_step, panel_limit(),
              scene == SCENE_SQUARES ? DEMO_MAX_SQUARES : scene == SCENE_GAME ? MAX_HOSTILE_BULLETS : scene == SCENE_TH ? TH_MAX_BULLETS : 0,
              scene == SCENE_GAME ? game_seed_size() : 0, demo_square_size(), alpha_fx, vsync, use_hw, autoplay, show_stats);
    tx_printf("A4 id=%u bgm=%d track=%s volume=%d tone=%d motion=0 motion_min_px=0 table=%d stress_auto=%d density=%d verbose=%d period=%d bank_sounds=%d mixer=%d audio_ctrl=%u pressure_supported=1 music_supported=0 core_hz=%u coop_supported=1 coop_start_supported=1 video_recover_supported=0 lang_supported=1 lang=%d party_supported=1 selected_players=%d\r\n",
              id, !aud_bgm_muted(), aud_bgm_current() >= 0 ? aud_name(aud_bgm_current()) : "?", aud.master, aud.tone,
              demo_square_table(), th.stress_auto, th_stress_dens(&th), report_verbose, img_period, aud.nsnd, aud.mixer,
              aud.hdmi ? *(volatile uint32_t *)(uintptr_t)(AUD_HDMI_BASE + AUD_R_CTRL) : 0u, (unsigned)BSP_CLINT_HZ,
              gfx_lang, game.party_size < 1 ? 1 : game.party_size);
    tx_printf("V5 id=%u supported=0 resolution=0 requested=0 pending=0 output_w=%u output_h=%u nominal_hz=60 render_w=%u render_h=%u scale=1 video_rev=0 bars=%d trial_supported=0 video_trial=0 trial_ms=0 link_supported=0 link_stage=0 link_status=0 link_dvi=0 link_fault=0 recoveries=0\r\n",
              id, (unsigned)disp_w, (unsigned)disp_h, (unsigned)disp_w, (unsigned)disp_h, test_bars);
    tx_printf("E5 id=%u accel_supported=1 volume_step_supported=1 submit_mode=%d band=%d band_want=%d cache=%d overlap=%d overlap_active=%d seed_table=%d seed_build=%d sort=%d kpath=%d rd_bursts=%u wr_bursts=%u rd_words=%u wide=%u\r\n",
              id, seed_mode(), blit_st.band, band_want, !!blit_st.cflag, overlap, use_hw && overlap, blit_st.has_table,
              render_seed_table, render_sort_keys, panel_kpath(), blit_st.rd_max, blit_st.wr_max, blit_st.rw_max, blit_st.wide);
    control_input_state(id);
    control_party_line("id", id);
}

/* "恢复加速配置"：硬件绘制、DDR 列表、分带、源缓存、K 通路（含半透明）、弹幕表、逻辑重叠、读字额度、
 * 宽通路、方块填充表。数量 / 尺寸 / 透明 / vsync / 场景不动。 */
static int control_fast(void)
{
    if (!hw_avail || !blit_st.has_list || !blit_st.has_band) return 0;
    blit_wait();
    set_hw(1);
    band_want = 1;
    set_seed_mode(2);
    blit_band_set_kmode(0);
    if (blit_st.has_cache) blit_set_cache(1);
    blit_st.has_table = (blit_rd(BLIT_R_CAPS) & BLIT_CAPS_TABLE) != 0;
    overlap = 1;
#if SPROUT_STATS
    render_seed_table = 1;
    render_sort_keys = 1;
#endif
    set_game_limit();
    if (blit_st.has_wide) blit_set_wide(BLIT_WIDE_RESET);
    if (!demo_square_table()) demo_square_table_toggle();
    return 1;
}

static int control_apply(ControlEvent e)
{
    const int v = e.value;
    int changed = 1;
    if (e.type == CT_MENU || e.type == CT_SCENE || e.type == CT_RESTART ||
        e.type == CT_PARTY_START || e.type == CT_COOP_START || e.type == CT_STRESS)
        matrix_restart_confirm = 0;
    switch (e.type) {
    case CT_STATUS: changed = 0; break;
    case CT_INPUT_DIAG: case CT_MATRIX_CAL: case CT_MATRIX_BIND: case CT_KEYBOARD_SYNC:
        return control_input_command(e);
    case CT_COOP_START: if (!coop_start(v)) return 0; break;
    case CT_PLAYERS: if (!panel_players(v)) return 0; break;
    case CT_LANG: if (v < 0 || v > 1) return 0; set_language(v); break;
    case CT_CAMPAIGN:
        if(v<1||v>6)return 0;
        party_start(0);game_start_scene(&game,v);view_dirty=1;break;
    case CT_PARTY_START: party_start(0); break;
    case CT_PICK_FOR: if (!panel_pick_for(v)) return 0; changed = 0; break;
    case CT_FAST: if (!control_fast()) return 0; break;
    case CT_BARS: if (v < 0 || v > 1) return 0; test_bars = v; hdmi_test_pattern(v); break;
    case CT_SCENE:
        if (v < 0 || v >= SCENE_COUNT) return 0;
        if (hw_avail) blit_wait();
        scene = v; view_dirty = 1; dir_x = dir_y = 0; key_pressed = 0; th_edge = 0;
        pad_set_state(0, now());
        break;
    case CT_START:
        if (input_confirm_restart(1)) {
            if (hw_avail) blit_wait();
        } else if (scene == SCENE_GAME && game.phase == PAUSED && !game.limit_mode) {
            party_start(0); /* Apply selected party only on explicit confirmation. */
        } else key_pressed |= KEY_START;
        autoplay = 0; th_bot_last = 0; changed = 0; break;
    case CT_PAUSE:   key_pressed |= KEY_PAUSE; changed = 0; break;
    case CT_RESTART: if (hw_avail) blit_wait(); key_pressed |= KEY_RESTART; break;
    case CT_MENU:    if (hw_avail) blit_wait(); key_pressed |= KEY_MENU; autoplay = 0; th_bot_last = 0; break;
    case CT_STRESS:
        if (hw_avail) blit_wait();
        if (scene == SCENE_TH) {                       /* 值 2 = 第 2 关，其他 = 第 1 关（两关分开测） */
            th_start_stage(&th, TH_STRESS, v == 2); autoplay = 0; th_bot_last = 0;
        }
        else if (scene == SCENE_GAME) {
            if (v < 0 || v > STRESS_CUSTOM) return 0;
            if (v) game_start_stress(&game, 0xC0FFEEu, v); else game.phase = STRESS_SELECT;
            autoplay = 0;
        } else return 0;
        break;
    case CT_SKILL:
        if (scene == SCENE_TH) key_pressed |= KEY_PICK1;
        else if (scene == SCENE_GAME && game_combat_enabled(&game)) key_pressed |= KEY_SKILL;
        else return 0;
        changed = 0; break;
    case CT_PICK:
        if (v < 1 || v > 3) return 0;
        if(scene==SCENE_GAME && !game.limit_mode && (game.phase==TITLE || game.phase==PAUSED)) autoplay=0;
        key_pressed |= KEY_PICK1 << (v - 1); changed = 0; break;
    case CT_MOVE:
        if (v < 0 || v > 1023) return 0;
        pad_set_state((uint32_t)v, now());
        if (v && autoplay) { autoplay = 0; tx_puts("[autoplay OFF: control panel]\r\n"); }
        changed = 0; break;
    case CT_STOP: pad_set_state(0, now()); dir_x = dir_y = 0; changed = 0; break;
    case CT_STEP: if (!control_is_step(v)) return 0; count_step = v; break;
    case CT_DELTA:
        if (v != -1 && v != 1) return 0;
        if (scene != SCENE_SQUARES && scene != SCENE_GAME) return 0;
        set_count(count_now() + v * count_step); break;
    case CT_COUNT:
        if (scene != SCENE_SQUARES && scene != SCENE_GAME) return 0;
        set_count(v); break;
    case CT_BULLET_SIZE:
        if (scene != SCENE_GAME) return 0;
        if (hw_avail) blit_wait();
        if (!game_set_seed_size(v)) return 0;
        break;
    case CT_SQUARE_SIZE: if (!demo_set_square_size(v)) return 0; break;
    case CT_PERIOD: if (v < 1 || v > 60) return 0; img_period = v; break;
    case CT_DENSITY: if (scene != SCENE_TH || (v != -1 && v != 1)) return 0; th_stress_step(&th, v); break;
    case CT_AUTO: if (v < 0 || v > 1) return 0; autoplay = v; dir_x = dir_y = 0; pad_set_state(0, now()); break;
    case CT_VSYNC: if (v < 0 || v > 1) return 0; vsync = v; break;
    case CT_ALPHA: if (v < 0 || v > 1) return 0; alpha_fx = v; break;
    case CT_BACKEND: if (v < 0 || v > 1 || (v && !hw_avail)) return 0; if (hw_avail) blit_wait(); set_hw(v); break;
    case CT_MOTION: if (v != 0) return 0; break;               /* 没有硬件运动 */
    case CT_TABLE:
        if (v < 0 || v > 1) return 0;
        if (hw_avail) blit_wait();
        if (v != demo_square_table()) demo_square_table_toggle();
        break;
    case CT_BGM: if (v < 0 || v > 1) return 0; aud_bgm_mute(!v); break;
    case CT_VOLUME: if (v < 0 || v > 256) return 0; aud_master(v); break;
    case CT_VOLUME_DELTA: {
        int pct;
        if (v != -1 && v != 1) return 0;
        pct = (aud.master * 100 + 128) / 256 + 5 * v;
        pct = pct < 0 ? 0 : pct > 100 ? 100 : pct;
        aud_master((pct * 256 + 50) / 100);
        break;
    }
    case CT_TONE: if (v < 0 || v > 1) return 0; aud_tone(v); break;
    case CT_STATS:
#if SPROUT_STATS
        if (v < 0 || v > 1) return 0;
        show_stats = v;
        break;
#else
        return 0;
#endif
    case CT_VERBOSE: if (v < 0 || v > 1) return 0; report_verbose = v; break;
    case CT_LEGACY: if (v < 32 || v > 126 || v == ':') return 0; handle_char((char)v); break;
    default: return 0;                     /* 1080P、合作、输入诊断、曲目、bench ... */
    }
    if (changed) { ++settings_epoch; stats_reset = 1; }
    return 1;
}

/* 每个串口字节：':' 开头的一行 = 面板命令；其余照旧当按键 */
static int panel_feed(uint8_t c)
{
    ControlEvent e = { CT_NONE, 0, 0, 0 };
    const int r = control_feed(&control_rx, c, &e);
    if (r == 2) {
        const int ok = control_apply(e);
        tx_printf("ACK id=%u ok=%d code=%s\r\n", e.id, ok, ok ? "ok" : "invalid_or_unavailable");
        if (ok && e.type != CT_MOVE && e.type != CT_STOP) control_state(e.id);
    }
    return r != 0;
}
