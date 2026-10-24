`timescale 1ns/1ps
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
 reg[31:0]mem[0:16777215],expected[0:460799];
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
 string input_path,expected_path;reg[31:0]q;integer list_end;
 initial begin
  if(!$value$plusargs("input=%s",input_path)||!$value$plusargs("expected=%s",expected_path)||!$value$plusargs("end=%d",list_end))$fatal(1,"args");
  for(k=0;k<16777216;k=k+1)mem[k]=0;
  $readmemh(input_path,mem);$readmemh(expected_path,expected);
  repeat(6)@(negedge clk);rst=0;
  apb(0,12'hbc,0,q);if((q&12)!=12)$fatal(1,"CAPS2 %h",q);
  apb(1,12'h94,(45<<16)|2560,q);apb(1,12'h98,104858,q);apb(1,12'h9c,32'h400014,q);
  apb(1,12'h38,32'h300000,q);apb(1,12'h3c,list_end,q);
  repeat(12)@(negedge clk);
  while(busy)@(negedge clk);
  apb(0,12'h04,0,q);if((q&220)!=0)$fatal(1,"blit status %h",q);
  for(k=0;k<460800;k=k+1)if(mem[16384+k]!==expected[k])$fatal(1,"pixel word %0d got %h expected %h",k,mem[16384+k],expected[k]);
  if(arp!=arr||awp!=awr||wp!=wrp||br!=bc)$fatal(1,"not drained");
  $display("PASS native 1280x720 SQTAB framebuffer exact, cycles %0d AR %0d AW %0d",cycles,arp,awp);$finish;
 end
endmodule
