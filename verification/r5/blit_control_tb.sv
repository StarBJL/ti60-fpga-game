`timescale 1ns/1ps
// Portable behavioral test. No AMD device mapping is used as Ti60 evidence.
module blit_fifo_check #(parameter DW=23, AW=4)(output reg done);
  reg clk=0, rst=1, push=0, pop=0;
  reg [DW-1:0] din=0;
  wire full, valid;
  wire [AW:0] space;
  wire [AW+1:0] count;
  wire [DW-1:0] dout;
  reg [DW-1:0] model[0:100000];
  integer wr=0, rd=0, n=0, cy=0, seed=32'h52500000+AW;
  integer full_hits=0, empty_hits=0, simultaneous=0;
  reg take_in, take_out;
  blit_rfifo #(.DW(DW),.AW(AW)) dut(.*);
  always #5 clk=~clk;
  initial begin
    done=0;
    repeat(3) @(negedge clk);
    rst=0;
    for(cy=0;cy<12000;cy=cy+1) begin
      // Fill, drain and then randomized stalls including simultaneous R/W.
      if(cy<100) begin push=1; pop=0; end
      else if(cy<200) begin push=0; pop=1; end
      else begin push=($random(seed)&3)!=0; pop=($random(seed)&3)!=0; end
      din=$random(seed);
      #1;
      if(full) full_hits=full_hits+1;
      if(!valid) empty_hits=empty_hits+1;
      if(valid && dout!==model[rd]) $fatal(1,"FIFO data AW=%0d cycle=%0d",AW,cy);
      take_in=push&&!full; take_out=pop&&valid;
      if(take_in&&take_out) simultaneous=simultaneous+1;
      if(take_in) begin model[wr]=din; wr=wr+1; end
      if(take_out) rd=rd+1;
      n=n+take_in-take_out;
      @(posedge clk); #1;
      if(count!==n) $fatal(1,"FIFO count AW=%0d got=%0d expected=%0d",AW,count,n);
      if(count>(1<<AW)+1) $fatal(1,"FIFO overflow");
      @(negedge clk);
    end
    push=0; pop=1;
    while(n>0) begin
      #1;
      if(valid) begin
        if(dout!==model[rd]) $fatal(1,"FIFO drain data");
        rd=rd+1; n=n-1;
      end
      @(posedge clk); #1;
      if(count!==n) $fatal(1,"FIFO drain count");
      @(negedge clk);
    end
    if(full_hits==0 || empty_hits==0 || simultaneous<100) $fatal(1,"coverage missing");
    $display("FIFO_PASS DW=%0d AW=%0d cycles=%0d full=%0d empty=%0d simultaneous=%0d",DW,AW,cy,full_hits,empty_hits,simultaneous);
    done=1;
  end
endmodule

module blit_control_tb;
  wire done_bd, done_lf;
  blit_fifo_check #(.DW(23),.AW(4)) bd(done_bd);
  blit_fifo_check #(.DW(32),.AW(5)) lf(done_lf);
  initial begin
    wait(done_bd&&done_lf);
    $display("TEST_PASS: R5 descriptor/list register queues");
    $finish;
  end
  initial begin #200000; $fatal(1,"watchdog"); end
endmodule
