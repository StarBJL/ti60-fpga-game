/* R19 visual-only local occlusion, cached union silhouette, and hysteresis.
 * Included once by render.c after sprites and the shared WeaponPose helpers.
 * 2026-10-26 移植自队友版；多人时每个玩家一份淡入淡出历史（locator_load / locator_save），
 * 不用游戏逻辑里的近邻缓存（occlusion_near），每帧扫本帧的弹幕表（普通模式最多 1024 颗）。
 * 作用：弹幕把角色盖住 80% 以上时淡入角色轮廓 + 中心点，低于 65% 淡出。 */
#define LOC_W CARROT_LOC_W
#define LOC_H CARROT_LOC_H
#define LOC_X CARROT_LOC_X
#define LOC_Y CARROT_LOC_Y
static uint64_t loc_body_rows[7][LOC_H],loc_actor[LOC_H];
static uint32_t loc_seed_rows[4][25],loc_laser_rows[8][CARROT_LASER_SIZE];
/* Immutable lazy cache for all seven appearances and 32 aim headings plus
 * idle. DDR/BSS storage, not FPGA block RAM. Once published, an outline is
 * never overwritten during normal play; frames in flight may retain it.
 * This avoids rebuilding ~8.5 KiB and invalidating the source cache whenever
 * the weapon returns to a previously seen pose. */
#define LOC_POSES (7*33)
/* 大缓冲放到 DDR 0x0400_0000 的 .artbss（game.ld，不下载、启动不清零，用之前一定先写）：
 * 程序区（.bss）只有 15 MB，2026-10-26 加了萝卜新美术后放不下了 */
#ifdef __riscv
#define DDR_BIG(al) __attribute__((section(".artbss"),aligned(al)))
#else
#define DDR_BIG(al) __attribute__((aligned(al)))
#endif
static Pixel loc_outline_pixels[LOC_POSES][LOC_W*LOC_H] DDR_BIG(64);   /* 2 MB；用到某个姿势时才画（loc_outline_valid） */
static Sprite loc_outline[LOC_POSES];
static uint64_t loc_pose_rows[LOC_POSES][LOC_H];
static uint16_t loc_pose_total[LOC_POSES];
static uint8_t loc_pose_valid[LOC_POSES],loc_outline_valid[LOC_POSES];
static int loc_buffer,loc_level,loc_heading,loc_idle,loc_dirty,loc_shape_dirty,loc_actor_total;
typedef struct { int covered,total,percent,active,fade; } RenderLocatorInfo;
static RenderLocatorInfo loc_info;
static const Game *loc_game;
static int loc_ticks,loc_visual,loc_x,loc_y,loc_previous_cover,loc_previous_total,loc_phase;
typedef struct { RenderLocatorInfo info; const Game *game; int ticks,visual,x,y,cover,total,phase; } LocatorHistory;
static LocatorHistory locator_history[GAME_PLAYERS];
static void locator_save(int id){
    locator_history[id]=(LocatorHistory){loc_info,loc_game,loc_ticks,loc_visual,loc_x,loc_y,loc_previous_cover,loc_previous_total,loc_phase};
}
static void locator_load(int id){
    const LocatorHistory *h=&locator_history[id];
    loc_info=h->info;loc_game=h->game;loc_ticks=h->ticks;loc_visual=h->visual;loc_x=h->x;loc_y=h->y;
    loc_previous_cover=h->cover;loc_previous_total=h->total;loc_phase=h->phase;
}
static unsigned loc_bits(uint64_t v){
    return (unsigned)__builtin_popcount((uint32_t)v)+(unsigned)__builtin_popcount((uint32_t)(v>>32));
}
static void render_locator_reset(void){
    memset(&loc_info,0,sizeof loc_info);loc_game=0;loc_ticks=-1;
    memset(locator_history,0,sizeof locator_history);
    for(int i=0;i<GAME_PLAYERS;i++)locator_history[i].ticks=-1;
}
static void locator_build_outline(void);
static const Sprite *render_locator_outline_asset(void){locator_build_outline();return &loc_outline[loc_buffer];}
static void init_player_locator(void){
    int l,x,y,s,m;
    memset(loc_body_rows,0,sizeof loc_body_rows);
    memset(loc_seed_rows,0,sizeof loc_seed_rows);memset(loc_laser_rows,0,sizeof loc_laser_rows);
    for(l=0;l<7;l++)for(y=0;y<CARROT_HERO_H;y++)for(x=0;x<CARROT_HERO_W;x++)
        if(hero_sprites[l].pixels[y*CARROT_HERO_W+x]!=COLOR_KEY)
            loc_body_rows[l][y+LOC_Y-CARROT_HERO_ANCHOR_Y]|=(uint64_t)1<<(x+LOC_X-CARROT_HERO_W/2);
    (void)m;
    for(s=0;s<4;s++){                       /* 普通模式的 25x25 敌弹（压力模式不显示轮廓） */
        const Sprite *a=&normal_seeds[s];
        for(y=0;y<a->height&&y<25;y++)for(x=0;x<a->width&&x<25;x++)
            if(a->pixels[y*a->width+x]!=COLOR_KEY)loc_seed_rows[s][y]|=1u<<x;
    }
    for(s=0;s<8;s++)for(y=0;y<CARROT_LASER_SIZE;y++)for(x=0;x<CARROT_LASER_SIZE;x++)
        if(laser_sprites[s].pixels[y*CARROT_LASER_SIZE+x]!=COLOR_KEY)loc_laser_rows[s][y]|=1u<<x;
    memset(loc_pose_valid,0,sizeof loc_pose_valid);
    memset(loc_outline_valid,0,sizeof loc_outline_valid);
    for(s=0;s<LOC_POSES;s++)loc_outline[s]=(Sprite){LOC_W,LOC_H,loc_outline_pixels[s],0};
    loc_level=0;render_locator_reset();
}
static void locator_pose_state(const Game *g,const GamePlayer *actor){
    int level=game_player_level(g,actor)>7?7:game_player_level(g,actor),idle=actor->weapon_idle_ticks>30,h=actor->weapon_heading&31;
    if(level==loc_level&&idle==loc_idle&&(idle||h==loc_heading))return;
    loc_level=level;loc_idle=idle;loc_heading=h;
    loc_buffer=(level-1)*33+(idle?32:h);
    if(loc_pose_valid[loc_buffer]){
        memcpy(loc_actor,loc_pose_rows[loc_buffer],sizeof loc_actor);
        loc_actor_total=loc_pose_total[loc_buffer];
        loc_shape_dirty=!loc_outline_valid[loc_buffer];
        return;
    }
    memcpy(loc_actor,loc_body_rows[level-1],sizeof loc_actor);
    for(int k=0;k<2;k++){
        WeaponPose p=weapon_pose_player(actor,k,idle);
        const Sprite *a=render_gun_asset(level,p.heading);
        int dx=p.x-actor->x/FIX-CARROT_GUN_HALF+LOC_X,dy=p.y-actor->y/FIX-CARROT_GUN_HALF+LOC_Y;
        for(int y=0;y<CARROT_GUN_SIZE;y++)for(int x=0;x<CARROT_GUN_SIZE;x++)if(a->pixels[y*CARROT_GUN_SIZE+x]!=COLOR_KEY){
            int xx=x+dx,yy=y+dy;
            if((unsigned)xx<LOC_W&&(unsigned)yy<LOC_H)loc_actor[yy]|=(uint64_t)1<<xx;
        }
    }
    loc_actor_total=0;
    for(int y=0;y<LOC_H;y++)loc_actor_total+=(int)loc_bits(loc_actor[y]);
    memcpy(loc_pose_rows[loc_buffer],loc_actor,sizeof loc_actor);
    loc_pose_total[loc_buffer]=(uint16_t)loc_actor_total;
    loc_pose_valid[loc_buffer]=1;
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
    loc_outline_valid[loc_buffer]=1;
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
        if(hit){left[y+sy]&=~hit;if(!left[y+sy])++n;}
    }
    return n;
}
/* id = 玩家 0..2：先取回他的历史，算完再存回去 */
static void prepare_player_locator(const Game *g,const GamePlayer *actor,int id){
    if(g->limit_mode){render_locator_reset();return;}
    locator_load(id);
    uint64_t left[LOC_H];
    int bob=actor->moving?(g->ticks/7)%2:0,cx=actor->x/FIX,cy=actor->y/FIX;
    int ox=cx-LOC_X,oy=cy-LOC_Y-bob,total=0,covered=0,count=0;
    locator_pose_state(g,actor);memcpy(left,loc_actor,sizeof left);total=loc_actor_total;
    int live_rows=0;for(int y=0;y<LOC_H;y++)live_rows+=left[y]!=0;
    for(int i=0;i<MAX_BULLETS&&live_rows>0;i++)if(g->bullets[i].active){
        const Bullet *b=&g->bullets[i];int style=(unsigned)b->style<8u?b->style:0;
        live_rows-=locator_cover(left,loc_laser_rows[style],CARROT_LASER_SIZE,b->x/FIX-CARROT_LASER_SIZE/2-ox,b->y/FIX-CARROT_LASER_SIZE/2-oy);
    }
    const GfxItem *items;int n=game_seed_draw(g,&items),half=game_seed_half(g),size=game_seed_extent(g);
    const uint32_t (*rows)[25]=loc_seed_rows;
    if(n>=0){
        int end=n;
        for(int i=0;i<end&&live_rows>0;i++){
            const GfxItem *b=&items[i];
            if(b->x<=ox-size||b->x>=ox+LOC_W||b->y<=oy-size||b->y>=oy+LOC_H)continue;
            int style=(unsigned)b->sprite<4u?b->sprite:0;++count;
            live_rows-=locator_cover(left,rows[style],size,b->x-ox,b->y-oy);
        }
    }else{
        int end=game_hostile_used(g);const Seed *pool=end?game_hostile_at(g,0):0;
        for(int i=0;i<end&&live_rows>0;i++)if(pool[i].active){
            const Seed *b=&pool[i];int x=b->x/FIX-half,y=b->y/FIX-half;
            if(x<=ox-size||x>=ox+LOC_W||y<=oy-size||y>=oy+LOC_H)continue;
            int style=(unsigned)b->style<4u?b->style:0;++count;
            live_rows-=locator_cover(left,rows[style],size,x-ox,y-oy);
        }
    }
    covered=total;for(int y=0;y<LOC_H;y++)covered-=(int)loc_bits(left[y]);
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
    (void)count;
    loc_game=g;loc_ticks=g->ticks;loc_visual=g->visual_ticks;loc_x=cx;loc_y=cy;
    loc_previous_cover=covered;loc_previous_total=total;loc_phase=(int)g->phase;
    locator_save(id);
}

