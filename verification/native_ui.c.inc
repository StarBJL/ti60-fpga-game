/* Native 1280x720 UI. World sprites remain proportionate; no framebuffer stretch. */
static void hud(Gfx *r,const Game *g,RenderInfo info) {
    char t[160];int sec=g->ticks/60,budget=(int)((int64_t)info.work_us*100/16667);
    gfx_fill_rect(r,0,0,1280,87,INK);
    gfx_text(r,24,12,"萝卜求生",3,CREAM);
    if(g->limit_mode)snprintf(t,sizeof t,"%s 敌%d 弹%d/%d",g->stress_tier==STRESS_CUSTOM?"自定义":g->stress_tier==1?"低档":g->stress_tier==2?"中档":"高档",g->enemy_count,game_hostile_count(g),game_hostile_slots(g));
    else snprintf(t,sizeof t,"无尽模式 怪物 %d",g->enemy_count);
    gfx_text(r,24,53,t,1,g->limit_mode?GOLD:MUTED);
    gfx_text(r,360,12,"生命",2,MUTED);snprintf(t,sizeof t,"%d/%d",g->hp,g->max_hp);gfx_text(r,515,12,t,2,CREAM);
    gfx_fill_rect(r,360,37,270,15,EDGE);gfx_fill_rect(r,362,39,266*g->hp/g->max_hp,11,g->hp<30?RED:GREEN);
    snprintf(t,sizeof t,"经验 %d/%d",g->xp,g->next_xp);gfx_text(r,360,62,t,1,CYAN);
    gfx_text(r,710,12,g->limit_mode?"耗时 毫秒":"存活时间",2,MUTED);
    if(g->limit_mode)snprintf(t,sizeof t,"%d.%02d",info.work_us/1000,(info.work_us%1000)/10);
    else snprintf(t,sizeof t,"%02d:%02d",sec/60,sec%60);
    gfx_text(r,710,41,t,3,CREAM);
    gfx_text(r,930,12,g->limit_mode?"预算":"击败",2,MUTED);
    if(g->limit_mode)snprintf(t,sizeof t,"%d%%",budget);else snprintf(t,sizeof t,"%03d",g->kills);
    gfx_text(r,930,41,t,3,budget>=100&&g->limit_mode?RED:GOLD);
    snprintf(t,sizeof t,"等级 %02d",g->level);gfx_text(r,1120,14,t,2,CYAN);
    snprintf(t,sizeof t,"%s %d秒",sec%60<30?"敌潮":"首领",30-sec%30);gfx_text(r,1120,51,t,1,MUTED);
    gfx_fill_rect(r,22,80,1236,3,EDGE);gfx_fill_rect(r,22,80,1236*g->xp/g->next_xp,3,CYAN);
    gfx_fill_rect(r,0,672,1280,48,INK);
    gfx_text(r,24,689,"WASD 移动  P 暂停  M 菜单",2,MUTED);
    gfx_text(r,550,691,info.hw?"硬件绘制":"处理器绘制",1,info.hw?GREEN:GOLD);
    snprintf(t,sizeof t,"%d.%d 毫秒  %d 帧/秒",info.ms/10,info.ms%10,info.fps);
    gfx_text(r,1250-gfx_text_width(t,2),687,t,2,info.fps>=50?GREEN:GOLD);
    if(g->limit_mode&&g->stress_tier==STRESS_CUSTOM)gfx_text(r,24,95,"自定义档：- / + 每次调整1000弹；1至4选择档位",1,GOLD);
}
#if SPROUT_STATS
static void stats_overlay(Gfx *r,const Game *g,RenderInfo info) {
    char t[160];int y=480;panel(r,24,y,650,177,EDGE);
    gfx_text(r,40,y+12,"每帧耗时 微秒",2,CYAN);
    snprintf(t,sizeof t,"逻辑 %d 峰值 %d 同步 %d",info.logic_us,info.logic_max_us,info.vsync_us);gfx_text(r,40,y+43,t,1,CREAM);
    snprintf(t,sizeof t,"提交 %d 队列等待 %d 尾部等待 %d",info.submit_us,info.qstall_us,info.tail_us);gfx_text(r,40,y+66,t,1,CREAM);
    snprintf(t,sizeof t,"弹幕 %d/%d 丢弃 %d",game_hostile_count(g),game_hostile_slots(g),game_dropped_shots(g));gfx_text(r,40,y+89,t,1,MUTED);
    snprintf(t,sizeof t,"绘制负载 %d%% 读取 %d 写入 %d MB/s",info.blt,info.rd_mbs,info.wr_mbs);gfx_text(r,40,y+112,t,1,CYAN);
    snprintf(t,sizeof t,"处理器 %d帧  硬件 %d帧  当前 %s",info.fps_mode[0],info.fps_mode[1],info.hw?"硬件":"处理器");gfx_text(r,40,y+142,t,1,GOLD);
}
#endif
static void menu_center(Gfx *r,int y,const char *s,int sc,Pixel c){center(r,640,y,s,sc,c);}
int render_title_hit(int x,int y){if(x>=360&&x<920){if(y>=405&&y<465)return 0;if(y>=485&&y<545)return 1;}return -1;}
int render_stress_hit(int x,int y){int i;for(i=0;i<4;i++)if(x>=340&&x<940&&y>=270+i*64&&y<324+i*64)return i;return -1;}
int render_upgrade_hit(int x,int y){int i;for(i=0;i<3;i++)if(x>=70+i*390&&x<430+i*390&&y>=280&&y<560)return i;return -1;}
void render_game(const Game *g,Gfx *r,RenderInfo info) {
    char t[160];int i;uint32_t t0=clock_now(),th,finish;
    fx_alpha=info.alpha_fx;draw_world(r,g,info.batch);
    if(fx_alpha&&g->phase!=PLAYING)gfx_fill_alpha(r,0,0,1280,720,INK,A_DIM);
    th=clock_now();hud(r,g,info);
    if(g->phase==TITLE){
        panel(r,240,130,800,490,EDGE);menu_center(r,164,"萝卜求生",6,GOLD);
        menu_center(r,251,"闪避弹幕 自动射击 收集经验 不断成长",2,MUTED);
        gfx_blit(r,640-player.width/2,300,&player,1);
        gfx_fill_rect(r,360,405,560,60,GREEN);menu_center(r,423,"回车 开始游戏",3,INK);
        gfx_fill_rect(r,360,485,560,60,GOLD);menu_center(r,503,"L 压力测试",3,INK);
        menu_center(r,578,"低档 / 中档 / 高档 / 自定义",2,MUTED);
    }else if(g->phase==STRESS_SELECT){
        static const char *names[]={"1 低档 32怪物 1024弹","2 中档 64怪物 2048弹","3 高档 128怪物 4096弹","4 自定义 最高40000弹"};
        panel(r,240,130,800,490,EDGE);menu_center(r,163,"压力测试",4,GOLD);menu_center(r,225,"按数字1至4选择档位",2,CREAM);
        for(i=0;i<4;i++){gfx_fill_rect(r,340,270+i*64,600,54,i==0?GREEN:i==1?GOLD:RED);menu_center(r,285+i*64,names[i],2,INK);}
        menu_center(r,577,"M 返回菜单；自定义档中 - / + 调整弹幕数",1,MUTED);
    }else if(g->phase==UPGRADE){
        panel(r,35,160,1210,460,EDGE);menu_center(r,186,"升级选择",4,GREEN);
        snprintf(t,sizeof t,"等级 %d 每次选择一项",g->level);menu_center(r,239,t,2,CREAM);
        for(i=0;i<3;i++){int x=70+i*390,id=g->choices[i];panel(r,x,280,360,280,EDGE);
            snprintf(t,sizeof t,"%d",i+1);gfx_text(r,x+24,300,t,4,GOLD);
            center(r,x+180,372,game_upgrade_name(id),2,CREAM);
            center(r,x+180,417,game_upgrade_line(id,0),1,GREEN);
            center(r,x+180,451,game_upgrade_line(id,1),1,MUTED);
            center(r,x+180,503,"按对应数字确认",1,GOLD);}
    }else if(g->phase==PAUSED||g->phase==DEFEAT){
        panel(r,300,200,680,330,g->phase==DEFEAT?RED:EDGE);
        menu_center(r,235,g->phase==DEFEAT?"游戏结束":"游戏暂停",4,g->phase==DEFEAT?RED:CREAM);
        snprintf(t,sizeof t,"存活 %02d:%02d 击败 %d 等级 %d",g->ticks/3600,g->ticks/60%60,g->kills,g->level);menu_center(r,335,t,2,GOLD);
        menu_center(r,410,g->phase==DEFEAT?"回车 再来一局":"P 继续游戏",3,GREEN);menu_center(r,480,"M 返回菜单",2,MUTED);
    }
#if SPROUT_STATS
    if(info.show_stats)stats_overlay(r,g,info);
#endif
    finish=clock_now();RSPLIT(hud,finish-th);RSPLIT(total,finish-t0);
}
