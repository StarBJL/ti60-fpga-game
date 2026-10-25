"""PTY-only browser/serial fixture. Never opens a physical serial device."""
from pathlib import Path
import os,sys,json,threading,select,time,re
v=Path(__file__).resolve().parent;root=v.parents[1];sys.path.insert(0,str(root/'tools/control_panel'))
from server import Session,make_server
master,slave=os.openpty();port=os.ttyname(slave);session=Session(v/'pty_logs')
state=dict(link_supported=1,link_stage=0,link_status=4,link_dvi=0,link_fault=0,recoveries=0,coop_start_supported=1,video_recover_supported=1,trial_supported=1,video_trial=0,trial_ms=0,core_hz=202083333,players=1,matrix_auto=1,phase=2,scene=1,count=1000,step=100,bullet=13,square=0,alpha=1,vsync=1,hw=1,auto=1,motion=1,table=1,bgm=1,volume=256,tone=0,stats=0,verbose=0,period=20,resolution=0,music_auto=1,music_index=9,music_total=10,bars=0,submit_mode=3,band=0,cache=0,overlap=0,overlap_active=0,seed_table=1,seed_build=0,sort=1,kpath=1,rd_bursts=16,wr_bursts=128,rd_words=64,wide=0)
trial_deadline=0
link_deadline=0
running=True;sequence=0;epoch=1;was_connected=False;buffer=b''
commands=[]
diag=dict(input_diag_supported=1,input_diag=0,matrix_events=0,matrix_last=0,matrix_seen=0,matrix_logical=0,matrix_cal=0,matrix_step=0,matrix_cal_error=0,matrix_wait=0,matrix_mapped=0,matrix_map='x0123456789abcdef',matrix_candidate='x----------------',keyboard_bytes=0,keyboard_headers=0,keyboard_tail='x',keyboard_faults=0,keyboard_acks=0,keyboard_tx_timeouts=0,mouse_reports=0,status_reports=0)
cal=[];seen_stimulus=''
def diagnostics(rid=0):
 send(f'I5 id={rid} supported=1 held=0 matrix_keys=0 matrix_held=0 keyboard_held=0 keyboard_reports={diag.get("test_reports",0)} malformed=0 rollover=0 ghosts=0 status_reports=0 uart_error=0\r\n')
 send(f'I9 id={rid} '+' '.join(f'{k}={val}' for k,val in diag.items() if not k.startswith('test_'))+'\r\n')

songs=["bgm_title","bgm_menu2","bgm_stage1","bgm_stage2","bgm_boss1","bgm_boss2","bgm_garden","bgm_garden2","bgm_stress","bgm_stress2"]
def send(s):os.write(master,s.encode())
def board():
 global buffer,sequence,was_connected,epoch,trial_deadline,link_deadline,cal,seen_stimulus
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
     if name=='input_diag':diag['input_diag']=value
     if name=='matrix_cal':
      if value==1:cal=[];diag.update(input_diag=1,matrix_cal=1,matrix_step=0,matrix_cal_error=0,matrix_wait=0,matrix_candidate='x----------------')
      if value==0:diag['matrix_cal']=0
      if value==2:diag.update(matrix_cal=0,matrix_mapped=1,matrix_map='x'+''.join(format(x,'x') for x in cal))
      if value==3:cal=cal[:-1]
     if name=='matrix_bind':cal.append(value&15)
     if name in ('matrix_bind','matrix_cal'):
      diag['matrix_step']=len(cal);diag['matrix_candidate']='x'+''.join(format(x,'x') for x in cal)+'-'*(16-len(cal))
     if name=='keyboard_sync':diag['keyboard_acks']+=1
     if name not in ('status','stop','move'):epoch+=1
     if name=='scene':state['phase']=0
     if name=='coop':state.update(scene=value,auto=0,players=3,phase=2 if value==1 else 1)
     if name=='video_recover':state.update(resolution=0,video_trial=0,link_stage=1,link_dvi=0,recoveries=state['recoveries']+1);link_deadline=time.monotonic()+.4
     if name=='video_dvi':state.update(resolution=1,video_trial=1,link_dvi=value,link_stage=1);link_deadline=time.monotonic()+.4;trial_deadline=time.monotonic()+10
     if name=='start':state.update(players=3,phase=1)
     if name=='resolution':state["video_trial"]=int(value==1);trial_deadline=time.monotonic()+10;state["link_stage"]=1;link_deadline=time.monotonic()+.4
     if name=='video_confirm':state["video_trial"]=0
     if name=='stress':state['phase']=2;state['count']=99999;state['players']=1
     if name in ('next','prev'):state['music_index']=(state['music_index']+(1 if name=='next' else -1))%10;state['music_auto']=0
     if name=='music':state['music_auto']=int(value==-1);state['music_index']=([2,6,9,1][state['scene']] if value==-1 else value)
     if name=='fast':state.update(wide=16781312,rd_bursts=16,wr_bursts=128,rd_words=64,hw=1,submit_mode=2,band=1,cache=1,overlap=1,seed_table=1,seed_build=1,sort=1,kpath=1,motion=1,table=1)
     if name=='key' and value==53:state['band']=1-state['band']
     if name=='volume_delta':state['volume']=(max(0,min(100,(state['volume']*100+128)//256+5*value))*256+50)//100
     if name=='delta':state['count']=max(1,min(99999,state['count']+value*state['step']))
     elif name=='backend':state['hw']=value
     elif name in state:state[name]=value
     if name=='scene' and state['music_auto']:state['music_index']=[2,6,9,1][value]
     if name=='scene' and value==0:state['bullet']=100
     if name=='scene' and value==1:state['bullet']=13
     state['overlap_active']=int(state['hw'] and state['overlap'] and state['scene']!=2 and not state['resolution'])
     send(f'ACK id={rid} ok=1 code=ok\r\n')
     diagnostics(rid)
     if name not in ('move','stop'):
      send('S4 id='+rid+f' epoch={epoch} phase={state["phase"]} cap=99999 '+' '.join(f'{k}={state[k]}' for k in ['scene','step','count','bullet','square','alpha','vsync','hw','auto','stats'])+'\r\n')
      send('E5 id='+rid+' accel_supported=1 volume_step_supported=1 '+' '.join(f'{k}={state[k]}' for k in ['submit_mode','band','cache','overlap','overlap_active','seed_table','seed_build','sort','kpath','rd_bursts','wr_bursts','rd_words','wide'])+'\r\n')
      send('A4 id='+rid+f' track={songs[state["music_index"]]} pressure_supported=1 music_supported=1 coop_start_supported=1 video_recover_supported=1 music_auto={state["music_auto"]} music_index={state["music_index"]} music_total=10 density=16 stress_auto=0 '+' '.join(f'{k}={state[k]}' for k in ['bgm','volume','tone','motion','table','verbose','period'])+'\r\n')
      send(f'V5 id={rid} resolution={state["resolution"]} output_w={1920 if state["resolution"] else 1280} output_h={1080 if state["resolution"] else 720} nominal_hz={30 if state["resolution"] else 60} render_w=1280 render_h=720 scale={"1.5" if state["resolution"] else "1"} pending=0 supported=1 link_supported=1 link_stage={state["link_stage"]} link_status=4 link_dvi={state["link_dvi"]} link_fault=0 recoveries={state["recoveries"]} trial_supported=1 video_trial={state["video_trial"]} trial_ms={state["trial_ms"]} video_rev=328193 bars={state["bars"]}\r\n')
   stim=v/'stimulus.json'
   if stim.exists():
    text=stim.read_text()
    if text!=seen_stimulus:
     seen_stimulus=text;x=json.loads(text)
     if 'bit' in x:
      bit=x['bit'];diag['matrix_events']+=1;diag['matrix_last']=1<<bit;diag['matrix_seen']|=1<<bit
      if diag['matrix_cal'] and len(cal)<16:
       if bit in cal:diag['matrix_cal_error']=2
       else:cal.append(bit);diag.update(matrix_step=len(cal),matrix_cal_error=0,matrix_candidate='x'+''.join(format(q,'x') for q in cal)+'-'*(16-len(cal)))
     if 'keyboard' in x:diag.update(keyboard_bytes=22,keyboard_headers=2,keyboard_tail='x000000000057ab010000000000000000',test_reports=2)
     diagnostics()
   if state['link_stage'] and time.monotonic()>=link_deadline:state['link_stage']=0
   if state['video_trial']:
    state['trial_ms']=max(0,int((trial_deadline-time.monotonic())*1000))
    if not state['trial_ms']:state.update(video_trial=0,resolution=0);epoch+=1
   if time.monotonic()-last>.3:
    last=time.monotonic();sequence+=1;s=sequence
    send(f'V5 resolution={state["resolution"]} output_w={1920 if state["resolution"] else 1280} output_h={1080 if state["resolution"] else 720} nominal_hz={30 if state["resolution"] else 60} render_w=1280 render_h=720 scale={"1.5" if state["resolution"] else "1"} pending=0 supported=1 link_supported=1 link_stage={state["link_stage"]} link_status=4 link_dvi={state["link_dvi"]} link_fault=0 recoveries={state["recoveries"]} trial_supported=1 video_trial={state["video_trial"]} trial_ms={state["trial_ms"]} video_rev=328193 bars={state["bars"]}\r\n')
    diagnostics()
    send(f'T7 seq={s} players={state["players"]} matrix_auto=1 p1_life=90 p1_skill=60 p1_score=5 p1_x=200 p1_y=300 p1_held=4 p2_life=80 p2_skill=120 p2_score=8 p2_x=400 p2_y=250 p2_held=1 p3_life=70 p3_skill=180 p3_score=12 p3_x=600 p3_y=350 p3_held=8\r\n')
    send(f'M4 seq={s} epoch={epoch} phase={state["phase"]} target={state["count"]} '+' '.join(f'{k}={state[k]}' for k in ['scene','count','step','bullet','square','alpha','vsync','hw','motion','table','bgm','submit_mode','band','cache','overlap','overlap_active','seed_table','seed_build','sort','kpath','rd_bursts','wr_bursts','rd_words','wide'])+'\r\n')
    send(f'F4 seq={s} elapsed_us=1000000 render10=701 logic10=600 present10=600 repeat10=0 scan10=600 valid=1 video_uf=0 audio_uf=0 drops=0\r\n')
    send(f'P4 seq={s} logic_us=1000 logic_max_us=1200 submit_us=900 wait_us=12000 work_us=13900 work_max_us=15000 over=0 busy=90 phase1_us=2200 bands_us=11000 profile=1\r\n')
    send(f'C4 seq={s} core_hz=202083333 square_band_bound=12340 players={state["players"]} font_hit=500 font_miss=20 font_reset=0 label_hit=40 label_miss=5 font_pixels=89088 track={songs[state["music_index"]]} music_auto={state["music_auto"]} music_index={state["music_index"]} music_total=10 pressure_supported=1 player_shots=0 locator_us=0 locator_on=0 enemy_us=200 player_logic_us=0 hostile_logic_us=1000 hud_us=100 band_splits=0 band_overflow=0 w=1280 h=720 auto={state["auto"]} stats={state["stats"]} verbose={state["verbose"]} tone={state["tone"]} volume={state["volume"]} density=16 stress_auto=0 bank_sounds=33 mixer=1 audio_ctrl=51\r\n')
  else:was_connected=False;time.sleep(.03)
http=make_server(session,0)
(v/'pty_fixture.json').write_text(json.dumps({'port':port,'url':f'http://127.0.0.1:{http.server_port}','pid':os.getpid()}))
threading.Thread(target=board,daemon=True).start();print('PTY fixture ready',port,flush=True)
try:http.serve_forever(poll_interval=.1)
except KeyboardInterrupt:pass
finally:running=False;session.disconnect();http.server_close();os.close(master);os.close(slave)
