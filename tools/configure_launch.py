#!/usr/bin/env python3
"""Resolve the R6 prebuilt launch configuration after moving the project. No download."""
from pathlib import Path
import argparse,xml.etree.ElementTree as ET,sys
p=argparse.ArgumentParser();p.add_argument('--root',type=Path);p.add_argument('--release',action='store_true');a=p.parse_args()
r=a.root.resolve() if a.root else Path(__file__).resolve().parents[1]
soc=r/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc';g=soc/'software/standalone/game';out=r/'BOARD_TEST/r6';out.mkdir(parents=True,exist_ok=True)
kind='release' if a.release else 'debug';elf=out/('ti60_r6_'+kind+'.elf');file=out/('ti60_r6_'+kind+'.launch');tree=ET.parse(g/'game_ti.launch');ocd=soc/'bsp/efinix/EfxSapphireSoc/openocd'
exe='.exe' if sys.platform=='win32' else ''
fields={
'ilg.gnumcueclipse.debug.gdbjtag.openocd.gdbServerOther':f'-c "set bsp_loc {{{soc.as_posix()}}}"\n-f "{(ocd/"ftdi_ti.cfg").as_posix()}"\n-f "{(ocd/"debug_ti.cfg").as_posix()}"',
'ilg.gnumcueclipse.debug.gdbjtag.openocd.targetRegFile':str(ocd/'sapphire_soc_32bit-reg.xml'),
'ilg.gnumcueclipse.debug.gdbjtag.svdPath':str(ocd/'sapphire_soc_xsvd.json'),
'org.eclipse.cdt.debug.gdbjtag.core.imageFileName':str(elf),
'org.eclipse.cdt.debug.gdbjtag.core.symbolsFileName':str(elf),
'org.eclipse.cdt.launch.PROGRAM_NAME':str(elf),'org.eclipse.cdt.launch.PROJECT_ATTR':'game_r6',
'ilg.gnumcueclipse.debug.gdbjtag.openocd.gdbServerExecutable':'${eclipse_home}/../openocd/bin/openocd'+exe,
'org.eclipse.cdt.dsf.gdb.DEBUG_NAME':'${eclipse_home}/../toolchain/bin/riscv-none-elf-gdb'+exe}
for e in tree.getroot():
 if e.get('key') in fields:e.set('value',fields[e.get('key')])
ET.indent(tree,space='    ');tree.write(file,encoding='utf-8',xml_declaration=True);print(file)
