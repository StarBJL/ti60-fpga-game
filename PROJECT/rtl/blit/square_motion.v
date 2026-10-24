// MOT1: exact Q4 motion + complete Tile histogram. Objects stay in DDR.
// A 64x32 synchronous scratch RAM and 512x16 histogram map to three BRAM10s.
// One 256-byte burst at a time; completion waits for the final B response.
`timescale 1ns/1ps
module square_motion #(
 parameter WIDTH=1280, TOP=40, BOTTOM=676, MAX_COUNT=30000,
 parameter COLS=(WIDTH+31)/32, TILES=COLS*((BOTTOM+31)/32)
)( 
 input wire clk,rst,
 input wire [11:0] paddr,input wire psel,penable,pwrite,
 input wire [31:0] pwdata,output reg [31:0] prdata,output wire pready,
 output wire arvalid,input wire arready,output wire [31:0] araddr,output wire [7:0] arlen,
 input wire rvalid,output wire rready,input wire [127:0] rdata,input wire rlast,input wire [1:0] rresp,
 output wire awvalid,input wire awready,output wire [31:0] awaddr,output wire [7:0] awlen,
 output wire wvalid,input wire wready,output wire [127:0] wdata,output wire [15:0] wstrb,output wire wlast,
 input wire bvalid,output wire bready,input wire [1:0] bresp
);
 localparam IDLE=0,CLEAR=1,AR=2,RX=3,STORE=4,RDXY=5,GETXY=6,GETM=7,
  MOVE=8,BOUNCE=9,WXY=10,WM=11,GEO=12,HREAD=13,HWRITE=14,
  AW=15,TREAD=16,T0=17,T1=18,T2=19,T3=20,WSEND=21,BRESP=22,FAIL=23;
 reg [4:0] state;
 reg busy,done,error,advance,bad_rx;
 reg [31:0] base,cycles,sequence_id,error_addr,total_refs;
 reg [15:0] count,left,peak;
 reg [31:0] chunk_addr;
 reg [5:0] chunk_n,obj;
 reg [3:0] burst_last,rx_beat,tx_beat;
 reg [1:0] lane;
 reg [127:0] rx_hold,tx_hold;
 reg [9:0] clear_index,hist_index,tile,row_tile;
 reg [1:0] col,cols,row,rows;
 reg [31:0] xy,motion,next_xy,next_motion;
 reg signed [17:0] xraw,yraw;
 reg [15:0] maxx,maxy;
 reg [7:0] size;
 reg [31:0] scratch[0:63];
 reg [15:0] histogram[0:1023];
 reg [31:0] scratch_q;
 reg [15:0] hist_q;
 reg [5:0] scratch_addr;
 reg scratch_we;
 reg [31:0] scratch_wdata;
 wire [9:0] hist_addr = busy ? (state==CLEAR?clear_index:tile) : hist_index;
 wire hist_we=busy&&(state==CLEAR||state==HWRITE);
 wire [15:0] hist_wdata=state==CLEAR?16'd0:hist_q+16'd1;
 wire access=psel&&penable&&pready;
 wire watchdog=busy&&(&cycles[23:0]);
 reg timed_out;
 wire [31:0] status={28'd0,timed_out,error,done,busy};
 assign pready=!(psel&&!pwrite&&paddr[7:0]==8'h28&&busy&&!timed_out);
 always @* begin
  prdata=0;
  case(paddr[7:0])
   8'h00:prdata=32'h4d4f5432;8'h04:prdata=32'h00020000;
   8'h08,8'h28:prdata=status;8'h0c:prdata=base;8'h10:prdata={16'd0,count};
   8'h18:prdata=cycles;8'h1c:prdata=sequence_id;8'h20:prdata={22'd0,hist_index};
   8'h24:prdata={16'd0,hist_q};8'h2c:prdata=error_addr;
   8'h30:prdata=total_refs;8'h34:prdata={16'd0,peak};
   8'h38:prdata=WIDTH;8'h3c:prdata=(BOTTOM<<16)|TOP;8'h40:prdata=MAX_COUNT;
  endcase
 end
 always @* begin
  scratch_addr=0;scratch_we=0;scratch_wdata=0;
  case(state)
   STORE:begin scratch_addr={rx_beat,lane};scratch_we=1;scratch_wdata=rx_hold[lane*32+:32];end
   RDXY: scratch_addr={obj[4:0],1'b0};
   GETXY: scratch_addr={obj[4:0],1'b1};
   WXY:begin scratch_addr={obj[4:0],1'b0};scratch_we=1;scratch_wdata=next_xy;end
   WM:begin scratch_addr={obj[4:0],1'b1};scratch_we=1;scratch_wdata=next_motion;end
   TREAD:scratch_addr={tx_beat,2'd0};
   T0:scratch_addr={tx_beat,2'd1};
   T1:scratch_addr={tx_beat,2'd2};
   T2:scratch_addr={tx_beat,2'd3};
  endcase
 end
 always @(posedge clk) begin
  scratch_q<=scratch[scratch_addr];
  if(scratch_we)scratch[scratch_addr]<=scratch_wdata;
  hist_q<=histogram[hist_addr];
  if(hist_we)histogram[hist_addr]<=hist_wdata;
 end
 assign arvalid=state==AR;assign araddr=chunk_addr;assign arlen={4'd0,burst_last};
 assign rready=state==RX;
 assign awvalid=state==AW;assign awaddr=chunk_addr;assign awlen={4'd0,burst_last};
 assign wvalid=state==WSEND;assign wdata=tx_hold;
 assign wlast=tx_beat==burst_last;
 assign wstrb=(wlast&&chunk_n[0])?16'h00ff:16'hffff;
 assign bready=state==BRESP;
 // Reflection is evaluated in widened signed lanes: includes negative velocity.
 wire bx=xraw<0||xraw>$signed({2'b0,maxx});
 wire by=yraw<(TOP*16)||yraw>$signed({2'b0,maxy});
 wire [15:0] reflected_x=xraw<0?-xraw:(xraw>$signed({2'b0,maxx})?({maxx,1'b0}-xraw):xraw);
 wire [15:0] reflected_y=yraw<(TOP*16)?((TOP*32-yraw)):(yraw>$signed({2'b0,maxy})?({maxy,1'b0}-yraw):yraw);
 wire [15:0] q4_edge=({8'd0,size}-16'd1)<<4;
 wire [15:0] farx=next_xy[15:0]+q4_edge,fary=next_xy[31:16]+q4_edge;
 always @(posedge clk) begin
  if(rst)begin
   state<=IDLE;busy<=0;done<=0;error<=0;timed_out<=0;cycles<=0;sequence_id<=0;
   base<=0;count<=0;hist_index<=0;error_addr<=0;total_refs<=0;peak<=0;
   clear_index<=0;tile<=0;row_tile<=0;chunk_addr<=0;left<=0;chunk_n<=0;obj<=0;
   advance<=0;bad_rx<=0;rx_beat<=0;tx_beat<=0;burst_last<=0;lane<=0;
  end else begin
   if(busy)cycles<=cycles+1;
   if(watchdog)timed_out<=1;
   if(access&&pwrite&&!busy)case(paddr[7:0])
    8'h0c:base<=pwdata;
    8'h10:count<=pwdata[15:0];
    8'h20:hist_index<=pwdata[9:0];
    8'h14:if(pwdata[1:0]!=0)begin
     done<=0;error<=0;timed_out<=0;cycles<=0;total_refs<=0;peak<=0;error_addr<=0;
     if(base[7:0]!=0||base<32'h1000||base>=32'h00f00000||base+({16'd0,count}<<3)>32'h00f00000||count>MAX_COUNT)begin
      error<=1;done<=1;
     end else begin
      busy<=1;state<=CLEAR;clear_index<=0;left<=count;chunk_addr<=base;advance<=pwdata[0];
     end
    end
   endcase
   if(access&&!pwrite&&paddr[7:0]==8'h24&&!busy)hist_index<=hist_index+1;
   case(state)
    CLEAR:if(clear_index==TILES-1)begin
     if(left==0)begin busy<=0;done<=1;sequence_id<=sequence_id+1;state<=IDLE;end
     else begin chunk_n<=left>32?32:left[5:0];burst_last<=left>32?15:((left+1)>>1)-1;state<=AR;end
    end else clear_index<=clear_index+1;
    AR:if(arready)begin rx_beat<=0;bad_rx<=0;state<=RX;end
    RX:if(rvalid)begin
     rx_hold<=rdata;lane<=0;bad_rx<=bad_rx|(rresp!=0)|(rlast!=(rx_beat==burst_last));state<=STORE;
    end
    STORE:if(lane==3)begin
     if(rx_beat==burst_last)begin obj<=0;state<=bad_rx?FAIL:RDXY;end
     else begin rx_beat<=rx_beat+1;state<=RX;end
    end else lane<=lane+1;
    RDXY:state<=GETXY;
    GETXY:begin xy<=scratch_q;state<=GETM;end
    GETM:begin motion<=scratch_q;size<=scratch_q[23:16];state<=MOVE;end
    MOVE:begin
     if(size<6||size>40||motion[31:24]>=8||motion[7:0]==8'h80||motion[15:8]==8'h80||xy[15:0]>((WIDTH-size)<<4)||xy[31:16]<(TOP*16)||xy[31:16]>((BOTTOM-size)<<4))state<=FAIL;
     else begin
      xraw<=$signed({2'b0,xy[15:0]})+(advance?$signed(motion[7:0]):18'sd0);
      yraw<=$signed({2'b0,xy[31:16]})+(advance?$signed(motion[15:8]):18'sd0);
      maxx<=(WIDTH-size)<<4;maxy<=(BOTTOM-size)<<4;state<=BOUNCE;
     end
    end
    BOUNCE:begin
     next_xy<={reflected_y,reflected_x};
     next_motion<={motion[31:16],(by?-motion[15:8]:motion[15:8]),(bx?-motion[7:0]:motion[7:0])};state<=WXY;
    end
    WXY:state<=WM;
    WM:state<=GEO;
    GEO:begin
     tile<=(next_xy[31:25]*COLS)+next_xy[15:9];
     row_tile<=(next_xy[31:25]*COLS)+next_xy[15:9];
     cols<=farx[15:9]-next_xy[15:9];rows<=fary[13:9]-next_xy[29:25];col<=0;row<=0;state<=HREAD;
    end
    HREAD:state<=HWRITE;
    HWRITE:begin
     total_refs<=total_refs+1;if(hist_q+1>peak)peak<=hist_q+1;
     if(col!=cols)begin col<=col+1;tile<=tile+1;state<=HREAD;end
     else if(row!=rows)begin row<=row+1;col<=0;row_tile<=row_tile+COLS;tile<=row_tile+COLS;state<=HREAD;end
     else if(obj+1==chunk_n)begin tx_beat<=0;state<=AW;end
     else begin obj<=obj+1;state<=RDXY;end
    end
    AW:if(awready)state<=TREAD;
    TREAD:state<=T0;
    T0:begin tx_hold[31:0]<=scratch_q;state<=T1;end
    T1:begin tx_hold[63:32]<=scratch_q;state<=T2;end
    T2:begin tx_hold[95:64]<=scratch_q;state<=T3;end
    T3:begin tx_hold[127:96]<=scratch_q;state<=WSEND;end
    WSEND:if(wready)begin
     if(wlast)state<=BRESP;else begin tx_beat<=tx_beat+1;state<=TREAD;end
    end
    BRESP:if(bvalid)begin
     if(bresp!=0)state<=FAIL;
     else if(left==chunk_n)begin busy<=0;done<=1;sequence_id<=sequence_id+1;state<=IDLE;end
     else begin
      left<=left-chunk_n;chunk_addr<=chunk_addr+256;
      chunk_n<=left-chunk_n>32?32:left-chunk_n;
      burst_last<=left-chunk_n>32?15:((left-chunk_n+1)>>1)-1;state<=AR;
     end
    end
    FAIL:begin busy<=0;done<=1;error<=1;error_addr<=chunk_addr;state<=IDLE;end
   endcase
  end
 end
endmodule
