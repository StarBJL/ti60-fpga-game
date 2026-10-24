`timescale 1ns/1ps
module input_hub_tb;
  localparam BIT_CYCLES=16,ROW_CYCLES=8,FRAME_CYCLES=32;
  reg clk=0;always #5 clk=~clk;
  reg rst=1;reg[15:0] pressed=0;reg[3:0] cols;
  wire[3:0] row_out,row_oe;reg rx=1;wire tx;
  reg[4:0] addr=0;reg sel=0,en=0,wr=0;reg[31:0] wdata=0;wire[31:0] rdata;
  reg[31:0] value;integer i,j;reg[7:0] sampled;integer checks=0;
  input_hub #(.CLK_HZ(1600000),.BAUD(100000),.ROW_CYCLES(ROW_CYCLES),.DEBOUNCE_FRAMES(8))
    dut(.clk(clk),.rst(rst),.matrix_col(cols),.matrix_row_out(row_out),.matrix_row_oe(row_oe),
      .uart_rx(rx),.uart_tx(tx),.paddr(addr),.psel(sel),.penable(en),.pwrite(wr),.pwdata(wdata),.prdata(rdata));
  always @* begin
    cols=15;
    if(row_oe[0])cols=cols&~pressed[3:0];
    if(row_oe[1])cols=cols&~pressed[7:4];
    if(row_oe[2])cols=cols&~pressed[11:8];
    if(row_oe[3])cols=cols&~pressed[15:12];
  end
  always @(negedge clk) if(!rst)begin
    if(row_out!==0)$fatal(1,"matrix row must only drive zero");
    if(row_oe==0 || (row_oe&(row_oe-1))!=0)$fatal(1,"matrix row OE not onehot");
    if(dut.count>64)$fatal(1,"RX FIFO count overflow");
  end
  task ticks(input integer n);begin repeat(n)@(negedge clk);end endtask
  task read_apb(input[4:0] a,output[31:0] v);begin
    @(negedge clk);addr=a;sel=1;en=0;wr=0;
    @(negedge clk);en=1;#1;v=rdata;
    @(negedge clk);sel=0;en=0;
  end endtask
  task write_apb(input[4:0] a,input[31:0] v);begin
    @(negedge clk);addr=a;sel=1;en=0;wr=1;wdata=v;
    @(negedge clk);en=1;
    @(negedge clk);sel=0;en=0;wr=0;
  end endtask
  task uart_send(input[7:0] b,input bad_stop);integer k;begin
    @(negedge clk);rx=0;ticks(BIT_CYCLES);
    for(k=0;k<8;k=k+1)begin rx=b[k];ticks(BIT_CYCLES);end
    rx=!bad_stop;ticks(BIT_CYCLES);rx=1;ticks(BIT_CYCLES*2);
  end endtask
  task expect_status(input[6:0] n,input err);begin
    read_apb(8,value);checks=checks+1;
    if(value[14:8]!==n||value[2]!==err||value[0]!==(n!=0)||value[1]!==(n==64))
      $fatal(1,"RX status expected count=%d err=%d got %h",n,err,value);
  end endtask
  initial begin
    ticks(5);rst=0;ticks(4);
    read_apb(0,value);if(value!==32'h494e5035)$fatal(1,"ID");
    read_apb(4,value);if(value!==0)$fatal(1,"reset matrix");expect_status(0,0);
    // A full candidate starts at stable=1; six more frames must not publish.
    pressed=16'h8402;wait(dut.candidate==16'h8402);ticks(6*FRAME_CYCLES);
    read_apb(4,value);if(value!==0)$fatal(1,"debounce published before eight frames");
    ticks(2*FRAME_CYCLES);read_apb(4,value);if(value!==16'h8402)$fatal(1,"debounced press");
    // Bounce for one complete scan and return: old stable key state persists.
    pressed=0;ticks(FRAME_CYCLES*2);pressed=16'h8402;ticks(FRAME_CYCLES*2);
    read_apb(4,value);if(value!==16'h8402)$fatal(1,"bounce changed stable state");
    pressed=0;ticks(FRAME_CYCLES*10);read_apb(4,value);if(value!==0)$fatal(1,"debounced release");
    // False start shorter than half a bit never publishes.
    rx=0;ticks(3);rx=1;ticks(BIT_CYCLES*12);expect_status(0,0);
    // Exercise both pointer wraps without overflowing.
    for(i=0;i<100;i=i+1)begin
      uart_send(i[7:0]^8'ha5,0);expect_status(1,0);read_apb(12,value);
      if(value[7:0]!== (i[7:0]^8'ha5))$fatal(1,"RX byte order/wrap %d",i);
    end
    expect_status(0,0);read_apb(12,value);if(value!==0)$fatal(1,"empty read");expect_status(0,0);
    uart_send(8'h5a,1);expect_status(0,1);write_apb(20,1);expect_status(0,0);
    // Exact 64-byte capacity, overflow sticky, existing bytes preserved.
    for(i=0;i<64;i=i+1)uart_send(i[7:0],0);
    expect_status(64,0);uart_send(8'hee,0);expect_status(64,1);
    for(i=0;i<64;i=i+1)begin read_apb(12,value);if(value[7:0]!==i[7:0])$fatal(1,"overflow corrupts FIFO");end
    expect_status(0,1);write_apb(20,1);expect_status(0,0);
    // Clear also deliberately discards queued stale bytes.
    uart_send(8'h11,0);uart_send(8'h22,0);write_apb(20,1);expect_status(0,0);
    // Capture a transmit frame independently at bit midpoints.
    fork
      begin
        @(negedge tx);ticks(BIT_CYCLES/2);if(tx!==0)$fatal(1,"TX start");
        for(j=0;j<8;j=j+1)begin ticks(BIT_CYCLES);sampled[j]=tx;end
        ticks(BIT_CYCLES);if(tx!==1)$fatal(1,"TX stop");
        if(sampled!==8'ha6)$fatal(1,"TX bit order %h",sampled);
      end
      begin write_apb(16,8'ha6);read_apb(8,value);if(!value[3])$fatal(1,"TX busy");end
    join
    ticks(BIT_CYCLES*2);read_apb(8,value);if(value[3])$fatal(1,"TX busy did not clear");
    if(tx!==1)$fatal(1,"TX idle");
    $display("PASS input_hub: 8-frame debounce, bounce/release, onehot open-drain rows, UART 8N1, FIFO wrap/overflow/clear, framing and TX (%0d status checks)",checks);
    $finish;
  end
  initial begin #10000000;$fatal(1,"timeout");end
endmodule
