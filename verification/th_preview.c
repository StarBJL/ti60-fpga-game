#include "th.h"
#include <stdio.h>
#include <assert.h>
static Th game;static Pixel pixels[1280*720+32];
int main(void){Gfx gfx;ThRenderInfo info={60,0,0,1};FILE *f;int i;
 th_set_view(1);th_art_init();th_init(&game,417);
 for(i=1280*720;i<1280*720+32;i++)pixels[i]=0x5aa5;
 gfx_begin(&gfx,pixels,1280,720);th_render(&game,&gfx,info);gfx_end(&gfx);
 for(i=1280*720;i<1280*720+32;i++)assert(pixels[i]==0x5aa5);
 f=fopen("touhou_title.rgb565","wb");assert(f);fwrite(pixels,2,1280*720,f);fclose(f);
 th_start(&game,TH_NORMAL);for(i=0;i<600;i++)th_tick(&game,0,0,1);
 gfx_begin(&gfx,pixels,1280,720);th_render(&game,&gfx,info);gfx_end(&gfx);
 for(i=1280*720;i<1280*720+32;i++)assert(pixels[i]==0x5aa5);
 f=fopen("touhou_play.rgb565","wb");assert(f);fwrite(pixels,2,1280*720,f);fclose(f);
 puts("PASS native Touhou: translated frame and menu, 600 logic ticks and framebuffer guards");return 0;
}
