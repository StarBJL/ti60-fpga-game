/* Bounded decoder for the official LZ4 block format. Only runs during startup.
 * RGB565 values are preserved exactly; no palette reduction or frame-time decode. */
#include "scene_assets.h"
#include "campaign_art.h"
#include <string.h>
extern const uint8_t gallery_dragons_lz4[],carrot_arena_lz4[];
extern const unsigned gallery_dragons_lz4_size,carrot_arena_lz4_size;
int scene_lz4_decode(const uint8_t *src,size_t bytes,uint8_t *dst,size_t cap) {
    size_t i=0,o=0;
    if(!src||!dst)return 0;
    while(i<bytes) {
        unsigned token=src[i++];size_t n=token>>4;
        if(n==15) {unsigned a;do{if(i>=bytes)return 0;a=src[i++];if(n>cap||a>cap-n)return 0;n+=a;}while(a==255);}
        if(n>bytes-i||n>cap-o)return 0;
        memcpy(dst+o,src+i,n);i+=n;o+=n;
        if(i==bytes)return o==cap;
        if(bytes-i<2)return 0;
        size_t off=(size_t)src[i]|((size_t)src[i+1]<<8);i+=2;
        if(!off||off>o)return 0;
        n=(token&15u)+4u;
        if((token&15u)==15) {unsigned a;do{if(i>=bytes)return 0;a=src[i++];if(n>cap||a>cap-n)return 0;n+=a;}while(a==255);}
        if(n>cap-o)return 0;
        /* Forward copying is required for overlapping LZ4 matches. */
        while(n--) {dst[o]=dst[o-off];++o;}
    }
    return 0;
}
int scene_assets_init(void) {return campaign_art_init();}
const Sprite *carrot_story_background(void) {return campaign_background(0);}
int scene_gallery_decode(Pixel *dst,int w,int h) {
    if(w!=SCENE_ASSET_W||h!=SCENE_ASSET_H)return 0;
    return scene_lz4_decode(gallery_dragons_lz4,gallery_dragons_lz4_size,(uint8_t *)dst,(size_t)w*h*sizeof(Pixel));
}
