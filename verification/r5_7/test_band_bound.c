#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#define BSP_CLINT 0
#define BSP_CLINT_HZ 200000000u
#define BLIT_MODEL_PTR(a) ((uint8_t *)(uintptr_t)(a))
#include "blit_model.h"
#include "blit.h"
#include "gfx_hw.h"
static uint32_t fake_regs[32],jobs,processed,clock_ticks;
static uint32_t test_clock(void){
 if(fake_regs[0x14/4]){
  unsigned n=fake_regs[4];uint32_t address=fake_regs[3];
  assert(!(address&255)&&n<=30000&&address>=0x1000&&address+n*8<0xf00000);
  uint32_t *p=(void*)(uintptr_t)address;assert(p);unsigned hist[880]={0},peak=0;
  for(unsigned i=0;i<n;i++){
   uint32_t xy=p[2*i],m=p[2*i+1];int size=m>>16&255,x=(xy&65535)+(int8_t)m,y=(xy>>16)+(int8_t)(m>>8);
   int vx=(int8_t)m,vy=(int8_t)(m>>8),mx=(1280-size)*16,my=(676-size)*16;
   if(x<0){x=-x;vx=-vx;}else if(x>mx){x=2*mx-x;vx=-vx;}
   if(y<640){y=1280-y;vy=-vy;}else if(y>my){y=2*my-y;vy=-vy;}
   p[2*i]=(uint32_t)y<<16|x;p[2*i+1]=(m&0xffff0000)|((uint32_t)(uint8_t)vy<<8)|(uint8_t)vx;
   for(int ty=(y>>4)/32;ty<=((y>>4)+size-1)/32;ty++)for(int tx=(x>>4)/32;tx<=((x>>4)+size-1)/32;tx++){unsigned n=++hist[ty*40+tx];if(n>peak)peak=n;}
  }
  fake_regs[0x34/4]=peak;processed+=n;jobs++;fake_regs[5]=0;fake_regs[2]=2;fake_regs[6]=100+n*23;fake_regs[7]++;
 }
 return ++clock_ticks;
}
#include "demo_under_test.h"
static uint16_t golden[1280*720];
int main(void){
 void *fb=mmap((void*)0x1000000,0x1000000,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED,-1,0);assert(fb==(void*)0x1000000);
 fake_regs[0]=0x4d4f5432;fake_regs[1]=0x20000;fake_regs[14]=1280;fake_regs[15]=(676<<16)|40;fake_regs[16]=30000;fake_regs[17]=(40<<16)|1;
 blit_init();demo_init(0x1600000);assert(demo_motion_available());
 const int sizes[]={1,3,6,0};
 for(int j=0;j<4;j++){
  assert(demo_set_square_size(sizes[j]));unsigned cpu,hardware;processed=jobs=0;
  assert(demo_motion_benchmark(99999,&cpu,&hardware)==1);assert(processed==99999&&jobs==4);
  assert(demo_motion_cycles()==99999*23+400);
  unsigned bands[45]={0},max=0;
  for(int k=0;k<99999;k++){unsigned y=squares[k].xy>>20,size=squares[k].motion>>16&255;
   for(unsigned b=y/16;b<=(y+size-1)/16;b++){unsigned n=++bands[b];if(n>max)max=n;}}
  assert(demo_band_bound()>=max&&demo_band_bound()<=99999);
  if(j<3)assert(demo_band_bound()<32000);
  if(j<3){
   Gfx g;RenderInfo info={0};info.alpha_fx=1;info.hw=0;
   /* Independent 8-bit Alpha reference (CPU backend intentionally uses Alpha5). */
   for(int k=0;k<1280*720;k++)golden[k]=RGB565(24,38,36);
   for(int k=0;k<99999;k++){
    const Square *sq=&squares[k];int x=(sq->xy&65535)>>4,y=sq->xy>>20,n=(sq->motion>>16)&255;
    unsigned c=square_palette[sq->motion>>24],a=k&1?151:256;
    for(int yy=0;yy<n;yy++)for(int xx=0;xx<n;xx++){
     unsigned d=golden[(y+yy)*1280+x+xx],out=0;const int sh[]={11,5,0},mask[]={31,63,31};
     for(int t=0;t<3;t++)out|=(( ((c>>sh[t])&mask[t])*a+((d>>sh[t])&mask[t])*(256-a)+128)>>8)<<sh[t];
     golden[(y+yy)*1280+x+xx]=out;
    }
   }
   /* Compare world only: HUD intentionally reports different backend names. */
   info.hw=1;blit_set_mode(BLIT_MODE_LIST);blit_set_band(1);GfxHwTarget target;
   gfx_begin_hw(&g,&target,0x1000000,1280,720);demo_squares_render(&g,info,99999);gfx_end(&g);blit_wait();
   uint16_t *actual=(void*)0x1000000;fprintf(stderr,"j %d items %u commands %u\n",j,bm_sq_items,bm_sq_commands);assert(bm_sq_items==(unsigned)(j+1)*99999&&bm_sq_commands==(unsigned)(j+1)*4);assert(!bm_lerr&&!bm_fault&&!bm_berr);
   for(int i=40*1280;i<676*1280;i++)assert(actual[i]==golden[i]);
   assert(blit_st.bsplits==0);
  }
 }
 /* Dense objects are a negative control: the bound must reject band mode. */
 assert(demo_set_square_size(1));for(int i=0;i<99999;i++){squares[i].xy=(320u<<16)|320u;squares[i].motion=1u<<16;}
 demo_squares_tick(99999);assert(demo_band_bound()==99999);
 Gfx dense;GfxHwTarget target;RenderInfo info={0};info.hw=info.alpha_fx=1;
 gfx_begin_hw(&dense,&target,0x1000000,1280,720);demo_squares_render(&dense,info,99999);gfx_end(&dense);blit_wait();assert(blit_st.bsplits>0&&!bm_lerr&&!bm_fault);
 motion_min_px=6;assert(demo_set_square_size(1));assert(!demo_motion_enabled());processed=jobs=0;demo_squares_tick(99999);assert(!jobs&&!processed&&!motion_fault&&motion_cycles==0);
 assert(!demo_set_square_size(2)&&!demo_set_square_size(41));
 puts("PASS 99999 squares, 4 aligned MOT2 jobs, CPU/HW motion equality, 1/3/6px SQTAB alpha parity and full world pixels");
}
