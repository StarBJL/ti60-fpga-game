#!/usr/bin/env python3
"""
th_pixgen.py - generates src/th_pix.c: the character pixel maps of the Touhou-style
game (player 4 frames, fairy / big fairy 2 wing frames, boss 2 frames).

Sources: tools/art/player.txt, tools/art/fairy.txt (LEFT half, mirrored; one
character per pixel, letters = palette entries below, '.' = transparent; a space
counts as skin 's'), the wings and the boss are drawn by the functions below.
A dark outline 'K' is added around every silhouette automatically.

    python tools/th_pixgen.py              -> src/th_pix.c (+ tools/art/preview.png if Pillow is installed)

Fairy colours: g h H (hair) and d D (dress) are replaced per fairy colour at run
time (th_art.c); every other letter uses PAL.
"""
import math, os, sys
HERE=os.path.dirname(os.path.abspath(__file__))
ART=os.path.join(HERE,'art')
PAL = {
 'K':(36,22,40),'g':(36,110,64),'h':(78,180,96),'H':(160,232,140),'s':(255,226,204),'S':(232,180,164),
 'e':(30,130,120),'E':(255,255,255),'m':(200,90,100),'w':(248,248,252),'W':(190,198,222),'r':(226,52,72),
 'R':(150,28,52),'c':(64,176,96),'C':(36,120,64),'b':(120,64,44),'l':(130,226,90),'L':(60,150,50),'y':(255,214,90),
}
def load(fn, mirror=True):
    rows=[r.rstrip('\n') for r in open(fn) if r.strip('\n')!='' or True]
    rows=[r for r in rows if r!='']
    out=[]
    for r in rows:
        r=r.replace(' ','s')
        out.append(r + r[::-1] if mirror else r)
    return out
def outline(rows):
    h=len(rows); w=len(rows[0]); g=[list(r) for r in rows]
    for y in range(h):
        for x in range(w):
            if rows[y][x]=='.':
                for dx,dy in((1,0),(-1,0),(0,1),(0,-1)):
                    X,Y=x+dx,y+dy
                    if 0<=X<w and 0<=Y<h and rows[Y][X] not in '.K': g[y][x]='K'; break
    return [''.join(r) for r in g]
def img(rows, bg=(120,120,140)):
    from PIL import Image
    h=len(rows); w=len(rows[0]); im=Image.new('RGB',(w,h),bg)
    for y,r in enumerate(rows):
        assert len(r)==w,(y,len(r))
        for x,c in enumerate(r):
            if c!='.': im.putpixel((x,y),PAL[c])
    return im
def put(rows, ov):
    g=[list(r) for r in rows]
    for x,y,c in ov: g[y][x]=c
    return [''.join(r) for r in g]
def shift_rows(rows, f):
    out=[]
    for y,r in enumerate(rows):
        s=f(y)
        if s>0: r=('.'*s+r)[:len(r)]
        elif s<0: r=(r[-s:]+'.'*(-s))
        out.append(r)
    return out
SPROUT0=[(16,4,'C'),(16,3,'C'),(16,2,'C'),(15,2,'l'),(14,1,'l'),(13,1,'l'),(14,2,'L'),(17,1,'l'),(18,1,'l'),(19,0,'l'),(18,0,'l'),(17,2,'L')]
SPROUT1=[(16,4,'C'),(16,3,'C'),(16,2,'C'),(15,2,'l'),(14,2,'l'),(13,2,'l'),(14,3,'L'),(17,1,'l'),(18,1,'l'),(19,1,'l'),(18,2,'L'),(17,2,'l')]
PIN=[(7,10,'l'),(8,10,'l'),(7,11,'L'),(8,11,'l'),(9,11,'l'),(8,12,'L')]
def player(frame):
    base=load(os.path.join(ART,'player.txt'))
    base=put(base,(SPROUT1 if frame==3 else SPROUT0)+PIN)
    lean = -1 if frame==0 else 1 if frame==2 else 0
    if lean: base=shift_rows(base, lambda y: 2*lean if y<17 else lean if y<31 else (-lean if 33<=y<41 else 0))
    return outline(base)
def wings(w,h,cx,cy,scale,flap):
    g=[['.']*w for _ in range(h)]
    for side in(-1,1):
        for (ax,ay,rx,ry,ang) in ((10,-5,8,4,-25),(9,3,6,3,20)):
            a=math.radians((ang+flap)*side*-1)
            ex=cx+side*ax*scale; ey=cy+ay*scale
            for y in range(h):
                for x in range(w):
                    dx=x+0.5-ex; dy=y+0.5-ey
                    u=dx*math.cos(a)+dy*math.sin(a); v=-dx*math.sin(a)+dy*math.cos(a)
                    d=(u/(rx*scale))**2+(v/(ry*scale))**2
                    if d<=1: g[y][x]= 'V' if d>0.55 else 'v'
    return [''.join(r) for r in g]
def over(bg, fg, ox=0, oy=0):
    g=[list(r) for r in bg]
    for y,r in enumerate(fg):
        for x,c in enumerate(r):
            if c!='.': g[y+oy][x+ox]=c
    return [''.join(r) for r in g]
def fairy(big, flap):
    body=load(os.path.join(ART,'fairy.txt'))
    if not big:
        w=wings(32,32,16,14,1.0,flap)
        return outline(over(w,body))
    w=wings(48,48,24,22,1.5,flap)
    return outline(over(w,body,8,9))
PAL.update({'v':(200,236,255),'V':(140,200,240),'d':(70,110,230),'D':(40,70,170)})
PAL.update({'p':(240,128,190),'P':(255,206,232),'q':(176,64,140),'k':(255,176,206),'n':(214,86,146),
            'y':(255,214,90),'Y':(196,146,40),'u':(170,90,210),'U':(96,40,140)})
W,H=64,80
def blank(): return [['.']*W for _ in range(H)]
def fill(g, f):
    for y in range(H):
        for x in range(W):
            c=f(x+0.5,y+0.5)
            if c: g[y][x]=c
def boss(frame):
    g=blank(); cx=32
    sway = 1 if frame else 0
    # back hair: head circle + curtain
    def hair(x,y):
        dx=abs(x-cx)
        if (dx/17)**2+((y-24)/17)**2<=1: return 'p'
        if 24<=y<=66:
            hw=17+ (y-24)*0.12 + (1.5*math.sin(y*0.35+sway) if y>40 else 0)
            tip = 66-3*abs(math.sin(dx*0.55))
            if dx<=hw and y<=tip:
                return 'q' if int(dx+ (y*0.15))%5==0 else 'p'
        return None
    fill(g,hair)
    # sleeves
    def sleeve(x,y):
        dx=abs(x-cx)
        y0,y1 = 32,(52 if frame==0 else 46)
        if y<y0 or y>y1: return None
        t=(y-y0)/(y1-y0)
        inner=10-2*t; outer=12+15*t if frame==0 else 12+18*t
        if inner<=dx<=outer:
            if y>y1-2.5: return 'n'
            if y>y1-4.5: return 'k'
            return 'W' if dx>outer-2 else 'w'
    fill(g,sleeve)
    # hands at sleeve ends
    for side in(-1,1):
        hx = cx+side*(24 if frame==0 else 27); hy = 53 if frame==0 else 47
        for yy in range(hy,hy+3):
            for xx in range(hx-1,hx+2): g[yy][int(xx)]='s'
    # skirt
    def skirt(x,y):
        dx=abs(x-cx)
        if 47<=y<=72:
            hw=10+(y-47)*0.5
            if dx<=hw:
                if int(dx)%5==3: return 'W'
                return 'w'
        if 69<=y<=76 and dx<=23:
            k=(dx%5)-2.5
            d=k*k+(y-72.5)**2
            if d<=5.2 or y<=72.0: return 'k' if d<=2.5 or y<=72.0 else 'n'
    fill(g,skirt)
    # torso
    def torso(x,y):
        dx=abs(x-cx)
        if 31<=y<=48 and dx<=9.5-max(0,(y-40))*0.25:
            if dx<=2.5: return 'k' if y>33 else 'n'
            return 'W' if dx>7.5 else 'w'
    fill(g,torso)
    # sash + bow
    for y in range(45,49):
        for x in range(cx-11,cx+11): g[y][x]='y' if y<48 else 'Y'
    for (dx,dy,c) in [(-4,-2,'n'),(-3,-2,'n'),(-5,-1,'n'),(-4,-1,'k'),(-3,-1,'k'),(-2,-1,'n'),(-4,0,'n'),(-3,0,'n'),
                      (-1,-1,'r'),(0,-1,'r'),(-1,0,'r'),(0,0,'r'),(-1,1,'n'),(0,2,'n'),(-2,2,'n'),(0,3,'n')]:
        for s in(1,-1):
            xx = cx+dx if s==1 else cx-1-dx
            g[46+dy][xx]=c
    # neck
    for y in range(28,32):
        for x in range(cx-3,cx+3): g[y][x]='S' if y<30 else 's'
    # face
    def face(x,y):
        dx=abs(x-cx)
        if (dx/9.2)**2+((y-21)/8.6)**2<=1 and y>=13: return 'S' if dx>7.6 else 's'
    fill(g,face)
    # fringe + top hair (front)
    def fringe(x,y):
        dx=abs(x-cx)
        if (dx/15)**2+((y-21)/14)**2<=1:
            fb = 17 + 2.6*abs(math.sin((x-cx)*0.5)) + (1 if dx<1.5 else 0)
            if y<=fb: 
                arc = 10+ (dx/13)**2*4
                if abs(y-arc)<0.8 and dx<11: return 'P'
                return 'p'
        # side locks in front
        if 9<=dx<=12.5 and 16<=y<=44-(dx-9)*1.5:
            return 'q' if dx>11.6 else 'p'
    fill(g,fringe)
    # eyes
    eye=["KKKKK","KEuuU","KuuUU","KuUUU","SKKKS"]
    for r,row in enumerate(eye):
        for c,ch in enumerate(row):
            if ch=='.': continue
            g[20+r][cx-8+c]=ch
            g[20+r][cx+7-c]=ch
    g[27][cx-1]='m'; g[27][cx]='m'
    for s in (-1,1):
        g[26][cx+s*7 - (1 if s<0 else 0)]='k'; g[26][cx+s*6-(1 if s<0 else 0)]='k'
    # flower crown
    def flower(fx,fy,petal,center):
        for (dx,dy) in((0,-1),(-1,0),(1,0),(0,1),(-1,-1),(1,1),(1,-1),(-1,1)):
            c = petal if (dx==0 or dy==0) else ('n' if petal=='k' else petal)
            if dx and dy: continue
            g[fy+dy][fx+dx]=c
        g[fy][fx]=center
    for (fx,fy,p) in [(32,8,'k'),(25,9,'w'),(39,9,'w'),(20,12,'k'),(44,12,'k')]:
        flower(fx,fy,p,'y')
    for (lx,ly) in [(28,9),(36,9),(22,10),(42,10)]: g[ly][lx]='c'
    rows=[''.join(r) for r in g]
    return outline(rows)

PAL.update({'p':(240,128,190),'P':(255,206,232),'q':(176,64,140),'k':(255,176,206),'n':(214,86,146),
            'y':(255,214,90),'Y':(196,146,40),'u':(170,90,210),'U':(96,40,140)})

# ---------------------------------------------------------------- stage 2 boss: moon princess
PAL.update({'a':(206,206,238),'A':(248,248,255),'z':(140,140,196),'o':(36,44,108),'O':(70,88,160),
            'x':(255,246,190),'i':(206,220,255),'t':(150,30,40)})

def boss2(frame):
    g=blank(); cx=32
    sway = 1 if frame else 0
    # very long straight silver hair, down to the hem
    def hair(x,y):
        dx=abs(x-cx)
        if (dx/16)**2+((y-23)/16)**2<=1: return 'a'
        if 23<=y<=77:
            hw=16+(y-23)*0.16+(1.2*math.sin(y*0.3+sway) if y>44 else 0)
            tip=77-4*abs(math.sin(dx*0.5))
            if dx<=hw and y<=tip:
                return 'z' if int(dx+y*0.1)%4==0 else 'a'
        return None
    fill(g,hair)
    # sleeves: navy outside, light inside, gold edge
    def sleeve(x,y):
        dx=abs(x-cx)
        y0,y1 = 31,(54 if frame==0 else 45)
        if y<y0 or y>y1: return None
        t=(y-y0)/(y1-y0)
        inner=9-2*t; outer=12+(16 if frame==0 else 19)*t
        if inner<=dx<=outer:
            if y>y1-1.5: return 'y'
            if dx<inner+3 and y>y0+4: return 'i'
            return 'O' if dx>outer-2.5 else 'o'
    fill(g,sleeve)
    for side in(-1,1):
        hx = cx+side*(25 if frame==0 else 28); hy = 55 if frame==0 else 46
        for yy in range(hy,hy+3):
            for xx in range(hx-1,hx+2): g[yy][int(xx)]='s'
    # long dress
    def skirt(x,y):
        dx=abs(x-cx)
        if 46<=y<=76:
            hw=9+(y-46)*0.52
            if dx<=hw:
                if y>=74: return 'y'
                if dx<=2.5: return 'i'
                if (int(x)*7+int(y)*13)%41==0: return 'x'          # star dots
                return 'O' if dx>hw-2 else 'o'
    fill(g,skirt)
    def torso(x,y):
        dx=abs(x-cx)
        if 30<=y<=47 and dx<=9.5-max(0,(y-40))*0.3:
            if dx<=2.5: return 'i'
            if dx<=3.5: return 'y'
            return 'O' if dx>7.5 else 'o'
    fill(g,torso)
    # silver sash with a gold moon clasp
    for y in range(44,47):
        for x in range(cx-10,cx+10): g[y][x]='A' if y==44 else 'a'
    for (dx,dy) in [(-1,-1),(0,-1),(-2,0),(1,0),(-2,1),(1,1),(-1,2),(0,2)]:
        g[45+dy][cx+dx]='y'
    for y in range(27,31):
        for x in range(cx-3,cx+3): g[y][x]='S' if y<29 else 's'
    def face(x,y):
        dx=abs(x-cx)
        if (dx/9.0)**2+((y-20)/8.5)**2<=1 and y>=12: return 'S' if dx>7.4 else 's'
    fill(g,face)
    def fringe(x,y):
        dx=abs(x-cx)
        if (dx/15)**2+((y-20)/14)**2<=1:
            fb = 15.5 + 1.8*abs(math.sin((x-cx)*0.7))      # straight "hime" fringe
            if y<=fb:
                arc = 9+(dx/13)**2*4
                if abs(y-arc)<0.8 and dx<11: return 'A'
                return 'a'
        if 9<=dx<=12.5 and 15<=y<=48-(dx-9)*1.2:          # long side locks
            return 'z' if dx>11.6 else 'a'
    fill(g,fringe)
    eye=["KKKKK","KErrR","KrrRR","KrRtt","SKKKS"]
    for r,row in enumerate(eye):
        for c,ch in enumerate(row):
            g[19+r][cx-8+c]=ch
            g[19+r][cx+7-c]=ch
    g[26][cx-1]='m'; g[26][cx]='m'
    # crescent moon crown (left of the head) + a small star on the right
    for y in range(2,14):
        for x in range(14,30):
            d1=(x-21.5)**2+(y-8)**2; d2=(x-24.0)**2+(y-6.5)**2
            if d1<=30 and d2>18: g[y][x]='y' if d1<=22 else 'Y'
    for (dx,dy) in [(0,-2),(0,-1),(-2,0),(-1,0),(0,0),(1,0),(2,0),(0,1),(0,2)]:
        g[9+dy][41+dx]='x'
    rows=[''.join(r) for r in g]
    return outline(rows)

def c_maps(name, maps, w, h):
    out=['const char %s[%d][%d] = {' % (name, len(maps), w*h+1)]
    for m in maps:
        assert len(m)==h and all(len(r)==w for r in m)
        out.append('  {')
        out += ['    "%s"' % r for r in m]
        out.append('  },')
    out.append('};')
    return '\n'.join(out)

def main():
    players=[player(f) for f in range(4)]
    fairies=[fairy(0,0), fairy(0,25)]
    bigs=[fairy(1,0), fairy(1,25)]
    bosses=[boss(0), boss(1)]
    bosses2=[boss2(0), boss2(1)]
    used=sorted(set(''.join(''.join(m) for m in players+fairies+bigs+bosses+bosses2))-{'.'})
    src=['/* th_pix.c - GENERATED by tools/th_pixgen.py, do not edit by hand:',
         ' * change tools/art/player.txt, fairy.txt or the script and run  python tools/th_pixgen.py',
         ' * Character pixel maps (one char per pixel, \'.\' = transparent), see th_art.c. */',
         '#include "th.h"', '',
         'const ThPixPal th_pix_pal[] = {']
    src += ['    { \'%s\', %3d, %3d, %3d },' % ((c,)+PAL[c]) for c in used]
    src += ['    { 0, 0, 0, 0 }', '};', '']
    src.append(c_maps('th_pix_player', players, 32, 48)); src.append('')
    src.append(c_maps('th_pix_fairy', fairies, 32, 32)); src.append('')
    src.append(c_maps('th_pix_bigfairy', bigs, 48, 48)); src.append('')
    src.append(c_maps('th_pix_boss', bosses, 64, 80)); src.append('')
    src.append(c_maps('th_pix_boss2', bosses2, 64, 80)); src.append('')
    dst=os.path.join(HERE,'..','src','th_pix.c')
    open(dst,'w',newline='\n').write('\n'.join(src))
    print('wrote', os.path.normpath(dst))
    try:
        from PIL import Image
        ims=[img(m) for m in players+fairies+bigs+bosses+bosses2]
        Wt=sum(i.width+8 for i in ims); Ht=max(i.height for i in ims)
        S=Image.new('RGB',(Wt,Ht),(40,44,80)); x=0
        for i in ims: S.paste(i,(x,0)); x+=i.width+8
        S=S.resize((S.width*4,S.height*4),Image.NEAREST)
        S.save(os.path.join(ART,'preview.png')); print('wrote tools/art/preview.png')
    except ImportError:
        pass

if __name__=='__main__':
    main()
