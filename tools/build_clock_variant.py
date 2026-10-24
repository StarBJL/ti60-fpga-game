#!/usr/bin/env python3
"""Create an isolated, reproducible Ti60 processing-clock candidate.
DDR remains 100/400 MHz and the complete HDMI PLL/display chain is unchanged.
No device is programmed. Exact 202 MHz is not a legal selected PLL ratio;
202p083333 uses 25 MHz * 97 / 12 and integer BSP timing of 202083333 Hz.
"""
from pathlib import Path
import argparse,json,re,shutil,subprocess,sys
r=Path(__file__).resolve().parents[1]
a=argparse.ArgumentParser(description=__doc__);a.add_argument('--profile',choices=['202p083333','205'],required=True);a.add_argument('--out',type=Path,required=True);a.add_argument('--prepare-only',action='store_true');a.add_argument('--efinity',type=Path);a.add_argument('--toolchain',type=Path);args=a.parse_args()
dest=args.out.resolve()
if dest.exists():a.error('Output must be a new directory. Existing projects are never overwritten.')
if r==dest or r in dest.parents and 'PROJECT' in dest.relative_to(r).parts:a.error('Do not place the candidate inside PROJECT.')
hz,m,fb,div=(202083333,1,97,12) if args.profile=='202p083333' else (205000000,2,41,10)
shutil.copytree(r/'PROJECT',dest/'PROJECT',ignore=shutil.ignore_patterns('outflow','work_syn','work_pnr','build','build_release','obj_files','__pycache__','*.o','*.elf','*.bin','*.asm','*.map'))
for name in ['build_hardware.py','build_software.py']:shutil.copy2(r/name,dest/name)
p=dest/'PROJECT/par/ddr_demo_ti60'
s=(p/'ddr_demo_ti60.peri.xml').read_text();start=s.index('<efxpt:pll name="pll_inst1"');end=s.index('</efxpt:pll>',start)+len('</efxpt:pll>');pll=s[start:end]
assert 'name="pll_oc"' not in s,'Source must be the 200MHz base'
pll=pll.replace('name="pll_inst1"','name="pll_oc"').replace('pll_def="PLL_TL0"','pll_def="PLL_BL0"').replace('locked_name="user_pll_locked"','locked_name="oc_pll_locked"').replace('pll_fb_clk','oc_fb_clk').replace('name="core_clk"','name="axi_clk"')
pll=re.sub(r'multiplier="\d+"',f'multiplier="{m}"',pll);pll=re.sub(r'pre_divider="\d+"','pre_divider="1"',pll);pll=re.sub(r'post_divider="\d+"','post_divider="2"',pll)
pll=re.sub(r'(name="oc_fb_clk" number="0" out_divider=")\d+',fr'\g<1>{fb}',pll);pll=re.sub(r'(name="axi_clk" number="1" out_divider=")\d+',fr'\g<1>{div}',pll)
(p/'ddr_demo_ti60.peri.xml').write_text(s[:end]+'\n        '+pll+s[end:])
f=dest/'PROJECT/rtl/ddr3_example_top.v';s=f.read_text();assert 'CORE_CLOCK_HZ = 200000000' in s
s=s.replace('CORE_CLOCK_HZ = 200000000',f'CORE_CLOCK_HZ = {hz}').replace('ASYN_AXI_CLK       = `ASYN_AXI_CLK','ASYN_AXI_CLK       = 1').replace('   input                              pll_fb_clk,','   input                              oc_fb_clk,\n   input                              oc_pll_locked,\n   input                              pll_fb_clk,').replace('assign sys_rst       = pll_locked & user_pll_locked;','assign sys_rst       = pll_locked & user_pll_locked & oc_pll_locked;');f.write_text(s)
f=p/'sdc/ddr3.sdc';s=f.read_text();s=re.sub(r'(create_clock -period )\d+\.?\d*(\s+axi_clk)',fr'\g<1>{1e9/hz:.9f}\2',s)
s+=f'\n# Independent processing PLL; DDR clock remains unchanged at 100MHz.\ncreate_clock -period {1000/(25*m):.9f} oc_fb_clk\nset_clock_groups -asynchronous -group {{axi_clk}} -group {{pll_fb_clk core_clk hdmi_tx_slow_clk hdmi_tx_fast_clk}}\n';f.write_text(s)
for rel in ['embedded_sw/soc/bsp/efinix/EfxSapphireSoc/include/soc.h','ip/soc/source/soc_config']:
 f=p/rel;f.write_text(re.sub(r'(SYSTEM_CLINT_HZ |--systemFrequency )\d+',fr'\g<1>{hz}',f.read_text()))
f=p/'ip/soc/soc.v';s=f.read_text();assert "bridge_uartConfigReg_clockDivider <= 20'h000d8;" in s
f.write_text(s.replace("bridge_uartConfigReg_clockDivider <= 20'h000d8;",f"bridge_uartConfigReg_clockDivider <= 20'h{hz//(115200*8)-1:05x};"))
f=p/'ip/soc/settings.json';s=f.read_text();s=re.sub(r'("Frequency"\s*:\s*)("?)200\2',fr'\g<1>\g<2>{202 if m==1 else 205}\2',s);f.write_text(s)
(dest/'clock_candidate.json').write_text(json.dumps({'profile':args.profile,'actual_hz':hz,'exact_hz_ratio':[25_000_000*m*fb,div],'pll':[m,1,2,fb,div],'ddr_hz':[100000000,400000000],'hdmi_pixel_hz':74375000,'extra_pll':'PLL_BL0','hardware_tested':False},indent=2)+'\n')
print('Prepared',dest,'processing Hz',hz,flush=True)
if not args.prepare_only:
 subprocess.run([sys.executable,str(dest/'build_hardware.py'),*(['--efinity',str(args.efinity)] if args.efinity else [])],check=True)
 subprocess.run([sys.executable,str(dest/'build_software.py'),*(['--toolchain',str(args.toolchain)] if args.toolchain else [])],check=True)
