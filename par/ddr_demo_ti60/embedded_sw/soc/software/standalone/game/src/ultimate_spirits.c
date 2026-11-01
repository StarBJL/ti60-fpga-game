/* Compact four-stage ultimate animation, built once from the existing embedded
 * masters. All frames stay immutable while asynchronous BitBlt readers run.
 * Ordinary BSS is used; the existing art arena and linked data are unchanged. */
#include "ultimate_spirits.h"
#include "scene_assets.h"
extern const uint8_t ultimate_tiger_packed[],ultimate_phoenix_packed[];
extern const unsigned ultimate_tiger_size,ultimate_phoenix_size;
#define FRAME_PIXELS (160*90+224*126+288*162+320*180)
static uint8_t decode_scratch[768*432*3] __attribute__((aligned(64)));
static uint8_t frame_raw[2][FRAME_PIXELS*3] __attribute__((aligned(64)));
static Pixel mote_px[2][4][32*16];
static uint8_t mote_a[2][4][32*16];
static Sprite spirits[2][4],motes[2][4];
static int initialized;
static const int16_t wave[64]={
 0,25,50,74,98,121,142,162,181,198,213,226,237,245,251,255,
 256,255,251,245,237,226,213,198,181,162,142,121,98,74,50,25,
 0,-25,-50,-74,-98,-121,-142,-162,-181,-198,-213,-226,-237,-245,-251,-255,
 -256,-255,-251,-245,-237,-226,-213,-198,-181,-162,-142,-121,-98,-74,-50,-25};
static int sn(int a){return wave[(unsigned)a&63u];}
static int cs(int a){return sn(a+16);}
static int ab(int a){return a<0?-a:a;}
static int clamp(int a,int lo,int hi){return a<lo?lo:a>hi?hi:a;}
static unsigned hash(unsigned i,unsigned cast){unsigned n=i*374761393u+cast*668265263u;n=(n^(n>>13))*1274126177u;return n^(n>>16);}
static void build_frames(int kind,int sw,int sh){
 static const int w[4]={160,224,288,320},h[4]={90,126,162,180};
 const Pixel*src=(const Pixel*)(const void*)decode_scratch;
 const uint8_t*sa=decode_scratch+sw*sh*2;
 unsigned offset=0;
 for(int f=0;f<4;f++){
  int n=w[f]*h[f];Pixel*px=(Pixel*)(void*)(frame_raw[kind]+offset);
  uint8_t*a=frame_raw[kind]+offset+n*2;
  for(int y=0;y<h[f];y++)for(int x=0;x<w[f];x++){
   int sx=x*sw/w[f],sy=y*sh/h[f],q=y*w[f]+x;
   if(kind){
    /* Drooped wings open into the final span across the four actual frames. */
    int edge=ab(sx*2-sw);
    sy-=(int)((int64_t)(3-f)*edge*edge*sh/(sw*sw*12));
   }else sx+=sn(y*64/h[f]+f*5)*(3-f)*5/256;
   if(sx>=0&&sx<sw&&sy>=0&&sy<sh){px[q]=src[sy*sw+sx];a[q]=sa[sy*sw+sx];}
   else{px[q]=0;a[q]=0;}
   int edge=x<w[f]-1-x?x:w[f]-1-x;
   if(y<edge)edge=y;if(h[f]-1-y<edge)edge=h[f]-1-y;
   if(edge<3)a[q]=(uint8_t)(a[q]*edge/3); /* feather the outer texture boundary */
  }
  spirits[kind][f]=(Sprite){w[f],h[f],px,a};offset+=(unsigned)n*3u;
 }
}
int ultimate_spirits_init(void){
 if(initialized)return initialized==1;
 if(!scene_lz4_decode(ultimate_tiger_packed,ultimate_tiger_size,decode_scratch,640*360*3)){initialized=-1;return 0;}
 build_frames(0,640,360);
 if(!scene_lz4_decode(ultimate_phoenix_packed,ultimate_phoenix_size,decode_scratch,768*432*3)){initialized=-1;return 0;}
 build_frames(1,768,432);
 for(int role=0;role<2;role++)for(int f=0;f<4;f++){
  for(int y=0;y<16;y++)for(int x=0;x<32;x++){
   int dx=x-16,dy=y-8,u=(dx*cs(f*5)+dy*sn(f*5))/256;
   int v=(-dx*sn(f*5)+dy*cs(f*5))/256,q=y*32+x;
   int width=role?(14-ab(u))/3:2-ab(u)/7;
   int inside=ab(u)<14&&ab(v)<=width;
   mote_px[role][f][q]=role?(ab(v)<1?RGB565(255,246,230):RGB565(255,105,180)):
                                    (ab(v)<1?RGB565(220,255,255):RGB565(45,155,255));
   mote_a[role][f][q]=inside?(uint8_t)clamp((14-ab(u))*25,0,220):0;
  }
  motes[role][f]=(Sprite){32,16,mote_px[role][f],mote_a[role][f]};
 }
 initialized=1;return 1;
}
static void mote(Gfx*r,int kind,int frame,int x,int y,int opacity){
 gfx_blit_alpha(r,x-16,y-8,&motes[kind][frame&3],0,clamp(opacity,0,255));
}
void ultimate_spirit_draw(Gfx*r,const CarrotFighter*f,int role,int alpha){
 if(initialized!=1||role<1||role>2||!f->skill_ticks)return;
 int kind=role-1,wind=f->skill_total/3,age=f->skill_age,release=age-wind;
 int fade=f->skill_ticks<24?f->skill_ticks*255/24:255;
 int ox=f->skill_x,oy=f->skill_y-8;
 if(wind<1)wind=1;
 if(release<0){
  for(int k=0;k<12;k++){
   int phase=(age*2+k*7)&63,radius=48-age*28/wind+(k%3)*5;
   mote(r,kind,(age/5+k)&3,ox+cs(phase)*radius/256,
        oy+sn(phase)*radius/640-(kind?age*30/wind:0),fade*(70+age*100/wind)/255);
  }
  return;
 }
 int fi=clamp(release/8,0,3);
 if(f->skill_ticks<24)fi=clamp(f->skill_ticks/6,0,fi);
 const Sprite*s=&spirits[kind][fi];
 int reveal=release<8?(release+1)*255/8:255,strength=fade*reveal/255;
 int cx=ox+(kind?-18:18),cy=oy-(kind?42:25)-(kind?clamp(release/3,0,22):0);
 cx=clamp(cx,s->width/2+16,r->width-s->width/2-16);
 cy=clamp(cy,s->height/2+72,r->height-s->height/2-48);
 int left=cx-s->width/2,top=cy-s->height/2,pulse=201+sn(release*2)*19/256;
 if(kind){
  for(int x=0;x<s->width;x+=16){
   int width=s->width-x<16?s->width-x:16;
   int lift=sn(release*2)*ab(x-s->width/2)/(256*22);
   gfx_blit_part_alpha(r,left+x,top+lift,s,x,0,width,s->height,0,strength*pulse/255);
  }
 }else{
  for(int y=0;y<s->height;y+=12){
   int height=s->height-y<12?s->height-y:12;
   gfx_blit_part_alpha(r,left+sn(release*3+y/6)/256,top+y,s,0,y,s->width,height,0,strength*pulse/255);
  }
 }
 for(int k=0;k<18;k++){
  unsigned n=hash((unsigned)k,(unsigned)f->shot_cycle);
  int phase=(release*3+(int)(n&63u))&63,x,y;
  if(kind){
   x=ox+(int)((n>>8)%181u)-90+sn(phase+k)*8/256;
   y=oy+18-phase*2-(int)((n>>18)&31u);
  }else{
   int radius=42+phase,angle=k*64/18;
   x=ox+cs(angle)*radius/256;y=oy+sn(angle)*radius/384;
  }
  int opacity=strength*(64-phase)*2/255;
  mote(r,kind,k+release/5,x,y,opacity);
  if(alpha&&(k%6)==0)gfx_fill_add(r,x,y,2,2,kind?RGB565(255,225,240):RGB565(180,245,255),opacity);
 }
}
