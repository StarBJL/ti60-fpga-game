#ifndef TI60_MUSIC_H
#define TI60_MUSIC_H
enum { MUSIC_MENU,MUSIC_TH_STAGE,MUSIC_TH_STAGE2,MUSIC_BOSS,MUSIC_GARDEN,MUSIC_STRESS,MUSIC_SQUARES,MUSIC_IMAGES,MUSIC_CONTEXTS };
#define MUSIC_TRACKS 10
int music_track(int context);
int music_follow_scene(void);
int music_index(void);
int music_select(int index);
void music_previous(void);
int music_enabled(void);
void music_enable(int on);
void music_next(void);
int music_context(void);
void music_idle_tick(int scene);
#endif
