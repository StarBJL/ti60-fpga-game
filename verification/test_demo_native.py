"""Host-render the real demo implementation; replace only board MMIO/driver calls."""
from pathlib import Path
import subprocess
v=Path(__file__).parent;src=v.parent/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src'
code=(src/'demo.c').read_text(encoding='utf-8')
code=code.replace('#include "blit.h"','').replace('#include "bsp.h"','')
code=code.replace('#define MOT_REG(o) (*(volatile uint32_t *)(uintptr_t)(0xF8101300u+(o)))','#define MOT_REG(o) fake_regs[(o)/4]')
prefix='''#include <stdint.h>
#define BSP_CLINT 0
#define BSP_CLINT_HZ 200000000u
#define BLIT_MODE_LIST 1
#define BLIT_R_CAPS2 0xbc
#define BLIT_STAT(x) ((void)0)
static uint32_t fake_regs[32],list_words[4];
static struct {int depth,mode;unsigned cmds;}blit_st;
static uint32_t clint_getTimeLow(int p){(void)p;return 0;}
static void blit_wait(void){}
static void blit_fence(void){}
static void blit_dcache_invalidate(void){}
static uint32_t blit_rd(int p){(void)p;return 0;}
static void blit_list_draw_ctx(uint32_t a,uint32_t b,int c){(void)a;(void)b;(void)c;}
static uint32_t *blit_list_reserve(int n){(void)n;return list_words;}
static void blit_list_commit(uint32_t *p){(void)p;}
'''
suffix='''
#include <windows.h>
#include <assert.h>
#include <stdio.h>
static Pixel fb[1280*720+32];
int main(void){
 int i,k;Gfx g;RenderInfo info={0};void *bank=VirtualAlloc((void*)0x20000000,3*0x200000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
 assert(bank==(void*)0x20000000);memset(bank,0x5a,3*0x200000);
 demo_geometry(1280,720);demo_init(0x20000000);info.fps=60;info.alpha_fx=1;
 for(i=0;i<3;i++){
  Pixel *p=(Pixel*)((char*)bank+i*0x200000);assert(images[i].width==1280&&images[i].height==720);
  for(k=1280*720;k<0x200000/2;k++)assert(p[k]==0x5a5a);
  for(k=1280*720;k<1280*720+32;k++)fb[k]=0x5aa5;
  gfx_begin(&g,fb,1280,720);demo_images_render(&g,info,i*20,20);gfx_end(&g);
  for(k=1280*720;k<1280*720+32;k++)assert(fb[k]==0x5aa5);
  if(i==1){FILE *f=fopen("images_native.rgb565","wb");assert(f);fwrite(fb,2,1280*720,f);fclose(f);}
 }
 gfx_begin(&g,fb,1280,720);demo_squares_tick(30000);demo_squares_render(&g,info,30000);gfx_end(&g);
 for(k=1280*720;k<1280*720+32;k++)assert(fb[k]==0x5aa5);
 {FILE *f=fopen("squares_native.rgb565","wb");assert(f);fwrite(fb,2,1280*720,f);fclose(f);}
 puts("PASS native demos: three 1280x720 images fit in 2MB slots, Chinese overlays, 30K moving squares and framebuffer guards");
 VirtualFree(bank,0,MEM_RELEASE);return 0;
}
'''
(v/'demo_native_generated.c').write_text(prefix+code+suffix,encoding='utf-8')
gcc='D:/vivdo2021/Vivado/2021.2/tps/mingw/6.2.0/win64.o/nt/bin/gcc.exe'
subprocess.run([gcc,'-O2','-I',str(src),str(v/'demo_native_generated.c'),str(src/'gfx.c'),str(src/'mini_snprintf.c'),'-o',str(v/'test_demo_native.exe')],check=True)
subprocess.run([str(v/'test_demo_native.exe')],cwd=v,check=True)
