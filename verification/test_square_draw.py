"""Real blitter + wide bridge RTL, native 1280x720, packed table vs independent RGB565 pixels."""
from pathlib import Path
import ctypes,os,subprocess,random,json,sys,time
root=Path(__file__).resolve().parents[1];out=Path(__file__).parent/'square_draw';out.mkdir(exist_ok=True)
env=dict(os.environ);env['PATH']='C:/msys64/ucrt64/bin;'+env['PATH'];ctypes.windll.kernel32.SetErrorMode(0x8003)
def run(cmd,timeout=240):
 r=subprocess.run([str(x) for x in cmd],env=env,cwd=root,text=True,capture_output=True,timeout=timeout,creationflags=0x08000000)
 if r.returncode:raise RuntimeError(r.stdout+r.stderr)
 return r.stdout+r.stderr
wrapper=(root/'PROJECT/sim/blit/tb_blit_top.v').read_text()
wrapper=wrapper.replace('.AXI_ID         (4\'d1              )',".WIN_BYTES (2560), .AXI_ID (4'd1)")
# Keep explicit parameter override independent of formatting of the baseline wrapper.
if '.WIN_BYTES' not in wrapper:wrapper=wrapper.replace('blit_engine #(', 'blit_engine #(.WIN_BYTES(2560),')
(out/'wrapper.v').write_text(wrapper)
sv='''`timescale 1ns/1ps
module test;
 reg clk=0,rst=1;always #2.5 clk=~clk;
 reg[11:0] apb_paddr=0;reg apb_psel=0,apb_penable=0,apb_pwrite=0;reg[31:0]apb_pwdata=0;
 wire[31:0]apb_prdata;wire apb_pready;
 wire s_arvalid,s_arready,s_rvalid,s_rready,s_rlast;
 wire[31:0]s_araddr;wire[7:0]s_arlen;wire[127:0]s_rdata;
 wire s_awvalid,s_awready,s_wvalid,s_wready,s_wlast,s_bvalid,s_bready;
 wire[31:0]s_awaddr;wire[7:0]s_awlen;wire[127:0]s_wdata;wire[15:0]s_wstrb;
 wire busy;
 tb_blit_top dut(.clk(clk),.rst(rst),.apb_paddr(apb_paddr),.apb_psel(apb_psel),.apb_penable(apb_penable),
 .apb_pwrite(apb_pwrite),.apb_pwdata(apb_pwdata),.apb_prdata(apb_prdata),.apb_pready(apb_pready),
 .hd_arvalid(1'b0),.hd_araddr(32'd0),.hd_arlen(8'd0),
 .s_arvalid(s_arvalid),.s_arready(s_arready),.s_araddr(s_araddr),.s_arlen(s_arlen),
 .s_rvalid(s_rvalid),.s_rready(s_rready),.s_rdata(s_rdata),.s_rlast(s_rlast),.s_rresp(2'b0),
 .s_awvalid(s_awvalid),.s_awready(s_awready),.s_awaddr(s_awaddr),.s_awlen(s_awlen),
 .s_wvalid(s_wvalid),.s_wready(s_wready),.s_wdata(s_wdata),.s_wstrb(s_wstrb),.s_wlast(s_wlast),
 .s_bvalid(s_bvalid),.s_bready(s_bready),.s_bresp(2'b0),.busy(busy));
 reg[31:0]mem[0:4194303],expected[0:460799];
 reg[31:0]ar_a[0:262143],aw_a[0:262143];reg[7:0]ar_l[0:262143],aw_l[0:262143];
 reg[127:0]wf_d[0:262143];reg[15:0]wf_s[0:262143];reg wf_last[0:262143];
 integer arp=0,awp=0,arr=0,awr=0,wp=0,wrp=0,ri=0,wi=0,bc=0,br=0,cycles=0;
 reg ar_allow=0,aw_allow=0,w_allow=0,r_offer=0;
 assign s_arready=ar_allow && arp-arr<24;
 assign s_awready=aw_allow && awp-awr<24;
 assign s_wready=w_allow && wp-wrp<32;
 assign s_rvalid=arr<arp && r_offer;assign s_rlast=ri==ar_l[arr];
 wire[31:0]ra=(ar_a[arr]>>2)+ri*4;
 assign s_rdata={mem[ra+3],mem[ra+2],mem[ra+1],mem[ra]};
 assign s_bvalid=br<bc;
 integer j,k,addr;
 always @(negedge clk)if(!rst)begin
  ar_allow<=$urandom_range(0,3)!=0;aw_allow<=$urandom_range(0,3)!=0;w_allow<=$urandom_range(0,3)!=0;
  if(!s_rvalid||s_rready)r_offer<=$urandom_range(0,3)!=0;
 end
 always @(posedge clk)if(!rst)begin
  cycles<=cycles+1;
  if(s_arvalid&&s_arready)begin
   if(s_araddr[3:0]!=0 || ((s_araddr&4095)+(s_arlen+1)*16)>4096)$fatal(1,"AR boundary");
   ar_a[arp]<=s_araddr;ar_l[arp]<=s_arlen;arp<=arp+1;
  end
  if(s_rvalid&&s_rready)begin if(s_rlast)begin arr<=arr+1;ri<=0;end else ri<=ri+1;end
  if(s_awvalid&&s_awready)begin
   if(s_awaddr[3:0]!=0 || ((s_awaddr&4095)+(s_awlen+1)*16)>4096)$fatal(1,"AW boundary");
   aw_a[awp]<=s_awaddr;aw_l[awp]<=s_awlen;awp<=awp+1;
  end
  if(s_wvalid&&s_wready)begin wf_d[wp]<=s_wdata;wf_s[wp]<=s_wstrb;wf_last[wp]<=s_wlast;wp<=wp+1;end
  if(awr<awp && wrp<wp)begin
   if(wf_last[wrp]!=(wi==aw_l[awr]))$fatal(1,"WLAST");
   addr=(aw_a[awr]>>2)+wi*4;
   for(j=0;j<16;j=j+1)if(wf_s[wrp][j])mem[addr+(j>>2)][(j%4)*8+:8]<=wf_d[wrp][j*8+:8];
   wrp<=wrp+1;
   if(wi==aw_l[awr])begin awr<=awr+1;wi<=0;bc<=bc+1;end else wi<=wi+1;
  end
  if(s_bvalid&&s_bready)br<=br+1;
  if(cycles>30000000)$fatal(1,"timeout decoder=%d bc=%d",dut.u_blit.d_state,dut.u_blit.bc_state);
 end
 task apb(input bit write,input[11:0] a,input[31:0] d,output[31:0] q);
 begin
  @(negedge clk);apb_psel=1;apb_penable=0;apb_pwrite=write;apb_paddr=a;apb_pwdata=d;
  @(negedge clk);apb_penable=1;
  @(posedge clk);while(!apb_pready)@(posedge clk);#0.1;q=apb_prdata;
  @(negedge clk);apb_psel=0;apb_penable=0;
 end endtask
 reg[1023:0]input_path,expected_path;reg[31:0]q;integer list_end;
 initial begin
  if(!$value$plusargs("input=%s",input_path)||!$value$plusargs("expected=%s",expected_path)||!$value$plusargs("end=%d",list_end))$fatal(1,"args");
  for(k=0;k<4194304;k=k+1)mem[k]=0;
  $readmemh(input_path,mem);$readmemh(expected_path,expected);
  repeat(6)@(negedge clk);rst=0;
  apb(0,12'hbc,0,q);if((q&12)!=12)$fatal(1,"CAPS2 %h",q);
  apb(1,12'h94,(45<<16)|2560,q);apb(1,12'h98,104858,q);apb(1,12'h9c,32'h400012,q);
  apb(1,12'h38,32'h280000,q);apb(1,12'h3c,list_end,q);
  repeat(12)@(negedge clk);
  while(busy)@(negedge clk);
  apb(0,12'h04,0,q);if((q&28)!=0)$fatal(1,"blit status %h",q);
  for(k=0;k<460800;k=k+1)if(mem[16384+k]!==expected[k])$fatal(1,"pixel word %0d got %h expected %h",k,mem[16384+k],expected[k]);
  if(arp!=arr||awp!=awr||wp!=wrp||br!=bc)$fatal(1,"not drained");
  $display("PASS native 1280x720 SQTAB framebuffer exact, cycles %0d AR %0d AW %0d",cycles,arp,awp);$finish;
 end
endmodule
'''
(out/'test.sv').write_text(sv)
run(['C:/msys64/ucrt64/bin/iverilog.exe','-g2012','-s','test','-o',out/'test.vvp',out/'test.sv',out/'wrapper.v',*[root/'PROJECT/rtl/blit'/f for f in ['blit_engine.v','blit_axi_rd_arb.v','blit_axi_wide.v']]])
palette=[0xf32a,0x3dbb,0xb70d,0xf5eb,0x9bdc,0xeb50,0x7698,0xf77a]
results=[]
for case,(n,alpha) in enumerate([(0,1),(65,0),(65,1),(20000,1),(30000,1)]):
 rng=random.Random(921+case);pixels=[0]*921600;objects=[]
 # Small squares at 20K exercise table count and odd/even across every burst.
 for i in range(n):
  size=6 if n>=20000 else rng.randrange(6,41);x=rng.randrange(1280-size+1);y=rng.randrange(40,676-size+1)
  if n==65 and i<8:x=[0,1274,31,32,63,64,1200,1274][i];size=6;y=[40,40,63,64,65,650,670,670][i]
  pal=i%8;objects.extend([(y*16<<16)|(x*16)|rng.randrange(16),(pal<<24)|(size<<16)|0x0101])
  a=150 if alpha and i%2 else 255;c=palette[pal];a9=a+(a>>7)
  for yy in range(y,y+size):
   off=yy*1280+x
   for xx in range(size):
    d=pixels[off+xx]
    pixels[off+xx]=sum((((((c>>sh)&mask)*a9+((d>>sh)&mask)*(256-a9)+128)>>8)<<sh) for sh,mask in [(11,31),(5,63),(0,31)])
 # Context REC sets only DSTR and BASE without pushing; table then ends band.
 words=[0x30000000,0x10000,0x00000400|2|512,2560,0x10000,0x60000000|(alpha<<16)|n,0x200000,0x40000000]
 with(out/'input.hex').open('w') as f:
  for addr,data in [(0x200000,objects),(0x280000,words)]:f.write(f'@{addr//4:x}\n'+''.join(f'{v:08x}\n' for v in data))
 (out/'expected.hex').write_text(''.join(f'{pixels[i]|pixels[i+1]<<16:08x}\n' for i in range(0,len(pixels),2)))
 t=time.monotonic();log=run(['C:/msys64/ucrt64/bin/vvp.exe',out/'test.vvp',f'+input={out/"input.hex"}',f'+expected={out/"expected.hex"}',f'+end={0x280000+len(words)*4}'])
 (out/f'case{case}.log').write_text(log);entry=dict(count=n,alpha=alpha,seconds=round(time.monotonic()-t,2),log=log.strip());results.append(entry);print(entry,flush=True)
(out/'results.json').write_text(json.dumps(results,indent=2))
