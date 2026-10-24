#include "pad.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 pad_use_terminal();pad_set_physical(0);
 pad_set_state(PAD_UP|PAD_SHOT,1000);pad_set_physical(PAD_LEFT|PAD_SHOT);
 assert(pad_held(1001,1)==(PAD_UP|PAD_LEFT|PAD_SHOT));
 assert(pad_pressed(1001,1)==(PAD_UP|PAD_LEFT|PAD_SHOT));
 pad_set_state(0,1002);assert(pad_held(1003,1)==(PAD_LEFT|PAD_SHOT));assert(!pad_pressed(1003,1));
 pad_set_state(PAD_RIGHT,1010);pad_set_physical(0);assert(pad_held(1011,1)==PAD_RIGHT);assert(pad_pressed(1011,1)==PAD_RIGHT);
 pad_set_physical(PAD_UP);assert(pad_pressed(1012,1)==PAD_UP);assert(pad_held(1500,1)==PAD_UP);assert(!pad_pressed(1501,1));
 pad_set_physical(0);assert(!pad_held(1502,1));assert(!pad_pressed(1502,1));assert(pad_has_state());
 pad_set_physical(PAD_BOMB);pad_set_physical(0);assert(pad_pressed(1503,1)==PAD_BOMB);assert(!pad_pressed(1504,1));
 puts("PASS: PC/physical held OR; independent releases; PC lease expiry leaves physical held; physical short tap consumed once; released state remains explicit");
 return 0;
}
