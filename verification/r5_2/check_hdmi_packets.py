"""Decode final TMDS characters, not internal packet-builder wires.
HDMI 1.4 Table 5-13: SB6 = PR CR UR VR PL CL UL VL. The inherited
checker used a mutually wrong field order; this test uses the corrected decoder.
"""
from pathlib import Path
import sys,json,math,collections
v=Path(__file__).resolve().parent
sys.path.insert(0,str(v.parents[1]/'PROJECT/sim/hdmi'))
import tb_hdmi as ref
ref.MODE720=True;ref.POL=1;ref.CTS=74375;ref.F_PIX=74.375e6;ref.PB2=0x28
class Decoder(ref.TB):
    def __init__(self):
        self.pk=[];self.rx=[];self.rx_t=[];self.nisl=0;self.t=0
        self.pcm=[];self.modes=collections.Counter();self.lines=collections.Counter()
        self.phase=0;self.mode=0
    def sync_levels(self,h,v):
        hs=int(ref.H_ACT+ref.H_FP<=h<ref.H_ACT+ref.H_FP+ref.H_SYNC)
        vss,vse=self.v_act+ref.V_FP,self.v_act+ref.V_FP+ref.V_SYNC
        vs=int((v==vss-1 and h>=ref.H_ACT+ref.H_FP) or (vss<=v<vse-1) or (v==vse-1 and h<ref.H_ACT+ref.H_FP))
        return hs,vs
    def packet(self,v,ch):
        a=len(self.rx);super().packet(v,ch)
        for x in self.rx[a:]:self.pcm.append((self.mode,self.phase,self.t,*x))
        self.modes[(self.mode,self.pk[-1][2][0])]+=1
    def line(self,rows):
        mode,phase,vy=rows[0][1],rows[0][2],rows[0][4]
        self.mode,self.phase=mode,phase
        self.v_act,self.v_tot=(1080,1125) if mode else (720,750)
        ref.H_ACT,ref.H_TOT,ref.H_FP,ref.H_SYNC,ref.V_FP,ref.V_SYNC,ref.VIC=(1920,2200,88,44,4,5,34) if mode else (1280,1650,110,40,5,5,4)
        hstart=0 if vy>=self.v_act else ref.H_ACT
        if len(rows)!=ref.H_TOT-hstart:return # only the partial transition line
        syms={r[3]:(r[3],r[4],False,r[5:]) for r in rows}
        if set(syms)!=set(range(hstart,ref.H_TOT)):raise AssertionError('counter discontinuity')
        self.t=rows[-1][0];self.lines[mode]+=1;h=hstart
        next_act=vy<self.v_act-1 or vy==self.v_tot-1
        while h<ref.H_TOT:
            _,_,_,(c0,c1,c2)=syms[h];hs,vs=self.sync_levels(h,vy)
            if h==ref.H_ACT+4 and ref.CTRL.get(c1)==1 and ref.CTRL.get(c2)==1:
                h=self.check_island(vy,syms,h);continue
            if next_act and ref.H_TOT-10<=h<ref.H_TOT-2:
                ok=ref.CTRL.get(c0)==(vs*2+hs) and ref.CTRL.get(c1)==1 and ref.CTRL.get(c2)==0
                if not ok:ref.err(f'video preamble mode{mode} v{vy} h{h}')
            elif next_act and h>=ref.H_TOT-2:
                if (c0,c1,c2)!=(ref.GB_A,ref.GB_B,ref.GB_A):ref.err(f'video guard mode{mode} v{vy} h{h}')
            elif not (ref.CTRL.get(c0)==vs*2+hs and ref.CTRL.get(c1)==0 and ref.CTRL.get(c2)==0):
                ref.err(f'control / CTA sync mode{mode} v{vy} h{h}')
            h+=1

d=Decoder();rows=[];old=None
path=Path(sys.argv[1]) if len(sys.argv)>1 else v/'tmds_blank.txt'
with path.open() as f:
    for line in f:
        fields=line.split();r=tuple(int(x,16 if i>=5 else 10) for i,x in enumerate(fields))
        key=r[1],r[4]
        if rows and (key!=old or r[3]<=rows[-1][3]):d.line(rows);rows=[]
        old=key;rows.append(r)
if rows:d.line(rows)
# FIFO-fed PCM must survive both mode switches without loss or reordering.
normal=[x for x in d.pcm if x[1]==0]
for k,x in enumerate(normal):
    expected=((k*37+11)&65535,((k*1001)^0x5a5a)&65535)
    if x[3:5]!=expected:ref.err(f'PCM sample {k}: {x[3:5]} != {expected}');break
# The B bit anchors the 192-sample IEC block independently of line timing.
bidx=[k for k,x in enumerate(d.rx) if x[2]]
for a,b in zip(bidx,bidx[1:]):
    if b-a!=192:ref.err(f'IEC block distance {b-a}')
if bidx:
    for k,x in enumerate(d.rx[bidx[0]:]):
        if x[3]!=int(k%192 in (2,25,33)):ref.err(f'IEC channel status sample{k}');break
# Ignore only the first line's pending FIFO samples when switching to tone.
tone=[x for x in d.pcm if x[1]==1][16:]
if tone:
    phases=[k for k,p in enumerate(ref.TONE) if p==tone[0][3]]
    if not any(all(x[3]==x[4]==ref.TONE[(ph+k)%48] for k,x in enumerate(tone)) for ph in phases):ref.err('1 kHz tone sequence')
if len(d.rx_t)>500:
    hz=(len(d.rx_t)-201)*ref.F_PIX/(d.rx_t[-1]-d.rx_t[200])
    if abs(hz-48000)>150:ref.err(f'PCM rate {hz}')
else:hz=None;ref.err('insufficient audio coverage')
for mode in (0,1):
    for ty in (1,2,0x82,0x84):
        if not d.modes[mode,ty]:ref.err(f'no packet mode{mode} type{ty}')
categories=collections.Counter('control / CTA sync' if x.startswith('control / CTA sync') else x.split(':')[0] for x in ref.errors)
result=dict(error_categories=dict(categories),passed=not ref.errors,error_count=len(ref.errors),errors=ref.errors[:30],blank_lines=dict(d.lines),packets={f'{m}:{t:02x}':n for (m,t),n in d.modes.items()},fifo_samples=len(normal),tone_samples=len(tone),audio_rate_hz=hz,hardware_tested=False)
(v/('packets_result.json' if path==v/'tmds_blank.txt' else 'packets_baseline_result.json')).write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2));sys.exit(bool(ref.errors))
