#include "campaign_art.h"
#include "scene_assets.h"
#ifdef __riscv
#define MEM __attribute__((section(".artbss"),aligned(64)))
#else
#define MEM __attribute__((aligned(64)))
#endif
extern const uint8_t v4_bg0[],v4_bg1[],v4_bg2[],v4_bg3[],v4_bg4[],v4_bg5[];
extern const unsigned v4_bg0_size,v4_bg1_size,v4_bg2_size,v4_bg3_size,v4_bg4_size,v4_bg5_size;
extern const Pixel v4_hero_pixels[3][9][3600];
static Pixel backgrounds[6][1280*720] MEM;
static Pixel crystals[6][24*28] MEM;
static Pixel burst_pixels[3][60*60] MEM;
static Sprite bg[6],heroes[3][9],gems[6],burst_heroes[3];
static int ready;
int campaign_art_init(void){
 if(ready)return ready==1;
 const uint8_t *data[6]={v4_bg0,v4_bg1,v4_bg2,v4_bg3,v4_bg4,v4_bg5};
 unsigned sizes[6]={v4_bg0_size,v4_bg1_size,v4_bg2_size,v4_bg3_size,v4_bg4_size,v4_bg5_size};
 int s,x,y,p,l;
 for(s=0;s<6;s++){
  if(!scene_lz4_decode(data[s],sizes[s],(uint8_t*)backgrounds[s],sizeof backgrounds[s])){ready=-1;return 0;}
  bg[s]=(Sprite){1280,720,backgrounds[s],0};
  for(y=0;y<28;y++)for(x=0;x<24;x++){
   int dx=x-12,dy=y-14,ax=dx<0?-dx:dx,ay=dy<0?-dy:dy;
   int shape=s==0?ax*2+ay:s==1?ax*3+ay:s==2?(ax>5?ax-5:5-ax)*2+ay:s==3?(ax<2?ay:ay<2?ax:ax+ay):s==4?ax*2+ay/2:ax+ay;
   Pixel c=COLOR_KEY;if(shape<13)c=RGB565(240,209,134);if(shape<10)c=RGB565(80+ax*7,190,225);if(shape<5)c=RGB565(235,255,255);
   crystals[s][y*24+x]=c;
  }
  gems[s]=(Sprite){24,28,crystals[s],0};
 }
 for(p=0;p<3;p++)for(l=0;l<9;l++)heroes[p][l]=(Sprite){60,60,(Pixel*)v4_hero_pixels[p][l],0};
 /* LV9 active form is a cached body variant; no per-frame pixel transform. */
 for(p=0;p<3;p++) {
  static const unsigned light[3][3]={{255,190,85},{110,215,255},{255,145,220}};
  for(y=0;y<60;y++)for(x=0;x<60;x++) {
   Pixel c=v4_hero_pixels[p][8][y*60+x];
   if(c!=COLOR_KEY) {
    unsigned r=(c>>11)*255/31,g=((c>>5)&63)*255/63,b=(c&31)*255/31;
    if(r+g+b>160) c=RGB565((r*3+light[p][0])/4,(g*3+light[p][1])/4,(b*3+light[p][2])/4);
    /* Bright eyes/face marks stay inside the face, retaining the dark silhouette. */
    else if(x>=18 && x<42 && y>=22 && y<41)c=RGB565(240,255,255);
   }
   burst_pixels[p][y*60+x]=c;
  }
  burst_heroes[p]=(Sprite){60,60,burst_pixels[p],0};
 }
 ready=1;return 1;
}
const Sprite *campaign_background(int s){return ready==1&&s>=0&&s<6?&bg[s]:0;}
const Sprite *campaign_hero(int p,int l){return ready==1&&p>=0&&p<3&&l>=1&&l<=9&&!(p==0&&l<=7)?&heroes[p][l-1]:0;}
const Sprite *campaign_crystal(int s){return ready==1&&s>=0&&s<6?&gems[s]:0;}
const Sprite *campaign_burst_hero(int p){return ready==1&&p>=0&&p<3?&burst_heroes[p]:0;}
