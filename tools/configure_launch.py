#!/usr/bin/env python3
"""Update the bundled Eclipse launch file after moving this folder. Does not program hardware."""
from pathlib import Path
import argparse,xml.etree.ElementTree as ET,sys
p=argparse.ArgumentParser();p.add_argument('--root',type=Path);a=p.parse_args()
root=a.root.resolve() if a.root else Path(__file__).resolve().parents[1]
file=Path(__file__).resolve().parents[1]/'BOARD_TEST/ti60_r5_debug.launch'
tree=ET.parse(file);soc=root/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc';elf=root/'BOARD_TEST/ti60_r5_debug.elf'
ocd=soc/'bsp/efinix/EfxSapphireSoc/openocd'
mapping={
'ilg.gnumcueclipse.debug.gdbjtag.openocd.gdbServerOther':f'-c "set bsp_loc {{{soc.as_posix()}}}"\n-f "{(ocd/"ftdi_ti.cfg").as_posix()}"\n-f "{(ocd/"debug_ti.cfg").as_posix()}"',
'ilg.gnumcueclipse.debug.gdbjtag.openocd.targetRegFile':str(ocd/'sapphire_soc_32bit-reg.xml'),
'ilg.gnumcueclipse.debug.gdbjtag.svdPath':str(ocd/'sapphire_soc_xsvd.json'),
'org.eclipse.cdt.debug.gdbjtag.core.imageFileName':str(elf),
'org.eclipse.cdt.debug.gdbjtag.core.symbolsFileName':str(elf),
'org.eclipse.cdt.launch.PROGRAM_NAME':str(elf),
'ilg.gnumcueclipse.debug.gdbjtag.openocd.gdbServerExecutable':'${eclipse_home}/../openocd/bin/openocd'+('.exe' if sys.platform=='win32' else ''),
'org.eclipse.cdt.dsf.gdb.DEBUG_NAME':'${eclipse_home}/../toolchain/bin/riscv-none-elf-gdb'+('.exe' if sys.platform=='win32' else '')}
for e in tree.getroot():
    if e.get('key') in mapping:e.set('value',mapping[e.get('key')])
ET.indent(tree,space='    ');tree.write(file,encoding='utf-8',xml_declaration=True)
print('Updated',file,'for',root)
