#!/usr/bin/env python3
"""Rebuild Ti60 R5 with Efinity 2026.1 on Linux or Windows; never programs a board."""
from pathlib import Path
import argparse,os,shutil,subprocess,sys,hashlib,json,re
root=Path(__file__).resolve().parent
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--efinity',type=Path,help='Efinity installation root (contains bin/, scripts/, pt/)')
p.add_argument('--flow',choices=['map','interface','pnr','pgm','compile'],default='compile')
a=p.parse_args()
candidates=[a.efinity] if a.efinity else []
if os.environ.get('EFINITY_HOME'):candidates.append(Path(os.environ['EFINITY_HOME']))
found=shutil.which('efx_run')
if found:candidates.append(Path(found).resolve().parents[1])
candidates.extend([Path('/home/user/fpga_contest/vivado/efinity-2026.1.132-linux-x64/efinity/2026.1'),Path('D:/Efinity/2026.1')])
ef=next((q.resolve() for q in candidates if (q/'scripts/efx_run.py').is_file()),None)
if not ef:p.error('Set EFINITY_HOME or pass --efinity /path/to/efinity/2026.1')
win=sys.platform=='win32';python=ef/('python311/bin/python.exe' if win else 'bin/python3')
if not python.is_file():p.error('Bundled Python not found: '+str(python))
env=dict(os.environ);env['EFINITY_HOME']=str(ef)
for name,rel in {'EFXPT_HOME':'pt','EFXPGM_HOME':'pgm','EFXDBG_HOME':'debugger','EFXIPM_HOME':'ipm'}.items():env[name]=str(ef/rel)
env['PYTHONHOME']=str(ef/'python311' if win else ef);env['PYTHONNOUSERSITE']='1'
env['QT_PLUGIN_PATH']=str(ef/'bin' if win else ef/'lib/plugins')
env['PYTHONPATH']=str(ef/'lib')
env['PATH']=os.pathsep.join(str(ef/x) for x in ['bin','scripts','pgm/bin','debugger/bin','ipm/bin'])+os.pathsep+env.get('PATH','')
env.setdefault('EFINITY_USER_DIR_INI',str(Path(os.environ.get('LOCALAPPDATA',''))/'efinity/user_dir.ini') if win else str(Path.home()/'.local/share/efinity/user_dir.ini'))
prj=root/'PROJECT/par/ddr_demo_ti60';logs=root/'verification/r5_7';logs.mkdir(parents=True,exist_ok=True)
log=logs/('efinity_'+a.flow+'.log')
print('Efinity:',ef,'; flow:',a.flow,'; log:',log,flush=True)
with log.open('w',encoding='utf-8') as f:
 result=subprocess.run([str(python),'-u',str(ef/'scripts/efx_run.py'),'ddr_demo_ti60.xml','--prj','-f',a.flow],cwd=prj,env=env,stdout=f,stderr=subprocess.STDOUT)
if result.returncode:
 print(log.read_text(errors='replace')[-6000:]);raise SystemExit(result.returncode)
if a.flow in ('compile','pnr','pgm'):
 report=prj/'outflow/ddr_demo_ti60.timing.rpt'
 timing=report.read_text() if report.is_file() else ''
 match=re.search(r'Clock Relationship Summary \(begin\)(.*?)Clock Relationship Summary \(end\)',timing,re.S)
 relations=[] if not match else re.findall(r'^\s*(\S+)\s+(\S+)\s+(-?\d+\.\d+)\s+(-?\d+\.\d+)\s+\([RF]-[RF]\)',match.group(1),re.M)
 if len(relations)<14 or any(float(row[3])<0 for row in relations):
  print('Timing failed or incomplete: candidate bit is NOT promoted to BOARD_TEST.',[(x[0],x[3]) for x in relations if float(x[3])<0]);raise SystemExit(2)
 print('All setup and hold clock relationships pass; worst slack:',min(float(x[3]) for x in relations),'ns')
if a.flow in ('compile','pgm'):
 bit=prj/'outflow/ddr_demo_ti60.bit'
 if not bit.is_file():p.error('Efinity returned success but no bit was produced')
 dest=root/'BOARD_TEST/r5_7/ti60_r5_7.bit';dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(bit,dest)
 print('Built:',dest,'SHA256:',hashlib.sha256(dest.read_bytes()).hexdigest())
print('Completed. Inspect timing reports before board use. No device was programmed.')
