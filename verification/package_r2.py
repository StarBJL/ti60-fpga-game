from pathlib import Path
import json,hashlib,shutil,xml.etree.ElementTree as ET,re
root=Path(__file__).resolve().parents[1];p=root/'PROJECT';v=root/'verification';out=p/'par/ddr_demo_ti60/outflow'
g=p/'par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game';dest=root/'BOARD_TEST';dest.mkdir(exist_ok=True)
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
def text(f):return f.read_text(encoding='utf-8-sig')
assert 'pgm :\tPASS' in text(v/'hardware_compile.log')
assert len(json.loads(text(v/'motion/results.json')))==7
assert len(json.loads(text(v/'square_draw/results.json')))==5
assert json.loads(text(v/'cpu_motion.json'))['exact']
assert 'PASS' in text(v/'audio_av.log')
assert '0 / 3000 random vectors differ' in text(v/'netlist_cone.log')
for name in ['native_game','native_touhou','game_sound','font']:assert 'PASS' in text(v/(name+'.log'))
for name in ['software_stats0.log','software_stats1.log']:
 assert 'Memory region' in text(v/name) and 'error:' not in text(v/name)
for src,name in [(out/'ddr_demo_ti60.bit','720p_merge_r2.bit'),(g/'build/game.elf','720p_merge_r2_debug.elf'),(g/'build_release/game.elf','720p_merge_r2_release.elf')]:
 shutil.copy2(src,dest/name)
tree=ET.parse(g/'game_ti.launch')
oldroots=['D:\\FPGA_competition\\Ti60_720p_SQUARE20K_MERGE\\PROJECT','E:\\AAA\\Ti60F225_DemoBoard_v4\\08_ti60f225_soc_demo\\09_Ti60F225_hardjtag_demo']
for n in tree.getroot():
 if 'value' in n.attrib:
  value=n.get('value')
  for old in oldroots:value=value.replace(old,str(p))
  n.set('value',value)
tree.write(g/'game_ti.launch',encoding='utf-8',xml_declaration=True)
for n in tree.getroot():
 key=n.get('key','')
 if key in ['org.eclipse.cdt.launch.PROGRAM_NAME','org.eclipse.cdt.debug.gdbjtag.core.imageFileName','org.eclipse.cdt.debug.gdbjtag.core.symbolsFileName']:n.set('value',str(dest/'720p_merge_r2_debug.elf'))
 if key.endswith(('useProjBinaryForImage','useProjBinaryForSymbols')):n.set('value','false')
 if key.endswith(('useFileForImage','useFileForSymbols')):n.set('value','true')
tree.write(dest/'720p_merge_r2_debug.launch',encoding='utf-8',xml_declaration=True)
(dest/'reports').mkdir(exist_ok=True)
for suffix in ['place.rpt','timing.rpt','cdc.rpt','hier_util.rpt']:shutil.copy2(out/('ddr_demo_ti60.'+suffix),dest/'reports'/('ddr_demo_ti60.'+suffix))
original=Path('D:/FPGA_competition/Ti60_720p_SQUARE20K_MERGE')
# Original R1 deliverables must still match their existing manifest.
old=json.loads(text(original/'BOARD_TEST/manifest.json'))
assert all(sha(original/'BOARD_TEST'/k)==a['sha256'] for k,a in old['artifacts'].items())
assert sha(g/'sound/sound_bank.bin')==sha(original/'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/sound/sound_bank.bin')
sources=list((g/'src').glob('*'))+[p/'rtl/blit/blit_engine.v',p/'rtl/blit/square_motion.v',p/'rtl/ddr3_example_top.v',p/'rtl/hdmi/hdmi_fb.v',p/'par/ddr_demo_ti60/ddr_demo_ti60.xml']
data=dict(build_id='720P-MERGE-R2-ZH-FULL-20261023',board_downloaded=False,board_fps_measured=False,
 base='Ti60_720p_SQUARE20K_MERGE R1, preserved',characters='Original base artwork with static 1.5x native scaling; R19 not imported',
 native_resolution=[1280,720],square_capacity=30000,square_fine_step=100,carrot_custom_capacity=40000,core_hz=200000000,
 RAM=[253,256],XLR=[55848,60800],DSP=[32,160],setup_slack_ns=0.053,hold_slack_ns=0.025,
 artifacts={f.relative_to(dest).as_posix():dict(bytes=f.stat().st_size,sha256=sha(f)) for f in sorted(dest.rglob('*')) if f.is_file() and f.name!='manifest.json'},
 source_sha256={f.relative_to(p).as_posix():sha(f) for f in sources if f.is_file()},
 logger_sha256={str(f.relative_to(root)):sha(f) for f in [root/'Open_Serial_Log.cmd',root/'tools/serial_monitor.ps1']},
 offline_checks=dict(motion=7,full_frame_square_draw=5,cpu_elf_motion='20K exact',native_carrot='PASS',native_touhou='PASS',native_images='PASS with board-only calls stubbed',
 audio_events='PASS',audio_hdmi='PASS shortened raster',font='PASS clipping/UTF8/cache reuse',netlist_credit_vectors=3000))
(dest/'manifest.json').write_text(json.dumps(data,ensure_ascii=False,indent=2),encoding='utf-8')
(root/'logs').mkdir(exist_ok=True)
print('Packaged R2 bit + 2 ELFs + IDE launch + reports; R1 artifacts verified unchanged.')
print(json.dumps(data['artifacts'],indent=2))
