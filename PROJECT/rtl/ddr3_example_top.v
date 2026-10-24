//***************************************************************/
//
// Moudel Name    : ddr3_example_top.v
// Version        : 1.1
// Date Created   : 2023-02-23 10:37:59
// Last Modified  : 2023-02-24 15:27:41
// Abstract       : ---
//
//***************************************************************/
//Modification History
//1.initial
//***************************************************************/
`include "ddr3_controller/ddr3_parameter.vh"

`define Efinity_Debug
`timescale 1ps/1ps
// `define SOFT_JTAG

module ddr3_example_top #
(
parameter                       RANK_RATIO         = 1,       // # of unique CS outputs per rank
parameter                       CK_RATIO           = `CK_RATIO, 
parameter                       ASYN_AXI_CLK       = `ASYN_AXI_CLK, 
parameter                       RANKS              = `RANKS,
parameter                       CK_WIDTH           = `CK_WIDTH,       // # of CK/CK# outputs to memory   
parameter                       CKE_WIDTH          = `CKE_WIDTH,       // # of cke outputs
parameter                       CS_WIDTH           = `CS_WIDTH,       // # of unique CS outputs
parameter                       BANK_WIDTH         = `BANK_WIDTH,       // # of bank bits
parameter                       ROW_WIDTH          = `ROW_WIDTH,       // DRAM address bus width
parameter                       COL_WIDTH          = `COL_WIDTH,      // column address width
parameter                       DM_WIDTH           = `DM_WIDTH,       // # of DM (data mask)
parameter                       DQS_WIDTH          = `DQS_WIDTH,       // # of DQS (strobe)
parameter                       DQ_WIDTH           = `DQ_WIDTH,      // # of DQ (data)
parameter                       ODT_WIDTH          = `ODT_WIDTH,
parameter                       DQ_CNT_WIDTH       = `DQ_CNT_WIDTH,       // = ceil(log2(DQ_WIDTH))
parameter                       DQS_CNT_WIDTH      = `DQS_CNT_WIDTH,       // = ceil(log2(DQS_WIDTH))  
parameter                       DRAM_WIDTH         = `DRAM_WIDTH,       // # of DQ per DQS   
parameter                       DATA_WIDTH         = `DATA_WIDTH,
parameter                       ADDR_WIDTH         = `ADDR_WIDTH,    
parameter                       AXI_ID_WIDTH       = `AXI_ID_WIDTH,
parameter                       AXI_ADDR_WIDTH     = `AXI_ADDR_WIDTH,
parameter                       AXI_DATA_WIDTH     = `AXI_DATA_WIDTH
)
(

   // Clock and reset ports
   input                              axi_clk,      
   input                              core_clk,     // system clock: CPU, BitBlt, HDMI DMA (2026-10-11: 200 MHz)
   input                              pll_fb_clk,   // 100 MHz (pll_inst1 feedback output): DDR controller + SoC
                                                    // memory side; must be sdram_clk / CK_RATIO (4) = 100 MHz
   input                              sdram_clk,    // SDRAM CK @ 400MHz
   input                              rx_cal_clk,   // SDRAM CK @ 400MHz
   input                              tx_cal_clk,   // SDRAM CK @ 400MHz
   input                              tx_cal_clk_90edge,   // SDRAM CK @ 400MHz
   input                              pll_locked,
   input                              user_pll_locked,
  //  input                              peripheralClk,
   //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
   output  wire                       system_uart_0_io_txd,
   input                              system_uart_0_io_rxd,   
   input  [3:0]                       matrix_row_IN,
   output [3:0]                       matrix_row_OUT,
   output [3:0]                       matrix_row_OE,
   input  [3:0]                       matrix_col,
   input                              keyboard_rx,
   output                             keyboard_tx,
   input  [3:0]                       soc_gpio_IN,
   output [3:0]                       soc_gpio_OUT,
   output [3:0]                       soc_gpio_OE,
    // debug core ports
`ifdef  Efinity_Debug  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& 
`ifdef SOFT_JTAG
  input                               io_jtag_tms,
  input                               io_jtag_tdi,
  output  wire                        io_jtag_tdo,
  input                               io_jtag_tck,
`else
  input                               jtag_inst1_CAPTURE ,
  input                               jtag_inst1_DRCK    ,
  input                               jtag_inst1_RESET   ,
  input                               jtag_inst1_RUNTEST ,
  input                               jtag_inst1_SEL     ,
  input                               jtag_inst1_SHIFT   ,
  input                               jtag_inst1_TCK     ,
  input                               jtag_inst1_TDI     ,
  input                               jtag_inst1_TMS     ,
  input                               jtag_inst1_UPDATE  ,
  output                              jtag_inst1_TDO     ,
`endif 

`endif  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
   // PLL status flags  
   output [2:0]                       pll_shift,  
   output [4:0]                       pll_shift_sel,
   output                             pll_shift_ena,  
   // memory interface ports
   output                             ddr_ck_hi,
   output                             ddr_ck_lo,
   output                             ddr_reset_n,
   output [CKE_WIDTH-1:0]             ddr_cke,     
   output [ROW_WIDTH-1:0]             ddr_addr,
   output [BANK_WIDTH-1:0]            ddr_ba,
   output                             ddr_cas_n,

   output [CS_WIDTH*RANK_RATIO-1:0]   ddr_cs_n,
   output                             ddr_ras_n,
   output                             ddr_we_n,
   
   input  [DQS_WIDTH-1:0]             ddr_dqs_in_hi,
   input  [DQS_WIDTH-1:0]             ddr_dqs_in_lo,
   input  [DQ_WIDTH-1:0]              ddr_dq_in_hi,
   input  [DQ_WIDTH-1:0]              ddr_dq_in_lo,
   
   output [DQS_WIDTH-1:0]             ddr_dqs_oe,
   output [DQS_WIDTH-1:0]             ddr_dqs_oe_n,
   output [DQ_WIDTH-1:0]              ddr_dq_oe,  
   output [DQS_WIDTH-1:0]             ddr_dqs_out_hi,
   output [DQS_WIDTH-1:0]             ddr_dqs_out_lo,
   output [DQ_WIDTH-1:0]              ddr_dq_out_hi,
   output [DQ_WIDTH-1:0]              ddr_dq_out_lo,
   output [DM_WIDTH-1:0]              ddr_dm_hi,
   output [DM_WIDTH-1:0]              ddr_dm_lo,
   output [ODT_WIDTH-1:0]             ddr_odt,

   input                              system_spi_0_io_data_0_IN,
   input                              system_spi_0_io_data_1_IN,
   output                             system_spi_0_io_ss,
   output                             system_spi_0_io_sclk_write,
   output                             system_spi_0_io_data_0_OUT,
   output                             system_spi_0_io_data_0_OE,
   output                             system_spi_0_io_data_1_OUT,
   output                             system_spi_0_io_data_1_OE,

   // HDMI TX (LVDS TX blocks used as 10:1 TMDS serializers, PLL_BR0)
   input                              hdmi_tx_slow_clk,     // pixel clock 25.00 MHz
   input                              hdmi_tx_locked,
   output [9:0]                       tmds_clk_o,
   output [9:0]                       tmds_data0_o,
   output [9:0]                       tmds_data1_o,
   output [9:0]                       tmds_data2_o,
   output                             tmds_clk_TX_OE,
   output                             tmds_data0_TX_OE,
   output                             tmds_data1_TX_OE,
   output                             tmds_data2_TX_OE,
   output                             tmds_clk_TX_RST,
   output                             tmds_data0_TX_RST,
   output                             tmds_data1_TX_RST,
   output                             tmds_data2_TX_RST
   );


      
//Parameter Define
parameter FREQ = 200;			// default is 100 MHz.  Redefine as needed.
`ifndef SIM
    localparam CNT_INIT = 1.5*FREQ*1000;
`else
    localparam CNT_INIT = 10;
`endif    
  localparam   AIW               = AXI_ID_WIDTH      ;
  localparam   ADW               = AXI_DATA_WIDTH    ;
  localparam   ABN               = AXI_DATA_WIDTH/8   ;
  /////////////////////////////////////////////////////////
  // Wire declarations
  reg  [19:0]                       cnt;    
  wire                              clk;
  wire                              rst;
  wire                              mmcm_locked;
  reg                               aresetn;
  wire                              app_sr_active;
  wire                              app_ref_ack;
  wire                              app_zq_ack;

  wire                              cal_done;

  // Slave Interface Write Address Ports
  wire [AXI_ID_WIDTH-1:0]           s_axi_awid;
  wire [AXI_ADDR_WIDTH-1:0]         s_axi_awaddr;
  wire [7:0]                        s_axi_awlen;
  wire [2:0]                        s_axi_awsize;
  wire [1:0]                        s_axi_awburst;
  wire [0:0]                        s_axi_awlock;
  wire [3:0]                        s_axi_awcache;
  wire [2:0]                        s_axi_awprot;
  wire                              s_axi_awvalid;
  wire                              s_axi_awready;
   // Slave Interface Write Data Ports
  wire [AXI_DATA_WIDTH-1:0]         s_axi_wdata;
  wire [(AXI_DATA_WIDTH/8)-1:0]     s_axi_wstrb;
  wire                              s_axi_wlast;
  wire                              s_axi_wvalid;
  wire                              s_axi_wready;
   // Slave Interface Write Response Ports
  wire                              s_axi_bready;
  wire [AXI_ID_WIDTH-1:0]           s_axi_bid;
  wire [1:0]                        s_axi_bresp;
  wire                              s_axi_bvalid;
   // Slave Interface Read Address Ports
  wire [AXI_ID_WIDTH-1:0]           s_axi_arid;
  wire [AXI_ADDR_WIDTH-1:0]         s_axi_araddr;
  wire [7:0]                        s_axi_arlen;
  wire [2:0]                        s_axi_arsize;
  wire [1:0]                        s_axi_arburst;
  wire [0:0]                        s_axi_arlock;
  wire [3:0]                        s_axi_arcache;
  wire [2:0]                        s_axi_arprot;
  wire                              s_axi_arvalid;
  wire                              s_axi_arready;
   // Slave Interface Read Data Ports
  wire                              s_axi_rready;
  wire [AXI_ID_WIDTH-1:0]           s_axi_rid;
  wire [AXI_DATA_WIDTH-1:0]         s_axi_rdata;
  wire [1:0]                        s_axi_rresp;
  wire                              s_axi_rlast;
  wire                              s_axi_rvalid;


//***************************************************************************
  wire [2:0]                        vio_pll_shift;  
  wire [4:0]                        vio_pll_shift_sel;
  wire                              vio_pll_shift_ena; 
  wire                              pos_pll_shift_ena; 
  wire [2:0]                        phy_pll_shift;  
  wire [4:0]                        phy_pll_shift_sel;
  wire                              phy_pll_shift_ena; 
  wire                              ddr_reset; 
  wire                              sys_rst;
  wire  [2:0]                       phy_wr_pll_shift;
  wire                              idelay_ld       ;
  wire                              mpr_rdlvl_dly   ;
  wire  [7:0]                       wrlvl_dq_check  ;
  wire  [7:0]                       rd_level_dqs_check;  
  wire  [2:0]                       rdlvl_shift;  
  wire  [2:0]                       wrlvl_shift;  
  wire  [15:0]                      debug_fifo;  
  wire  [15:0]                      overflow_fifo;  
  wire  [6:0]                       init_cur_state;  
  wire  [35:0]                      ddr_debug_port;  
  wire                              app_rdy;  
  wire                              user_clk;  
  wire                              ddr_rstn;  
  wire                                phy_rddata_valid;
  wire [2*CK_RATIO*DQ_WIDTH-1:0]      phy_rd_data;
  wire [2*CK_RATIO*DATA_WIDTH-1:0]    app_rd_data_to_axi; 
  wire                                app_rd_data_end;
  wire                                app_rd_data_valid;
  wire [2*CK_RATIO*DQ_WIDTH-1:0]      rd_data;          
  wire                                rd_data_en;            
  wire                                rd_data_end;

  // ---- SoC <-> DDR AXI ID remap (SoC io_ddrA ID is 8 bit, ddr3_top is 4 bit)
  //      SoC ID = {arbiter_sel, 7'bid}: CPU -> 0x00..0x7F, io_ddrMasters_0 -> 0x80..
  //      DDR ID = {soc_id[7], soc_id[2:0]}; mapped back on the B/R channels.
  wire [7:0]                          soc_axi_awid;
  wire [7:0]                          soc_axi_arid;
  wire [7:0]                          soc_axi_bid;
  wire [7:0]                          soc_axi_rid;
  wire [31:0]                         soc_axi_awaddr;
  wire [31:0]                         soc_axi_araddr;

  // ---- APB slave 0 (0xF810_0000) -> HDMI framebuffer registers
  wire [15:0]                         apb_paddr;
  wire                                apb_psel;
  wire                                apb_penable;
  wire                                apb_pready;
  wire                                apb_pwrite;
  wire [31:0]                         apb_pwdata;
  wire [31:0]                         apb_prdata;
  wire                                apb_pslverror;

  wire                                soc_system_reset;
  wire                                hdmi_irq;

  // ---- io_ddrMasters_0 (128-bit AXI4 user master into SoC DDR arbiter,
  //      SoC IP: AXIMasterWidth = 128; driven by blit_axi_wide)
  wire                                dm0_reset;
  wire                                dm0_arvalid;
  wire                                dm0_arready;
  wire [31:0]                         dm0_araddr;
  wire [7:0]                          dm0_arlen;
  wire [2:0]                          dm0_arsize;
  wire [1:0]                          dm0_arburst;
  wire [3:0]                          dm0_arid;
  wire                                dm0_rvalid;
  wire                                dm0_rready;
  wire [127:0]                        dm0_rdata;
  wire                                dm0_rlast;
  wire [1:0]                          dm0_rresp;
  wire [3:0]                          dm0_rid;
  wire                                dm0_awvalid;
  wire                                dm0_awready;
  wire [31:0]                         dm0_awaddr;
  wire [7:0]                          dm0_awlen;
  wire [2:0]                          dm0_awsize;
  wire [1:0]                          dm0_awburst;
  wire [3:0]                          dm0_awid;
  wire                                dm0_wvalid;
  wire                                dm0_wready;
  wire [127:0]                        dm0_wdata;
  wire [15:0]                         dm0_wstrb;
  wire                                dm0_wlast;
  wire                                dm0_bvalid;
  wire                                dm0_bready;
  wire [3:0]                          dm0_bid;
  wire [1:0]                          dm0_bresp;

  // ---- 32-bit side of blit_axi_wide: AR/R from blit_axi_rd_arb,
  //      AW/W/B from blit_engine
  wire                                nw_arvalid;
  wire                                nw_arready;
  wire [31:0]                         nw_araddr;
  wire [7:0]                          nw_arlen;
  wire [2:0]                          nw_arsize;
  wire [1:0]                          nw_arburst;
  wire [3:0]                          nw_arid;
  wire                                nw_rvalid;
  wire                                nw_rready;
  wire [31:0]                         nw_rdata;
  wire                                nw_rlast;
  wire [1:0]                          nw_rresp;
  wire [3:0]                          nw_rid;
  wire                                nw_awvalid;
  wire                                nw_awready;
  wire [31:0]                         nw_awaddr;
  wire [7:0]                          nw_awlen;
  wire [2:0]                          nw_awsize;
  wire [1:0]                          nw_awburst;
  wire [3:0]                          nw_awid;
  wire                                nw_wvalid;
  wire                                nw_wready;
  wire [31:0]                         nw_wdata;
  wire [3:0]                          nw_wstrb;
  wire                                nw_wlast;
  wire                                nw_bvalid;
  wire                                nw_bready;
  wire [1:0]                          nw_bresp;

  // ---- BitBlt 128-bit wide path (C2): blit_engine x_* <-> blit_axi_wide x_*
  wire                                bx_arvalid;
  wire                                bx_arready;
  wire [31:0]                         bx_araddr;
  wire [3:0]                          bx_arlen;
  wire                                bx_rvalid;
  wire [127:0]                        bx_rdata;
  wire                                bx_rlast;
  wire [1:0]                          bx_rresp;
  wire                                bx_awvalid;
  wire                                bx_awready;
  wire [31:0]                         bx_awaddr;
  wire [3:0]                          bx_awlen;
  wire                                bx_wvalid;
  wire                                bx_wready;
  wire [127:0]                        bx_wdata;
  wire [15:0]                         bx_wstrb;
  wire                                bx_wlast;

  // ---- HDMI scan-out DMA read port (m0 of blit_axi_rd_arb)
  wire                                hd_arvalid;
  wire                                hd_arready;
  wire [31:0]                         hd_araddr;
  wire [7:0]                          hd_arlen;
  wire [2:0]                          hd_arsize;
  wire [1:0]                          hd_arburst;
  wire [3:0]                          hd_arid;
  wire                                hd_rvalid;
  wire                                hd_rready;
  wire [31:0]                         hd_rdata;
  wire                                hd_rlast;
  wire [1:0]                          hd_rresp;
  wire [3:0]                          hd_rid;

  // ---- 2026-10-19 audio: aud_rd_mux puts the HDMI scan-out DMA (vd_*, priority)
  //      and the audio mixer (mx_*) on m0 of blit_axi_rd_arb (hd_*)
  wire                                vd_arvalid;
  wire                                vd_arready;
  wire [31:0]                         vd_araddr;
  wire [7:0]                          vd_arlen;
  wire [2:0]                          vd_arsize;
  wire [1:0]                          vd_arburst;
  wire [3:0]                          vd_arid;
  wire                                vd_rvalid;
  wire                                vd_rready;
  wire                                mx_arvalid;
  wire                                mx_arready;
  wire [31:0]                         mx_araddr;
  wire [7:0]                          mx_arlen;
  wire [2:0]                          mx_arsize;
  wire [1:0]                          mx_arburst;
  wire                                mx_rvalid;
  wire                                mx_rready;
  // mixer -> HDMI audio sample FIFO
  wire                                aud_wvalid;
  wire                                aud_wready;
  wire [31:0]                         aud_wdata;
  wire [8:0]                          aud_level;

  // ---- BitBlt engine read port (m1 of blit_axi_rd_arb)
  wire                                bl_arvalid;
  wire                                bl_arready;
  wire [31:0]                         bl_araddr;
  wire [7:0]                          bl_arlen;
  wire [2:0]                          bl_arsize;
  wire [1:0]                          bl_arburst;
  wire [3:0]                          bl_arid;
  wire                                bl_rvalid;
  wire                                bl_rready;
  wire [31:0]                         bl_rdata;
  wire                                bl_rlast;
  wire [1:0]                          bl_rresp;
  wire [3:0]                          bl_rid;

  // ---- APB slave 0 split: paddr[13:12] = 00 -> hdmi_fb (0xF810_0000),
  //                          01 -> blit_engine (0xF810_1000),
  //                          1x -> audio_mixer (0xF810_2000, 2026-10-19)
  wire apb_sel_input = (apb_paddr[15:5]==11'h004);
  wire [31:0] input_prdata;
  input_hub u_input (
    .clk(user_clk),.rst(soc_system_reset),
    .matrix_col(matrix_col),.matrix_row_out(matrix_row_OUT),.matrix_row_oe(matrix_row_OE),
    .uart_rx(keyboard_rx),.uart_tx(keyboard_tx),
    .paddr(apb_paddr[4:0]),.psel(apb_psel & apb_sel_input),.penable(apb_penable),
    .pwrite(apb_pwrite),.pwdata(apb_pwdata),.prdata(input_prdata));
  wire apb_sel_motion = ~apb_paddr[13] & apb_paddr[12] & (apb_paddr[11:8]==4'h3);
  wire [31:0] mo_prdata;wire mo_pready;
  wire                                apb_sel_mix  = apb_paddr[13];
  wire                                apb_sel_blit = ~apb_paddr[13] & apb_paddr[12];
  wire [31:0]                         hd_prdata,  bl_prdata,  mx_prdata;
  wire                                hd_pready,  bl_pready,  mx_pready;
  wire                                hd_pslverr, bl_pslverr, mx_pslverr;
  wire                                blit_busy;

  
// Start of User Design top instance
//***************************************************************************
// The User design is instantiated below. The memory interface ports are
// connected to the top-level and the application interface ports are
// connected to the traffic generator module. This provides a reference
// for connecting the memory controller to system.
//***************************************************************************
// 2026-10-11: two clock domains. The DDR controller runs at sdram_clk / CK_RATIO
// (100 MHz, its DDR timing counters are in these clocks) on ddr_ctl_clk; the SoC
// (io_systemClk), the io_ddrMasters_0 user port, BitBlt and the HDMI DMA run on
// user_clk = core_clk (200 MHz). The SoC crosses between them itself:
// io_memoryClk domain with StreamFifoCC (CPU path) and Axi4CC (io_ddrMasters_0).
wire ddr_ctl_clk = pll_fb_clk;

always @(posedge ddr_ctl_clk or negedge sys_rst) begin
	if (!sys_rst)
		cnt <= 0;
    else if (cnt != CNT_INIT) 
        cnt <= cnt + 20'd1;
	else 
        cnt <= cnt;
end
assign ddr_rstn = (cnt == CNT_INIT);


generate
if (ASYN_AXI_CLK) begin
assign user_clk = axi_clk;

end else begin
assign user_clk = core_clk;
end
endgenerate
//***************************************************************************
// The traffic generation module instantiated below drives traffic (patterns)
// on the application interface of the memory controller
//***************************************************************************
//***************************************************************************

assign sys_rst       = pll_locked & user_pll_locked;

//***************************************************************************
ddr3_top                 u_ddr3_top
      (
      
    .axi_clk                (ddr_ctl_clk          ),   // ASYN_AXI_CLK = 0: AXI side = core
    .core_clk               (ddr_ctl_clk         ),
    .sdram_clk              (sdram_clk           ),  
    .rx_cal_clk             (rx_cal_clk          ),
    .tx_cal_clk             (tx_cal_clk          ),
    .tx_cal_clk_90edge      (tx_cal_clk_90edge   ),
    .rstn                   (ddr_rstn            ),      
    .pll_shift              (pll_shift           ),
    .pll_shift_sel          (pll_shift_sel       ),
    .pll_shift_ena          (pll_shift_ena       ), 
///////////////DDR BUS
    .ddr_ck_hi              (ddr_ck_hi           ),
    .ddr_ck_lo              (ddr_ck_lo           ),
    .ddr_cke                (ddr_cke             ),    
    .ddr_reset_n            (ddr_reset_n         ),
    .ddr_cs_n               (ddr_cs_n            ),
    .ddr_ras_n              (ddr_ras_n           ),
    .ddr_cas_n              (ddr_cas_n           ),
    .ddr_we_n               (ddr_we_n            ),     
    .ddr_addr               (ddr_addr            ),
    .ddr_ba                 (ddr_ba              ),

    .ddr_dqs_oe             (ddr_dqs_oe          ),
    .ddr_dqs_oe_n           (ddr_dqs_oe_n        ),
    .ddr_dq_oe              (ddr_dq_oe           ),
    .ddr_dqs_in_hi          (ddr_dqs_in_hi       ),
    .ddr_dqs_in_lo          (ddr_dqs_in_lo       ),
    .ddr_dq_in_hi           (ddr_dq_in_hi        ),
    .ddr_dq_in_lo           (ddr_dq_in_lo        ),

    .ddr_dqs_out_hi         (ddr_dqs_out_hi      ),
    .ddr_dqs_out_lo         (ddr_dqs_out_lo      ),
    .ddr_dq_out_hi          (ddr_dq_out_hi       ),
    .ddr_dq_out_lo          (ddr_dq_out_lo       ),
    
    .ddr_dm_hi              (ddr_dm_hi           ),
    .ddr_dm_lo              (ddr_dm_lo           ),
    .ddr_odt                (ddr_odt             ),

// Application interface ports
       .app_sr_req                     (1'b0),
       .app_ref_req                    (1'b0),
       .app_zq_req                     (1'b0),
       .app_sr_active                  (app_sr_active),
       .app_ref_ack                    (app_ref_ack),
       .app_zq_ack                     (app_zq_ack),

// Slave Interface Write Address Ports
       .s_axi_awid                     (s_axi_awid        ),
       .s_axi_awaddr                   (s_axi_awaddr      ),
       .s_axi_awlen                    (s_axi_awlen       ),
       .s_axi_awsize                   (s_axi_awsize      ),
       .s_axi_awburst                  (s_axi_awburst     ),
       .s_axi_awlock                   (s_axi_awlock      ),
       .s_axi_awcache                  (s_axi_awcache     ),
       .s_axi_awprot                   (s_axi_awprot      ),
       .s_axi_awqos                    (4'h0              ),
       .s_axi_awvalid                  (s_axi_awvalid     ),
       .s_axi_awready                  (s_axi_awready     ),
// Slave Interface Write Data Ports
       .s_axi_wdata                    (s_axi_wdata       ),
       .s_axi_wstrb                    (s_axi_wstrb       ),
       .s_axi_wlast                    (s_axi_wlast       ),
       .s_axi_wvalid                   (s_axi_wvalid      ),
       .s_axi_wready                   (s_axi_wready      ),
// Slave Interface Write Response Ports
       .s_axi_bid                      (s_axi_bid         ),
       .s_axi_bresp                    (s_axi_bresp       ),
       .s_axi_bvalid                   (s_axi_bvalid      ),
       .s_axi_bready                   (s_axi_bready      ),
// Slave Interface Read Address Ports
       .s_axi_arid                     (s_axi_arid        ),
       .s_axi_araddr                   (s_axi_araddr      ),
       .s_axi_arlen                    (s_axi_arlen       ),
       .s_axi_arsize                   (s_axi_arsize      ),
       .s_axi_arburst                  (s_axi_arburst     ),
       .s_axi_arlock                   (s_axi_arlock      ),
       .s_axi_arcache                  (s_axi_arcache     ),
       .s_axi_arprot                   (s_axi_arprot      ),
       .s_axi_arqos                    (4'h0              ),
       .s_axi_arvalid                  (s_axi_arvalid     ),
       .s_axi_arready                  (s_axi_arready     ),
// Slave Interface Read Data Ports
       .s_axi_rid                      (s_axi_rid         ),
       .s_axi_rdata                    (s_axi_rdata       ),
       .s_axi_rresp                    (s_axi_rresp       ),
       .s_axi_rlast                    (s_axi_rlast       ),
       .s_axi_rvalid                   (s_axi_rvalid      ),
       .s_axi_rready                   (s_axi_rready      ),
//DEBUG       
       .wrlvl_dq_check                 (wrlvl_dq_check    ) ,
       .rd_level_dqs_check             (rd_level_dqs_check) ,
       .rdlvl_shift                    (rdlvl_shift) ,
       .wrlvl_shift                    (wrlvl_shift) ,
       .init_cur_state                 (init_cur_state    ) ,
       .idelay_ld                      (idelay_ld         ) ,
       .mpr_rdlvl_dly                  (mpr_rdlvl_dly     ) ,
       .ddr_debug_port                 (ddr_debug_port    ) ,
       .cal_done                       (cal_done          ) 
       );
// End of User Design top instance

//***************************************************************************


//***************************************************************************
soc u_sapphire_soc(
    .io_asyncReset                      (!pll_locked                        ),
    .io_systemClk                       (user_clk                           ),

    //UART 0
    .system_uart_0_io_txd               (system_uart_0_io_txd               ),
    .system_uart_0_io_rxd               (system_uart_0_io_rxd               ),
    .io_memoryClk                       (ddr_ctl_clk                        ),   // = DDR controller clock
    .io_memoryReset                     (                                   ),
    .io_systemReset                     (soc_system_reset                   ),
    //External Memory AXI4 Interface
    .io_ddrA_aw_payload_prot            (                                  ),//(                                  ),
    .io_ddrA_aw_payload_qos             (                                  ),//(                                  ),
    .io_ddrA_aw_payload_cache           (                                  ),//(                                  ),
    .io_ddrA_aw_payload_lock            (s_axi_awlock                     ),//(m1_axi_awlock                     ),
    .io_ddrA_aw_payload_burst           (s_axi_awburst                    ),//(m1_axi_awburst                    ),
    .io_ddrA_aw_payload_size            (s_axi_awsize                     ),//(m1_axi_awsize                     ),
    .io_ddrA_aw_payload_len             (s_axi_awlen                      ),//(m1_axi_awlen                      ),
    .io_ddrA_aw_payload_region          (                                 ),//(                                  ),
    .io_ddrA_aw_payload_id              (soc_axi_awid                     ),//(m1_axi_awid                       ),
    .io_ddrA_aw_payload_addr            (soc_axi_awaddr                   ),//(m1_axi_awaddr                     ),
    .io_ddrA_aw_ready                   (s_axi_awready                    ),//(m1_axi_awready                    ),
    .io_ddrA_aw_valid                   (s_axi_awvalid                    ),//(m1_axi_awvalid                    ),
    .io_ddrA_w_payload_last             (s_axi_wlast                      ),//(m1_axi_wlast                      ),
    .io_ddrA_w_ready                    (s_axi_wready                     ),//(m1_axi_wready                     ),
    .io_ddrA_w_valid                    (s_axi_wvalid                     ),  //(m1_axi_wvalid                     ),    
    .io_ddrA_w_payload_strb             (s_axi_wstrb                      ),//(m1_axi_wstrb                      ),
    .io_ddrA_w_payload_data             (s_axi_wdata                      ),//(m1_axi_wdata                      ),
    .io_ddrA_b_payload_resp             (                                 ),//(                                  ),
    .io_ddrA_b_payload_id               (soc_axi_bid                      ),//(m1_axi_bid                        ),
    .io_ddrA_b_ready                    (s_axi_bready                     ),//(m1_axi_bready                     ),
    .io_ddrA_b_valid                    (s_axi_bvalid                     ),//(m1_axi_bvalid                     ),
    .io_ddrA_ar_payload_prot            (                                 ),//(                                  ),
    .io_ddrA_ar_payload_qos             (                                 ),//(                                  ),
    .io_ddrA_ar_payload_cache           (                                 ),//(                                  ),
    .io_ddrA_ar_payload_region          (                                 ),//(                                  ),
    .io_ddrA_ar_payload_lock            (s_axi_arlock                     ),//(m1_axi_arlock                     ),
    .io_ddrA_ar_payload_burst           (s_axi_arburst                    ),//(m1_axi_arburst                    ),
    .io_ddrA_ar_payload_size            (s_axi_arsize                     ),//(m1_axi_arsize                     ),
    .io_ddrA_ar_payload_len             (s_axi_arlen                      ),//(m1_axi_arlen                      ),
    .io_ddrA_ar_payload_id              (soc_axi_arid                     ),//(m1_axi_arid                       ),
    .io_ddrA_ar_payload_addr            (soc_axi_araddr                   ),//(m1_axi_araddr                     ),
    .io_ddrA_ar_ready                   (s_axi_arready                    ),//(m1_axi_arready                    ),
    .io_ddrA_ar_valid                   (s_axi_arvalid                    ),//(m1_axi_arvalid                    ),
    .io_ddrA_r_payload_last             (s_axi_rlast                      ),//(m1_axi_rlast                      ),
    .io_ddrA_r_payload_resp             (s_axi_rresp                      ),//(m1_axi_rresp                      ),
    .io_ddrA_r_payload_id               (soc_axi_rid                      ),//(m1_axi_rid                        ),
    .io_ddrA_r_payload_data             (s_axi_rdata                      ),//(m1_axi_rdata                      ),
    .io_ddrA_r_ready                    (s_axi_rready                     ),//(m1_axi_rready                     ),
    .io_ddrA_r_valid                    (s_axi_rvalid                     ),//(m1_axi_rvalid                     ),
     //SPI 0
    .system_spi_0_io_sclk_write         (system_spi_0_io_sclk_write         ),
    .system_spi_0_io_data_0_writeEnable (system_spi_0_io_data_0_OE ),
    .system_spi_0_io_data_0_read        (system_spi_0_io_data_0_IN        ),
    .system_spi_0_io_data_0_write       (system_spi_0_io_data_0_OUT       ),
    .system_spi_0_io_data_1_writeEnable (system_spi_0_io_data_1_OE ),
    .system_spi_0_io_data_1_read        (system_spi_0_io_data_1_IN        ),
    .system_spi_0_io_data_1_write       (system_spi_0_io_data_1_OUT       ),
    .system_spi_0_io_data_2_writeEnable (                                   ),
    .system_spi_0_io_data_2_read        (                                   ),
    .system_spi_0_io_data_2_write       (                                   ),
    .system_spi_0_io_data_3_writeEnable (                                   ),
    .system_spi_0_io_data_3_read        (                                   ),
    .system_spi_0_io_data_3_write       (                                   ),
    .system_spi_0_io_ss                 (system_spi_0_io_ss                 ),
    //APB 0
    .io_apbSlave_0_PADDR                (apb_paddr                          ),
    .io_apbSlave_0_PSEL                 (apb_psel                           ),
    .io_apbSlave_0_PENABLE              (apb_penable                        ),
    .io_apbSlave_0_PREADY               (apb_pready                         ),
    .io_apbSlave_0_PWRITE               (apb_pwrite                         ),
    .io_apbSlave_0_PWDATA               (apb_pwdata                         ),
    .io_apbSlave_0_PRDATA               (apb_prdata                         ),
    .io_apbSlave_0_PSLVERROR            (apb_pslverror                      ),

    //User interrupt A (PLIC) <- HDMI frame IRQ
    .userInterruptA                     (hdmi_irq                           ),

    //io_ddrMasters_0 : 128-bit AXI4 user master (blit_axi_wide)
    //  AR/R = HDMI scan-out + BitBlt (blit_axi_rd_arb), AW/W/B = BitBlt
    .io_ddrMasters_0_clk                (user_clk                           ),
    .io_ddrMasters_0_reset              (dm0_reset                          ),
    .io_ddrMasters_0_ar_valid           (dm0_arvalid                        ),
    .io_ddrMasters_0_ar_ready           (dm0_arready                        ),
    .io_ddrMasters_0_ar_payload_addr    (dm0_araddr                         ),
    .io_ddrMasters_0_ar_payload_id      (dm0_arid                           ),
    .io_ddrMasters_0_ar_payload_region  (4'd0                               ),
    .io_ddrMasters_0_ar_payload_len     (dm0_arlen                          ),
    .io_ddrMasters_0_ar_payload_size    (dm0_arsize                         ),
    .io_ddrMasters_0_ar_payload_burst   (dm0_arburst                        ),
    .io_ddrMasters_0_ar_payload_lock    (1'b0                               ),
    .io_ddrMasters_0_ar_payload_cache   (4'd0                               ),
    .io_ddrMasters_0_ar_payload_qos     (4'd0                               ),
    .io_ddrMasters_0_ar_payload_prot    (3'd0                               ),
    .io_ddrMasters_0_r_valid            (dm0_rvalid                         ),
    .io_ddrMasters_0_r_ready            (dm0_rready                         ),
    .io_ddrMasters_0_r_payload_data     (dm0_rdata                          ),
    .io_ddrMasters_0_r_payload_id       (dm0_rid                            ),
    .io_ddrMasters_0_r_payload_resp     (dm0_rresp                          ),
    .io_ddrMasters_0_r_payload_last     (dm0_rlast                          ),
    // write channels: BitBlt engine (the only writer on this port)
    .io_ddrMasters_0_aw_valid           (dm0_awvalid                        ),
    .io_ddrMasters_0_aw_ready           (dm0_awready                        ),
    .io_ddrMasters_0_aw_payload_addr    (dm0_awaddr                         ),
    .io_ddrMasters_0_aw_payload_id      (dm0_awid                           ),
    .io_ddrMasters_0_aw_payload_region  (4'd0                               ),
    .io_ddrMasters_0_aw_payload_len     (dm0_awlen                          ),
    .io_ddrMasters_0_aw_payload_size    (dm0_awsize                         ),
    .io_ddrMasters_0_aw_payload_burst   (dm0_awburst                        ),
    .io_ddrMasters_0_aw_payload_lock    (1'b0                               ),
    .io_ddrMasters_0_aw_payload_cache   (4'd0                               ),
    .io_ddrMasters_0_aw_payload_qos     (4'd0                               ),
    .io_ddrMasters_0_aw_payload_prot    (3'd0                               ),
    .io_ddrMasters_0_w_valid            (dm0_wvalid                         ),
    .io_ddrMasters_0_w_ready            (dm0_wready                         ),
    .io_ddrMasters_0_w_payload_data     (dm0_wdata                          ),
    .io_ddrMasters_0_w_payload_strb     (dm0_wstrb                          ),
    .io_ddrMasters_0_w_payload_last     (dm0_wlast                          ),
    .io_ddrMasters_0_b_valid            (dm0_bvalid                         ),
    .io_ddrMasters_0_b_ready            (dm0_bready                         ),
    .io_ddrMasters_0_b_payload_id       (dm0_bid                            ),
    .io_ddrMasters_0_b_payload_resp     (dm0_bresp                          ),

    .system_gpio_0_io_write             ( soc_gpio_OUT                      ),//,
    .system_gpio_0_io_read              ( soc_gpio_IN                       ),//
    .system_gpio_0_io_writeEnable       ( soc_gpio_OE                       ),//, 


    //Hard Jtag Tap
    .jtagCtrl_tck                       (jtag_inst1_TCK                     ),
    .jtagCtrl_tdi                       (jtag_inst1_TDI                     ),
    .jtagCtrl_tdo                       (jtag_inst1_TDO                     ),
    .jtagCtrl_enable                    (jtag_inst1_SEL                     ),
    .jtagCtrl_capture                   (jtag_inst1_CAPTURE                 ),
    .jtagCtrl_shift                     (jtag_inst1_SHIFT                   ),
    .jtagCtrl_update                    (jtag_inst1_UPDATE                  ),
    .jtagCtrl_reset                     (jtag_inst1_RESET                   )

);


//***************************************************************************
// AXI ID remap between SoC (8-bit) and ddr3_top (AXI_ID_WIDTH = 4)
//***************************************************************************
assign s_axi_awid     = {soc_axi_awid[7], soc_axi_awid[2:0]};
assign s_axi_arid     = {soc_axi_arid[7], soc_axi_arid[2:0]};
assign soc_axi_bid    = {s_axi_bid[3], 4'b0000, s_axi_bid[2:0]};
assign soc_axi_rid    = {s_axi_rid[3], 4'b0000, s_axi_rid[2:0]};
assign s_axi_awaddr   = soc_axi_awaddr[AXI_ADDR_WIDTH-1:0];
assign s_axi_araddr   = soc_axi_araddr[AXI_ADDR_WIDTH-1:0];

//***************************************************************************
// HDMI framebuffer scan-out (APB regs @0xF8100000 + AXI read DMA + TMDS)
//***************************************************************************
wire [9:0] tmds_clk_w, tmds_d0_w, tmds_d1_w, tmds_d2_w;

// Video mode (2026-10-21): 1 = 1280x720@60, 0 = 640x480@60 (25.00 / 125 MHz).
// 720p pixel clock: CEA says 74.25 MHz, but the Ti60 PLL (PFD >= 16 MHz, so N = 1 with
// the 25 MHz input) can not make it; pll_hdmi gives 74.375 MHz (+0.17 %, 60.10 Hz) and
// the TMDS fast clock 371.875 MHz (x5, 90 deg). ACR_CTS must be the pixel clock in kHz.
// Also sizes the BitBlt band window (1280 or 640 px rows). Software reads the mode
// from hdmi_fb RES (0x14) and blit CAPS2[2].
localparam HDMI_720P = 1;

hdmi_fb_top #(
    .DUAL_MODE(1),
    .H_ACT        (HDMI_720P ? 1280 : 640),
    .H_FP         (HDMI_720P ? 110  : 16),
    .H_SYNC       (HDMI_720P ? 40   : 96),
    .H_BP         (HDMI_720P ? 220  : 42),
    .V_ACT        (HDMI_720P ? 720  : 480),
    .V_FP         (HDMI_720P ? 5    : 10),
    .V_SYNC       (HDMI_720P ? 5    : 2),
    .V_BP         (HDMI_720P ? 20   : 33),
    .HS_POL       (HDMI_720P ? 1'b1 : 1'b0),
    .VS_POL       (HDMI_720P ? 1'b1 : 1'b0),
    .FIFO_AW      (HDMI_720P ? 11   : 10),
    .ACR_CTS      (HDMI_720P ? 20'd74375 : 20'd25000),   // pixel clock / 1 kHz
    .AVI_VIC      (HDMI_720P ? 8'd4 : 8'd1),
    .AVI_PB2      (HDMI_720P ? 8'h28 : 8'h18),
    .DEFAULT_FB   (32'h0100_0000)
) u_hdmi_fb (
    .clk            (user_clk          ),
    .rst            (soc_system_reset  ),
    .axi_rst        (dm0_reset         ),

    .apb_paddr      (apb_paddr         ),
    .apb_psel       (apb_psel & ~apb_sel_blit & ~apb_sel_mix & ~apb_sel_input),
    .apb_penable    (apb_penable       ),
    .apb_pwrite     (apb_pwrite        ),
    .apb_pwdata     (apb_pwdata        ),
    .apb_prdata     (hd_prdata         ),
    .apb_pready     (hd_pready         ),
    .apb_pslverror  (hd_pslverr        ),
    .irq            (hdmi_irq          ),

    .axi_arvalid    (vd_arvalid        ),
    .axi_arready    (vd_arready        ),
    .axi_araddr     (vd_araddr         ),
    .axi_arlen      (vd_arlen          ),
    .axi_arsize     (vd_arsize         ),
    .axi_arburst    (vd_arburst        ),
    .axi_arid       (vd_arid           ),
    .axi_rvalid     (vd_rvalid         ),
    .axi_rready     (vd_rready         ),
    .axi_rdata      (hd_rdata          ),
    .axi_rlast      (hd_rlast          ),
    .axi_rresp      (hd_rresp          ),
    .axi_rid        (hd_rid            ),

    .pix_clk        (hdmi_tx_slow_clk  ),
    .pix_rst_n      (hdmi_tx_locked    ),
    .tmds_clk       (tmds_clk_w        ),
    .tmds_d0        (tmds_d0_w         ),
    .tmds_d1        (tmds_d1_w         ),
    .tmds_d2        (tmds_d2_w         ),

    .aud_wvalid     (aud_wvalid        ),
    .aud_wready     (aud_wready        ),
    .aud_wdata      (aud_wdata         ),
    .aud_level      (aud_level         )
);

//***************************************************************************
// Audio mixer (APB regs @0xF8102000): 16 PCM voices from DDR -> HDMI audio FIFO.
// Its DDR reads share m0 of blit_axi_rd_arb with the HDMI scan-out (aud_rd_mux).
//***************************************************************************
audio_mixer #(
    .LVL_W          (9                 )
) u_audio_mixer (
    .clk            (user_clk          ),
    .rst            (soc_system_reset  ),
    .axi_rst        (dm0_reset         ),

    .apb_paddr      (apb_paddr[11:0]   ),
    .apb_psel       (apb_psel & apb_sel_mix),
    .apb_penable    (apb_penable       ),
    .apb_pwrite     (apb_pwrite        ),
    .apb_pwdata     (apb_pwdata        ),
    .apb_prdata     (mx_prdata         ),
    .apb_pready     (mx_pready         ),
    .apb_pslverror  (mx_pslverr        ),

    .axi_arvalid    (mx_arvalid        ),
    .axi_arready    (mx_arready        ),
    .axi_araddr     (mx_araddr         ),
    .axi_arlen      (mx_arlen          ),
    .axi_arsize     (mx_arsize         ),
    .axi_arburst    (mx_arburst        ),
    .axi_rvalid     (mx_rvalid         ),
    .axi_rready     (mx_rready         ),
    .axi_rdata      (hd_rdata          ),
    .axi_rlast      (hd_rlast          ),

    .o_valid        (aud_wvalid        ),
    .o_ready        (aud_wready        ),
    .o_data         (aud_wdata         ),
    .o_level        (aud_level         )
);

aud_rd_mux u_aud_rd_mux (
    .clk            (user_clk          ),
    .rst            (dm0_reset         ),
    .a_arvalid      (vd_arvalid        ),
    .a_arready      (vd_arready        ),
    .a_araddr       (vd_araddr         ),
    .a_arlen        (vd_arlen          ),
    .a_arsize       (vd_arsize         ),
    .a_arburst      (vd_arburst        ),
    .a_rvalid       (vd_rvalid         ),
    .b_arvalid      (mx_arvalid        ),
    .b_arready      (mx_arready        ),
    .b_araddr       (mx_araddr         ),
    .b_arlen        (mx_arlen          ),
    .b_arsize       (mx_arsize         ),
    .b_arburst      (mx_arburst        ),
    .b_rvalid       (mx_rvalid         ),
    .s_arvalid      (hd_arvalid        ),
    .s_arready      (hd_arready        ),
    .s_araddr       (hd_araddr         ),
    .s_arlen        (hd_arlen          ),
    .s_arsize       (hd_arsize         ),
    .s_arburst      (hd_arburst        ),
    .s_rvalid       (hd_rvalid         ),
    .s_rlast        (hd_rlast          )
);

// board has P/N swapped on the HDMI pairs -> invert (same as 03_hdmi_tx_demo)
assign tmds_clk_o        = ~tmds_clk_w;
assign tmds_data0_o      = ~tmds_d0_w;
assign tmds_data1_o      = ~tmds_d1_w;
assign tmds_data2_o      = ~tmds_d2_w;
assign tmds_clk_TX_OE    = 1'b1;
assign tmds_data0_TX_OE  = 1'b1;
assign tmds_data1_TX_OE  = 1'b1;
assign tmds_data2_TX_OE  = 1'b1;
assign tmds_clk_TX_RST   = 1'b0;
assign tmds_data0_TX_RST = 1'b0;
assign tmds_data1_TX_RST = 1'b0;
assign tmds_data2_TX_RST = 1'b0;

//***************************************************************************
// BitBlt 2D engine (APB regs @0xF8101000, AXI4 burst DMA on io_ddrMasters_0)
//***************************************************************************
assign apb_prdata    = apb_sel_input ? input_prdata : apb_sel_motion ? mo_prdata : apb_sel_mix ? mx_prdata  : apb_sel_blit ? bl_prdata  : hd_prdata;
assign apb_pready    = apb_sel_input ? 1'b1 : apb_sel_motion ? mo_pready : apb_sel_mix ? mx_pready  : apb_sel_blit ? bl_pready  : hd_pready;
assign apb_pslverror = apb_sel_input ? 1'b0 : apb_sel_motion ? 1'b0 : apb_sel_mix ? mx_pslverr : apb_sel_blit ? bl_pslverr : hd_pslverr;

blit_axi_rd_arb u_blit_rd_arb (
    .clk            (user_clk          ),
    .rst            (dm0_reset         ),

    .m0_arvalid     (hd_arvalid        ),
    .m0_arready     (hd_arready        ),
    .m0_araddr      (hd_araddr         ),
    .m0_arlen       (hd_arlen          ),
    .m0_arsize      (hd_arsize         ),
    .m0_arburst     (hd_arburst        ),
    .m0_rvalid      (hd_rvalid         ),
    .m0_rdata       (hd_rdata          ),
    .m0_rlast       (hd_rlast          ),
    .m0_rresp       (hd_rresp          ),
    .m0_rid         (hd_rid            ),

    .m1_arvalid     (bl_arvalid        ),
    .m1_arready     (bl_arready        ),
    .m1_araddr      (bl_araddr         ),
    .m1_arlen       (bl_arlen          ),
    .m1_arsize      (bl_arsize         ),
    .m1_arburst     (bl_arburst        ),
    .m1_rvalid      (bl_rvalid         ),
    .m1_rdata       (bl_rdata          ),
    .m1_rlast       (bl_rlast          ),
    .m1_rresp       (bl_rresp          ),
    .m1_rid         (bl_rid            ),

    .s_arvalid      (nw_arvalid        ),
    .s_arready      (nw_arready        ),
    .s_araddr       (nw_araddr         ),
    .s_arlen        (nw_arlen          ),
    .s_arsize       (nw_arsize         ),
    .s_arburst      (nw_arburst        ),
    .s_arid         (nw_arid           ),
    .s_rvalid       (nw_rvalid         ),
    .s_rready       (nw_rready         ),
    .s_rdata        (nw_rdata          ),
    .s_rlast        (nw_rlast          ),
    .s_rresp        (nw_rresp          ),
    .s_rid          (nw_rid            )
);

blit_engine #(
    .CQ_AW          (4                 ),
    .RING_AW        (10                ),
    .WF_AW          (9                 ),
    .AXI_ID         (4'd1              ),
    .WIN_BYTES      (HDMI_720P ? 2560 : 1280)
) u_blit (
    .clk            (user_clk          ),
    .rst            (soc_system_reset | dm0_reset),

    .apb_paddr      (apb_paddr[11:0]   ),
    .apb_psel       (apb_psel & apb_sel_blit & ~apb_sel_motion),
    .apb_penable    (apb_penable       ),
    .apb_pwrite     (apb_pwrite        ),
    .apb_pwdata     (apb_pwdata        ),
    .apb_prdata     (bl_prdata         ),
    .apb_pready     (bl_pready         ),
    .apb_pslverror  (bl_pslverr        ),

    .m_arvalid      (bl_arvalid        ),
    .m_arready      (bl_arready        ),
    .m_araddr       (bl_araddr         ),
    .m_arlen        (bl_arlen          ),
    .m_arsize       (bl_arsize         ),
    .m_arburst      (bl_arburst        ),
    .m_arid         (bl_arid           ),
    .m_rvalid       (bl_rvalid         ),
    .m_rready       (bl_rready         ),
    .m_rdata        (bl_rdata          ),
    .m_rlast        (bl_rlast          ),
    .m_rresp        (bl_rresp          ),

    .m_awvalid      (nw_awvalid        ),
    .m_awready      (nw_awready        ),
    .m_awaddr       (nw_awaddr         ),
    .m_awlen        (nw_awlen          ),
    .m_awsize       (nw_awsize         ),
    .m_awburst      (nw_awburst        ),
    .m_awid         (nw_awid           ),
    .m_wvalid       (nw_wvalid         ),
    .m_wready       (nw_wready         ),
    .m_wdata        (nw_wdata          ),
    .m_wstrb        (nw_wstrb          ),
    .m_wlast        (nw_wlast          ),
    .m_bvalid       (nw_bvalid         ),
    .m_bready       (nw_bready         ),
    .m_bresp        (nw_bresp          ),

    .x_arvalid      (bx_arvalid        ),
    .x_arready      (bx_arready        ),
    .x_araddr       (bx_araddr         ),
    .x_arlen        (bx_arlen          ),
    .x_rvalid       (bx_rvalid         ),
    .x_rdata        (bx_rdata          ),
    .x_rlast        (bx_rlast          ),
    .x_rresp        (bx_rresp          ),
    .x_awvalid      (bx_awvalid        ),
    .x_awready      (bx_awready        ),
    .x_awaddr       (bx_awaddr         ),
    .x_awlen        (bx_awlen          ),
    .x_wvalid       (bx_wvalid         ),
    .x_wready       (bx_wready         ),
    .x_wdata        (bx_wdata          ),
    .x_wstrb        (bx_wstrb          ),
    .x_wlast        (bx_wlast          ),

    .busy           (blit_busy         )
);

// 32-bit (arbiter / engine) <-> 128-bit io_ddrMasters_0: packs W into
// 128-bit beats and releases each AW after its W, splits R beats into words
wire  gx_arvalid;
wire  gx_arready;
wire [31:0] gx_araddr;
wire [7:0] gx_arlen;
wire [2:0] gx_arsize;
wire [1:0] gx_arburst;
wire [3:0] gx_arid;
wire  gx_rvalid;
wire  gx_rready;
wire [127:0] gx_rdata;
wire  gx_rlast;
wire [1:0] gx_rresp;
wire  gx_awvalid;
wire  gx_awready;
wire [31:0] gx_awaddr;
wire [7:0] gx_awlen;
wire [2:0] gx_awsize;
wire [1:0] gx_awburst;
wire [3:0] gx_awid;
wire  gx_wvalid;
wire  gx_wready;
wire [127:0] gx_wdata;
wire [15:0] gx_wstrb;
wire  gx_wlast;
wire  gx_bvalid;
wire  gx_bready;
wire [1:0] gx_bresp;
wire  mo_arvalid;
wire  mo_arready;
wire [31:0] mo_araddr;
wire [7:0] mo_arlen;
wire  mo_rvalid;
wire  mo_rready;
wire [127:0] mo_rdata;
wire  mo_rlast;
wire [1:0] mo_rresp;
wire  mo_awvalid;
wire  mo_awready;
wire [31:0] mo_awaddr;
wire [7:0] mo_awlen;
wire  mo_wvalid;
wire  mo_wready;
wire [127:0] mo_wdata;
wire [15:0] mo_wstrb;
wire  mo_wlast;
wire  mo_bvalid;
wire  mo_bready;
wire [1:0] mo_bresp;
blit_axi_wide #(
    .RQ_AW          (4                 )
) u_blit_wide (
    .clk            (user_clk          ),
    .rst            (dm0_reset         ),

    .u_arvalid      (nw_arvalid        ),
    .u_arready      (nw_arready        ),
    .u_araddr       (nw_araddr         ),
    .u_arlen        (nw_arlen          ),
    .u_arid         (nw_arid           ),
    .u_rvalid       (nw_rvalid         ),
    .u_rready       (nw_rready         ),
    .u_rdata        (nw_rdata          ),
    .u_rlast        (nw_rlast          ),
    .u_rresp        (nw_rresp          ),
    .u_rid          (nw_rid            ),
    .u_awvalid      (nw_awvalid        ),
    .u_awready      (nw_awready        ),
    .u_awaddr       (nw_awaddr         ),
    .u_awlen        (nw_awlen          ),
    .u_wvalid       (nw_wvalid         ),
    .u_wready       (nw_wready         ),
    .u_wdata        (nw_wdata          ),
    .u_wstrb        (nw_wstrb          ),
    .u_wlast        (nw_wlast          ),
    .u_bvalid       (nw_bvalid         ),
    .u_bready       (nw_bready         ),
    .u_bresp        (nw_bresp          ),

    .x_arvalid      (bx_arvalid        ),
    .x_arready      (bx_arready        ),
    .x_araddr       (bx_araddr         ),
    .x_arlen        (bx_arlen          ),
    .x_rvalid       (bx_rvalid         ),
    .x_rdata        (bx_rdata          ),
    .x_rlast        (bx_rlast          ),
    .x_rresp        (bx_rresp          ),
    .x_awvalid      (bx_awvalid        ),
    .x_awready      (bx_awready        ),
    .x_awaddr       (bx_awaddr         ),
    .x_awlen        (bx_awlen          ),
    .x_wvalid       (bx_wvalid         ),
    .x_wready       (bx_wready         ),
    .x_wdata        (bx_wdata          ),
    .x_wstrb        (bx_wstrb          ),
    .x_wlast        (bx_wlast          ),

    .d_arvalid      (gx_arvalid       ),
    .d_arready      (gx_arready       ),
    .d_araddr       (gx_araddr        ),
    .d_arlen        (gx_arlen         ),
    .d_arsize       (gx_arsize        ),
    .d_arburst      (gx_arburst       ),
    .d_arid         (gx_arid          ),
    .d_rvalid       (gx_rvalid        ),
    .d_rready       (gx_rready        ),
    .d_rdata        (gx_rdata         ),
    .d_rlast        (gx_rlast         ),
    .d_rresp        (gx_rresp         ),
    .d_awvalid      (gx_awvalid       ),
    .d_awready      (gx_awready       ),
    .d_awaddr       (gx_awaddr        ),
    .d_awlen        (gx_awlen         ),
    .d_awsize       (gx_awsize        ),
    .d_awburst      (gx_awburst       ),
    .d_awid         (gx_awid          ),
    .d_wvalid       (gx_wvalid        ),
    .d_wready       (gx_wready        ),
    .d_wdata        (gx_wdata         ),
    .d_wstrb        (gx_wstrb         ),
    .d_wlast        (gx_wlast         ),
    .d_bvalid       (gx_bvalid        ),
    .d_bready       (gx_bready        ),
    .d_bresp        (gx_bresp         )
);

//***************************************************************************


//***************************************************************************

square_motion u_square_motion(
 .clk(user_clk),.rst(soc_system_reset | dm0_reset),
 .paddr(apb_paddr[11:0]),.psel(apb_psel & apb_sel_motion),
 .penable(apb_penable),.pwrite(apb_pwrite),.pwdata(apb_pwdata),.prdata(mo_prdata),.pready(mo_pready),
 .arvalid(mo_arvalid),.arready(mo_arready),.araddr(mo_araddr),.arlen(mo_arlen),
 .rvalid(mo_rvalid),.rready(mo_rready),.rdata(mo_rdata),.rlast(mo_rlast),.rresp(mo_rresp),
 .awvalid(mo_awvalid),.awready(mo_awready),.awaddr(mo_awaddr),.awlen(mo_awlen),
 .wvalid(mo_wvalid),.wready(mo_wready),.wdata(mo_wdata),.wstrb(mo_wstrb),.wlast(mo_wlast),
 .bvalid(mo_bvalid),.bready(mo_bready),.bresp(mo_bresp));
motion_axi_arb u_motion_arb(
 .clk(user_clk),.rst(dm0_reset),
 .u_arvalid({mo_arvalid,gx_arvalid}),.u_arready({mo_arready,gx_arready}),
 .u_araddr({mo_araddr,gx_araddr}),.u_arlen({mo_arlen,gx_arlen}),
 .u_rvalid({mo_rvalid,gx_rvalid}),.u_rready({mo_rready,gx_rready}),
 .u_rdata(gx_rdata),.u_rlast(gx_rlast),.u_rresp(gx_rresp),
 .u_awvalid({mo_awvalid,gx_awvalid}),.u_awready({mo_awready,gx_awready}),
 .u_awaddr({mo_awaddr,gx_awaddr}),.u_awlen({mo_awlen,gx_awlen}),
 .u_wvalid({mo_wvalid,gx_wvalid}),.u_wready({mo_wready,gx_wready}),
 .u_wdata({mo_wdata,gx_wdata}),.u_wstrb({mo_wstrb,gx_wstrb}),.u_wlast({mo_wlast,gx_wlast}),
 .u_bvalid({mo_bvalid,gx_bvalid}),.u_bready({mo_bready,gx_bready}),.u_bresp(gx_bresp),
 .arvalid(dm0_arvalid),
 .arready(dm0_arready),
 .araddr(dm0_araddr),
 .arlen(dm0_arlen),
 .arid(dm0_arid),
 .rvalid(dm0_rvalid),
 .rready(dm0_rready),
 .rdata(dm0_rdata),
 .rlast(dm0_rlast),
 .rresp(dm0_rresp),
 .rid(dm0_rid),
 .awvalid(dm0_awvalid),
 .awready(dm0_awready),
 .awaddr(dm0_awaddr),
 .awlen(dm0_awlen),
 .awid(dm0_awid),
 .wvalid(dm0_wvalid),
 .wready(dm0_wready),
 .wdata(dm0_wdata),
 .wstrb(dm0_wstrb),
 .wlast(dm0_wlast),
 .bvalid(dm0_bvalid),
 .bready(dm0_bready),
 .bresp(dm0_bresp),
 .bid(dm0_bid));
assign dm0_arsize=3'd4;assign dm0_arburst=2'b01;
assign dm0_awsize=3'd4;assign dm0_awburst=2'b01;
assign mo_rdata=gx_rdata;assign mo_rlast=gx_rlast;assign mo_rresp=gx_rresp;assign mo_bresp=gx_bresp;


endmodule
