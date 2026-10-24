#include "game_sound.h"
#include "audio.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
AudState aud={1,1,28,0,256,0,0};static Game game;
static int song,volume,played[16],ticks;
static const char *names[]={"shot","hit","kill","death","powerup","over","pause","bgm_title","bgm_stage2","bgm_boss1"};
int aud_find(const char *name){int i;for(i=0;i<10;i++)if(!strcmp(name,names[i]))return i;return -1;}
int aud_check_bank(void){return 1;}
void aud_bgm(int snd,int vol,int fade){song=snd;(void)vol;(void)fade;}
void aud_bgm_volume(int vol){volume=vol;}
int aud_play(int snd,int vol,int pan){assert(snd>=0&&snd<10&&vol>0&&vol<=256&&pan==0);++played[snd];return 2;}
void aud_tick(void){++ticks;}
int main(void){int i;
 game.phase=TITLE;game_sound_tick(&game);assert(song==7&&volume==256);
 game.phase=PLAYING;game.sfx=GS_SHOT|GS_HIT|GS_KILL;game_sound_tick(&game);
 assert(song==8&&played[0]==1&&played[1]==1&&played[2]==1&&game.sfx==0);
 game.sfx=GS_SHOT|GS_HIT;game_sound_tick(&game);assert(played[0]==1&&played[1]==1);
 for(i=0;i<5;i++)game_sound_tick(&game);
 game.sfx=GS_SHOT|GS_HIT;game_sound_tick(&game);assert(played[0]==2&&played[1]==2);
 game.phase=PAUSED;game_sound_tick(&game);assert(volume==64&&played[6]==1);
 game.phase=PLAYING;game.enemies[0].active=1;game.enemies[0].kind=3;game_sound_tick(&game);assert(song==9&&volume==256);
 game.phase=DEFEAT;game_sound_tick(&game);assert(song==-1&&played[5]==1);
 puts("PASS carrot audio: title/play/boss BGM, shot/hit/kill event routing, rate limiting, pause attenuation and defeat cue");return 0;
}
