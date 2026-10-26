#include "gfx.h"
#include "zh_font_data.h"
#include <string.h>

/*
 * GFX_CPU_WORD_FILL (set in makefile, default 1):
 *   1 = CPU fill_rect writes 2 pixels per 32-bit store (half the bus transactions
 *       on the write-through D$ -> DDR path)
 *   0 = original per-pixel 16-bit store loop from sprout_run_final
 */
#ifndef GFX_CPU_WORD_FILL
#define GFX_CPU_WORD_FILL 1
#endif

void gfx_begin(Gfx *g, Pixel *pixels, int width, int height) {
    gfx_begin_stride(g,pixels,width,height,width);
}
void gfx_begin_stride(Gfx *g, Pixel *pixels, int width, int height, int stride) {
    g->pixels=pixels; g->width=width; g->height=height; g->stride=stride;
    g->ops=0; g->context=0;
    g->fills=0; g->blits=0; g->pixels_written=0;
    g->alphas=0; g->alpha_pixels=0;
}
void gfx_begin_backend(Gfx *g,const GfxOps *ops,void *context,int width,int height) {
    g->pixels=0; g->width=width; g->height=height; g->stride=width;
    g->ops=ops; g->context=context;
    g->fills=0; g->blits=0; g->pixels_written=0;
    g->alphas=0; g->alpha_pixels=0;
    if(ops && ops->begin_frame) ops->begin_frame(context);
}
void gfx_end(Gfx *g) {
    if(g->ops && g->ops->end_frame) g->ops->end_frame(g->context);
}

/*
w,h为宽和高
g为幕布
       x=0 1 2 3 4 5 6 7
y=0      · · · · · · · ·
y=1      · · R R R · · ·
y=2      · · R R R · · ·
y=3      · · · · · · · ·
*/
#if GFX_CPU_WORD_FILL
/*
保证4字节写入，内存对齐。
*/
static void fill_row(Pixel *p,int n,Pixel c) {
    uint32_t c2=((uint32_t)c<<16)|c;
    uint32_t *w;
    if(((uintptr_t)p&2u) && n>0) { *p++=c; --n; }
    w=(uint32_t *)p;
    while(n>=8) { w[0]=c2; w[1]=c2; w[2]=c2; w[3]=c2; w+=4; n-=8; }
    while(n>=2) { *w++=c2; n-=2; }
    if(n) *(Pixel *)w=c;
}
#endif
void gfx_fill_rect(Gfx *g,int x,int y,int w,int h,Pixel c) {
    int row,x2,y2;
#if !GFX_CPU_WORD_FILL
    int col;
#endif
    if(w<=0 || h<=0) return;
    x2=x+w; y2=y+h;
    if(x<0) x=0;
    if(y<0) y=0;
    if(x2>g->width) x2=g->width;
    if(y2>g->height) y2=g->height;
    if(x>=x2 || y>=y2) return;
    ++g->fills;
    if(g->ops) {
        if(g->ops->solid_fill) g->ops->solid_fill(g->context,x,y,x2-x,y2-y,c);
        g->pixels_written+=(uint32_t)((x2-x)*(y2-y));
        return;
    }
#if GFX_CPU_WORD_FILL
    for(row=y;row<y2;row++) fill_row(&g->pixels[row*g->stride+x],x2-x,c);
#else
    for(row=y;row<y2;row++) for(col=x;col<x2;col++) g->pixels[row*g->stride+col]=c;
#endif
    g->pixels_written+=(uint32_t)((x2-x)*(y2-y));
}
void gfx_blit_part(Gfx *g,int x,int y,const Sprite *s,int sx,int sy,int w,int h,int key_enabled) {
    int row,col;
    if(sx<0) { w+=sx; x-=sx; sx=0; }
    if(sy<0) { h+=sy; y-=sy; sy=0; }
    if(sx+w>s->width) w=s->width-sx;
    if(sy+h>s->height) h=s->height-sy;
    if(x<0) { sx-=x; w+=x; x=0; }
    if(y<0) { sy-=y; h+=y; y=0; }
    if(x+w>g->width) w=g->width-x;
    if(y+h>g->height) h=g->height-y;
    if(w<=0 || h<=0) return;
    ++g->blits;
    if(g->ops) {
        if(g->ops->block_copy)
            g->ops->block_copy(g->context,x,y,s,sx,sy,w,h,key_enabled);
        g->pixels_written+=(uint32_t)(w*h);
        return;
    }
    for(row=0;row<h;row++) {
        Pixel *dst=&g->pixels[(y+row)*g->stride+x];
        const Pixel *src=&s->pixels[(sy+row)*s->width+sx];
        if(!key_enabled) { memcpy(dst,src,(size_t)w*sizeof(Pixel)); g->pixels_written+=(uint32_t)w; }
        else for(col=0;col<w;col++) if(src[col]!=COLOR_KEY) { dst[col]=src[col]; ++g->pixels_written; }
    }
}
void gfx_blit(Gfx *g,int x,int y,const Sprite *s,int key_enabled) {
    gfx_blit_part(g,x,y,s,0,0,s->width,s->height,key_enabled);
}
/* one item of a batched call: mode = GFX_BLIT_KEY | GFX_BLIT_ADD. Batched draws
 * ignore the A8 plane (the engine's DRAW / TAB items have none), additive too. */
static void blit_one(Gfx *g,int x,int y,const Sprite *s,int mode) {
    if(mode&GFX_BLIT_ADD) { Sprite t=*s; t.alpha=0; gfx_blit_add(g,x,y,&t,mode&GFX_BLIT_KEY,255); }
    else                  gfx_blit(g,x,y,s,mode&GFX_BLIT_KEY);
}
/* counters of n items handed to the backend in one call (same as one call per item) */
static void count_batch(Gfx *g,int mode,int n,uint32_t px) {
    if(mode&GFX_BLIT_ADD) { g->alphas+=(uint32_t)n; g->alpha_pixels+=px; }
    else                  g->blits+=(uint32_t)n;
    g->pixels_written+=px;
}
void gfx_blit_list(Gfx *g,const Sprite *sprites,const GfxItem *it,int n,int key_enabled) {
    int i,start=0,W=g->width,H=g->height; uint32_t px=0,run=0;
    if(!g->ops || !g->ops->copy_list) {
        for(i=0;i<n;i++) blit_one(g,it[i].x,it[i].y,&sprites[it[i].sprite],key_enabled);
        return;
    }
    for(i=0;i<n;i++) {
        const Sprite *s=&sprites[it[i].sprite]; int x=it[i].x,y=it[i].y,w=s->width,h=s->height;
        if(x>=0 && y>=0 && w>0 && h>0 && x+w<=W && y+h<=H) { run+=(uint32_t)(w*h); continue; }
        /* needs clipping (or is off screen): flush the run, draw this one alone */
        if(i>start) { g->ops->copy_list(g->context,sprites,it+start,i-start,key_enabled); count_batch(g,key_enabled,i-start,run); }
        run=0;
        blit_one(g,x,y,s,key_enabled);
        start=i+1;
    }
    if(n>start) { g->ops->copy_list(g->context,sprites,it+start,n-start,key_enabled); count_batch(g,key_enabled,n-start,run); }
    (void)px;
}
/* 调用者保证全部在屏内：不检查，整表一次交给后端（见 gfx.h） */
void gfx_blit_list_inside(Gfx *g,const Sprite *sprites,const GfxItem *it,int n,int key_enabled,uint32_t pixels) {
    int i;
    if(n<=0) return;
    if(!g->ops || !g->ops->copy_list) {
        for(i=0;i<n;i++) blit_one(g,it[i].x,it[i].y,&sprites[it[i].sprite],key_enabled);
        return;
    }
    g->ops->copy_list(g->context,sprites,it,n,key_enabled);
    count_batch(g,key_enabled,n,pixels);
}
void gfx_blit_table_alpha(Gfx *g,const Sprite *sprites,int nsprites,const GfxItem *it,int n,int mode,int alpha,uint32_t pixels) {
    int i;
    if(n<=0 || alpha<=0) return;
    if(alpha>=255) { gfx_blit_table(g,sprites,nsprites,it,n,mode,pixels); return; }
    if(g->ops && g->ops->copy_table_ga && g->width<=4096 && g->height<=4096 &&
       g->ops->copy_table_ga(g->context,sprites,nsprites,it,n,mode,alpha)) {
        g->alphas+=(uint32_t)n; g->alpha_pixels+=pixels; g->pixels_written+=pixels;
        return;
    }
    for(i=0;i<n;i++) {
        const Sprite *s=&sprites[it[i].sprite];
        if(mode&GFX_BLIT_ADD) gfx_blit_add(g,it[i].x,it[i].y,s,(mode&GFX_BLIT_KEY)!=0,alpha);
        else                  gfx_blit_alpha(g,it[i].x,it[i].y,s,(mode&GFX_BLIT_KEY)!=0,alpha);
    }
}
int gfx_fill_table(Gfx *g,const GfxFillItem *it,int n,int add) {
    int i;
    if(n<=0) return 0;
    if(g->ops && g->ops->fill_table && g->width<=4096 && g->height<=4096 &&
       g->ops->fill_table(g->context,it,n,add)) return 1;
    for(i=0;i<n;i++) {                              /* 逐项（CPU 后端 / 没有 FTAB 的位流） */
        const uint32_t a=it[i].xyw,b=it[i].cha;
        const int x=(int)(a&0xFFFu),y=(int)((a>>12)&0xFFFu),w=(int)(a>>24),h=(int)((b>>16)&0xFFu),al=(int)(b>>24);
        const Pixel c=(Pixel)(b&0xFFFFu);
        if(!w || !h) continue;
        if(add) gfx_fill_add(g,x,y,w,h,c,al);
        else if(al==255) gfx_fill_rect(g,x,y,w,h,c);
        else gfx_fill_alpha(g,x,y,w,h,c,al);
    }
    return 0;
}
void gfx_blit_table(Gfx *g,const Sprite *sprites,int nsprites,const GfxItem *it,int n,int key_enabled,uint32_t pixels) {
    if(n<=0) return;
    /* 引擎表项的 x、y 只有 12 位：目标不超过 4096x4096 时（项都在目标内）才交给 copy_table */
    if(g->ops && g->ops->copy_table && g->width<=4096 && g->height<=4096 &&
       g->ops->copy_table(g->context,sprites,nsprites,it,n,key_enabled)) {
        count_batch(g,key_enabled,n,pixels);
        return;
    }
    gfx_blit_list_inside(g,sprites,it,n,key_enabled,pixels);
}
/* ---------------------------------------------------------------------------
 * Alpha blending (CPU reference implementation).
 *
 * RGB565 is spread into one 32-bit word as 00000GGGGGG00000RRRRR000000BBBBB
 * (mask 0x07E0F81F) so all three channels are blended with ONE multiply:
 *     out = (s*a + d*(32-a)) >> 5  ==  ((s-d)*a + (d<<5)) >> 5   (mod 2^32)
 * Each channel*32 still fits below the next field, so the result is exact.
 * The CPU uses 5-bit alpha (33 levels); the hardware engine can use all 8 bits.
 * ------------------------------------------------------------------------- */
#define GFX_SPREAD_MASK 0x07E0F81Fu
static inline uint32_t spread565(uint32_t c) { return (c|(c<<16))&GFX_SPREAD_MASK; }
static inline uint32_t pack565(uint32_t x) { x&=GFX_SPREAD_MASK; return (x|(x>>16))&0xFFFFu; }
static inline uint32_t blend_spread(uint32_t xs,uint32_t d,uint32_t a5) {
    uint32_t xd=spread565(d);
    /* +16 per field (0x02008010) rounds instead of truncating, so stacked
       translucent layers do not drift darker (and match the blitter's +128) */
    return pack565((((xs-xd)*a5)+(xd<<5)+0x02008010u)>>5);
}
static inline uint32_t alpha5(int a) { return a<=0 ? 0u : a>=255 ? 32u : (uint32_t)(a*33)>>8; }

void gfx_fill_alpha(Gfx *g,int x,int y,int w,int h,Pixel c,int alpha) {
    int row,x2,y2,n; uint32_t a5=alpha5(alpha),xs=spread565(c);
    if(w<=0 || h<=0 || !a5) return;
    if(a5>=32) { gfx_fill_rect(g,x,y,w,h,c); return; }
    x2=x+w; y2=y+h;
    if(x<0) x=0;
    if(y<0) y=0;
    if(x2>g->width) x2=g->width;
    if(y2>g->height) y2=g->height;
    if(x>=x2 || y>=y2) return;
    ++g->alphas;
    n=x2-x;
    g->alpha_pixels+=(uint32_t)(n*(y2-y));
    g->pixels_written+=(uint32_t)(n*(y2-y));
    if(g->ops) {
        if(g->ops->alpha_fill) g->ops->alpha_fill(g->context,x,y,n,y2-y,c,alpha);
        return;
    }
    for(row=y;row<y2;row++) {
        Pixel *p=&g->pixels[row*g->stride+x]; int k=n; uint32_t *wp;
        if(((uintptr_t)p&2u) && k>0) { *p=(Pixel)blend_spread(xs,*p,a5); ++p; --k; }
        wp=(uint32_t *)p;
        /* two pixels per 32-bit load/store: half the bus transactions */
        while(k>=2) {
            uint32_t v=*wp;
            *wp++=blend_spread(xs,v&0xFFFFu,a5)|(blend_spread(xs,v>>16,a5)<<16);
            k-=2;
        }
        if(k) { p=(Pixel *)wp; *p=(Pixel)blend_spread(xs,*p,a5); }
    }
}

void gfx_blit_part_alpha(Gfx *g,int x,int y,const Sprite *s,
    int sx,int sy,int w,int h,int key_enabled,int alpha) {
    int row,col;
    uint32_t ga5=alpha5(alpha),k16;
    if(!ga5) return;
    if(ga5>=32 && !s->alpha) { gfx_blit_part(g,x,y,s,sx,sy,w,h,key_enabled); return; }
    if(sx<0){x-=sx;w+=sx;sx=0;}
    if(sy<0){y-=sy;h+=sy;sy=0;}
    if(sx+w>s->width)w=s->width-sx;
    if(sy+h>s->height)h=s->height-sy;
    if(x<0) { sx-=x; w+=x; x=0; }
    if(y<0) { sy-=y; h+=y; y=0; }
    if(x+w>g->width) w=g->width-x;
    if(y+h>g->height) h=g->height-y;
    if(w<=0 || h<=0) return;
    ++g->alphas;
    g->alpha_pixels+=(uint32_t)(w*h);
    g->pixels_written+=(uint32_t)(w*h);
    if(g->ops) {
        if(g->ops->alpha_copy) g->ops->alpha_copy(g->context,x,y,s,sx,sy,w,h,key_enabled,alpha);
        return;
    }
    /* per-pixel: a5 = pa * global / 255 scaled to 0..32  ->  (pa*k16)>>16 */
    k16=(uint32_t)(alpha<0?0:alpha>255?255:alpha); k16=(k16+(k16>>7))*33u;
    for(row=0;row<h;row++) {
        Pixel *dst=&g->pixels[(y+row)*g->stride+x];
        const Pixel *src=&s->pixels[(sy+row)*s->width+sx];
        if(s->alpha) {
            const uint8_t *pa=&s->alpha[(sy+row)*s->width+sx];
            for(col=0;col<w;col++) {
                uint32_t a5=(pa[col]*k16)>>16;
                if(!a5) continue;
                if(key_enabled && src[col]==COLOR_KEY) continue;
                dst[col]=(Pixel)(a5>=32 ? src[col] : blend_spread(spread565(src[col]),dst[col],a5));
            }
        } else {
            for(col=0;col<w;col++) {
                if(key_enabled && src[col]==COLOR_KEY) continue;
                dst[col]=(Pixel)blend_spread(spread565(src[col]),dst[col],ga5);
            }
        }
    }
}
void gfx_blit_alpha(Gfx *g,int x,int y,const Sprite *s,int key_enabled,int alpha) {
    int sx=0,sy=0,w=s->width,h=s->height,row,col;
    uint32_t ga5=alpha5(alpha),k16;
    if(!ga5) return;
    if(ga5>=32 && !s->alpha) { gfx_blit(g,x,y,s,key_enabled); return; }
    if(x<0) { sx=-x; w+=x; x=0; }
    if(y<0) { sy=-y; h+=y; y=0; }
    if(x+w>g->width) w=g->width-x;
    if(y+h>g->height) h=g->height-y;
    if(w<=0 || h<=0) return;
    ++g->alphas;
    g->alpha_pixels+=(uint32_t)(w*h);
    g->pixels_written+=(uint32_t)(w*h);
    if(g->ops) {
        if(g->ops->alpha_copy) g->ops->alpha_copy(g->context,x,y,s,sx,sy,w,h,key_enabled,alpha);
        return;
    }
    /* per-pixel: a5 = pa * global / 255 scaled to 0..32  ->  (pa*k16)>>16 */
    k16=(uint32_t)(alpha<0?0:alpha>255?255:alpha); k16=(k16+(k16>>7))*33u;
    for(row=0;row<h;row++) {
        Pixel *dst=&g->pixels[(y+row)*g->stride+x];
        const Pixel *src=&s->pixels[(sy+row)*s->width+sx];
        if(s->alpha) {
            const uint8_t *pa=&s->alpha[(sy+row)*s->width+sx];
            for(col=0;col<w;col++) {
                uint32_t a5=(pa[col]*k16)>>16;
                if(!a5) continue;
                if(key_enabled && src[col]==COLOR_KEY) continue;
                dst[col]=(Pixel)(a5>=32 ? src[col] : blend_spread(spread565(src[col]),dst[col],a5));
            }
        } else {
            for(col=0;col<w;col++) {
                if(key_enabled && src[col]==COLOR_KEY) continue;
                dst[col]=(Pixel)blend_spread(spread565(src[col]),dst[col],ga5);
            }
        }
    }
}

/* ---------------------------------------------------------------------------
 * Additive blending (2026-10-11): same integer formula as the BitBlt engine's
 * ADD (CMD[6]): per channel dst + ((src * a9 + 128) >> 8), saturated, where
 * a9 = alpha + (alpha >> 7) (0..256). So both backends give identical pixels.
 * ------------------------------------------------------------------------- */
static inline uint32_t alpha9(int a) { a=a<0?0:a>255?255:a; return (uint32_t)a+((uint32_t)a>>7); }
static inline Pixel add565(Pixel d,Pixel s,uint32_t a9) {
    uint32_t r=((d>>11)&31u)+((((s>>11)&31u)*a9+128u)>>8);
    uint32_t gg=((d>>5)&63u)+((((s>>5)&63u)*a9+128u)>>8);
    uint32_t b=(d&31u)+(((s&31u)*a9+128u)>>8);
    if(r>31u) r=31u;
    if(gg>63u) gg=63u;
    if(b>31u) b=31u;
    return (Pixel)((r<<11)|(gg<<5)|b);
}

void gfx_fill_add(Gfx *g,int x,int y,int w,int h,Pixel c,int alpha) {
    int row,col,x2=x+w,y2=y+h; uint32_t a9=alpha9(alpha);
    if(w<=0 || h<=0) return;
    if(x<0) x=0;
    if(y<0) y=0;
    if(x2>g->width) x2=g->width;
    if(y2>g->height) y2=g->height;
    if(x>=x2 || y>=y2) return;
    ++g->alphas;
    g->alpha_pixels+=(uint32_t)((x2-x)*(y2-y));
    g->pixels_written+=(uint32_t)((x2-x)*(y2-y));
    if(g->ops) {
        if(g->ops->add_fill) g->ops->add_fill(g->context,x,y,x2-x,y2-y,c,alpha);
        return;
    }
    for(row=y;row<y2;row++) {
        Pixel *p=&g->pixels[row*g->stride];
        for(col=x;col<x2;col++) p[col]=add565(p[col],c,a9);
    }
}

void gfx_blit_add(Gfx *g,int x,int y,const Sprite *s,int key_enabled,int alpha) {
    int sx=0,sy=0,w=s->width,h=s->height,row,col; uint32_t ga9=alpha9(alpha);
    if(x<0) { sx=-x; w+=x; x=0; }
    if(y<0) { sy=-y; h+=y; y=0; }
    if(x+w>g->width) w=g->width-x;
    if(y+h>g->height) h=g->height-y;
    if(w<=0 || h<=0) return;
    ++g->alphas;
    g->alpha_pixels+=(uint32_t)(w*h);
    g->pixels_written+=(uint32_t)(w*h);
    if(g->ops) {
        if(g->ops->add_copy) g->ops->add_copy(g->context,x,y,s,sx,sy,w,h,key_enabled,alpha);
        return;
    }
    for(row=0;row<h;row++) {
        Pixel *dst=&g->pixels[(y+row)*g->stride+x];
        const Pixel *src=&s->pixels[(sy+row)*s->width+sx];
        const uint8_t *pa=s->alpha ? &s->alpha[(sy+row)*s->width+sx] : 0;
        for(col=0;col<w;col++) {
            uint32_t a9=ga9;
            if(key_enabled && src[col]==COLOR_KEY) continue;
            if(pa) { uint32_t e=(pa[col]*ga9)>>8; a9=e+(e>>7); }   /* A8 plane x global, like the engine */
            dst[col]=add565(dst[col],src[col],a9);
        }
    }
}

void gfx_border(Gfx *g,int x,int y,int w,int h,int t,Pixel c) {
    gfx_fill_rect(g,x,y,w,t,c); gfx_fill_rect(g,x,y+h-t,w,t,c);
    gfx_fill_rect(g,x,y,t,h,c); gfx_fill_rect(g,x+w-t,y,t,h,c);
}

/* 
Original 5x7 bitmap font: portable to ROM on FPGA. 
A-Z
*/

static const unsigned char letters[36][7]={
 {14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{14,17,16,16,16,17,14},
 {30,17,17,17,17,17,30},{31,16,16,30,16,16,31},{31,16,16,30,16,16,16},
 {14,17,16,23,17,17,15},{17,17,17,31,17,17,17},{31,4,4,4,4,4,31},
 {7,2,2,2,18,18,12},{17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
 {17,27,21,21,17,17,17},{17,25,21,19,17,17,17},{14,17,17,17,17,17,14},
 {30,17,17,30,16,16,16},{14,17,17,17,21,18,13},{30,17,17,30,20,18,17},
 {15,16,16,14,1,1,30},{31,4,4,4,4,4,4},{17,17,17,17,17,17,14},
 {17,17,17,17,17,10,4},{17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
 {17,17,10,4,4,4,4},{31,1,2,4,8,16,31},
 {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},
 {30,1,1,14,1,1,30},{2,6,10,18,31,2,2},{31,16,16,30,1,1,30},
 {14,16,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},
 {14,17,17,15,1,1,14}
};
/* Glyph index: A-Z 0..25, 0-9 26..35, then the symbols; -1 = blank (space...) */
#define FONT_GLYPHS 44
static int glyph_index(char ch) {
    static const char sym[]=":.-+/!%>";
    int i;
    if(ch>='a' && ch<='z') ch=(char)(ch-'a'+'A');
    if(ch>='A' && ch<='Z') return ch-'A';
    if(ch>='0' && ch<='9') return ch-'0'+26;
    for(i=0;sym[i];i++) if(sym[i]==ch) return 36+i;
    return -1;
}
static unsigned char glyph_bits(int idx,int row) {
    if(idx<36) return letters[idx][row];
    switch(idx) {
    case 36: return row==2||row==5 ? 4:0;                               /* : */
    case 37: return row==6 ? 4:0;                                       /* . */
    case 38: return row==3 ? 14:0;                                      /* - */
    case 39: return row==3 ? 14 : row==2||row==4 ? 4:0;                 /* + */
    case 40: return row==0 ? 1 : row<3 ? 2 : row<5 ? 4 : 8;             /* / */
    case 41: return row<4||row==6 ? 4:0;                                /* ! */
    case 42: { static const unsigned char p[]={25,25,2,4,8,19,19}; return p[row]; }  /* % */
    default: return row==1||row==5 ? 8 : row==2||row==4 ? 4 : row==3 ? 2:0;         /* > */
    }
}

/* One glyph without an atlas: each run of lit pixels is one fill, and equal
 * consecutive rows are merged into one taller fill ('I', 'H' stems...). */
static void glyph_runs(Gfx *g,int x,int y,int idx,int scale,Pixel color) {
    int row=0;
    while(row<7) {
        unsigned char bits=glyph_bits(idx,row);
        int rows=1,col=0;
        while(row+rows<7 && glyph_bits(idx,row+rows)==bits) ++rows;
        while(col<5) {
            int len=0;
            while(col<5 && !(bits&(1u<<(4-col)))) ++col;
            while(col+len<5 && (bits&(1u<<(4-col-len)))) ++len;
            if(len) gfx_fill_rect(g,x+col*scale,y+row*scale,len*scale,rows*scale,color);
            col+=len;
        }
        row+=rows;
    }
}

/* ---------------------------------------------------------------------------
 * Font atlas: all 44 glyphs pre-drawn in one colour at one scale on a
 * COLOR_KEY background, as one strip (glyph i at x = i*5*scale). A character
 * is then a single keyed copy of 5s x 7s pixels: on the BitBlt engine one
 * command (DST + SRC + CMD) instead of ~15 one-pixel FILLs, on the CPU one
 * clipped loop instead of ~15 gfx_fill_rect calls.
 * Atlases are built on first use per (colour, scale) into a static pool. When
 * the pool is full everything is dropped - after waiting for the backend, as
 * queued copies may still read the old atlases.
 * ------------------------------------------------------------------------- */
#ifndef GFX_FONT_ATLAS
#define GFX_FONT_ATLAS 1
#endif
#ifndef GFX_FONT_ATLAS_MAX_SCALE
#define GFX_FONT_ATLAS_MAX_SCALE 3      /* 6, 7 (titles) are rare: runs are fine */
#endif
#ifndef GFX_FONT_ATLAS_CPU_MAX_SCALE
/* CPU backend: a keyed copy tests all 35*s*s pixels of the cell, merged runs
 * only touch the lit ones - the atlas only wins at scale 1 there. */
#define GFX_FONT_ATLAS_CPU_MAX_SCALE 1
#endif
#ifndef GFX_FONT_POOL_PIXELS
#define GFX_FONT_POOL_PIXELS (512u*1024u)   /* 256 KB: ~40 colour/scale atlases */
#endif
#define GFX_FONT_SLOTS 48

#ifdef __riscv
#define UI_DDR __attribute__((section(".artbss"),aligned(64)))
#else
#define UI_DDR __attribute__((aligned(64)))
#endif
static GfxFontStats font_stats;
#if GFX_FONT_ATLAS
typedef struct { Pixel color; uint8_t scale; Sprite strip; } FontAtlas;
static Pixel     font_pool[GFX_FONT_POOL_PIXELS] UI_DDR;
static uint32_t  font_used;
GfxFontStats gfx_font_stats(void){font_stats.used_pixels=font_used;return font_stats;}
static FontAtlas font_slot[GFX_FONT_SLOTS];
static int       font_slots;

static uint32_t font_epoch;
#define ZH_CACHE_SLOTS 512
#define ZH_CACHE_WAYS 4
typedef struct { uint16_t code,color; uint8_t em; Sprite sprite; } ZhSlot;
static ZhSlot zh_slot[ZH_CACHE_SLOTS];
static uint8_t zh_victim[ZH_CACHE_SLOTS/ZH_CACHE_WAYS];

void gfx_font_reset(Gfx *g) {
    if(g && g->ops && g->ops->wait_idle) g->ops->wait_idle(g->context);
    font_used=0; font_slots=0;
    ++font_epoch;++font_stats.resets;
    memset(zh_slot,0,sizeof(zh_slot));
    memset(zh_victim,0,sizeof(zh_victim));
}

static const Sprite *font_atlas(Gfx *g,int scale,Pixel color) {
    int i,idx,row,col,w,h;
    uint32_t need;
    Pixel *p;
    for(i=0;i<font_slots;i++)
        if(font_slot[i].color==color && font_slot[i].scale==scale){++font_stats.hits;return &font_slot[i].strip;}
    ++font_stats.misses;
    w=FONT_GLYPHS*5*scale; h=7*scale;
    need=((uint32_t)(w*h)+31u)&~31u;                /* keep every strip 64-byte aligned */
    if(need>GFX_FONT_POOL_PIXELS) return 0;
    if(font_slots>=GFX_FONT_SLOTS || font_used+need>GFX_FONT_POOL_PIXELS) gfx_font_reset(g);
    p=&font_pool[font_used];
    for(i=0;i<w*h;i++) p[i]=COLOR_KEY;
    for(idx=0;idx<FONT_GLYPHS;idx++)
        for(row=0;row<7;row++) {
            unsigned char bits=glyph_bits(idx,row);
            for(col=0;col<5;col++) if(bits&(1u<<(4-col))) {
                int yy,xx;
                Pixel *q=p+(row*scale)*w+idx*5*scale+col*scale;
                for(yy=0;yy<scale;yy++) for(xx=0;xx<scale;xx++) q[yy*w+xx]=color;
            }
        }
    font_used+=need;
    font_slot[font_slots].color=color;
    font_slot[font_slots].scale=(uint8_t)scale;
    font_slot[font_slots].strip.width=w;
    font_slot[font_slots].strip.height=h;
    font_slot[font_slots].strip.pixels=p;
    font_slot[font_slots].strip.alpha=0;
    /* the engine reads the new strip from DDR: drain the CPU stores first */
    if(g->ops && g->ops->publish) g->ops->publish(g->context);
    return &font_slot[font_slots++].strip;
}
#else
void gfx_font_reset(Gfx *g) { (void)g; ++font_stats.resets; }
#endif

/* Strict UTF-8 decoder: an invalid/truncated byte advances once and draws a box. */
static unsigned text_codepoint(const char **cursor){
    const unsigned char *p=(const unsigned char *)*cursor;
    unsigned c=*p++;
    if(c>=0xc2&&c<=0xdf&&(*p&0xc0)==0x80){c=((c&31)<<6)|(*p++&63);}
    else if(c>=0xe0&&c<=0xef&&p[0]&&p[1]&&(p[0]&0xc0)==0x80&&(p[1]&0xc0)==0x80){
        unsigned n=((c&15)<<12)|((p[0]&63)<<6)|(p[1]&63);p+=2;
        c=n>=0x800&&(n<0xd800||n>0xdfff)?n:0xfffd;
    }else if(c>=128)c=0xfffd;
    *cursor=(const char *)p;return c;
}
#if !GFX_FONT_ATLAS
GfxFontStats gfx_font_stats(void){return font_stats;}
#endif
static int zh_index(unsigned c){
    int lo=0,hi=ZH_GLYPHS-1;
    while(lo<=hi){int m=(lo+hi)/2;if(zh_codes[m]==c)return m;if(zh_codes[m]<c)lo=m+1;else hi=m-1;}
    return -1;
}
static int zh_em(int scale){return scale==1?12:8*scale;}
static void zh_draw(Gfx *g,int x,int y,unsigned code,int scale,Pixel color){
    int em=zh_em(scale),index=zh_index(code),xx,yy;
    if(index<0){gfx_border(g,x,y,em,em,1,color);return;}
#if GFX_FONT_ATLAS
    if(g->ops&&color!=COLOR_KEY){
        unsigned hash=(code*2654435761u+(unsigned)color*97u+(unsigned)em*8191u)&(ZH_CACHE_SLOTS/ZH_CACHE_WAYS-1);
        int way;ZhSlot *slot=0;
        for(way=0;way<ZH_CACHE_WAYS;way++){
            ZhSlot *candidate=&zh_slot[hash*ZH_CACHE_WAYS+way];
            if(candidate->code==code&&candidate->color==color&&candidate->em==em){slot=candidate;break;}
            if(!slot&&!candidate->code)slot=candidate;
        }
        if(!slot){slot=&zh_slot[hash*ZH_CACHE_WAYS+zh_victim[hash]];zh_victim[hash]=(zh_victim[hash]+1)&(ZH_CACHE_WAYS-1);}
        if(slot->code!=code||slot->color!=color||slot->em!=em){
            ++font_stats.misses;
            unsigned need=((unsigned)(em*em)+31u)&~31u;
            Pixel *p;
            if(need>GFX_FONT_POOL_PIXELS)return;
            if(font_used+need>GFX_FONT_POOL_PIXELS)gfx_font_reset(g);
            p=&font_pool[font_used];font_used+=need;
            for(yy=0;yy<em;yy++)for(xx=0;xx<em;xx++)
                p[yy*em+xx]=(zh_bitmap[index][yy*12/em]&(1u<<(11-xx*12/em)))?color:COLOR_KEY;
            slot->code=(uint16_t)code;slot->color=color;slot->em=(uint8_t)em;
            slot->sprite.width=em;slot->sprite.height=em;slot->sprite.pixels=p;slot->sprite.alpha=0;
            if(g->ops->publish)g->ops->publish(g->context);
        }
        else ++font_stats.hits;
        gfx_blit(g,x,y,&slot->sprite,1);return;
    }
#endif
    /* Startup software sprites and CPU fallback: draw only horizontal ink runs. */
    for(yy=0;yy<em;yy++)for(xx=0;xx<em;){
        int first=xx;
        while(xx<em&&(zh_bitmap[index][yy*12/em]&(1u<<(11-xx*12/em))))++xx;
        if(xx>first)gfx_fill_rect(g,x+first,y+yy,xx-first,1,color);
        else ++xx;
    }
}

void gfx_text(Gfx *g,int x,int y,const char *text,int scale,Pixel color) {
    int start=x,cw=5*scale,ch=7*scale;
    const Sprite *atlas=0;
#if GFX_FONT_ATLAS
    uint32_t epoch=font_epoch;
#endif
    if(scale<1) return;
#if GFX_FONT_ATLAS
    /* Allocate ASCII atlases only when an ASCII glyph is actually present. */
#endif
    while(*text) {
        unsigned c=text_codepoint(&text);int idx;
        if(c=='\n') { x=start; y+=gfx_text_height(scale)+2*scale; continue; }
        if(c>=128){
            int em=zh_em(scale);
            if(x<g->width&&y<g->height&&x+em>0&&y+em>0)zh_draw(g,x,y,c,scale,color);
            x+=em+scale;continue;
        }
        /* A Chinese cache refill can reset the shared ASCII atlas pool. Reacquire
         * its descriptor before each ASCII glyph; no dangling/overwritten strip. */
#if GFX_FONT_ATLAS
        if(epoch!=font_epoch){
            atlas=0;epoch=font_epoch;
        }
#endif
        idx=glyph_index((char)c);
        if(idx>=0 && x<g->width && y<g->height && x+cw>0 && y+ch>0) {
#if GFX_FONT_ATLAS
            if(!atlas&&scale<=(g->ops?GFX_FONT_ATLAS_MAX_SCALE:GFX_FONT_ATLAS_CPU_MAX_SCALE)&&color!=COLOR_KEY)
                atlas=font_atlas(g,scale,color);
#endif
            int ay=y+(zh_em(scale)-ch)/2;
            if(atlas) gfx_blit_part(g,x,ay,atlas,idx*cw,0,cw,ch,1);
            else      glyph_runs(g,x,ay,idx,scale,color);
        }
        x+=6*scale;
    }
}
int gfx_text_width(const char *text,int scale) {
    int width=0,best=0;
    if(scale<1)return 0;
    while(*text){unsigned c=text_codepoint(&text);if(c=='\n'){if(width>best)best=width;width=0;}
        else width+=c>=128?zh_em(scale)+scale:6*scale;}
    if(width>best)best=width;
    return best>0?best-scale:0;
}

int gfx_text_height(int scale){return scale<1?0:zh_em(scale);}
#define LABEL_SLOTS 48
#define LABEL_PIXELS (192u*1024u)
typedef struct {char text[128];Pixel color;uint8_t scale;Sprite sprite;} StaticLabel;
static StaticLabel labels[LABEL_SLOTS];static unsigned label_count,label_used;
static Pixel label_pixels[LABEL_PIXELS] UI_DDR;
void gfx_label(Gfx *g,int x,int y,const char *text,int scale,Pixel color){
    if(scale<1)return;
    for(unsigned i=0;i<label_count;i++)if(labels[i].scale==scale&&labels[i].color==color&&!strcmp(labels[i].text,text)){
        ++font_stats.label_hits;gfx_blit(g,x,y,&labels[i].sprite,1);return;
    }
    int w=gfx_text_width(text,scale),h=gfx_text_height(scale);unsigned len=(unsigned)strlen(text),need=(unsigned)(w*h);
    if(w<=0||h<=0)return;
    if(len>=128||strchr(text,'\n')||label_count==LABEL_SLOTS||label_used+((need+31u)&~31u)>LABEL_PIXELS){gfx_text(g,x,y,text,scale,color);return;}
    StaticLabel *label=&labels[label_count++];strcpy(label->text,text);label->scale=scale;label->color=color;
    label->sprite=(Sprite){w,h,label_pixels+label_used,0};label_used+=(need+31u)&~31u;
    Gfx target;gfx_begin(&target,(Pixel*)label->sprite.pixels,w,h);gfx_fill_rect(&target,0,0,w,h,COLOR_KEY);
    int xx=0;const char *p=text;
    while(*p){unsigned c=text_codepoint(&p);if(c>=128){zh_draw(&target,xx,0,c,scale,color);xx+=zh_em(scale)+scale;}
        else {int idx=glyph_index((char)c);if(idx>=0)glyph_runs(&target,xx,(h-7*scale)/2,idx,scale,color);xx+=6*scale;}}
    ++font_stats.label_misses;if(g->ops&&g->ops->publish)g->ops->publish(g->context);gfx_blit(g,x,y,&label->sprite,1);
}
