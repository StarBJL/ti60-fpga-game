#include "music.h"
#include "audio.h"
static int enabled=1,context=MUSIC_MENU,manual=-1;
static const char *const songs[MUSIC_TRACKS]={
 "bgm_title","bgm_menu2","bgm_stage1","bgm_stage2","bgm_boss1","bgm_boss2",
 "bgm_garden","bgm_garden2","bgm_stress","bgm_stress2"
};
/* Automatic scene defaults retain the previous first choice for each context. */
static const unsigned char scene_song[MUSIC_CONTEXTS]={0,2,3,4,6,8,9,1};
int music_enabled(void){return enabled;}
int music_context(void){return context;}
int music_follow_scene(void){return manual<0;}
int music_index(void){return manual<0?scene_song[context]:manual;}
int music_select(int index){if(index< -1||index>=MUSIC_TRACKS)return 0;manual=index;return 1;}
void music_enable(int on){enabled=on!=0;if(!enabled)aud_bgm(-1,0,20);}
void music_next(void){manual=(music_index()+1)%MUSIC_TRACKS;}
void music_previous(void){manual=(music_index()+MUSIC_TRACKS-1)%MUSIC_TRACKS;}
int music_track(int c){
 if(c<0||c>=MUSIC_CONTEXTS)c=MUSIC_MENU;context=c;
 if(!enabled)return -1;
 int s=aud_find(songs[music_index()]);
 if(s<0)s=aud_find(c==MUSIC_MENU?"bgm_title":c==MUSIC_BOSS?"bgm_boss1":"bgm_stage2");
 return s;
}
void music_idle_tick(int scene){
 if(aud.mixer){aud_bgm(music_track(scene==2?MUSIC_SQUARES:MUSIC_IMAGES),150,40);aud_bgm_volume(256);}
 aud_tick();
}
