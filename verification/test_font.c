/* Exercise cache reuse and UTF-8 edge cases with the actual renderer. */
#include "../PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/gfx.c"
#include <assert.h>
#include <stdio.h>
static Pixel pixels[640*480+64];
static int publishes,waits;
static void publish(void *p){(void)p;++publishes;}
static void wait_idle(void *p){(void)p;++waits;}
int main(void){
    Gfx g;int i;unsigned used,epoch;
    GfxOps ops={0};ops.publish=publish;ops.wait_idle=wait_idle;
    gfx_begin(&g,pixels,640,480);
    assert(gfx_text_width("生命",1)==25&&gfx_text_width("60帧",2)==40);
    assert(gfx_text_width("",1)==0&&gfx_text_width("生命",0)==0);
    gfx_text(&g,-8,-8,"萝卜求生 生命 技能 就绪",1,1234);
    gfx_text(&g,635,475,"中文边界",2,4321);
    gfx_text(&g,0,0,"\xe4\xb8",1,1234); /* truncated UTF-8, must not read beyond terminator */
    for(i=640*480;i<640*480+64;i++)assert(pixels[i]==0);
    /* Null blit hooks fall back to the CPU framebuffer but exercise font cache. */
    g.ops=&ops;g.pixels=pixels;gfx_font_reset(&g);
    for(i=0;i<5;i++){
        gfx_text(&g,10,10,"萝卜求生 自定 敌128 弹13880/14000",1,1234);
        gfx_text(&g,10,30,"生命 999/999 经验0/10 耗时16.63 预算99% 等级01",1,2345);
        gfx_text(&g,10,50,"逻辑13892 峰13928 同步0 提交2730 队等0 尾等2",1,3456);
        gfx_text(&g,10,70,"硬件绘制 J 环形爆发 就绪",1,4567);
    }
    used=font_used;epoch=font_epoch;publishes=0;
    for(i=0;i<120;i++){
        gfx_text(&g,10,10,"萝卜求生 自定 敌128 弹13880/14000",1,1234);
        gfx_text(&g,10,30,"生命 999/999 经验0/10 耗时16.63 预算99% 等级01",1,2345);
        gfx_text(&g,10,50,"逻辑13892 峰13928 同步0 提交2730 队等0 尾等2",1,3456);
        gfx_text(&g,10,70,"硬件绘制 J 环形爆发 就绪",1,4567);
    }
    assert(font_used==used&&font_epoch==epoch&&publishes==0);
    printf("PASS UTF-8 width, clipping, truncation, shared-cache reuse: %u/%u pixels; no allocation/reset for 120 warm frames\n",used,GFX_FONT_POOL_PIXELS);
    return 0;
}
