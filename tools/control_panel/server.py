#!/usr/bin/env python3
"""Local Ti60 console: actual serial I/O, normalized telemetry, durable session logs.

Bind only to loopback. Opening the UI never opens a serial device or sends data.
Install pyserial only if absent: python3 -m pip install --user pyserial
"""
from __future__ import annotations
from collections import deque
from datetime import datetime, timezone
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import argparse, csv, html, io, json, mimetypes, os, re, secrets, sys, threading, time, urllib.parse, webbrowser

from serial_ports import CONSOLE_VERSION, enumerate_ports
from r2_presets import PROFILES, REQUIRED_FIELDS, public_profiles, command_plan, mismatches

HERE=Path(getattr(sys, '_MEIPASS', Path(__file__).resolve().parent))
ROOT=Path(sys.executable).resolve().parent if getattr(sys,'frozen',False) else HERE.parents[1]
def matrix_key_help_html():
    """Read-only help from the exact firmware map, never a browser action configuration."""
    definition=ROOT/'par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/matrix_keys.def'
    if not definition.is_file():definition=HERE/'matrix_keys.def'
    if not definition.is_file():return '<p>未找到固件键位定义，无法显示键位表。</p>'
    keys=[]
    for line in definition.read_text(encoding='utf-8-sig').splitlines():
        if line.startswith('KEY('):
            fields=next(csv.reader([line[4:-1]],skipinitialspace=True))
            if len(fields)!=8:raise ValueError('固件键位定义格式错误')
            keys.append(fields)
    if [int(k[0]) for k in keys]!=list(range(1,17)):raise ValueError('固件键位定义不完整')
    unsupported={12,15}  # Fn+S12 and Fn+S15 are reserved in this firmware.
    cards=[]
    for k in keys:
        number=int(k[0]);fn=k[7]
        if number in unsupported:fn+=' · 当前固件未实现'
        cards.append('<div class="matrix-key-card"><span class="matrix-key-number">S'+html.escape(k[0])+
                     '</span><div class="matrix-key-copy"><strong>'+html.escape(k[6])+'</strong><small>'+
                     ('Fn 组合：'+html.escape(fn) if k[7]!='—' else 'Fn 组合：—')+'</small></div></div>')
    return ('<section class="matrix-help" aria-labelledby="matrixHelpTitle"><h3 id="matrixHelpTitle">16键矩阵演示</h3>'
            '<p class="matrix-lead">展示16键矩阵的按键布局及主要功能，可用于查看每个按键的功能定义与Fn组合功能。</p>'
            '<div class="matrix-key-grid">'+''.join(cards)+'</div>'
            '<div class="matrix-tips"><strong>使用说明与提示</strong><ul>'
            '<li>S12仅在萝卜标题／暂停状态下选择下一局人数；S3开始新局／确认；S13继续当前局。</li>'
            '<li>S14请求重开；S3确认；S1返回菜单并取消。</li>'
            '<li>萝卜当前自动射击，S8释放当前角色技能，S16冲刺。Fn＋S12选择P2来源、Fn＋S15下一曲当前未实现。</li>'
            '<li>矩阵校准只调整物理顺序，不改变功能表。本表适用于重新构建并下载的ELF。</li>'
            '</ul></div></section>')

def default_log_root():
    # A packaged app may live in a read-only folder. Never use its current directory.
    if getattr(sys,'frozen',False):
        base=Path(os.environ.get('LOCALAPPDATA',Path.home()/'.local/share'))
        return base/'Ti60R5'/'logs'
    return ROOT/'logs/r5'

COMMANDS={'campaign','status','scene','start','pause','restart','menu','stress','skill','pick','move','stop','step','delta','count','bullet','square','period','density','auto','vsync','alpha','backend','motion','table','bgm','next','volume','tone','stats','verbose','bench','key','resolution','fast','bars','volume_delta','prev','music','video_confirm','video_recover','video_dvi','coop','input_diag','matrix_cal','matrix_bind','keyboard_sync','players','party_start','pick_for','lang'}
NO_ARGUMENT={'status','start','pause','restart','menu','skill','stop','next','prev','bench','fast','video_confirm','video_recover','keyboard_sync','party_start'}
FIELDS=['p2_parry_cd','p2_reflected','p3_shield','p3_shield_cd','p3_blocked','campaign_endless','endless_wave','ultimate_spawned','campaign_scene','campaign_remaining','campaign_complete','campaign_timeout','crystal_type','timestamp','boot','seq','epoch','scene','phase','count','target','step','bullet','square','alpha','vsync','hw','motion','table','bgm','track','resolution','output_w','output_h','output_hz','render_w','render_h','scale','pending','elapsed_us','render_fps','logic_hz','present_fps','repeat_fps','scan_hz','valid','video_uf','audio_uf','logic_us','logic_max_us','submit_us','wait_us','work_us','work_max_us','over','busy','phase1_us','bands_us','profile','font_hit','font_miss','font_reset','label_hit','label_miss','drops','submit_mode','band','cache','overlap','overlap_active','seed_table','seed_build','sort','kpath','rd_bursts','wr_bursts','rd_words','wide','video_rev','bars','tone','volume','bank_sounds','mixer','audio_ctrl','auto','density','stress_auto','stats','verbose','music_auto','music_index','music_total','r2_preset','r2_stage','r2_conditions_match','r2_initial_target','pressure_supported','motion_min_px','player_shots','locator_us','locator_on','enemy_us','player_logic_us','hostile_logic_us','hud_us','band_splits','band_overflow','matrix_auto','core_hz','square_band_bound','players','video_trial','trial_ms','p1_life','p1_skill','p1_score','p1_x','p1_y','p1_held','p2_life','p2_skill','p2_score','p2_x','p2_y','p2_held','p3_life','p3_skill','p3_score','p3_x','p3_y','p3_held','link_supported','link_stage','link_status','link_dvi','link_fault','recoveries','input_supported','matrix_keys','matrix_held','keyboard_held','keyboard_reports','malformed','rollover','ghosts','uart_error','input_diag_supported','input_diag','matrix_events','matrix_last','matrix_seen','matrix_logical','matrix_cal','matrix_step','matrix_cal_error','matrix_wait','matrix_mapped','matrix_map','matrix_candidate','keyboard_down','keyboard_bytes','keyboard_headers','keyboard_tail','keyboard_faults','keyboard_acks','keyboard_tx_timeouts','mouse_reports','status_reports']
def utc():return datetime.now(timezone.utc).isoformat(timespec='milliseconds')
def parse_values(line):
    result={}
    for key,value in re.findall(r'(\w+)=([^\s]+)',line):
        result[key]=int(value) if re.fullmatch(r'-?\d+',value) else value
    return result
def command_wire(request_id,name,value=None):
    if name not in COMMANDS:raise ValueError('未知功能命令')
    if name in NO_ARGUMENT:
        if value is not None:raise ValueError('此命令不需要参数')
        return f':{request_id} {name}\n'.encode('ascii')
    if not isinstance(value,int) or isinstance(value,bool) or not -2147483647<=value<=2147483647:raise ValueError('参数必须为整数')
    if name=='step' and value not in (1,10,100):raise ValueError('步长只能是 1、10 或 100')
    if name=='delta' and value not in (-1,1):raise ValueError('增减方向无效')
    if name=='move' and not 0<=value<=1023:raise ValueError('按键状态无效')
    if name=='volume_delta' and value not in (-1,1):raise ValueError('音量步进方向只能为 -1 或 1')
    if name=='music' and not -1<=value<=9:raise ValueError('曲目应为 0–9；-1 表示跟随场景')
    if name=='bars' and value not in (0,1):raise ValueError('彩条状态只能为 0 或 1')
    if name=='coop' and value not in (0,1):raise ValueError('三人入口只能选择东方0或萝卜1')
    if name=='video_dvi' and value not in (0,1):raise ValueError('DVI诊断只接受0或1')
    if name=='input_diag' and value not in (0,1):raise ValueError('输入检测只接受0或1')
    if name=='campaign' and value not in range(1,7):raise ValueError('关卡只能是 1 到 6')
    if name=='players' and value not in (1,2,3):raise ValueError('人数只能是 1、2 或 3')
    if name=='lang' and value not in (0,1):raise ValueError('界面语言只能是 0（英文）或 1（中文）')
    if name=='pick_for' and not (value//10 in (1,2,3) and value%10 in (1,2,3)):raise ValueError('pick_for 应为 11..33（十位玩家，个位选项）')
    if name=='matrix_cal' and value not in (0,1,2,3):raise ValueError('校准操作无效')
    if name=='matrix_bind' and not 0<=value<=255:raise ValueError('矩阵映射超出范围')
    if name=='resolution' and value not in (0,1):raise ValueError('输出模式只能为 0（720P60）或 1（1080P30）')
    return f':{request_id} {name} {value}\n'.encode('ascii')

class Collector:
    def __init__(self):
        self.state={};self.samples=deque(maxlen=600);self.lines=deque(maxlen=300);self.parts={};self.complete=set();self.acks=deque(maxlen=80);self.cursor=0;self.boot=0;self.compatible=False;self.dualvideo=False;self.responses={}
    def feed(self,line):
        self.cursor+=1;self.lines.append({'id':self.cursor,'time':utc(),'text':line})
        if re.search(r'=== (?:720P-|TI60-|Ti60-|R5-)',line):
            self.boot+=1;self.parts.clear();self.complete.clear();self.state={};self.compatible=False;self.dualvideo=False;self.responses.clear()
            self.state['firmware']=line.strip('= ')
        if line.startswith(('R4:','R5:')):self.compatible=True
        tag=line.split(' ',1)[0]
        if tag in ('S4','A4','V5','E5','I5','I9','G4') or (tag=='T7' and 'seq=' not in line):
            self.compatible=True;values=parse_values(line)
            if tag=='I5':values['input_supported']=values.pop('supported',0)
            if tag=='V5':
                self.dualvideo=values.get('supported')==1
                if 'nominal_hz' in values:values['output_hz']=values['nominal_hz']
            if tag=='S4' and 'count' in values:values['target']=values.pop('count')
            rid=values.get('id')
            if isinstance(rid,int) and rid>0:
                response=self.responses.setdefault(rid,{'tags':set(),'values':{}})
                response['tags'].add(tag);response['values'].update(values)
                if len(self.responses)>32:del self.responses[next(iter(self.responses))]
            self.state.update(values);return None
        if tag=='ACK':self.acks.append(dict(parse_values(line),time=utc()));return None
        if tag not in ('M4','F4','P4','C4','T7'):return None
        data=parse_values(line);seq=data.get('seq')
        if not isinstance(seq,int):return None
        self.compatible=True
        if seq in self.complete:return None
        self.parts.setdefault(seq,{})[tag]=data
        if len(self.parts)>12:del self.parts[next(iter(self.parts))]
        self.state.update({k:v for k,v in data.items() if k not in ('seq','epoch')})
        if not {'M4','F4','P4','C4'}.issubset(self.parts[seq]):return None
        row={'timestamp':utc(),'boot':self.boot}
        for key in ('resolution','output_w','output_h','output_hz','render_w','render_h','scale','pending','video_rev','bars','video_trial','trial_ms','link_supported','link_stage','link_status','link_dvi','link_fault','recoveries','input_supported','matrix_keys','matrix_held','keyboard_held','keyboard_reports','malformed','rollover','ghosts','uart_error','input_diag_supported','input_diag','matrix_events','matrix_last','matrix_seen','matrix_logical','matrix_cal','matrix_step','matrix_cal_error','matrix_wait','matrix_mapped','matrix_map','matrix_candidate','keyboard_down','keyboard_bytes','keyboard_headers','keyboard_tail','keyboard_faults','keyboard_acks','keyboard_tx_timeouts','mouse_reports','status_reports'):
            if key in self.state:row[key]=self.state[key]
        for part in ('T7','M4','F4','P4','C4'):row.update(self.parts[seq].get(part,{}))
        for raw,name in [('render10','render_fps'),('logic10','logic_hz'),('present10','present_fps'),('repeat10','repeat_fps'),('scan10','scan_hz')]:
            row[name]=row.get(raw,0)/10
        if not row.get('valid'):
            row['present_fps']=row['repeat_fps']=row['scan_hz']=None
        if row.get('profile')==0:
            row['busy']=row['phase1_us']=row['bands_us']=None
        self.samples.append(row);self.complete.add(seq);del self.parts[seq]
        if len(self.complete)>1000:self.complete={seq}
        return row

class Session:
    def __init__(self,log_root):
        self.lock=threading.RLock();self.serial=None;self.reader=None;self.running=False;self.collector=Collector();self.error='';self.port='';self.baud=115200;self.request_id=0;self.log_root=Path(log_root);self.record=None;self.files={};self.label='V3.1-CONSOLE';self.keyboard_until=0;self.keyboard_mask=0
        self.preset_changed=threading.Condition(self.lock);self.preset_generation=0
        self.preset={'status':'idle'};self.preset_thread=None
        self.preset_ack_timeout=5.0;self.preset_verify_timeout=10.0
        self.discovering=False;self.discovery=[];self.pad_sequences={};self.port_info=None;self.remembered=False
        self.preference_path=self.log_root/'last_console.json'
        try:self.last_identity=json.loads(self.preference_path.read_text(encoding='utf-8')).get('identity','')
        except (OSError,ValueError,AttributeError):self.last_identity=''

    def ports(self):
        try:ports=enumerate_ports()
        except ImportError:return []
        for p in ports:p['last_verified']=bool(self.last_identity and p.get('identity')==self.last_identity and p['role']!='ftdi_non_uart')
        return ports
    def _remember_console(self):
        if self.remembered or not self.collector.compatible:return
        self.remembered=True
        identity=(self.port_info or {}).get('identity','')
        if not identity:return
        self.last_identity=identity
        try:
            self.log_root.mkdir(parents=True,exist_ok=True)
            self.preference_path.write_text(json.dumps({'identity':identity,'port':self.port,'verified':utc()},ensure_ascii=False,indent=2),encoding='utf-8')
        except OSError:pass
    def auto_connect(self,baud=115200,label=None):
        # Explicit UI action. Read-only probes, never send to an unidentified port.
        if baud!=115200:raise ValueError('自动识别使用本工程默认 115200；其他波特率请手动连接')
        import serial
        with self.lock:
            if self.serial or self.discovering:raise ValueError('请先断开现有连接，或等待当前识别结束')
            self.discovering=True;self.discovery=[]
        winner=None
        try:
            ports=self.ports()
            candidates=[p for p in ports if p['role'] in ('ti60_uart_candidate','ftdi_unknown') or p.get('last_verified')]
            candidates.sort(key=lambda p:(not p.get('last_verified'),not p.get('recommended')))
            if not candidates:raise ValueError('没有可识别的 Ti60 FT4232H 日志候选；请刷新或手动选择串口')
            verified=[]
            for info in candidates:
                device=None;found=False;reason='未读到有效 Ti60 遥测'
                with self.lock:self.discovery.append({'port':info['device'],'status':'识别中'})
                try:
                    device=serial.Serial(port=None,baudrate=baud,timeout=.1,write_timeout=.5)
                    device.dtr=False;device.rts=False
                    if os.name=='posix':device.exclusive=True
                    device.port=info['device'];device.open()
                    buf=b'';tags=set();deadline=time.monotonic()+2.2
                    while time.monotonic()<deadline:
                        buf+=device.read(max(1,min(device.in_waiting,2048)))
                        while b'\n' in buf:
                            line,buf=buf.split(b'\n',1);line=line.decode('utf-8',errors='replace').strip()
                            m=re.match(r'^(M4|F4|P4|C4)\s+.*\bseq=\d+',line)
                            if m:tags.add(m[1])
                            if len(tags)>=2:found=True;break
                        if found:break
                        if len(buf)>16384:buf=buf[-4096:]
                    if found:verified.append(info);reason='已收到 Ti60 遥测'
                except (OSError,serial.SerialException) as e:reason='无法打开：'+str(e)
                finally:
                    if device is not None:device.close()
                    with self.lock:self.discovery[-1].update(status=reason,verified=found)
                if found and info.get('last_verified'):break
            if len(verified)==1:winner=verified[0]['device']
            elif len(verified)>1:raise ValueError('发现多块有效开发板，请按列表选择后手动连接')
            else:raise ValueError('未识别到运行中的 Ti60 固件；请确认 ELF 已运行、串口未被 IDE/终端占用')
        finally:
            with self.lock:self.discovering=False
        self.connect(winner,baud,label)
        return {'ok':True,'port':winner,'probes':list(self.discovery)}
    def pad(self,mask,sequence,client):
        if not isinstance(mask,int) or isinstance(mask,bool) or not 0<=mask<=1023:raise ValueError('按键状态无效')
        if not isinstance(sequence,int) or isinstance(sequence,bool) or not 0<=sequence<2**53:raise ValueError('按键序号无效')
        if not isinstance(client,str) or not re.fullmatch(r'[a-zA-Z0-9_-]{1,64}',client):raise ValueError('按键来源无效')
        with self.lock:
            if not self.serial or not self.collector.compatible:raise ValueError('请先连接并识别固件')
            if self.preset_busy() and mask:raise ValueError('压力预设正在执行')
            if self.collector.state.get('input_diag') and mask:raise ValueError('请先结束输入检测')
            if sequence<=self.pad_sequences.get(client,-1):return {'ok':True,'stale':True}
            if client not in self.pad_sequences and len(self.pad_sequences)>=16:self.pad_sequences.pop(next(iter(self.pad_sequences)))
            self.pad_sequences[client]=sequence
            low=mask&127;high=(mask>>7)&127
            self.serial.write(bytes((0xA5,low,high,(low^high^0x5A)&127)))
            if mask!=self.keyboard_mask:self.event('PAD',mask=mask,sequence=sequence,client=client)
            self.keyboard_mask=mask;self.keyboard_until=time.monotonic()+.35
            return {'ok':True,'mask':mask}

    def event(self,action,**fields):
        if self.files:
            self.files['events'].write(json.dumps(dict(time=utc(),action=action,**fields),ensure_ascii=False)+'\n');self.files['events'].flush()
    def _finish_record(self):
        if not self.files:return
        self.event('RECORD_END')
        for f in self.files.values():f.flush();f.close()
        self.files={}
    def new_record(self,label=None):
        with self.lock:
            if not self.serial:raise ValueError('请先连接串口')
            if self.preset_busy():raise ValueError('预设正在执行，请稍候新建记录')
            self._finish_record();self.label=(label or self.label)[:40]
            slug=re.sub(r'[^\w-]','_',self.label) or 'R5'
            now=datetime.now();folder=self.log_root/now.strftime('%Y%m%d');folder.mkdir(parents=True,exist_ok=True)
            stem=folder/(now.strftime('uart_%H%M%S_%f_')+slug)
            self.record={'raw':str(stem.with_suffix('.txt')),'events':str(stem.with_suffix('.events.jsonl')),'metrics':str(stem.with_suffix('.csv'))}
            self.files={'raw':open(self.record['raw'],'ab'),'events':open(self.record['events'],'a',encoding='utf-8'),'metrics':open(self.record['metrics'],'a',encoding='utf-8-sig',newline='')}
            self.writer=csv.DictWriter(self.files['metrics'],FIELDS,extrasaction='ignore');self.writer.writeheader();self.files['metrics'].flush()
            (self.log_root/'latest.json').write_text(json.dumps(dict(started=utc(),port=self.port,baud=self.baud,label=self.label,files=self.record),ensure_ascii=False,indent=2),encoding='utf-8')
            self.event('RECORD_START',port=self.port,baud=self.baud,label=self.label)
            return self.record
    def connect(self,port,baud=115200,label=None):
        if not isinstance(port,str) or not port or len(port)>256:raise ValueError('请选择或填写串口')
        # Re-enumerate before opening a known FTDI channel: USB numbers may
        # have changed since refresh. Never probe JTAG by sending serial data.
        info=next((p for p in self.ports() if p['device']==port),None)
        if info and info.get('role')=='ftdi_non_uart':
            raise ValueError(info['label']+'。请选择 FT4232H 通道 C 的日志口。')
        if baud not in (38400,57600,115200,300000):raise ValueError('不支持的波特率')
        try:import serial
        except ImportError:raise ValueError('缺少 pyserial：请使用 Start_Control_Panel.cmd / .sh，或用当前 Python 安装 requirements.txt')
        with self.lock:
            if self.serial or self.discovering:raise ValueError('请先断开当前串口或等待识别结束')
            options=dict(port=port,baudrate=baud,timeout=.1,write_timeout=.5)
            import os
            if os.name=='posix':options['exclusive']=True
            try:
                options['port']=None
                device=serial.Serial(**options);device.dtr=False;device.rts=False
                device.port=port;device.open()
            except Exception as e:raise ValueError('无法打开串口：'+str(e))
            self.preset_generation+=1;self.preset={'status':'idle'}
            self.serial=device;self.port=port;self.baud=baud;self.error='';self.collector=Collector();self.running=True;self.keyboard_until=0;self.keyboard_mask=0;self.pad_sequences={};self.port_info=info;self.remembered=False
            try:self.new_record(label)
            except Exception:
                device.close();self.serial=None;self.running=False;raise
            self.reader=threading.Thread(target=self._read,args=(device,),daemon=True);self.reader.start()
    def _read(self,device):
        buffer=b''
        try:
            while self.running and self.serial is device:
                chunk=device.read(max(1,min(device.in_waiting,4096)))
                with self.lock:
                    if self.keyboard_mask and time.monotonic()>self.keyboard_until:
                        self._command('move',0);self.keyboard_mask=0
                    if not chunk:continue
                    if self.files:self.files['raw'].write(chunk);self.files['raw'].flush()
                    buffer+=chunk
                    while b'\n' in buffer:
                        line,buffer=buffer.split(b'\n',1);text=line.decode('utf-8',errors='replace').rstrip('\r')
                        self._ingest(text);self._remember_console()
                    if len(buffer)>16384:
                        self.collector.feed('[超长串口行已截断] '+buffer[:200].decode(errors='replace'));buffer=b''
        except Exception as e:
            with self.lock:
                if self.running:self.error='串口已断开：'+str(e);self.event('ERROR',detail=self.error)
        finally:
            with self.lock:
                if self.serial is device:
                    self._cancel_preset('串口连接中断');self.running=False;self.serial=None;self.keyboard_mask=0;self._finish_record()
                    try:device.close()
                    except Exception:pass
    def _command(self,name,value=None):
        if not self.serial:raise ValueError('串口尚未连接')
        if not self.collector.compatible:raise ValueError('尚未识别到 R4 / R5 控制协议。请等待遥测或点击“读取帮助”；旧固件只记录日志，不发送扩展命令。')
        if name=='campaign' and not self.collector.state.get('campaign_supported'):raise ValueError('六关跳转需要 V4 ELF')
        if name=='resolution' and not self.collector.dualvideo:raise ValueError('分辨率切换需要 R5 固件与配套 bit；旧 R4 仅支持原有功能')
        if name in ('fast','bars') and not self.collector.state.get('accel_supported'):raise ValueError('此功能需要 R5.2 固件；请先下载配套 ELF')
        if name=='volume_delta' and not self.collector.state.get('volume_step_supported'):raise ValueError('音量增减按钮需要包含 volume_step_supported 的新固件')
        if name in ('prev','music') and not self.collector.state.get('music_supported'):raise ValueError('全曲目选择需要 R5.3 ELF')
        if name in ('input_diag','matrix_cal','matrix_bind','keyboard_sync') and not self.collector.state.get('input_diag_supported'):
            raise ValueError('输入校准与字节诊断需要R5.9 ELF；当前bit可继续使用R5.8')
        self.request_id=(self.request_id+1)%1000000000
        wire=command_wire(self.request_id,name,value)
        self.serial.write(wire);self.event('TX',id=self.request_id,command=name,value=value,wire=wire.decode().strip())
        return {'id':self.request_id,'sent':True}
    def command(self,name,value=None):
        with self.lock:
            if self.preset_busy() and not (name in ('status','stop') or (name=='move' and value==0)):
                raise ValueError('R2 预设正在执行，请等待条件确认或先断开取消')
            result=self._command(name,value)
            if name=='move':self.keyboard_mask=value;self.keyboard_until=time.monotonic()+.35
            if name=='stop':self.keyboard_mask=0
            return result
    def help(self):
        with self.lock:
            if not self.serial:raise ValueError('串口尚未连接')
            if self.preset_busy():raise ValueError('预设正在执行，请稍候读取帮助')
            self.serial.write(b'h');self.event('TX_LEGACY',wire='h')
    def disconnect(self):
        with self.lock:
            device=self.serial
            if device:
                if self.collector.compatible:
                    try:self._command('move',0)
                    except Exception:pass
                self._cancel_preset('用户断开连接');self.event('DISCONNECT');self.running=False;self.serial=None;self.keyboard_mask=0
                try:device.close()
                except Exception:pass
                self._finish_record()
        if self.reader and self.reader is not threading.current_thread():self.reader.join(timeout=1)
    def snapshot(self,after=0):
        with self.lock:
            return {'console_version':CONSOLE_VERSION,'pad4':True,'discovering':self.discovering,'discovery':list(self.discovery),'boot':self.collector.boot,'responses':{rid:{'tags':sorted(response['tags']),'values':dict(response['values'])} for rid,response in self.collector.responses.items()},'preset':self.preset_snapshot(),'presets_supported':True,'preset_unavailable':self.preset_unavailable(),'connected':self.serial is not None,'compatible':self.collector.compatible,'dualvideo':self.collector.dualvideo,'port':self.port,'baud':self.baud,'error':self.error,'record':self.record,'label':self.label,'cursor':self.collector.cursor,'state':dict(self.collector.state),'lines':[x for x in self.collector.lines if x['id']>after],'samples':list(self.collector.samples)[-180:],'acks':list(self.collector.acks)[-20:]}
    def preset_busy(self):
        return self.preset.get('status') in ('applying','verifying')
    def preset_snapshot(self):
        result=dict(self.preset)
        if result.get('status')=='applied':
            profile=PROFILES[result['id']]
            result['mismatches']=mismatches(profile,self.collector.state)
            result['conditions_match']=not result['mismatches']
            result['current_target']=self.collector.state.get('target')
        return result
    def _cancel_preset(self,reason):
        if self.preset_busy():
            self.preset.update(status='cancelled',error=reason)
            self.event('PRESET_CANCEL',preset=self.preset.get('id'),reason=reason)
        self.preset_generation+=1;self.preset_changed.notify_all()
    def _ingest(self,line):
        # Called under the session lock by the serial reader (also test fixtures).
        previous_boot=self.collector.boot
        row=self.collector.feed(line)
        if self.collector.boot!=previous_boot and self.preset.get('status')=='applied':
            self.preset.update(status='cancelled',error='固件已重启，旧的条件确认已失效，请重新选择预设')
            self.event('PRESET_INVALIDATED',preset=self.preset.get('id'),reason='firmware_reboot')
        if row:
            p=self.preset;profile=PROFILES.get(p.get('id'))
            row.update(r2_preset=p.get('id',''),r2_stage=p.get('status','idle'),
                       r2_initial_target=profile['count'] if profile else '',
                       r2_conditions_match=int(p.get('status')=='applied' and not mismatches(profile,row)) if profile else '')
            if self.files:self.writer.writerow(row);self.files['metrics'].flush()
        self.preset_changed.notify_all()
        return row
    def _preset_context(self,generation,device,collector,boot):
        if generation!=self.preset_generation or self.serial is not device or self.collector is not collector:
            raise ValueError('连接或任务已改变，预设已停止')
        if collector.boot!=boot:raise ValueError('检测到固件重启，预设已停止')
    def _preset_command(self,name,value,generation,device,collector,boot):
        with self.preset_changed:
            self._preset_context(generation,device,collector,boot)
            rid=self._command(name,value)['id'];deadline=time.monotonic()+self.preset_ack_timeout
            while True:
                self._preset_context(generation,device,collector,boot)
                ack=next((a for a in collector.acks if a.get('id')==rid),None)
                if ack:
                    if ack.get('ok')!=1:raise ValueError(f'设备拒绝 {name} 指令：{ack.get("code","unknown")}')
                    return rid
                remaining=deadline-time.monotonic()
                if remaining<=0:raise ValueError(f'{name} 指令未收到回执；后续设置已停止')
                self.preset_changed.wait(min(remaining,.2))
    def preset_unavailable(self):
        if not self.serial or not self.collector.compatible:return '连接设备并等待读取固件状态后可使用。'
        latest=self.collector.samples[-1] if self.collector.samples else {}
        # 2026-10-26（本工程固件）：没有 1080P（V5 supported=0），预设不再要求双分辨率
        if not self.collector.state.get('accel_supported'):
            return '需要具有加速恢复功能的固件。'
        if latest.get('boot')!=self.collector.boot or latest.get('profile')!=1 or any(k not in latest for k in REQUIRED_FIELDS):
            return '请使用 R5.3 调试 ELF，并等待完整测量数据；发行版不用于 R2 统计条件对照。'
        return ''
    def start_preset(self,key):
        if not isinstance(key,str) or key not in PROFILES:raise ValueError('未知 R2 预设')
        with self.lock:
            if self.preset_busy():raise ValueError('已有预设正在执行，请等待完成')
            unavailable=self.preset_unavailable()
            if unavailable:raise ValueError(unavailable)
            self.preset_generation+=1;generation=self.preset_generation
            profile=PROFILES[key];plan=command_plan(profile);device=self.serial;collector=self.collector
            self.keyboard_mask=0;self.keyboard_until=0
            self.preset=dict(operation=generation,id=key,label=profile['label'],group=profile['group'],status='applying',
                             step=0,total=len(plan),command='',started=utc(),initial_target=profile['count'])
            self.event('PRESET_BEGIN',preset=key,expected=profile['expected'],target=profile['count'],steps=plan)
            self.preset_thread=threading.Thread(target=self._run_preset,
                args=(profile,plan,generation,device,collector,collector.boot),daemon=True)
            self.preset_thread.start()
            return dict(accepted=True,preset=key,operation=generation)
    def _run_preset(self,profile,plan,generation,device,collector,boot):
        try:
            last_id=0
            for index,(name,value) in enumerate(plan):
                with self.lock:
                    self._preset_context(generation,device,collector,boot)
                    self.preset.update(step=index+1,command=name)
                last_id=self._preset_command(name,value,generation,device,collector,boot)
            with self.preset_changed:
                self._preset_context(generation,device,collector,boot)
                self.preset.update(status='verifying',command='核对状态与新测量窗口')
                deadline=time.monotonic()+self.preset_verify_timeout
                # A successful ACK alone is insufficient. Read this request's
                # complete status response, then require a subsequent full row.
                while True:
                    self._preset_context(generation,device,collector,boot)
                    response=collector.responses.get(last_id,{})
                    if response.get('tags',set())>={'S4','A4','V5','E5'}:break
                    remaining=deadline-time.monotonic()
                    if remaining<=0:raise ValueError('状态回报不完整，不能确认预设成功')
                    self.preset_changed.wait(min(remaining,.2))
                min_epoch=response['values'].get('epoch')
                if not isinstance(min_epoch,int):raise ValueError('设备状态缺少有效的测量 epoch，不能确认预设')
                floor=collector.samples[-1]['seq'] if collector.samples else -1
                problems=[]
                while True:
                    self._preset_context(generation,device,collector,boot)
                    row=collector.samples[-1] if collector.samples else None
                    if row and row.get('seq',-1)>floor and isinstance(row.get('epoch'),int) and row['epoch']>=min_epoch:
                        problems=mismatches(profile,row,include_target=True)
                        if not problems:
                            self.preset.update(status='applied',confirmed_seq=row['seq'],confirmed_epoch=row['epoch'],finished=utc())
                            self.event('PRESET_CONFIRMED',preset=profile['id'],seq=row['seq'],epoch=row['epoch'])
                            # This row confirmed settings but was recorded during
                            # verification; subsequent rows carry match=1.
                            return
                    remaining=deadline-time.monotonic()
                    if remaining<=0:
                        detail='；'.join(f'{x["label"]}={x["actual"]}，应为{x["expected"]}' for x in problems[:5])
                        raise ValueError('未确认新的完整测量窗口'+('：'+detail if detail else '，请检查固件遥测'))
                    self.preset_changed.wait(min(remaining,.2))
        except Exception as exc:
            # An asynchronous job must always leave a visible terminal state.
            with self.lock:
                if generation==self.preset_generation:
                    self.preset.update(status='failed',error=str(exc),finished=utc())
                    self.event('PRESET_FAILED',preset=profile['id'],step=self.preset.get('step'),reason=str(exc))

    def export(self,kind):
        with self.lock:
            if not self.record or kind not in self.record:raise ValueError('还没有可以导出的记录')
            if self.files:
                for f in self.files.values():f.flush()
            p=Path(self.record[kind]);return p.name,p.read_bytes()

def make_server(session,port=8765):
    token=secrets.token_urlsafe(32)
    class Handler(BaseHTTPRequestHandler):
        def log_message(self,*args):pass
        def reply(self,status,data,ctype='application/json; charset=utf-8',filename=None):
            if isinstance(data,(dict,list)):data=json.dumps(data,ensure_ascii=False).encode()
            elif isinstance(data,str):data=data.encode()
            self.send_response(status);self.send_header('Content-Type',ctype);self.send_header('Content-Length',str(len(data)));self.send_header('Cache-Control','no-store');self.send_header('X-Content-Type-Options','nosniff')
            if filename:self.send_header('Content-Disposition',"attachment; filename*=UTF-8''"+urllib.parse.quote(filename))
            self.end_headers();self.wfile.write(data)
        def trusted(self):
            host=self.headers.get('Host','');allowed={f'127.0.0.1:{self.server.server_port}',f'localhost:{self.server.server_port}'}
            origin=self.headers.get('Origin');return host in allowed and (not origin or origin in {'http://'+x for x in allowed})
        def do_GET(self):
            if not self.trusted():return self.reply(403,{'error':'仅允许本机访问'})
            parts=urllib.parse.urlsplit(self.path);query=urllib.parse.parse_qs(parts.query)
            try:
                if parts.path=='/api/state':return self.reply(200,dict(session.snapshot(int(query.get('after',['0'])[0])),token=token))
                if parts.path=='/api/presets':return self.reply(200,{'profiles':public_profiles()})
                if parts.path=='/api/ports':return self.reply(200,session.ports())
                if parts.path=='/api/export':
                    name,data=session.export(query.get('kind',['metrics'])[0]);return self.reply(200,data,'application/octet-stream',name)
                names={'/':'index.html','/app.js':'app.js','/style.css':'style.css','/wiring':'wiring.html','/keys':'keys.html','/ui-font.woff2':'fonts/ui-v6.woff2'}
                if parts.path not in names:return self.reply(404,{'error':'不存在'})
                p=HERE/names[parts.path]
                data=p.read_bytes()
                if p.suffix=='.html':data=data.replace(b'<!-- MATRIX_KEY_HELP -->',matrix_key_help_html().encode('utf-8'))
                return self.reply(200,data,('font/woff2' if p.suffix=='.woff2' else mimetypes.guess_type(p.name)[0] or 'text/plain'))
            except (ValueError,OSError) as e:return self.reply(400,{'error':str(e)})
        def do_POST(self):
            if not self.trusted() or self.headers.get('X-Ti60-Token')!=token:return self.reply(403,{'error':'请刷新本地控制页面'})
            try:
                n=int(self.headers.get('Content-Length','0'))
                if n<0 or n>4096:raise ValueError('请求过大')
                d=json.loads(self.rfile.read(n) or b'{}');path=urllib.parse.urlsplit(self.path).path
                if not isinstance(d,dict):raise ValueError('请求格式错误')
                if path=='/api/connect':session.connect(d.get('port'),d.get('baud',115200),d.get('label'));result={'ok':True}
                elif path=='/api/disconnect':session.disconnect();result={'ok':True}
                elif path=='/api/auto-connect':result=session.auto_connect(d.get('baud',115200),d.get('label'))
                elif path=='/api/pad':result=session.pad(d.get('mask'),d.get('seq'),d.get('client'))
                elif path=='/api/preset':result=session.start_preset(d.get('id'))
                elif path=='/api/command':result=session.command(d.get('command'),d.get('value'))
                elif path=='/api/help':session.help();result={'ok':True}
                elif path=='/api/record':result=session.new_record(d.get('label'))
                elif path=='/api/mark':
                    with session.lock:session.event('MARK',text=str(d.get('text',''))[:500]);result={'ok':True}
                else:return self.reply(404,{'error':'不存在'})
                self.reply(200,result)
            except (ValueError,TypeError,OSError) as e:self.reply(400,{'error':str(e)})
    return ThreadingHTTPServer(('127.0.0.1',port),Handler)

def main():
    # Frozen Python ignores PYTHONUTF8. A Western Windows code page must not
    # crash the console when printing Chinese UI text or a Unicode log path.
    for stream in (sys.stdout,sys.stderr):
        if stream is not None and hasattr(stream,'reconfigure'):
            stream.reconfigure(encoding='utf-8',errors='backslashreplace')
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--port',type=int,default=0);ap.add_argument('--log-root',type=Path,default=default_log_root());ap.add_argument('--open',action='store_true');args=ap.parse_args()
    if not 0<=args.port<=65535:ap.error('--port 必须为 0–65535，0 表示自动选择空闲端口')
    session=Session(args.log_root.expanduser().resolve())
    try:server=make_server(session,args.port)
    except OSError as e:ap.error('启动失败（端口可能已占用，可用 --port 更换）：'+str(e))
    url=f'http://127.0.0.1:{server.server_port}';print('Ti60 实验台：'+url,flush=True);print('选择串口并点击连接后才会打开设备。日志：'+str(args.log_root),flush=True)
    if args.open:webbrowser.open(url)
    try:server.serve_forever(poll_interval=.2)
    except KeyboardInterrupt:pass
    finally:session.disconnect();server.server_close()
if __name__=='__main__':main()
