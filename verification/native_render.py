from pathlib import Path
import re
root=Path(__file__).resolve().parents[1];s=root/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src'
f=s/'render.c';x=f.read_text(encoding='utf-8')
x=x.replace('#define ARENA_X0 14','#define ARENA_X0 20').replace('#define ARENA_Y0 60','#define ARENA_Y0 90').replace('#define ARENA_X1 626','#define ARENA_X1 1260').replace('#define ARENA_Y1 443','#define ARENA_Y1 666')
x=x.replace('14,60,612,383','20,90,1240,576').replace('18,64,604,375','24,94,1232,568')
x=x.replace('x<624','x<1258').replace('x,60','x,90').replace('x,437','x,660').replace('y=80;y<439','y=110;y<662').replace('14,y','20,y').replace('620,y','1254,y')
for a,b in [('598u','1224u'),('586u','1212u'),('370u','562u'),('358u','550u')]:x=x.replace(a,b)
# Static scaling at startup, shared by CPU and hardware draws. Original pixels
# are retained and sampled with nearest-neighbour; no per-frame resampling.
at=x.index('void render_init(void)')
helper='''static Pixel native_sprite_px[48000];static uint8_t native_sprite_a[48000];static unsigned native_used;
static void native_sprite(Sprite *sp,int w,int h){
    unsigned off=native_used;int x,y,sw=sp->width,sh=sp->height;
    if(off+(unsigned)(w*h)>48000u)return;
    for(y=0;y<h;y++)for(x=0;x<w;x++){
        int a=y*sh/h*sw+x*sw/w;native_sprite_px[off+y*w+x]=sp->pixels[a];
        if(sp->alpha)native_sprite_a[off+y*w+x]=sp->alpha[a];
    }
    sp->pixels=native_sprite_px+off;if(sp->alpha)sp->alpha=native_sprite_a+off;
    sp->width=w;sp->height=h;native_used+=w*h;
}
'''
x=x[:at]+helper+x[at:]
needle='    init_shadow_sprites();\n}'
replacement='''    init_shadow_sprites();
    native_used=0;
    {Sprite *actors[]={&player,&slime,&runner,&brute,&elite,&spitter,&gem};int i;
     for(i=0;i<7;i++)native_sprite(actors[i],actors[i]->width*3/2,actors[i]->height*3/2);
     for(i=0;i<4;i++)native_sprite(&hostile_sprites[i],13,13);
     for(i=0;i<SHADOW_COUNT;i++)native_sprite(&shadow_sprites[i],shadow_sprites[i].width*3/2,shadow_sprites[i].height*3/2);
    }
}'''
assert needle in x;x=x.replace(needle,replacement)
x=x.replace('gfx_blit_alpha(r,x-12,y-22-bob,&player','gfx_blit_alpha(r,x-player.width/2,y-player.height+15-bob,&player')
# All seed paths use the same enlarged sprite origin and dimensions.
start=x.index('static int draw_seeds');end=x.index('static void hud',start)
part=x[start:end].replace('x-4,y-4','x-6,y-6').replace('b->x/FIX-4','b->x/FIX-6').replace('b->y/FIX-4','b->y/FIX-6').replace('x<=-9','x<=-13').replace('y<=-9','y<=-13').replace('*9u*9u','*13u*13u')
x=x[:start]+part+x[end:]
at=x.index('static void hud');x=x[:at]+(root/'verification/native_ui.c.inc').read_text(encoding='utf-8')
f.write_text(x,encoding='utf-8')
f=s/'game.c';x=f.read_text(encoding='utf-8')
for a,b in [('x=30+rand_n(g,580); y=80+rand_n(g,344);','x=45+rand_n(g,1190); y=120+rand_n(g,516);'),('x=20; else if (side==1) x=620','x=30; else if (side==1) x=1250'),('y=68; else y=432','y=102; else y=648'),('g->y=252*FIX','g->y=378*FIX'),('28*FIX,612*FIX','42*FIX,1238*FIX'),('82*FIX,422*FIX','123*FIX,633*FIX'),('22*FIX,618*FIX','33*FIX,1247*FIX'),('80*FIX,424*FIX','120*FIX,636*FIX')]:x=x.replace(a,b)
# Keep stress arena travel times comparable after the larger physical extent.
x=x.replace('b->x<8*FIX','b->x<16*FIX')
f.write_text(x,encoding='utf-8')
print('Native game world, static scaled original sprites and four-tier Chinese UI ready.')
