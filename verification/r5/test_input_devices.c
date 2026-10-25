#include "input_devices.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static struct {InputAction a;int v;} events[256];static unsigned count;
static void cb(void *opaque,InputAction a,int v){assert(opaque==&count);if(a==INPUT_SOURCE_HELD)return;assert(count<256);events[count].a=a;events[count++].v=v;}
static void report(InputDevices *d,unsigned key,unsigned mod,unsigned now){
    const unsigned char h[]={0x57,0xab,1};unsigned i;
    for(i=0;i<3;i++)input_ch9350_feed(d,h[i],now);
    input_ch9350_feed(d,(unsigned char)mod,now);input_ch9350_feed(d,0,now);
    input_ch9350_feed(d,(unsigned char)key,now);for(i=0;i<5;i++)input_ch9350_feed(d,0,now);
}
int main(void){
    InputDevices d;unsigned n,i;input_devices_init(&d,cb,&count);
    input_matrix_snapshot(&d,(1u<<1)|(1u<<2));assert(count==1&&events[0].a==INPUT_HELD&&events[0].v==17);
    input_matrix_snapshot(&d,(1u<<1)|(1u<<2));assert(count==1);
    input_matrix_snapshot(&d,0);assert(events[count-1].v==0);
    input_matrix_snapshot(&d,1u<<10);assert(events[count-1].a==INPUT_STEP_NEXT);
    input_matrix_snapshot(&d,0);input_matrix_snapshot(&d,(1u<<3)|(1u<<14));
    assert(events[count-1].a==INPUT_TRACK_NEXT);input_matrix_snapshot(&d,0);
    input_matrix_snapshot(&d,1u<<14);assert(events[count-1].a==INPUT_STATUS);
    input_matrix_snapshot(&d,0);
    assert(input_matrix_has_ghost(0x0033));assert(!input_matrix_has_ghost(0x0013));
    n=count;input_matrix_snapshot(&d,0x0033);assert(d.ghost_latched&&d.ghost_reports==1&&count==n);
    input_matrix_snapshot(&d,1u<<1);assert(count==n);input_matrix_snapshot(&d,0);assert(!d.ghost_latched);
    input_matrix_snapshot(&d,1u<<1);assert(events[count-1].v==1);
    report(&d,0x1d,2,100);assert(events[count-1].a==INPUT_HELD&&events[count-1].v==81);
    n=count;report(&d,0x1d,2,101);assert(count==n); /* identical report: no repeat */
    input_matrix_snapshot(&d,0);assert(events[count-1].v==80);report(&d,0,0,103);assert(events[count-1].v==0);
    report(&d,0x3f,0,104);assert(events[count-1].a==INPUT_STEP_SET&&events[count-1].v==100);
    n=count;report(&d,0x3f,0,105);assert(count==n);report(&d,0,0,106);
    report(&d,0x40,0,107);assert(events[count-1].v==10);report(&d,0x41,0,108);assert(events[count-1].v==1);
    report(&d,0x42,0,109);assert(events[count-1].a==INPUT_RESOLUTION_NEXT);
    report(&d,0x2f,2,110);assert(events[count-1].a==INPUT_SQUARE_DELTA&&events[count-1].v==-1);
    report(&d,0x30,0,111);assert(events[count-1].a==INPUT_BULLET_DELTA&&events[count-1].v==1);
    report(&d,0x1d,0,112);assert(d.keyboard_held==16);report(&d,1,0,113);assert(d.keyboard_held==0&&d.rollover_reports==1);
    /* Header bytes inside fixed-length payload do not cause false resync. */
    {unsigned char mouse[]={0x57,0xab,2,0x57,0xab,1,0};n=count;for(i=0;i<sizeof(mouse);i++)input_ch9350_feed(&d,mouse[i],120);assert(d.mouse_reports==1&&count==n);}
    input_ch9350_feed(&d,0x57,130);input_ch9350_feed(&d,0xab,130);input_ch9350_feed(&d,1,130);
    input_ch9350_feed(&d,0,130);input_devices_poll(&d,151);assert(!d.rx_state&&d.malformed_frames==1);
    report(&d,0x1d,0,160);assert(d.keyboard_held==16);input_devices_poll(&d,10000);assert(d.keyboard_held==16); /* change-only reports */
    input_keyboard_release(&d);assert(d.keyboard_held==0&&events[count-1].v==0);
    /* Dequeue timestamps are not UART arrival timestamps. A valid report can
       straddle two slow render-loop polls without a genuine serial gap. */
    input_ch9350_feed(&d,0x57,10020);input_ch9350_feed(&d,0xab,10020);
    input_ch9350_feed(&d,1,10020);input_ch9350_feed(&d,0,10020);
    input_ch9350_feed(&d,0,10053);input_ch9350_feed(&d,0x1d,10053);
    for(i=0;i<5;i++)input_ch9350_feed(&d,0,10053);
    input_devices_poll(&d,10053);assert(d.keyboard_held==16&&events[count-1].v==16);
    input_keyboard_release(&d);assert(events[count-1].v==0);
    {unsigned char status[]={0x57,0xab,0x80,0xa1};for(i=0;i<sizeof status;i++)input_ch9350_feed(&d,status[i],10010);
     assert(events[count-1].a==INPUT_CH9350_STATE&&events[count-1].v==0xa1&&d.status_reports==1);}
    /* Exhaust every matrix state: accepted patterns never contain rectangles. */
    for(i=0;i<65536;i++){unsigned a,b,ghost=0;for(a=0;a<4;a++)for(b=a+1;b<4;b++){
      unsigned m=((i>>(4*a))&(i>>(4*b)))&15u;ghost|=!!(m&&(m&(m-1u)));}
      assert(input_matrix_has_ghost((uint16_t)i)==(int)ghost);}
    printf("PASS: input matrix/Fn/ghost, USB HID held/release/rollover, steps 100/10/1, resolution, framing and source merge (%u events)\n",count);
    return 0;
}
