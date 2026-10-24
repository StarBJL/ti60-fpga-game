#!/usr/bin/env python3
"""Build R5 firmware with the existing Efinity toolchain, without touching RTL."""
from pathlib import Path
import argparse, hashlib, json, os, shutil, subprocess, re

root=Path(__file__).resolve().parent
parser=argparse.ArgumentParser()
parser.add_argument('--stats',choices=['0','1','both'],default='both')
parser.add_argument('--toolchain',help='Directory containing riscv-none-elf-gcc, or its full executable path')
args=parser.parse_args()
given=args.toolchain or os.environ.get('EFINITY_RISCV_TOOLCHAIN')
candidates=[Path(given)] if given else []
if shutil.which('riscv-none-elf-gcc'):candidates.append(Path(shutil.which('riscv-none-elf-gcc')))
candidates.extend([Path('/home/user/fpga_contest/riscv/efinity-riscv-ide-2026.1/toolchain/bin'),Path('D:/Efinity/efinity-riscv-ide-2026.1/toolchain/bin')])
gcc=None
for p in candidates:
    for q in ([p/'riscv-none-elf-gcc',p/'riscv-none-elf-gcc.exe'] if p.is_dir() else [p]):
        if q.is_file():gcc=q.resolve();break
    if gcc:break
if not gcc:parser.error('Pass --toolchain /path/to/efinity/toolchain/bin')
make=shutil.which('make')
if not make:
    p=gcc.parents[2]/'build_tools/bin/make.exe'
    if p.is_file():make=str(p)
if not make:parser.error('GNU make is required')
game=root/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game'
out=root/'verification/r5_3';out.mkdir(parents=True,exist_ok=True)
prefix=str(gcc.parent/'riscv-none-elf-')
env=dict(os.environ);env['PATH']=str(Path(make).parent)+os.pathsep+str(gcc.parent)+os.pathsep+env.get('PATH','')
for stats in ([1,0] if args.stats=='both' else [int(args.stats)]):
    name='debug' if stats else 'release';obj='build' if stats else 'build_release'
    log=out/('build_'+name+'.log')
    with log.open('w') as f:
        subprocess.run([make,'-B','-j4','OBJDIR='+obj,'STATS='+str(stats),'DEBUG=no','BENCH=yes','SOUND=1','RISCV_BIN='+prefix],cwd=game,env=env,stdout=f,stderr=subprocess.STDOUT,check=True)
    dest=root/'BOARD_TEST/r5_3'/('ti60_r5_3_'+name+'.elf');dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(game/obj/'game.elf',dest)
    print(name,':',dest,'; log:',log)
manifest=root/'BOARD_TEST/r5_3/manifest.json'
if manifest.exists():
    data=json.loads(manifest.read_text());data['board_downloaded']=False;data['board_fps_measured']=False;data['validation_status']='rebuild_requires_retesting'
    for p in (root/'BOARD_TEST/r5_3').glob('*'):
        if p.is_file() and p.suffix in ('.elf','.bit','.launch'):
            data.setdefault('artifacts',{})[p.name]={'path':p.relative_to(root).as_posix(),'bytes':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()}
    data['source_sha256']={str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in list((game/'src').glob('*'))+list((root/'tools/control_panel').glob('*')) if p.is_file()}
    for name in ['debug','release']:
        log=out/('build_'+name+'.log')
        if log.exists():data.setdefault('memory_regions_bytes',{})[name]={k:int(v) for k,v in re.findall(r'(ram|snd|art):\s+(\d+) B',log.read_text())}
    manifest.write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n')
