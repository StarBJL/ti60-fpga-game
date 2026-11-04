`timescale 1ns/1ps
// Directed AXI read arbiter test: priority, AR backpressure lock, RID routing.
module tb_blit_axi_rd_arb;
  reg clk=0, rst=1;
  always #5 clk=~clk;
  reg m0_arvalid=0, m1_arvalid=0, s_arready=0;
  reg [31:0] m0_araddr=32'h1000, m1_araddr=32'h2000;
  wire m0_arready, m1_arready, s_arvalid, s_rready;
  wire [31:0] s_araddr;
  wire [7:0] s_arlen;
  wire [2:0] s_arsize;
  wire [1:0] s_arburst;
  wire [3:0] s_arid;
  reg s_rvalid=0, s_rlast=0;
  reg [31:0] s_rdata=0;
  reg [1:0] s_rresp=0;
  reg [3:0] s_rid=0;
  wire m0_rvalid, m1_rvalid;
  wire [31:0] m0_rdata, m1_rdata;
  wire m0_rlast, m1_rlast;
  wire [1:0] m0_rresp, m1_rresp;
  wire [3:0] m0_rid, m1_rid;

  blit_axi_rd_arb dut (
    .clk(clk), .rst(rst),
    .m0_arvalid(m0_arvalid), .m0_arready(m0_arready),
    .m0_araddr(m0_araddr), .m0_arlen(8'd3),
    .m0_arsize(3'd2), .m0_arburst(2'd1),
    .m0_rvalid(m0_rvalid), .m0_rdata(m0_rdata),
    .m0_rlast(m0_rlast), .m0_rresp(m0_rresp), .m0_rid(m0_rid),
    .m1_arvalid(m1_arvalid), .m1_arready(m1_arready),
    .m1_araddr(m1_araddr), .m1_arlen(8'd7),
    .m1_arsize(3'd2), .m1_arburst(2'd1),
    .m1_rvalid(m1_rvalid), .m1_rdata(m1_rdata),
    .m1_rlast(m1_rlast), .m1_rresp(m1_rresp), .m1_rid(m1_rid),
    .s_arvalid(s_arvalid), .s_arready(s_arready),
    .s_araddr(s_araddr), .s_arlen(s_arlen),
    .s_arsize(s_arsize), .s_arburst(s_arburst), .s_arid(s_arid),
    .s_rvalid(s_rvalid), .s_rready(s_rready),
    .s_rdata(s_rdata), .s_rlast(s_rlast),
    .s_rresp(s_rresp), .s_rid(s_rid)
  );

  task check;
    input cond;
    input [8*80-1:0] label;
    begin
      if (cond !== 1'b1) begin
        $display("FAIL: %0s at t=%0t", label, $time);
        $fatal(1);
      end
    end
  endtask

  initial begin
    repeat(2) @(negedge clk);
    rst=0;
    m0_arvalid=1; m1_arvalid=1; s_arready=1;
    #1;
    check(s_arvalid && s_arid==4'd0 && s_araddr==32'h1000
          && m0_arready && !m1_arready, "HDMI priority");
    @(posedge clk); #1;
    @(negedge clk);
    m0_arvalid=0; m1_arvalid=1; s_arready=0;
    #1; check(s_araddr==32'h2000 && s_arid==4'd1, "initial m1");
    @(posedge clk); #1;
    @(negedge clk);
    m0_arvalid=1;
    #1; check(s_arvalid && s_araddr==32'h2000 && s_arid==4'd1
              && !m0_arready, "AR lock under backpressure");
    s_arready=1;
    #1; check(m1_arready && !m0_arready, "m1 handshake");
    @(posedge clk); #1;
    @(negedge clk);
    m1_arvalid=0;
    #1; check(s_araddr==32'h1000 && s_arid==4'd0, "unlock to m0");
    s_rvalid=1; s_rdata=32'h12345678; s_rresp=2'd0;
    s_rlast=1; s_rid=4'd0;
    #1; check(s_rready && m0_rvalid && !m1_rvalid
              && m0_rdata==32'h12345678 && m0_rlast, "RID0 demux");
    s_rid=4'd1; s_rdata=32'h89abcdef;
    #1; check(!m0_rvalid && m1_rvalid
              && m1_rdata==32'h89abcdef && m1_rlast
              && m1_rid==4'd1, "RID1 demux");
    s_rvalid=0;
    #1; check(!m0_rvalid && !m1_rvalid && s_rready, "idle R");
    $display("PASS: blit_axi_rd_arb priority / lock / RID route");
    $finish;
  end
endmodule
