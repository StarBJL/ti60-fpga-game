#!/usr/bin/env python3
"""Build clock-matched verified ELFs; never programs hardware.
Default profile and IDE build/game.elf are 225 MHz. Production BSP stays read-only;
clock metadata overrides use disposable copies plus an explicit GAME_CORE_HZ define.
"""
from pathlib import Path
import argparse,os,shutil,subprocess,re,json,hashlib,tempfile
R=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--toolchain',type=Path,required=True);p.add_argument('--profile',choices=['all','200','205p882353','225'],default='225');a=p.parse_args()
gcc=next((d/n for d in (a.toolchain/'bin',a.toolchain) for n in ('riscv-none-elf-gcc','riscv-none-elf-gcc.exe') if (d/n).exists()),None)
if not gcc:raise SystemExit('Missing riscv-none-elf-gcc')
assets=json.loads((R/'assets/v4/runtime/manifest.json').read_text())
if len(assets)!=6 or any(x.get('runtime_dimensions')!=[1280,720] for x in assets):raise SystemExit('Run tools/build_campaign_art.py to completion before building HD720 ELF files.')
prefix=str(gcc)[:-(7 if str(gcc).endswith('.exe') else 3)];g=R/'par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game'
bsp=R/'par/ddr_demo_ti60/embedded_sw/soc/bsp/efinix/EfxSapphireSoc/include/soc.h'
original=bsp.read_bytes()
def source_digest():
 h=hashlib.sha256()
 for f in sorted((g/'src').glob('*'))+[g/'makefile',g/'sound/sound_bank.bin',g/'sound/sound_bank.S']:
  if f.is_file():h.update(str(f.relative_to(g)).encode());h.update(f.read_bytes())
 return h.hexdigest()
source_hash=source_digest()
profiles=[('200',200000000),('205p882353',205882353),('225',225000000)]
with tempfile.TemporaryDirectory(prefix='ti60-v6-bsp-') as temp:
 for label,hz in profiles:
  if a.profile not in ('all',label):continue
  text=original.decode();text,n=re.subn(r'(#define SYSTEM_CLINT_HZ )\d+',lambda m:m[1]+str(hz),text)
  assert n==1
  build_bsp=Path(temp)/label;shutil.copytree(bsp.parent.parent,build_bsp)
  (build_bsp/'include/soc.h').write_bytes(text.encode())
  out=R/'BOARD_TEST'/('verified_'+label);out.mkdir(parents=True,exist_ok=True)
  for kind,stats in [('debug','1'),('release','0')]:
   folder=('build' if kind=='debug' else 'build_release') if label=='225' else 'build_'+label+'_'+kind
   cmd=['make','-B','-j8','RISCV_BIN='+prefix,'BSP=efinix/EfxSapphireSoc','BSP_PATH='+str(build_bsp),'CFLAGS_ARGS=-DGAME_CORE_HZ='+str(hz)+'u','DEBUG=no','STATS='+stats,'SOUND=1','OBJDIR='+folder,folder+'/game.elf']
   run=subprocess.run(cmd,cwd=g,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
   (out/('build_'+kind+'.log')).write_text(run.stdout)
   if run.returncode:print(run.stdout);raise SystemExit(run.returncode)
   assert source_digest()==source_hash, 'Source changed during ELF compilation; rebuild all profiles'
   if re.search(r'warning:|error:',run.stdout,re.I):raise SystemExit('Compiler diagnostics require review: '+str(out))
   dst=out/('ti60_verified_'+label+'_'+kind+'.elf');shutil.copy2(g/folder/'game.elf',dst)
   for suffix in ('map',):
    f=g/folder/('game.'+suffix)
    if f.exists():shutil.copy2(f,out/('ti60_verified_'+label+'_'+kind+'.'+suffix))
   print(label,kind,dst.stat().st_size,hashlib.sha256(dst.read_bytes()).hexdigest(),flush=True)
  (out/'software_clock.json').write_text(json.dumps({'clock_hz_rounded':hz,'exact_frequency_mhz':'3500/17' if label=='205p882353' else label,'first_five_scene_budget_seconds':60,'minimum_clear_seconds':40,'endless_wave_seconds':50,'last_scene_endless':True,'uart_baud':115200,'firmware':'TI60-V6-AWAKENING-20261031','source_digest':source_hash,'application_clock_define':'GAME_CORE_HZ='+str(hz)+'u','board_tested':False},indent=2))
assert bsp.read_bytes()==original, "production BSP must remain unchanged"

