#include "control.h"
#include <string.h>
#include <limits.h>
typedef struct {const char *name;ControlType type;int argument;} Command;
static const Command commands[]={
 {"status",CT_STATUS,0},{"scene",CT_SCENE,1},{"start",CT_START,0},{"pause",CT_PAUSE,0},
 {"restart",CT_RESTART,0},{"menu",CT_MENU,0},{"stress",CT_STRESS,1},{"skill",CT_SKILL,0},
 {"pick",CT_PICK,1},{"move",CT_MOVE,1},{"stop",CT_STOP,0},{"step",CT_STEP,1},
 {"delta",CT_DELTA,1},{"count",CT_COUNT,1},{"bullet",CT_BULLET_SIZE,1},{"square",CT_SQUARE_SIZE,1},
 {"period",CT_PERIOD,1},{"density",CT_DENSITY,1},{"auto",CT_AUTO,1},{"vsync",CT_VSYNC,1},{"alpha",CT_ALPHA,1},
 {"backend",CT_BACKEND,1},{"motion",CT_MOTION,1},{"table",CT_TABLE,1},{"bgm",CT_BGM,1},
 {"next",CT_NEXT_TRACK,0},{"volume",CT_VOLUME,1},{"tone",CT_TONE,1},{"stats",CT_STATS,1},
 {"verbose",CT_VERBOSE,1},{"bench",CT_BENCH,0},{"key",CT_LEGACY,1},{"resolution",CT_RESOLUTION,1}
};
static int integer(const char **s,int *v){
    int neg=0;unsigned n=0;if(**s=='-'){neg=1;(*s)++;}
    if(**s<'0'||**s>'9')return 0;
    while(**s>='0'&&**s<='9'){unsigned digit=(unsigned)(*(*s)++-'0');if(n>214748364u||(n==214748364u&&digit>7u))return 0;n=n*10u+digit;}
    *v=neg?-(int)n:(int)n;return 1;
}
int control_is_step(int v){return v==1||v==10||v==100;}
int control_parse(const char *s,ControlEvent *e){
    int id,value=0;const char *start;unsigned len;e->type=CT_INVALID;e->id=0;e->value=0;e->has_value=0;
    if(!integer(&s,&id)||id<0||*s++!=' ')return 0;e->id=(unsigned)id;
    start=s;while(*s&&*s!=' ')s++;len=(unsigned)(s-start);
    for(unsigned i=0;i<sizeof commands/sizeof commands[0];i++)if(strlen(commands[i].name)==len&&!memcmp(start,commands[i].name,len)){
        if(commands[i].argument){if(*s++!=' '||!integer(&s,&value)||*s)return 0;e->has_value=1;}
        else if(*s)return 0;e->type=commands[i].type;e->value=value;return 1;
    }return 0;
}
int control_feed(ControlParser *p,unsigned char c,ControlEvent *e){
    if(!p->active){if(c!=':')return 0;p->active=1;p->used=0;p->overflow=0;return 1;}
    if(c=='\r')return 1;
    if(c=='\n'){
        p->data[p->used]=0;p->active=0;
        if(p->overflow){e->type=CT_INVALID;e->id=0;e->value=0;e->has_value=0;}
        else control_parse(p->data,e);return 2;
    }
    if(c<32||c>126||p->used>=sizeof p->data-1)p->overflow=1;
    else if(!p->overflow)p->data[p->used++]=(char)c;
    return 1;
}
