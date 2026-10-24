#include "render.h"
#include "mini_snprintf.h"   /* SoC port: replaces <stdio.h> snprintf (no newlib stdio/malloc) */
#include <string.h>
/* Native R2 720p arena with R19 carrot art and gameplay. */
#undef SCREEN_W
#undef SCREEN_H
#define SCREEN_W 1280
#define SCREEN_H 720

#define INK RGB565(15,24,31)
#define PANEL RGB565(22,35,42)
#define EDGE RGB565(65,85,84)
#define CREAM RGB565(242,237,213)
#define MUTED RGB565(145,163,152)
#define GREEN RGB565(181,225,111)
#define GOLD RGB565(242,190,93)
#define RED RGB565(238,112,112)
#define CYAN RGB565(118,208,198)

/* 64 字节对齐：和帧缓冲同相位（模 16），整屏背景拷贝才能走引擎 128 位宽通路（C2） */
static Pixel background_pixels[SCREEN_W*SCREEN_H] __attribute__((aligned(64)));
static Sprite background={SCREEN_W,SCREEN_H,background_pixels,0};
static Pixel player_pixels[24*32], slime_pixels[24*26], runner_pixels[20*22];
static Pixel brute_pixels[36*38], elite_pixels[52*56], spitter_pixels[22*28], gem_pixels[9*11];
static Sprite player={24,32,player_pixels,0}, slime={24,26,slime_pixels,0};
static Sprite runner={20,22,runner_pixels,0}, brute={36,38,brute_pixels,0};
static Sprite elite={52,56,elite_pixels,0}, spitter={22,28,spitter_pixels,0}, gem={9,11,gem_pixels,0};
/* Alpha levels used by the translucent effects (0..255). */
#define A_DIM      150   /* full-screen dim behind menus / pause  */
#define A_PANEL    215   /* menu & stats panels                   */
#define A_PANEL_SH 110   /* panel drop shadow                     */
#define A_BANNER   180   /* event banner                          */
#define A_BOSSBAR  170   /* boss health bar backing               */
#define A_SHADOW   150   /* peak opacity of character shadows     */

/* Soft ground shadows: per-pixel alpha sprites (RGB565 plane + A8 plane). */
#define SHADOW_H     11
#define SHADOW_MAX_W 52
static const int shadow_radius[]={8,9,10,13,15,23};
#define SHADOW_COUNT ((int)(sizeof(shadow_radius)/sizeof(shadow_radius[0])))
static Pixel shadow_px[SHADOW_MAX_W*SHADOW_H];          /* one flat colour, shared */
static uint8_t shadow_alpha[SHADOW_COUNT][SHADOW_MAX_W*SHADOW_H];
static Sprite shadow_sprites[SHADOW_COUNT];

static int fx_alpha=1;   /* RenderInfo.alpha_fx of the frame being drawn */
/* Translucent fill when alpha effects are on, plain opaque fill when off. */
static void afill(Gfx *g,int x,int y,int w,int h,Pixel c,int a) {
    if(fx_alpha) gfx_fill_alpha(g,x,y,w,h,c,a); else gfx_fill_rect(g,x,y,w,h,c);
}

static Pixel hostile_pixels[4][9*9];
static Sprite hostile_sprites[4]={{9,9,hostile_pixels[0],0},
    {9,9,hostile_pixels[1],0},{9,9,hostile_pixels[2],0},{9,9,hostile_pixels[3],0}};

/* The normal game uses the same diamond/star/ring family as pressure mode,
 * with source-kind colours and visible widths 9 / 13 / 17. Fixed 17px canvas
 * keeps table generation and all render backends in agreement. */
static Pixel normal_seed_pixels[4][17*17];
static Sprite normal_seeds[4]={{17,17,normal_seed_pixels[0],0},{17,17,normal_seed_pixels[1],0},
    {17,17,normal_seed_pixels[2],0},{17,17,normal_seed_pixels[3],0}};
static void init_normal_seeds(void){
    static const int size[4]={9,9,17,13};
    int s,x,y;
    for(s=0;s<4;s++)for(y=0;y<17;y++)for(x=0;x<17;x++){
        int n=size[s],off=(17-n)/2;
        normal_seed_pixels[s][y*17+x]=x<off||y<off||x>=off+n||y>=off+n?COLOR_KEY:
            hostile_pixels[s?s:1][((y-off)*9/n)*9+(x-off)*9/n];
    }
}
#include "hero_evolution.h"

/* Eight yellow laser directions. One RGB565/keyed 19x19 sprite per shot;
 * all eight drawings are generated once at startup in DDR, not BRAM. */
#define LASER_SIZE 19
#define NATIVE_LASER_SIZE CARROT_LASER_SIZE
static Pixel laser_pixels[8][LASER_SIZE*LASER_SIZE];
static Sprite laser_sprites[8];
static void init_laser_sprites(void) {
    static const signed char ux[8]={ 1, 1, 0,-1,-1,-1, 0, 1};
    static const signed char uy[8]={ 0, 1, 1, 1, 0,-1,-1,-1};
    int dir,x,y;
    for(dir=0;dir<8;dir++) {
        int scale=ux[dir]*ux[dir]+uy[dir]*uy[dir];
        laser_sprites[dir].width=LASER_SIZE;
        laser_sprites[dir].height=LASER_SIZE;
        laser_sprites[dir].pixels=laser_pixels[dir];
        laser_sprites[dir].alpha=0;
        for(y=0;y<LASER_SIZE;y++) for(x=0;x<LASER_SIZE;x++) {
            int rx=x-LASER_SIZE/2,ry=y-LASER_SIZE/2;
            int along=rx*ux[dir]+ry*uy[dir];
            int across=rx*uy[dir]-ry*ux[dir];
            int width;
            Pixel color=COLOR_KEY;
            if(across<0) across=-across;
            width=(along<=-6*scale || along>=6*scale) ? scale/2 : 2*scale;
            if(along>=-8*scale && along<=7*scale && across<=width) {
                if(along>=5*scale) color=CREAM;                       /* tip */
                else if(across==0 && along>=-2*scale) color=RGB565(255,239,148);
                else if(along>=-3*scale) color=GOLD;                  /* shaft */
                else color=RGB565(156,112,49);                       /* tail */
            }
            laser_pixels[dir][y*LASER_SIZE+x]=color;
        }
    }
}

/* x: dark edge, b: body, h: highlight, w: bright center, .: transparent. */
static void init_hostile_sprites(void) {
    static const char patterns[4][9][10]={
        {"...xxx...","..xbbbx..",".xbbbbbx.","xbbhhhbbx","xbbhwhbbx",
         "xbbhhhbbx",".xbbbbbx.","..xbbbx..","...xxx..."},
        {"....x....","...xbx...","..xbbbx..",".xbbhbbx.","xbbhwhbbx",
         ".xbbhbbx.","..xbbbx..","...xbx...","....x...."},
        {"....x....",".x..b..x.","..xbbbx..","...bhb...","xbhhwhhbx",
         "...bhb...","..xbbbx..",".x..b..x.","....x...."},
        {"...xxx...","..xbbbx..",".xbbhbbx.","xbb...bbx","xbh.w.hbx",
         "xbb...bbx",".xbbhbbx.","..xbbbx..","...xxx..."}
    };
    static const Pixel body[4]={RGB565(240,103,84),RGB565(63,181,217),
        RGB565(239,106,135),RGB565(158,120,224)};
    static const Pixel light[4]={RGB565(255,182,99),RGB565(155,238,241),
        RGB565(255,186,154),RGB565(223,178,249)};
    int style,y,x;
    for(style=0;style<4;style++) for(y=0;y<9;y++) for(x=0;x<9;x++) {
        char mark=patterns[style][y][x];
        hostile_pixels[style][y*9+x]=mark=='x' ? INK : mark=='b' ? body[style] :
            mark=='h' ? light[style] : mark=='w' ? CREAM : COLOR_KEY;
    }
}

/* Ellipse w x SHADOW_H: solid core, alpha falls off linearly over the outer rim. */
#include "weapon_pose.h"
#include "hero_locator.h"
static void init_shadow_sprites(void) {
    int i,x,y;
    for(i=0;i<SHADOW_MAX_W*SHADOW_H;i++) shadow_px[i]=RGB565(6,14,12);
    for(i=0;i<SHADOW_COUNT;i++) {
        int w=shadow_radius[i]*2+6,h=SHADOW_H;
        if(w>SHADOW_MAX_W) w=SHADOW_MAX_W;
        for(y=0;y<h;y++) for(x=0;x<w;x++) {
            int dx=2*x+1-w,dy=2*y+1-h;                 /* half-pixel units */
            int d=dx*dx*256/(w*w)+dy*dy*256/(h*h);      /* 0 centre .. 256 rim */
            int a=d>=256 ? 0 : (256-d)*A_SHADOW*2/256;  /* soft outer half */
            shadow_alpha[i][y*w+x]=(uint8_t)(a>A_SHADOW ? A_SHADOW : a);
        }
        shadow_sprites[i].width=w; shadow_sprites[i].height=h;
        shadow_sprites[i].pixels=shadow_px; shadow_sprites[i].alpha=shadow_alpha[i];
    }
}

static void center(Gfx *g,int cx,int y,const char *s,int scale,Pixel c) {
    gfx_text(g,cx-gfx_text_width(s,scale)/2,y,s,scale,c);
}
/* Translucent panel: soft drop shadow + see-through body, opaque frame. */
static void panel(Gfx *g,int x,int y,int w,int h,Pixel edge) {
    afill(g,x+w,y+5,5,h,INK,A_PANEL_SH);
    afill(g,x+5,y+h,w-5,5,INK,A_PANEL_SH);
    afill(g,x+2,y+2,w-4,h-4,PANEL,A_PANEL);
    gfx_border(g,x,y,w,h,2,edge);
}
static void ellipse(Gfx *g,int x,int y,int w,int h,Pixel c) {
    int yy,xx,rx=w/2,ry=h/2;
    for(yy=0;yy<h;yy++) for(xx=0;xx<w;xx++) {
        int dx=xx-rx,dy=yy-ry;
        if(dx*dx*ry*ry+dy*dy*rx*rx<=rx*rx*ry*ry) gfx_fill_rect(g,x+xx,y+yy,1,1,c);
    }
}
static void sprout(Gfx *g,int x,int y,Pixel color) {
    gfx_fill_rect(g,x,y,2,8,color);
    gfx_fill_rect(g,x-5,y-2,5,3,color); gfx_fill_rect(g,x-3,y+1,4,2,color);
    gfx_fill_rect(g,x+2,y-4,5,3,color); gfx_fill_rect(g,x+1,y-1,4,2,color);
}

/* ---------------------------------------------------------------------------
 * Ground texture, generated once at boot (integer only, no FPU).
 * Two octaves of value noise give grass patches of different shade, a third
 * low-frequency noise carves bare-soil patches; per-pixel grain + 4x4 ordered
 * dither hide RGB565 banding. Then tufts, clover, pebbles and a few small
 * pale flowers are scattered on top (kept dull and tiny so they never look
 * like seeds or gems). Nothing here touches the game RNG.
 * ------------------------------------------------------------------------- */
#define ARENA_X0 20
#define ARENA_Y0 90
#define ARENA_X1 1260
#define ARENA_Y1 666
static uint32_t hash2(int x,int y,uint32_t seed) {
    uint32_t h=(uint32_t)x*374761393u+(uint32_t)y*668265263u+seed*2246822519u;
    h=(h^(h>>13))*1274126177u;
    return h^(h>>16);
}
/* smooth value noise 0..255, cell = 1<<shift pixels */
static int vnoise(int x,int y,int shift,uint32_t seed) {
    int cx=x>>shift,cy=y>>shift,m=(1<<shift)-1;
    int fx=((x&m)<<8)>>shift,fy=((y&m)<<8)>>shift,a,b,c,d,top,bot;
    fx=fx*fx*(768-2*fx)>>16; fy=fy*fy*(768-2*fy)>>16;   /* smoothstep, 0..255 */
    a=(int)(hash2(cx,cy,seed)&255u);   b=(int)(hash2(cx+1,cy,seed)&255u);
    c=(int)(hash2(cx,cy+1,seed)&255u); d=(int)(hash2(cx+1,cy+1,seed)&255u);
    top=a+((b-a)*fx>>8); bot=c+((d-c)*fx>>8);
    return top+((bot-top)*fy>>8);
}
static int clamp255(int v) { return v<0 ? 0 : v>255 ? 255 : v; }
static int mix(int a,int b,int t) { return a+(b-a)*t/256; }       /* t 0..256 */
static void ground(Gfx *g) {
    static const signed char bayer[16]={0,8,2,10,12,4,14,6,3,11,1,9,15,7,13,5};
    uint32_t seed=0x55667788u;
    int x,y,i;
    for(y=ARENA_Y0;y<ARENA_Y1;y++) for(x=ARENA_X0;x<ARENA_X1;x++) {
        int n=(vnoise(x,y,6,11u)*5+vnoise(x,y,4,23u)*3)>>3;       /* grass shade */
        int soil=vnoise(x,y,7,37u)-150;                          /* >0 = soil */
        int grain=(int)(hash2(x,y,51u)&15u)-8;
        int r=mix(30,50,n),gr=mix(50,78,n),b=mix(42,54,n),t,e,dth;
        if(soil>0) {                          /* bare soil, soft 24-step edge */
            t=soil*256/24; if(t>256) t=256;
            r=mix(r,76+(n>>4),t); gr=mix(gr,62+(n>>4),t); b=mix(b,46,t);
            grain=grain*3/2;
        }
        r+=grain; gr+=grain; b+=grain/2;
        /* darken towards the fence: 0 at 28 px inside, -18% at the edge */
        e=x-ARENA_X0; if(ARENA_X1-1-x<e) e=ARENA_X1-1-x;
        if(y-ARENA_Y0<e) e=y-ARENA_Y0;
        if(ARENA_Y1-1-y<e) e=ARENA_Y1-1-y;
        if(e<28) { t=256-(28-e)*46/28; r=r*t>>8; gr=gr*t>>8; b=b*t>>8; }
        dth=bayer[(y&3)*4+(x&3)];               /* ordered dither before 565 */
        r=clamp255(r+(dth>>1)); gr=clamp255(gr+(dth>>2)); b=clamp255(b+(dth>>1));
        g->pixels[y*g->stride+x]=RGB565(r,gr,b);
    }
    /* grass tufts: 3-5 blades, lighter tips; skipped on soil */
    for(i=0;i<700;i++) {
        int k,blades,lean;
        seed=seed*1664525u+1013904223u; x=ARENA_X0+8+(int)((seed>>8)%596u);
        seed=seed*1664525u+1013904223u; y=ARENA_Y0+10+(int)((seed>>8)%368u);
        if(vnoise(x,y,7,37u)>140) continue;
        blades=3+(int)(seed>>28)%3; lean=(int)(seed>>24)&1 ? 1 : -1;
        for(k=0;k<blades;k++) {
            int bx=x+k*2-blades,hgt=2+(int)((seed>>(k*3))&3u)+(k==blades/2);
            Pixel base=(seed>>(k+4))&1 ? RGB565(58,88,58) : RGB565(48,76,52);
            gfx_fill_rect(g,bx,y-hgt+1,1,hgt,base);
            gfx_fill_rect(g,bx+(k<blades/2?-lean:lean)*((hgt>3)?1:0),y-hgt,1,1,RGB565(96,128,76));
        }
        gfx_fill_rect(g,x-blades,y+1,blades*2,1,RGB565(24,40,34));   /* tuft root shade */
    }
    /* clover patches: tiny 3-leaf clusters */
    for(i=0;i<120;i++) {
        seed=seed*1664525u+1013904223u; x=ARENA_X0+10+(int)((seed>>8)%590u);
        seed=seed*1664525u+1013904223u; y=ARENA_Y0+10+(int)((seed>>8)%362u);
        if(vnoise(x,y,7,37u)>140) continue;
        gfx_fill_rect(g,x,y,2,2,RGB565(66,104,64)); gfx_fill_rect(g,x+2,y+1,2,2,RGB565(60,96,60));
        gfx_fill_rect(g,x+1,y+2,2,2,RGB565(72,112,68)); gfx_fill_rect(g,x+1,y+1,1,1,RGB565(98,134,80));
    }
    /* pebbles: mostly on soil */
    for(i=0;i<260;i++) {
        int w,h,on_soil;
        seed=seed*1664525u+1013904223u; x=ARENA_X0+6+(int)((seed>>8)%1224u);
        seed=seed*1664525u+1013904223u; y=ARENA_Y0+6+(int)((seed>>8)%562u);
        on_soil=vnoise(x,y,7,37u)>150;
        if(!on_soil && (seed>>29)) continue;               /* 1 in 8 on grass */
        w=2+(int)((seed>>20)&3u); h=2+(int)((seed>>22)&1u);
        gfx_fill_rect(g,x+1,y+h,w,1,RGB565(30,34,30));                 /* shadow */
        gfx_fill_rect(g,x,y,w,h,(seed>>24)&1 ? RGB565(104,98,84) : RGB565(92,90,80));
        gfx_fill_rect(g,x,y,w-1,1,RGB565(134,128,110));                /* highlight */
    }
    /* a few pale flowers, deliberately small and desaturated */
    for(i=0;i<45;i++) {
        Pixel petal;
        seed=seed*1664525u+1013904223u; x=ARENA_X0+12+(int)((seed>>8)%1212u);
        seed=seed*1664525u+1013904223u; y=ARENA_Y0+12+(int)((seed>>8)%550u);
        if(vnoise(x,y,7,37u)>140) continue;
        petal=(seed>>26)%3==0 ? RGB565(196,190,168) : (seed>>26)%3==1 ? RGB565(200,184,120) : RGB565(176,160,178);
        gfx_fill_rect(g,x,y+2,1,2,RGB565(50,80,52));                   /* stem */
        gfx_fill_rect(g,x-1,y,3,1,petal); gfx_fill_rect(g,x,y-1,1,3,petal);
        gfx_fill_rect(g,x,y,1,1,RGB565(214,170,80));
    }
}

static Pixel pressure_pixels[7][4][25*25] __attribute__((aligned(64)));
static Sprite pressure_sprites[7][4];
static const int pressure_sizes[7]={1,3,6,9,13,17,25};
static void init_pressure_sprites(void){
    for(int k=0;k<7;k++)for(int s=0;s<4;s++){
        int n=pressure_sizes[k];pressure_sprites[k][s]=(Sprite){n,n,pressure_pixels[k][s],0};
        for(int y=0;y<n;y++)for(int x=0;x<n;x++)
            pressure_pixels[k][s][y*n+x]=hostile_pixels[s][(n<9?((2*y+1)*9/(2*n)):y*9/n)*9+(n<9?((2*x+1)*9/(2*n)):x*9/n)];
        /* A 1px ring must remain visible even though its source centre is key. */
        if(n==1){Pixel best=0;unsigned light=0;
            for(int a=0;a<81;a++){Pixel p=hostile_pixels[s][a];unsigned q=(p>>11)+(p>>5&63)+(p&31);
                if(p!=COLOR_KEY&&q>light){best=p;light=q;}}
            pressure_pixels[k][s][0]=best;
        }
    }
}
int render_set_pressure_size(int pixels){
    int k;for(k=0;k<7&&pressure_sizes[k]!=pixels;k++);if(k==7)return 0;
    for(int s=0;s<4;s++)hostile_sprites[s]=pressure_sprites[k][s];
    init_player_locator();return 1;
}
static Pixel native_sprite_px[48000];static uint8_t native_sprite_a[48000];static unsigned native_used;
static void native_sprite(Sprite *sp,int w,int h){
    unsigned off=native_used;int x,y,sw=sp->width,sh=sp->height;
    if(off+(unsigned)(w*h)>48000u)return;
    for(y=0;y<h;y++)for(x=0;x<w;x++){
        int a=y*sh/h*sw+x*sw/w;native_sprite_px[off+y*w+x]=sp->pixels[a];
        if(sp->alpha)native_sprite_a[off+y*w+x]=sp->alpha[a];
    }
    sp->pixels=native_sprite_px+off;if(sp->alpha)sp->alpha=native_sprite_a+off;
    sp->width=w;sp->height=h;native_used+=w*h;
}
void render_init(void) {
    Gfx a; int x,y;
    gfx_begin(&a,background_pixels,SCREEN_W,SCREEN_H);
    gfx_fill_rect(&a,0,0,SCREEN_W,SCREEN_H,INK);
    ground(&a);
    gfx_border(&a,20,90,1240,576,2,RGB565(78,94,70));
    gfx_border(&a,24,94,1232,568,1,RGB565(28,43,39));
    for(x=16;x<1258;x+=32) {
        gfx_fill_rect(&a,x,90,2,6,RGB565(101,109,76));
        gfx_fill_rect(&a,x,660,2,6,RGB565(101,109,76));
    }
    for(y=110;y<662;y+=32) {
        gfx_fill_rect(&a,20,y,6,2,RGB565(101,109,76));
        gfx_fill_rect(&a,1254,y,6,2,RGB565(101,109,76));
    }
    gfx_begin(&a,slime_pixels,24,26); gfx_fill_rect(&a,0,0,24,26,COLOR_KEY);
    ellipse(&a,1,5,22,19,INK); ellipse(&a,3,6,18,16,RGB565(76,148,196));
    gfx_fill_rect(&a,5,8,8,3,RGB565(151,210,239));
    gfx_fill_rect(&a,4,21,6,3,INK); gfx_fill_rect(&a,15,21,6,3,INK);
    gfx_fill_rect(&a,7,12,3,4,INK); gfx_fill_rect(&a,15,12,3,4,INK);
    gfx_fill_rect(&a,8,12,1,1,CREAM); gfx_fill_rect(&a,16,12,1,1,CREAM);
    gfx_fill_rect(&a,10,18,5,2,RGB565(57,88,63));
    gfx_fill_rect(&a,8,2,2,5,RGB565(73,100,70));
    gfx_fill_rect(&a,13,0,4,5,RGB565(162,188,107));
    gfx_fill_rect(&a,10,3,4,3,RGB565(123,153,87));

    gfx_begin(&a,runner_pixels,20,22); gfx_fill_rect(&a,0,0,20,22,COLOR_KEY);
    ellipse(&a,1,3,18,17,INK); ellipse(&a,3,4,14,13,RGB565(76,169,204));
    gfx_fill_rect(&a,5,5,8,2,RGB565(151,225,241));
    gfx_fill_rect(&a,4,10,5,2,INK); gfx_fill_rect(&a,12,10,5,2,INK);
    gfx_fill_rect(&a,7,12,2,2,CREAM); gfx_fill_rect(&a,12,12,2,2,CREAM);
    gfx_fill_rect(&a,4,18,4,3,INK); gfx_fill_rect(&a,13,18,4,3,INK);
    gfx_fill_rect(&a,5,0,3,5,RGB565(107,126,73)); gfx_fill_rect(&a,10,1,5,3,RGB565(146,166,83));

    gfx_begin(&a,brute_pixels,36,38); gfx_fill_rect(&a,0,0,36,38,COLOR_KEY);
    ellipse(&a,1,6,34,29,INK); ellipse(&a,4,7,28,25,RGB565(133,84,183));
    ellipse(&a,5,8,22,18,RGB565(182,132,223));
    gfx_fill_rect(&a,5,3,5,9,RGB565(110,103,126)); gfx_fill_rect(&a,24,2,6,9,RGB565(110,103,126));
    gfx_fill_rect(&a,9,15,6,3,INK); gfx_fill_rect(&a,22,15,6,3,INK);
    gfx_fill_rect(&a,12,18,3,3,RED); gfx_fill_rect(&a,22,18,3,3,RED);
    gfx_fill_rect(&a,14,26,9,3,INK); gfx_fill_rect(&a,6,32,9,4,INK); gfx_fill_rect(&a,22,32,9,4,INK);

    gfx_begin(&a,elite_pixels,52,56); gfx_fill_rect(&a,0,0,52,56,COLOR_KEY);
    ellipse(&a,1,11,50,41,INK); ellipse(&a,4,12,44,36,RGB565(195,80,143));
    ellipse(&a,6,14,34,23,RGB565(238,136,184));
    gfx_fill_rect(&a,9,5,5,13,GOLD); gfx_fill_rect(&a,24,1,5,14,GOLD); gfx_fill_rect(&a,39,5,5,13,GOLD);
    gfx_fill_rect(&a,10,13,33,4,GOLD);
    gfx_fill_rect(&a,11,24,11,4,INK); gfx_fill_rect(&a,32,24,11,4,INK);
    gfx_fill_rect(&a,16,28,5,4,RED); gfx_fill_rect(&a,32,28,5,4,RED);
    gfx_fill_rect(&a,20,37,15,4,INK); gfx_fill_rect(&a,23,37,3,3,CREAM);
    gfx_fill_rect(&a,8,49,12,5,INK); gfx_fill_rect(&a,33,49,12,5,INK);

    gfx_begin(&a,spitter_pixels,22,28); gfx_fill_rect(&a,0,0,22,28,COLOR_KEY);
    ellipse(&a,1,8,20,16,INK); ellipse(&a,3,9,16,13,RGB565(64,132,168));
    gfx_fill_rect(&a,6,10,8,3,RGB565(148,214,220));
    gfx_fill_rect(&a,9,1,4,9,RGB565(42,88,108));
    gfx_fill_rect(&a,7,0,8,4,RGB565(186,214,96));
    gfx_fill_rect(&a,9,1,4,2,CREAM);
    gfx_fill_rect(&a,5,14,4,3,INK); gfx_fill_rect(&a,13,14,4,3,INK);
    gfx_fill_rect(&a,6,14,2,2,CREAM); gfx_fill_rect(&a,14,14,2,2,CREAM);
    gfx_fill_rect(&a,4,22,5,4,INK); gfx_fill_rect(&a,13,22,5,4,INK);

    gfx_begin(&a,gem_pixels,9,11); gfx_fill_rect(&a,0,0,9,11,COLOR_KEY);
    gfx_fill_rect(&a,3,0,3,2,INK); gfx_fill_rect(&a,1,2,7,6,INK); gfx_fill_rect(&a,3,8,3,2,INK);
    gfx_fill_rect(&a,3,2,3,6,CYAN); gfx_fill_rect(&a,2,3,5,3,CYAN);
    gfx_fill_rect(&a,3,2,2,2,CREAM);
    init_hostile_sprites();
    init_laser_sprites();
    init_normal_seeds();
    init_hero_evolution();
    init_shadow_sprites();
    native_used=0;
    {Sprite *actors[]={&player,&slime,&runner,&brute,&elite,&spitter,&gem};int i;
     for(i=0;i<7;i++)native_sprite(actors[i],actors[i]->width*3/2,actors[i]->height*3/2);
     /* Pressure sprites are selected from the dedicated immutable size bank. */
     for(i=0;i<4;i++)native_sprite(&normal_seeds[i],25,25);
     for(i=0;i<8;i++)native_sprite(&laser_sprites[i],NATIVE_LASER_SIZE,NATIVE_LASER_SIZE);
     for(i=0;i<SHADOW_COUNT;i++)native_sprite(&shadow_sprites[i],shadow_sprites[i].width*3/2,shadow_sprites[i].height*3/2);
    }
    init_pressure_sprites();
    render_set_pressure_size(game_seed_size());
}

static void shadow(Gfx *g,int x,int y,int radius) {
    int i=0; const Sprite *s;
    if(!fx_alpha) {                               /* original opaque shadow */
        gfx_fill_rect(g,x-radius,y-2,radius*2,5,RGB565(24,38,36));
        gfx_fill_rect(g,x-radius+3,y-4,radius*2-6,9,RGB565(24,38,36));
        return;
    }
    while(i<SHADOW_COUNT-1 && shadow_radius[i]<radius) ++i;
    s=&shadow_sprites[i];
    gfx_blit_alpha(g,x-s->width/2,y-s->height/2,s,0,GFX_OPAQUE);
}
static void draw_enemy(Gfx *r,const Enemy *e,int tick) {
    const Sprite *s=e->kind==4?&spitter:e->kind==3?&elite:e->kind==2?&brute:e->kind==1?&runner:&slime;
    int x=e->x/FIX,y=e->y/FIX,bob=((tick+e->id*7)/12)%2;
    if(e->spawn) {
        Pixel c=(e->spawn/6)%2 ? RED : RGB565(106,77,62);
        gfx_border(r,x-10,y-10,20,20,2,c);
        gfx_fill_rect(r,x-4,y-1,9,2,c); gfx_fill_rect(r,x,y-5,2,10,c); return;
    }
    shadow(r,x,y+8,game_enemy_radius(e->kind));
    if(e->kind==3 && e->attack_timer<=24) {
        int size=28+(24-e->attack_timer)/3;
        gfx_border(r,x-size,y-size,size*2,size*2,2,RED);
        gfx_fill_rect(r,x-4,y-8,8,8,GOLD);
    }
    if(e->kind==4 && e->attack_timer<=14) {
        gfx_border(r,x-20,y-18,28,28,2,CYAN);
        gfx_fill_rect(r,x-2,y-20,4,6,RGB565(186,214,96));
    }
    gfx_blit(r,x-s->width/2,y-s->height+12-bob,s,1);
    if(e->hit) {
        gfx_fill_rect(r,x-4,y-13,8,2,CREAM); gfx_fill_rect(r,x-1,y-16,2,8,CREAM);
    }
    if(e->kind>=2 && e->hp<e->max_hp) {
        int w=e->kind==3 ? 44 : 26;
        gfx_fill_rect(r,x-w/2,y-s->height+5,w,3,INK);
        gfx_fill_rect(r,x-w/2,y-s->height+5,w*e->hp/e->max_hp,2,e->kind==3?RED:e->kind==4?CYAN:GOLD);
    }
}
static void draw_player_gun(Gfx *r,const WeaponPose *p,int palette,int bob,int alpha){
    gfx_blit_alpha(r,p->x-CARROT_GUN_HALF,p->y-CARROT_GUN_HALF-bob,&gun_sprites[palette*32+p->heading],1,alpha);
}
static void draw_player(Gfx *r,const Game *g) {
    int x=g->x/FIX,y=g->y/FIX,bob=g->moving ? (g->ticks/7)%2 : 0;
    int level=game_appearance_level(g),palette=level<4?0:level<6?1:2;
    WeaponPose p[2]={weapon_pose(g,0,g->weapon_idle_ticks>30),weapon_pose(g,1,g->weapon_idle_ticks>30)};
    const Sprite *s=render_hero_asset(level,(g->ticks/9)&1);
    int alpha=255-loc_info.fade;
    /* Body and both turning guns are below projectiles. Face protection only
     * affects this actor pass, never bullets. Immunity has no clearing mask. */
    if(alpha){
    for(int k=0;k<2;k++)if(!p[k].front)draw_player_gun(r,&p[k],palette,bob,alpha);
    if(alpha==255)gfx_blit(r,x-s->width/2,y-CARROT_HERO_ANCHOR_Y-bob,s,1);
    else {
        /* Split around the protected face so it is blended exactly once. */
        int ox=x-30,oy=y-CARROT_HERO_ANCHOR_Y-bob;
        gfx_blit_part_alpha(r,ox,oy,s,0,0,60,21,1,alpha);
        gfx_blit_part_alpha(r,ox,oy+21,s,0,21,12,23,1,alpha);
        gfx_blit_part_alpha(r,ox+48,oy+21,s,48,21,12,23,1,alpha);
        gfx_blit_part_alpha(r,ox,oy+44,s,0,44,60,16,1,alpha);
    }
    for(int k=0;k<2;k++)if(p[k].front)draw_player_gun(r,&p[k],palette,bob,alpha);
    /* The actual keyed cheek/face pixels occlude a turning gun, never a box.
     * Front hands may overlap armour below, but cannot repaint eyes or mouth. */
    gfx_blit_part_alpha(r,x-18,y-12-bob,s,12,21,36,23,1,alpha);
    }
    if(g->muzzle&&g->weapon_idle_ticks<=30)for(int k=0;k<2;k++)if(g->weapon_flash_mask&(1<<k)){
        gfx_fill_rect(r,p[k].mx-1,p[k].my-1-bob,3,3,GOLD);
        gfx_fill_rect(r,p[k].mx,p[k].my-bob,1,1,CREAM);
    }
    if(g->invincible && (g->ticks/6)%2)
        gfx_fill_rect(r,x-2,y+13,5,2,GOLD);
}
static void draw_player_locator(Gfx *r,const Game *g){
    if(!loc_info.fade)return;
    int x=g->x/FIX,y=g->y/FIX,bob=g->moving?(g->ticks/7)%2:0;
    const Sprite *s=render_locator_outline_asset();
    /* The interior is COLOR_KEY: no body repaint, erasure or per-bullet test.
     * The union of body and live guns avoids edges cutting through the face. */
    if(loc_dirty&&r->ops&&r->ops->publish){r->ops->publish(r->context);loc_dirty=0;}
    gfx_blit_alpha(r,x-LOC_X,y-LOC_Y-bob,s,1,(fx_alpha?190:255)*loc_info.fade/255);
    /* Logical collision centre stays fixed when the artwork bobs by one pixel.
     * This locator indicates the centre; it does not redefine collision radii. */
    gfx_fill_alpha(r,x-2,y-2,5,5,CREAM,loc_info.fade);
    gfx_fill_alpha(r,x-1,y-1,3,3,RGB565(255,64,64),loc_info.fade);
}
/* 分段计时只在调试版（SPROUT_STATS=1）编译；发行版 clock_now() = 0，RSPLIT 什么也不存。 */
#if SPROUT_STATS
uint32_t (*render_clock)(void);
RenderSplit render_split;
static uint32_t clock_now(void) { return render_clock ? render_clock() : 0u; }
#define RSPLIT(f, v) (render_split.f = (v))
#else
#define clock_now() 0u
#define RSPLIT(f, v) ((void)(v))
#endif

/* Hostile seeds as a batched draw list, SEED_CHUNK items per gfx_blit_list call
 * (1 KB, stays in the D$ while the backend reads it back). */
#define SEED_CHUNK 128
static GfxItem seed_list[SEED_CHUNK];
#if SPROUT_STATS
int render_seed_table=1;
#endif

static int draw_seeds(Gfx *r,const Game *g,int batch) {
    const Sprite *seeds=g->limit_mode?hostile_sprites:normal_seeds;
    int half=game_seed_half(g),size=game_seed_extent(g);
    int nh=game_hostile_used(g),i,k=0,drawn=0; const Bullet *hp=nh?game_hostile_at(g,0):0;  /* contiguous pool, slots >= nh are free */
    if(!batch) {
        for(i=0;i<nh;i++) if(hp[i].active) {
            const Bullet *b=&hp[i]; int x=b->x/FIX,y=b->y/FIX;
            int style=b->style>=0 && b->style<4 ? b->style : 0;
            gfx_blit(r,x-half,y-half,&seeds[style],1);
            ++drawn;
        }
        return drawn;
    }
    if(render_seed_table) {             /* table from game_tick: no second pool walk, no copying */
        const GfxItem *items; int n=game_seed_draw(g,&items);
        if(n>=0) {
            /* 整表都在屏内（game_tick 已判断）：不再逐项检查，整表一次提交 */
            /* 引擎读表（TAB，做不了时自动退回 gfx_blit_list_inside）：表是 game 的双缓冲，
             * 本帧引擎画完前（blit_wait 前）最多再跑一次 game_tick，写的是另一张 */
            if(n && !game_seed_draw_clip(g)) gfx_blit_table(r,seeds,GAME_SEED_STYLES,items,n,1,(uint32_t)n*(uint32_t)size*(uint32_t)size);
            else if(n) gfx_blit_list(r,seeds,items,n,1);
            return n;
        }
    }
    for(i=0;i<nh;i++) if(hp[i].active) {
        const Bullet *b=&hp[i]; int x=b->x/FIX-half,y=b->y/FIX-half;
        if(x<=-size || y<=-size || x>=SCREEN_W || y>=SCREEN_H) continue;   /* off screen: gfx_blit draws nothing either */
        seed_list[k].x=(int16_t)x; seed_list[k].y=(int16_t)y;
        seed_list[k].sprite=b->style>=0 && b->style<4 ? b->style : 0;
        ++drawn;
        if(++k==SEED_CHUNK) { gfx_blit_list(r,seeds,seed_list,k,1); k=0; }
    }
    if(k) gfx_blit_list(r,seeds,seed_list,k,1);
    return drawn;
}

#if SPROUT_STATS
/* Seed submit anatomy (diagnostic, see render.h). */
static GfxItem prof_list[SEED_CHUNK];
static int prof_k,prof_n;
static volatile int prof_sink;
static Sprite tiny_sprites[4];

int render_seed_part(Gfx *r,const Game *g,int part) {
    int half=game_seed_half(g),size=game_seed_extent(g);
    int nh=game_hostile_used(g),i,k=0,n=0,sum=0; const Bullet *hp=nh?game_hostile_at(g,0):0;
    switch(part) {
    case SEED_PART_PREP:            /* first 128 on-screen seeds -> prof_list, total count -> prof_n */
        for(i=0;i<4;i++) {
            tiny_sprites[i].width=1; tiny_sprites[i].height=1;
            tiny_sprites[i].pixels=g->limit_mode?&hostile_pixels[i][4*9+4]:&normal_seed_pixels[i][8*17+8]; tiny_sprites[i].alpha=0;
        }
        for(i=0;i<nh;i++) if(hp[i].active) {
            const Bullet *b=&hp[i]; int x=b->x/FIX-half,y=b->y/FIX-half;
            if(x<=-size || y<=-size || x>=SCREEN_W || y>=SCREEN_H) continue;
            if(k<SEED_CHUNK) {
                prof_list[k].x=(int16_t)x; prof_list[k].y=(int16_t)y;
                prof_list[k].sprite=b->style>=0 && b->style<4 ? b->style : 0; ++k;
            }
            ++n;
        }
        prof_k=k; prof_n=n;
        return nh;
    case SEED_PART_WALK:            /* pool walk + on-screen test, no stores */
        for(i=0;i<nh;i++) if(hp[i].active) {
            const Bullet *b=&hp[i]; int x=b->x/FIX-half,y=b->y/FIX-half;
            if(x<=-size || y<=-size || x>=SCREEN_W || y>=SCREEN_H) continue;
            sum+=x+y+b->style; ++n;
        }
        prof_sink=sum;
        return n;
    case SEED_PART_BUILD:           /* walk + fill seed_list, nothing submitted */
        for(i=0;i<nh;i++) if(hp[i].active) {
            const Bullet *b=&hp[i]; int x=b->x/FIX-half,y=b->y/FIX-half;
            if(x<=-size || y<=-size || x>=SCREEN_W || y>=SCREEN_H) continue;
            seed_list[k].x=(int16_t)x; seed_list[k].y=(int16_t)y;
            seed_list[k].sprite=b->style>=0 && b->style<4 ? b->style : 0;
            ++n;
            if(++k==SEED_CHUNK) k=0;
        }
        prof_sink=seed_list[0].x;
        return n;
    case SEED_PART_SUBMIT:          /* prebuilt list, same total count, no pool reads */
    case SEED_PART_TINY:
        if(!prof_k) return 0;
        while(n<prof_n) {
            k=prof_n-n<prof_k ? prof_n-n : prof_k;
            gfx_blit_list(r,part==SEED_PART_TINY?tiny_sprites:(g->limit_mode?hostile_sprites:normal_seeds),prof_list,k,1);
            n+=k;
        }
        return n;
    case SEED_PART_FULL:
        return draw_seeds(r,g,1);
    case SEED_PART_BG:
        gfx_blit(r,0,0,&background,0);
        return 1;
    }
    return 0;
}
#endif

/* Enemy draw order: by y, equal y by slot index (= the original stable
 * insertion sort over slot order). Writes order[], ys[] (y of each), returns n.
 * render_sort_keys = 1: the y keys are copied once into ys[] (one pass over the
 * enemy array) and the sort starts from last frame's order, so it is nearly
 * sorted and touches only the 1.5 KB key arrays; (y, slot) is a total order,
 * so the result is the same as the original whatever the start order.
 * render_sort_keys = 0: the original, which reads enemies[].y (44 B structs,
 * 8.4 KB > D$) in every compare step of an O(n^2) sort. */
#if SPROUT_STATS
int render_sort_keys=1;
#endif
static uint8_t sort_prev[MAX_ENEMIES],sort_seen[MAX_ENEMIES];
static int sort_prev_n;

static int enemy_order(const Game *g,int *order,int *ys) {
    int i,j,n=0;
    if(!render_sort_keys) {
        for(i=0;i<MAX_ENEMIES;i++) if(g->enemies[i].active) order[n++]=i;
        for(i=1;i<n;i++) {
            int key=order[i]; j=i-1;
            while(j>=0 && g->enemies[order[j]].y>g->enemies[key].y) { order[j+1]=order[j]; --j; }
            order[j+1]=key;
        }
        for(i=0;i<n;i++) ys[i]=g->enemies[order[i]].y;
        sort_prev_n=0;                              /* next keyed sort starts fresh */
        return n;
    }
    {
        int y_of[MAX_ENEMIES];
        for(i=0;i<MAX_ENEMIES;i++) {               /* one sequential pass over the structs */
            const Enemy *e=&g->enemies[i];
            sort_seen[i]=(uint8_t)(e->active!=0);
            y_of[i]=e->y;
        }
        for(i=0;i<sort_prev_n;i++) {               /* last frame's order, still active */
            int k=sort_prev[i];
            if(sort_seen[k]==1) { sort_seen[k]=2; order[n]=k; ys[n++]=y_of[k]; }
        }
        for(i=0;i<MAX_ENEMIES;i++)                  /* newly active slots */
            if(sort_seen[i]==1) { order[n]=i; ys[n++]=y_of[i]; }
    }
    for(i=1;i<n;i++) {
        int key=order[i],ky=ys[i]; j=i-1;
        while(j>=0 && (ys[j]>ky || (ys[j]==ky && order[j]>key))) { order[j+1]=order[j]; ys[j+1]=ys[j]; --j; }
        order[j+1]=key; ys[j+1]=ky;
    }
    for(i=0;i<n;i++) sort_prev[i]=(uint8_t)order[i];
    sort_prev_n=n;
    return n;
}

static void draw_world(Gfx *r,const Game *g,int batch) {
    uint32_t t,t1;
    int i,n,order[MAX_ENEMIES],ys[MAX_ENEMIES];
    t=clock_now();
    gfx_blit(r,0,0,&background,0);
    for(i=0;i<MAX_GEMS;i++) if(g->gems[i].active) {
        gfx_blit(r,g->gems[i].x/FIX-4,g->gems[i].y/FIX-5,&gem,1);
    }
    t1=clock_now(); RSPLIT(bg,t1-t); t=t1;
    n=enemy_order(g,order,ys);
    t1=clock_now(); RSPLIT(sort,t1-t); t=t1;
    prepare_player_locator(g);
    t1=clock_now(); RSPLIT(locator,t1-t); t=t1;
    shadow(r,g->x/FIX,g->y/FIX+21,14);
    for(i=0;i<n;i++)draw_enemy(r,&g->enemies[order[i]],g->ticks);
    draw_player(r,g);
    t1=clock_now(); RSPLIT(enemies,t1-t); t=t1;
    for(i=0;i<MAX_BULLETS;i++) if(g->bullets[i].active) {
        const Bullet *b=&g->bullets[i]; int x=b->x/FIX,y=b->y/FIX;
        gfx_blit(r,x-NATIVE_LASER_SIZE/2,y-NATIVE_LASER_SIZE/2,&laser_sprites[(unsigned)b->style<8u?b->style:0],1);
    }
    t1=clock_now(); RSPLIT(pbullets,t1-t); t=t1;
    RSPLIT(seeds_n,(uint32_t)draw_seeds(r,g,batch));
    t1=clock_now(); RSPLIT(seeds,t1-t); t=t1;
    for(i=0;i<MAX_PARTICLES;i++) if(g->particles[i].life) {
        const Particle *p=&g->particles[i]; Pixel c=p->color==1?RED:p->color==2?GREEN:GOLD;
        gfx_fill_rect(r,p->x/FIX,p->y/FIX,p->size,p->size,c);
    }
    draw_player_locator(r,g);
    if(g->hurt_flash) gfx_border(r,20,90,1240,576,3,RED);
    RSPLIT(particles,clock_now()-t);
}

/* Native 1280x720 UI. World sprites remain proportionate; no framebuffer stretch. */
static void hud(Gfx *r,const Game *g,RenderInfo info) {
    char t[160];int sec=g->ticks/60,budget=(int)((int64_t)info.work_us*100/16667);
    gfx_fill_rect(r,0,0,1280,87,INK);
    gfx_text(r,24,12,"萝卜求生",3,CREAM);
    if(g->limit_mode)snprintf(t,sizeof t,"%s 敌%d 弹%d/%d",g->stress_tier==STRESS_CUSTOM?"自定义":g->stress_tier==1?"低档":g->stress_tier==2?"中档":"高档",g->enemy_count,game_hostile_count(g),game_hostile_slots(g));
    else snprintf(t,sizeof t,"无尽模式 怪物 %d",g->enemy_count);
    gfx_text(r,24,53,t,1,g->limit_mode?GOLD:MUTED);
    gfx_text(r,360,12,"生命",2,MUTED);snprintf(t,sizeof t,"%d/%d",g->hp,g->max_hp);gfx_text(r,515,12,t,2,CREAM);
    gfx_fill_rect(r,360,37,270,15,EDGE);gfx_fill_rect(r,362,39,266*g->hp/g->max_hp,11,g->hp<30?RED:GREEN);
    snprintf(t,sizeof t,"经验 %d/%d",g->xp,g->next_xp);gfx_text(r,360,62,t,1,CYAN);
    gfx_text(r,710,12,g->limit_mode?"耗时 毫秒":"存活时间",2,MUTED);
    if(g->limit_mode)snprintf(t,sizeof t,"%d.%02d",info.work_us/1000,(info.work_us%1000)/10);
    else snprintf(t,sizeof t,"%02d:%02d",sec/60,sec%60);
    gfx_text(r,710,41,t,3,CREAM);
    gfx_text(r,930,12,g->limit_mode?"预算":"击败",2,MUTED);
    if(g->limit_mode)snprintf(t,sizeof t,"%d%%",budget);else snprintf(t,sizeof t,"%03d",g->kills);
    gfx_text(r,930,41,t,3,budget>=100&&g->limit_mode?RED:GOLD);
    snprintf(t,sizeof t,"等级 %02d",g->level);gfx_text(r,1120,14,t,2,CYAN);
    snprintf(t,sizeof t,"%s %d秒",sec%60<30?"敌潮":"首领",30-sec%30);gfx_text(r,1120,51,t,1,MUTED);
    gfx_fill_rect(r,22,80,1236,3,EDGE);gfx_fill_rect(r,22,80,1236*g->xp/g->next_xp,3,CYAN);
    gfx_fill_rect(r,0,672,1280,48,INK);
    gfx_text(r,24,682,"WASD 移动  P 暂停  M 菜单",1,MUTED);
    if(g->skill_cooldown)snprintf(t,sizeof t,"J 环形爆发 冷却 %d.%d秒",g->skill_cooldown/60,(g->skill_cooldown%60)/6);
    else snprintf(t,sizeof t,"J 环形爆发 就绪");
    gfx_text(r,24,701,t,1,g->skill_cooldown?MUTED:GREEN);
    snprintf(t,sizeof t,"总弹 %d  我方 %d",game_total_bullets(g),game_player_count(g));gfx_text(r,385,701,t,1,GOLD);
    gfx_text(r,550,691,info.hw?"硬件绘制":"处理器绘制",1,info.hw?GREEN:GOLD);
    snprintf(t,sizeof t,"%d.%d 毫秒  渲染%d帧/秒",info.ms/10,info.ms%10,info.fps);
    gfx_text(r,1250-gfx_text_width(t,2),687,t,2,info.fps>=50?GREEN:GOLD);
    if(g->limit_mode){snprintf(t,sizeof t,"弹体%dpx 步长%d",game_seed_size(),info.quantity_step?info.quantity_step:100);gfx_text(r,680,705,t,1,CYAN);}
    if(info.present_valid){snprintf(t,sizeof t,"呈现%d.%d 逻辑%d.%d",info.present_fps10/10,info.present_fps10%10,info.logic_hz10/10,info.logic_hz10%10);gfx_text(r,1250-gfx_text_width(t,1),705,t,1,MUTED);}
    if(g->limit_mode&&g->stress_tier==STRESS_CUSTOM)gfx_text(r,24,95,"自定义档：- / + 按所选步长调整；M返回菜单",1,GOLD);
}
#if SPROUT_STATS
static void stats_overlay(Gfx *r,const Game *g,RenderInfo info) {
    char t[160];int y=480;panel(r,24,y,650,177,EDGE);
    gfx_text(r,40,y+12,"每帧耗时 微秒",2,CYAN);
    snprintf(t,sizeof t,"逻辑 %d 峰值 %d 同步 %d",info.logic_us,info.logic_max_us,info.vsync_us);gfx_text(r,40,y+43,t,1,CREAM);
    snprintf(t,sizeof t,"提交 %d 队列等待 %d 尾部等待 %d",info.submit_us,info.qstall_us,info.tail_us);gfx_text(r,40,y+66,t,1,CREAM);
    snprintf(t,sizeof t,"弹幕 %d/%d 丢弃 %d",game_hostile_count(g),game_hostile_slots(g),game_dropped_shots(g));gfx_text(r,40,y+89,t,1,MUTED);
    snprintf(t,sizeof t,"绘制负载 %d%% 读取 %d 写入 %d MB/s",info.blt,info.rd_mbs,info.wr_mbs);gfx_text(r,40,y+112,t,1,CYAN);
    snprintf(t,sizeof t,"处理器 %d帧  硬件 %d帧  当前 %s",info.fps_mode[0],info.fps_mode[1],info.hw?"硬件":"处理器");gfx_text(r,40,y+142,t,1,GOLD);
}
#endif
static void menu_center(Gfx *r,int y,const char *s,int sc,Pixel c){center(r,640,y,s,sc,c);}
static void menu_label(Gfx*r,int y,const char*s,int sc,Pixel c){gfx_label(r,640-gfx_text_width(s,sc)/2,y,s,sc,c);}
int render_title_hit(int x,int y){if(x>=360&&x<920){if(y>=405&&y<465)return 0;if(y>=485&&y<545)return 1;}return -1;}
int render_stress_hit(int x,int y){int i;for(i=0;i<4;i++)if(x>=340&&x<940&&y>=270+i*64&&y<324+i*64)return i;return -1;}
int render_upgrade_hit(int x,int y){int i;for(i=0;i<3;i++)if(x>=70+i*390&&x<430+i*390&&y>=280&&y<560)return i;return -1;}
void render_game(const Game *g,Gfx *r,RenderInfo info) {
    char t[160];int i;uint32_t t0=clock_now(),th,finish;
    fx_alpha=info.alpha_fx;draw_world(r,g,info.batch);
    if(fx_alpha&&g->phase!=PLAYING)gfx_fill_alpha(r,0,0,1280,720,INK,A_DIM);
    th=clock_now();hud(r,g,info);
    if(g->phase==TITLE){
        panel(r,240,130,800,490,EDGE);menu_label(r,164,"萝卜求生",6,GOLD);
        menu_label(r,251,"闪避弹幕 自动射击 收集经验 不断成长",2,MUTED);
        gfx_blit(r,640-player.width/2,300,&player,1);
        gfx_fill_rect(r,360,405,560,60,GREEN);menu_label(r,423,"回车 开始游戏",3,INK);
        gfx_fill_rect(r,360,485,560,60,GOLD);menu_label(r,503,"L 压力测试",3,INK);
        menu_label(r,578,"低档 / 中档 / 高档 / 自定义",2,MUTED);
    }else if(g->phase==STRESS_SELECT){
        static const char *names[]={"1 低档 32怪物 1024弹","2 中档 64怪物 2048弹","3 高档 128怪物 4096弹","4 自定义 最高99999弹"};
        panel(r,240,130,800,490,EDGE);menu_label(r,163,"压力测试",4,GOLD);menu_label(r,225,"按数字1至4选择档位",2,CREAM);
        for(i=0;i<4;i++){gfx_fill_rect(r,340,270+i*64,600,54,i==0?GREEN:i==1?GOLD:RED);menu_center(r,285+i*64,names[i],2,INK);}
        menu_label(r,577,"M 返回菜单；自定义档中 - / + 调整弹幕数",1,MUTED);
    }else if(g->phase==UPGRADE){
        panel(r,35,160,1210,460,EDGE);menu_label(r,186,"升级选择",4,GREEN);
        snprintf(t,sizeof t,"等级 %d 每次选择一项",g->level);menu_center(r,239,t,2,CREAM);
        for(i=0;i<3;i++){int x=70+i*390,id=g->choices[i];panel(r,x,280,360,280,EDGE);
            snprintf(t,sizeof t,"%d",i+1);gfx_text(r,x+24,300,t,4,GOLD);
            center(r,x+180,372,game_upgrade_name(id),2,CREAM);
            center(r,x+180,417,game_upgrade_line(id,0),1,GREEN);
            center(r,x+180,451,game_upgrade_line(id,1),1,MUTED);
            center(r,x+180,477,game_upgrade_line(id,2),1,MUTED);
            center(r,x+180,523,"按对应数字确认",1,GOLD);}
    }else if(g->phase==PAUSED||g->phase==DEFEAT){
        panel(r,300,200,680,330,g->phase==DEFEAT?RED:EDGE);
        menu_center(r,235,g->phase==DEFEAT?"游戏结束":"游戏暂停",4,g->phase==DEFEAT?RED:CREAM);
        snprintf(t,sizeof t,"存活 %02d:%02d 击败 %d 等级 %d",g->ticks/3600,g->ticks/60%60,g->kills,g->level);menu_center(r,335,t,2,GOLD);
        menu_center(r,410,g->phase==DEFEAT?"回车 再来一局":"P 继续游戏",3,GREEN);menu_label(r,480,"M 返回菜单",2,MUTED);
    }
#if SPROUT_STATS
    if(info.show_stats)stats_overlay(r,g,info);
#endif
    finish=clock_now();RSPLIT(hud,finish-th);RSPLIT(total,finish-t0);
}
