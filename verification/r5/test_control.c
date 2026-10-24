#include "control.h"
#include "pad.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void parse(const char *s,ControlType kind,int value){ControlEvent e;assert(control_parse(s,&e));assert(e.id==17&&e.type==kind&&e.value==value);}
int main(void){
 parse("17 volume_delta -1",CT_VOLUME_DELTA,-1);parse("17 volume_delta 1",CT_VOLUME_DELTA,1);
 parse("17 fast",CT_FAST,0);parse("17 bars 1",CT_BARS,1);
 parse("17 step 1",CT_STEP,1);parse("17 step 10",CT_STEP,10);parse("17 step 100",CT_STEP,100);
 parse("17 delta -1",CT_DELTA,-1);parse("17 count 14001",CT_COUNT,14001);parse("17 scene 2",CT_SCENE,2);parse("17 status",CT_STATUS,0);
 ControlEvent e;const char *bad[]={"17 unknown 2","17 count 1x","17 count","17 status 0","-1 scene 2","17 count 2147483648","17 count --1","17 step 1 2","17step 1",""};
 for(unsigned i=0;i<sizeof bad/sizeof bad[0];i++){assert(!control_parse(bad[i],&e));assert(e.type==CT_INVALID);}
 ControlParser p={0};assert(control_feed(&p,'w',&e)==0);const char*s=":17 count 1001\r\n";int n=0;
 while(*s)if(control_feed(&p,*s++,&e)==2)n++;assert(n==1&&e.type==CT_COUNT&&e.value==1001);
 control_feed(&p,':',&e);for(int i=0;i<300;i++)assert(control_feed(&p,'g',&e)==1);assert(control_feed(&p,'\n',&e)==2&&e.type==CT_INVALID);assert(control_feed(&p,'g',&e)==0);
 assert(control_is_step(1)&&control_is_step(10)&&control_is_step(100)&&!control_is_step(1000));
 pad_set_state(PAD_LEFT|PAD_SHOT,1000);assert(pad_held(1001,1)==(PAD_LEFT|PAD_SHOT));assert(pad_pressed(1001,1)==(PAD_LEFT|PAD_SHOT));assert(!pad_pressed(1002,1));
 pad_set_state(0,1010);pad_set_state(PAD_BOMB,1020);pad_set_state(0,1021);assert(pad_pressed(1022,1)==PAD_BOMB);assert(!pad_pressed(1023,1));
 pad_set_state(PAD_RIGHT,1100);assert(!pad_held(1401,1));assert(!pad_pressed(1401,1));assert(!pad_packets&&!pad_errors);assert(pad_has_state());pad_use_terminal();assert(!pad_has_state());
 parse("17 resolution 0",CT_RESOLUTION,0);parse("17 resolution 1",CT_RESOLUTION,1);
 pad_set_physical(0);assert(pad_has_state()&&!pad_held(100000,1));
 pad_set_physical(PAD_UP|PAD_SHOT);assert(pad_has_state());
 pad_set_state(PAD_LEFT,2000);assert(pad_held(2001,1)==(PAD_LEFT|PAD_UP|PAD_SHOT));
 assert(pad_pressed(2001,1)==(PAD_LEFT|PAD_UP|PAD_SHOT));
 pad_set_state(0,2010);assert(pad_held(2011,1)==(PAD_UP|PAD_SHOT));
 assert(pad_held(100000,1)==(PAD_UP|PAD_SHOT));assert(!pad_pressed(100000,1));
 pad_set_physical(0);assert(!pad_held(100001,1));
 pad_set_physical(PAD_BOMB);pad_set_physical(0);assert(pad_pressed(100002,1)==PAD_BOMB);
 pad_use_terminal();assert(!pad_has_state());
 puts("PASS physical/PC OR, independent release, unlimited physical hold, atomic protocol, malformed/overflow frames, 100/10/1 steps, held and short-tap edges, release timeout, physical packet counters unchanged");return 0;
}
