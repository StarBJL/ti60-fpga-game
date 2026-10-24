// Simulation wrapper: blit_engine + blit_axi_rd_arb + blit_axi_wide exactly as
// wired in ddr3_example_top.v; the HDMI read master and the 128-bit SoC DDR
// port (io_ddrMasters_0) are modelled by tb_blit.py.  The 32-bit side between
// the engine/arbiter and blit_axi_wide is exported (n_*) for protocol checks.
module tb_blit_top (
    input  wire         clk,
    input  wire         rst,

    input  wire [11:0]  apb_paddr,
    input  wire         apb_psel,
    input  wire         apb_penable,
    input  wire         apb_pwrite,
    input  wire [31:0]  apb_pwdata,
    output wire [31:0]  apb_prdata,
    output wire         apb_pready,
    output wire         apb_pslverror,

    // fake HDMI read master (m0)
    input  wire         hd_arvalid,
    output wire         hd_arready,
    input  wire [31:0]  hd_araddr,
    input  wire [7:0]   hd_arlen,
    output wire         hd_rvalid,
    output wire [31:0]  hd_rdata,
    output wire         hd_rlast,

    // io_ddrMasters_0 (128-bit)
    output wire         s_arvalid,
    input  wire         s_arready,
    output wire [31:0]  s_araddr,
    output wire [7:0]   s_arlen,
    output wire [2:0]   s_arsize,
    output wire [1:0]   s_arburst,
    output wire [3:0]   s_arid,
    input  wire         s_rvalid,
    output wire         s_rready,
    input  wire [127:0] s_rdata,
    input  wire         s_rlast,
    input  wire [1:0]   s_rresp,

    output wire         s_awvalid,
    input  wire         s_awready,
    output wire [31:0]  s_awaddr,
    output wire [7:0]   s_awlen,
    output wire [2:0]   s_awsize,
    output wire [1:0]   s_awburst,
    output wire [3:0]   s_awid,
    output wire         s_wvalid,
    input  wire         s_wready,
    output wire [127:0] s_wdata,
    output wire [15:0]  s_wstrb,
    output wire         s_wlast,
    input  wire         s_bvalid,
    output wire         s_bready,
    input  wire [1:0]   s_bresp,

    // 32-bit side (observation only)
    output wire         n_arvalid,
    output wire         n_arready,
    output wire [31:0]  n_araddr,
    output wire [7:0]   n_arlen,
    output wire [2:0]   n_arsize,
    output wire [1:0]   n_arburst,
    output wire [3:0]   n_arid,
    output wire         n_rvalid,
    output wire         n_rready,
    output wire [31:0]  n_rdata,
    output wire         n_rlast,
    output wire [3:0]   n_rid,
    output wire         n_awvalid,
    output wire         n_awready,
    output wire [31:0]  n_awaddr,
    output wire [7:0]   n_awlen,
    output wire [2:0]   n_awsize,
    output wire [1:0]   n_awburst,
    output wire [3:0]   n_awid,
    output wire         n_wvalid,
    output wire         n_wready,
    output wire [31:0]  n_wdata,
    output wire [3:0]   n_wstrb,
    output wire         n_wlast,

    // engine 128-bit wide path (C2, observation only)
    output wire         x_arvalid,
    output wire         x_arready,
    output wire [31:0]  x_araddr,
    output wire [3:0]   x_arlen,
    output wire         x_rvalid,
    output wire [127:0] x_rdata,
    output wire         x_rlast,
    output wire         x_awvalid,
    output wire         x_awready,
    output wire [31:0]  x_awaddr,
    output wire [3:0]   x_awlen,
    output wire         x_wvalid,
    output wire         x_wready,
    output wire [127:0] x_wdata,
    output wire [15:0]  x_wstrb,
    output wire         x_wlast,

    output wire         busy
);
    wire [1:0]  x_rresp;
    wire        bl_arvalid, bl_arready, bl_rvalid, bl_rready, bl_rlast;
    wire [31:0] bl_araddr, bl_rdata;
    wire [7:0]  bl_arlen;
    wire [2:0]  bl_arsize;
    wire [1:0]  bl_arburst, bl_rresp, hd_rresp, n_rresp, n_bresp;
    wire [3:0]  bl_arid, bl_rid, hd_rid;
    wire        n_bvalid, n_bready;

    blit_axi_rd_arb u_arb (
        .clk(clk), .rst(rst),
        .m0_arvalid(hd_arvalid), .m0_arready(hd_arready), .m0_araddr(hd_araddr),
        .m0_arlen(hd_arlen), .m0_arsize(3'd2), .m0_arburst(2'd1),
        .m0_rvalid(hd_rvalid), .m0_rdata(hd_rdata), .m0_rlast(hd_rlast),
        .m0_rresp(hd_rresp), .m0_rid(hd_rid),
        .m1_arvalid(bl_arvalid), .m1_arready(bl_arready), .m1_araddr(bl_araddr),
        .m1_arlen(bl_arlen), .m1_arsize(bl_arsize), .m1_arburst(bl_arburst),
        .m1_rvalid(bl_rvalid), .m1_rdata(bl_rdata), .m1_rlast(bl_rlast),
        .m1_rresp(bl_rresp), .m1_rid(bl_rid),
        .s_arvalid(n_arvalid), .s_arready(n_arready), .s_araddr(n_araddr),
        .s_arlen(n_arlen), .s_arsize(n_arsize), .s_arburst(n_arburst), .s_arid(n_arid),
        .s_rvalid(n_rvalid), .s_rready(n_rready), .s_rdata(n_rdata),
        .s_rlast(n_rlast), .s_rresp(n_rresp), .s_rid(n_rid)
    );

    blit_engine #(.CQ_AW(4), .RING_AW(10), .WF_AW(9), .AXI_ID(4'd1)) u_blit (
        .clk(clk), .rst(rst),
        .apb_paddr(apb_paddr), .apb_psel(apb_psel), .apb_penable(apb_penable),
        .apb_pwrite(apb_pwrite), .apb_pwdata(apb_pwdata), .apb_prdata(apb_prdata),
        .apb_pready(apb_pready), .apb_pslverror(apb_pslverror),
        .m_arvalid(bl_arvalid), .m_arready(bl_arready), .m_araddr(bl_araddr),
        .m_arlen(bl_arlen), .m_arsize(bl_arsize), .m_arburst(bl_arburst), .m_arid(bl_arid),
        .m_rvalid(bl_rvalid), .m_rready(bl_rready), .m_rdata(bl_rdata),
        .m_rlast(bl_rlast), .m_rresp(bl_rresp),
        .m_awvalid(n_awvalid), .m_awready(n_awready), .m_awaddr(n_awaddr),
        .m_awlen(n_awlen), .m_awsize(n_awsize), .m_awburst(n_awburst), .m_awid(n_awid),
        .m_wvalid(n_wvalid), .m_wready(n_wready), .m_wdata(n_wdata),
        .m_wstrb(n_wstrb), .m_wlast(n_wlast),
        .m_bvalid(n_bvalid), .m_bready(n_bready), .m_bresp(n_bresp),
        .x_arvalid(x_arvalid), .x_arready(x_arready), .x_araddr(x_araddr), .x_arlen(x_arlen),
        .x_rvalid(x_rvalid), .x_rdata(x_rdata), .x_rlast(x_rlast), .x_rresp(x_rresp),
        .x_awvalid(x_awvalid), .x_awready(x_awready), .x_awaddr(x_awaddr), .x_awlen(x_awlen),
        .x_wvalid(x_wvalid), .x_wready(x_wready), .x_wdata(x_wdata),
        .x_wstrb(x_wstrb), .x_wlast(x_wlast),
        .busy(busy)
    );

    blit_axi_wide u_wide (
        .clk(clk), .rst(rst),
        .u_arvalid(n_arvalid), .u_arready(n_arready), .u_araddr(n_araddr),
        .u_arlen(n_arlen), .u_arid(n_arid),
        .u_rvalid(n_rvalid), .u_rready(n_rready), .u_rdata(n_rdata),
        .u_rlast(n_rlast), .u_rresp(n_rresp), .u_rid(n_rid),
        .u_awvalid(n_awvalid), .u_awready(n_awready), .u_awaddr(n_awaddr),
        .u_awlen(n_awlen),
        .u_wvalid(n_wvalid), .u_wready(n_wready), .u_wdata(n_wdata),
        .u_wstrb(n_wstrb), .u_wlast(n_wlast),
        .u_bvalid(n_bvalid), .u_bready(n_bready), .u_bresp(n_bresp),
        .x_arvalid(x_arvalid), .x_arready(x_arready), .x_araddr(x_araddr), .x_arlen(x_arlen),
        .x_rvalid(x_rvalid), .x_rdata(x_rdata), .x_rlast(x_rlast), .x_rresp(x_rresp),
        .x_awvalid(x_awvalid), .x_awready(x_awready), .x_awaddr(x_awaddr), .x_awlen(x_awlen),
        .x_wvalid(x_wvalid), .x_wready(x_wready), .x_wdata(x_wdata),
        .x_wstrb(x_wstrb), .x_wlast(x_wlast),
        .d_arvalid(s_arvalid), .d_arready(s_arready), .d_araddr(s_araddr),
        .d_arlen(s_arlen), .d_arsize(s_arsize), .d_arburst(s_arburst), .d_arid(s_arid),
        .d_rvalid(s_rvalid), .d_rready(s_rready), .d_rdata(s_rdata),
        .d_rlast(s_rlast), .d_rresp(s_rresp),
        .d_awvalid(s_awvalid), .d_awready(s_awready), .d_awaddr(s_awaddr),
        .d_awlen(s_awlen), .d_awsize(s_awsize), .d_awburst(s_awburst), .d_awid(s_awid),
        .d_wvalid(s_wvalid), .d_wready(s_wready), .d_wdata(s_wdata),
        .d_wstrb(s_wstrb), .d_wlast(s_wlast),
        .d_bvalid(s_bvalid), .d_bready(s_bready), .d_bresp(s_bresp)
    );
endmodule
