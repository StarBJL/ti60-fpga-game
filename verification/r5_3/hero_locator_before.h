/* R19 visual-only local occlusion, cached union silhouette, and hysteresis.
 * Included once by render.c after sprites and the shared WeaponPose helpers. */
#define LOC_W CARROT_LOC_W
#define LOC_H CARROT_LOC_H
#define LOC_X CARROT_LOC_X
#define LOC_Y CARROT_LOC_Y
static uint64_t loc_body_rows[7][LOC_H],loc_actor[LOC_H];
static uint32_t loc_seed_rows[2][4][25],loc_laser_rows[8][CARROT_LASER_SIZE];
/* A source stays immutable while hardware consumes the previous frame.
 * R2 may record a new frame while two older frames still reference sources.
 * Three rotating buffers match the R2 seed-table lifetime. */
static Pixel loc_outline_pixels[3][LOC_W*LOC_H] __attribute__((aligned(64)));
static Sprite loc_outline[3];
static int loc_buffer,loc_level,loc_heading,loc_idle,loc_dirty,loc_shape_dirty,loc_actor_total;
static RenderLocatorInfo loc_info;
static const Game *loc_game;
static int loc_ticks,loc_visual,loc_x,loc_y,loc_previous_cover,loc_previous_total,loc_phase;
static unsigned loc_bits(uint64_t v){
    return (unsigned)__builtin_popcount((uint32_t)v)+(unsigned)__builtin_popcount((uint32_t)(v>>32));
}
void render_locator_reset(void){
    memset(&loc_info,0,sizeof loc_info);loc_game=0;loc_ticks=-1;
}
RenderLocatorInfo render_locator_info(void){return loc_info;}
static void locator_build_outline(void);
const Sprite *render_locator_outline_asset(void){locator_build_outline();return &loc_outline[loc_buffer];}
const Sprite *render_projectile_asset(int pressure,int style){
    if((unsigned)style>=4u)style=0;return pressure?&hostile_sprites[style]:&normal_seeds[style];
}
const Sprite *render_laser_asset(int style){return &laser_sprites[(unsigned)style<8u?style:0];}
static void init_player_locator(void){
    int l,x,y,s,m;
    memset(loc_body_rows,0,sizeof loc_body_rows);
    memset(loc_seed_rows,0,sizeof loc_seed_rows);memset(loc_laser_rows,0,sizeof loc_laser_rows);
    for(l=0;l<7;l++)for(y=0;y<CARROT_HERO_H;y++)for(x=0;x<CARROT_HERO_W;x++)
        if(hero_sprites[l].pixels[y*CARROT_HERO_W+x]!=COLOR_KEY)
            loc_body_rows[l][y+LOC_Y-CARROT_HERO_ANCHOR_Y]|=(uint64_t)1<<(x+LOC_X-CARROT_HERO_W/2);
    for(m=0;m<2;m++)for(s=0;s<4;s++){
        const Sprite *a=m?&normal_seeds[s]:&hostile_sprites[s];
        for(y=0;y<a->height;y++)for(x=0;x<a->width;x++)
            if(a->pixels[y*a->width+x]!=COLOR_KEY)loc_seed_rows[m][s][y]|=1u<<x;
    }
    for(s=0;s<8;s++)for(y=0;y<CARROT_LASER_SIZE;y++)for(x=0;x<CARROT_LASER_SIZE;x++)
        if(laser_sprites[s].pixels[y*CARROT_LASER_SIZE+x]!=COLOR_KEY)loc_laser_rows[s][y]|=1u<<x;
    for(s=0;s<3;s++)loc_outline[s]=(Sprite){LOC_W,LOC_H,loc_outline_pixels[s],0};
    loc_level=0;render_locator_reset();
}
static void locator_pose(const Game *g){
    int level=game_appearance_level(g),idle=g->weapon_idle_ticks>30,h=g->weapon_heading&31;
    if(level==loc_level&&idle==loc_idle&&(idle||h==loc_heading))return;
    loc_level=level;loc_idle=idle;loc_heading=h;
    memcpy(loc_actor,loc_body_rows[level-1],sizeof loc_actor);
    for(int k=0;k<2;k++){
        WeaponPose p=weapon_pose(g,k,idle);
        const Sprite *a=render_gun_asset(level,p.heading);
        int dx=p.x-g->x/FIX-CARROT_GUN_HALF+LOC_X,dy=p.y-g->y/FIX-CARROT_GUN_HALF+LOC_Y;
        for(int y=0;y<CARROT_GUN_SIZE;y++)for(int x=0;x<CARROT_GUN_SIZE;x++)if(a->pixels[y*CARROT_GUN_SIZE+x]!=COLOR_KEY){
            int xx=x+dx,yy=y+dy;
            if((unsigned)xx<LOC_W&&(unsigned)yy<LOC_H)loc_actor[yy]|=(uint64_t)1<<xx;
        }
    }
    loc_actor_total=0;
    for(int y=0;y<LOC_H;y++)loc_actor_total+=(int)loc_bits(loc_actor[y]);
    loc_shape_dirty=1;
}
static void locator_build_outline(void){
    uint64_t edge1[LOC_H],edge2[LOC_H];
    const uint64_t limit=UINT64_MAX;
    if(!loc_shape_dirty)return;
    loc_shape_dirty=0;
    for(int y=0;y<LOC_H;y++)edge1[y]=((loc_actor[y]<<1)|(loc_actor[y]>>1)|
        (y?loc_actor[y-1]:0)|(y+1<LOC_H?loc_actor[y+1]:0))&~loc_actor[y]&limit;
    for(int y=0;y<LOC_H;y++)edge2[y]=((edge1[y]<<1)|(edge1[y]>>1)|
        (y?edge1[y-1]:0)|(y+1<LOC_H?edge1[y+1]:0))&~loc_actor[y]&~edge1[y]&limit;
    loc_buffer=(loc_buffer+1)%3;
    loc_dirty=1;
    for(int y=0;y<LOC_H;y++)for(int x=0;x<LOC_W;x++){
        uint64_t bit=(uint64_t)1<<x;
        loc_outline_pixels[loc_buffer][y*LOC_W+x]=edge1[y]&bit?RGB565(170,178,190):
            edge2[y]&bit?RGB565(10,12,18):COLOR_KEY;
    }
}
static int locator_cover(uint64_t *left,const uint32_t *rows,int size,int x,int y){
    int n=0;
    if(x<=-size||y<=-size||x>=LOC_W||y>=LOC_H)return 0;
    int start=y<0?-y:0,end=y+size>LOC_H?LOC_H-y:size;
    for(int sy=start;sy<end;sy++)if(rows[sy]&&left[y+sy]){
        uint64_t row=x<0?(uint64_t)rows[sy]>>-x:(uint64_t)rows[sy]<<x;
        uint64_t hit=left[y+sy]&row;
        if(hit){n+=(int)loc_bits(hit);left[y+sy]&=~hit;}
    }
    return n;
}
static void prepare_player_locator(const Game *g){
    uint64_t left[LOC_H];
    int bob=g->moving?(g->ticks/7)%2:0,cx=g->x/FIX,cy=g->y/FIX;
    int ox=cx-LOC_X,oy=cy-LOC_Y-bob,total=0,covered=0,count=0;
    locator_pose(g);memcpy(left,loc_actor,sizeof left);total=loc_actor_total;
    for(int i=0;i<MAX_BULLETS&&covered<total;i++)if(g->bullets[i].active){
        const Bullet *b=&g->bullets[i];int style=(unsigned)b->style<8u?b->style:0;
        covered+=locator_cover(left,loc_laser_rows[style],CARROT_LASER_SIZE,b->x/FIX-CARROT_LASER_SIZE/2-ox,b->y/FIX-CARROT_LASER_SIZE/2-oy);
    }
    const GfxItem *items;int n=game_seed_draw(g,&items),half=game_seed_half(g),size=half*2+1;
    int cached=n>=0&&g->occlusion_x==cx&&g->occlusion_y==cy&&g->occlusion_half==half&&!g->occlusion_near_overflow;
    const uint32_t (*rows)[25]=loc_seed_rows[g->limit_mode?0:1];
    if(n>=0){
        int end=cached?g->occlusion_near_n:n;
        for(int i=0;i<end&&covered<total;i++){
            const GfxItem *b=&items[cached?g->occlusion_near[i]:i];
            if(b->x<=ox-size||b->x>=ox+LOC_W||b->y<=oy-size||b->y>=oy+LOC_H)continue;
            int style=(unsigned)b->sprite<4u?b->sprite:0;++count;
            covered+=locator_cover(left,rows[style],size,b->x-ox,b->y-oy);
        }
    }else{
        int end=game_hostile_used(g);const Bullet *pool=end?game_hostile_at(g,0):0;
        for(int i=0;i<end&&covered<total;i++)if(pool[i].active){
            const Bullet *b=&pool[i];int x=b->x/FIX-half,y=b->y/FIX-half;
            if(x<=ox-size||x>=ox+LOC_W||y<=oy-size||y>=oy+LOC_H)continue;
            int style=(unsigned)b->style<4u?b->style:0;++count;
            covered+=locator_cover(left,rows[style],size,x-ox,y-oy);
        }
    }
    int changed=loc_game!=g||loc_ticks!=g->ticks||loc_visual!=g->visual_ticks||loc_x!=cx||loc_y!=cy||
        loc_previous_cover!=covered||loc_previous_total!=total||loc_phase!=(int)g->phase;
    if(loc_game!=g||g->ticks<loc_ticks||g->phase==TITLE||g->phase==STRESS_SELECT){
        loc_info.active=loc_info.fade=0;
    }
    if(changed){
        if(covered*100>=total*80)loc_info.active=1;
        else if(covered*100<=total*65)loc_info.active=0;
        if(!covered||g->phase==TITLE||g->phase==STRESS_SELECT)loc_info.fade=loc_info.active=0;
        else if(loc_info.active){loc_info.fade+=64;if(loc_info.fade>255)loc_info.fade=255;}
        else {loc_info.fade-=64;if(loc_info.fade<0)loc_info.fade=0;}
    }
    loc_info.covered=covered;loc_info.total=total;loc_info.percent=total?covered*100/total:0;
    loc_info.colored=loc_info.fade<255;loc_info.near_items=count;loc_info.used_near_cache=cached;
    loc_game=g;loc_ticks=g->ticks;loc_visual=g->visual_ticks;loc_x=cx;loc_y=cy;
    loc_previous_cover=covered;loc_previous_total=total;loc_phase=(int)g->phase;
}
