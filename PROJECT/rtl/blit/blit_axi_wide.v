// =============================================================================
// blit_axi_wide.v - 32-bit <-> 128-bit bridge in front of io_ddrMasters_0.
//
// The SoC port is generated with AXIMasterWidth = 128 (= the DDR controller
// width), so the SoC no longer puts its own Axi4Upsizer behind the clock-
// crossing FIFOs.  That upsizer handed the controller one 128-bit beat every
// 4 clocks (it packs 32-bit words at 1/clk), which kept the controller's
// write path busy 4x longer than needed (bg COPY 2.06 clk/word on the board).
// Here the packing happens before the SoC FIFOs and the AW is only released
// once the whole burst is in them, so the controller sees full-rate W beats.
// The engine and the HDMI DMA keep their 32-bit interfaces; no block RAM.
//
// * AR: passed through (addr aligned down to 16 B, len in 128-bit beats,
//   size 4, ARID 0 so every burst returns in order).  One queue entry per
//   burst {id[0], first lane, 32-bit len} drives the R serializer.
// * R: 128-bit beats are split into 32-bit words, 1 word/clk, starting at the
//   burst's first lane; RREADY is given on lane 3 or the last word, the unused
//   lanes are dropped.  RID = {3'b0, id[0]} as blit_axi_rd_arb expects.
//   The SoC R FIFO (128-bit) absorbs the burst, the controller is not held.
// * AW/W: the engine AW goes into a 4-deep queue; its W words are packed into
//   128-bit beats (strobe 0 on unused lanes) behind one output register.
//   The downstream AW of a burst is issued after its last W beat has been
//   accepted (bursts are <= 16 beats = the SoC W FIFO depth).
//   Fallback: if W has been stalled for 16 clocks with no finished burst
//   waiting for its AW (a slave that wants AW first), the AW of the burst
//   being written is issued early.
// * B: passed through (one B per engine burst, in order).
// Bursts never cross a 256-byte block (engine, HDMI), so 32-bit bursts of
// <= 64 words become <= 16 beats.
// * x_* (C2): 128-bit side channel of the engine for aligned opaque COPY/FILL.
//   x_ar shares AR with u_ar (u first, a pending AR keeps its grant); the
//   read queue marks the burst "wide" and its R beats go out on x_r whole
//   (no RREADY from the engine, it reserved the space). x_aw goes into the
//   same AW queue (wide flag), its W beats bypass the packer straight into
//   the output register, so AW/W/B order stays the issue order.
// =============================================================================
`timescale 1ns / 1ps

module blit_axi_wide #(
    parameter RQ_AW = 4                 // read queue: 2^RQ_AW bursts in flight
) (
    input  wire         clk,
    input  wire         rst,

    // ---- 32-bit side: AR/R from blit_axi_rd_arb, AW/W/B from blit_engine
    input  wire         u_arvalid,
    output wire         u_arready,
    input  wire [31:0]  u_araddr,
    input  wire [7:0]   u_arlen,
    input  wire [3:0]   u_arid,
    output wire         u_rvalid,
    input  wire         u_rready,
    output wire [31:0]  u_rdata,
    output wire         u_rlast,
    output wire [1:0]   u_rresp,
    output wire [3:0]   u_rid,

    input  wire         u_awvalid,
    output wire         u_awready,
    input  wire [31:0]  u_awaddr,
    input  wire [7:0]   u_awlen,
    input  wire         u_wvalid,
    output wire         u_wready,
    input  wire [31:0]  u_wdata,
    input  wire [3:0]   u_wstrb,
    input  wire         u_wlast,
    output wire         u_bvalid,
    input  wire         u_bready,
    output wire [1:0]   u_bresp,

    // ---- 128-bit side channel from blit_engine (C2)
    input  wire         x_arvalid,
    output wire         x_arready,
    input  wire [31:0]  x_araddr,           // 16-byte aligned
    input  wire [3:0]   x_arlen,            // beats - 1
    output wire         x_rvalid,           // always accepted
    output wire [127:0] x_rdata,
    output wire         x_rlast,
    output wire [1:0]   x_rresp,
    input  wire         x_awvalid,
    output wire         x_awready,
    input  wire [31:0]  x_awaddr,
    input  wire [3:0]   x_awlen,
    input  wire         x_wvalid,
    output wire         x_wready,
    input  wire [127:0] x_wdata,
    input  wire [15:0]  x_wstrb,
    input  wire         x_wlast,

    // ---- 128-bit side: io_ddrMasters_0
    output wire         d_arvalid,
    input  wire         d_arready,
    output wire [31:0]  d_araddr,
    output wire [7:0]   d_arlen,
    output wire [2:0]   d_arsize,
    output wire [1:0]   d_arburst,
    output wire [3:0]   d_arid,
    input  wire         d_rvalid,
    output wire         d_rready,
    input  wire [127:0] d_rdata,
    input  wire         d_rlast,
    input  wire [1:0]   d_rresp,

    output wire         d_awvalid,
    input  wire         d_awready,
    output wire [31:0]  d_awaddr,
    output wire [7:0]   d_awlen,
    output wire [2:0]   d_awsize,
    output wire [1:0]   d_awburst,
    output wire [3:0]   d_awid,
    output wire         d_wvalid,
    input  wire         d_wready,
    output wire [127:0] d_wdata,
    output wire [15:0]  d_wstrb,
    output wire         d_wlast,
    input  wire         d_bvalid,
    output wire         d_bready,
    input  wire [1:0]   d_bresp
);

    // =========================================================================
    // AR + R
    // =========================================================================
    reg  [11:0]      rq_mem [0:(1<<RQ_AW)-1];      // {wide, id[0], lane0[1:0], len[7:0]}
    reg  [RQ_AW:0]   rq_wp, rq_rp;
    wire [RQ_AW:0]   rq_used  = rq_wp - rq_rp;
    wire             rq_full  = rq_used[RQ_AW];
    wire             rq_empty = (rq_used == {(RQ_AW+1){1'b0}});
    wire [11:0]      rq_head  = rq_mem[rq_rp[RQ_AW-1:0]];

    // AR select: u_ar first; an AR offered but not yet taken keeps its source
    reg              ar_lock, ar_lsel;
    wire             ar_xsel  = ar_lock ? ar_lsel : ~u_arvalid;
    wire [8:0]       ar_end   = {7'd0, u_araddr[3:2]} + {1'b0, u_arlen};
    wire             ar_hs    = d_arvalid & d_arready;

    assign u_arready = d_arready & ~rq_full & ~ar_xsel;
    assign x_arready = d_arready & ~rq_full & ar_xsel;
    assign d_arvalid = (ar_xsel ? x_arvalid : u_arvalid) & ~rq_full;
    assign d_araddr  = ar_xsel ? {x_araddr[31:4], 4'd0} : {u_araddr[31:4], 4'd0};
    assign d_arlen   = ar_xsel ? {4'd0, x_arlen} : {1'b0, ar_end[8:2]};
    assign d_arsize  = 3'b100;
    assign d_arburst = 2'b01;
    assign d_arid    = 4'd0;

    reg              r_act;                         // inside a burst (lane/left valid)
    reg  [1:0]       r_lane;
    reg  [7:0]       r_left;
    wire [1:0]       r_cl  = r_act ? r_lane : rq_head[9:8];
    wire [7:0]       r_cn  = r_act ? r_left : rq_head[7:0];
    wire             r_end = (r_cn == 8'd0);
    wire             r_hs  = u_rvalid & u_rready;

    wire             r_wide = rq_head[11];

    assign u_rvalid = d_rvalid & ~rq_empty & ~r_wide;
    assign u_rdata  = r_cl[1] ? (r_cl[0] ? d_rdata[127:96] : d_rdata[95:64])
                              : (r_cl[0] ? d_rdata[63:32]  : d_rdata[31:0]);
    assign u_rlast  = r_end;
    assign u_rresp  = d_rresp;
    assign u_rid    = {3'd0, rq_head[10]};
    assign d_rready = ~rq_empty & (r_wide | (u_rready & ((r_cl == 2'd3) | r_end)));

    assign x_rvalid = d_rvalid & ~rq_empty & r_wide;
    assign x_rdata  = d_rdata;
    assign x_rlast  = d_rlast;
    assign x_rresp  = d_rresp;

    always @(posedge clk) begin
        if (ar_hs)
            rq_mem[rq_wp[RQ_AW-1:0]] <= {ar_xsel, u_arid[0], u_araddr[3:2], u_arlen};
    end

    always @(posedge clk) begin
        if (rst) begin
            rq_wp  <= {(RQ_AW+1){1'b0}};
            rq_rp  <= {(RQ_AW+1){1'b0}};
            r_act  <= 1'b0;
            r_lane <= 2'd0;
            r_left <= 8'd0;
            ar_lock <= 1'b0;
            ar_lsel <= 1'b0;
        end else begin
            ar_lock <= d_arvalid & ~d_arready;
            ar_lsel <= ar_xsel;
            if (ar_hs)
                rq_wp <= rq_wp + {{RQ_AW{1'b0}}, 1'b1};
            if (x_rvalid & d_rlast)
                rq_rp <= rq_rp + {{RQ_AW{1'b0}}, 1'b1};
            if (r_hs) begin
                if (r_end) begin
                    r_act <= 1'b0;
                    rq_rp <= rq_rp + {{RQ_AW{1'b0}}, 1'b1};
                end else begin
                    r_act  <= 1'b1;
                    r_lane <= r_cl + 2'd1;
                    r_left <= r_cn - 8'd1;
                end
            end
        end
    end

    // =========================================================================
    // AW + W
    // =========================================================================
    // AW queue {wide, addr[31:4], lane0[1:0], beats-1[3:0]}; an entry is freed
    // when both its W words are packed (aq_pp) and its downstream AW is out (aq_ip)
    reg  [34:0]  aq_mem [0:3];
    reg  [2:0]   aq_wp, aq_pp, aq_ip;
    wire [2:0]   aq_ui   = aq_wp - aq_ip;
    wire [2:0]   aq_up   = aq_wp - aq_pp;
    wire         aq_full = aq_ui[2] | aq_up[2];
    wire [8:0]   aw_end  = {7'd0, u_awaddr[3:2]} + {1'b0, u_awlen};
    wire         uaw_hs  = u_awvalid & u_awready;
    wire         xaw_hs  = x_awvalid & x_awready;
    wire [34:0]  aq_p    = aq_mem[aq_pp[1:0]];      // burst being packed
    wire [34:0]  aq_i    = aq_mem[aq_ip[1:0]];      // next downstream AW

    assign u_awready = ~aq_full;
    assign x_awready = ~aq_full & ~u_awvalid;

    always @(posedge clk) begin
        if (uaw_hs)
            aq_mem[aq_wp[1:0]] <= {1'b0, u_awaddr[31:4], u_awaddr[3:2], aw_end[5:2]};
        else if (xaw_hs)
            aq_mem[aq_wp[1:0]] <= {1'b1, x_awaddr[31:4], 2'b00, x_awlen};
    end

    // W packer: p_* collects lanes, o_* is the beat offered downstream
    reg          p_act;
    reg  [1:0]   p_lane;
    reg  [127:0] p_data;
    reg  [15:0]  p_strb;
    reg          o_valid, o_last;
    reg  [127:0] o_data;
    reg  [15:0]  o_strb;

    wire [1:0]   p_cl   = p_act ? p_lane : aq_p[5:4];
    wire         o_hs   = o_valid & d_wready;
    assign u_wready     = (aq_up != 3'd0) & ~aq_p[34] & (~o_valid | d_wready);
    assign x_wready     = (aq_up != 3'd0) &  aq_p[34] & (~o_valid | d_wready);
    wire         xw_hs  = x_wvalid & x_wready;      // wide beat: straight to o_*
    wire         p_hs   = u_wvalid & u_wready;
    wire         p_beat = p_hs & ((p_cl == 2'd3) | u_wlast);
    wire [127:0] p_dsh  = {96'd0, u_wdata} << {p_cl, 5'd0};
    wire [15:0]  p_ssh  = {12'd0, u_wstrb} << {p_cl, 2'd0};

    assign d_wvalid = o_valid;
    assign d_wdata  = o_data;
    assign d_wstrb  = o_strb;
    assign d_wlast  = o_last;

    // downstream AW: wrc = bursts whose W is complete but AW not yet issued;
    // ep = early AW pending (fallback), ed = the burst now in W already had its AW
    reg  [2:0]   wrc;
    reg          ep, ed;
    reg  [3:0]   w_to;
    wire         wb_done = o_hs & o_last;
    wire         w_stuck = o_valid & ~d_wready & (wrc == 3'd0) & ~ep & ~ed;

    assign d_awvalid = (wrc != 3'd0) | ep;
    assign d_awaddr  = {aq_i[33:6], 4'd0};
    assign d_awlen   = {4'd0, aq_i[3:0]};
    assign d_awsize  = 3'b100;
    assign d_awburst = 2'b01;
    assign d_awid    = 4'd0;
    wire         daw_hs  = d_awvalid & d_awready;

    always @(posedge clk) begin
        if (rst) begin
            aq_wp   <= 3'd0;
            aq_pp   <= 3'd0;
            aq_ip   <= 3'd0;
            p_act   <= 1'b0;
            p_lane  <= 2'd0;
            p_data  <= 128'd0;
            p_strb  <= 16'd0;
            o_valid <= 1'b0;
            o_last  <= 1'b0;
            o_data  <= 128'd0;
            o_strb  <= 16'd0;
            wrc     <= 3'd0;
            ep      <= 1'b0;
            ed      <= 1'b0;
            w_to    <= 4'd0;
        end else begin
            if (uaw_hs | xaw_hs)
                aq_wp <= aq_wp + 3'd1;

            if (xw_hs) begin
                o_data <= x_wdata;
                o_strb <= x_wstrb;
                o_last <= x_wlast;
                if (x_wlast)
                    aq_pp <= aq_pp + 3'd1;
            end
            if (p_hs) begin
                if (p_beat) begin
                    o_data <= p_data | p_dsh;
                    o_strb <= p_strb | p_ssh;
                    o_last <= u_wlast;
                    p_data <= 128'd0;
                    p_strb <= 16'd0;
                    p_lane <= 2'd0;
                    if (u_wlast) begin
                        p_act <= 1'b0;
                        aq_pp <= aq_pp + 3'd1;
                    end else begin
                        p_act <= 1'b1;
                    end
                end else begin
                    p_data <= p_data | p_dsh;
                    p_strb <= p_strb | p_ssh;
                    p_lane <= p_cl + 2'd1;
                    p_act  <= 1'b1;
                end
            end
            if (p_beat | xw_hs)
                o_valid <= 1'b1;
            else if (o_hs)
                o_valid <= 1'b0;

            if (daw_hs)
                aq_ip <= aq_ip + 3'd1;
            if (ep) begin
                if (daw_hs) begin
                    ep <= 1'b0;
                    ed <= ~wb_done;
                end else if (wb_done) begin
                    ep  <= 1'b0;                    // W finished first: normal AW
                    wrc <= 3'd1;
                end
            end else begin
                wrc <= wrc + {2'd0, wb_done & ~ed} - {2'd0, daw_hs};
                if (wb_done & ed)
                    ed <= 1'b0;
                if (w_stuck & (w_to == 4'd15))
                    ep <= 1'b1;
            end
            w_to <= w_stuck ? w_to + 4'd1 : 4'd0;
        end
    end

    // =========================================================================
    // B
    // =========================================================================
    assign u_bvalid = d_bvalid;
    assign u_bresp  = d_bresp;
    assign d_bready = u_bready;

endmodule
