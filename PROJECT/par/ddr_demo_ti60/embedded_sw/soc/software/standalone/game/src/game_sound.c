#include "game_sound.h"
#include "audio.h"
#include "music.h"
void game_sound_tick(Game *g)
{
    static unsigned tick,next[6];static int last_phase=-1;
    static const char *names[6]={"shot","hit","kill","death","powerup","powerup"};
    static const int volume[6]={65,100,170,200,210,190},gap[6]={4,4,2,20,10,10};
    unsigned ev=g->sfx;int i,boss=0,bgm;g->sfx=0;++tick;
    if(aud.mixer&&!aud.nsnd&&tick%60u==0)aud_check_bank();
    if(g->phase==PLAYING||g->phase==PAUSED||g->phase==UPGRADE)for(i=0;i<MAX_ENEMIES;i++)if(g->enemies[i].active&&g->enemies[i].kind==3){boss=1;break;}
    bgm=music_track(g->phase==TITLE||g->phase==STRESS_SELECT?MUSIC_MENU:boss?MUSIC_BOSS:g->limit_mode?MUSIC_STRESS:MUSIC_GARDEN);
    if(g->phase==DEFEAT)bgm=-1;
    aud_bgm(bgm,190,40);aud_bgm_volume(g->phase==PAUSED?64:256);
    for(i=0;i<6;i++)if((ev&(1u<<i))&&(int32_t)(tick-next[i])>=0){aud_play(aud_find(names[i]),volume[i],0);next[i]=tick+gap[i];}
    if(last_phase!=(int)g->phase){
        if(g->phase==DEFEAT)aud_play(aud_find("over"),220,0);
        else if(g->phase==PAUSED)aud_play(aud_find("pause"),180,0);
        else if(g->phase==UPGRADE)aud_play(aud_find("powerup"),210,0);
        last_phase=g->phase;
    }
    aud_tick();
}
