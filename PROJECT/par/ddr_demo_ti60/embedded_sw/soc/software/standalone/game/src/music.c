#include "music.h"
#include "audio.h"
static int enabled=1,context;
static unsigned variants[MUSIC_CONTEXTS];
static const char *const songs[MUSIC_CONTEXTS][2]={
 {"bgm_title","bgm_menu2"},{"bgm_stage1","bgm_stage2"},{"bgm_stage2","bgm_stage1"},{"bgm_boss1","bgm_boss2"},
 {"bgm_garden","bgm_garden2"},{"bgm_stress","bgm_stress2"},{"bgm_stress2","bgm_stress"},
 {"bgm_menu2","bgm_garden2"}
};
int music_enabled(void){return enabled;}
int music_context(void){return context;}
void music_enable(int on){enabled=on!=0;if(!enabled)aud_bgm(-1,0,20);}
void music_next(void){variants[context]^=1u;}
int music_track(int c){
 if(c<0||c>=MUSIC_CONTEXTS)c=MUSIC_MENU;context=c;
 if(!enabled)return -1;
 int s=aud_find(songs[c][variants[c]&1u]);
 if(s<0)s=aud_find(c==MUSIC_MENU?"bgm_title":c==MUSIC_BOSS?"bgm_boss1":"bgm_stage2");
 return s;
}
void music_idle_tick(int scene){
 if(aud.mixer){aud_bgm(music_track(scene==2?MUSIC_SQUARES:MUSIC_IMAGES),150,40);aud_bgm_volume(256);}
 aud_tick();
}
