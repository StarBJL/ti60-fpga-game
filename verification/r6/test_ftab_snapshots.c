#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#define BSP_CLINT 0
#define BSP_CLINT_HZ 200000000u
#define BLIT_MODEL_PTR(a) ((uint8_t *)(uintptr_t)(a))
#include "blit_model.h"
#include "blit.h"
#include "gfx_hw.h"
static uint32_t fake_regs[32],clock_ticks;
static uint32_t test_clock(void){return ++clock_ticks;}
#include "demo_under_test.h"
static uint16_t expected[1280*720];
static void render(int path,int n,int alpha){
 Gfx g;GfxHwTarget target;RenderInfo info={0};info.hw=1;info.alpha_fx=alpha;
 assert(demo_set_square_path(path));blit_set_band(1);
 gfx_begin_hw(&g,&target,0x1000000,1280,720);demo_squares_render(&g,info,n);gfx_end(&g);blit_wait();
 assert(!bm_lerr&&!bm_fault&&!bm_berr);
}
int main(void){
 void *fb=mmap((void*)0x1000000,0x1000000,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED,-1,0);assert(fb==(void*)0x1000000);
 blit_init();demo_init(0x1600000);blit_set_mode(BLIT_MODE_LIST);
 const int sizes[]={1,3,6,0},counts[]={17000,65537,99999};
 for(int sz=0;sz<4;sz++)for(int k=0;k<3;k++)for(int alpha=0;alpha<2;alpha++){
  const int n=counts[k];assert(demo_set_square_size(sizes[sz]));assert(demo_set_square_path(2));demo_square_alpha(alpha);demo_squares_tick(n);
  int first=fill_db;size_t bytes=n*sizeof(GfxFillItem);void *snapshot=malloc(bytes);assert(snapshot);memcpy(snapshot,fill_tables[first],bytes);
  demo_squares_tick(n);assert(!memcmp(snapshot,fill_tables[first],bytes));demo_squares_tick(n);assert(!memcmp(snapshot,fill_tables[first],bytes));free(snapshot);
  render(1,n,alpha);memcpy(expected,(void*)0x1000000,sizeof expected);render(2,n,alpha);
  uint16_t *actual=(void*)0x1000000;for(int i=40*1280;i<676*1280;i++)assert(actual[i]==expected[i]);
  printf("PASS FTAB/SQTAB pixel equality size=%d count=%d alpha=%d; two previous DMA snapshots unchanged\n",sizes[sz],n,alpha);
 }
 return 0;
}
