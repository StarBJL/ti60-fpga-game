#include <stdio.h>
#include "gfx.c"
#include "th.h"
static unsigned publishes,waits,copies,fills;static Th th;
static void pub(void*c){(void)c;++publishes;}static void wait(void*c){(void)c;++waits;}
static void fill(void*c,int x,int y,int w,int h,Pixel p){(void)c;(void)x;(void)y;(void)w;(void)h;(void)p;++fills;}
static void copy(void*c,int x,int y,const Sprite*s,int sx,int sy,int w,int h,int k){(void)c;(void)x;(void)y;(void)s;(void)sx;(void)sy;(void)w;(void)h;(void)k;++copies;}
static void afill(void*c,int x,int y,int w,int h,Pixel p,int a){(void)a;fill(c,x,y,w,h,p);}
static void acopy(void*c,int x,int y,const Sprite*s,int sx,int sy,int w,int h,int k,int a){(void)a;copy(c,x,y,s,sx,sy,w,h,k);}
int main(void){Gfx g;GfxOps ops={0};ops.solid_fill=fill;ops.block_copy=copy;ops.alpha_fill=afill;ops.alpha_copy=acopy;ops.publish=pub;ops.wait_idle=wait;ops.add_fill=afill;ops.add_copy=acopy;
 th_set_view(1);th_art_init();th_init(&th,417);gfx_begin_backend(&g,&ops,0,1280,720);gfx_font_reset(&g);
 for(int f=0;f<12;f++){unsigned p=publishes,w=waits,e=font_epoch,c=copies;th_render(&th,&g,(ThRenderInfo){60,1,0,1});
 printf("frame=%d publish=%u waits=%u resets=%u copies=%u pool=%u\n",f,publishes-p,waits-w,font_epoch-e,copies-c,font_used);}
 return 0;}
