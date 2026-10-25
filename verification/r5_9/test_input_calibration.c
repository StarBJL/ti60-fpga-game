#include "input_devices.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned callbacks,resolutions,statuses,acks;
static void cb(void *o,InputAction a,int v){(void)o;(void)v;callbacks++;resolutions+=a==INPUT_RESOLUTION_NEXT;statuses+=a==INPUT_STATUS;acks+=a==INPUT_CH9350_STATE;}
static void press(InputDevices *d,unsigned bit){input_matrix_snapshot(d,0);input_matrix_snapshot(d,(uint16_t)(1u<<bit));input_matrix_snapshot(d,0);}
static void keyboard(InputDevices *d,unsigned key){unsigned b[]={0x57,0xab,1,0,0,key,0,0,0,0,0};for(unsigned i=0;i<11;i++)input_ch9350_feed(d,(uint8_t)b[i],1);}
int main(void){
 InputDevices d;input_devices_init(&d,cb,0);
 press(&d,14);assert(statuses==1&&resolutions==0&&d.matrix_events==1&&d.matrix_last==0x4000&&d.matrix_raw==0);
 assert(input_matrix_calibration(&d,1));unsigned n=callbacks;
 press(&d,0);press(&d,0);assert(d.cal_step==1&&d.cal_error==2);
 input_matrix_snapshot(&d,3);input_matrix_snapshot(&d,0);assert(d.cal_step==1&&d.cal_error==1);
 assert(!input_matrix_calibration(&d,2));assert(input_matrix_calibration(&d,3));assert(!d.cal_step);
 unsigned map[16];for(unsigned i=0;i<16;i++){unsigned row=i/4;map[i]=(row==1?3:row==3?1:row)*4+i%4;press(&d,map[i]);}
 assert(d.cal_step==16&&d.cal_seen==65535&&callbacks==n);
 assert(input_matrix_calibration(&d,2));assert(d.map_applied&&d.diagnostic&&!d.calibrating);
 assert(input_set_diagnostic(&d,0));input_matrix_snapshot(&d,0x4000);assert(d.matrix_previous==(1u<<6)&&d.matrix_held==8&&resolutions==0);
 input_matrix_snapshot(&d,0);assert(d.matrix_last==0x4000&&!d.matrix_held);
 assert(input_set_diagnostic(&d,1));n=callbacks;keyboard(&d,0x42);assert(d.keyboard_reports==1&&d.keyboard_bytes==11&&d.keyboard_headers==1&&callbacks==n);assert(!input_set_diagnostic(&d,0));
 keyboard(&d,0x1a);assert(d.keyboard_held==1&&!input_set_diagnostic(&d,0));keyboard(&d,0);assert(!d.keyboard_held);
 const unsigned char status[]={0x57,0xab,0x80,0xa1};for(unsigned i=0;i<4;i++)input_ch9350_feed(&d,status[i],2);
 assert(acks==1&&d.keyboard_bytes==37&&d.rx_history_used==16);assert(input_set_diagnostic(&d,0));
 assert(input_matrix_calibration(&d,1));assert(!input_matrix_bind(&d,16));assert(input_matrix_bind(&d,0));assert(!input_matrix_bind(&d,16));
 assert(input_matrix_calibration(&d,0));assert(d.matrix_map[6]==14&&d.diagnostic);assert(input_set_diagnostic(&d,0));
 /* A full restoration is transactional and still suppresses controls. */
 assert(input_matrix_calibration(&d,1));for(unsigned i=0;i<16;i++)assert(input_matrix_bind(&d,(i<<4)|((i*7)&15)));
 input_matrix_snapshot(&d,1);assert(!input_matrix_calibration(&d,2));input_matrix_snapshot(&d,0);assert(input_matrix_calibration(&d,2));assert(input_set_diagnostic(&d,0));
 input_matrix_snapshot(&d,0x0033);assert(d.ghost_latched&&!d.matrix_held);input_matrix_snapshot(&d,0);
 /* All 256 single-key checks over independent permutations: no remapped
    matrix position can produce a resolution request. */
 for(unsigned k=0;k<16;k++){
  assert(input_matrix_calibration(&d,1));for(unsigned i=0;i<16;i++)assert(input_matrix_bind(&d,(i<<4)|((i+k)&15)));
  assert(input_matrix_calibration(&d,2));assert(input_set_diagnostic(&d,0));
  for(unsigned i=0;i<16;i++){input_matrix_snapshot(&d,(uint16_t)(1u<<((i+k)&15)));assert(d.matrix_previous==(1u<<i));input_matrix_snapshot(&d,0);}
 }
 assert(resolutions==0);
 puts("PASS: observed S7/raw0x4000 -> S15 cannot change video; staged 16-key calibration/restore/undo/cancel; duplicate/chord guards; raw ghost checks; isolated USB reports and byte history; 256 remapped key checks");
}
