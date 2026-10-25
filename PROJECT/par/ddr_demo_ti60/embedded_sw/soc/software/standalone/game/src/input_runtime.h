/* Included after control_runtime.h; physical devices feed the same dispatcher. */
static InputDevices devices;
static int devices_ready,devices_present,keyboard_ack_pending;
static int matrix_autofire_before_diagnostic;
static uint32_t keyboard_faults,keyboard_acks,keyboard_tx_timeouts;
static void physical_action(void *opaque,InputAction action,int value);
static void input_ready(void){
    if(!devices_ready){input_devices_init(&devices,physical_action,0);devices_ready=1;
        devices_present=HDMI_REG(0x80)==0x494e5035u;}
}
static int control_input_command(ControlEvent e){
    int ok=0,was;input_ready();if(!devices_present)return 0;was=devices.diagnostic;
    if(e.type==CT_INPUT_DIAG)ok=input_set_diagnostic(&devices,e.value);
    else if(e.type==CT_MATRIX_CAL)ok=input_matrix_calibration(&devices,e.value);
    else if(e.type==CT_MATRIX_BIND)ok=e.value>=0&&input_matrix_bind(&devices,(unsigned)e.value);
    else if(e.type==CT_KEYBOARD_SYNC&&devices.diagnostic){keyboard_ack_pending=1;ok=1;}
    if(ok&&!was&&devices.diagnostic)matrix_autofire_before_diagnostic=matrix_autofire;
    if(ok&&was&&!devices.diagnostic)matrix_autofire=matrix_autofire_before_diagnostic;
    if(ok&&devices.diagnostic){pad_set_device(1,0);pad_set_device(2,0);pad_set_physical(0);matrix_autofire=0;}
    return ok;
}
static void input_hex(char *out,const uint8_t *bytes,unsigned n){
    static const char hex[]="0123456789abcdef";unsigned i;
    for(i=0;i<n&&i<16;i++){out[2*i]=hex[bytes[i]>>4];out[2*i+1]=hex[bytes[i]&15u];}out[2*i]=0;
}
static void control_input_state(unsigned id){
    tx_printf("I5 id=%u supported=%u held=%u matrix_keys=%u matrix_held=%u keyboard_held=%u keyboard_reports=%u malformed=%u rollover=%u ghosts=%u status_reports=%u uart_error=%u\r\n",
      id,(unsigned)devices_present,(unsigned)devices.published_held,(unsigned)devices.matrix_raw,(unsigned)devices.matrix_held,(unsigned)devices.keyboard_held,(unsigned)devices.keyboard_reports,
      (unsigned)devices.malformed_frames,(unsigned)devices.rollover_reports,(unsigned)devices.ghost_reports,
      (unsigned)devices.status_reports,devices_present?((HDMI_REG(0x88)>>2)&1u):0u);
    {
        static const char hex[]="0123456789abcdef";
        char map[17],candidate[17],tail[33];uint8_t bytes[16];unsigned i,n=devices.rx_history_used<16?devices.rx_history_used:16;
        for(i=0;i<16;i++){map[i]=hex[devices.matrix_map[i]&15u];candidate[i]=i<devices.cal_step?hex[devices.cal_map[i]&15u]:'-';}
        map[16]=candidate[16]=0;
        for(i=0;i<n;i++)bytes[i]=devices.rx_history[(devices.rx_history_head+16u-devices.rx_history_used+i)&15u];
        input_hex(tail,bytes,n);
        tx_printf("I9 id=%u input_diag_supported=1 input_diag=%u matrix_events=%u matrix_last=%u matrix_seen=%u matrix_logical=%u matrix_cal=%u matrix_step=%u matrix_cal_error=%u matrix_wait=%u matrix_mapped=%u matrix_map=x%s matrix_candidate=x%s keyboard_down=%u keyboard_bytes=%u keyboard_headers=%u keyboard_tail=x%s keyboard_faults=%u keyboard_acks=%u keyboard_tx_timeouts=%u mouse_reports=%u status_reports=%u\r\n",
          id,(unsigned)devices.diagnostic,(unsigned)devices.matrix_events,(unsigned)devices.matrix_last,(unsigned)devices.matrix_seen,(unsigned)devices.matrix_previous,
          (unsigned)devices.calibrating,(unsigned)devices.cal_step,(unsigned)devices.cal_error,(unsigned)devices.cal_wait,(unsigned)devices.map_applied,map,candidate,
          (unsigned)devices.keyboard_down,(unsigned)devices.keyboard_bytes,(unsigned)devices.keyboard_headers,tail,(unsigned)keyboard_faults,(unsigned)keyboard_acks,(unsigned)keyboard_tx_timeouts,(unsigned)devices.mouse_reports,(unsigned)devices.status_reports);
    }
}
static void physical_action(void *opaque,InputAction action,int value){
    ControlEvent e={CT_NONE,value,0,1};(void)opaque;
    if(action!=INPUT_CH9350_STATE)pad_set_physical(devices.published_held);
    switch(action){
    case INPUT_SOURCE_HELD:{
        unsigned source=(unsigned)value>>16,mask=(unsigned)value&65535u;
        if(source==2&&scene==SCENE_TH&&th.diff!=TH_STRESS&&th.mode==TH_PLAY&&
           (mask&PAD_SHOT)&&!(pad_source_held(2,now(),BSP_CLINT_HZ/1000u)&PAD_SHOT))matrix_autofire=!matrix_autofire;
        pad_set_device(source,mask);return;}
    case INPUT_CH9350_STATE:keyboard_ack_pending=1;return;
    case INPUT_HELD:
        pad_set_physical((unsigned)value);
        if(value && autoplay){autoplay=0;stats_reset=1;++settings_epoch;}
        return;
    case INPUT_SCENE_NEXT:e.type=CT_SCENE;e.value=(scene+1)%SCENE_COUNT;break;
    case INPUT_START_PAUSE:
        e.type=((scene==SCENE_TH&&(th.mode==TH_PLAY||th.mode==TH_PAUSE))||
                (scene==SCENE_GAME&&(game.phase==PLAYING||game.phase==PAUSED)))?CT_PAUSE:CT_START;break;
    case INPUT_STRESS_MENU:e.type=CT_STRESS;e.value=scene==SCENE_GAME?(game.phase==STRESS_SELECT?4:0):1;break;
    case INPUT_COUNT_DELTA:e.type=CT_DELTA;break;
    case INPUT_STEP_NEXT:e.type=CT_STEP;e.value=count_step==100?10:count_step==10?1:100;break;
    case INPUT_STEP_SET:e.type=CT_STEP;break;
    case INPUT_SIZE_NEXT:handle_char(scene==SCENE_SQUARES?'}':']');return;
    case INPUT_BULLET_DELTA:handle_char(value<0?'[':']');return;
    case INPUT_SQUARE_DELTA:handle_char(value<0?'{':'}');return;
    case INPUT_RESOLUTION_NEXT:e.type=CT_RESOLUTION;e.value=!(HDMI_MODE&1u);break;
    case INPUT_MENU:e.type=CT_MENU;break;
    case INPUT_PICK:
        if((scene==SCENE_GAME&&!game.limit_mode)||(scene==SCENE_TH&&th.diff!=TH_STRESS)){player_pick[devices.source]=value;return;}
        e.type=CT_PICK;break;
    case INPUT_RESTART:e.type=CT_RESTART;break;
    case INPUT_AUTO_TOGGLE:e.type=CT_AUTO;e.value=!autoplay;break;
    case INPUT_ALPHA_TOGGLE:e.type=CT_ALPHA;e.value=!alpha_fx;break;
    case INPUT_VOLUME_DELTA:e.type=CT_VOLUME;e.value=bounded(aud.master+value*16,0,256);break;
    case INPUT_STATS_TOGGLE:e.type=CT_STATS;e.value=!show_stats;break;
    case INPUT_BACKEND_TOGGLE:e.type=CT_BACKEND;e.value=!use_hw;break;
    case INPUT_VSYNC_TOGGLE:e.type=CT_VSYNC;e.value=!vsync;break;
    case INPUT_BGM_TOGGLE:e.type=CT_BGM;e.value=!music_enabled();break;
    case INPUT_TRACK_NEXT:e.type=CT_NEXT_TRACK;break;
    case INPUT_STATUS:e.type=CT_STATUS;break;
    case INPUT_VIDEO_CONFIRM:e.type=CT_VIDEO_CONFIRM;break;
    case INPUT_VIDEO_RECOVER:e.type=CT_VIDEO_RECOVER;break;
    case INPUT_PAUSE:e.type=CT_PAUSE;break;
    case INPUT_TONE_TOGGLE:e.type=CT_TONE;e.value=!aud.tone;break;
    case INPUT_LEGACY:handle_char((char)value);return;
    default:return;
    }
    if(control_apply(e))control_state(0);
    else tx_puts("[physical control unavailable in this scene]\r\n");
}
static uint32_t input_milliseconds(void){
    static uint32_t previous,wraps;uint32_t tick=now();
    if(tick<previous)++wraps;previous=tick;
    return (uint32_t)((((uint64_t)wraps<<32)|tick)*1000u/BSP_CLINT_HZ);
}
static void poll_matrix_rescue(uint16_t keys,uint32_t ms){
    static int held,sent;static uint32_t start;
    if(devices.diagnostic||keys!=(1u<<14)||devices.ghost_latched){held=sent=0;return;}
    if(!held){held=1;start=ms;}
    if(!sent&&(uint32_t)(ms-start)>=2000u){
        sent=1;
        if(control_apply((ControlEvent){CT_VIDEO_RECOVER,0,0,0})){
            tx_puts("[matrix S15 held: force 720p recovery]\r\n");control_state(0);
        }
    }
}
static void poll_input_devices(void){
    unsigned limit=64;
    uint32_t ms=input_milliseconds();
    input_ready();
    if(!devices_present)return;
    input_matrix_snapshot(&devices,(uint16_t)HDMI_REG(0x84));
    poll_matrix_rescue(devices.matrix_previous,ms);
    if(HDMI_REG(0x88)&4u){keyboard_faults++;input_keyboard_release(&devices);HDMI_REG(0x94)=1;
        tx_puts("[keyboard UART error: released and FIFO cleared]\r\n");}
    while(limit--&&(HDMI_REG(0x88)&1u))input_ch9350_feed(&devices,(uint8_t)HDMI_REG(0x8c),ms);
    input_devices_poll(&devices,ms);
    if(keyboard_ack_pending){
        /* CH9350 DS v2.7 8.3.9/8.3.11: mode-2 status needs an 11-byte reply.
         * Send a contiguous 8N1 frame (~0.96 ms); gaps must not span a render.
         * PID slots=0, LEDs=off, USB state=0xff, config=0, protocol version=0x20.
         * This is not the state-0 special command (USB state 0x80).
         */
        static const uint8_t reply[11]={0x57,0xab,0x12,0,0,0,0,0,0xff,0,0x20};
        unsigned i;uint32_t start=now();keyboard_ack_pending=0;
        for(i=0;i<sizeof reply;i++){
            while(HDMI_REG(0x88)&8u)if((uint32_t)(now()-start)>20000u*TICKS_PER_US){
                keyboard_tx_timeouts++;tx_puts("[keyboard TX timeout]\r\n");return;}
            HDMI_REG(0x90)=reply[i];
        }
        keyboard_acks++;
    }
}
