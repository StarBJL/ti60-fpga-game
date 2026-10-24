#include "th.h"
#include <string.h>
#ifdef __riscv
#define STRESS_ART __attribute__((section(".artbss"),aligned(64)))
#else
#define STRESS_ART __attribute__((aligned(64)))
#endif
#define STRESS_PIXELS (768u*1024u)
static Pixel scaled_pixels[STRESS_PIXELS] STRESS_ART;
Sprite th_stress_bspr[TBK_COUNT][64];
uint8_t th_stress_cx[TBK_COUNT][64],th_stress_cy[TBK_COUNT][64],th_stress_half[TBK_COUNT];
static int scale_percent=100;
int th_pressure_scale(void){return scale_percent;}
int th_pressure_scale_set(int percent){
    unsigned need=0,at=0;
    if(percent!=75&&percent!=100&&percent!=125&&percent!=150)return 0;
    for(int b=0;b<TBK_COUNT;b++)for(int k=0;k<th_bspr_n[b];k++){
        const Sprite *s=&th_bspr[b][k];unsigned w=(s->width*percent+50)/100,h=(s->height*percent+50)/100;
        if(!w)w=1;if(!h)h=1;need+=(w*h+31u)&~31u;
    }
    if(need>STRESS_PIXELS)return 0;
    for(int b=0;b<TBK_COUNT;b++){
        th_stress_half[b]=(th_bk_hv[b]*percent+99)/100+1;
        for(int k=0;k<th_bspr_n[b];k++){
            const Sprite *s=&th_bspr[b][k];int w=(s->width*percent+50)/100,h=(s->height*percent+50)/100;
            if(w<1)w=1;if(h<1)h=1;
            if(percent==100){th_stress_bspr[b][k]=*s;th_stress_cx[b][k]=th_bspr_cx[b][k];th_stress_cy[b][k]=th_bspr_cy[b][k];continue;}
            Pixel *dst=scaled_pixels+at;
            for(int y=0;y<h;y++)for(int x=0;x<w;x++)dst[y*w+x]=s->pixels[(y*s->height/h)*s->width+x*s->width/w];
            th_stress_bspr[b][k]=(Sprite){w,h,dst,0};
            th_stress_cx[b][k]=(th_bspr_cx[b][k]*percent+50)/100;
            th_stress_cy[b][k]=(th_bspr_cy[b][k]*percent+50)/100;
            at+=((unsigned)(w*h)+31u)&~31u;
        }
        if(percent==100)th_stress_half[b]=th_bk_hv[b];
    }
    scale_percent=percent;return 1;
}
