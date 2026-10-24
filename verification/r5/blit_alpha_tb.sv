`timescale 1ns/1ps
// Arithmetic-only check of the actual P2..P6 pipeline. Hierarchical force is
// intentionally limited to its documented input boundary; integrated AXI,
// ordering/key/edge checks are covered by PROJECT/sim/blit/tb_blit.py.
module blit_alpha_tb;
  reg clk=0, rst=1;
  reg [31:0] src=0,dst=0;
  reg [7:0] pa0=0,pa1=0;
  reg [8:0] ga9=0;
  reg apl=0, blend=0, add=0, valid=0;
  reg [31:0] expected[0:20003];
  integer i, ga, seed=32'h52414c50;
  blit_engine dut(.clk(clk),.rst(rst),.apb_psel(1'b0),.apb_penable(1'b0),
    .apb_pwrite(1'b0),.apb_paddr(12'd0),.apb_pwdata(32'd0),
    .m_arready(1'b0),.m_rvalid(1'b0),.m_rdata(32'd0),.m_rlast(1'b0),
    .m_awready(1'b0),.m_wready(1'b0),.m_bvalid(1'b0),
    .x_arready(1'b0),.x_rvalid(1'b0),.x_rdata(128'd0),.x_rlast(1'b0),
    .x_awready(1'b0),.x_wready(1'b0));
  always #5 clk=~clk;
  function automatic [15:0] golden(input [15:0] s,d,input integer pa,g,input bit ap,bl,ad);
    integer a,e,r,gr,b,inv;
    begin
      e=(pa*g)>>8;
      a=!bl?256:(ap?(e+(e>>7)):g);
      inv=ad?256:256-a;
      r=((((s>>11)&31)*a+((d>>11)&31)*inv+128)>>8);
      gr=((((s>>5)&63)*a+((d>>5)&63)*inv+128)>>8);
      b=(((s&31)*a+(d&31)*inv+128)>>8);
      if(r>31) r=31; if(gr>63) gr=63; if(b>31) b=31;
      golden=(r<<11)|(gr<<5)|b;
    end
  endfunction
  initial begin
    force dut.p2_s_lo=src[15:0]; force dut.p2_s_hi=src[31:16];
    force dut.p2_d=dst; force dut.p2_pa_lo=pa0; force dut.p2_pa_hi=pa1;
    force dut.p2_ga9=ga9; force dut.p2_apl=apl; force dut.p2_blend=blend;
    force dut.p2_add=add; force dut.p2_v=valid;
    force dut.p2_m_lo=1'b1; force dut.p2_m_hi=1'b1; force dut.p2_wq=14'd0;
    repeat(3) @(negedge clk);
    rst=0;
    for(i=0;i<20003;i=i+1) begin
      valid=i<20000;
      src=$random(seed); dst=$random(seed);
      // Every global alpha, endpoint A8 values and randomized pairs occur.
      ga=i%256; ga9=ga+(ga>>7);
      pa0=(i%7==0)?0:((i%7==1)?255:$random(seed));
      pa1=(i%5==0)?0:((i%5==1)?255:$random(seed));
      apl=(i>>8)&1; add=(i>>9)&1; blend=add|apl|((i%3)!=0);
      expected[i]={golden(src[31:16],dst[31:16],pa1,ga9,apl,blend,add),
                   golden(src[15:0],dst[15:0],pa0,ga9,apl,blend,add)};
      @(posedge clk); #1;
      if(i>=3 && (dut.p6_v!==1'b1 || dut.p6_data!==expected[i-3]))
        $fatal(1,"Alpha pipeline vector %0d got %h expected %h valid %b",i-3,dut.p6_data,expected[i-3],dut.p6_v);
      @(negedge clk);
    end
    $display("TEST_PASS: 20000 RGB565 pairs, global/A8 alpha, additive saturation, one pair/cycle");
    $finish;
  end
  initial begin #250000; $fatal(1,"watchdog"); end
endmodule
