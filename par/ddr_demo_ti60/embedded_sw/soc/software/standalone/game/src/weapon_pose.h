#ifndef CARROT_WEAPON_POSE_H
#define CARROT_WEAPON_POSE_H
/* 双枪的位置 / 朝向（2026-10-26，照搬队友版 weapon_pose.h，GamePlayer 换成本工程的 Game）。
 * 开火（game.c：子弹从枪口出）和画图（render.c）共用，所以两边永远一致。
 * x,y = 枪的中心（25x25 枪图的中点），mx,my = 枪口，heading = 0..31，front = 枪在身体前面（后画）。 */
#include "weapon_units.h"
typedef struct {int x,y,mx,my,heading,front;} WeaponPose;
static inline int weapon_round(int v){return v>=0?(v+128)/256:-((-v+128)/256);}
static inline int weapon_heading_for(int dx,int dy){
    int best=-2147483647,index=0;
    for(int k=0;k<32;k++){
        int dot=dx*weapon_unit[k][0]+dy*weapon_unit[k][1];
        if(dot>best){best=dot;index=k;}
    }
    return index;
}
/* gun 0 = 左 1 = 右；idle = 收枪（很久没开火）：两把枪放在身体两侧 */
static inline WeaponPose weapon_pose_player(const GamePlayer *g,int gun,int idle){
    WeaponPose p;int side=gun?5:-5,cx=g->x/FIX,cy=g->y/FIX;
    int h=idle?(gun?28:20):(g->weapon_heading&31);
    int ux=weapon_unit[h][0],uy=weapon_unit[h][1],px=-uy,py=ux;
    p.heading=h;
    p.x=idle?cx+(gun?17:-17):cx+weapon_round((ux*8+px*side)*3/2);
    p.y=idle?cy+8:cy+weapon_round((uy*8+py*side)*3/2);
    p.mx=p.x+weapon_round((ux*9-px*2)*3/2);
    p.my=p.y+weapon_round((uy*9-py*2)*3/2);
    p.front=p.y>=cy;
    return p;
}
/* 当前玩家（actor）；多人时画别的玩家用 weapon_pose_player */
static inline WeaponPose weapon_pose(const Game *g,int gun,int idle){return weapon_pose_player(&g->actor,gun,idle);}
#endif
