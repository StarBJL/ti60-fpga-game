/* One typed control dispatcher shared by framed host commands and legacy keys.
 * Included after the legacy diagnostic handler so old measurement keys survive. */
static ControlParser control_rx;
static int count_step=100,report_verbose;
static unsigned settings_epoch=1;
static int video_trial;static uint32_t video_trial_start;
static unsigned video_switch_state,video_switch_target,video_link_dvi,video_link_fault,video_recoveries;
static uint32_t video_switch_start,video_switch_frame;
static void video_request(unsigned target,int force,unsigned dvi){
    video_trial=0;video_switch_target=target;video_link_dvi=target?dvi:0;video_link_fault=0;
    if(force){++video_recoveries;test_bars=0;hdmi_test_pattern(0);}
    if(hdmi_link_supported()){
        HDMI_LINK_CTRL=1u|(video_link_dvi<<1);video_switch_state=1;video_switch_start=now();
    }else{HDMI_MODE=target;video_switch_state=0;video_trial=target!=0;video_trial_start=now();}
    stats_reset=1;
}
static void set_game_limit(void);
/* An explicit baseline restores all cross-scene A/B switches. Workload,
 * dimensions, alpha, vsync, scene, and gameplay state remain as selected. */
static int control_fast(void){
    if(!hw_avail||!blit_st.has_list||!blit_st.has_band)return 0;
    blit_wait();set_hw(1);band_want=1;set_seed_mode(2);
    blit_band_set_koff(0);blit_set_cache(1);
    blit_st.has_table=(blit_rd(BLIT_R_CAPS)&BLIT_CAPS_TABLE)!=0;
    overlap=1;
#if SPROUT_STATS
    render_seed_table=1;render_sort_keys=1;
#endif
    set_game_limit();blit_set_wide(BLIT_WIDE_RESET);
    if(!demo_table_enabled())demo_table_toggle();
    if(demo_motion_available()&&!demo_motion_error()&&!demo_motion_enabled())demo_motion_toggle();
#if SPROUT_STATS
    auto_compare=0;seed_prof_req=0;eng_prof_req=0;
#endif
    return 1;
}
static void control_engine_state(unsigned id){
    tx_printf("E5 id=%u accel_supported=1 volume_step_supported=1 submit_mode=%d band=%d band_want=%d cache=%d overlap=%d overlap_active=%d seed_table=%d seed_build=%d sort=%d kpath=%d rd_bursts=%u wr_bursts=%u rd_words=%u wide=%u\r\n",
      id,seed_mode(),blit_st.band,band_want,!!blit_st.cflag,overlap,
      use_hw&&overlap&&scene!=SCENE_SQUARES&&!hdmi_mode_1080(),blit_st.has_table,
      render_seed_table,render_sort_keys,!blit_st.band_koff,blit_st.rd_max,blit_st.wr_max,blit_st.rw_max,blit_st.wide);
}
static int bounded(int v,int lo,int hi){return v<lo?lo:v>hi?hi:v;}
static void control_fence(void){if(hw_avail)blit_wait();}
static void control_move(unsigned held){
    pad_set_state(held,now());
    dir_x=dir_y=0;if(held&&autoplay){autoplay=0;stats_reset=1;++settings_epoch;}
}
static int control_limit(void){return scene==SCENE_SQUARES?sq_count:scene==SCENE_GAME?game_hostile_slots(&game):scene==SCENE_TH?th.stress_cap:3;}
static void control_input_state(unsigned id);
static int control_input_command(ControlEvent e);
static unsigned video_trial_left(void){
    if(!video_trial)return 0;uint32_t elapsed=now()-video_trial_start;
    if(elapsed>=(uint32_t)BSP_CLINT_HZ*10u)return 0;
    return 10000u-(unsigned)((uint64_t)elapsed*1000u/BSP_CLINT_HZ);
}
static void control_video_state(unsigned id){
    unsigned supported=hdmi_dual_supported(),mode=supported?HDMI_MODE:0;
    tx_printf("V5 id=%u supported=%u resolution=%u requested=%u pending=%u output_w=%u output_h=%u nominal_hz=%u render_w=1280 render_h=720 scale=%s pixel_hz=74375000 video_rev=%u bars=%u trial_supported=1 video_trial=%d trial_ms=%u link_supported=%u link_stage=%u link_status=%u link_dvi=%u link_fault=%u recoveries=%u\r\n",
       id,supported,(mode>>1)&1u,mode&1u,(mode>>2)&1u,(mode&2u)?1920u:1280u,(mode&2u)?1080u:720u,
       (mode&2u)?30u:60u,(mode&2u)?"1.5":"1",supported?HDMI_VIDEO_REV:0u,(HDMI_CTRL&HDMI_CTRL_TEST)?1u:0u,video_trial,video_trial_left(),hdmi_link_supported(),video_switch_state,hdmi_link_supported()?HDMI_LINK_STATUS:0u,video_link_dvi,video_link_fault,video_recoveries);
}
static void control_party_line(const char *key,unsigned id){
    int count=scene==SCENE_GAME?game.player_count:scene==SCENE_TH?th.player_count:0;
    int life[3]={0},skill[3]={0},x[3]={0},y[3]={0};unsigned score[3]={0},held[3]={0};uint32_t time=now();
    for(int i=0;i<3;i++){
        held[i]=pad_source_held((unsigned)i,time,BSP_CLINT_HZ/1000u);
        if(i>=count)continue;
        if(scene==SCENE_GAME){const GamePlayer *p=game_player(&game,i);life[i]=p->hp;skill[i]=p->skill_cooldown;score[i]=p->kills;x[i]=p->x/FIX;y[i]=p->y/FIX;}
        else if(scene==SCENE_TH){const ThPlayer *p=th_player(&th,i);life[i]=p->out?-1:p->lives;skill[i]=p->bombs;score[i]=p->score;x[i]=p->px/TH_ONE;y[i]=p->py/TH_ONE;}
    }
    tx_printf("T7 %s=%u scene=%d players=%d matrix_auto=%d p1_life=%d p1_skill=%d p1_score=%u p1_x=%d p1_y=%d p1_held=%u p2_life=%d p2_skill=%d p2_score=%u p2_x=%d p2_y=%d p2_held=%u p3_life=%d p3_skill=%d p3_score=%u p3_x=%d p3_y=%d p3_held=%u\r\n",
       key,id,scene,count,matrix_autofire,life[0],skill[0],score[0],x[0],y[0],held[0],life[1],skill[1],score[1],x[1],y[1],held[1],life[2],skill[2],score[2],x[2],y[2],held[2]);
}
static void control_state(unsigned id){
    tx_printf("S4 id=%u epoch=%u scene=%d phase=%d step=%d count=%d cap=%d bullet=%d square=%d alpha=%d vsync=%d hw=%d auto=%d stats=%d\r\n",
      id,settings_epoch,scene,scene==SCENE_TH?th.mode:scene==SCENE_GAME?game.phase:0,count_step,control_limit(),
      scene==SCENE_SQUARES?DEMO_MAX_SQUARES:scene==SCENE_GAME?MAX_HOSTILE_BULLETS:scene==SCENE_TH?TH_MAX_BULLETS:3,
      scene==SCENE_TH?th_pressure_scale():game_seed_size(),demo_square_size(),alpha_fx,vsync,use_hw,autoplay,show_stats);
    tx_printf("A4 id=%u bgm=%d track=%s volume=%d tone=%d motion=%d motion_min_px=%d table=%d stress_auto=%d density=%d verbose=%d period=%d bank_sounds=%d mixer=%d audio_ctrl=%u pressure_supported=1 music_supported=1 music_auto=%d music_index=%d music_total=10 core_hz=%u coop_supported=1 coop_start_supported=1 video_recover_supported=1\r\n",
      id,music_enabled(),aud_name(aud_bgm_current()),aud.master,aud.tone,demo_motion_enabled(),demo_motion_min_size(),demo_table_enabled(),
      th.stress_auto,th_stress_dens(&th),report_verbose,img_period,aud.nsnd,aud.mixer,HDMI_REG(0x20),music_follow_scene(),music_index(),(unsigned)BSP_CLINT_HZ);
    control_video_state(id);control_engine_state(id);control_input_state(id);control_party_line("id",id);
}
static int control_count(int v){
    if(scene==SCENE_SQUARES){control_fence();sq_count=bounded(v,1,DEMO_MAX_SQUARES);return 1;}
    if(scene==SCENE_GAME&&game.stress_tier==STRESS_CUSTOM&&game.limit_mode){control_fence();game_set_stress_bullets(&game,v);return 1;}
    if(scene==SCENE_TH&&th.diff==TH_STRESS){
        control_fence();th_stress_set_auto(&th,0);th.stress_cap=bounded(v,1,TH_MAX_BULLETS);
        if(th.nbullets>th.stress_cap)th.nbullets=th.stress_cap;th_rebuild_draw(&th);return 1;
    }return 0;
}
static int control_apply(ControlEvent e){
    int v=e.value,changed=1;
    switch(e.type){
    case CT_STATUS:changed=0;break;
    case CT_INPUT_DIAG:case CT_MATRIX_CAL:case CT_MATRIX_BIND:case CT_KEYBOARD_SYNC:
        return control_input_command(e);
    case CT_FAST:if(!control_fast())return 0;break;
    case CT_BARS:if(v<0||v>1)return 0;hdmi_test_pattern(v);break;
    case CT_RESOLUTION:
        if(v<0||v>1||!hdmi_dual_supported())return 0;
        if(!video_switch_state&&(unsigned)v==((HDMI_MODE>>1)&1u)&&!(HDMI_MODE&4u)){changed=0;break;}
        control_fence();video_request((unsigned)v,0,0);break;
    case CT_VIDEO_RECOVER:
        if(!hdmi_dual_supported())return 0;video_request(0,1,0);break;
    case CT_VIDEO_DVI:
        if(v<0||v>1||!hdmi_link_supported())return 0;
        control_fence();video_request(1,0,(unsigned)v);break;
    case CT_COOP_START:
        if(v!=SCENE_TH&&v!=SCENE_GAME)return 0;
        control_fence();scene=v;autoplay=0;th_bot_last=0;dir_x=dir_y=0;key_pressed=th_edge=0;control_move(0);
        memset(player_pick,0,sizeof player_pick);memset(player_slow,0,sizeof player_slow);matrix_autofire=1;
        if(scene==SCENE_TH){th_start(&th,TH_NORMAL);th_coop_enable(&th,3);}
        else{game_start(&game,now()|1u);game_coop_enable(&game,3);}
        view_dirty=1;break;
    case CT_VIDEO_CONFIRM:if(!video_trial||video_switch_state)return 0;video_trial=0;break;
    case CT_SCENE:
        if(v<0||v>=SCENE_COUNT)return 0;control_fence();scene=v;aud_tone(0);
        dir_x=dir_y=0;key_pressed=th_edge=0;control_move(0);view_dirty=1;th_bot_last=0;break;
    case CT_START:autoplay=0;th_bot_last=0;key_pressed|=KEY_START;break;
    case CT_PAUSE:key_pressed|=KEY_PAUSE;changed=0;break;
    case CT_RESTART:control_fence();key_pressed|=KEY_RESTART;autoplay=0;th_bot_last=0;break;
    case CT_MENU:control_fence();key_pressed|=KEY_MENU;autoplay=0;th_bot_last=0;break;
    case CT_STRESS:
        if(scene!=SCENE_GAME&&scene!=SCENE_TH)return 0;
        if(scene==SCENE_GAME&&(v<0||v>4))return 0;
        control_fence();autoplay=0;th_bot_last=0;key_pressed=0;control_move(0);
        if(scene==SCENE_TH){th_start(&th,TH_STRESS);th_stress_set_auto(&th,0);}
        else if(v)game_start_stress(&game,0xc0ffeeu,v);else game.phase=STRESS_SELECT;break;
    case CT_SKILL:
        if(scene==SCENE_GAME)key_pressed|=KEY_SKILL;else if(scene==SCENE_TH)key_pressed|=KEY_PICK1;else return 0;
        autoplay=0;th_bot_last=0;changed=0;break;
    case CT_PICK:if(v<1||v>3)return 0;key_pressed|=KEY_PICK1<<(v-1);changed=0;break;
    case CT_MOVE:if(v<0||v>1023)return 0;control_move((unsigned)v);changed=0;break;
    case CT_STOP:control_move(0);changed=0;break;
    case CT_STEP:if(!control_is_step(v))return 0;count_step=v;break;
    case CT_DELTA:if(v!=-1&&v!=1)return 0;if(!control_count(control_limit()+v*count_step))return 0;break;
    case CT_COUNT:if(!control_count(v))return 0;break;
    case CT_BULLET_SIZE:
        if(scene==SCENE_TH){control_fence();if(!th_pressure_scale_set(v))return 0;th_rebuild_draw(&th);}
        else if(scene==SCENE_GAME){
            if(v!=1&&v!=3&&v!=6&&v!=9&&v!=13&&v!=17&&v!=25)return 0;control_fence();
            if(!render_set_pressure_size(v))return 0;game_set_seed_size(&game,v);
        }else return 0;
        if(hw_avail){blit_fence();blit_cache_inv();}break;
    case CT_SQUARE_SIZE:control_fence();if(!demo_set_square_size(v))return 0;break;
    case CT_PERIOD:if(v<1||v>60)return 0;img_period=v;break;
    case CT_DENSITY:if(scene!=SCENE_TH||(v!=-1&&v!=1))return 0;th_stress_step(&th,v);break;
    case CT_AUTO:if(v<0||v>1)return 0;autoplay=v;dir_x=dir_y=0;control_move(0);break;
    case CT_VSYNC:if(v<0||v>1)return 0;if(v!=vsync)handle_legacy_char('v');break;
    case CT_ALPHA:if(v<0||v>1)return 0;alpha_fx=v;break;
    case CT_BACKEND:if(v<0||v>1||(v&&!hw_avail))return 0;control_fence();set_hw(v);break;
    case CT_MOTION:if(v&&demo_square_size()&&demo_square_size()<demo_motion_min_size())return 0;if(v<0||v>1||(v&&(!demo_motion_available()||demo_motion_error())))return 0;control_fence();if(v!=demo_motion_enabled())demo_motion_toggle();break;
    case CT_TABLE:if(v<0||v>1)return 0;control_fence();if(v!=demo_table_enabled())demo_table_toggle();break;
    case CT_BGM:if(v<0||v>1)return 0;music_enable(v);break;
    case CT_NEXT_TRACK:music_next();break;
    case CT_PREVIOUS_TRACK:music_previous();break;
    case CT_MUSIC_SELECT:if(!music_select(v))return 0;break;
    case CT_VOLUME:if(v<0||v>256)return 0;aud_master(v);break;
    case CT_VOLUME_DELTA:
        if(v!=-1&&v!=1)return 0;
        /* One button step = five displayed percentage points. Relative
         * updates execute against current device state, including rapid taps. */
        aud_master((bounded((aud.master*100+128)/256+5*v,0,100)*256+50)/100);break;
    case CT_TONE:if(v<0||v>1)return 0;aud_tone(v);break;
    case CT_STATS:
#if SPROUT_STATS
        if(v<0||v>1)return 0;show_stats=v;break;
#else
        return 0;
#endif
    case CT_VERBOSE:if(v<0||v>1)return 0;report_verbose=v;break;
    case CT_BENCH:if(scene!=SCENE_SQUARES)return 0;handle_legacy_char('&');break;
    case CT_LEGACY:if(v<32||v>126||v==':')return 0;handle_legacy_char((char)v);break;
    default:return 0;
    }
    if(changed){++settings_epoch;stats_reset=1;}return 1;
}
static void poll_video_trial(void){
    uint32_t elapsed=now()-video_switch_start;
    if(video_switch_state==1 && elapsed>=(uint32_t)BSP_CLINT_HZ*15u/100u){
        HDMI_MODE=video_switch_target;video_switch_frame=hdmi_frame_cnt();video_switch_start=now();video_switch_state=2;
    }else if(video_switch_state==2){
        int stable=!(HDMI_MODE&4u)&&((HDMI_MODE>>1)&1u)==video_switch_target;
        if(stable&&(uint32_t)(hdmi_frame_cnt()-video_switch_frame)>=2u&&elapsed>=(uint32_t)BSP_CLINT_HZ*15u/100u){
            HDMI_LINK_CTRL=video_link_dvi<<1;video_switch_state=0;
            video_trial=video_switch_target!=0;video_trial_start=now();stats_reset=1;
            tx_puts("[video link restarted]\r\n");control_video_state(0);
        }else if(elapsed>=(uint32_t)BSP_CLINT_HZ){
            /* Always release the physical lanes, even if ack/counters fail. */
            HDMI_MODE=0;HDMI_LINK_CTRL=0;video_switch_state=0;video_trial=0;video_link_dvi=0;video_link_fault=1;stats_reset=1;
            tx_puts("[video link restart timeout; requested 720p]\r\n");control_video_state(0);
        }
    }

    if(video_trial&&(uint32_t)(now()-video_trial_start)>=(uint32_t)BSP_CLINT_HZ*10u){
        video_request(0,1,0);++settings_epoch;
        tx_puts("[video trial expired: returning to 720p]\r\n");control_video_state(0);
    }
}
static int cycle_value(const int *items,int n,int current,int direction){
    int i;for(i=0;i<n&&items[i]!=current;i++);if(i==n)i=0;return items[(i+n+direction)%n];
}
static void handle_char(char c){
    ControlEvent e={CT_NONE,0,0,0};int consumed=control_feed(&control_rx,(unsigned char)c,&e);
    if(consumed){
        if(consumed==2){int ok=control_apply(e);tx_printf("ACK id=%u ok=%d code=%s\r\n",e.id,ok,ok?"ok":"invalid_or_unavailable");
            if(ok&&e.type!=CT_MOVE&&e.type!=CT_STOP)control_state(e.id);}
        return;
    }
    /* Keep escape sequences and latched terminal movement compatible. */
    if(esc_state){handle_legacy_char(c);return;}
    switch(c){
    case '\r':case '\n':case ' ':e.type=CT_START;break;
    case 'p':case 'P':e.type=CT_PAUSE;break;
    case 'r':case 'R':e.type=CT_RESTART;break;
    case 'm':case 'M':e.type=CT_MENU;break;
    case 'n':case 'N':e.type=CT_SCENE;e.value=(scene+1)%SCENE_COUNT;break;
    case 'l':case 'L':e.type=CT_STRESS;e.value=0;break;
    case '4':if(scene==SCENE_GAME){e.type=CT_STRESS;e.value=4;}break;
    case 'j':case 'J':if(scene==SCENE_GAME){e.type=CT_SKILL;}break;
    case '[':case ']':{
        const int carrot[]={1,3,6,9,13,17,25},thscale[]={1,3,6,75,100,125,150};e.type=CT_BULLET_SIZE;
        e.value=cycle_value(scene==SCENE_TH?thscale:carrot,7,scene==SCENE_TH?th_pressure_scale():game_seed_size(),c==']'?1:-1);break;}
    case '{':case '}':{const int sizes[]={0,1,3,6,8,12,16,24,32,40};e.type=CT_SQUARE_SIZE;e.value=cycle_value(sizes,10,demo_square_size(),c=='}'?1:-1);break;}
    case '\\':e.type=CT_NEXT_TRACK;break;
    case ';':e.type=CT_BGM;e.value=!music_enabled();break;
    case ',':case '.':case '-':case '+':case '=':
        if(scene==SCENE_SQUARES||(scene==SCENE_GAME&&game.stress_tier==STRESS_CUSTOM)||(scene==SCENE_TH&&th.diff==TH_STRESS)){
            e.type=CT_DELTA;e.value=(c=='.'||c=='+'||c=='=')?1:-1;}break;
    }
    if(e.type!=CT_NONE){int ok=control_apply(e);if(!ok)tx_puts("[control unavailable in this scene]\r\n");else if(e.type!=CT_PAUSE&&e.type!=CT_SKILL)control_state(0);}
    else handle_legacy_char(c);
}
