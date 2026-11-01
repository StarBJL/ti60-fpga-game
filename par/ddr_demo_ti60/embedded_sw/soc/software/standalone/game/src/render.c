#include "render.h"
#include "campaign_art.h"
#include "v5_art.h"
#include "v6_art.h"
#include "mini_snprintf.h"   /* SoC port: replaces <stdio.h> snprintf (no newlib stdio/malloc) */
#include "coop_fx.h"
#include "scene_assets.h"
#include "carrot_fx.h"         /* 多人：玩家颜色 / 染色 / 光晕 / 标签（2026-10-26，队友版） */
#include <string.h>

#define INK RGB565(15,24,31)
#define PANEL RGB565(22,35,42)
#define EDGE RGB565(65,85,84)
#define CREAM RGB565(242,237,213)
#define MUTED RGB565(145,163,152)
#define GREEN RGB565(181,225,111)
#define GOLD RGB565(242,190,93)
#define RED RGB565(238,112,112)
#define CYAN RGB565(118,208,198)

/* 2026-10-25：萝卜场景原生 1280x720（SPROUT_W x SPROUT_H），场地变大，角色美术大小不变。
 * 菜单面板（标题 / 压力选择 / 升级 / 暂停 / 失败）还是原来 640x480 的排版，整体平移到屏幕中间：
 * UX(x) / UY(y) = 原来的坐标 + 偏移。HUD 顶栏 / 底栏铺满整个宽度。 */
#define UI_DX ((SPROUT_W-640)/2)
#define UI_DY ((SPROUT_H-480)/2)
#define UX(x) ((x)+UI_DX)
#define UY(y) ((y)+UI_DY)
#define BAR_Y (SPROUT_H-32)          /* 底栏（640x480 时 448） */

/* 64 字节对齐：和帧缓冲同相位（模 16），整屏背景拷贝才能走引擎 128 位宽通路（C2） */
#ifdef __riscv
static Pixel background_pixels[SPROUT_W*SPROUT_H] __attribute__((section(".artbss"),aligned(64)));
#else
static Pixel background_pixels[SPROUT_W*SPROUT_H] __attribute__((aligned(64)));
#endif
static Sprite background={SPROUT_W,SPROUT_H,background_pixels,0};
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

/* ---- 队友版 1.5 倍美术（2026-10-26 移植）------------------------------------------
 * 普通模式敌弹：菱形 / 星 / 环三种（宽 9 / 17 / 13），画在 17x17 画布上，开机放大成 25x25；
 * 弹型 0、1 = 小菱形（喷吐怪），2 = 大星（Boss），3 = 环。 */
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
/* 玩家子弹：8 个方向的黄色激光（19x19，开机放大成 29x29），方向在开火时定（Bullet.style） */
#define LASER_SIZE 19
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
/* 开机把原尺寸的图最近邻放大到 w x h（1.5 倍），换到 native_* 缓冲里（图案不变） */
static Pixel native_sprite_px[48000]; static uint8_t native_sprite_a[48000]; static unsigned native_used;
static void native_sprite(Sprite *sp,int w,int h){
    unsigned off=native_used; int x,y,sw=sp->width,sh=sp->height;
    if(off+(unsigned)(w*h)>48000u) return;
    for(y=0;y<h;y++) for(x=0;x<w;x++){
        int a=y*sh/h*sw+x*sw/w; native_sprite_px[off+y*w+x]=sp->pixels[a];
        if(sp->alpha) native_sprite_a[off+y*w+x]=sp->alpha[a];
    }
    sp->pixels=native_sprite_px+off; if(sp->alpha) sp->alpha=native_sprite_a+off;
    sp->width=w; sp->height=h; native_used+=(unsigned)(w*h+1)&~1u;
}

/* 压力模式弹幕的几种大小（2026-10-25，参照队友版 1 3 6 9 13 17 25）：开机由 9x9 图最近邻缩放生成，
 * 每种大小一组 4 个样式，地址各不相同（换大小 = 换精灵地址，引擎的槽 / 源缓存按地址自动换，不用停引擎）。
 * 9 就是原图。1x1 用身体颜色（最近邻取到的是白色高光，4 种样式会一模一样）。 */
#define SEED_SIZE_N 7
static const int seed_sizes[SEED_SIZE_N]={1,3,6,9,13,17,25};
static Pixel seed_pixels[4*(2+10+36+82+170+290+626)] __attribute__((aligned(4)));   /* 每张按偶数像素起始 */
static Sprite seed_sprites[SEED_SIZE_N][4];
static void init_seed_sizes(void) {
    static const Pixel body1[4]={RGB565(240,103,84),RGB565(63,181,217),RGB565(239,106,135),RGB565(158,120,224)};
    int k,style,x,y,at=0;
    for(k=0;k<SEED_SIZE_N;k++) for(style=0;style<4;style++) {
        const int s=seed_sizes[k]; Pixel *p=&seed_pixels[at];
        for(y=0;y<s;y++) for(x=0;x<s;x++)
            p[y*s+x]=hostile_pixels[style][((2*y+1)*9/(2*s))*9+(2*x+1)*9/(2*s)];
        if(s==1) p[0]=body1[style];
        seed_sprites[k][style].width=s; seed_sprites[k][style].height=s;
        seed_sprites[k][style].pixels=p; seed_sprites[k][style].alpha=0;
        at+=(s*s+1)&~1;
    }
}
/* 这一局弹幕用哪组图：普通模式 9x9 原图，压力模式按 game_seed_size() */
static const Sprite *seed_set(const Game *g) {
    int k,s=game_seed_extent(g);
    if(!g->limit_mode) return v5_seeds()?v5_seeds():normal_seeds;          /* 普通模式：队友版 25x25 敌弹 */
    if(s==9) return hostile_sprites;                 /* 原图（和以前同一地址） */
    for(k=0;k<SEED_SIZE_N;k++) if(seed_sizes[k]==s) return seed_sprites[k];
    return hostile_sprites;
}

/* Ellipse w x SHADOW_H: solid core, alpha falls off linearly over the outer rim. */
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
#define ARENA_X0 SPROUT_ARENA_X0      /* 640x480 时 14 60 626 443 */
#define ARENA_Y0 SPROUT_ARENA_Y0
#define ARENA_X1 SPROUT_ARENA_X1
#define ARENA_Y1 SPROUT_ARENA_Y1
#define ARENA_W  (ARENA_X1-ARENA_X0)  /* 612 -> 1252 */
#define ARENA_H  (ARENA_Y1-ARENA_Y0)  /* 383 -> 623 */
/* 装饰物个数按场地面积放大（640x480 场地 612x383 时：草丛 700 三叶草 120 石子 260 小花 45） */
#define DECO(n)  ((int)((n)*(long long)(ARENA_W*ARENA_H)/(612*383)))
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
    for(i=0;i<DECO(700);i++) {
        int k,blades,lean;
        seed=seed*1664525u+1013904223u; x=ARENA_X0+8+(int)((seed>>8)%(unsigned)(ARENA_W-16));
        seed=seed*1664525u+1013904223u; y=ARENA_Y0+10+(int)((seed>>8)%(unsigned)(ARENA_H-15));
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
    for(i=0;i<DECO(120);i++) {
        seed=seed*1664525u+1013904223u; x=ARENA_X0+10+(int)((seed>>8)%(unsigned)(ARENA_W-22));
        seed=seed*1664525u+1013904223u; y=ARENA_Y0+10+(int)((seed>>8)%(unsigned)(ARENA_H-21));
        if(vnoise(x,y,7,37u)>140) continue;
        gfx_fill_rect(g,x,y,2,2,RGB565(66,104,64)); gfx_fill_rect(g,x+2,y+1,2,2,RGB565(60,96,60));
        gfx_fill_rect(g,x+1,y+2,2,2,RGB565(72,112,68)); gfx_fill_rect(g,x+1,y+1,1,1,RGB565(98,134,80));
    }
    /* pebbles: mostly on soil */
    for(i=0;i<DECO(260);i++) {
        int w,h,on_soil;
        seed=seed*1664525u+1013904223u; x=ARENA_X0+6+(int)((seed>>8)%(unsigned)(ARENA_W-14));
        seed=seed*1664525u+1013904223u; y=ARENA_Y0+6+(int)((seed>>8)%(unsigned)(ARENA_H-13));
        on_soil=vnoise(x,y,7,37u)>150;
        if(!on_soil && (seed>>29)) continue;               /* 1 in 8 on grass */
        w=2+(int)((seed>>20)&3u); h=2+(int)((seed>>22)&1u);
        gfx_fill_rect(g,x+1,y+h,w,1,RGB565(30,34,30));                 /* shadow */
        gfx_fill_rect(g,x,y,w,h,(seed>>24)&1 ? RGB565(104,98,84) : RGB565(92,90,80));
        gfx_fill_rect(g,x,y,w-1,1,RGB565(134,128,110));                /* highlight */
    }
    /* a few pale flowers, deliberately small and desaturated */
    for(i=0;i<DECO(45);i++) {
        Pixel petal;
        seed=seed*1664525u+1013904223u; x=ARENA_X0+12+(int)((seed>>8)%(unsigned)(ARENA_W-26));
        seed=seed*1664525u+1013904223u; y=ARENA_Y0+12+(int)((seed>>8)%(unsigned)(ARENA_H-25));
        if(vnoise(x,y,7,37u)>140) continue;
        petal=(seed>>26)%3==0 ? RGB565(196,190,168) : (seed>>26)%3==1 ? RGB565(200,184,120) : RGB565(176,160,178);
        gfx_fill_rect(g,x,y+2,1,2,RGB565(50,80,52));                   /* stem */
        gfx_fill_rect(g,x-1,y,3,1,petal); gfx_fill_rect(g,x,y-1,1,3,petal);
        gfx_fill_rect(g,x,y,1,1,RGB565(214,170,80));
    }
}

/* 队友版角色美术（2026-10-26）：7 级形态的萝卜（40x40 -> 60x60）、3 套配色 x 32 个朝向的枪
 * （17x17 -> 25x25）、被弹幕盖住时的轮廓提示。数据在 hero_art_data.h（队友版原样）。 */
#include "hero_evolution.h"
#include "weapon_pose.h"
#include "hero_locator.h"


void render_init(void) {
    Gfx a; int x,y;
    carrot_fx_init();
    scene_assets_init();
    campaign_art_init();
    gfx_begin(&a,background_pixels,SPROUT_W,SPROUT_H);
    gfx_fill_rect(&a,0,0,SPROUT_W,SPROUT_H,INK);
    ground(&a);
    gfx_border(&a,ARENA_X0,ARENA_Y0,ARENA_W,ARENA_H,2,RGB565(78,94,70));          /* 14,60,612,383 */
    gfx_border(&a,ARENA_X0+4,ARENA_Y0+4,ARENA_W-8,ARENA_H-8,1,RGB565(28,43,39));
    for(x=ARENA_X0+2;x<ARENA_X1-2;x+=32) {                                          /* 围栏桩 */
        gfx_fill_rect(&a,x,ARENA_Y0,2,6,RGB565(101,109,76));
        gfx_fill_rect(&a,x,ARENA_Y1-6,2,6,RGB565(101,109,76));
    }
    for(y=ARENA_Y0+20;y<ARENA_Y1-4;y+=32) {
        gfx_fill_rect(&a,ARENA_X0,y,6,2,RGB565(101,109,76));
        gfx_fill_rect(&a,ARENA_X1-6,y,6,2,RGB565(101,109,76));
    }
    gfx_begin(&a,player_pixels,24,32); gfx_fill_rect(&a,0,0,24,32,COLOR_KEY);
    gfx_fill_rect(&a,5,28,5,3,INK); gfx_fill_rect(&a,15,28,5,3,INK);
    ellipse(&a,1,7,22,23,INK); ellipse(&a,3,8,18,20,RGB565(183,122,62));
    ellipse(&a,4,8,16,16,RGB565(225,177,100));
    gfx_fill_rect(&a,6,10,5,2,RGB565(250,213,139));
    sprout(&a,11,4,RGB565(121,165,78));
    gfx_fill_rect(&a,7,16,3,4,INK); gfx_fill_rect(&a,15,16,3,4,INK);
    gfx_fill_rect(&a,8,16,1,1,CREAM); gfx_fill_rect(&a,16,16,1,1,CREAM);
    gfx_fill_rect(&a,11,22,4,1,INK);
    gfx_fill_rect(&a,3,22,3,3,RGB565(180,113,60));
    gfx_fill_rect(&a,16,24,2,2,RGB565(166,103,60));
    gfx_fill_rect(&a,4,14,2,2,RGB565(194,142,78));

    gfx_begin(&a,slime_pixels,24,26); gfx_fill_rect(&a,0,0,24,26,COLOR_KEY);
    ellipse(&a,1,5,22,19,INK); ellipse(&a,3,6,18,16,RGB565(76,148,196));   /* 队友版配色 */
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
    init_seed_sizes();
    init_laser_sprites();
    init_normal_seeds();
    init_hero_evolution();                    /* 也把 player（标题画面用）换成 1 级萝卜 */
    init_shadow_sprites();
    native_used=0;
    {   /* 1.5 倍（队友版原生 720p 尺寸）：角色 / 怪物 / 宝石 / 普通敌弹 / 激光 / 影子 */
        Sprite *actors[]={&player,&slime,&runner,&brute,&elite,&spitter,&gem}; int i;
        for(i=0;i<7;i++) native_sprite(actors[i],actors[i]->width*3/2,actors[i]->height*3/2);
        for(i=0;i<4;i++) native_sprite(&normal_seeds[i],25,25);
        for(i=0;i<8;i++) native_sprite(&laser_sprites[i],CARROT_LASER_SIZE,CARROT_LASER_SIZE);
        for(i=0;i<SHADOW_COUNT;i++) native_sprite(&shadow_sprites[i],shadow_sprites[i].width*3/2,shadow_sprites[i].height*3/2);
    }
    v5_art_init();
    v6_art_init();
    init_player_locator();
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
static void draw_enemy(Gfx *r,const Enemy *e,int tick,int scene) {
    const Sprite *s=e->kind==4?&spitter:e->kind==3?&elite:e->kind==2?&brute:e->kind==1?&runner:&slime;
    if(scene>=0){const Sprite *v=v5_enemy(e->family,e->kind,e->hit?3:e->attack_anim?2:e->kind>=2&&e->attack_timer<18?1:0,e->hp<e->max_hp/2);if(v)s=v;}
    int x=e->x/FIX,y=e->y/FIX,bob=((tick+e->id*7)/12)%2;
    if(e->spawn && e->kind==3 && scene>=0){
        gfx_blit_alpha(r,x-s->width/2,y-s->height+12,s,1,fx_alpha?150:255);
        gfx_border(r,x-58,y-18,116,38,2,GOLD);return;
    }
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
        gfx_border(r,x-20,y-18,28,28,2,CYAN);          /* 队友版位置（1.5 倍的喷吐怪） */
        gfx_fill_rect(r,x-2,y-20,4,6,RGB565(186,214,96));
    }
    gfx_blit_alpha(r,x-s->width/2,y-s->height+12-bob,s,1,scene>=0&&fx_alpha&&e->kind!=3?225:255);
    if(e->hit) {
        gfx_fill_rect(r,x-4,y-13,8,2,CREAM); gfx_fill_rect(r,x-1,y-16,2,8,CREAM);
    }
    if(e->kind>=2 && e->hp<e->max_hp) {
        int w=e->kind==3 ? 44 : 26;
        gfx_fill_rect(r,x-w/2,y-s->height+5,w,3,INK);
        gfx_fill_rect(r,x-w/2,y-s->height+5,w*e->hp/e->max_hp,2,e->kind==3?RED:e->kind==4?CYAN:GOLD);
    }
}
/* 萝卜 + 双枪（2026-10-26，照搬队友版 draw_player_at，单人）：在身体后面的枪先画，再画身体，
 * 再画前面的枪，最后把脸（cheek/face 那块）再盖一次，枪转到脸前也不会挡住眼睛。
 * 被弹幕盖住时（hero_locator.h）身体按 loc_info.fade 变半透明，另外画轮廓。 */
static void draw_player_gun(Gfx *r,const WeaponPose *p,int palette,int bob,int alpha){
    gfx_blit_alpha(r,p->x-CARROT_GUN_HALF,p->y-CARROT_GUN_HALF-bob,&gun_sprites[palette*32+p->heading],1,alpha);
}
/* a = 这个玩家的状态，id = 0..2（多人时 P2 / P3 染色，coop_fx_tint；单人 id 0 = 原图） */
static void draw_player(Gfx *r,const Game *g,const GamePlayer *a,int id) {
    int x=a->x/FIX,y=a->y/FIX,bob=a->moving ? (g->ticks/7)%2 : 0;
    int level=game_player_level(g,a),palette=level<4?0:level<6?1:2;
    const CarrotFighter *cf=&g->combat.player[id];
    int pose=cf->skill_ticks?(cf->skill_age<cf->skill_total/3?2:3):(a->muzzle?1:0);
    const Sprite *custom=g->limit_mode?0:v5_hero(id,pose);
    if(custom){
        const Sprite *outline=v6_outline(id,pose);
        if(outline)gfx_blit_alpha(r,x-32,y-CARROT_HERO_ANCHOR_Y-bob-2,outline,0,255);
        gfx_blit_alpha(r,x-30,y-CARROT_HERO_ANCHOR_Y-bob,custom,1,255);
        if(a->invincible && (g->ticks/6)%2)gfx_fill_rect(r,x-2,y+13,5,2,GOLD);
        return;
    }
    const int idle=a->weapon_idle_ticks>30;
    WeaponPose p[2]={weapon_pose_player(a,0,idle),weapon_pose_player(a,1,idle)};
    const Sprite *s=coop_fx_tint(render_hero_asset(level,(g->ticks/9)&1),id,r);
    int alpha=g->limit_mode?255:255-loc_info.fade;
    if(alpha){
        int k;
        for(k=0;k<2;k++) if(!p[k].front) draw_player_gun(r,&p[k],palette,bob,alpha);
        if(alpha==255) gfx_blit(r,x-s->width/2,y-CARROT_HERO_ANCHOR_Y-bob,s,1);
        else {
            /* 绕开脸分块画，脸只混合一次 */
            int ox=x-30,oy=y-CARROT_HERO_ANCHOR_Y-bob;
            gfx_blit_part_alpha(r,ox,oy,s,0,0,60,21,1,alpha);
            gfx_blit_part_alpha(r,ox,oy+21,s,0,21,12,23,1,alpha);
            gfx_blit_part_alpha(r,ox+48,oy+21,s,48,21,12,23,1,alpha);
            gfx_blit_part_alpha(r,ox,oy+44,s,0,44,60,16,1,alpha);
        }
        for(k=0;k<2;k++) if(p[k].front) draw_player_gun(r,&p[k],palette,bob,alpha);
        gfx_blit_part_alpha(r,x-18,y-12-bob,s,12,21,36,23,1,alpha);
    }
    if(a->muzzle && !idle) { int k; for(k=0;k<2;k++) if(a->weapon_flash_mask&(1<<k)) {
        gfx_fill_rect(r,p[k].mx-1,p[k].my-1-bob,3,3,GOLD);
        gfx_fill_rect(r,p[k].mx,p[k].my-bob,1,1,CREAM);
    } }
    if(a->invincible && (g->ticks/6)%2)
        gfx_fill_rect(r,x-2,y+13,5,2,GOLD);
}
/* 被弹幕盖住时的轮廓 + 中心点（只在普通模式，hero_locator.h 决定淡入淡出） */
static void draw_player_locator(Gfx *r,const Game *g,const GamePlayer *a){
    int x,y,bob; const Sprite *s;
    if(g->limit_mode||!loc_info.fade) return;
    x=a->x/FIX; y=a->y/FIX; bob=a->moving?(g->ticks/7)%2:0;
    s=render_locator_outline_asset();
    if(loc_dirty&&r->ops&&r->ops->publish){r->ops->publish(r->context);loc_dirty=0;}
    gfx_blit_alpha(r,x-LOC_X,y-LOC_Y-bob,s,1,(fx_alpha?190:255)*loc_info.fade/255);
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
    int nh=game_hostile_used(g),i,k=0,drawn=0; const Seed *hp=nh?game_hostile_at(g,0):0;  /* contiguous pool, slots >= nh are free */
    const Sprite *spr=seed_set(g);                       /* 压力模式可换大小（game_seed_size） */
    const int half=game_seed_half(g),ext=game_seed_extent(g);
    const int opacity=!g->limit_mode&&fx_alpha?160:255;
    if(!batch) {
        for(i=0;i<nh;i++) if(hp[i].active) {
            const Seed *b=&hp[i]; int x=b->x/FIX,y=b->y/FIX;
            int style=(unsigned)b->style<(unsigned)game_seed_styles(g) ? b->style : 0;
            gfx_blit_alpha(r,x-half,y-half,&spr[style],1,opacity);
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
            /* 表是按 1280x720 判断"全在屏内"的；画面比它小（480p 位流上只显示左上角）时必须走带裁剪的列表 */
            const int fits=r->width>=SPROUT_W && r->height>=SPROUT_H;
            if(opacity<255){
                if(n&&fits&&!game_seed_draw_clip(g))gfx_blit_table_alpha(r,spr,game_seed_styles(g),items,n,1,opacity,(uint32_t)n*(uint32_t)(ext*ext));
                else if(n)gfx_blit_table_alpha_clipped(r,spr,game_seed_styles(g),items,n,1,opacity);
            }else{
                if(n && fits && !game_seed_draw_clip(g))gfx_blit_table(r,spr,game_seed_styles(g),items,n,1,(uint32_t)n*(uint32_t)(ext*ext));
                else if(n)gfx_blit_table_clipped(r,spr,game_seed_styles(g),items,n,1);
            }
            return n;
        }
    }
    for(i=0;i<nh;i++) if(hp[i].active) {
        const Seed *b=&hp[i]; int x=b->x/FIX-half,y=b->y/FIX-half;
        if(x<=-ext || y<=-ext || x>=SPROUT_W || y>=SPROUT_H) continue;   /* off screen: gfx_blit draws nothing either */
        seed_list[k].x=(int16_t)x; seed_list[k].y=(int16_t)y;
        seed_list[k].sprite=(unsigned)b->style<(unsigned)game_seed_styles(g) ? b->style : 0;
        ++drawn;
        if(++k==SEED_CHUNK){if(opacity==255)gfx_blit_list(r,spr,seed_list,k,1);else for(int q=0;q<k;q++)gfx_blit_alpha(r,seed_list[q].x,seed_list[q].y,&spr[seed_list[q].sprite],1,opacity);k=0;}
    }
    if(k){if(opacity==255)gfx_blit_list(r,spr,seed_list,k,1);else for(int q=0;q<k;q++)gfx_blit_alpha(r,seed_list[q].x,seed_list[q].y,&spr[seed_list[q].sprite],1,opacity);}
    return drawn;
}

#if SPROUT_STATS
/* Seed submit anatomy (diagnostic, see render.h). */
static GfxItem prof_list[SEED_CHUNK];
static int prof_k,prof_n;
static volatile int prof_sink;
static Sprite tiny_sprites[4];

int render_seed_part(Gfx *r,const Game *g,int part) {
    int nh=game_hostile_used(g),i,k=0,n=0,sum=0; const Seed *hp=nh?game_hostile_at(g,0):0;
    switch(part) {
    case SEED_PART_PREP:            /* first 128 on-screen seeds -> prof_list, total count -> prof_n */
        for(i=0;i<4;i++) {
            tiny_sprites[i].width=1; tiny_sprites[i].height=1;
            tiny_sprites[i].pixels=&hostile_pixels[i][4*9+4]; tiny_sprites[i].alpha=0;
        }
        for(i=0;i<nh;i++) if(hp[i].active) {
            const Seed *b=&hp[i]; int x=b->x/FIX-4,y=b->y/FIX-4;
            if(x<=-9 || y<=-9 || x>=SPROUT_W || y>=SPROUT_H) continue;
            if(k<SEED_CHUNK) {
                prof_list[k].x=(int16_t)x; prof_list[k].y=(int16_t)y;
                prof_list[k].sprite=(unsigned)b->style<GAME_SEED_STYLES ? b->style : 0; ++k;
            }
            ++n;
        }
        prof_k=k; prof_n=n;
        return nh;
    case SEED_PART_WALK:            /* pool walk + on-screen test, no stores */
        for(i=0;i<nh;i++) if(hp[i].active) {
            const Seed *b=&hp[i]; int x=b->x/FIX-4,y=b->y/FIX-4;
            if(x<=-9 || y<=-9 || x>=SPROUT_W || y>=SPROUT_H) continue;
            sum+=x+y+b->style; ++n;
        }
        prof_sink=sum;
        return n;
    case SEED_PART_BUILD:           /* walk + fill seed_list, nothing submitted */
        for(i=0;i<nh;i++) if(hp[i].active) {
            const Seed *b=&hp[i]; int x=b->x/FIX-4,y=b->y/FIX-4;
            if(x<=-9 || y<=-9 || x>=SPROUT_W || y>=SPROUT_H) continue;
            seed_list[k].x=(int16_t)x; seed_list[k].y=(int16_t)y;
            seed_list[k].sprite=(unsigned)b->style<GAME_SEED_STYLES ? b->style : 0;
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
            gfx_blit_list(r,part==SEED_PART_TINY?tiny_sprites:hostile_sprites,prof_list,k,1);
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
static uint16_t sort_prev[MAX_ENEMIES]; /* Slots 256..383 must survive frame reuse. */
static uint8_t sort_seen[MAX_ENEMIES];
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
    for(i=0;i<n;i++) sort_prev[i]=(uint16_t)order[i];
    sort_prev_n=n;
    return n;
}

static void draw_world(Gfx *r,const Game *g,int batch) {
    uint32_t t,t1;
    int i,n,order[MAX_ENEMIES],ys[MAX_ENEMIES],drawn=0;
    t=clock_now();
    const Sprite *back=(!g->limit_mode && g->phase!=STRESS_SELECT) ? campaign_background(g->campaign.scene) : 0;
    gfx_blit(r,0,0,back ? back : &background,0);
    for(i=0;i<MAX_GEMS;i++) if(g->gems[i].active) {
        const Sprite *cr=g->limit_mode?0:campaign_crystal(g->campaign.scene);
        gfx_blit_alpha(r,g->gems[i].x/FIX-(cr?12:4),g->gems[i].y/FIX-(cr?14:5),cr?cr:&gem,1,cr&&fx_alpha?115:255);
    }
    t1=clock_now(); RSPLIT(bg,t1-t); t=t1;
    n=enemy_order(g,order,ys);
    t1=clock_now(); RSPLIT(sort,t1-t); t=t1;
    /* 队友版顺序：轮廓状态 -> 角色影子 -> 全部怪物 -> 角色（角色总在怪物上面）。
     * 多人：倒下的玩家不画；每人一个彩色光晕 + "P1".."P3" 标签（coop_fx_actor），P2 / P3 的角色和子弹染色 */
    {
    const int players=g->player_count>1 ? g->player_count : 1;
    int p;
    if(g->limit_mode) { if(loc_game||loc_info.total||loc_info.fade) render_locator_reset(); }
    else for(p=0;p<players;p++) if(players==1||game_player(g,p)->hp>0) if(!v5_hero(p,0))prepare_player_locator(g,game_player(g,p),p);
    for(p=0;p<players;p++) if(players==1||game_player(g,p)->hp>0) {
        const GamePlayer *a=game_player(g,p); shadow(r,a->x/FIX,a->y/FIX+21,14);
    }
    for(i=0;i<n;i++) draw_enemy(r,&g->enemies[order[i]],g->ticks,g->limit_mode?-1:g->campaign.scene);
    if(game_combat_enabled(g)) carrot_fx_under(r,g,fx_alpha);
    (void)drawn; (void)ys;
    for(p=0;p<players;p++) if(players==1||game_player(g,p)->hp>0) {
        const GamePlayer *a=game_player(g,p);
        if(!g->limit_mode) locator_load(p);              /* 身体按这个玩家的 loc_info.fade 淡出 */
        if(players>1) coop_fx_actor(r,a->x/FIX,a->y/FIX,p,(unsigned)g->ticks,a->moving,0,a->hurt_flash,fx_alpha);
        if(game_combat_enabled(g) && fx_alpha && g->combat.player[p].trail_count) {
            const CarrotFighter *f=&g->combat.player[p];
            const Sprite *body=v5_hero(p,0);
            if(!body)body=coop_fx_tint(render_hero_asset(game_player_level(g,a),0),p,r);
            int k;
            for(k=0;k<f->trail_count;k++)
                gfx_blit_alpha(r,f->trail_x[k]-30,f->trail_y[k]-CARROT_HERO_ANCHOR_Y,body,1,(70+k*40)*f->trail_ticks/18);
        }
        if(g->limit_mode)draw_player(r,g,a,p);
    }
    t1=clock_now(); RSPLIT(enemies,t1-t); t=t1;
    for(i=0;i<MAX_BULLETS;i++) if(g->bullets[i].active) {   /* 玩家子弹 = 8 方向激光（队友版），多人按发射者染色 */
        const Bullet *b=&g->bullets[i]; int x=b->x/FIX,y=b->y/FIX;
        const Sprite *ls=&laser_sprites[(unsigned)b->style<8u?b->style:0];
        if(game_combat_enabled(g)) carrot_fx_projectile(r,g,i,fx_alpha);
        else gfx_blit(r,x-CARROT_LASER_SIZE/2,y-CARROT_LASER_SIZE/2,players>1 ? coop_fx_tint(ls,b->owner,r) : ls,1);
    }
    t1=clock_now(); RSPLIT(pbullets,t1-t); t=t1;
    RSPLIT(seeds_n,(uint32_t)draw_seeds(r,g,batch));
    t1=clock_now(); RSPLIT(seeds,t1-t); t=t1;
    for(i=0;i<MAX_PARTICLES;i++) if(g->particles[i].life) {
        const Particle *p=&g->particles[i]; Pixel c=p->color==1?RED:p->color==2?GREEN:GOLD;
        if(g->limit_mode||!fx_alpha)gfx_fill_rect(r,p->x/FIX,p->y/FIX,p->size,p->size,c);else gfx_fill_alpha(r,p->x/FIX,p->y/FIX,p->size,p->size,c,135);
    }
    if(game_combat_enabled(g)){
        carrot_fx_over(r,g,fx_alpha);
        for(int p=0;p<g->player_count;p++)if(game_player(g,p)->hp>0)draw_player(r,g,game_player(g,p),p);
    }
    if(!g->limit_mode) {
        for(p=0;p<players;p++) if(players==1||game_player(g,p)->hp>0) {
            const GamePlayer *a=game_player(g,p);
            if(!v5_hero(p,0)){locator_load(p); locator_pose_state(g,a); draw_player_locator(r,g,a);}
        }
        locator_load(0);
    }
    {   /* 红框：任何一个玩家刚被打 */
        int hurt=0;
        for(p=0;p<players;p++) hurt|=game_player(g,p)->hurt_flash;
        if(hurt) gfx_border(r,ARENA_X0,ARENA_Y0,ARENA_W,ARENA_H,3,RED);
    }
    }
    RSPLIT(particles,clock_now()-t);
}

static void hud(Gfx *r,const Game *g,RenderInfo info) {
    /* 720p：左边几块位置不变，右边的 LEVEL / FPS 靠右（RX = 多出来的宽度），横幅 / Boss 条居中 */
    enum { RX=SPROUT_W-640, CX=SPROUT_W/2 };
    char text[128]; int elapsed=g->ticks/60,i,boss=-1,boss_count=0;
    int cinematic=!g->limit_mode&&g->combat.boss_id&&g->campaign.ticks-g->combat.boss_intro_start<g->combat.boss_intro_ticks;
    if(!g->limit_mode)for(int p=0;p<g->player_count;p++)if(g->combat.player[p].skill_ticks)cinematic=1;
    gfx_fill_rect(r,0,0,SPROUT_W,57,INK);
    if(g->player_count>1) {
        /* 多人顶栏（2026-10-26 / 10-28）：每人一块 = 名字 + 设备 + 击杀 / 血条 / 在飞的子弹数，颜色 = 玩家色；
         * 右边一块是全队共享的等级 / 经验 / 时间，下面一条全宽经验条 */
        int p;
        const int n=g->player_count,pw=n>=3?370:540,ps=pw+14;
        for(p=0;p<n;p++) {
            const GamePlayer *a=game_player(g,p);
            const int x=18+p*ps,down=a->hp<=0;
            const Pixel col=down?MUTED:coop_fx_color(p);
            const char *dev=p==0?TR("UART"):n==2?TR("USB / KEYPAD"):p==1?TR("USB"):TR("KEYPAD");
            snprintf(text,sizeof(text),"P%d %s",p+1,dev);
            gfx_text(r,x,7,text,1,col);
            snprintf(text,sizeof(text),TR("KILLS %03d"),a->kills);
            gfx_text(r,x+pw-gfx_text_width(text,1),7,text,1,GOLD);
            gfx_fill_rect(r,x,20,pw-130,10,RGB565(54,64,60));
            if(!down) gfx_fill_rect(r,x+1,21,(pw-132)*a->hp/(a->max_hp>0?a->max_hp:1),8,a->hp<30?RED:col);
            if(down) snprintf(text,sizeof(text),TR("DOWN"));
            else snprintf(text,sizeof(text),TR("HP %d/%d"),a->hp,a->max_hp);
            gfx_text(r,x+pw-122,21,text,1,down?RED:CREAM);
            const CarrotFighter *cf=&g->combat.player[p];
            if(p==0)snprintf(text,sizeof(text),TR("SWIFT / SHOTS %d"),a->player_active);
            else if(p==1)snprintf(text,sizeof(text),TR("PARRY %dS / SHOTS %d"),(cf->reflect_cooldown+59)/60,a->player_active);
            else {int ready=cf->shield_cooldown,quiet=240-cf->shield_quiet;if(!cf->shield_charges&&quiet>ready)ready=quiet;snprintf(text,sizeof(text),TR("SHIELD %d / RECHARGE %dS"),cf->shield_charges,(ready+59)/60);}
            gfx_text(r,x,36,text,1,MUTED);
        }
        {   /* 全队 */
            const int tx=18+n*ps;
            snprintf(text,sizeof(text),TR("TEAM LV %02d"),g->team_level);
            gfx_text(r,tx,7,text,1,CYAN);
            snprintf(text,sizeof(text),TR("XP %d/%d"),g->team_xp,g->team_next_xp);
            gfx_text(r,tx,21,text,1,CREAM);
            snprintf(text,sizeof(text),"%02d:%02d",elapsed/60,elapsed%60);
            gfx_text(r,tx,36,text,1,MUTED);
        }
        gfx_fill_rect(r,18,49,SPROUT_W-36,3,RGB565(52,67,64));
        gfx_fill_rect(r,18,49,g->team_xp>=g->team_next_xp?SPROUT_W-36:(int)((int64_t)(SPROUT_W-36)*g->team_xp/(g->team_next_xp>0?g->team_next_xp:1)),3,CYAN);
        goto bottom_bar;
    }
    gfx_text(r,18,10,TR("SPROUT RUN"),2,CREAM);
    if(g->limit_mode) {
        const char *tier=g->stress_tier==STRESS_LOW ? TR("LOW") :
            g->stress_tier==STRESS_MEDIUM ? TR("MID") : g->stress_tier==STRESS_HIGH ? TR("HIGH") : TR("CUSTOM");
        snprintf(text,sizeof(text),TR("%s F%d S%d/%d %dPX"),tier,g->enemy_count,
            game_hostile_count(g),game_hostile_slots(g),game_seed_extent(g));
    } else snprintf(text,sizeof(text),TR("CAMPAIGN / FOES %03d"),g->enemy_count);
    gfx_text(r,19,32,text,1,g->limit_mode?GOLD:MUTED);
    gfx_text(r,202,10,TR("HEALTH"),1,MUTED);
    snprintf(text,sizeof(text),"%d/%d",g->hp,g->max_hp); gfx_text(r,277,10,text,1,CREAM);
    gfx_fill_rect(r,202,24,123,11,RGB565(54,64,60));
    gfx_fill_rect(r,203,25,121*g->hp/g->max_hp,9,g->hp<30?RED:GREEN);
    snprintf(text,sizeof(text),TR("XP %d/%d"),g->xp,g->next_xp); gfx_text(r,202,40,text,1,CYAN);
    if(g->limit_mode) {
        int budget=(int)((int64_t)info.work_us*100/16667);
        gfx_text(r,347,10,TR("WORK MS"),1,MUTED);
        snprintf(text,sizeof(text),"%d.%02d",info.work_us/1000,(info.work_us%1000)/10);
        gfx_text(r,347,24,text,info.work_us<10000?2:1,CREAM);
        gfx_text(r,435,10,TR("BUDGET"),1,MUTED);
        snprintf(text,sizeof(text),"%d%%",budget);
        gfx_text(r,435,24,text,2,budget>=100?RED:budget>=70?GOLD:GREEN);
        if(g->stress_tier==STRESS_CUSTOM) {
            gfx_text(r,540,14,TR("- + COUNT   BRACKETS SIZE"),1,MUTED);
            gfx_text(r,540,30,TR("UART: AT N ENTER SETS COUNT"),1,MUTED);
        }
    } else {
        gfx_text(r,347,10,TR("SURVIVED"),1,MUTED);
        snprintf(text,sizeof(text),"%02d:%02d",elapsed/60,elapsed%60);
        gfx_text(r,347,24,text,elapsed<6000?2:1,CREAM);
        gfx_text(r,435,10,TR("KILLS"),1,MUTED);
        snprintf(text,sizeof(text),"%03d",g->kills); gfx_text(r,435,24,text,2,GOLD);
    }
    snprintf(text,sizeof(text),TR("LEVEL %02d"),g->level); gfx_text(r,RX+527,11,text,1,CYAN);
    if(!g->limit_mode){
        if(g->campaign.scene==5)snprintf(text,sizeof(text),TR("ENDLESS W%d"),g->campaign.endless_wave+1);
        else snprintf(text,sizeof(text),TR("S%d %02dS"),g->campaign.scene+1,game_scene_remaining(g));
        gfx_text(r,RX+527,28,text,1,MUTED);
    }
    gfx_fill_rect(r,18,49,SPROUT_W-36,3,RGB565(52,67,64));
    gfx_fill_rect(r,18,49,g->xp>=g->next_xp?SPROUT_W-36:(int)((int64_t)(SPROUT_W-36)*g->xp/g->next_xp),3,CYAN);
bottom_bar:
    if(!g->limit_mode && g->phase!=TITLE && g->phase!=STRESS_SELECT){
        if(g->campaign.scene==5)snprintf(text,sizeof(text),TR("6 / 6 ENDLESS / WAVE %d / POWER +%d"),g->campaign.endless_wave+1,g->campaign.endless_wave*3);
        else snprintf(text,sizeof(text),TR("%d / 6 %s %02dS"),g->campaign.scene+1,TR(game_scene_name(g->campaign.scene)),game_scene_remaining(g));
        int sy=cinematic?216:59;gfx_fill_alpha(r,16,sy,gfx_text_width(text,1)+8,16,INK,220);
        gfx_text(r,20,sy+2,text,1,GOLD);
    }
    gfx_fill_rect(r,0,BAR_Y,SPROUT_W,SPROUT_H-BAR_Y,INK);
    {
        const char *hint=g->player_count>2?TR("3 PLAYERS: UART / USB KEYBOARD / KEYPAD   P PAUSE   M MENU"):
                         g->player_count>1?TR("2 PLAYERS: UART / USB KEYBOARD OR KEYPAD   P PAUSE   M MENU"):TR("WASD MOVE   P PAUSE   M MENU");
        const int hx=g->player_count>1?18+gfx_text_width(hint,1)+24:248;
        gfx_text(r,18,BAR_Y+9,hint,1,MUTED);
        if(info.hw_avail) gfx_text(r,hx,BAR_Y+9,info.hw?TR("HW BLIT"):TR("CPU DRAW"),1,info.hw?GREEN:GOLD);
        else gfx_text(r,hx,BAR_Y+9,TR("AUTO FIRE"),1,GREEN);
    }
    {
        /* FPS and the work per frame (logic + draw, without vsync_wait) */
        /* 2026-10-28：显示帧率（上屏的新画面 / 秒，大字）+ 渲染帧率（主循环，小字两行：渲染 / 每帧耗时） */
        char rate[32],cost[32],ren[32]; int fps_w,ms_w,x;
        snprintf(rate,sizeof(rate),TR("SHOW %d FPS"),info.show_fps);
        snprintf(ren,sizeof(ren),TR("RENDER %d FPS"),info.fps);
        snprintf(cost,sizeof(cost),TR("%d.%d MS"),info.ms<0?0:info.ms/10,info.ms<0?0:info.ms%10);
        fps_w=gfx_text_width(rate,2); ms_w=gfx_text_width(ren,1);
        if(gfx_text_width(cost,1)>ms_w) ms_w=gfx_text_width(cost,1);
        x=SPROUT_W-16-fps_w;
        gfx_text(r,x,BAR_Y+4,rate,2,info.show_fps>=58?GREEN:info.show_fps>=30?GOLD:RED);
        x-=14+ms_w;
        gfx_text(r,x,BAR_Y+3,ren,1,info.fps>=58?GREEN:info.fps>=30?GOLD:RED);
        gfx_text(r,x,BAR_Y+17,cost,1,info.ms>160?RED:GOLD);      /* 14 px 行距：中文 12 px 也不叠 */
    }
    if(g->banner && g->phase==PLAYING && !cinematic) {
        const char *label=g->limit_mode && g->event==EVENT_START ? TR("STRESS TEST / M TO MENU") :
            g->event==EVENT_HORDE ? TR("MASSIVE HORDE / KEEP MOVING") :
            g->event==EVENT_BOSS ? TR(game_scene_name(g->campaign.scene)) :
            g->event==EVENT_UPGRADE ? TR("UPGRADE READY / KEEP GROWING") : g->campaign.scene==5 ? TR("ENDLESS / SIX CLANS / ECLIPSE EVERY 3 WAVES") : TR("SURVIVE 40S / DEFEAT BOSS BY 60S");
        int w=gfx_text_width(label,1)+26;
        afill(r,CX-w/2,68,w,21,PANEL,A_BANNER);
        gfx_fill_rect(r,CX-w/2,68,w,1,GOLD); gfx_fill_rect(r,CX-w/2,88,w,1,GOLD);
        center(r,CX,75,label,1,GOLD);
    }
    for(i=0;i<MAX_ENEMIES;i++) if(g->enemies[i].active && g->enemies[i].kind==3) {
        if(boss<0) boss=i;
        ++boss_count;
    }
    if(boss>=0 && (g->phase==PLAYING || g->phase==PAUSED)) {
        const Enemy *e=&g->enemies[boss];
        int by=94;
        for(int p=0;p<g->player_count;p++)if(g->combat.player[p].skill_ticks)by=224;
        if(g->campaign.ticks-g->combat.boss_intro_start<g->combat.boss_intro_ticks)by=224;
        afill(r,CX-180,by,360,32,INK,A_BOSSBAR);
        snprintf(text,sizeof(text),"%s %d/%d x%d",TR(game_boss_name(e->family)),e->hp,e->max_hp,boss_count);
        center(r,CX,by+3,text,1,RED);
        gfx_fill_rect(r,CX-172,by+23,344,5,EDGE);
        gfx_fill_rect(r,CX-172,by+23,(int)((int64_t)344*e->hp/e->max_hp),5,RED);
    }
}

#if SPROUT_STATS
/* "CPU 38 FPS 25.1 MS" for one backend, bright when it is the active one. */
static int mode_line(Gfx *r,int x,int y,const char *name,int fps,int draw_us,int active) {
    char text[48];
    if(fps>0) snprintf(text,sizeof(text),TR("%s %d FPS %d.%d MS"),name,fps,draw_us/1000,(draw_us%1000)/100);
    else snprintf(text,sizeof(text),TR("%s -- FPS"),name);
    gfx_text(r,x,y,text,1,active?GREEN:MUTED);
    return gfx_text_width(text,1);
}
/* F1 frame-stats overlay: drawn last so it sits on top of menus too.
 * Frame time split (us per frame, see main.c): LOGIC game update (MAX = worst
 * tick), SUBMIT drawing / queueing without QSTALL (queue full), TAIL = waiting
 * for the engine after the last command, VSYNC = waiting for the flip. */
static void stats_overlay(Gfx *r,const Game *g,RenderInfo info) {
    char text[128]; int h=info.hw_avail?116:88,y=ARENA_Y1+3-h-5,w;   /* 场地左下角（640x480 时 446-h-5） */
    panel(r,22,y,276,h,EDGE);
    gfx_text(r,32,y+10,TR("FRAME TIME US"),1,CYAN);
    if(info.hw_avail) gfx_text(r,232,y+10,info.hw?TR("HW BLIT"):TR("CPU DRAW"),1,info.hw?GREEN:GOLD);
    snprintf(text,sizeof(text),TR("LOGIC %d MAX %d VSYNC %d"),info.logic_us,info.logic_max_us,info.vsync_us);
    gfx_text(r,32,y+24,text,1,CREAM);
    snprintf(text,sizeof(text),TR("SUBMIT %d QSTALL %d TAIL %d"),info.submit_us,info.qstall_us,info.tail_us);
    gfx_text(r,32,y+38,text,1,CREAM);
    /* submit split: seed loop / HUD + panels / everything else */
    snprintf(text,sizeof(text),TR("SEEDS %d HUD %d SCENE %d %s"),info.seeds_us,info.hud_us,info.scene_us,
        info.batch==3?TR("HWWORD"):info.batch==2?TR("HWLIST"):info.batch?TR("LIST"):TR("CALLS"));
    gfx_text(r,32,y+52,text,1,CREAM);
    y+=14;
    if(g->limit_mode) snprintf(text,sizeof(text),TR("SHOTS %d/%d DROP %d"),
        game_hostile_count(g),game_hostile_slots(g),game_dropped_shots(g));
    else snprintf(text,sizeof(text),TR("FILL %u BLIT %u ALPHA %u"),r->fills,r->blits,r->alphas);
    gfx_text(r,32,y+52,text,1,MUTED);
    if(!info.hw_avail) return;
    /* the engine: queue entries after submit / peak, busy share, AXI MB/s */
    if(info.hw) snprintf(text,sizeof(text),TR("Q %d/%d:%d BLT %d%% RD %d WR %d MB/S"),
        info.q_end,info.q_peak,info.q_depth,info.blt,info.rd_mbs,info.wr_mbs);
    else snprintf(text,sizeof(text),TR("G: SWITCH TO BITBLT ENGINE"));
    gfx_text(r,32,y+66,text,1,info.hw?CYAN:MUTED);
    w=mode_line(r,32,y+80,TR("CPU"),info.fps_mode[0],info.draw_mode[0],!info.hw);
    mode_line(r,32+w+18,y+80,TR("HW"),info.fps_mode[1],info.draw_mode[1],info.hw);
}
#endif

static void upgrade_icon(Gfx *r,int id,int x,int y) {
    if(id==RAPID) {
        gfx_fill_rect(r,x-12,y-10,8,8,GOLD); gfx_fill_rect(r,x-4,y-4,10,8,GOLD);
        gfx_fill_rect(r,x+2,y+3,8,8,GOLD); gfx_fill_rect(r,x+7,y+7,7,5,GOLD);
    } else if(id==POWER) {
        gfx_fill_rect(r,x-8,y-10,16,20,GOLD); gfx_fill_rect(r,x-12,y-5,24,10,GOLD);
        gfx_fill_rect(r,x-3,y-7,6,14,CREAM);
    } else if(id==BOOTS) {
        gfx_fill_rect(r,x-9,y-10,9,14,CYAN); gfx_fill_rect(r,x-9,y+2,21,9,CYAN);
        gfx_fill_rect(r,x-9,y+10,24,3,CREAM);
    } else if(id==VITALITY) {
        gfx_fill_rect(r,x-11,y-8,9,10,RED); gfx_fill_rect(r,x+2,y-8,9,10,RED);
        gfx_fill_rect(r,x-10,y,20,7,RED); gfx_fill_rect(r,x-6,y+7,12,4,RED);
        gfx_fill_rect(r,x-2,y+11,4,3,RED);
    } else if(id==MAGNET) {
        gfx_fill_rect(r,x-12,y-9,7,17,CYAN); gfx_fill_rect(r,x+5,y-9,7,17,CYAN);
        gfx_fill_rect(r,x-9,y+6,18,7,CYAN); gfx_fill_rect(r,x-12,y-9,7,5,CREAM);
        gfx_fill_rect(r,x+5,y-9,7,5,CREAM);
    } else if(id==ULT_POWER){
        gfx_fill_add(r,x-3,y-18,7,36,GOLD,220);gfx_fill_add(r,x-18,y-3,36,7,GOLD,220);gfx_fill_rect(r,x-5,y-5,11,11,CREAM);
    } else if(id==ULT_AREA){
        gfx_border(r,x-18,y-18,36,36,2,CYAN);gfx_border(r,x-10,y-10,20,20,2,CREAM);
    } else if(id==ULT_HASTE){
        gfx_border(r,x-16,y-16,32,32,2,GOLD);gfx_fill_rect(r,x-1,y-12,3,14,CREAM);gfx_fill_rect(r,x,y,12,3,CREAM);
    } else if(id==PASSIVE){
        for(int k=0;k<15;k++)gfx_fill_alpha(r,x-15+k,y+k,30-2*k,2,CYAN,200);gfx_border(r,x-15,y-12,30,12,2,CREAM);
    } else {
        sprout(r,x-9,y-3,GREEN); sprout(r,x+8,y-3,GREEN);
        gfx_fill_rect(r,x-15,y+8,30,4,RGB565(129,111,73));
    }
}

/* 鼠标命中：菜单整体平移了 UI_DX / UI_DY，先换回原来 640x480 的坐标 */
int render_title_hit(int x,int y) {
    x-=UI_DX; y-=UI_DY;
    if(x>=181 && x<459 && y>=308 && y<340) return 0;
    if(x>=181 && x<459 && y>=348 && y<380) return 1;
    return -1;
}
int render_stress_hit(int x,int y) {
    int i;
    x-=UI_DX; y-=UI_DY;
    for(i=0;i<4;i++) if(x>=165 && x<475 && y>=190+i*46 && y<228+i*46) return i;
    return -1;
}
int render_upgrade_hit(int x,int y) {
    int i;
    x-=UI_DX; y-=UI_DY;
    for(i=0;i<3;i++) if(x>=32+i*198 && x<32+i*198+180 && y>=190 && y<373) return i;
    return -1;
}

/* 人数选择（标题 / 暂停，2026-10-28）：三个格子 1 2 3，当前选中的高亮；下面一行是设备分配。
 * restart = 1（暂停里）：按了会用新人数重开一局 */
static void party_picker(Gfx *r,const Game *g,int y,int restart) {
    static const char *const map[GAME_PLAYERS]={"P1: UART + USB KEYBOARD + KEYPAD",
        "P1: UART   P2: USB KEYBOARD / KEYPAD","P1: UART   P2: USB KEYBOARD   P3: KEYPAD"};
    const int sel=g->party_size<1?1:g->party_size>GAME_PLAYERS?GAME_PLAYERS:g->party_size;
    const char *label=restart?TR("PLAYERS - NEW RUN"):TR("PLAYERS");
    const int lw=gfx_text_width(label,1),bw=34,gap=8,total=lw+12+GAME_PLAYERS*bw+(GAME_PLAYERS-1)*gap;
    int x=UX(320)-total/2,i;
    char t[8];
    gfx_text(r,x,y+6,label,1,MUTED);
    x+=lw+12;
    for(i=0;i<GAME_PLAYERS;i++) {
        const int on=i+1==sel;
        gfx_fill_rect(r,x,y,bw,20,on?coop_fx_color(i):EDGE);
        snprintf(t,sizeof(t),"%d",i+1);
        center(r,x+bw/2,y+3,t,2,on?INK:CREAM);
        x+=bw+gap;
    }
    center(r,UX(320),y+26,TR(map[sel-1]),1,CREAM);
}
/* 多人升级（2026-10-28）：每个玩家一列，3 张卡竖着排；选了的那张高亮（玩家色框 + 底色 + PICKED），
 * 另外两张变暗；倒下的玩家显示 DOWN。全部选完后（upgrade_hold）整屏再停一会儿。 */
static void upgrade_party(Gfx *r,const Game *g) {
    char t[64];
    const int n=g->player_count,X0=40,W=SPROUT_W-80,Y0=94,H=560,cw=(W-20*(n+1))/n;
    int p,i;
    panel(r,X0,Y0,W,H,EDGE);
    snprintf(t,sizeof(t),TR("TEAM LEVEL %d / EVERYONE PICKS ONE"),g->team_level);
    center(r,SPROUT_W/2,Y0+14,t,3,GREEN);
    center(r,SPROUT_W/2,Y0+44,g->upgrade_hold?TR("ALL PICKED - GO!"):TR("PRESS 1 / 2 / 3 ON YOUR OWN KEYS - P1 MAY PICK FOR THE OTHERS"),1,g->upgrade_hold?GREEN:MUTED);
    for(p=0;p<n;p++) {
        const GamePlayer *a=game_player(g,p);
        const int x=X0+20+p*(cw+20),down=a->hp<=0,picked=g->pick_slot[p],waiting=(g->upgrade_mask>>p)&1;
        const Pixel col=down?MUTED:coop_fx_color(p);
        snprintf(t,sizeof(t),"P%d",p+1);
        gfx_text(r,x,Y0+68,t,3,col);
        gfx_text(r,x+52,Y0+75,down?TR("DOWN"):waiting?TR("CHOOSING..."):TR("READY"),1,down?RED:waiting?GOLD:GREEN);
        for(i=0;i<3;i++) {
            const int y=Y0+102+i*146,id=a->choices[i],sel=!down&&picked==i;
            panel(r,x,y,cw,134,sel?col:EDGE);
            if(down) { afill(r,x,y,cw,134,INK,160); continue; }
            if(sel) {
                afill(r,x+1,y+1,cw-2,132,col,70);
                gfx_border(r,x-3,y-3,cw+6,140,3,col);
            }
            snprintf(t,sizeof(t),"%d",i+1);
            gfx_text(r,x+12,y+12,t,3,sel?col:MUTED);
            upgrade_icon(r,id,x+cw/2,y+36);
            center(r,x+cw/2,y+62,TR(game_class_upgrade_name(p,id)),1,CREAM);
            center(r,x+cw/2,y+80,TR(game_class_upgrade_line(p,id,0)),1,GREEN);
            center(r,x+cw/2,y+96,TR(game_class_upgrade_line(p,id,1)),1,MUTED);
            if(sel) center(r,x+cw/2,y+112,TR("PICKED!"),2,col);
            if(picked>=0 && !sel) afill(r,x,y,cw,134,INK,150);
        }
    }
}

void render_game(const Game *g,Gfx *r,RenderInfo info) {
    char text[128]; int i; uint32_t t0=clock_now(),th,t;
    fx_alpha=info.alpha_fx;
    draw_world(r,g,info.batch);
    /* Any menu / pause / upgrade / defeat screen: dim the whole frame (60%),
     * then draw HUD and panel on top so FPS stays readable. This full-screen
     * read-modify-write is the main alpha load for the CPU-vs-BitBlt comparison. */
    if(fx_alpha && g->phase!=PLAYING) gfx_fill_alpha(r,0,0,SPROUT_W,SPROUT_H,INK,A_DIM);
    th=clock_now();
    gfx_text_cache(1);                         /* HUD / 面板 / 统计：文字大多不变，一串一条命令 */
    hud(r,g,info);
    if(!g->limit_mode&&g->phase==PLAYING)carrot_fx_ui(r,g,fx_alpha);
    /* 下面的面板都是原来 640x480 的排版，UX/UY 平移到 1280x720 中间 */
    if(g->phase==TITLE) {
        panel(r,UX(112),UY(86),416,366,EDGE);
        gfx_fill_rect(r,UX(112),UY(86),416,4,GREEN);
        center(r,UX(320),UY(108),TR("A TINY SURVIVAL GAME"),1,GREEN);
        center(r,UX(320),UY(137),TRX("@carrot SPROUT","SPROUT"),6,CREAM);
        center(r,UX(320),UY(185),TRX("@carrot RUN","RUN"),6,GOLD);
        gfx_blit(r,UX(320)-player.width/2,UY(256)-player.height/2,&player,1);   /* 1 级萝卜（1.5 倍） */
        center(r,UX(320),UY(282),TR("SIX REALMS / THREE CLASSES"),1,CREAM);   /* 中文 12 px：两行之间留 2 px */
        center(r,UX(320),UY(296),TR("DODGE. AUTO-FIRE. COLLECT. GROW."),1,MUTED);
        gfx_fill_rect(r,UX(181),UY(308),278,32,GREEN);
        center(r,UX(320),UY(318),TR("ENTER TO START"),2,INK);
        gfx_fill_rect(r,UX(181),UY(348),278,32,GOLD);
        center(r,UX(320),UY(358),TR("L STRESS TEST"),2,INK);
        center(r,UX(320),UY(392),TR("CHOOSE LOW / MEDIUM / HIGH / CUSTOM"),1,MUTED);
        party_picker(r,g,UY(410),0);
    } else if(g->phase==STRESS_SELECT) {
        static const char *labels[]={"1 LOW / 32 FOES / 1024 SHOTS",
            "2 MEDIUM / 64 FOES / 2048 SHOTS",
            "3 HIGH / 128 FOES / 4096 SHOTS"};
        panel(r,UX(112),UY(86),416,329,EDGE);
        center(r,UX(320),UY(112),TR("STRESS TEST"),3,GOLD);
        center(r,UX(320),UY(160),TR("SELECT LOAD / CLICK OR PRESS 1-4"),1,CREAM);
        for(i=0;i<4;i++) {
            Pixel color=i==0?GREEN:i==1?GOLD:i==2?RED:CYAN;
            gfx_fill_rect(r,UX(165),UY(190+i*46),310,38,color);
            if(i<3) center(r,UX(320),UY(203+i*46),TR(labels[i]),1,INK);
            else {
                snprintf(text,sizeof(text),TR("4 CUSTOM / 128 FOES / %d SHOTS"),game_stress_bullets());
                center(r,UX(320),UY(203+i*46),text,1,INK);
            }
        }
        center(r,UX(320),UY(375),TR("CUSTOM: - + COUNT / BRACKETS SIZE"),1,MUTED);
        center(r,UX(320),UY(392),TR("ESC / M / BACKSPACE TO TITLE"),1,MUTED);
    } else if(g->phase==UPGRADE && g->player_count>1) {
        upgrade_party(r,g);
    } else if(g->phase==UPGRADE) {
        panel(r,UX(20),UY(100),600,297,EDGE);
        center(r,UX(320),UY(120),TR("TIME TO GROW"),3,GREEN);
        {   /* 多人：轮到谁（他的设备或 P1 选）；单人和以前一样 */
            const GamePlayer *ua=game_player(g,g->player_count>1?g->upgrade_actor:0);
            if(g->player_count>1) snprintf(text,sizeof(text),TR("P%d LEVEL %d / CHOOSE YOUR UPGRADE"),g->upgrade_actor+1,ua->level);
            else snprintf(text,sizeof(text),TR("LEVEL %d / CHOOSE YOUR UPGRADE"),g->level);
        }
        center(r,UX(320),UY(152),text,1,g->player_count>1?coop_fx_color(g->upgrade_actor):CREAM);
        for(i=0;i<3;i++) {
            /* 2026-10-28：选中后（upgrade_hold 期间）选中的卡高亮（绿框 + 底色 + PICKED），另外两张变暗 */
            const int picked=g->upgrade_hold ? g->pick_slot[0] : -1;
            int x=UX(32+i*198),id=game_player(g,g->player_count>1?g->upgrade_actor:0)->choices[i],hover=picked<0&&render_upgrade_hit(info.mouse_x,info.mouse_y)==i;
            panel(r,x,UY(190),180,183,hover||picked==i?GREEN:EDGE);
            if(picked==i) {
                afill(r,x+1,UY(190)+1,178,181,GREEN,70);
                gfx_border(r,x-3,UY(190)-3,186,189,3,GREEN);
            }
            snprintf(text,sizeof(text),"0%d",i+1); gfx_text(r,x+12,UY(203),text,2,hover||picked==i?GREEN:MUTED);
            upgrade_icon(r,id,x+90,UY(245));
            center(r,x+90,UY(279),TR(game_class_upgrade_name(0,id)),1,CREAM);
            center(r,x+90,UY(301),TR(game_class_upgrade_line(0,id,0)),1,GREEN);
            center(r,x+90,UY(318),TR(game_class_upgrade_line(0,id,1)),1,MUTED);
            if(picked==i) center(r,x+90,UY(348),TR("PICKED!"),2,GREEN);
            else { snprintf(text,sizeof(text),TR("PRESS %d"),i+1); center(r,x+90,UY(348),text,1,hover?GREEN:GOLD); }
            if(picked>=0 && picked!=i) afill(r,x,UY(190),180,183,INK,150);
        }
        center(r,UX(320),UY(380),"1 / 2 / 3   AUTO PICK FIRST AFTER 5S",1,MUTED);
    } else if(g->phase==PAUSED) {
        panel(r,UX(135),UY(154),370,216,EDGE);
        center(r,UX(320),UY(177),TR("TAKE A BREATHER"),2,CREAM);
        center(r,UX(320),UY(215),TR("THE GARDEN CAN WAIT."),1,MUTED);
        center(r,UX(320),UY(252),TR("P / ESC TO RESUME"),2,GREEN);
        gfx_fill_rect(r,UX(190),UY(280),260,36,GOLD);
        center(r,UX(320),UY(291),TR("M / CLICK TO TITLE"),1,INK);
        if(!g->limit_mode) party_picker(r,g,UY(326),1);
    } else if(g->phase==VICTORY){
        panel(r,UX(110),UY(107),420,260,GOLD);
        center(r,UX(320),UY(145),TR("SIX REALMS CLEARED"),2,GOLD);
        center(r,UX(320),UY(205),TR("TWIN DRAGONS DEFEATED"),1,CREAM);
        center(r,UX(320),UY(280),TR("ENTER / R TO RESTART"),2,GREEN);
    } else if(g->phase==DEFEAT) {
        panel(r,UX(110),UY(107),420,297,RED);
        center(r,UX(320),UY(133),g->campaign.timed_out?TR("TIME LIMIT REACHED"):TR("BACK TO THE SOIL"),2,RED);
        center(r,UX(320),UY(171),TR("A SMALL POTATO. A BRAVE RUN."),1,CREAM);
        snprintf(text,sizeof(text),TR("TIME %02d:%02d"),g->ticks/3600,(g->ticks/60)%60);
        center(r,UX(320),UY(207),text,2,CREAM);
        if(g->player_count>1) {                  /* 多人：全队击杀 + 最高等级 */
            int p,kills=0,level=0;
            for(p=0;p<g->player_count;p++){ const GamePlayer *a=game_player(g,p); kills+=a->kills; if(a->level>level) level=a->level; }
            snprintf(text,sizeof(text),TR("TEAM %d KILLS / BEST LEVEL %d"),kills,level);
        } else snprintf(text,sizeof(text),TR("%d KILLS / LEVEL %d"),g->kills,g->level);
        center(r,UX(320),UY(241),text,2,GOLD);
        snprintf(text,sizeof(text),TR("BOSSES DEFEATED %d"),g->bosses_killed);
        center(r,UX(320),UY(279),text,1,MUTED);
        gfx_fill_rect(r,UX(174),UY(317),292,37,GOLD);
        center(r,UX(320),UY(329),TR("ENTER TO TRY AGAIN"),2,INK);
        center(r,UX(320),UY(378),TR("R RESTART   F1 STATS"),1,MUTED);
    }
#if SPROUT_STATS
    if(info.show_stats) stats_overlay(r,g,info);
#endif
    gfx_text_cache(0);
    t=clock_now();
    RSPLIT(hud,t-th);
    RSPLIT(total,t-t0);
}
