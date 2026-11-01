// R5 input page at 0xF8100080. All state in 200 MHz system clock domain.
// Matrix rows are open-drain: OUT=0, OE=onehot, inactive rows high impedance.
module input_hub #(
 parameter CLK_HZ=200000000, parameter BAUD=115200,
 parameter ROW_CYCLES=CLK_HZ/4000, parameter DEBOUNCE_FRAMES=8
)(input wire clk,input wire rst,
 input wire [3:0] matrix_col, output wire [3:0] matrix_row_out,
 output wire [3:0] matrix_row_oe,input wire uart_rx,output wire uart_tx,
 input wire [4:0] paddr,input wire psel,input wire penable,input wire pwrite,
 input wire [31:0] pwdata,output reg [31:0] prdata);
 localparam BIT_CYCLES=(CLK_HZ+BAUD/2)/BAUD;
 (* async_reg="true" *) reg [3:0] col_s1,col_s2;
 (* async_reg="true" *) reg [1:0] rx_s;
 reg [15:0] scan_counter;
 reg [1:0] row;
 reg [15:0] sample,candidate,keys;
 reg [3:0] stable;
 wire [15:0] complete_sample={~col_s2,sample[11:0]};
 assign matrix_row_out=4'b0000;
 assign matrix_row_oe=rst ? 4'b0000 : 4'b0001<<row;
 always @(posedge clk) begin
   if(rst) begin col_s1<=15;col_s2<=15;rx_s<=3;end
   else begin col_s1<=matrix_col;col_s2<=col_s1;rx_s<={rx_s[0],uart_rx};end
 end
 always @(posedge clk) begin
   if(rst) begin scan_counter<=0;row<=0;sample<=0;candidate<=0;keys<=0;stable<=0;end
   else if(scan_counter==ROW_CYCLES-1) begin
     scan_counter<=0;row<=row+1'b1;
     case(row)
       0:sample[3:0]<=~col_s2;
       1:sample[7:4]<=~col_s2;
       2:sample[11:8]<=~col_s2;
       3:begin
         sample[15:12]<=~col_s2;
         if(complete_sample!=candidate) begin candidate<=complete_sample;stable<=1;end
         else if(stable<DEBOUNCE_FRAMES) begin
           stable<=stable+1'b1;
           if(stable==DEBOUNCE_FRAMES-1) keys<=candidate;
         end
       end
     endcase
   end else scan_counter<=scan_counter+1'b1;
 end
 // UART 8N1, midpoint sampling. Stop-bit error and FIFO overflow are sticky.
 reg [11:0] rx_timer;
 reg [3:0] rx_state;
 reg [7:0] rx_shift,rx_byte;
 reg rx_push,rx_bad;
 always @(posedge clk) begin
   if(rst) begin rx_timer<=0;rx_state<=0;rx_shift<=0;rx_byte<=0;rx_push<=0;rx_bad<=0;end
   else begin
     rx_push<=0;rx_bad<=0;
     if(rx_state==0) begin
       if(!rx_s[1]) begin rx_state<=1;rx_timer<=BIT_CYCLES/2-1;end
     end else if(rx_timer!=0) rx_timer<=rx_timer-1'b1;
     else begin
       rx_timer<=BIT_CYCLES-1;
       if(rx_state==1) begin if(!rx_s[1])rx_state<=2;else rx_state<=0;end
       else if(rx_state<10) begin rx_shift<={rx_s[1],rx_shift[7:1]};rx_state<=rx_state+1'b1;end
       else begin rx_state<=0;if(rx_s[1])begin rx_byte<=rx_shift;rx_push<=1;end else rx_bad<=1;end
     end
   end
 end
 reg [7:0] rx_fifo[0:63];
 reg [5:0] rp,wp;
 reg [6:0] count;
 reg error;
 wire access=psel&&penable;
 wire pop=access&&!pwrite&&paddr[4:2]==3&&count!=0;
 wire push=rx_push&&(count!=64||pop);
 wire clear=access&&pwrite&&paddr[4:2]==5&&pwdata[0];
 always @(posedge clk) begin
   if(push)rx_fifo[wp]<=rx_byte;
   if(rst||clear)begin rp<=0;wp<=0;count<=0;error<=0;end
   else begin
     if(push)wp<=wp+1'b1;
     if(pop)rp<=rp+1'b1;
     case({push,pop})2'b10:count<=count+1'b1;2'b01:count<=count-1'b1;default:;endcase
     if(rx_bad||(rx_push&&!push))error<=1;
   end
 end
 reg [9:0] tx_shift;
 reg [3:0] tx_left;
 reg [11:0] tx_timer;
 assign uart_tx=tx_left!=0 ? tx_shift[0] : 1'b1;
 always @(posedge clk) begin
   if(rst)begin tx_left<=0;tx_shift<=10'h3ff;tx_timer<=0;end
   else if(tx_left!=0)begin
     if(tx_timer!=0)tx_timer<=tx_timer-1'b1;
     else begin tx_timer<=BIT_CYCLES-1;tx_shift<={1'b1,tx_shift[9:1]};tx_left<=tx_left-1'b1;end
   end else if(access&&pwrite&&paddr[4:2]==4)begin
     tx_shift<={1'b1,pwdata[7:0],1'b0};tx_left<=10;tx_timer<=BIT_CYCLES-1;
   end
 end
 always @* begin
   case(paddr[4:2])
     0:prdata=32'h494e5035; // INP5
     1:prdata={16'd0,keys};
     2:prdata={16'd0,1'b0,count,4'd0,tx_left!=0,error,count==64,count!=0};
     3:prdata=count!=0 ? {24'd0,rx_fifo[rp]} : 0;
     4:prdata=BAUD;
     5:prdata={31'd0,error};
     default:prdata=0;
   endcase
 end
endmodule
