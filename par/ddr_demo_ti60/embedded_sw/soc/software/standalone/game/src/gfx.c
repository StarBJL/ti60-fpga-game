#include "gfx.h"
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
static uint32_t gfx_frame_no;                  /* 后端帧序号（字符串缓存判断"引擎还可能在读"，2026-10-31） */
void gfx_begin_backend(Gfx *g,const GfxOps *ops,void *context,int width,int height) {
    g->pixels=0; g->width=width; g->height=height; g->stride=width;
    g->ops=ops; g->context=context;
    g->fills=0; g->blits=0; g->pixels_written=0;
    g->alphas=0; g->alpha_pixels=0;
    ++gfx_frame_no;
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
void gfx_blit_table_clipped(Gfx *g,const Sprite *sprites,int nsprites,const GfxItem *it,int n,int mode) {
    int i,start=0;
    uint32_t pixels=0;
    if(n<=0) return;
    if(!g->ops || !g->ops->copy_table) {
        gfx_blit_list(g,sprites,it,n,mode);
        return;
    }
    for(i=0;i<n;i++) {
        const Sprite *s=&sprites[it[i].sprite];
        const int x=it[i].x,y=it[i].y,w=s->width,h=s->height;
        /* Subtraction avoids x+w overflow. TAB never receives a clipped item. */
        if(x>=0 && y>=0 && w>0 && h>0 && w<=g->width && h<=g->height &&
           x<=g->width-w && y<=g->height-h) {
            pixels+=(uint32_t)w*(uint32_t)h;
            continue;
        }
        /* Each slice aliases the caller's stable table. Keep painter's order,
         * and retain gfx_blit_table's per-call capability check / fallback. */
        if(i>start) gfx_blit_table(g,sprites,nsprites,it+start,i-start,mode,pixels);
        pixels=0;
        blit_one(g,x,y,s,mode);
        start=i+1;
    }
    if(n>start) gfx_blit_table(g,sprites,nsprites,it+start,n-start,mode,pixels);
}
void gfx_blit_table_alpha_clipped(Gfx *g,const Sprite *sprites,int nsprites,const GfxItem *it,int n,int mode,int alpha) {
    if(n<=0 || alpha<=0)return;
    if(alpha>=255){gfx_blit_table_clipped(g,sprites,nsprites,it,n,mode);return;}
    int start=0;uint32_t pixels=0;
    for(int i=0;i<n;i++){
        const Sprite *s=&sprites[it[i].sprite];int x=it[i].x,y=it[i].y,w=s->width,h=s->height;
        if(x>=0&&y>=0&&w>0&&h>0&&w<=g->width&&h<=g->height&&x<=g->width-w&&y<=g->height-h){pixels+=(uint32_t)w*(uint32_t)h;continue;}
        if(i>start)gfx_blit_table_alpha(g,sprites,nsprites,it+start,i-start,mode,alpha,pixels);
        if(mode&GFX_BLIT_ADD)gfx_blit_add(g,x,y,s,mode&GFX_BLIT_KEY,alpha);
        else gfx_blit_alpha(g,x,y,s,mode&GFX_BLIT_KEY,alpha);
        start=i+1;pixels=0;
    }
    if(n>start)gfx_blit_table_alpha(g,sprites,nsprites,it+start,n-start,mode,alpha,pixels);
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

/* 精灵的一部分半透明（2026-10-26，队友版：萝卜被弹幕盖住淡出时，脸只混合一次） */
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

/* V6 font rollback: retain the V5 ASCII renderer and current Chinese strings.
 * ASCII uses the original 5x7 keyed atlas/runs, not the A8 glyph path.
 * Language-switch and asynchronous cache-lifetime fixes are retained below. */
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
#define GFX_FONT_POOL_PIXELS (128u*1024u)   /* 256 KB: ~40 colour/scale atlases */
#endif
#define GFX_FONT_SLOTS 48

#if GFX_FONT_ATLAS
typedef struct { Pixel color; uint8_t scale; Sprite strip; } FontAtlas;
static Pixel     font_pool[GFX_FONT_POOL_PIXELS] __attribute__((aligned(64)));
static uint32_t  font_used;
static FontAtlas font_slot[GFX_FONT_SLOTS];
static int       font_slots;
static int       font_publish_pending;

/* CPU-created atlas data must also be published on its first hardware use. */
static void font_publish(Gfx *g) {
    if(font_publish_pending && g->ops && g->ops->publish) {
        g->ops->publish(g->context); font_publish_pending=0;
    }
}

static void ascii_font_reset(Gfx *g) {
    if(g && g->ops && g->ops->wait_idle) g->ops->wait_idle(g->context);
    font_used=0; font_slots=0; font_publish_pending=0;
}

static const Sprite *font_atlas(Gfx *g,int scale,Pixel color) {
    int i,idx,row,col,w,h;
    uint32_t need;
    Pixel *p;
    for(i=0;i<font_slots;i++)
        if(font_slot[i].color==color && font_slot[i].scale==scale) {
            font_publish(g); return &font_slot[i].strip;
        }
    w=FONT_GLYPHS*5*scale; h=7*scale;
    need=((uint32_t)(w*h)+31u)&~31u;                /* keep every strip 64-byte aligned */
    if(need>GFX_FONT_POOL_PIXELS) return 0;
    if(font_slots>=GFX_FONT_SLOTS || font_used+need>GFX_FONT_POOL_PIXELS) ascii_font_reset(g);
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
    font_publish_pending=1;
    font_publish(g);
    return &font_slot[font_slots++].strip;
}
#else
static void ascii_font_reset(Gfx *g) { (void)g; }
#endif

/* A language switch invalidates BOTH font caches after one completion wait.
 * panel_link.h already drains hardware before calling this with a null Gfx. */
void gfx_font_reset(Gfx *g) {
    if(g && g->ops && g->ops->wait_idle) g->ops->wait_idle(g->context);
    ascii_font_reset(0);
    gfx_cjk_reset(0);
}

/* ---------------------------------------------------------------------------
 * UI language + CJK glyphs (2026-10-28, see gfx.h). Data: ui_zh_data.h (tools/lang/gen_zh.py).
 * ------------------------------------------------------------------------- */
#include "ui_zh_data.h"
#ifdef __riscv
#define CJK_DDR __attribute__((section(".artbss"),aligned(64)))   /* DDR, not loaded, written before use */
#else
#define CJK_DDR __attribute__((aligned(64)))
#endif
int gfx_lang;
void gfx_set_lang(int lang) { gfx_lang=lang!=0; }

/* Dictionary strings are immutable. Do not cache only a caller's pointer:
 * dynamic labels may reuse the same address after a language/scene switch.
 * 2026-10-31：缓存的键改成字符串内容（哈希），命中时再 strcmp 校验一次，每帧不用再二分查词典。 */
#define TR_CACHE 512
static uint32_t tr_hash[TR_CACHE];
static const char *tr_en[TR_CACHE], *tr_zh[TR_CACHE];   /* tr_en = 0：词典里没有这串 */
const char *gfx_tr(const char *s) {
    int lo=0,hi=UI_ZH_ENTRIES-1;
    uint32_t h=2166136261u; unsigned k; const unsigned char *p;
    if(!s)return s;
    for(p=(const unsigned char *)s;*p;p++) h=(h^*p)*16777619u;
    k=(h*2654435761u)>>23;                                /* 9 bits */
    if(tr_hash[k]==h) {
        if(!tr_en[k]) return s;
        if(!strcmp(tr_en[k],s)) return tr_zh[k];
    }
    tr_hash[k]=h; tr_en[k]=0; tr_zh[k]=0;
    while(lo<=hi){int mid=(lo+hi)/2,c=strcmp(ui_zh_dict[mid].en,s);
        if(!c){tr_en[k]=ui_zh_dict[mid].en;tr_zh[k]=ui_zh_dict[mid].zh;return tr_zh[k];}
        if(c<0)lo=mid+1;else hi=mid-1;
    }
    return s;
}

const char *gfx_trx(const char *key,const char *english) {
    const char *t=gfx_tr(key);
    return t==key ? english : t;
}
/* decode one UTF-8 character (*p advanced); invalid bytes -> 0xFFFD */
static uint32_t utf8_next(const char **p) {
    const unsigned char *s=(const unsigned char *)*p;
    uint32_t c=*s++;
    if(c>=0xE0 && c<0xF0 && (s[0]&0xC0)==0x80 && (s[1]&0xC0)==0x80) { c=((c&15u)<<12)|((s[0]&63u)<<6)|(s[1]&63u); s+=2; }
    else if(c>=0xC0 && c<0xE0 && (s[0]&0xC0)==0x80) { c=((c&31u)<<6)|(s[0]&63u); s+=1; }
    else if(c>=0xF0) { while((*s&0xC0)==0x80) ++s; c=0xFFFD; }
    else if(c>=0x80) c=0xFFFD;
    *p=(const char *)s;
    return c;
}
/* glyph of cp for this text scale: the native size, else the biggest smaller size that has it.
 * Returns the set index (-1 = not in the font), *gi = glyph index; *adv = advance in pixels. */
static int cjk_find(uint32_t cp,int scale,int *gi,int *adv) {
    int si=scale-1;
    if(si<0) si=0;
    if(si>=UI_ZH_SIZES) si=UI_ZH_SIZES-1;
    *adv=ui_zh_set[si].px;
    for(;si>=0;si--) {
        const UiZhSet *z=&ui_zh_set[si];
        int lo=0,hi=(int)z->n-1;
        while(lo<=hi) {
            const int mid=(lo+hi)/2;
            if(z->codes[mid]==cp) { *gi=mid; *adv=cp<128?6*scale:z->px; return si; }   /* smaller fallback: its own advance */
            if(z->codes[mid]<cp) lo=mid+1; else hi=mid-1;
        }
    }
    return -1;
}
/* expanded A8 glyphs and flat colour planes (one per size and colour), both in DDR, built on first use */
#define CJK_A8_POOL   (768u*1024u)
#define CJK_PLANE_POOL (192u*1024u)                 /* pixels */
#define CJK_PLANES 96
static uint8_t  cjk_a8_pool[CJK_A8_POOL] CJK_DDR;
static Pixel    cjk_plane_pool[CJK_PLANE_POOL] CJK_DDR;
static uint8_t *cjk_a8[UI_ZH_GLYPHS];
static uint32_t cjk_a8_used,cjk_plane_used;
static struct { const Pixel *p; Pixel color; uint8_t px; } cjk_plane[CJK_PLANES];
static int cjk_planes,cjk_publish_pending;
void gfx_cjk_reset(Gfx *g) {
    if(g && g->ops && g->ops->wait_idle) g->ops->wait_idle(g->context);
    memset(cjk_a8,0,sizeof cjk_a8);
    cjk_a8_used=0; cjk_plane_used=0; cjk_planes=0; cjk_publish_pending=0;
}
static const uint8_t *cjk_glyph_a8(Gfx *g,int si,int gi,int *dirty) {
    const UiZhSet *z=&ui_zh_set[si];
    const uint32_t n=(uint32_t)z->px*z->px,bytes=(n+63u)&~63u;
    uint8_t **slot=&cjk_a8[z->base+gi],*d;
    const uint8_t *b;
    uint32_t i;
    if(*slot) return *slot;
    if(cjk_a8_used+bytes>CJK_A8_POOL) gfx_cjk_reset(g);
    d=cjk_a8_pool+cjk_a8_used; cjk_a8_used+=bytes;
    b=z->bits+(uint32_t)gi*((n+1u)/2u);
    for(i=0;i<n;i++) d[i]=(uint8_t)(((b[i>>1]>>((i&1u)*4u))&15u)*17u);
    *slot=d; *dirty=1;cjk_publish_pending=1;
    return d;
}
static const Pixel *cjk_color_plane(Gfx *g,int px,Pixel color,int *dirty) {
    int i;
    const uint32_t n=((uint32_t)px*(uint32_t)px+31u)&~31u;
    Pixel *p;
    for(i=0;i<cjk_planes;i++) if(cjk_plane[i].px==px && cjk_plane[i].color==color) return cjk_plane[i].p;
    if(cjk_planes>=CJK_PLANES || cjk_plane_used+n>CJK_PLANE_POOL) gfx_cjk_reset(g);
    p=cjk_plane_pool+cjk_plane_used; cjk_plane_used+=n;
    for(i=0;i<px*px;i++) p[i]=color;
    cjk_plane[cjk_planes].p=p; cjk_plane[cjk_planes].color=color; cjk_plane[cjk_planes].px=(uint8_t)px;
    ++cjk_planes; *dirty=1;cjk_publish_pending=1;
    return p;
}

/* Reserve both glyph and colour-plane storage before taking either pointer.
 * Otherwise allocating the second could reset/reuse the first while an
 * asynchronous blit still refers to it. Every reset fences the backend. */
static void text_reserve(Gfx *g,int si,int gi,Pixel color) {
    const UiZhSet *z=&ui_zh_set[si];
    uint32_t n=(uint32_t)z->px*z->px;
    int plane=0;
    for(int i=0;i<cjk_planes;i++)if(cjk_plane[i].px==z->px&&cjk_plane[i].color==color){plane=1;break;}
    if((!cjk_a8[z->base+gi] && cjk_a8_used+((n+63u)&~63u)>CJK_A8_POOL) ||
       (!plane && (cjk_planes>=CJK_PLANES || cjk_plane_used+((n+31u)&~31u)>CJK_PLANE_POOL)))gfx_cjk_reset(g);
}

/* ---------------------------------------------------------------------------
 * 字符串缓存（2026-10-31，gfx_text_cache）：BitBlt 后端把整串字先画进一张小图，内容 / 颜色 / 大小
 * 没变的帧只发一条拷贝，不再每个字一条命令（萝卜 HUD 每帧 200 多个字 -> 几十条）。
 * 纯 ASCII = 色键图；带中文 = A8 图 + 纯色平面（和逐字画时走同样的通路，像素不变）。
 * 小图放环形池，最近 LABEL_HOLD 帧用过的不覆盖（引擎可能还没读完）；腾不出来就这一次照旧逐字画。
 * ------------------------------------------------------------------------- */
#define LABEL_SETS 32u
#define LABEL_WAYS 4u
#define LABEL_MIN_LEN 3
#define LABEL_MAX_PIXELS 16384                  /* 也是最大的纯色平面 128 x 128 */
#define LABEL_HOLD 8u
#define LABEL_PIX_POOL (96u*1024u)              /* 像素 */
#define LABEL_A8_POOL  (192u*1024u)             /* 字节 */
typedef struct {
    uint32_t h1,h2,frame,off,size;
    uint16_t w,h;
    int16_t dy;                                 /* 小图顶边相对 y（中文字比 ASCII 高） */
    uint8_t a8,valid;
} TextLabel;
static TextLabel text_label[LABEL_SETS*LABEL_WAYS];
/* 放程序区 .bss（和 font_pool 一样，引擎照样能读）：.artbss 的 32 MB 已经满了（2026-10-31） */
static Pixel   label_pix[LABEL_PIX_POOL] __attribute__((aligned(64)));
static uint8_t label_a8[LABEL_A8_POOL] __attribute__((aligned(64)));
static uint32_t label_head[2];
static int text_cache_on;
void gfx_text_cache(int on) { text_cache_on=on; }

/* 在环形池里拿 n 个单元：还可能在用的小图（每帧都画的字一直在用）跳过去，转完一圈还放不下就放弃 */
static int label_take(int a8,uint32_t n,uint32_t *off) {
    const uint32_t cap=a8?LABEL_A8_POOL:LABEL_PIX_POOL;
    uint32_t o=label_head[a8],i;
    int wrapped=0;
    n=(n+15u)&~15u;
    if(n>cap) return 0;
    if(o+n>cap) { o=0; wrapped=1; }
    for(i=0;i<LABEL_SETS*LABEL_WAYS;i++) {
        const TextLabel *e=&text_label[i];
        if(e->valid && e->a8==a8 && e->off<o+n && o<e->off+e->size && gfx_frame_no-e->frame<LABEL_HOLD) {
            o=e->off+e->size;
            if(o+n>cap) { if(wrapped) return 0; o=0; wrapped=1; }
            i=(uint32_t)-1;                    /* 新位置从头再查 */
        }
    }
    for(i=0;i<LABEL_SETS*LABEL_WAYS;i++) {
        TextLabel *e=&text_label[i];
        if(e->valid && e->a8==a8 && e->off<o+n && o<e->off+e->size) e->valid=0;
    }
    *off=o; label_head[a8]=o+n;
    return 1;
}
/* 5x7 字形写进小图：a = 0 写颜色，否则写 A8 = 255 */
static void label_ascii(Pixel *p,uint8_t *a,int w,int x,int y,int idx,int scale,Pixel color) {
    int row,col,yy,xx;
    for(row=0;row<7;row++) {
        const unsigned char bits=glyph_bits(idx,row);
        for(col=0;col<5;col++) if(bits&(1u<<(4-col))) {
            const int o=(y+row*scale)*w+x+col*scale;
            for(yy=0;yy<scale;yy++) for(xx=0;xx<scale;xx++) {
                if(a) a[o+yy*w+xx]=255; else p[o+yy*w+xx]=color;
            }
        }
    }
}
/* 新建一张小图。返回 0 = 不缓存（调用者逐字画） */
static int label_build(TextLabel *e,const char *text,int scale,Pixel color,int cjk) {
    const int ch=7*scale;
    const char *s=text;
    int w=gfx_text_width(text,scale),top=0,bottom=ch,x=0,gi,adv,si;
    uint32_t off,n;
    if(w<=0) return 0;
    if(cjk) while(*s) {
        if((unsigned char)*s>=0x80) {
            uint32_t cp=utf8_next(&s);
            si=cjk_find(cp,scale,&gi,&adv);
            if(si<0) si=cjk_find('?',scale,&gi,&adv);
            if(si>=0) {
                const int px=ui_zh_set[si].px,yy=(ch-px)/2;
                if(yy<top) top=yy;
                if(yy+px>bottom) bottom=yy+px;
                if(x+px>w) w=x+px;
            }
            x+=adv;
        } else { x+=6*scale; ++s; }
    }
    n=(uint32_t)w*(uint32_t)(bottom-top);
    if((int)n>LABEL_MAX_PIXELS || !label_take(cjk,n,&off)) return 0;
    e->off=off; e->size=(n+15u)&~15u; e->w=(uint16_t)w; e->h=(uint16_t)(bottom-top); e->dy=(int16_t)top; e->a8=(uint8_t)cjk;
    if(!cjk) {
        Pixel *p=label_pix+off;
        uint32_t i;
        for(i=0;i<n;i++) p[i]=COLOR_KEY;
        for(;*text;x+=6*scale,++text) {
            const int idx=glyph_index(*text);
            if(idx>=0) label_ascii(p,0,w,x,0,idx,scale,color);
        }
    } else {
        uint8_t *a=label_a8+off;
        memset(a,0,n);
        x=0; s=text;
        while(*s) {
            if((unsigned char)*s>=0x80) {
                uint32_t cp=utf8_next(&s);
                si=cjk_find(cp,scale,&gi,&adv);
                if(si<0) si=cjk_find('?',scale,&gi,&adv);
                if(si>=0) {
                    const UiZhSet *z=&ui_zh_set[si];
                    const uint32_t px=z->px,np=px*px;
                    const uint8_t *b=z->bits+(uint32_t)gi*((np+1u)/2u);
                    uint8_t *d=a+((ch-(int)px)/2-top)*w+x;
                    uint32_t i=0,row,col;
                    for(row=0;row<px;row++,d+=w) for(col=0;col<px;col++,i++) {
                        const uint8_t v=(uint8_t)(((b[i>>1]>>((i&1u)*4u))&15u)*17u);
                        if(v>d[col]) d[col]=v;
                    }
                }
                x+=adv;
            } else {
                const int idx=glyph_index(*s++);
                if(idx>=0) label_ascii(0,a,w,x,-top,idx,scale,color);
                x+=6*scale;
            }
        }
    }
    return 1;
}
/* 用小图画这一串。返回 0 = 没画（调用者逐字画） */
static int label_text(Gfx *g,int x,int y,const char *text,int scale,Pixel color) {
    uint32_t h1=2166136261u^color^((uint32_t)scale<<16),h2=0x9E3779B1u+color,set,i;
    TextLabel *e=0,*victim=0;
    const unsigned char *s=(const unsigned char *)text;
    int cjk=0,fresh=0;
    Sprite spr;
    for(;*s;s++) {
        if(*s=='\n') return 0;
        if(*s>=0x80) cjk=1;
        h1=(h1^*s)*16777619u;
        h2=(h2+*s)*0x85EBCA6Bu; h2^=h2>>15;
    }
    if((const char *)s-text<LABEL_MIN_LEN || scale>GFX_FONT_ATLAS_MAX_SCALE || color==COLOR_KEY) return 0;
    set=(h1^(h1>>16))&(LABEL_SETS-1u);
    for(i=0;i<LABEL_WAYS;i++) {
        TextLabel *c=&text_label[set*LABEL_WAYS+i];
        if(c->valid && c->h1==h1 && c->h2==h2) { e=c; break; }
        if(!victim || (victim->valid && (!c->valid || (int32_t)(c->frame-victim->frame)<0))) victim=c;
    }
    if(!e) {
        if(victim->valid && gfx_frame_no-victim->frame<LABEL_HOLD) return 0;
        victim->valid=0;
        if(!label_build(victim,text,scale,color,cjk)) return 0;
        victim->h1=h1; victim->h2=h2; victim->valid=1;
        e=victim; fresh=1;
    }
    e->frame=gfx_frame_no;
    spr.width=e->w; spr.height=e->h;
    if(e->a8) {
        const int need=e->w*e->h,px=need<=32*32?32:need<=64*64?64:128;
        int dirty=0;
        spr.alpha=label_a8+e->off;
        spr.pixels=cjk_color_plane(g,px,color,&dirty);
        if((fresh||dirty) && g->ops->publish) g->ops->publish(g->context);
        gfx_blit_alpha(g,x,y+e->dy,&spr,0,255);
    } else {
        spr.alpha=0;
        spr.pixels=label_pix+e->off;
        if(fresh && g->ops->publish) g->ops->publish(g->context);
        gfx_blit_part(g,x,y,&spr,0,0,e->w,e->h,1);
    }
    return 1;
}

void gfx_text(Gfx *g,int x,int y,const char *text,int scale,Pixel color) {
    int start=x,cw,ch,dirty=0;
#if GFX_FONT_ATLAS
    const Sprite *atlas=0;
    int atlas_checked=0;
#endif
    if(scale<1 || !text)return;
    if(text_cache_on && g->ops && label_text(g,x,y,text,scale,color)) return;
    cw=5*scale; ch=7*scale;
    while(*text){
        if(*text=='\n'){x=start;y+=9*scale;++text;continue;}
        if((unsigned char)*text>=0x80){
            int gi=0,adv,si;uint32_t cp=utf8_next(&text);
            si=cjk_find(cp,scale,&gi,&adv);
            if(si<0){cp='?';si=cjk_find(cp,scale,&gi,&adv);}
            if(si>=0){
                int px=ui_zh_set[si].px,yy=y+(ch-px)/2;
                if(x<g->width&&yy<g->height&&x+px>0&&yy+px>0){
                    /* Allocate atomically with respect to cache resets: neither
                     * plane may invalidate the pointer returned for the other. */
                    text_reserve(g,si,gi,color);
                    const uint8_t *mask=cjk_glyph_a8(g,si,gi,&dirty);
                    const Pixel *plane=cjk_color_plane(g,px,color,&dirty);
                    Sprite spr={px,px,plane,mask};
                    if((dirty||cjk_publish_pending)&&g->ops&&g->ops->publish){
                        g->ops->publish(g->context);dirty=0;cjk_publish_pending=0;
                    }
                    gfx_blit_alpha(g,x,yy,&spr,0,255);
                }
            }
            x+=adv;
        }else{
            int idx=glyph_index(*text++);
            if(idx>=0&&x<g->width&&y<g->height&&x+cw>0&&y+ch>0){
#if GFX_FONT_ATLAS
                /* Chinese-only labels do not allocate an unused ASCII atlas. */
                if(!atlas_checked){
                    atlas_checked=1;
                    if(scale<=(g->ops?GFX_FONT_ATLAS_MAX_SCALE:GFX_FONT_ATLAS_CPU_MAX_SCALE)&&color!=COLOR_KEY)
                        atlas=font_atlas(g,scale,color);
                }
                if(atlas)gfx_blit_part(g,x,y,atlas,idx*cw,0,cw,ch,1);
                else
#endif
                    glyph_runs(g,x,y,idx,scale,color);
            }
            x+=6*scale;
        }
    }
}
int gfx_text_width(const char *text,int scale) {
    int w=0,max=0,last=0;
    if(scale<1||!text)return 0;
    while(*text){
        if(*text=='\n'){
            if(w-last>max)max=w-last;
            w=last=0;++text;continue;
        }
        if((unsigned char)*text>=0x80){
            int gi=0,adv;uint32_t cp=utf8_next(&text);
            if(cjk_find(cp,scale,&gi,&adv)<0){cp='?';cjk_find(cp,scale,&gi,&adv);}
            w+=adv;last=cp<128?scale:0;
        }else{w+=6*scale;last=scale;++text;}
    }
    return w-last>max?w-last:max;
}
/* V5's text baseline is 7*scale; CJK ink is centred on that same baseline. */
int gfx_text_height(int scale){return scale<1?0:7*scale;}
