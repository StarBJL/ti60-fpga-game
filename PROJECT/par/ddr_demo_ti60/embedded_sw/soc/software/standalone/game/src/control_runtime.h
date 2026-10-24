/* One typed control dispatcher shared by framed host commands and legacy keys.
 * Included after the legacy diagnostic handler so old measurement keys survive. */
static ControlParser control_rx;
static int count_step=100,report_verbose;
static unsigned settings_epoch=1;
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
static void control_video_state(unsigned id){
    unsigned supported=hdmi_dual_supported(),mode=supported?HDMI_MODE:0;
    tx_printf("V5 id=%u supported=%u resolution=%u requested=%u pending=%u output_w=%u output_h=%u nominal_hz=%u render_w=1280 render_h=720 scale=%s pixel_hz=74375000 video_rev=%u bars=%u\r\n",
       id,supported,(mode>>1)&1u,mode&1u,(mode>>2)&1u,(mode&2u)?1920u:1280u,(mode&2u)?1080u:720u,
       (mode&2u)?30u:60u,(mode&2u)?"1.5":"1",supported?HDMI_VIDEO_REV:0u,(HDMI_CTRL&HDMI_CTRL_TEST)?1u:0u);
}
static void control_state(unsigned id){
    tx_printf("S4 id=%u epoch=%u scene=%d phase=%d step=%d count=%d cap=%d bullet=%d square=%d alpha=%d vsync=%d hw=%d auto=%d stats=%d\r\n",
      id,settings_epoch,scene,scene==SCENE_TH?th.mode:scene==SCENE_GAME?game.phase:0,count_step,control_limit(),
      scene==SCENE_SQUARES?DEMO_MAX_SQUARES:scene==SCENE_GAME?MAX_HOSTILE_BULLETS:scene==SCENE_TH?TH_MAX_BULLETS:3,
      scene==SCENE_TH?th_pressure_scale():game_seed_size(),demo_square_size(),alpha_fx,vsync,use_hw,autoplay,show_stats);
    tx_printf("A4 id=%u bgm=%d track=%s volume=%d tone=%d motion=%d table=%d stress_auto=%d density=%d verbose=%d period=%d bank_sounds=%d mixer=%d audio_ctrl=%u\r\n",
      id,music_enabled(),aud_name(aud_bgm_current()),aud.master,aud.tone,demo_motion_enabled(),demo_table_enabled(),
      th.stress_auto,th_stress_dens(&th),report_verbose,img_period,aud.nsnd,aud.mixer,HDMI_REG(0x20));
    control_video_state(id);control_engine_state(id);control_input_state(id);
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
    case CT_FAST:if(!control_fast())return 0;break;
    case CT_BARS:if(v<0||v>1)return 0;hdmi_test_pattern(v);break;
    case CT_RESOLUTION:
        if(v<0||v>1||!hdmi_dual_supported())return 0;
        control_fence();HDMI_MODE=(unsigned)v;break;
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
            if(v!=9&&v!=13&&v!=17&&v!=25)return 0;control_fence();
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
    case CT_MOTION:if(v<0||v>1||(v&&(!demo_motion_available()||demo_motion_error())))return 0;control_fence();if(v!=demo_motion_enabled())demo_motion_toggle();break;
    case CT_TABLE:if(v<0||v>1)return 0;control_fence();if(v!=demo_table_enabled())demo_table_toggle();break;
    case CT_BGM:if(v<0||v>1)return 0;music_enable(v);break;
    case CT_NEXT_TRACK:music_next();break;
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
        const int carrot[]={9,13,17,25},thscale[]={75,100,125,150};e.type=CT_BULLET_SIZE;
        e.value=cycle_value(scene==SCENE_TH?thscale:carrot,4,scene==SCENE_TH?th_pressure_scale():game_seed_size(),c==']'?1:-1);break;}
    case '{':case '}':{const int sizes[]={0,6,8,12,16,24,32,40};e.type=CT_SQUARE_SIZE;e.value=cycle_value(sizes,8,demo_square_size(),c=='}'?1:-1);break;}
    case '\\':e.type=CT_NEXT_TRACK;break;
    case ';':e.type=CT_BGM;e.value=!music_enabled();break;
    case ',':case '.':case '-':case '+':case '=':
        if(scene==SCENE_SQUARES||(scene==SCENE_GAME&&game.stress_tier==STRESS_CUSTOM)||(scene==SCENE_TH&&th.diff==TH_STRESS)){
            e.type=CT_DELTA;e.value=(c=='.'||c=='+'||c=='=')?1:-1;}break;
    }
    if(e.type!=CT_NONE){int ok=control_apply(e);if(!ok)tx_puts("[control unavailable in this scene]\r\n");else if(e.type!=CT_PAUSE&&e.type!=CT_SKILL)control_state(0);}
    else handle_legacy_char(c);
}
