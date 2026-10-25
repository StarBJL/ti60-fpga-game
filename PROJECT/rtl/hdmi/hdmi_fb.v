// =============================================================================
// hdmi_fb.v  -  DDR framebuffer -> HDMI (DVI) scan-out for Sapphire SoC
//
//   * APB3 slave   : control registers (SoC io_apbSlave_0, CPU base 0xF810_0000)
//   * AXI4 master  : read-only DMA on SoC io_ddrMasters_0 (32-bit, INCR 64-beat)
//   * Async FIFO   : clk (100 MHz) -> pix_clk (25.00 MHz), 1024 x 32 bit
//   * Timing gen   : 640x480@60 or 1280x720@60 (parameters), RGB565 framebuffer
//   * TMDS encode  : hdmi_tmds_ch (hdmi_island.v; video path = encode.v algorithm)
//   * HDMI audio   : data islands (hdmi_island.v), 48 kHz 2ch 16-bit L-PCM,
//                    sample FIFO clk -> pix_clk (256 x 32), fed by audio_mixer or APB
//
// Framebuffer format : RGB565, little endian, 2 pixels / 32-bit word,
//                      pixel (x even) in bits [15:0], (x odd) in bits [31:16]
//                      stride = H_ACT*2 bytes, base must be 4 KB aligned
//
// Register map (byte offsets):
//   0x00 CTRL      [0] enable scan-out (0 = colour-bar test pattern)
//                  [1] force test pattern
//                  [2] vsync IRQ enable (userInterruptA)
//   0x04 FB_ADDR   framebuffer base to show from next frame (write = request flip)
//   0x08 FB_CUR    framebuffer base currently being scanned out (RO)
//   0x0C STATUS    [0] flip_pending (RO, cleared when FB_ADDR is latched)
//                  [1] vblank (RO, live)
//                  [2] underflow sticky (W1C)
//                  [3] frame IRQ pending (W1C)
//   0x10 FRAME_CNT frames started since reset (RO)
//   0x14 RES       {V_ACT[15:0], H_ACT[15:0]} (RO)
//   0x18 UF_CNT    number of lines that had a FIFO underflow (RO)
//   0x1C ID        0x48444D49 "HDMI" (RO)
//   0x20 AUD_CTRL  [0] HDMI mode (0 = plain DVI as before) [1] audio packets
//                  [2] 1 kHz test tone instead of the FIFO [5:4] tone L/R enable
//                  reset 0x33 = HDMI + audio on, tone off
//   0x24 AUD_LEVEL samples in the audio FIFO (clk side view, RO)
//   0x28 AUD_SENT  samples sent to the sink (FIFO, tone or silence) (RO)
//   0x2C AUD_UF    samples sent as silence because the FIFO was empty (RO)
//   0x30 AUD_DATA  W: push one sample {R[15:0], L[15:0]} (ignored when full)
//   0x50 MODE      W[0] requested 1080p30, R[1] active, R[2] pending
//   0x54 VIDEO_ID  "VID5" when dynamic 720p60/1080p30 is supported
//   0x58 CANVAS    fixed render dimensions (1280x720)
//   0x34 AUD_CAPS  [0] audio present, [15:8] FIFO address bits (RO)
//
// A new frame is latched at the END of the vsync pulse: FB_ADDR -> FB_CUR,
// FRAME_CNT++, flip_pending=0, IRQ pending=1, and the DMA starts prefetching.
// =============================================================================
`timescale 1ns / 1ps

module hdmi_fb_top #(
    parameter SYSTEM_CLOCK_HZ = 200000000,
    parameter DUAL_MODE = 0,              // 720p canvas -> 720p60 or scaled 1080p30
    parameter H_ACT  = 640,
    parameter H_FP   = 16,
    parameter H_SYNC = 96,
    parameter H_BP   = 42,                 // 48 in VESA; 42 -> 794x525 @ 25.000 MHz = 59.97 Hz
    parameter V_ACT  = 480,
    parameter V_FP   = 10,
    parameter V_SYNC = 2,
    parameter V_BP   = 33,
    parameter HS_POL = 1'b0,               // 0 = negative sync pulse
    parameter VS_POL = 1'b0,
    parameter [31:0] DEFAULT_FB = 32'h0100_0000,
    parameter FIFO_AW = 10,                // 1024 words (720p: 11)
    parameter AUD_AW  = 8,                 // audio FIFO 256 samples
    parameter [19:0] ACR_CTS = 20'd25000,  // pixel clock / 1 kHz (720p board PLL 74.375 MHz: 74375)
    parameter [7:0]  AVI_VIC = 8'd1,       // 1 = 640x480p60, 4 = 1280x720p60
    parameter [7:0]  AVI_PB2 = 8'h18       // 0x18 = 4:3, 0x28 = 16:9
)(
    // ---------------- system clock domain (APB + AXI, same clock) ----------
    output wire        link_tx_enable,    // lane OE: defaults to original always-on behavior
    output wire        link_tx_reset,     // all four serializers reset together
    input  wire        clk,
    input  wire        rst,                // APB/system reset, active high
    input  wire        axi_rst,            // io_ddrMasters_0_reset, active high

    input  wire [15:0] apb_paddr,
    input  wire        apb_psel,
    input  wire        apb_penable,
    input  wire        apb_pwrite,
    input  wire [31:0] apb_pwdata,
    output reg  [31:0] apb_prdata,
    output wire        apb_pready,
    output wire        apb_pslverror,
    output wire        irq,

    output reg         axi_arvalid,
    input  wire        axi_arready,
    output reg  [31:0] axi_araddr,
    output wire [7:0]  axi_arlen,
    output wire [2:0]  axi_arsize,
    output wire [1:0]  axi_arburst,
    output wire [3:0]  axi_arid,
    input  wire        axi_rvalid,
    output wire        axi_rready,
    input  wire [31:0] axi_rdata,
    input  wire        axi_rlast,
    input  wire [1:0]  axi_rresp,
    input  wire [3:0]  axi_rid,

    // ---------------- pixel clock domain -----------------------------------
    input  wire        pix_clk,
    input  wire        pix_rst_n,          // e.g. hdmi PLL locked

    // ---------------- audio sample input (clk domain) ----------------------
    input  wire        aud_wvalid,         // push one sample when aud_wready
    output wire        aud_wready,
    input  wire [31:0] aud_wdata,          // [15:0] left, [31:16] right (signed)
    output wire [AUD_AW:0] aud_level,      // samples in the FIFO
    output wire [9:0]  tmds_clk,
    output wire [9:0]  tmds_d0,
    output wire [9:0]  tmds_d1,
    output wire [9:0]  tmds_d2
);

    localparam H_TOT        = H_ACT + H_FP + H_SYNC + H_BP;
    localparam V_TOT        = V_ACT + V_FP + V_SYNC + V_BP;
    localparam BURST_LEN    = 64;                          // beats (32-bit)
    localparam BURST_BYTES  = BURST_LEN * 4;               // 256
    localparam FRAME_BYTES  = H_ACT * V_ACT * 2;
    localparam FRAME_BURSTS = FRAME_BYTES / BURST_BYTES;   // 2400 @ 640x480, 7200 @ 1280x720
    localparam FIFO_DEPTH   = (1 << FIFO_AW);
    localparam [15:0] H_ACT16 = H_ACT;
    localparam [15:0] V_ACT16 = V_ACT;
    localparam [7:0]  AUD_AW8 = AUD_AW;

    // =========================================================================
    // Pixel domain : reset, timing, FIFO read, pixel pipeline, TMDS encoding
    // =========================================================================
    wire pix_rst;
    vfb_rst_sync u_pix_rst (.clk(pix_clk), .arst_n(pix_rst_n), .srst(pix_rst));

    // control bits crossing clk -> pix (quasi static)
    reg  ctrl_en, ctrl_test, ctrl_irqen;
    reg  [5:0] aud_ctrl;
    (* async_reg = "true" *) reg [1:0] en_p_s, test_p_s;
    (* async_reg = "true" *) reg [5:0] actl_p1, actl_p2;
    always @(posedge pix_clk) begin
        en_p_s   <= {en_p_s[0],   ctrl_en};
        test_p_s <= {test_p_s[0], ctrl_test};
        actl_p1  <= aud_ctrl;
        actl_p2  <= actl_p1;
    end
    wire en_p   = en_p_s[1];
    wire test_p = test_p_s[1];

    // R5.8 link restart is independent of timing/DMA/FIFO state. Stopping all
    // four output lanes lets a sink reacquire after a mode change. Counters and
    // normal frame flushing continue; never reset one side of an active FIFO.
    reg [1:0] link_ctrl; // bit0 hold all TMDS lanes, bit1 1080-only DVI diagnostic
    (* async_reg = "true" *) reg [1:0] link_p1,link_p2;
    (* async_reg = "true" *) reg [2:0] link_s1,link_s2;
    reg [3:0] link_run;
    always @(posedge pix_clk) begin
        if(pix_rst)begin link_p1<=0;link_p2<=0;link_run<=4'hf;end
        else begin
            link_p1<=link_ctrl;link_p2<=link_p1;
            if(link_p2[0])link_run<=0;
            else link_run<={link_run[2:0],1'b1};
        end
    end
    assign link_tx_reset=link_p2[0];
    assign link_tx_enable=link_run[3]&~link_p2[0];
    always @(posedge clk) begin
        if(rst)begin link_s1<=3'b100;link_s2<=3'b100;end
        else begin link_s1<={link_tx_enable,link_p2};link_s2<=link_s1;end
    end

    // Single-bit request/ack CDC. Commit only inside vertical sync; keep the
    // 1280x720 framebuffer and DMA burst count unchanged in both output modes.
    reg mode_req;
    (* async_reg = "true" *) reg [1:0] mode_req_p;
    (* async_reg = "true" *) reg [1:0] mode_ack_s;
    reg mode_1080;
    always @(posedge pix_clk) begin
        if (pix_rst) mode_req_p <= 0;
        else mode_req_p <= {mode_req_p[0], mode_req};
    end
    always @(posedge clk) begin
        if (rst) mode_ack_s <= 0;
        else mode_ack_s <= {mode_ack_s[0], mode_1080};
    end
    wire [11:0] out_hact = mode_1080 ? 12'd1920 : H_ACT;
    wire [11:0] out_vact = mode_1080 ? 12'd1080 : V_ACT;
    wire [11:0] out_htot = mode_1080 ? 12'd2200 : H_TOT;
    wire [11:0] out_vtot = mode_1080 ? 12'd1125 : V_TOT;
    wire [11:0] out_hss = mode_1080 ? 12'd2008 : H_ACT+H_FP;
    wire [11:0] out_hse = mode_1080 ? 12'd2052 : H_ACT+H_FP+H_SYNC;
    wire [11:0] out_vss = mode_1080 ? 12'd1084 : V_ACT+V_FP;
    wire [11:0] out_vse = mode_1080 ? 12'd1089 : V_ACT+V_FP+V_SYNC;
    reg [11:0] h_cnt, v_cnt;
    always @(posedge pix_clk) begin
        if (pix_rst) begin
            h_cnt <= 12'd0;
            v_cnt <= 12'd0;
            mode_1080 <= 1'b0;
        end else if (DUAL_MODE && h_cnt == 0 && v_cnt == out_vss &&
                     mode_req_p[1] != mode_1080) begin
            mode_1080 <= mode_req_p[1];
            h_cnt <= 0;
            v_cnt <= mode_req_p[1] ? 12'd1084 : V_ACT+V_FP;
        end else if (h_cnt == out_htot - 1) begin
            h_cnt <= 12'd0;
            v_cnt <= (v_cnt == out_vtot - 1) ? 12'd0 : v_cnt + 12'd1;
        end else begin
            h_cnt <= h_cnt + 12'd1;
        end
    end

    wire de0 = (h_cnt < out_hact) && (v_cnt < out_vact);
    wire hs0 = (h_cnt >= out_hss) && (h_cnt < out_hse);
    wire vs0 = (v_cnt >= out_vss) && (v_cnt < out_vse);
    wire vb0 = (v_cnt >= out_vact);
    // CTA progressive timing: the VSYNC edges coincide with the leading
    // HSYNC edge in blanking, not the beginning of the active-coordinate row.
    // Keep vs0 for the existing internal DMA flush / frame latch interval.
    wire vs_tx0 = (v_cnt == out_vss - 1'b1) ? (h_cnt >= out_hss) :
                  (v_cnt == out_vse - 1'b1) ? (h_cnt < out_hss) : vs0;

    // stage-1 registered timing
    reg        de1, hs1, vs1, vb1, hodd1, uf1;
    reg [11:0] h1, v1;
    reg        vs_zone;          // = vsync pulse, used as FIFO flush window
    reg        frame_en;         // scan-out enabled for this frame

    wire        fifo_rempty;
    wire [31:0] fifo_rdata;
    // 3:2 nearest-neighbour scaler: each source pair -> A,A,B, source rows
    // even,even,odd. Only the repeated row reads this one-line cache (640x32).
    reg [1:0] scale_x, scale_y;
    reg [9:0] scale_word;
    always @(posedge pix_clk) begin
        if (pix_rst) begin scale_x<=0; scale_y<=0; scale_word<=0; end
        else begin
            if (h_cnt >= out_hact) begin scale_x<=0; scale_word<=0; end
            else if (scale_x == 2) begin scale_x<=0; scale_word<=scale_word+1'b1; end
            else scale_x<=scale_x+1'b1;
            if (v_cnt >= out_vact) scale_y<=0;
            else if (h_cnt == out_htot-1) scale_y <= scale_y==2 ? 0 : scale_y+1'b1;
        end
    end
    wire repeat_line = mode_1080 && scale_y==1;
    wire word_start = mode_1080 ? scale_x==0 : ~h_cnt[0];
    wire source_req = de0 & word_start & frame_en & ~repeat_line;
    wire fifo_ren = source_req & ~fifo_rempty;
    wire uf0 = source_req & fifo_rempty;
    reg cache_write_d, cache_bad_d, repeat_d, uf_word;
    reg [9:0] cache_addr_d;
    wire [15:0] cache_rdata;
    generate if (DUAL_MODE) begin : g_scale
        // Explicit 512x16 banks avoid Efinity rounding a 1280-entry array to
        // 2048 entries (four RAM10s). Source pixels 0..1279 use three RAM10s.
        reg [15:0] line_cache0 [0:511];
        reg [15:0] line_cache1 [0:511];
        reg [15:0] line_cache2 [0:511];
        reg [15:0] cache_pixel0, cache_pixel1, cache_pixel2;
        reg [1:0] cache_read_bank;
        reg cache_high_d;
        wire [10:0] cache_raddr = {scale_word,(scale_x==2)};
        wire [10:0] cache_waddr = {cache_addr_d,~cache_write_d};
        wire [15:0] cache_wdata = cache_bad_d ? 16'd0 : (cache_write_d ? fifo_rdata[15:0] : fifo_rdata[31:16]);
        always @(posedge pix_clk) begin
            if (pix_rst) cache_high_d<=0;
            else cache_high_d<=cache_write_d;
            if ((cache_write_d || cache_high_d) && cache_waddr[10:9]==0)
                line_cache0[cache_waddr[8:0]] <= cache_wdata;
            if ((cache_write_d || cache_high_d) && cache_waddr[10:9]==1)
                line_cache1[cache_waddr[8:0]] <= cache_wdata;
            if ((cache_write_d || cache_high_d) && cache_waddr[10:9]==2)
                line_cache2[cache_waddr[8:0]] <= cache_wdata;
            if (de0 && repeat_line) begin
                cache_pixel0 <= line_cache0[cache_raddr[8:0]];
                cache_pixel1 <= line_cache1[cache_raddr[8:0]];
                cache_pixel2 <= line_cache2[cache_raddr[8:0]];
                cache_read_bank <= cache_raddr[10:9];
            end
        end
        assign cache_rdata = cache_read_bank==0 ? cache_pixel0 :
                             cache_read_bank==1 ? cache_pixel1 : cache_pixel2;
    end else begin : g_no_scale
        assign cache_rdata = 16'd0;
    end endgenerate
    always @(posedge pix_clk) begin
        if (pix_rst) begin
            cache_write_d<=0; cache_bad_d<=0; cache_addr_d<=0; repeat_d<=0; uf_word<=0;
        end else begin
            cache_write_d <= source_req & mode_1080;
            if (source_req) begin cache_bad_d <= uf0; cache_addr_d <= scale_word; end
            repeat_d <= repeat_line;
            if (word_start) uf_word <= uf0;
        end
    end

    always @(posedge pix_clk) begin
        if (pix_rst) begin
            de1 <= 1'b0; hs1 <= 1'b0; vs1 <= 1'b0; vb1 <= 1'b0;
            hodd1 <= 1'b0; uf1 <= 1'b0; h1 <= 12'd0; v1 <= 12'd0;
            vs_zone <= 1'b0; frame_en <= 1'b0;
        end else begin
            de1   <= de0;
            hs1   <= hs0;
            vs1   <= vs_tx0;
            vb1   <= vb0;
            hodd1 <= mode_1080 ? scale_x==2 : h_cnt[0];
            uf1   <= word_start ? uf0 : uf_word;
            h1    <= h_cnt;
            v1    <= v_cnt;
            vs_zone <= vs0;
            // new frame configuration latched when the flush window ends
            if (vs_zone & ~vs0)
                frame_en <= en_p & ~test_p;
        end
    end

    // underflow: flag per line, toggle to system domain once per bad line
    reg uf_line, uf_tog;
    always @(posedge pix_clk) begin
        if (pix_rst) begin
            uf_line <= 1'b0;
            uf_tog  <= 1'b0;
        end else if (h_cnt == out_hact) begin
            if (uf_line) uf_tog <= ~uf_tog;
            uf_line <= 1'b0;
        end else if (uf0) begin
            uf_line <= 1'b1;
        end
    end

    // stage-1 pixel select (fifo_rdata valid one clock after fifo_ren)
    wire [15:0] pix565 = repeat_d ? cache_rdata : (hodd1 ? fifo_rdata[31:16] : fifo_rdata[15:0]);

    // colour bar test pattern (128 px wide bars)
    reg [23:0] bar_rgb;
    always @(*) begin
        case (h1[9:7])
            3'd0: bar_rgb = 24'hFFFFFF;
            3'd1: bar_rgb = 24'hFFFF00;
            3'd2: bar_rgb = 24'h00FFFF;
            3'd3: bar_rgb = 24'h00FF00;
            3'd4: bar_rgb = 24'hFF00FF;
            3'd5: bar_rgb = 24'hFF0000;
            3'd6: bar_rgb = 24'h0000FF;
            default: bar_rgb = 24'h000000;
        endcase
        if (v1[8:5] == 4'hE)          // grey ramp in the bottom 32 lines
            bar_rgb = {3{h1[9:2]}};
    end

    // HDMI data islands / preambles / guard bands (stage-1 aligned)
    wire        vpre1, vgb1, ipre1, igb1, idat1, ifirst1, hbit1;
    wire [3:0]  sbe1, sbo1;
    wire        afifo_ren, afifo_rempty;
    wire [31:0] afifo_rdata;
    wire        aud_smp_tog, aud_uf_tog;

    hdmi_island #(
        .H_ACT (H_ACT), .H_TOT (H_TOT), .V_ACT (V_ACT), .V_TOT (V_TOT),
        .DUAL_MODE (DUAL_MODE), .ACR_CTS (ACR_CTS), .AVI_VIC (AVI_VIC), .AVI_PB2 (AVI_PB2)
    ) u_island (
        .mode_1080   (mode_1080),
        .clk         (pix_clk),
        .rst         (pix_rst),
        .h_cnt       (h_cnt),
        .v_cnt       (v_cnt),
        .hdmi_en     (actl_p2[0] & ~(link_p2[1] & mode_1080)),
        .aud_en      (actl_p2[1]),
        .tone_en     (actl_p2[2]),
        .tone_lr     (actl_p2[5:4]),
        .fifo_ren    (afifo_ren),
        .fifo_rempty (afifo_rempty),
        .fifo_rdata  (afifo_rdata),
        .smp_tog     (aud_smp_tog),
        .uf_tog      (aud_uf_tog),
        .vpre1       (vpre1),
        .vgb1        (vgb1),
        .ipre1       (ipre1),
        .igb1        (igb1),
        .idat1       (idat1),
        .ifirst1     (ifirst1),
        .hbit1       (hbit1),
        .sbe1        (sbe1),
        .sbo1        (sbo1)
    );

    // stage-2 : RGB888 + per-channel symbol type into the encoders
    //   mode 0 control (c), 1 video, 2 TERC4 (t), 3 fixed symbol (f)
    reg [7:0] r2, g2, b2;
    reg       de2, hs2, vs2;
    reg [1:0] m0_2, m1_2, m2_2;
    reg [1:0] c0_2, c1_2, c2_2;
    reg [3:0] t0_2, t1_2, t2_2;
    reg [9:0] f0_2, f1_2, f2_2;
    wire      hs1l = hs1 ? HS_POL : ~HS_POL;
    wire      vs1l = vs1 ? VS_POL : ~VS_POL;
    localparam [9:0] GB_A = 10'b1011001100;   // video guard ch0 / ch2
    localparam [9:0] GB_B = 10'b0100110011;   // video guard ch1, island guard ch1 / ch2
    always @(posedge pix_clk) begin
        if (pix_rst) begin
            m0_2 <= 2'd0; m1_2 <= 2'd0; m2_2 <= 2'd0;
            c0_2 <= 2'd0; c1_2 <= 2'd0; c2_2 <= 2'd0;
            t0_2 <= 4'd0; t1_2 <= 4'd0; t2_2 <= 4'd0;
            f0_2 <= GB_A; f1_2 <= GB_B; f2_2 <= GB_A;
        end else begin
            c0_2 <= {vs1l, hs1l};
            c1_2 <= {1'b0, vpre1 | ipre1};     // CTL1, CTL0
            c2_2 <= {1'b0, ipre1};             // CTL3, CTL2
            t0_2 <= {~(igb1 | ifirst1) & idat1 | igb1, igb1 | hbit1, vs1l, hs1l};
            t1_2 <= sbe1;
            t2_2 <= sbo1;
            f0_2 <= GB_A;
            f1_2 <= GB_B;
            f2_2 <= vgb1 ? GB_A : GB_B;
            if (de1) begin
                m0_2 <= 2'd1; m1_2 <= 2'd1; m2_2 <= 2'd1;
            end else if (vgb1) begin
                m0_2 <= 2'd3; m1_2 <= 2'd3; m2_2 <= 2'd3;
            end else if (igb1) begin
                m0_2 <= 2'd2; m1_2 <= 2'd3; m2_2 <= 2'd3;
            end else if (idat1) begin
                m0_2 <= 2'd2; m1_2 <= 2'd2; m2_2 <= 2'd2;
            end else begin
                m0_2 <= 2'd0; m1_2 <= 2'd0; m2_2 <= 2'd0;
            end
        end
    end

    always @(posedge pix_clk) begin
        if (pix_rst) begin
            r2 <= 8'd0; g2 <= 8'd0; b2 <= 8'd0;
            de2 <= 1'b0; hs2 <= ~HS_POL; vs2 <= ~VS_POL;
        end else begin
            de2 <= de1;
            hs2 <= hs1l;
            vs2 <= vs1l;
            if (!de1) begin
                begin r2 <= 8'd0; g2 <= 8'd0; b2 <= 8'd0; end
            end else if (!frame_en) begin
                begin r2 <= bar_rgb[23:16]; g2 <= bar_rgb[15:8]; b2 <= bar_rgb[7:0]; end
            end else if (uf1) begin
                begin r2 <= 8'd0; g2 <= 8'd0; b2 <= 8'd0; end
            end else begin
                r2 <= {pix565[15:11], pix565[15:13]};
                g2 <= {pix565[10:5],  pix565[10:9]};
                b2 <= {pix565[4:0],   pix565[4:2]};
            end
        end
    end

    // channel 0 = blue (+ syncs), 1 = green, 2 = red; same 3-clock latency as dvi_encoder
    hdmi_tmds_ch u_tmds0 (.clk(pix_clk), .rst(pix_rst), .mode(m0_2), .d(b2), .c(c0_2),
                          .t(t0_2), .f(f0_2), .dout(tmds_d0));
    hdmi_tmds_ch u_tmds1 (.clk(pix_clk), .rst(pix_rst), .mode(m1_2), .d(g2), .c(c1_2),
                          .t(t1_2), .f(f1_2), .dout(tmds_d1));
    hdmi_tmds_ch u_tmds2 (.clk(pix_clk), .rst(pix_rst), .mode(m2_2), .d(r2), .c(c2_2),
                          .t(t2_2), .f(f2_2), .dout(tmds_d2));
    assign tmds_clk = 10'b1111100000;

    // =========================================================================
    // Clock-domain crossing pix -> clk
    // =========================================================================
    (* async_reg = "true" *) reg [2:0] vs_s;
    (* async_reg = "true" *) reg [1:0] vb_s;
    (* async_reg = "true" *) reg [2:0] uf_s;
    (* async_reg = "true" *) reg [2:0] asm_s, auf_s;
    always @(posedge clk) begin
        if (rst) begin
            vs_s <= 3'b000;
            vb_s <= 2'b00;
            uf_s <= 3'b000;
            asm_s <= 3'b000;
            auf_s <= 3'b000;
        end else begin
            vs_s <= {vs_s[1:0], vs_zone};
            vb_s <= {vb_s[0],   vb1};
            uf_s <= {uf_s[1:0], uf_tog};
            asm_s <= {asm_s[1:0], aud_smp_tog};
            auf_s <= {auf_s[1:0], aud_uf_tog};
        end
    end
    wire vs_z     = vs_s[1];
    wire vs_rise  = vs_s[1] & ~vs_s[2];
    wire vs_fall  = ~vs_s[1] & vs_s[2];      // frame latch point
    wire vblank_s = vb_s[1];
    wire uf_evt   = uf_s[1] ^ uf_s[2];
    wire asm_evt  = asm_s[1] ^ asm_s[2];
    wire auf_evt  = auf_s[1] ^ auf_s[2];

    // =========================================================================
    // APB3 registers
    // =========================================================================
    reg [31:0] fb_pending, fb_cur, frame_cnt, uf_cnt;
    reg        flip_pending, uf_sticky, irq_pend;
    reg [31:0] aud_sent, aud_uf;
    // Separate extension page: keep 0x20..0x34 HDMI audio ABI intact.
    // PRESENT counts changed framebuffer addresses at the scan latch point,
    // not content hashes. Valid only while real framebuffer scanning is enabled.
    reg [31:0] present_cnt, repeat_cnt, active_scan_cnt;

    wire apb_wr = apb_psel & apb_penable &  apb_pwrite;
    wire aud_apb_push = apb_wr & (apb_paddr[7:2] == 6'h0C);
    assign apb_pready    = 1'b1;
    assign apb_pslverror = 1'b0;
    assign irq           = irq_pend & ctrl_irqen;

    always @(posedge clk) begin
        if (rst) begin
            mode_req     <= 1'b0;
            link_ctrl    <= 2'd0;
            ctrl_en      <= 1'b0;
            ctrl_test    <= 1'b0;
            ctrl_irqen   <= 1'b0;
            fb_pending   <= DEFAULT_FB;
            fb_cur       <= DEFAULT_FB;
            frame_cnt    <= 32'd0;
            uf_cnt       <= 32'd0;
            flip_pending <= 1'b0;
            uf_sticky    <= 1'b0;
            irq_pend     <= 1'b0;
            aud_ctrl     <= 6'h33;
            aud_sent     <= 32'd0;
            aud_uf       <= 32'd0;
            present_cnt  <= 32'd0;
            repeat_cnt   <= 32'd0;
            active_scan_cnt <= 32'd0;
        end else begin
            if (asm_evt) aud_sent <= aud_sent + 32'd1;
            if (auf_evt) aud_uf   <= aud_uf + 32'd1;
            // hardware events
            if (vs_fall) begin
                if (ctrl_en & ~ctrl_test) begin
                    active_scan_cnt <= active_scan_cnt + 32'd1;
                    if (fb_pending != fb_cur) present_cnt <= present_cnt + 32'd1;
                    else repeat_cnt <= repeat_cnt + 32'd1;
                end
                fb_cur       <= fb_pending;
                flip_pending <= 1'b0;
                frame_cnt    <= frame_cnt + 32'd1;
                irq_pend     <= 1'b1;
            end
            if (uf_evt) begin
                uf_cnt    <= uf_cnt + 32'd1;
                uf_sticky <= 1'b1;
            end
            // software writes (take priority)
            if (apb_wr) begin
                case (apb_paddr[7:2])
                    6'h00: begin
                        ctrl_en    <= apb_pwdata[0];
                        ctrl_test  <= apb_pwdata[1];
                        ctrl_irqen <= apb_pwdata[2];
                    end
                    6'h01: begin
                        fb_pending   <= {apb_pwdata[31:8], 8'h00};
                        flip_pending <= 1'b1;
                    end
                    6'h03: begin
                        if (apb_pwdata[2]) uf_sticky <= 1'b0;
                        if (apb_pwdata[3]) irq_pend  <= 1'b0;
                    end
                    6'h08: aud_ctrl <= apb_pwdata[5:0];
                    6'h14: if (DUAL_MODE) mode_req <= apb_pwdata[0];
                    6'h18: link_ctrl <= apb_pwdata[1:0];
                    default: ;
                endcase
            end
        end
    end

    always @(*) begin
        case (apb_paddr[7:2])
            6'h00:   apb_prdata = {29'd0, ctrl_irqen, ctrl_test, ctrl_en};
            6'h01:   apb_prdata = fb_pending;
            6'h02:   apb_prdata = fb_cur;
            6'h03:   apb_prdata = {28'd0, irq_pend, uf_sticky, vblank_s, flip_pending};
            6'h04:   apb_prdata = frame_cnt;
            6'h05:   apb_prdata = mode_ack_s[1] ? {16'd1080,16'd1920} : {V_ACT16, H_ACT16};
            6'h06:   apb_prdata = uf_cnt;
            6'h07:   apb_prdata = 32'h48444D49;
            6'h08:   apb_prdata = {26'd0, aud_ctrl};
            6'h09:   apb_prdata = {{(31-AUD_AW){1'b0}}, aud_level};
            6'h0A:   apb_prdata = aud_sent;
            6'h0B:   apb_prdata = aud_uf;
            6'h0D:   apb_prdata = {16'd0, AUD_AW8, 8'h01};
            6'h10:   apb_prdata = present_cnt;     // +0x40
            6'h11:   apb_prdata = repeat_cnt;      // +0x44
            6'h12:   apb_prdata = active_scan_cnt; // +0x48 = present + repeat
            6'h13:   apb_prdata = 32'h50525332;    // +0x4c "PRS2"
            6'h14:   apb_prdata = {29'd0,(mode_req^mode_ack_s[1]),mode_ack_s[1],mode_req}; // +50
            6'h15:   apb_prdata = DUAL_MODE ? 32'h56494435 : 32'd0; // +54 VID5
            6'h16:   apb_prdata = {V_ACT16,H_ACT16}; // +58 internal canvas
            6'h17:   apb_prdata = 32'h00050800; // +5c R5.8 link restart support
            6'h18:   apb_prdata = {30'd0,link_ctrl}; // +60 requests
            6'h19:   apb_prdata = {29'd0,link_s2}; // +64 active [2] TX enable [1] 1080 DVI [0] hold
            6'h1a:   apb_prdata = 32'h4c4e4b38; // +68 LNK8
            6'h1e:   apb_prdata = SYSTEM_CLOCK_HZ; // +78 physical build clock metadata
            6'h1f:   apb_prdata = 32'h434c4b37; // +7c CLK7
            default: apb_prdata = 32'd0;
        endcase
    end

    // =========================================================================
    // AXI4 read DMA
    // =========================================================================
    wire dma_rst = rst | axi_rst;

    assign axi_arlen   = BURST_LEN - 1;
    assign axi_arsize  = 3'b010;       // 4 bytes / beat
    assign axi_arburst = 2'b01;        // INCR
    assign axi_arid    = 4'd0;         // keep 0 (SoC maps user port to id 0x80)
    assign axi_rready  = 1'b1;         // space is reserved before each AR

    wire [FIFO_AW:0] fifo_wcount;
    reg  [12:0]      outstanding;      // beats requested but not yet received
    reg  [12:0]      bursts_left;
    reg              running, drop_mode, start_pending;

    wire ar_hs = axi_arvalid & axi_arready;
    wire r_hs  = axi_rvalid  & axi_rready;

    wire [13:0] reserved  = {{(13-FIFO_AW){1'b0}}, fifo_wcount} + {1'b0, outstanding} + BURST_LEN;
    wire        credit_ok = (reserved <= FIFO_DEPTH);

    wire start = start_pending & ~vs_z & ~axi_arvalid & (outstanding == 13'd0);

    always @(posedge clk) begin
        if (dma_rst) begin
            axi_arvalid   <= 1'b0;
            axi_araddr    <= DEFAULT_FB;
            outstanding   <= 13'd0;
            bursts_left   <= 13'd0;
            running       <= 1'b0;
            drop_mode     <= 1'b1;
            start_pending <= 1'b0;
        end else begin
            // outstanding beat accounting
            case ({ar_hs, r_hs})
                2'b10:   outstanding <= outstanding + BURST_LEN;
                2'b01:   outstanding <= outstanding - 13'd1;
                2'b11:   outstanding <= outstanding + (BURST_LEN - 1);
                default: ;
            endcase

            if (ar_hs) begin
                axi_arvalid <= 1'b0;
                axi_araddr  <= axi_araddr + BURST_BYTES;
                bursts_left <= bursts_left - 13'd1;
            end

            // frame boundary handling
            if (vs_rise) begin
                running       <= 1'b0;     // stop requesting, drop late data
                drop_mode     <= 1'b1;
                start_pending <= 1'b0;
            end else if (vs_fall) begin
                start_pending <= ctrl_en & ~ctrl_test;
            end else if (start) begin
                start_pending <= 1'b0;
                running       <= 1'b1;
                drop_mode     <= 1'b0;
                axi_araddr    <= fb_cur;   // fb_cur was updated at vs_fall
                bursts_left   <= FRAME_BURSTS;
            end else if (running & ~axi_arvalid & ~vs_z &
                         (bursts_left != 13'd0) & credit_ok) begin
                axi_arvalid <= 1'b1;
            end
        end
    end

    // FIFO write side is flushed during the vsync window
    wire fifo_wrst = dma_rst | vs_z;
    wire fifo_wen  = r_hs & ~drop_mode;

    vfb_async_fifo #(.DW(32), .AW(FIFO_AW)) u_fifo (
        .wclk   (clk),
        .wrst   (fifo_wrst),
        .wen    (fifo_wen),
        .wdata  (axi_rdata),
        .wfull  (),
        .wcount (fifo_wcount),
        .rclk   (pix_clk),
        .rrst   (pix_rst | vs_zone),
        .ren    (fifo_ren),
        .rdata  (fifo_rdata),
        .rempty (fifo_rempty)
    );

    // =========================================================================
    // Audio sample FIFO clk -> pix_clk (BRAM). Writer: APB AUD_DATA (priority)
    // or the mixer port; reader: hdmi_island at 48 kHz.
    // =========================================================================
    wire afifo_wfull;
    assign aud_wready = ~afifo_wfull & ~aud_apb_push;

    vfb_async_fifo #(.DW(32), .AW(AUD_AW)) u_aud_fifo (
        .wclk   (clk),
        .wrst   (rst),
        .wen    (aud_apb_push | (aud_wvalid & aud_wready)),
        .wdata  (aud_apb_push ? apb_pwdata : aud_wdata),
        .wfull  (afifo_wfull),
        .wcount (aud_level),
        .rclk   (pix_clk),
        .rrst   (pix_rst),
        .ren    (afifo_ren),
        .rdata  (afifo_rdata),
        .rempty (afifo_rempty)
    );

endmodule


// =============================================================================
// Asynchronous FIFO (gray-code pointers, 1-clock registered read)
// =============================================================================
module vfb_async_fifo #(
    parameter DW = 32,
    parameter AW = 10
)(
    input  wire          wclk,
    input  wire          wrst,
    input  wire          wen,
    input  wire [DW-1:0] wdata,
    output wire          wfull,
    output wire [AW:0]   wcount,
    input  wire          rclk,
    input  wire          rrst,
    input  wire          ren,
    output reg  [DW-1:0] rdata,
    output wire          rempty
);
    reg [DW-1:0] mem [0:(1<<AW)-1];

    reg  [AW:0] wbin, wgray, rbin, rgray;
    (* async_reg = "true" *) reg [AW:0] rgray_w1, rgray_w2;
    (* async_reg = "true" *) reg [AW:0] wgray_r1, wgray_r2;

    // gray -> binary as a prefix XOR (valid for AW <= 11; kept function-free for vsim.py)
    wire [AW:0] rg = rgray_w2;
    wire [AW:0] rgray_w2_bin = rg ^ (rg >> 1) ^ (rg >> 2) ^ (rg >> 3) ^ (rg >> 4) ^ (rg >> 5) ^
                               (rg >> 6) ^ (rg >> 7) ^ (rg >> 8) ^ (rg >> 9) ^ (rg >> 10) ^ (rg >> 11);

    // ---------------- write side ----------------
    wire        wdo     = wen & ~wfull;
    wire [AW:0] wbin_n  = wbin + {{AW{1'b0}}, wdo};
    wire [AW:0] wgray_n = (wbin_n >> 1) ^ wbin_n;

    assign wfull  = (wgray == {~rgray_w2[AW:AW-1], rgray_w2[AW-2:0]});
    assign wcount = wbin - rgray_w2_bin;

    always @(posedge wclk) begin
        if (wdo) mem[wbin[AW-1:0]] <= wdata;
    end

    always @(posedge wclk) begin
        if (wrst) begin
            wbin <= {(AW+1){1'b0}};  wgray <= {(AW+1){1'b0}};
            rgray_w1 <= {(AW+1){1'b0}};  rgray_w2 <= {(AW+1){1'b0}};
        end else begin
            wbin <= wbin_n;  wgray <= wgray_n;
            rgray_w1 <= rgray;  rgray_w2 <= rgray_w1;
        end
    end

    // ---------------- read side -----------------
    wire        rdo     = ren & ~rempty;
    wire [AW:0] rbin_n  = rbin + {{AW{1'b0}}, rdo};
    wire [AW:0] rgray_n = (rbin_n >> 1) ^ rbin_n;

    assign rempty = (rgray == wgray_r2);

    always @(posedge rclk) begin
        if (rdo) rdata <= mem[rbin[AW-1:0]];
    end

    always @(posedge rclk) begin
        if (rrst) begin
            rbin <= {(AW+1){1'b0}};  rgray <= {(AW+1){1'b0}};
            wgray_r1 <= {(AW+1){1'b0}};  wgray_r2 <= {(AW+1){1'b0}};
        end else begin
            rbin <= rbin_n;  rgray <= rgray_n;
            wgray_r1 <= wgray;  wgray_r2 <= wgray_r1;
        end
    end
endmodule


// =============================================================================
// Reset synchroniser: async assert, sync de-assert. srst active high.
// =============================================================================
module vfb_rst_sync (
    input  wire clk,
    input  wire arst_n,
    output wire srst
);
    (* async_reg = "true" *) reg [3:0] sr;
    always @(posedge clk or negedge arst_n) begin
        if (!arst_n) sr <= 4'b0000;
        else         sr <= {sr[2:0], 1'b1};
    end
    assign srst = ~sr[3];
endmodule
