#!/usr/bin/env python3
"""Audit and package a rebuilt R5 delivery; never programs hardware."""
from pathlib import Path
import argparse,datetime,hashlib,json,re,shutil,zipfile
r=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--manifest-only',action='store_true');a=p.parse_args()
b=r/'BOARD_TEST';out=r/'PROJECT/par/ddr_demo_ti60/outflow';v=r/'verification/r5'
(b/'reports').mkdir(exist_ok=True)
for name in ['ddr_demo_ti60.timing.rpt','ddr_demo_ti60.hier_util.rpt','ddr_demo_ti60.route.rpt']:
 shutil.copy2(out/name,b/'reports'/name)
for name in ['blit_hdmi_validation.json','checks_control_main.json','build_debug.log','build_release.log','input_devices.log','input_hub.log','hdmi_verilator.log']:
 if (v/name).is_file():shutil.copy2(v/name,b/'reports'/name)

def digest(q):return hashlib.sha256(q.read_bytes()).hexdigest()
def record(q):return {'path':q.relative_to(r).as_posix(),'bytes':q.stat().st_size,'sha256':digest(q)}
t=(out/'ddr_demo_ti60.timing.rpt').read_text();clock={};section=None
for line in t.splitlines():
 if line.startswith('Setup (Max) Clock Relationship'):section='setup_ns'
 elif line.startswith('Hold (Min) Clock Relationship'):section='hold_ns'
 elif line.startswith('NOTE:'):section=None
 if section:
  m=re.match(r'^(\S+)\s+(\S+)\s+([-\d.]+)\s+([-\d.]+)\s+\(R-R\)',line)
  if m:clock.setdefault(m[1],{})[section]=float(m[4])
assert len(clock)==7 and all(d.get('setup_ns',-1)>=0 and d.get('hold_ns',-1)>=0 for d in clock.values()),'Post-route timing must pass'
util=(out/'ddr_demo_ti60.hier_util.rpt').read_text()
m=re.search(r'\| \+ddr3_example_top\s*\|\s*(\d+)\([^)]*\)\s*\|\s*(\d+)\([^)]*\)\s*\|\s*(\d+)',util);assert m
resources={'xlr':{'used':int(m[1]),'capacity':60800},'ram_blocks':{'used':int(m[2]),'capacity':256},'dsp':{'used':int(m[3]),'capacity':160}}
assert all(x['used']<=x['capacity'] for x in resources.values())
assert digest(out/'ddr_demo_ti60.bit')==digest(b/'ti60_r5.bit'),'Delivery bit differs from final output'
checks={q.name:json.loads(q.read_text()) for q in v.glob('checks_*.json')}
assert checks and all(c['passed'] for cs in checks.values() for c in cs),'Firmware checks failed'
win=json.loads((v/'windows_console/windows_ci_validation.json').read_text());assert win['exe_http_smoke']
rtl=json.loads((v/'blit_hdmi_validation.json').read_text())
source=[]
source.extend((r/'PROJECT/rtl').rglob('*.v'));source.extend((r/'PROJECT/rtl').rglob('*.vh'))
source.extend((r/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src').glob('*'))
source.extend([r/'PROJECT/par/ddr_demo_ti60/ddr_demo_ti60.xml',r/'PROJECT/par/ddr_demo_ti60/ddr_demo_ti60.peri.xml',r/'PROJECT/par/ddr_demo_ti60/sdc/ddr3.sdc',r/'PROJECT/par/ddr_demo_ti60/ip/soc/soc.v'])
source.extend(q for q in (r/'tools/control_panel').glob('*') if q.is_file())
artifacts={q.name:record(q) for q in [b/'ti60_r5.bit',b/'ti60_r5_debug.elf',b/'ti60_r5_release.elf',b/'ti60_r5_debug.launch',r/'dist/Ti60_R5_Windows_Console.zip',r/'dist/windows/Ti60Control.exe']}
mem={}
for kind in ['debug','release']:
 mem[kind]={k:int(n) for k,n in re.findall(r'(ram|snd|art):\s+(\d+) B',(v/('build_'+kind+'.log')).read_text())}
reports={q.name:record(q) for q in [b/'reports/ddr_demo_ti60.timing.rpt',b/'reports/ddr_demo_ti60.hier_util.rpt',out/'ddr_demo_ti60.map.v',v/'blit_hdmi_validation.json',v/'checks_control_main.json',v/'windows_console/windows_ci_validation.json']}
data={
 'version':'TI60-R5-DUALVIDEO-20261024','created_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
 'project_root':'/home/user/fpga_contest/1025/Ti60_R5_DUALVIDEO','repository':'https://github.com/18971355314bjl-gif/ti60-fpga-game','repository_visibility':'private',
 'validation_status':'target_build_and_offline_checks_passed_board_pending','board_downloaded':False,'board_fps_measured':False,
 'hardware':{'device':'Ti60F225','timing_model':'I3','tool':'Efinity 2026.1.132.4.5','core_hz':200000000,'pixel_hz':74375000,'resources':resources,'post_route_slack':clock,'all_reported_setup_hold_passed':True,
  'blitter_ram_baseline':85,'blitter_ram_r5':81,'new_scaler_ram':3,'new_uart_ram':1,'alpha_pixels_per_cycle':2},
 'video_modes':[{'id':0,'hdmi':'1280x720p','nominal_hz':60,'actual_hz':74375000/(1650*750),'vic':4},{'id':1,'hdmi':'1920x1080p','nominal_hz':30,'actual_hz':74375000/(2200*1125),'vic':34}],
 'render_canvas':[1280,720],'scaling_1080':'3:2 nearest neighbour; not native 1080 rendering','logic_target_hz':60,
 'capacity_limits':{'carrot_bullets':40000,'touhou_bullets':16384,'squares':30000,'meaning':'allocation caps, not measured stable FPS limits'},
 'matrix':{'rows_j7':[2,4,8,10],'columns_j7':[20,22,26,28],'bank':'1B','vccio':1.8,'requires_adapter_same_number_continuity_check':True,'keypad_header_order_must_be_measured':True},
 'ch9350':{'mode':'downstream USB host, state 2','baud':115200,'uart_logic_v':3.3,'tx_to_fpga_rx_j7':32,'rx_from_fpga_tx_j7':34,'module_supply_v':5,'board_tested':False},
 'windows':{'ci_passed':True,'ci_url':win['ci_run'],'platform':win['platform'],'physical_usb_tested':False},
 'memory_regions_bytes':mem,'artifacts':artifacts,'reports':reports,
 'source_sha256':{q.relative_to(r).as_posix():digest(q) for q in sorted(set(source)) if q.is_file()},
 'known_limits':['Nondefault LIMIT=2/2/128 with long DDR latency reproduces inherited wide-band deadlock in both R4 and R5; shipped defaults pass.','Diode-less matrix can ghost; ambiguous rectangles are rejected until release.','CH9350 state-2 silent unplug cannot be inferred from idle time; verify real-module disconnect behavior.','No R5 on-board FPS, HDMI sink, sound or electrical measurements have been claimed.']}
(b/'manifest.json').write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n')
(v/'release_validation.json').write_text(json.dumps({'version':data['version'],'target_build':True,'timing':clock,'resources':resources,'firmware_checks':checks,'windows_ci':win['ci_run'],'hardware_tested':False,'manifest_sha256':digest(b/'manifest.json')},ensure_ascii=False,indent=2)+'\n')
if not a.manifest_only:
 zip_path=r/'dist/Ti60_R5_BOARD_TEST.zip'
 with zipfile.ZipFile(zip_path,'w',zipfile.ZIP_DEFLATED,compresslevel=6) as z:
  for q in sorted(b.rglob('*')):
   if q.is_file() and 'legacy_r4' not in q.relative_to(b).parts:z.write(q,'BOARD_TEST/'+q.relative_to(b).as_posix())
  for name in ['BOARD_TEST_PLAN.csv','wiring_demo.png','WIRING_DEMO.html','INPUT_GUIDE.html']:
   q=r/'docs'/name;z.write(q,'docs/'+name)
 (r/'dist/release_checksums.json').write_text(json.dumps({zip_path.name:record(zip_path),'Ti60_R5_Windows_Console.zip':record(r/'dist/Ti60_R5_Windows_Console.zip')},indent=2)+'\n')
 print('Board package:',zip_path,zip_path.stat().st_size,'bytes',digest(zip_path))
print('Verified:',len(artifacts),'artifacts,',len(clock),'clock domains;',resources)
