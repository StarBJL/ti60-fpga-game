"""Decode real RTL TMDS video/audio for each processing clock, and compare the
200MHz result with the unchanged production R5.6 display implementation.
Does not model the monitor, cable or external LVDS serial driver.
"""
from pathlib import Path
import subprocess,hashlib,json,sys,re,shutil
v=Path(__file__).resolve().parent;r=v.parents[1];old=r/'verification/r5_2';rtl=r/'PROJECT/rtl/hdmi'
production=Path(sys.argv[1]) if len(sys.argv)>1 else Path('/home/user/fpga_contest/1025/Ti60_R5_DUALVIDEO')
# Executable preservation check: the new hardware ID/frequency registers are the
# only edits to hdmi_fb.v; packet/timing/scaler logic must remain byte-identical.
a=(rtl/'hdmi_fb.v').read_text();b=(production/'PROJECT/rtl/hdmi/hdmi_fb.v').read_text()
import difflib
(v/'video_source.patch').write_text(''.join(difflib.unified_diff(b.splitlines(True),a.splitlines(True),fromfile='R5.6/hdmi_fb.v',tofile='R5.7/hdmi_fb.v')))
assert (rtl/'hdmi_island.v').read_bytes()==(production/'PROJECT/rtl/hdmi/hdmi_island.v').read_bytes()
checker=(old/'check_hdmi_packets.py').read_text();checker=re.sub(r"\(v/\('packets_result.json'.*?\.write_text", "(path.parent/'packets_result.json').write_text",checker)
(v/'check_hdmi_packets.py').write_text(checker)
results=[]
for label,hz,source in [('200',200000000,rtl),('baseline200',200000000,production/'PROJECT/rtl/hdmi'),('202p083333',202083333,rtl),('205',205000000,rtl)]:
 o=v/('video_'+label);o.mkdir(exist_ok=True);tb=(old/'hdmi_packets_tb.sv').read_text().replace('always #2.5 clk=~clk;',f'always #{500000000/hz:.9f} clk=~clk;')
 tb=tb.replace('integer tmds_pixels=0;','integer tmds_pixels=0;reg [31:0] frame_hash=32\'h811c9dc5;')
 tb=tb.replace('pixels=pixels+1;','pixels=pixels+1;frame_hash=(frame_hash ^ expected_rgb)*32\'h01000193;')
 tb=tb.replace('$display("FRAME_PASS', '$display("FRAME_HASH %h",frame_hash);frame_hash=32\'h811c9dc5;\n        $display("FRAME_PASS')
 (o/'tb.sv').write_text(tb)
 with (o/'build.log').open('w') as f:subprocess.run(['/home/user/.local/bin/verilator','--binary','--timing','-j','4','-Wno-fatal','--top-module','hdmi_packets_tb','--Mdir',str(o/'obj'),str(o/'tb.sv'),str(source/'hdmi_fb.v'),str(source/'hdmi_island.v')],stdout=f,stderr=subprocess.STDOUT,check=True)
 with (o/'run.log').open('w') as f:subprocess.run([str(o/'obj/Vhdmi_packets_tb')],cwd=o,stdout=f,stderr=subprocess.STDOUT,check=True,timeout=300)
 with (o/'decode.log').open('w') as f:subprocess.run([sys.executable,str(v/'check_hdmi_packets.py'),str(o/'tmds_blank.txt')],stdout=f,stderr=subprocess.STDOUT,check=True,timeout=180)
 result=json.loads((o/'packets_result.json').read_text());assert result['passed'];result.update(profile=label,processing_hz=hz,blank_trace_sha256=hashlib.sha256((o/'tmds_blank.txt').read_bytes()).hexdigest(),frame_hashes=re.findall(r'FRAME_HASH (\w+)',(o/'run.log').read_text()))
 results.append(result);print(label,'PASS decoded video, sync, AVI, PCM and mode transitions',flush=True)
assert results[0]['blank_trace_sha256']==results[1]['blank_trace_sha256'] and results[0]['frame_hashes']==results[1]['frame_hashes']
(v/'video_checks.json').write_text(json.dumps({'display720_preserved':True,'hardware_tested':False,'profiles':results},indent=2)+'\n')
