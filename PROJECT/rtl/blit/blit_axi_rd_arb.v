// =============================================================================
// blit_axi_rd_arb.v - share the read half of io_ddrMasters_0 between the HDMI
// scan-out DMA (m0, fixed priority) and the BitBlt engine (m1).
//
// * AR: m0 wins whenever it requests; a pending (valid, not yet ready) request
//   keeps its grant so AR stays stable as AXI requires.
// * The downstream ARID is {3'b0, master}; R beats are routed back by RID[0].
//   Both masters reserve buffer space before issuing AR and keep RREADY = 1,
//   so the shared R channel never stalls.
// * AW / W / B are not touched (only the blitter writes).
// =============================================================================
`timescale 1ns / 1ps

module blit_axi_rd_arb (
    input  wire        clk,
    input  wire        rst,

    // m0 : HDMI scan-out (high priority)
    input  wire        m0_arvalid,
    output wire        m0_arready,
    input  wire [31:0] m0_araddr,
    input  wire [7:0]  m0_arlen,
    input  wire [2:0]  m0_arsize,
    input  wire [1:0]  m0_arburst,
    output wire        m0_rvalid,
    output wire [31:0] m0_rdata,
    output wire        m0_rlast,
    output wire [1:0]  m0_rresp,
    output wire [3:0]  m0_rid,

    // m1 : BitBlt engine
    input  wire        m1_arvalid,
    output wire        m1_arready,
    input  wire [31:0] m1_araddr,
    input  wire [7:0]  m1_arlen,
    input  wire [2:0]  m1_arsize,
    input  wire [1:0]  m1_arburst,
    output wire        m1_rvalid,
    output wire [31:0] m1_rdata,
    output wire        m1_rlast,
    output wire [1:0]  m1_rresp,
    output wire [3:0]  m1_rid,

    // shared slave port (io_ddrMasters_0 read channels)
    output wire        s_arvalid,
    input  wire        s_arready,
    output wire [31:0] s_araddr,
    output wire [7:0]  s_arlen,
    output wire [2:0]  s_arsize,
    output wire [1:0]  s_arburst,
    output wire [3:0]  s_arid,
    input  wire        s_rvalid,
    output wire        s_rready,
    input  wire [31:0] s_rdata,
    input  wire        s_rlast,
    input  wire [1:0]  s_rresp,
    input  wire [3:0]  s_rid
);

    reg  lock, lock_sel;
    wire sel = lock ? lock_sel : ~m0_arvalid;     // 0 = HDMI, 1 = blitter

    assign s_arvalid  = sel ? m1_arvalid : m0_arvalid;
    assign s_araddr   = sel ? m1_araddr  : m0_araddr;
    assign s_arlen    = sel ? m1_arlen   : m0_arlen;
    assign s_arsize   = sel ? m1_arsize  : m0_arsize;
    assign s_arburst  = sel ? m1_arburst : m0_arburst;
    assign s_arid     = {3'b000, sel};
    assign m0_arready = s_arready & ~sel;
    assign m1_arready = s_arready &  sel;

    always @(posedge clk) begin
        if (rst) begin
            lock     <= 1'b0;
            lock_sel <= 1'b0;
        end else begin
            lock     <= s_arvalid & ~s_arready;
            lock_sel <= sel;
        end
    end

    assign s_rready  = 1'b1;
    assign m0_rvalid = s_rvalid & ~s_rid[0];
    assign m1_rvalid = s_rvalid &  s_rid[0];
    assign m0_rdata  = s_rdata;
    assign m1_rdata  = s_rdata;
    assign m0_rlast  = s_rlast;
    assign m1_rlast  = s_rlast;
    assign m0_rresp  = s_rresp;
    assign m1_rresp  = s_rresp;
    assign m0_rid    = 4'd0;
    assign m1_rid    = 4'd1;

endmodule
