"""Preserve steady 720p/1080p symbols and exercise the new physical-lane guard.
The vendor LVDS serializer and real monitor remain hardware validation tasks.
"""
from pathlib import Path
import re,subprocess,json,hashlib
v=Path(__file__).resolve().parent;r=v.parents[1];old=r/'verification/r5_2';rtl=r/'PROJECT/rtl/hdmi';production=v/'reference_r57'
checker=(old/'check_hdmi_packets.py').read_text();checker=re.sub(r"\(v/\('packets_result.json'.*?\.write_text", "(path.parent/'packets_result.json').write_text",checker);(v/'check_hdmi_packets.py').write_text(checker)
result=[]
for name,source,guard in [('baseline',production,False),('steady',rtl,False),('link_restart',rtl,True)]:
 o=v/name;o.mkdir(exist_ok=True);tb=(old/'hdmi_packets_tb.sv').read_text()
 tb=tb.replace('integer tmds_pixels=0;',"integer tmds_pixels=0;reg[31:0] frame_hash=32'h811c9dc5;")
 tb=tb.replace('pixels=pixels+1;',"pixels=pixels+1;frame_hash=(frame_hash ^ expected_rgb)*32'h01000193;")
 tb=tb.replace('$display("FRAME_PASS',"$display(\"FRAME_HASH %h\",frame_hash);frame_hash=32'h811c9dc5;\n        $display(\"FRAME_PASS")
 if guard:
  tb=tb.replace('  wire [9:0] tmds_clk', '  wire link_oe,link_reset;\n  wire [9:0] tmds_clk')
  tb=tb.replace('.clk(clk),.rst(rst),.axi_rst', '.link_tx_enable(link_oe),.link_tx_reset(link_reset),.clk(clk),.rst(rst),.axi_rst',1)
  pos=tb.index('  function automatic [15:0] pixel')
  tb=tb[:pos]+'''  task link_change(input bit target);
    integer first_frame;
    begin
      apb_write('h60,1);repeat(20)@(negedge pix_clk);
      if(link_oe||!link_reset)$fatal(1,"lanes did not enter reset/quiet");
      apb_write('h50,target);wait(dut.mode_1080==target);first_frame=frames;wait(frames>=first_frame+2);
      if(link_oe||!link_reset)$fatal(1,"lanes restarted before release");
      apb_write('h60,0);repeat(20)@(negedge pix_clk);
      if(!link_oe||link_reset)$fatal(1,"lanes did not recover");
      $display("LINK_RESTART_PASS target=%0d",target);
    end
  endtask
'''+tb[pos:]
  tb=tb.replace("apb_write('h50,1)","link_change(1)").replace("apb_write('h50,0)","link_change(0)")
  # Guard tasks consume two extra frames each; use relative goals for later waits.
  tb=tb.replace('wait(frames==4)','wait(frames>=4)').replace('wait(frames==6)','wait(frames>=6)').replace('wait(frames==8)','wait(frames>=8)').replace('wait(frames==10)','wait(frames>=10)')
  # Disable packet trace for the additional deliberately-DVI diagnostic interval.
  tb=tb.replace('integer tracefd;', 'integer tracefd;')
  tb=tb.replace('integer tracefd', 'reg trace_enable=1;\n  integer tracefd',1)
  tb=tb.replace('if(hh[4]>=0', 'if(trace_enable&&hh[4]>=0')
  marker='    $display("PACKET_TRACE_PASS:'
  at=tb.index(marker)
  tb=tb[:at]+'''    trace_enable=0;
    apb_write('h60,2);repeat(5000)@(negedge pix_clk);
    if(dut.u_island.lhdmi)$fatal(1,"1080 DVI diagnostic still emits HDMI islands");
    apb_write('h60,3);repeat(20)@(negedge pix_clk);apb_write('h50,0);wait(!dut.mode_1080);repeat(5000)@(negedge pix_clk);
    if(!dut.u_island.lhdmi)$fatal(1,"DVI override must not alter 720p HDMI/audio");
    apb_write('h60,0);repeat(20)@(negedge pix_clk);if(!link_oe||link_reset)$fatal(1,"recovery did not release all lanes");
    $display("PASS 1080-only DVI diagnostic and 720p HDMI restoration");
'''+tb[at:]
 (o/'tb.sv').write_text(tb)
 with (o/'build.log').open('w') as f:subprocess.run(['/home/user/.local/bin/verilator','--binary','--timing','-j','4','-Wno-fatal','--top-module','hdmi_packets_tb','--Mdir',str(o/'obj'),str(o/'tb.sv'),str(source/'hdmi_fb.v'),str(source/'hdmi_island.v')],stdout=f,stderr=subprocess.STDOUT,check=True)
 with (o/'run.log').open('w') as f:subprocess.run([str(o/'obj/Vhdmi_packets_tb')],cwd=o,stdout=f,stderr=subprocess.STDOUT,check=True,timeout=300)
 with (o/'decode.log').open('w') as f:subprocess.run(['python3',str(v/'check_hdmi_packets.py'),str(o/'tmds_blank.txt')],stdout=f,stderr=subprocess.STDOUT,check=True,timeout=180)
 d=json.loads((o/'packets_result.json').read_text());assert d['passed'];d.update(name=name,frame_hashes=re.findall(r'FRAME_HASH (\w+)',(o/'run.log').read_text()),blank_trace_sha256=hashlib.sha256((o/'tmds_blank.txt').read_bytes()).hexdigest());result.append(d);print(name,'PASS',flush=True)
assert result[0]['frame_hashes']==result[1]['frame_hashes'] and result[0]['blank_trace_sha256']==result[1]['blank_trace_sha256']
(v/'video_checks.json').write_text(json.dumps({'steady720_and1080_preserved':True,'vendor_serializer_modeled':False,'monitor_tested':False,'results':result},indent=2)+'\n')
