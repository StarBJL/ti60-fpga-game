// Two 128-bit masters: port 0 existing graphics/HDMI, port 1 MOT1.
// Independent read IDs; write ownership spans BOTH AW and WLAST. This supports
// the existing graphics bridge's W-before-AW behavior without inserting stalls.
`timescale 1ns/1ps
module motion_axi_arb(
 input wire clk,rst,
 input wire [1:0] u_arvalid,output wire [1:0] u_arready,
 input wire [63:0] u_araddr,input wire [15:0] u_arlen,
 output wire [1:0] u_rvalid,input wire [1:0] u_rready,
 output wire [127:0] u_rdata,output wire u_rlast,output wire [1:0] u_rresp,
 input wire [1:0] u_awvalid,output wire [1:0] u_awready,
 input wire [63:0] u_awaddr,input wire [15:0] u_awlen,
 input wire [1:0] u_wvalid,output wire [1:0] u_wready,
 input wire [255:0] u_wdata,input wire [31:0] u_wstrb,input wire [1:0] u_wlast,
 output wire [1:0] u_bvalid,input wire [1:0] u_bready,output wire [1:0] u_bresp,
 output wire arvalid,input wire arready,output wire [31:0] araddr,
 output wire [7:0] arlen,output wire [3:0] arid,
 input wire rvalid,output wire rready,input wire [127:0] rdata,
 input wire rlast,input wire [1:0] rresp,input wire [3:0] rid,
 output wire awvalid,input wire awready,output wire [31:0] awaddr,
 output wire [7:0] awlen,output wire [3:0] awid,
 output wire wvalid,input wire wready,output wire [127:0] wdata,
 output wire [15:0] wstrb,output wire wlast,
 input wire bvalid,output wire bready,input wire [1:0] bresp,input wire [3:0] bid
);
 reg ar_lock,ar_owner,ar_round;
 wire ar_sel=ar_lock?ar_owner:((u_arvalid==2'b11)?ar_round:u_arvalid[1]);
 assign arvalid=u_arvalid[ar_sel];assign araddr=u_araddr[ar_sel*32+:32];
 assign arlen=u_arlen[ar_sel*8+:8];assign arid={3'd0,ar_sel};
 assign u_arready={2{arready}}&(ar_sel?2'b10:2'b01);
 assign u_rvalid={2{rvalid}}&(rid[0]?2'b10:2'b01);
 assign rready=u_rready[rid[0]];assign u_rdata=rdata;assign u_rlast=rlast;assign u_rresp=rresp;
 reg wr_active,wr_owner,wr_round,aw_done,w_done;
 wire [1:0] requesting=u_awvalid|u_wvalid;
 wire wr_sel=wr_active?wr_owner:((requesting==2'b11)?wr_round:requesting[1]);
 wire has_aw=wr_active&&aw_done,has_w=wr_active&&w_done;
 assign awvalid=u_awvalid[wr_sel]&&!has_aw;
 assign awaddr=u_awaddr[wr_sel*32+:32];assign awlen=u_awlen[wr_sel*8+:8];
 assign awid={3'd0,wr_sel};
 assign u_awready={2{awready&&!has_aw}}&(wr_sel?2'b10:2'b01);
 assign wvalid=u_wvalid[wr_sel]&&!has_w;
 assign wdata=u_wdata[wr_sel*128+:128];assign wstrb=u_wstrb[wr_sel*16+:16];
 assign wlast=u_wlast[wr_sel];
 assign u_wready={2{wready&&!has_w}}&(wr_sel?2'b10:2'b01);
 assign u_bvalid={2{bvalid}}&(bid[0]?2'b10:2'b01);
 assign bready=u_bready[bid[0]];assign u_bresp=bresp;
 wire aw_hs=awvalid&&awready,w_end=wvalid&&wready&&wlast;
 always @(posedge clk)begin
  if(rst)begin
   ar_lock<=0;ar_owner<=0;ar_round<=0;
   wr_active<=0;wr_owner<=0;wr_round<=0;aw_done<=0;w_done<=0;
  end else begin
   if(arvalid)begin
    ar_lock<=!arready;ar_owner<=ar_sel;if(arready)ar_round<=!ar_sel;
   end
   if(wr_active||requesting!=0)begin
    if((has_aw||aw_hs)&&(has_w||w_end))begin
     wr_active<=0;aw_done<=0;w_done<=0;wr_round<=!wr_sel;
    end else begin
     wr_active<=1;wr_owner<=wr_sel;aw_done<=has_aw||aw_hs;w_done<=has_w||w_end;
    end
   end
  end
 end
endmodule
