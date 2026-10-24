#include "music.h"
#include "audio.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
AudState aud={.hdmi=1,.mixer=1,.nsnd=33,.master=256};
static const char *const names[]={"bgm_title","bgm_menu2","bgm_stage1","bgm_stage2","bgm_boss1","bgm_boss2","bgm_garden","bgm_garden2","bgm_stress","bgm_stress2"};
static int stopped;
int aud_find(const char*n){for(int i=0;i<10;i++)if(!strcmp(n,names[i]))return 100+i;return -1;}
void aud_bgm(int s,int v,int f){if(s<0)stopped++;(void)v;(void)f;}
void aud_bgm_volume(int v){(void)v;}
void aud_tick(void){}
int main(void){
 assert(music_follow_scene());assert(music_track(MUSIC_STRESS)==108);
 unsigned seen=0;
 for(int i=0;i<10;i++){music_next();int song=music_track(MUSIC_STRESS)-100;assert(song>=0&&song<10);assert(!(seen&(1u<<song)));seen|=1u<<song;}
 assert(seen==1023&&music_index()==8&&!music_follow_scene());
 for(int i=0;i<10;i++)music_previous();assert(music_index()==8);
 for(int i=0;i<10;i++){assert(music_select(i));for(int c=0;c<MUSIC_CONTEXTS;c++)assert(music_track(c)==100+i);}
 int old=music_index();assert(!music_select(-2)&&!music_select(10));assert(music_index()==old);
 music_enable(0);assert(stopped&&music_track(MUSIC_MENU)==-1);music_next();music_enable(1);assert(music_track(MUSIC_GARDEN)==100);
 assert(music_select(-1)&&music_follow_scene());
 const int automatic[]={0,2,3,4,6,8,9,1};
 for(int c=0;c<MUSIC_CONTEXTS;c++)assert(music_track(c)==100+automatic[c]);
 assert(music_track(-1)==100);
 puts("PASS all ten tracks reachable exactly once per cycle; previous, direct selection, context persistence, auto defaults, mute/resume and invalid selection");
}
