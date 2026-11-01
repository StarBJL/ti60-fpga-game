#include "v6_art.h"
#include "v5_art.h"
#include "scene_assets.h"
#ifdef __riscv
#define MEM __attribute__((section(".artbss"),aligned(64)))
#else
#define MEM __attribute__((aligned(64)))
#endif
extern const uint8_t v6_portraits_packed[];extern const unsigned v6_portraits_size;
static uint8_t blob[6*160*160*3] MEM;
static Pixel outline_px[3][4][64*64] MEM;static uint8_t outline_a[3][4][64*64] MEM;
static Pixel ring_px[3][96*96] MEM;static uint8_t ring_a[3][96*96] MEM;
static Sprite portraits[6],outlines[3][4],rings[3];static int ready;
int v6_art_init(void){
 if(ready)return ready==1;if(!v5_art_init()||!scene_lz4_decode(v6_portraits_packed,v6_portraits_size,blob,sizeof blob)){ready=-1;return 0;}
 for(int i=0;i<6;i++)portraits[i]=(Sprite){160,160,(Pixel*)blob+i*25600,blob+6*25600*2+i*25600};
 static const Pixel col[3]={RGB565(255,230,180),RGB565(180,235,255),RGB565(255,200,240)};
 for(int role=0;role<3;role++){
  for(int pose=0;pose<4;pose++){
   const Sprite *s=v5_hero(role,pose);
   for(int y=0;y<64;y++)for(int x=0;x<64;x++){
    int sx=x-2,sy=y-2,inside=sx>=0&&sy>=0&&sx<60&&sy<60&&s->alpha[sy*60+sx]>80,best=99;
    if(!inside)for(int dy=-3;dy<=3;dy++)for(int dx=-3;dx<=3;dx++){int xx=sx+dx,yy=sy+dy;if(xx>=0&&yy>=0&&xx<60&&yy<60&&s->alpha[yy*60+xx]>120&&dx*dx+dy*dy<best)best=dx*dx+dy*dy;}
    /* 2026-10-31：软的外发光（离身体越远越淡），原来是 2 px 实心 205 = 抠图白边 */
    outline_px[role][pose][y*64+x]=col[role];outline_a[role][pose][y*64+x]=inside?0:best<=1?120:best<=2?85:best<=5?40:best<=9?12:0;
   }
   outlines[role][pose]=(Sprite){64,64,outline_px[role][pose],outline_a[role][pose]};
  }
  for(int y=0;y<96;y++)for(int x=0;x<96;x++){
   int dx=x-48,dy=y-48,d=dx*dx+dy*dy;
   int a=(d>1600&&d<2025)?180:(d>1200&&d<2116)?55:0;
   if(role==2&&d<1764)a+=18;
   ring_px[role][y*96+x]=col[role];ring_a[role][y*96+x]=(uint8_t)a;
  }
  rings[role]=(Sprite){96,96,ring_px[role],ring_a[role]};
 }
 ready=1;return 1;
}
const Sprite *v6_portrait(int role,int release){return ready==1&&role>=0&&role<3?&portraits[role+(release?3:0)]:0;}
const Sprite *v6_outline(int role,int pose){return ready==1&&role>=0&&role<3&&pose>=0&&pose<4?&outlines[role][pose]:0;}
const Sprite *v6_ring(int role){return ready==1&&role>=0&&role<3?&rings[role]:0;}
