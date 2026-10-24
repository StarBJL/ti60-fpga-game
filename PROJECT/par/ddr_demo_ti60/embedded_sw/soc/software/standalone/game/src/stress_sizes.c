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
    int fixed=percent==1||percent==3||percent==6;
    if(!fixed&&percent!=75&&percent!=100&&percent!=125&&percent!=150)return 0;
    for(int b=0;b<TBK_COUNT;b++)for(int k=0;k<th_bspr_n[b];k++){
        const Sprite *s=&th_bspr[b][k];unsigned w=fixed?percent:(s->width*percent+50)/100,h=fixed?percent:(s->height*percent+50)/100;
        if(!w)w=1;if(!h)h=1;need+=(w*h+31u)&~31u;
    }
    if(need>STRESS_PIXELS)return 0;
    for(int b=0;b<TBK_COUNT;b++){
        th_stress_half[b]=fixed?(percent+1)/2:(th_bk_hv[b]*percent+99)/100+1;
        for(int k=0;k<th_bspr_n[b];k++){
            const Sprite *s=&th_bspr[b][k];int w=fixed?percent:(s->width*percent+50)/100,h=fixed?percent:(s->height*percent+50)/100;
            if(w<1)w=1;if(h<1)h=1;
            if(percent==100){th_stress_bspr[b][k]=*s;th_stress_cx[b][k]=th_bspr_cx[b][k];th_stress_cy[b][k]=th_bspr_cy[b][k];continue;}
            Pixel *dst=scaled_pixels+at;
            int visible=0;
            for(int y=0;y<h;y++)for(int x=0;x<w;x++){
                int sx=fixed?(2*x+1)*s->width/(2*w):x*s->width/w,sy=fixed?(2*y+1)*s->height/(2*h):y*s->height/h;
                Pixel p=s->pixels[sy*s->width+sx];dst[y*w+x]=p;visible+=p!=COLOR_KEY;
            }
            if(fixed&&!visible){Pixel best=0;unsigned light=0;
                for(int i=0;i<s->width*s->height;i++){Pixel p=s->pixels[i];unsigned q=(p>>11)+(p>>5&63)+(p&31);
                    if(p!=COLOR_KEY&&q>light){best=p;light=q;}}
                dst[(h/2)*w+w/2]=best;
            }
            th_stress_bspr[b][k]=(Sprite){w,h,dst,0};
            th_stress_cx[b][k]=fixed?w/2:(th_bspr_cx[b][k]*percent+50)/100;
            th_stress_cy[b][k]=fixed?h/2:(th_bspr_cy[b][k]*percent+50)/100;
            at+=((unsigned)(w*h)+31u)&~31u;
        }
        if(percent==100)th_stress_half[b]=th_bk_hv[b];
    }
    scale_percent=percent;return 1;
}
