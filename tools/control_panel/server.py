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
import argparse, csv, io, json, mimetypes, os, re, secrets, sys, threading, time, urllib.parse, webbrowser

from serial_ports import CONSOLE_VERSION, enumerate_ports

HERE=Path(getattr(sys, '_MEIPASS', Path(__file__).resolve().parent))
ROOT=Path(sys.executable).resolve().parent if getattr(sys,'frozen',False) else HERE.parents[1]
def default_log_root():
    # A packaged app may live in a read-only folder. Never use its current directory.
    if getattr(sys,'frozen',False):
        base=Path(os.environ.get('LOCALAPPDATA',Path.home()/'.local/share'))
        return base/'Ti60R5'/'logs'
    return ROOT/'logs/r5'

COMMANDS={'status','scene','start','pause','restart','menu','stress','skill','pick','move','stop','step','delta','count','bullet','square','period','density','auto','vsync','alpha','backend','motion','table','bgm','next','volume','tone','stats','verbose','bench','key','resolution'}
NO_ARGUMENT={'status','start','pause','restart','menu','skill','stop','next','bench'}
FIELDS=['timestamp','boot','seq','epoch','scene','phase','count','target','step','bullet','square','alpha','vsync','hw','motion','table','bgm','track','resolution','output_w','output_h','output_hz','render_w','render_h','scale','pending','elapsed_us','render_fps','logic_hz','present_fps','repeat_fps','scan_hz','valid','video_uf','audio_uf','logic_us','logic_max_us','submit_us','wait_us','work_us','work_max_us','over','busy','phase1_us','bands_us','profile','font_hit','font_miss','font_reset','label_hit','label_miss','drops']
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
    if name=='resolution' and value not in (0,1):raise ValueError('输出模式只能为 0（720P60）或 1（1080P30）')
    return f':{request_id} {name} {value}\n'.encode('ascii')

class Collector:
    def __init__(self):
        self.state={};self.samples=deque(maxlen=600);self.lines=deque(maxlen=300);self.parts={};self.complete=set();self.acks=deque(maxlen=80);self.cursor=0;self.boot=0;self.compatible=False;self.dualvideo=False
    def feed(self,line):
        self.cursor+=1;self.lines.append({'id':self.cursor,'time':utc(),'text':line})
        if re.search(r'=== (?:720P-|TI60-|Ti60-|R5-)',line):
            self.boot+=1;self.parts.clear();self.complete.clear();self.state={};self.compatible=False;self.dualvideo=False
            self.state['firmware']=line.strip('= ')
        if line.startswith(('R4:','R5:')):self.compatible=True
        tag=line.split(' ',1)[0]
        if tag in ('S4','A4','V5'):
            self.compatible=True;values=parse_values(line)
            if tag=='V5':
                self.dualvideo=values.get('supported')==1
                if 'nominal_hz' in values:values['output_hz']=values['nominal_hz']
            if tag=='S4' and 'count' in values:values['target']=values.pop('count')
            self.state.update(values);return None
        if tag=='ACK':self.acks.append(dict(parse_values(line),time=utc()));return None
        if tag not in ('M4','F4','P4','C4'):return None
        data=parse_values(line);seq=data.get('seq')
        if not isinstance(seq,int):return None
        self.compatible=True
        if seq in self.complete:return None
        self.parts.setdefault(seq,{})[tag]=data
        if len(self.parts)>12:del self.parts[next(iter(self.parts))]
        self.state.update({k:v for k,v in data.items() if k not in ('seq','epoch')})
        if set(self.parts[seq])!={'M4','F4','P4','C4'}:return None
        row={'timestamp':utc(),'boot':self.boot}
        for key in ('resolution','output_w','output_h','output_hz','render_w','render_h','scale','pending'):
            if key in self.state:row[key]=self.state[key]
        for part in ('M4','F4','P4','C4'):row.update(self.parts[seq][part])
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
        self.lock=threading.RLock();self.serial=None;self.reader=None;self.running=False;self.collector=Collector();self.error='';self.port='';self.baud=115200;self.request_id=0;self.log_root=Path(log_root);self.record=None;self.files={};self.label='R5-CONSOLE';self.keyboard_until=0;self.keyboard_mask=0
    @staticmethod
    def ports():
        try:return enumerate_ports()
        except ImportError:return []
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
            if self.serial:raise ValueError('请先断开当前串口')
            options=dict(port=port,baudrate=baud,timeout=.1,write_timeout=.5)
            import os
            if os.name=='posix':options['exclusive']=True
            try:device=serial.Serial(**options)
            except Exception as e:raise ValueError('无法打开串口：'+str(e))
            self.serial=device;self.port=port;self.baud=baud;self.error='';self.collector=Collector();self.running=True;self.keyboard_until=0;self.keyboard_mask=0
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
                        row=self.collector.feed(text)
                        if row and self.files:self.writer.writerow(row);self.files['metrics'].flush()
                    if len(buffer)>16384:
                        self.collector.feed('[超长串口行已截断] '+buffer[:200].decode(errors='replace'));buffer=b''
        except Exception as e:
            with self.lock:
                if self.running:self.error='串口已断开：'+str(e);self.event('ERROR',detail=self.error)
        finally:
            with self.lock:
                if self.serial is device:
                    self.running=False;self.serial=None;self.keyboard_mask=0;self._finish_record()
                    try:device.close()
                    except Exception:pass
    def _command(self,name,value=None):
        if not self.serial:raise ValueError('串口尚未连接')
        if not self.collector.compatible:raise ValueError('尚未识别到 R4 / R5 控制协议。请等待遥测或点击“读取帮助”；旧固件只记录日志，不发送扩展命令。')
        if name=='resolution' and not self.collector.dualvideo:raise ValueError('分辨率切换需要 R5 固件与配套 bit；旧 R4 仅支持原有功能')
        self.request_id=(self.request_id+1)%1000000000
        wire=command_wire(self.request_id,name,value)
        self.serial.write(wire);self.event('TX',id=self.request_id,command=name,value=value,wire=wire.decode().strip())
        return {'id':self.request_id,'sent':True}
    def command(self,name,value=None):
        with self.lock:
            result=self._command(name,value)
            if name=='move':self.keyboard_mask=value;self.keyboard_until=time.monotonic()+.35
            if name=='stop':self.keyboard_mask=0
            return result
    def help(self):
        with self.lock:
            if not self.serial:raise ValueError('串口尚未连接')
            self.serial.write(b'h');self.event('TX_LEGACY',wire='h')
    def disconnect(self):
        with self.lock:
            device=self.serial
            if device:
                if self.collector.compatible:
                    try:self._command('move',0)
                    except Exception:pass
                self.event('DISCONNECT');self.running=False;self.serial=None;self.keyboard_mask=0
                try:device.close()
                except Exception:pass
                self._finish_record()
        if self.reader and self.reader is not threading.current_thread():self.reader.join(timeout=1)
    def snapshot(self,after=0):
        with self.lock:
            return {'console_version':CONSOLE_VERSION,'connected':self.serial is not None,'compatible':self.collector.compatible,'dualvideo':self.collector.dualvideo,'port':self.port,'baud':self.baud,'error':self.error,'record':self.record,'label':self.label,'cursor':self.collector.cursor,'state':dict(self.collector.state),'lines':[x for x in self.collector.lines if x['id']>after],'samples':list(self.collector.samples)[-180:],'acks':list(self.collector.acks)[-20:]}
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
                if parts.path=='/api/ports':return self.reply(200,session.ports())
                if parts.path=='/api/export':
                    name,data=session.export(query.get('kind',['metrics'])[0]);return self.reply(200,data,'application/octet-stream',name)
                names={'/':'index.html','/app.js':'app.js','/style.css':'style.css'}
                if parts.path not in names:return self.reply(404,{'error':'不存在'})
                p=HERE/names[parts.path];return self.reply(200,p.read_bytes(),mimetypes.guess_type(p.name)[0] or 'text/plain')
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
