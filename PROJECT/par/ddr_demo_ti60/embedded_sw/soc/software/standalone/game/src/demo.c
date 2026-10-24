#include "demo.h"
#include "mini_snprintf.h"
#include "blit.h"
#include "gfx_hw.h"
#include "bsp.h"
#include <string.h>
#define MOT_REG(o) (*(volatile uint32_t *)(uintptr_t)(0xF8101300u+(o)))
#define DEMO_CLOCK() clint_getTimeLow(BSP_CLINT)
static int scene_w=1280,scene_h=720,scene_bottom=676;
static int motion_ok,motion_on=1,motion_fault,table_on=1;
static uint32_t tick_cycles,table_frames,motion_cycles;
#define MOT_BATCH 29984 /* 32-object multiple, <= old hardware 30000 cap */
static uint32_t square_revision;
static int fixed_square_size,motion_min_px=6;
int demo_motion_min_size(void){return motion_min_px;}
int demo_square_size(void){return fixed_square_size;}
static const Pixel square_palette[8]={
 RGB565(240,103,84),RGB565(63,181,217),RGB565(181,225,111),RGB565(242,190,93),
 RGB565(158,120,224),RGB565(239,106,135),RGB565(118,208,198),RGB565(242,237,213)};
void demo_geometry(int w,int h){scene_w=w;scene_h=h;scene_bottom=h-44;}
int demo_motion_toggle(void){if(motion_ok&&!motion_fault)motion_on=!motion_on;return demo_motion_enabled();}
int demo_motion_available(void){return motion_ok;}
int demo_motion_enabled(void){return motion_ok&&motion_on&&!motion_fault&&(!fixed_square_size||fixed_square_size>=motion_min_px);}
int demo_motion_error(void){return motion_fault;}
unsigned demo_motion_cycles(void){return motion_cycles;}
unsigned demo_motion_sequence(void){return motion_ok?MOT_REG(0x1c):0;}
unsigned demo_tick_cycles(void){return tick_cycles;}
unsigned demo_table_frames(void){return table_frames;}
int demo_table_enabled(void){return table_on;}
int demo_table_toggle(void){table_on=!table_on;return table_on;}

#define INK   RGB565(15,24,31)
#define CREAM RGB565(242,237,213)
#define MUTED RGB565(145,163,152)
#define GREEN RGB565(181,225,111)
#define GOLD  RGB565(242,190,93)
#define RED   RGB565(238,112,112)
#define CYAN  RGB565(118,208,198)

#define TOP_H  40            /* info bars; the scene lives in between */
#define BOT_Y  scene_bottom
#define FIXS   4             /* square positions in 1/16 px */

/* Same eight-byte object consumed by MOT2 and SQTAB. Q4 positions; signed Q4 velocities. */
typedef struct { uint32_t xy,motion; } Square;
static Square squares[DEMO_MAX_SQUARES] __attribute__((aligned(256)));
static uint32_t square_pixels[DEMO_MAX_SQUARES+1],square_alpha_pixels[DEMO_MAX_SQUARES+1];
static Square benchmark_snapshot[DEMO_MAX_SQUARES] __attribute__((aligned(256)));
typedef char square_is_eight_bytes[sizeof(Square)==8?1:-1];
static Sprite images[3];
static uint32_t rng = 0x2545F491u;

static uint32_t rnd(void) { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; }

static void new_square(Square *s)
{
    uint32_t random_size=6+rnd()%35;
    uint32_t size=fixed_square_size?(uint32_t)fixed_square_size:random_size;
    uint32_t x=(rnd()%(uint32_t)(scene_w-size))<<FIXS;
    uint32_t y=(TOP_H+rnd()%(uint32_t)(BOT_Y-TOP_H-size))<<FIXS;
    uint32_t vx=(uint8_t)((int)(rnd()%97)-48),vy=(uint8_t)((int)(rnd()%97)-48);
    s->xy=(y<<16)|x;s->motion=((rnd()&7u)<<24)|(size<<16)|(vy<<8)|vx;
}

/* ------------------------------------------------------------------------- */
/* Pictures for the switching demo (integer only, generated once at boot).    */
/* ------------------------------------------------------------------------- */
static void gen_image(Pixel *p, int kind)
{
    int x, y;
    for (y = 0; y < scene_h; y++) for (x = 0; x < scene_w; x++) {
        int r, g, b;
        if (kind == 0) {                      /* colour gradient */
            r = x * 255 / (scene_w - 1); g = y * 255 / (scene_h - 1);
            b = 255 - (x + y) * 255 / (scene_w + scene_h);
        } else if (kind == 1) {               /* rings over a checkerboard */
            int dx = x - scene_w / 2, dy = y - scene_h / 2;
            int ring = ((dx * dx + dy * dy) >> 10) & 1, chk = ((x >> 5) ^ (y >> 5)) & 1;
            r = ring ^ chk ? 240 : 30; g = ring ? 190 : 60; b = chk ? 90 : 200;
        } else {                              /* "munching squares" xor texture */
            r = (x ^ y) & 255; g = ((x * 2) ^ y) & 255; b = (y * 3) & 255;
        }
        p[y * scene_w + x] = RGB565(r, g, b);
    }
}

void demo_init(uint32_t img_base)
{
    static const char *names[3] = { "图像一", "图像二", "图像三" };
    int i;
    for (i = 0; i < 3; i++) {
        Gfx g;
        Pixel *p = (Pixel *)(uintptr_t)(img_base + (uint32_t)i * 0x200000u);
        gen_image(p, i);
        gfx_begin(&g, p, scene_w, scene_h);
        gfx_fill_rect(&g, scene_w/2-180, scene_h/2-60, 360, 120, INK);
        gfx_text(&g, scene_w/2 - gfx_text_width(names[i], 7) / 2, scene_h/2-28, names[i], 7, CREAM);
        gfx_end(&g);
        images[i].width = scene_w; images[i].height = scene_h;
        images[i].pixels = p; images[i].alpha = 0;
    }
    for (i = 0; i < DEMO_MAX_SQUARES; i++) {
        uint32_t size;new_square(&squares[i]);size=(squares[i].motion>>16)&255u;
        square_pixels[i+1]=square_pixels[i]+size*size;
        square_alpha_pixels[i+1]=square_alpha_pixels[i]+((i&1)?size*size:0u);
    }
    motion_ok=MOT_REG(0)==0x4d4f5432u && (MOT_REG(4)&0xffff0000u)==0x00020000u &&
              MOT_REG(0x38)==(uint32_t)scene_w && MOT_REG(0x3c)==((uint32_t)BOT_Y<<16|TOP_H) && MOT_REG(0x40)>=MOT_BATCH;
    unsigned min_px=motion_ok?MOT_REG(0x44)&65535u:0;
    motion_min_px=min_px>=1&&min_px<=6?(int)min_px:6;
}

/* ------------------------------------------------------------------------- */
/* Info bars: scene name + load on top, CPU vs HW comparison at the bottom.   */
/* ------------------------------------------------------------------------- */
static void overlay(Gfx *g, RenderInfo info, const char *title, const char *detail)
{
    const int bottom=g->height-44;
    char text[200];
    int w;
    gfx_fill_rect(g, 0, 0, g->width, TOP_H, INK);
    gfx_text(g, 12, 6, title, 2, CREAM);
    gfx_text(g, 12, 27, detail, 1, MUTED);
    snprintf(text, sizeof(text), "%d 帧/秒", info.fps);
    w = gfx_text_width(text, 2);
    gfx_text(g, g->width - 12 - w, 6, text, 2, info.fps >= 50 ? GREEN : info.fps >= 30 ? GOLD : RED);
    gfx_text(g, g->width - 12 - w - 70, 10, info.hw ? "硬件绘制" : "处理器绘制", 1, info.hw ? GREEN : GOLD);
    snprintf(text, sizeof(text), "绘制 %d 微秒", info.render_us);
    gfx_text(g, g->width - 12 - gfx_text_width(text, 1), 27, text, 1, CREAM);

    gfx_fill_rect(g, 0, bottom, g->width, g->height - bottom, INK);
    if (info.fps_mode[0] > 0)
        snprintf(text, sizeof(text), "处理器 %d帧 %d.%d毫秒", info.fps_mode[0],
                 info.draw_mode[0] / 1000, (info.draw_mode[0] % 1000) / 100);
    else snprintf(text, sizeof(text), "处理器 --帧");
    gfx_text(g, 12, bottom + 8, text, 2, info.hw ? MUTED : GREEN);
    if (info.fps_mode[1] > 0)
        snprintf(text, sizeof(text), "硬件 %d帧 %d.%d毫秒", info.fps_mode[1],
                 info.draw_mode[1] / 1000, (info.draw_mode[1] % 1000) / 100);
    else snprintf(text, sizeof(text), "硬件 --帧");
    gfx_text(g, 330, bottom + 8, text, 2, info.hw ? GREEN : MUTED);
    if (info.hw)
        snprintf(text, sizeof(text), "提交%d 队列%d 尾部%d 同步%d微秒 负载%d%% 读%d 写%d MB/s",
                 info.submit_us, info.qstall_us, info.tail_us, info.vsync_us, info.blt,
                 info.rd_mbs, info.wr_mbs);
    else
        snprintf(text, sizeof(text), "逻辑%d 绘制%d 同步%d微秒 N切换 G绘制 V同步 E透明",
                 info.logic_us, info.submit_us, info.vsync_us);
    gfx_text(g, 12, bottom + 30, text, 1, MUTED);
}

/* ------------------------------------------------------------------------- */
void demo_reset_squares(void)
{
    int i;if(blit_st.depth)blit_wait();
    if(motion_ok && (MOT_REG(8)&1u))return;
    rng=0x2545F491u;
    for(i=0;i<DEMO_MAX_SQUARES;i++){
        uint32_t size;new_square(&squares[i]);size=(squares[i].motion>>16)&255u;
        square_pixels[i+1]=square_pixels[i]+size*size;
        square_alpha_pixels[i+1]=square_alpha_pixels[i]+((i&1)?size*size:0u);
    }
    square_revision=0;motion_fault=0;blit_fence();
}

int demo_set_square_size(int pixels){
    if(pixels!=0&&pixels!=1&&pixels!=3&&(pixels<6||pixels>40))return 0;
    if(blit_st.depth)blit_wait();
    if(motion_ok&&(MOT_REG(8)&1u))return 0;
    blit_dcache_invalidate();fixed_square_size=pixels;
    square_pixels[0]=square_alpha_pixels[0]=0;
    for(int i=0;i<DEMO_MAX_SQUARES;i++){
        Square *s=&squares[i];uint32_t size=pixels?(uint32_t)pixels:6u+((uint32_t)i*2654435761u)%35u;
        uint32_t x=s->xy&65535u,y=s->xy>>16,mx=(scene_w-size)<<4,my=(BOT_Y-size)<<4;
        if(x>mx)x=mx;if(y>my)y=my;if(y<(TOP_H<<4))y=TOP_H<<4;
        s->xy=(y<<16)|x;s->motion=(s->motion&0xff00ffffu)|(size<<16);
        square_pixels[i+1]=square_pixels[i]+size*size;
        square_alpha_pixels[i+1]=square_alpha_pixels[i]+((i&1)?size*size:0u);
    }
    ++square_revision;blit_fence();return 1;
}

static void square_cpu_step(int count)
{
    int i;motion_cycles=0;blit_dcache_invalidate();
    for(i=0;i<count;i++){
        Square *s=&squares[i];uint32_t m=s->motion;int size=(m>>16)&255;
        int x=(s->xy&65535u)+(int8_t)m,y=(s->xy>>16)+(int8_t)(m>>8);
        int vx=(int8_t)m,vy=(int8_t)(m>>8),mx=(scene_w-size)<<4,my=(BOT_Y-size)<<4;
        if(x<0){x=-x;vx=-vx;}if(x>mx){x=2*mx-x;vx=-vx;}
        if(y<(TOP_H<<4)){y=(TOP_H<<5)-y;vy=-vy;}if(y>my){y=2*my-y;vy=-vy;}
        s->xy=((uint32_t)y<<16)|(uint32_t)x;
        s->motion=(m&0xffff0000u)|((uint32_t)(uint8_t)vy<<8)|(uint8_t)vx;
    }
    blit_fence();
}
static int square_hw_step(int count)
{
    uint32_t t,st;
    motion_cycles=0;blit_fence();
    for(int offset=0;offset<count;){
        int n=count-offset;if(n>MOT_BATCH)n=MOT_BATCH;
        MOT_REG(0x0c)=(uint32_t)(uintptr_t)(squares+offset);MOT_REG(0x10)=(uint32_t)n;
        MOT_REG(0x14)=1;t=DEMO_CLOCK();
        do{st=MOT_REG(8);if(DEMO_CLOCK()-t>BSP_CLINT_HZ){motion_fault=1;return 0;}}while(st&1u);
        if((st&14u)!=2u){motion_fault=1;return 0;}
        motion_cycles+=MOT_REG(0x18);offset+=n;
    }
    return 1;
}
void demo_squares_tick(int count)
{
    uint32_t t;if(count<0)count=0;if(count>DEMO_MAX_SQUARES)count=DEMO_MAX_SQUARES;
    /* The table is a DMA source. Never update it while the previous frame reads it. */
    if(blit_st.depth)blit_wait();
    t=DEMO_CLOCK();
    if(!motion_fault){if(demo_motion_enabled())square_hw_step(count);else square_cpu_step(count);}
    tick_cycles=DEMO_CLOCK()-t;++square_revision;
}

/* Same initial objects, one CPU and one hardware tick; compare every output byte.
 * Both paths advance the same state once. Return 1 only after real MOT2 completion. */
int demo_motion_benchmark(int count,unsigned *cpu,unsigned *hardware)
{
    uint32_t t;int i,ok=1;if(!motion_ok||motion_fault)return -1;
    if(count<1||count>DEMO_MAX_SQUARES||(fixed_square_size&&fixed_square_size<motion_min_px))return -1;
    if(blit_st.depth)blit_wait();blit_dcache_invalidate();
    memcpy(benchmark_snapshot,squares,sizeof squares);
    t=DEMO_CLOCK();square_cpu_step(count);*cpu=DEMO_CLOCK()-t;
    /* Expected CPU result is checked without allocating a third object array. */
    for(i=0;i<count;i++){
        Square initial=benchmark_snapshot[i];benchmark_snapshot[i]=squares[i];squares[i]=initial;
    }
    blit_fence();t=DEMO_CLOCK();if(!square_hw_step(count))return 0;*hardware=DEMO_CLOCK()-t;
    blit_dcache_invalidate();
    for(i=0;i<count;i++){
        Square expected=benchmark_snapshot[i];
        if(memcmp(&squares[i],&expected,sizeof expected))ok=0;
        /* Reverse one CPU tick to restore would be ambiguous at a bounce; the
         * benchmark therefore keeps the common one-tick result as live state. */
    }
    ++square_revision;return ok;
}

void demo_squares_render(Gfx *g, RenderInfo info, int count)
{
    char detail[200];
    int i;
    if (count > DEMO_MAX_SQUARES) count = DEMO_MAX_SQUARES;
    gfx_fill_rect(g, 0, TOP_H, scene_w, BOT_Y - TOP_H, RGB565(24, 38, 36));
    /* A failed in-place DMA may still own the table. Do not read it on either
     * backend until an explicit, idle-only reset has rebuilt the objects. */
    if(motion_fault){
        overlay(g,info,"720p 运动方块压力测试","硬件运动出错，已停止；请重新启动程序");
        return;
    }
    if(info.hw && table_on && blit_st.mode==BLIT_MODE_LIST && (blit_rd(BLIT_R_CAPS2)&8u)){
        const GfxHwTarget *t=(const GfxHwTarget *)g->context;
        uint32_t *p;
        blit_list_draw_ctx(t->fb,t->pitch,0);
        for(int offset=0;offset<count;){
            /* Even chunks preserve SQTAB's odd/even Alpha pattern. */
            int n=count-offset;if(n>MOT_BATCH)n=MOT_BATCH;
            blit_band_admit((uint32_t)n);
            p=blit_list_reserve(2);p[0]=0x60000000u|(info.alpha_fx?0x10000u:0u)|(uint32_t)n;
            p[1]=(uint32_t)(uintptr_t)(squares+offset);blit_list_commit(p+2);offset+=n;
        }
        ++table_frames;
        BLIT_STAT(blit_st.cmds+=(uint32_t)count);
        g->fills+=(uint32_t)count-(info.alpha_fx?(uint32_t)count/2u:0u);
        g->pixels_written+=square_pixels[count];
        if(info.alpha_fx){g->alphas+=(uint32_t)count/2u;g->alpha_pixels+=square_alpha_pixels[count];}
    }else{
        blit_dcache_invalidate();
        for (i = 0; i < count; i++) {
            const Square *s = &squares[i];int size=(s->motion>>16)&255;
            int x=(s->xy&65535u)>>4,y=s->xy>>20;Pixel c=square_palette[s->motion>>24];
            if(info.alpha_fx&&(i&1))gfx_fill_alpha(g,x,y,size,size,c,150);
            else gfx_fill_rect(g,x,y,size,size,c);
        }
    }
    snprintf(detail,sizeof detail,"方块%d %dx%d 运动%s 耗时%u微秒%s",count,scene_w,scene_h,
             motion_fault?"错误":demo_motion_enabled()?"硬件":"处理器",(unsigned)(tick_cycles/(BSP_CLINT_HZ/1000000u)),
             info.alpha_fx?" 透明开启":" 透明关闭");
    overlay(g, info, "720p 运动方块压力测试", detail);
    gfx_text(g,800,scene_h-36,"逗号减100 句号加100 上限99999",1,CREAM);
}

void demo_images_render(Gfx *g, RenderInfo info, uint32_t frame, int period)
{
    char detail[200];
    int k = (int)((frame / (uint32_t)(period < 1 ? 1 : period)) % 3u);
    int bar = (int)((frame * 8u) % (uint32_t)scene_w);
    gfx_blit(g, 0, 0, &images[k], 0);                       /* 614 KB per frame */
    gfx_fill_rect(g, bar, TOP_H, 8, scene_h-44-TOP_H, CREAM);  /* moves 8 px / frame */
    snprintf(detail, sizeof(detail), "1280x720 每帧整图复制 图像%d/3 每%d帧切换",
             k + 1, period);
    overlay(g, info, "大图切换演示", detail);
}
