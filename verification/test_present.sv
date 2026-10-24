`timescale 1ns/1ps
module test;
 reg clk=0,rst=1;always #2.5 clk=~clk;
 reg [15:0] apb_paddr=0;reg apb_psel=0,apb_penable=0,apb_pwrite=0;reg [31:0]apb_pwdata=0;
 wire[31:0]apb_prdata;
 hdmi_fb_top dut(.clk(clk),.rst(rst),.axi_rst(rst),.pix_clk(clk),.pix_rst_n(~rst),
 .apb_paddr(apb_paddr),.apb_psel(apb_psel),.apb_penable(apb_penable),.apb_pwrite(apb_pwrite),.apb_pwdata(apb_pwdata),.apb_prdata(apb_prdata),
 .axi_arready(1'b0),.axi_rvalid(1'b0),.axi_rdata(32'd0),.axi_rlast(1'b0),.axi_rresp(2'b0),.axi_rid(4'b0),
 .aud_wvalid(1'b0),.aud_wdata(32'd0));
 task wr(input[11:0] a,input[31:0]d);begin
 @(negedge clk);apb_paddr=a;apb_pwdata=d;apb_psel=1;apb_penable=1;apb_pwrite=1;
 @(negedge clk);apb_psel=0;apb_penable=0;apb_pwrite=0;
 end endtask
 task scan;begin @(negedge clk);force dut.vs_fall=1;@(negedge clk);release dut.vs_fall;end endtask
 task ck(input[11:0]a,input[31:0]expected);begin
 @(negedge clk);apb_paddr=a;#0.1;if(apb_prdata!==expected)$fatal(1,"reg %h got %h expected %h",a,apb_prdata,expected);
 end endtask
 initial begin
 repeat(6)@(negedge clk);rst=0;force dut.vs_fall=0;
 ck(12'h4c,32'h50525332);ck(12'h20,32'h33);
 wr(0,1);wr(4,32'h1200000);scan();force dut.vs_fall=0;
 ck(12'h40,1);ck(12'h44,0);ck(12'h48,1);
 scan();force dut.vs_fall=0;ck(12'h44,1);ck(12'h48,2);
 wr(4,32'h1400000);scan();force dut.vs_fall=0;ck(12'h40,2);ck(12'h48,3);
 wr(0,3);scan();force dut.vs_fall=0;ck(12'h48,3);
 wr(12'h20,0);ck(12'h20,0);ck(12'h4c,32'h50525332);
 $display("PASS PRS2 new/repeat/scan count; test-pattern excluded; audio ABI retained");$finish;
 end
endmodule
