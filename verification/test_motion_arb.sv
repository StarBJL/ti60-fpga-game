`timescale 1ns/1ps
module test;
 reg clk=0,rst=1;always #2.5 clk=~clk;
 reg [1:0]u_arvalid=0,u_rready=0,u_awvalid=0,u_wvalid=0,u_wlast=0,u_bready=0;
 reg [63:0]u_araddr=0,u_awaddr=0;reg[15:0]u_arlen=0,u_awlen=0;
 reg[255:0]u_wdata=0;reg[31:0]u_wstrb=0;
 wire[1:0]u_arready,u_rvalid,u_awready,u_wready,u_bvalid,u_rresp,u_bresp;
 wire[127:0]u_rdata;wire u_rlast;
 wire arvalid,arready,awvalid,awready,wvalid,wready,wlast,rvalid,rready,rlast,bready;
 wire[31:0]araddr,awaddr;wire[7:0]arlen,awlen;wire[3:0]arid,awid;
 wire[127:0]wdata,rdata;wire[15:0]wstrb;wire[1:0]rresp;
 reg bvalid=0;reg[1:0]bresp=0;reg[3:0]bid=0;
 integer cycle=0,read_beats=0,read_idx=0,write_beats=0,read_total=0,write_total=0;
 reg[31:0]read_addr=0,write_addr=0;reg[3:0]read_id=0,write_id=0;
 reg aw_seen=0,w_seen=0;integer write_len=0;
 reg[127:0]saved_data[0:15];reg[15:0]saved_strb[0:15];
 motion_axi_arb dut(.*,.rid(read_id));
 function automatic[127:0]pattern(input[31:0]a,input integer i);
   pattern={a,32'h12345678^a,32'hfedcba98^i,32'h87654321+i};
 endfunction
 assign arready=read_beats==0 && cycle%5!=0;
 assign rvalid=read_beats!=0;assign rlast=read_beats==1;
 assign rdata=pattern(read_addr,read_idx);assign rresp=read_addr[8]?2'b10:2'b00;
 assign awready=!aw_seen&&!bvalid&&cycle%7!=0;
 assign wready=!w_seen&&!bvalid&&cycle%3!=0;
 integer k;
 always@(posedge clk)if(!rst)begin
   cycle<=cycle+1;
   if(arvalid&&arready)begin
     if(arid>1)$fatal(1,"read ID");read_id<=arid;read_addr<=araddr;
     read_beats<=arlen+1;read_idx<=0;read_total<=read_total+1;
   end
   if(rvalid&&rready)begin read_beats<=read_beats-1;read_idx<=read_idx+1;end
   if(awvalid&&awready)begin
     if(awid>1)$fatal(1,"write ID");
     aw_seen<=1;write_addr<=awaddr;write_id<=awid;write_len<=awlen+1;
   end
   if(wvalid&&wready)begin
     saved_data[write_beats]<=wdata;saved_strb[write_beats]<=wstrb;
     write_beats<=write_beats+1;if(wlast)w_seen<=1;
   end
   if(aw_seen&&w_seen&&!bvalid)begin
     if(write_beats!=write_len)$fatal(1,"AW/W burst length mismatch");
     for(k=0;k<write_beats;k=k+1)
       if(saved_data[k]!==pattern(write_addr,k)||saved_strb[k]!==16'hffff)
         $fatal(1,"write ownership/data mismatch addr=%h beat=%0d",write_addr,k);
     bvalid<=1;bid<=write_id;bresp<=write_addr[8]?2'b10:2'b00;
     write_total<=write_total+1;
   end
   if(bvalid&&bready)begin bvalid<=0;aw_seen<=0;w_seen<=0;write_beats<=0;end
 end
 // A stall must not change the downstream owner or any payload field.
 reg ar_stall=0,aw_stall=0,w_stall=0;
 reg[43:0]ar_hold,aw_hold;reg[144:0]w_hold;
 always@(posedge clk)if(!rst)begin
   if(ar_stall&&(!arvalid||{arid,arlen,araddr}!==ar_hold))$fatal(1,"AR changed under stall");
   if(aw_stall&&(!awvalid||{awid,awlen,awaddr}!==aw_hold))$fatal(1,"AW changed under stall");
   if(w_stall&&(!wvalid||{wlast,wstrb,wdata}!==w_hold))$fatal(1,"W changed under stall");
   ar_stall<=arvalid&&!arready;ar_hold<={arid,arlen,araddr};
   aw_stall<=awvalid&&!awready;aw_hold<={awid,awlen,awaddr};
   w_stall<=wvalid&&!wready;w_hold<={wlast,wstrb,wdata};
 end
 task automatic master_write(input integer p);
   integer j,b,n;reg[31:0]a;
   begin for(j=0;j<48;j=j+1)begin
     a=32'h1000+p*65536+j*256;n=1+j%16;
     fork
       begin
         repeat(j%2?0:12)@(negedge clk);
         @(negedge clk);u_awaddr[p*32+:32]=a;u_awlen[p*8+:8]=n-1;u_awvalid[p]=1;
         @(posedge clk);while(!u_awready[p])@(posedge clk);
         @(negedge clk);u_awvalid[p]=0;
       end
       begin
         repeat(j%2?12:0)@(negedge clk);
         for(b=0;b<n;b=b+1)begin
           @(negedge clk);u_wvalid[p]=1;u_wdata[p*128+:128]=pattern(a,b);u_wstrb[p*16+:16]=16'hffff;u_wlast[p]=b==n-1;
           @(posedge clk);while(!u_wready[p])@(posedge clk);
         end
         @(negedge clk);u_wvalid[p]=0;u_wlast[p]=0;
       end
     join
     repeat(j%4)@(negedge clk);u_bready[p]=1;
     @(posedge clk);while(!u_bvalid[p])@(posedge clk);
     if(u_bresp!==(a[8]?2'b10:2'b00))$fatal(1,"B response routing");
     @(negedge clk);u_bready[p]=0;
   end end
 endtask
 task automatic master_read(input integer p);
   integer j,b,n;reg[31:0]a;
   begin for(j=0;j<48;j=j+1)begin
     a=32'h8000+p*65536+j*256;n=1+j%16;
     @(negedge clk);u_araddr[p*32+:32]=a;u_arlen[p*8+:8]=n-1;u_arvalid[p]=1;
     @(posedge clk);while(!u_arready[p])@(posedge clk);
     @(negedge clk);u_arvalid[p]=0;
     for(b=0;b<n;b=b+1)begin
       repeat((b+j)%4)@(negedge clk);
       u_rready[p]=1;@(posedge clk);while(!u_rvalid[p])@(posedge clk);
       if(u_rdata!==pattern(a,b)||u_rlast!==(b==n-1)||u_rresp!==(a[8]?2'b10:2'b00))$fatal(1,"R routing/data");
       @(negedge clk);u_rready[p]=0;
     end
   end end
 endtask
 initial begin
   repeat(5)@(negedge clk);rst=0;
   fork master_write(0);master_write(1);master_read(0);master_read(1);join
   repeat(4)@(negedge clk);
   if(read_total!=96||write_total!=96)$fatal(1,"transaction count");
   $display("PASS arbiter: 96 read + 96 write bursts; two masters, 1..16 beats, AW/W ordering, stalls, IDs and error responses");$finish;
 end
 initial begin #1000000;$fatal(1,"watchdog");end
endmodule
