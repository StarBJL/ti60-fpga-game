#include "v5_art.h"
#include "v5_art_sizes.h"
#include "scene_assets.h"
#ifdef __riscv
#define MEM __attribute__((section(".artbss"),aligned(64)))
#else
#define MEM __attribute__((aligned(64)))
#endif
extern const unsigned v5_sprite_size,v5_sprite_pixels,v5_sprite_bytes,v5_sprite_count,v5_sprite_meta[][3];
extern const unsigned char v5_sprite_packed[];
static unsigned char blob[V5_ART_BYTES] MEM;
static Sprite art[V5_ART_COUNT],seeds[28];
static Pixel seed_pixels[28][25*25] MEM;
static int ready;
int v5_art_init(void){
 if(ready)return ready==1;
 if(v5_sprite_bytes!=sizeof blob || v5_sprite_count!=V5_ART_COUNT || !scene_lz4_decode(v5_sprite_packed,v5_sprite_size,blob,sizeof blob)){ready=-1;return 0;}
 for(unsigned i=0;i<v5_sprite_count;i++)art[i]=(Sprite){(int)v5_sprite_meta[i][0],(int)v5_sprite_meta[i][1],(Pixel*)blob+v5_sprite_meta[i][2],blob+v5_sprite_pixels*2+v5_sprite_meta[i][2]};
 static const int color[7][3]={{115,225,60},{70,235,190},{65,170,255},{255,115,35},{215,120,255},{255,200,90},{240,100,220}};
 for(int family=0;family<7;family++)for(int s=0;s<4;s++){
  int index=family*4+s;
  for(int y=0;y<25;y++)for(int x=0;x<25;x++){
   int dx=x-12,dy=y-12,ax=dx<0?-dx:dx,ay=dy<0?-dy:dy,d=dx*dx+dy*dy,on=0,core=0;
   switch(family){
    case 0:on=ax+ay/2<8 && d<112;core=ax+ay<5;break; /* leaf / thorn */
    case 1:on=ax*2+ay<17;core=ax*2+ay<6;break;      /* jade shard */
    case 2:on=(dx-(y/4%2?3:-3))*(dx-(y/4%2?3:-3))<9 && ay<12;core=ax<2;break;
    case 3:on=d<70 || (dy<0&&ax<5+dy/3&&dy>-12);core=d<16;break;
    case 4:on=(ax<3&&ay<11)||(ay<3&&ax<11)||ax+ay<8;core=ax+ay<4;break;
    case 5:on=d<95 && d>25;core=d<14;break;       /* yin-yang rings */
    default:on=(ax+ay>9&&ax+ay<15)||d<25;core=d<9;break;
   }
   Pixel c=COLOR_KEY;
   if(on||core){int light=core?255:185+s*20;int r=color[family][0],g=color[family][1],b=color[family][2];if(family==5&&dx<0){r=70;g=185;b=255;}c=core?RGB565(255,250,220):RGB565(r*light/255,g*light/255,b*light/255);}
   seed_pixels[index][y*25+x]=c;
  }
  seeds[index]=(Sprite){25,25,seed_pixels[index],0};
 }
 ready=1;return 1;
}
const Sprite *v5_enemy(int family,int kind,int attack,int enraged){
 if(ready!=1||family<0||family>6)return 0;
 if(family==6)return &art[48+(enraged?4:0)+(attack>=0&&attack<4?attack:0)];
 int col=kind==3?3:kind==2?2:kind==4?1:0;
 return &art[family*8+(attack?4:0)+col];
}
const Sprite *v5_hero(int role,int pose){return ready==1&&role>=0&&role<3&&pose>=0&&pose<4?&art[56+role*4+pose]:0;}
const Sprite *v5_seeds(void){return ready==1?seeds:0;}
