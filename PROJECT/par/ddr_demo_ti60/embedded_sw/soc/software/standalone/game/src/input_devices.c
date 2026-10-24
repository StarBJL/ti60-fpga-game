#include "input_devices.h"
#include <string.h>
enum { UP=1,DOWN=2,LEFT=4,RIGHT=8,SHOT=16,BOMB=32,SLOW=64,PAUSE=128,START=256 };
static void emit(InputDevices *d,InputAction a,int v){if(d->emit)d->emit(d->opaque,a,v);}
static void publish(InputDevices *d){
    uint16_t held=d->matrix_held|d->keyboard_held;
    if(d->matrix_held!=d->published_matrix){d->published_matrix=d->matrix_held;emit(d,INPUT_SOURCE_HELD,0x20000|d->matrix_held);}
    if(d->keyboard_held!=d->published_keyboard){d->published_keyboard=d->keyboard_held;emit(d,INPUT_SOURCE_HELD,0x10000|d->keyboard_held);}
    if(held!=d->published_held){d->published_held=held;emit(d,INPUT_HELD,held);}
}
void input_devices_init(InputDevices *d,InputEmit cb,void *arg){memset(d,0,sizeof(*d));d->emit=cb;d->opaque=arg;}
int input_matrix_has_ghost(uint16_t keys){
    int a,b;for(a=0;a<4;a++)for(b=a+1;b<4;b++){
        unsigned common=((keys>>(a*4))&(keys>>(b*4)))&15u;
        if(common&&(common&(common-1u)))return 1;
    }return 0;
}
static void matrix_action(InputDevices *d,unsigned key,int fn){
    static const InputAction base[16]={
        INPUT_SCENE_NEXT,INPUT_HELD,INPUT_HELD,INPUT_HELD,
        INPUT_HELD,INPUT_HELD,INPUT_HELD,INPUT_HELD,
        INPUT_COUNT_DELTA,INPUT_COUNT_DELTA,INPUT_STEP_NEXT,INPUT_SIZE_NEXT,
        INPUT_START_PAUSE,INPUT_STRESS_MENU,INPUT_RESOLUTION_NEXT,INPUT_HELD};
    static const InputAction shifted[16]={
        INPUT_MENU,INPUT_PICK,INPUT_PICK,INPUT_HELD,
        INPUT_PICK,INPUT_RESTART,INPUT_AUTO_TOGGLE,INPUT_ALPHA_TOGGLE,
        INPUT_VOLUME_DELTA,INPUT_VOLUME_DELTA,INPUT_STATS_TOGGLE,INPUT_BACKEND_TOGGLE,
        INPUT_VSYNC_TOGGLE,INPUT_BGM_TOGGLE,INPUT_TRACK_NEXT,INPUT_STATUS};
    InputAction a=fn?shifted[key]:base[key];int v=0;
    if(a==INPUT_HELD)return;
    if(a==INPUT_COUNT_DELTA||a==INPUT_VOLUME_DELTA)v=key==8?-1:1;
    if(a==INPUT_PICK)v=key==1?1:key==2?2:3;
    emit(d,a,v);
}
void input_matrix_snapshot(InputDevices *d,uint16_t keys){
    unsigned i;uint16_t rising;int fn;d->source=2;
    if(input_matrix_has_ghost(keys)){
        if(!d->ghost_latched)d->ghost_reports++;
        d->ghost_latched=1;
    }
    if(d->ghost_latched){
        d->matrix_held=0;d->matrix_previous=keys;publish(d);
        if(!keys)d->ghost_latched=0;
        return;
    }
    fn=!!(keys&(1u<<3));rising=keys&~d->matrix_previous;d->matrix_previous=keys;
    d->matrix_held=fn?0:
      ((keys&(1u<<1))?UP:0)|((keys&(1u<<5))?DOWN:0)|
      ((keys&(1u<<4))?LEFT:0)|((keys&(1u<<6))?RIGHT:0)|
      ((keys&(1u<<2))?SHOT:0)|((keys&(1u<<7))?BOMB:0)|
      ((keys&(1u<<15))?SLOW:0);
    publish(d);
    for(i=0;i<16;i++)if(rising&(1u<<i))matrix_action(d,i,fn);
}
static int contains(const uint8_t *r,unsigned code){int i;for(i=2;i<8;i++)if(r[i]==code)return 1;return 0;}
static unsigned held_for(unsigned code){
    switch(code){
    case 0x52:case 0x1a:return UP;case 0x51:case 0x16:return DOWN;
    case 0x50:case 0x04:return LEFT;case 0x4f:case 0x07:return RIGHT;
    case 0x1d:case 0x2c:return SHOT;case 0x1b:case 0x0d:return BOMB;
    case 0x29:return PAUSE;case 0x28:return START;default:return 0;
    }
}
static void keyboard_action(InputDevices *d,unsigned code,int shift){
    switch(code){
    case 0x11:emit(d,INPUT_SCENE_NEXT,0);break; /* N */
    case 0x13:emit(d,INPUT_PAUSE,0);break; /* P */
    case 0x15:emit(d,INPUT_RESTART,0);break; /* R */
    case 0x10:emit(d,INPUT_MENU,0);break; /* M */
    case 0x0f:emit(d,INPUT_STRESS_MENU,0);break; /* L */
    case 0x05:emit(d,INPUT_AUTO_TOGGLE,0);break;
    case 0x08:emit(d,INPUT_ALPHA_TOGGLE,0);break;
    case 0x09:emit(d,INPUT_STATS_TOGGLE,0);break;
    case 0x0a:emit(d,INPUT_BACKEND_TOGGLE,0);break;
    case 0x19:emit(d,INPUT_VSYNC_TOGGLE,0);break;
    case 0x17:emit(d,INPUT_TONE_TOGGLE,0);break;
    case 0x1e:case 0x1f:case 0x20:emit(d,INPUT_PICK,(int)code-0x1d);break;
    case 0x3f:emit(d,INPUT_STEP_SET,100);break;
    case 0x40:emit(d,INPUT_STEP_SET,10);break;
    case 0x41:emit(d,INPUT_STEP_SET,1);break;
    case 0x42:emit(d,INPUT_RESOLUTION_NEXT,0);break;
    case 0x43:emit(d,INPUT_VIDEO_CONFIRM,0);break;
    case 0x2d:case 0x36:emit(d,INPUT_COUNT_DELTA,-1);break;
    case 0x2e:case 0x37:emit(d,INPUT_COUNT_DELTA,1);break;
    case 0x2f:case 0x30:emit(d,shift?INPUT_SQUARE_DELTA:INPUT_BULLET_DELTA,code==0x2f?-1:1);break;
    case 0x31:emit(d,INPUT_TRACK_NEXT,0);break;
    case 0x33:emit(d,INPUT_BGM_TOGGLE,0);break;
    case 0x3a:case 0x0b:emit(d,INPUT_STATUS,0);break; /* F1/H */
    /* The remaining measurement controls use the existing diagnostic handler. */
    case 0x06:case 0x0c:case 0x0e:case 0x12:case 0x14:case 0x18:case 0x1c:
        emit(d,INPUT_LEGACY,(int)code-4+'a');break;
    case 0x21:case 0x22:case 0x23:case 0x24:case 0x25:case 0x26:case 0x27:
        emit(d,INPUT_LEGACY,code==0x27?'0':(int)code - 0x1e + '1');break;
    default:break;
    }
}
void input_keyboard_release(InputDevices *d){
    d->source=1;
    memset(d->keyboard_previous,0,sizeof(d->keyboard_previous));d->keyboard_held=0;d->rx_state=0;publish(d);
}
static void keyboard_report(InputDevices *d){
    unsigned i,held=0;uint8_t *r=d->rx_data;d->keyboard_reports++;d->source=1;
    /* ErrorRollOver / POSTFail / ErrorUndefined are not real key usages. */
    for(i=2;i<8;i++)if(r[i]>=1&&r[i]<=3){d->rollover_reports++;input_keyboard_release(d);return;}
    if(r[0]&0x22u)held|=SLOW;
    for(i=2;i<8;i++)held|=held_for(r[i]);
    d->keyboard_held=(uint16_t)held;publish(d);
    for(i=2;i<8;i++)if(r[i]&&!contains(d->keyboard_previous,r[i])){
        unsigned j;int duplicate=0;for(j=2;j<i;j++)if(r[j]==r[i])duplicate=1;
        if(!duplicate)keyboard_action(d,r[i],!!(r[0]&0x22u));
    }
    memcpy(d->keyboard_previous,r,8);
}
void input_devices_poll(InputDevices *d,uint32_t now_ms){
    if(d->rx_state&&(uint32_t)(now_ms-d->rx_last_ms)>20u){d->rx_state=0;d->malformed_frames++;}
}
void input_ch9350_feed(InputDevices *d,uint8_t byte,uint32_t now_ms){
    /* A rendering loop may dequeue a valid report in two batches >20 ms apart.
     * Only the caller's empty-FIFO poll may expire it; a queued byte is evidence
     * to continue the report, not evidence of a wire-level idle interval. */
    d->rx_last_ms=now_ms;
    switch(d->rx_state){
    case 0:if(byte==0x57)d->rx_state=1;break;
    case 1:if(byte==0xab)d->rx_state=2;else d->rx_state=byte==0x57?1:0;break;
    case 2:
        d->rx_tag=byte;d->rx_used=0;
        if(byte==1)d->rx_length=8;
        else if(byte==2)d->rx_length=4;
        else if(byte==4)d->rx_length=7;
        else if(byte==0x80)d->rx_length=1;
        else if(byte==0x87||byte==0x89){d->status_reports++;d->rx_state=0;break;}
        else{d->malformed_frames++;d->rx_state=byte==0x57?1:0;break;}
        d->rx_state=3;break;
    default:
        d->rx_data[d->rx_used++]=byte;
        if(d->rx_used==d->rx_length){
            d->rx_state=0;
            if(d->rx_tag==1)keyboard_report(d);
            else if(d->rx_tag==2||d->rx_tag==4)d->mouse_reports++;
            else {d->status_reports++;emit(d,INPUT_CH9350_STATE,d->rx_data[0]);}
        }break;
    }
}
