#include "input_devices.h"
#include <assert.h>
#include <stdio.h>
static int recover;static InputDevices d;
static void emit(void *o,InputAction a,int v){(void)o;(void)v;if(a==INPUT_VIDEO_RECOVER){assert(d.source==1);++recover;}}
static void report(unsigned char key){unsigned char b[]={0x57,0xab,1,0,0,key,0,0,0,0,0};for(unsigned i=0;i<sizeof b;i++)input_ch9350_feed(&d,b[i],100);}
int main(void){input_devices_init(&d,emit,0);report(0x45);assert(recover==1);report(0x45);assert(recover==1);report(0);report(0x45);assert(recover==2);puts("PASS USB F12 recovery rising edge, repeat suppression, release/repress");}
