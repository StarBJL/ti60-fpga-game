/* Count original game logic and render calls. No board timing is simulated. */
#include <stdio.h>
#include <string.h>
#include "game.h"
#include "gfx.h"
#include "render.h"

unsigned long long probe_slot_checks, probe_alloc_calls;
static Game game;
typedef struct { unsigned fill, copy, alpha, tiny_fill; } Counts;

static void fill(void *ctx,int x,int y,int w,int h,Pixel c) {
    Counts *p=ctx; (void)x; (void)y; (void)c;
    ++p->fill; if (w*h<=4) ++p->tiny_fill;
}
static void copy(void *ctx,int x,int y,const Sprite *s,int sx,int sy,int w,int h,int key) {
    Counts *p=ctx; (void)x;(void)y;(void)s;(void)sx;(void)sy;(void)w;(void)h;(void)key;
    ++p->copy;
}
static void alpha_fill(void *ctx,int x,int y,int w,int h,Pixel c,int a) {
    Counts *p=ctx; (void)x;(void)y;(void)w;(void)h;(void)c;(void)a; ++p->alpha;
}
static void alpha_copy(void *ctx,int x,int y,const Sprite *s,int sx,int sy,int w,int h,int key,int a) {
    Counts *p=ctx; (void)x;(void)y;(void)s;(void)sx;(void)sy;(void)w;(void)h;(void)key;(void)a;
    ++p->alpha;
}
static const GfxOps ops={0,fill,copy,0,alpha_fill,alpha_copy};

static void setup(int occupied,int fire) {
    int i;
    game_init(&game,0xC0FFEEu);
    game_start_stress(&game,0xC0FFEEu,STRESS_HIGH);
    game.banner=0; game.invincible=0; game.fire_timer=10000;
    for(i=0;i<MAX_ENEMIES;i++) if(game.enemies[i].active) {
        game.enemies[i].spawn=0;
        game.enemies[i].attack_timer=fire?1:25;
    }
    for(i=0;i<occupied;i++) {
        Bullet *b=&game.hostile[i];
        b->active=1; b->x=(40+i%200)*FIX; b->y=(100+i%100)*FIX;
        b->vx=b->vy=0; b->life=1000; b->damage=1; b->style=i%4;
    }
    probe_slot_checks=probe_alloc_calls=0;
}
static void allocation(int occupied,int fire) {
    Input in={0,0};
    setup(occupied,fire);
    game_tick(&game,in);
    printf("ALLOC occupied=%d fire=%d attempts=%llu checked=%llu dropped=%d after=%d\n",
        occupied,fire,probe_alloc_calls,probe_slot_checks,game.dropped_shots,game_hostile_count(&game));
}
static void rendering(int occupied,int stats,int fx) {
    Gfx g; Counts c={0}; RenderInfo info={0};
    setup(occupied,0);
    info.hw=info.hw_avail=1; info.show_stats=stats; info.alpha_fx=fx;
    info.fps=32; info.cpu=99; info.ms=310; info.logic_us=5061;
    info.render_us=25904; info.work_us=30965; info.submit_us=25902; info.wait_us=2; info.blt=43;
    info.fps_mode[0]=31; info.draw_mode[0]=29651; info.fps_mode[1]=32; info.draw_mode[1]=25904;
    info.mouse_x=info.mouse_y=-1;
    gfx_begin_backend(&g,&ops,&c,SCREEN_W,SCREEN_H);
    render_game(&game,&g,info); gfx_end(&g);
    printf("RENDER shots=%d stats=%d fx=%d cmds=%u fill=%u blit=%u alpha=%u tiny_fill=%u pixels=%u\n",
        occupied,stats,fx,c.fill+c.copy+c.alpha,c.fill,c.copy,c.alpha,c.tiny_fill,g.pixels_written);
}
int main(void) {
    printf("Host counts only; not FPGA timing. Bullet bytes=%u pool bytes=%u\n",
        (unsigned)sizeof(Bullet),(unsigned)sizeof(game.hostile));
    allocation(0,1); allocation(3900,1); allocation(4096,1); allocation(4096,0);
    render_init();
    rendering(0,1,1); rendering(4000,1,1); rendering(4000,0,1); rendering(4000,0,0);
    return 0;
}
