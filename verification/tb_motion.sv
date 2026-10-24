`timescale 1ns/1ps
module tb_square_motion;
 reg clk=0,rst=1;always #2.7 clk=~clk;
 reg [11:0] paddr=0;reg psel=0,penable=0,pwrite=0;reg [31:0] pwdata=0;
 wire [31:0] prdata;wire pready;
 wire arvalid,arready,rvalid,rready,rlast,awvalid,awready,wvalid,wready,wlast,bready;
 wire [31:0] araddr,awaddr;wire [7:0] arlen,awlen;wire [127:0] rdata,wdata;wire [15:0] wstrb;
 reg bvalid=0;wire [1:0] bresp;
 reg [31:0] mem[0:98303],expected[0:98303];reg [15:0] hist[0:879];
 integer n,mode,fault=0,seed=7,cycles=0,ra=0,rl=0,ri=0,wa=0,wl=0,wi=0,delay_b=0;
 square_motion dut(.*,.rresp(fault==1?2'b10:2'b00));
 reg read_active=0,write_active=0,read_offer=0,allow_ar=0,allow_aw=0,allow_w=0;
 reg [1023:0] input_path,expected_path,hist_path,expected2_path,hist2_path;integer pass;
 assign arready=!read_active&&allow_ar;
 assign rvalid=read_active&&read_offer;assign rlast=ri==rl;
 assign rdata={mem[(ra>>2)+ri*4+3],mem[(ra>>2)+ri*4+2],mem[(ra>>2)+ri*4+1],mem[(ra>>2)+ri*4]};
 assign awready=!write_active&&!bvalid&&delay_b==0&&allow_aw;
 assign wready=write_active&&allow_w;assign bresp=fault==2?2'b10:2'b00;
 integer j,k,ar_count=0,b_count=0;
 always @(negedge clk)if(!rst)begin
  allow_ar<=$urandom_range(0,3)!=0;allow_aw<=$urandom_range(0,3)!=0;allow_w<=$urandom_range(0,3)!=0;
  // Once asserted, VALID and payload remain stable until READY.
  if(!read_offer||rready)read_offer<=$urandom_range(0,3)!=0;
 end
 always @(posedge clk)begin
  cycles<=cycles+1;
  if(arvalid&&arready)begin
   if(araddr[3:0]!=0||arlen>15||((araddr&4095)+(arlen+1)*16)>4096)$fatal(1,"bad read burst");
   read_active<=1;ra<=araddr;rl<=arlen;ri<=0;ar_count<=ar_count+1;
  end
  if(rvalid&&rready)begin if(rlast)read_active<=0;else ri<=ri+1;end
  if(awvalid&&awready)begin
   if(awaddr[3:0]!=0||awlen>15||((awaddr&4095)+(awlen+1)*16)>4096)$fatal(1,"bad write burst");
   write_active<=1;wa<=awaddr;wl<=awlen;wi<=0;
  end
  if(wvalid&&wready)begin
   if(wlast!=(wi==wl))$fatal(1,"bad WLAST");
   for(j=0;j<16;j=j+1)if(wstrb[j])mem[(wa>>2)+wi*4+(j>>2)][(j%4)*8+:8]<=wdata[j*8+:8];
   if(wlast)begin write_active<=0;delay_b<=17+$urandom_range(0,21);end else wi<=wi+1;
  end
  if(delay_b!=0)begin delay_b<=delay_b-1;if(delay_b==1)bvalid<=1;end
  if(bvalid&&bready)begin bvalid<=0;b_count<=b_count+1;end
  if(dut.done&&!dut.error&&!rst&&(read_active||write_active||bvalid||delay_b!=0))$fatal(1,"completed before AXI drained");
 end
 task wr(input[7:0] off,input[31:0] value);
 begin
  @(negedge clk);paddr={4'h3,off};psel=1;pwrite=1;pwdata=value;penable=0;
  @(negedge clk);penable=1;
  while(!pready)@(negedge clk);
  @(negedge clk);psel=0;penable=0;pwrite=0;
 end endtask
 task rd(input[7:0] off,output[31:0] value);
 begin
  @(negedge clk);paddr={4'h3,off};psel=1;pwrite=0;penable=0;
  @(negedge clk);penable=1;#0.1;
  while(!pready)begin @(negedge clk);#0.1;end
  value=prdata;
  @(negedge clk);psel=0;penable=0;
 end endtask
 reg [31:0] value;integer refs_sum=0,peak_value=0;
 initial begin
  if(!$value$plusargs("input=%s",input_path)||!$value$plusargs("expected=%s",expected_path)||!$value$plusargs("hist=%s",hist_path)||!$value$plusargs("count=%d",n))$fatal(1,"missing arguments");
  if($value$plusargs("mode=%d",mode))begin end else mode=1;
  if($value$plusargs("fault=%d",fault))begin end
  if($value$plusargs("seed=%d",seed))begin end
  if(!$value$plusargs("expected2=%s",expected2_path)||!$value$plusargs("hist2=%s",hist2_path))$fatal(1,"missing repeat arguments");
  k=$urandom(seed);
  $readmemh(input_path,mem);$readmemh(expected_path,expected);$readmemh(hist_path,hist);
  repeat(8)@(negedge clk);rst=0;
  rd(0,value);if(value!==32'h4d4f5432)$fatal(1,"ID");
  rd(4,value);if(value!==32'h00020000)$fatal(1,"720p version");
  for(pass=0;pass<(fault?1:2);pass=pass+1)begin
  refs_sum=0;peak_value=0;
  wr(12,32'h10000);wr(16,n);wr(20,mode);
  rd(40,value); // one blocking APB transaction; no CPU polling loop
  if(fault!=0)begin if(!value[2])$fatal(1,"expected AXI error");end
  else begin
   if(value!==2)$fatal(1,"bad completion status %h",value);
   for(k=0;k<n*2+4;k=k+1)if(mem[16384+k]!==expected[16384+k])$fatal(1,"object word %0d got %h expected %h",k,mem[16384+k],expected[16384+k]);
   wr(32,0);
   for(k=0;k<880;k=k+1)begin
    rd(36,value);if(value!==hist[k])$fatal(1,"hist tile %0d got %0d expected %0d",k,value,hist[k]);
    refs_sum=refs_sum+hist[k];if(hist[k]>peak_value)peak_value=hist[k];
   end
   rd(48,value);if(value!==refs_sum)$fatal(1,"refs sum");
   rd(52,value);if(value!==peak_value)$fatal(1,"peak");
  end
  $display("engine pass %0d cycles %0d",pass,dut.cycles);
  if(pass==0)begin $readmemh(expected2_path,expected);$readmemh(hist2_path,hist);end
  end
  // Invalid alignment must reject without issuing DMA.
  k=ar_count;wr(12,32'h10008);wr(20,1);rd(8,value);
  if(value!==6||ar_count!=k)$fatal(1,"invalid descriptor caused DMA");
  wr(12,32'hffffff00);wr(16,20000);wr(20,1);rd(8,value);
  if(value!==6||ar_count!=k)$fatal(1,"overflow descriptor caused DMA");
  wr(12,32'h10000);wr(16,30001);wr(20,1);rd(8,value);
  if(value!==6||ar_count!=k)$fatal(1,"over-cap descriptor caused DMA");
  $display("PASS MOT2 n=%0d mode=%0d fault=%0d cycles=%0d reads=%0d responses=%0d",n,mode,fault,cycles,ar_count,b_count);$finish;
 end
 initial begin #100000000;$fatal(1,"watchdog state %0d",dut.state);end
endmodule
