/* Visual rules adapted from the referenced FUS1 soft-light/tint effects.
 * Immutable radial/tint assets are prepared in DDR; runtime Alpha/additive
 * composition uses the existing blit DSP pipeline. No extra BRAM required. */
#include "coop_fx.h"
#include "mini_snprintf.h"
#include <string.h>
#ifdef __riscv
#define FX_DDR __attribute__((section(".artbss"),aligned(64)))
#else
#define FX_DDR __attribute__((aligned(64)))
#endif
static Pixel glow_pixels[3][64*64] FX_DDR;
static uint8_t glow_alpha[64*64] FX_DDR,ring_alpha[8][64*64] FX_DDR;
static Sprite glows[3],rings[3][8];static int ready;
Pixel coop_fx_color(int p){static const Pixel c[]={RGB565(124,240,160),RGB565(95,195,255),RGB565(255,192,95)};return c[(unsigned)p<3u?p:0];}
static void init(void){
 if(ready)return;ready=1;
 for(int y=0;y<64;y++)for(int x=0;x<64;x++){
  int dx=2*x-63,dy=2*y-63,r=dx*dx+dy*dy,k=y*64+x;
  glow_alpha[k]=r<4096?(uint8_t)((4096-r)*180/4096):0;
  for(int f=0;f<8;f++){int radius=14+f*6,delta=r-radius*radius;if(delta<0)delta=-delta;ring_alpha[f][k]=delta<radius*5?(uint8_t)((radius*5-delta)*220/(radius*5)):0;}
  for(int p=0;p<3;p++)glow_pixels[p][k]=coop_fx_color(p);
 }
 for(int p=0;p<3;p++){glows[p]=(Sprite){64,64,glow_pixels[p],glow_alpha};for(int f=0;f<8;f++)rings[p][f]=(Sprite){64,64,glow_pixels[p],ring_alpha[f]};}
}
#define TINT_SLOTS 64
static Pixel tinted[TINT_SLOTS][3600] FX_DDR;
static struct {const Pixel *source;int w,h,player;Sprite result;} cache[TINT_SLOTS];static int used;
const Sprite *coop_fx_tint(const Sprite *s,int player,Gfx *g){
 if(player<=0||player>2||s->width*s->height>3600)return s;
 for(int i=0;i<used;i++)if(cache[i].source==s->pixels&&cache[i].w==s->width&&cache[i].h==s->height&&cache[i].player==player)return &cache[i].result;
 if(used==TINT_SLOTS)return s;
 int n=used++;for(int i=0;i<s->width*s->height;i++){
  Pixel c=s->pixels[i];if(c==COLOR_KEY){tinted[n][i]=c;continue;}
  int r=c>>11,gr=(c>>5)&63,b=c&31;
  int mid=(r*2+(gr>>1)*3+b)/6,hi=r,lo=r;
  if((gr>>1)>hi)hi=gr>>1;if(b>hi)hi=b;if((gr>>1)<lo)lo=gr>>1;if(b<lo)lo=b;
  /* Preserve neutral face/eye highlights; recolour saturated clothing/armour. */
  if(hi-lo>5){
   if(player==1){r=(r*80+mid*64)/256;gr=(gr*160+mid*192)/256;b=(b*100+mid*260)/256+3;}
   else{if(r<mid+8)r=mid+8;gr=(gr*50+mid*400)/256;b=b*100/256;}
  }
  if(r>31)r=31;if(gr>63)gr=63;if(b>31)b=31;tinted[n][i]=(Pixel)(r<<11|gr<<5|b);if(tinted[n][i]==COLOR_KEY)tinted[n][i]^=1;
 }
 cache[n].source=s->pixels;cache[n].w=s->width;cache[n].h=s->height;cache[n].player=player;cache[n].result=(Sprite){s->width,s->height,tinted[n],s->alpha};
 if(g->ops&&g->ops->publish)g->ops->publish(g->context);return &cache[n].result;
}
void coop_fx_actor(Gfx *g,int x,int y,int player,unsigned tick,int moving,int skill,int hit,int enabled){
 if((unsigned)player>=3)return;
 if(!ready){init();if(g->ops&&g->ops->publish)g->ops->publish(g->context);}
 if(enabled){
  gfx_blit_add(g,x-32,y-16,&glows[player],0,55);
  if(moving)gfx_blit_add(g,x-32,y-4,&glows[player],0,30);
  if(skill)gfx_blit_add(g,x-32,y-32,&rings[player][(tick>>1)&7],0,220);
  if(hit)gfx_blit_add(g,x-32,y-32,&glows[player],0,170);
 }
 char label[8];snprintf(label,sizeof label,"P%d",player+1);gfx_text(g,x-6,y+30,label,1,coop_fx_color(player));
}
