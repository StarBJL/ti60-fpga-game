#include "render.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
static Game game;
static Pixel pixels[1280*720+32];
static void image(const char *name){
    Gfx gfx;RenderInfo info={0};FILE *f;int i;
    info.fps=60;info.ms=153;info.work_us=15300;info.alpha_fx=1;info.batch=2;
    for(i=1280*720;i<1280*720+32;i++)pixels[i]=0x5aa5;
    gfx_begin(&gfx,pixels,1280,720);render_game(&game,&gfx,info);gfx_end(&gfx);
    for(i=1280*720;i<1280*720+32;i++)assert(pixels[i]==0x5aa5);
    assert(pixels[89*1280+1259]!=0); /* native right arena, not a 640-wide view */
    f=fopen(name,"wb");assert(f);assert(fwrite(pixels,2,1280*720,f)==1280*720);fclose(f);
}
int main(void){
    Input in={0};int i,j,seen[3]={0};const GfxItem *it;
    render_init();game_init(&game,417);game.phase=TITLE;image("title.rgb565");
    game.phase=STRESS_SELECT;image("stress_menu.rgb565");
    assert(render_stress_hit(500,477)==3);
    in.pressed=KEY_PICK4;game_tick(&game,in);in.pressed=0;
    assert(game.stress_tier==STRESS_CUSTOM&&game_hostile_slots(&game)==40000);
    game_set_stress_bullets(&game,14000);assert(game_hostile_slots(&game)==14000);
    for(i=0;i<300;i++){
        in.held=(i/75)%2?KEY_LEFT:KEY_RIGHT;game.invincible=60;game_tick(&game,in);
        assert(game.x>=42*FIX&&game.x<=1238*FIX&&game.y>=123*FIX&&game.y<=633*FIX);
        assert(game.hostile_active<=14000&&game.hostile_used<=14000);seen[game.seed_draw_buf]=1;
        j=game_seed_draw(&game,&it);assert(j>=0&&j<=14000);
        while(j--)assert(it[j].x>-13&&it[j].x<1280&&it[j].y>-13&&it[j].y<720);
    }
    assert(seen[0]&&seen[1]&&seen[2]);image("stress_play.rgb565");
    game_set_stress_bullets(&game,1000);assert(game.hostile_active<=1000&&game_hostile_slots(&game)==1000);
    game_set_stress_bullets(&game,99999);assert(game_hostile_slots(&game)==40000);
    game_start(&game,417);for(i=0;i<120;i++){game.invincible=60;game_tick(&game,in);}image("normal_play.rgb565");
    puts("PASS native game: Chinese title/4 tiers, custom cap 1K..40K, native movement, 13px seed table and triple buffering; framebuffer guards intact");
    return 0;
}
