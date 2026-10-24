/* Included after control_runtime.h; physical devices feed the same dispatcher. */
static InputDevices devices;
static int devices_ready,devices_present,keyboard_ack_pending;
static void control_input_state(unsigned id){
    tx_printf("I5 id=%u supported=%u held=%u keyboard_reports=%u malformed=%u rollover=%u ghosts=%u status_reports=%u uart_error=%u\r\n",
      id,(unsigned)devices_present,(unsigned)devices.published_held,(unsigned)devices.keyboard_reports,
      (unsigned)devices.malformed_frames,(unsigned)devices.rollover_reports,(unsigned)devices.ghost_reports,
      (unsigned)devices.status_reports,devices_present?((HDMI_REG(0x88)>>2)&1u):0u);
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
    case INPUT_STATUS:e.type=devices.source==2&&video_trial?CT_VIDEO_CONFIRM:CT_STATUS;break;
    case INPUT_VIDEO_CONFIRM:e.type=CT_VIDEO_CONFIRM;break;
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
static void poll_input_devices(void){
    unsigned limit=64;
    uint32_t ms=input_milliseconds();
    if(!devices_ready){input_devices_init(&devices,physical_action,0);devices_ready=1;
        devices_present=HDMI_REG(0x80)==0x494e5035u;}
    if(!devices_present)return;
    input_matrix_snapshot(&devices,(uint16_t)HDMI_REG(0x84));
    if(HDMI_REG(0x88)&4u){input_keyboard_release(&devices);HDMI_REG(0x94)=1;
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
                tx_puts("[keyboard TX timeout]\r\n");return;}
            HDMI_REG(0x90)=reply[i];
        }
    }
}
