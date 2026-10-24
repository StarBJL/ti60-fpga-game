"""PTY-only browser/serial fixture. Never opens a physical serial device."""
from pathlib import Path
import os,sys,json,threading,select,time,re
v=Path(__file__).resolve().parent;root=v.parents[2];sys.path.insert(0,str(root/'tools/control_panel'))
from server import Session,make_server
master,slave=os.openpty();port=os.ttyname(slave);session=Session(v/'pty_logs')
state=dict(scene=2,count=1000,step=100,bullet=13,square=0,alpha=1,vsync=1,hw=1,auto=0,motion=1,table=1,bgm=1,volume=256,tone=0,stats=0,verbose=0,period=20,resolution=0)
running=True;sequence=0;was_connected=False;buffer=b''
commands=[]
def send(s):os.write(master,s.encode())
def board():
 global buffer,sequence,was_connected
 last=0
 while running:
  if session.serial is not None:
   if not was_connected:send('=== TI60-R5-PTY-TEST (synthetic serial test, not board performance) ===\r\n');was_connected=True
   if select.select([master],[],[],.02)[0]:
    b=os.read(master,4096);buffer+=b
    if buffer==b'h':send('R5: PTY fixture\r\n');buffer=b''
    while b'\n' in buffer:
     line,buffer=buffer.split(b'\n',1);line=line.decode().strip();m=re.fullmatch(r':(\d+) (\w+)(?: (-?\d+))?',line)
     if not m:continue
     rid,name=m[1],m[2];value=int(m[3]) if m[3] is not None else None;commands.append(dict(id=int(rid),command=name,value=value,time=time.monotonic()))
     (v/'pty_commands.json').write_text(json.dumps(commands,indent=2))
     if name=='delta':state['count']=max(1,min(40000,state['count']+value*state['step']))
     elif name=='backend':state['hw']=value
     elif name in state:state[name]=value
     if name=='scene' and value==0:state['bullet']=100
     if name=='scene' and value==1:state['bullet']=13
     send(f'ACK id={rid} ok=1 code=ok\r\n')
     if name not in ('move','stop'):
      send('S4 id='+rid+' epoch=1 phase=2 cap=40000 '+' '.join(f'{k}={state[k]}' for k in ['scene','step','count','bullet','square','alpha','vsync','hw','auto','stats'])+'\r\n')
      send('A4 id='+rid+' track=bgm_stress density=16 stress_auto=0 '+' '.join(f'{k}={state[k]}' for k in ['bgm','volume','tone','motion','table','verbose','period'])+'\r\n')
   if time.monotonic()-last>.3:
    last=time.monotonic();sequence+=1;s=sequence
    send(f'V5 resolution={state["resolution"]} output_w={1920 if state["resolution"] else 1280} output_h={1080 if state["resolution"] else 720} nominal_hz={30 if state["resolution"] else 60} render_w=1280 render_h=720 scale={"1.5" if state["resolution"] else "1"} pending=0 supported=1\r\n')
    send(f'M4 seq={s} epoch=1 phase=2 target={state["count"]} '+' '.join(f'{k}={state[k]}' for k in ['scene','count','step','bullet','square','alpha','vsync','hw','motion','table','bgm'])+'\r\n')
    send(f'F4 seq={s} elapsed_us=1000000 render10=701 logic10=600 present10=600 repeat10=0 scan10=600 valid=1 video_uf=0 audio_uf=0 drops=0\r\n')
    send(f'P4 seq={s} logic_us=1000 logic_max_us=1200 submit_us=900 wait_us=12000 work_us=13900 work_max_us=15000 over=0 busy=90 phase1_us=2200 bands_us=11000\r\n')
    send(f'C4 seq={s} font_hit=500 font_miss=20 font_reset=0 label_hit=40 label_miss=5 font_pixels=89088 track=bgm_stress w=1280 h=720 auto=0 density=16 stress_auto=0\r\n')
  else:was_connected=False;time.sleep(.03)
http=make_server(session,0)
(v/'pty_fixture.json').write_text(json.dumps({'port':port,'url':f'http://127.0.0.1:{http.server_port}','pid':os.getpid()}))
threading.Thread(target=board,daemon=True).start();print('PTY fixture ready',port,flush=True)
try:http.serve_forever(poll_interval=.1)
except KeyboardInterrupt:pass
finally:running=False;session.disconnect();http.server_close();os.close(master);os.close(slave)
