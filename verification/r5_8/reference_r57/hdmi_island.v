// =============================================================================
// hdmi_island.v  -  HDMI data islands + audio for hdmi_fb_top (pixel clock domain)
//
//   hdmi_island   : per-line packet scheduler, packet bit generator with BCH ECC,
//                   48 kHz sample tick, audio sample capture (async FIFO / test
//                   tone), video preamble / guard band positions.
//   hdmi_tmds_ch  : one TMDS channel encoder: video (8b/10b, same algorithm and
//                   3-clock latency as encode.v / XAPP460), control tokens,
//                   TERC4 and fixed 10-bit symbols (guard bands).
//
// Line layout (HDMI mode, h = h_cnt, active video is h < H_ACT):
//   H_ACT+4  .. H_ACT+11   data island preamble   (only when the line has packets)
//   H_ACT+12 .. H_ACT+13   leading guard band
//   H_ACT+14 .. +32n-1     n packets (n = 0..3), TERC4
//   next 2               trailing guard band
//   H_TOT-10 .. H_TOT-3    video preamble         (only before an active line)
//   H_TOT-2  .. H_TOT-1    video guard band
// 640x480 (H_TOT 794): island ends at <= 752, then >= 32 control characters.
// 1280x720 (H_TOT 1650): island ends at <= 1392.
//
// Packets per line (decided at h = H_ACT-8, in this order):
//   audio sample packet  if samples were captured since the last line (1..4)
//   ACR (N 6144, CTS = f_pix / 1 kHz) once per CTS pixel clocks (= 1 ms)
//   AVI InfoFrame on line V_ACT, Audio InfoFrame on line V_ACT+1 (each frame)
// 48 kHz: acc += N per pixel clock, sample when acc reaches 128*CTS
//   -> fs = f_pix * N / (128 * CTS) = 25 MHz * 6144 / 3.2e6 = 48000 exactly
//      (720p on this board: 74.375 MHz, CTS 74375; any f_pix that is a whole
//      number of kHz gives exactly 48 kHz with CTS = f_pix / 1000).
//
// Kept inside the vsim.py subset (no functions / loops / signed).
// =============================================================================
`timescale 1ns / 1ps

module hdmi_island #(
    parameter DUAL_MODE = 0,
    parameter H_ACT = 640,
    parameter H_TOT = 794,
    parameter V_ACT = 480,
    parameter V_TOT = 525,
    parameter [19:0] ACR_N   = 20'd6144,
    parameter [19:0] ACR_CTS = 20'd25000,   // pixel clock / 1 kHz (720p board PLL: 74375)
    parameter [7:0]  AVI_VIC = 8'd1,        // CEA video code: 1 = 640x480p60, 4 = 1280x720p60
    parameter [7:0]  AVI_PB2 = 8'h18        // C M R: 0x18 = 4:3, 0x28 = 16:9 (same as picture)
)(
    input  wire        mode_1080,     // latched in the parent during vertical sync
    input  wire        clk,            // pixel clock
    input  wire        rst,            // sync, active high

    input  wire [11:0] h_cnt,          // stage-0 counters of hdmi_fb_top
    input  wire [11:0] v_cnt,

    input  wire        hdmi_en,        // quasi-static, already synchronised
    input  wire        aud_en,
    input  wire        tone_en,
    input  wire [1:0]  tone_lr,

    // audio sample FIFO, read side (rdata valid the clock after ren)
    output wire        fifo_ren,
    input  wire        fifo_rempty,
    input  wire [31:0] fifo_rdata,     // [15:0] left, [31:16] right, signed

    output reg         smp_tog,        // toggles per sample sent to the sink
    output reg         uf_tog,         // toggles per sample the FIFO could not supply

    // stage-1 aligned outputs (valid together with hdmi_fb_top de1/hs1/vs1)
    output reg         vpre1,          // video preamble
    output reg         vgb1,           // video guard band
    output reg         ipre1,          // data island preamble
    output reg         igb1,           // data island guard band (leading / trailing)
    output reg         idat1,          // data island packet character
    output reg         ifirst1,        // first character of the island (ch0 bit3 = 0)
    output wire        hbit1,          // channel 0 bit 2 (header + ECC)
    output wire [3:0]  sbe1,           // channel 1 bits: subpacket 3..0, even bit
    output wire [3:0]  sbo1            // channel 2 bits: subpacket 3..0, odd bit
);
    wire mode_hi = DUAL_MODE && mode_1080;
    wire [11:0] LATCH_H = mode_hi ? 12'd1912 : H_ACT - 8;
    wire [11:0] IPRE_H  = mode_hi ? 12'd1924 : H_ACT + 4;
    wire [11:0] IGB_H   = mode_hi ? 12'd1932 : H_ACT + 12;
    wire [11:0] IDAT_H  = mode_hi ? 12'd1934 : H_ACT + 14;
    wire [11:0] VPRE_H  = mode_hi ? 12'd2190 : H_TOT - 10;
    wire [11:0] VGB_H   = mode_hi ? 12'd2198 : H_TOT - 2;
    wire [11:0] V_AVI   = mode_hi ? 12'd1080 : V_ACT;
    wire [11:0] V_AIF   = mode_hi ? 12'd1081 : V_ACT + 1;
    wire [11:0] V_LAST  = mode_hi ? 12'd1124 : V_TOT - 1;
    wire [11:0] V_ACTM1 = mode_hi ? 12'd1079 : V_ACT - 1;
    wire [7:0] avi_vic = mode_hi ? 8'd34 : AVI_VIC;
    localparam [23:0] TICK_N   = ACR_N;
    localparam [23:0] TICK_SUB = 128 * ACR_CTS;                // 128*CTS
    localparam [23:0] TICK_TH  = 128 * ACR_CTS - ACR_N;        // 128*CTS - N
    wire [7:0]  AVI_CSUM = 8'd0 - (8'h91 + AVI_PB2 + avi_vic);   // 0x82 + 0x02 + 0x0D = 0x91

    // ---------------------------------------------------------------- 48 kHz tick
    reg  [23:0] acc;
    reg         tick;                  // one pixel clock per audio sample
    always @(posedge clk) begin
        if (rst) begin
            acc  <= 24'd0;
            tick <= 1'b0;
        end else if (acc >= TICK_TH) begin
            acc  <= acc + TICK_N - TICK_SUB;
            tick <= 1'b1;
        end else begin
            acc  <= acc + TICK_N;
            tick <= 1'b0;
        end
    end

    // ---------------------------------------------------------------- ACR timer (1 ms)
    reg  [19:0] acr_cnt;
    reg         acr_due;

    // ---------------------------------------------------------------- sample source
    // tick + tone       -> tone sample this clock
    // tick + FIFO data  -> pop now, capture the next clock (ren_d)
    // tick + FIFO empty -> silence, uf_tog
    reg  [5:0]  tone_ph;
    reg  [15:0] tone_s;
    reg         ren_d;
    wire        use_fifo = aud_en & ~tone_en;
    assign fifo_ren = tick & use_fifo & ~fifo_rempty;

    always @(*) begin
        case (tone_ph)
            6'd0: tone_s = 16'h0000;
            6'd1: tone_s = 16'h042D;
            6'd2: tone_s = 16'h0848;
            6'd3: tone_s = 16'h0C3F;
            6'd4: tone_s = 16'h1000;
            6'd5: tone_s = 16'h137B;
            6'd6: tone_s = 16'h16A1;
            6'd7: tone_s = 16'h1963;
            6'd8: tone_s = 16'h1BB6;
            6'd9: tone_s = 16'h1D90;
            6'd10: tone_s = 16'h1EE9;
            6'd11: tone_s = 16'h1FBA;
            6'd12: tone_s = 16'h2000;
            6'd13: tone_s = 16'h1FBA;
            6'd14: tone_s = 16'h1EE9;
            6'd15: tone_s = 16'h1D90;
            6'd16: tone_s = 16'h1BB6;
            6'd17: tone_s = 16'h1963;
            6'd18: tone_s = 16'h16A1;
            6'd19: tone_s = 16'h137B;
            6'd20: tone_s = 16'h1000;
            6'd21: tone_s = 16'h0C3F;
            6'd22: tone_s = 16'h0848;
            6'd23: tone_s = 16'h042D;
            6'd24: tone_s = 16'h0000;
            6'd25: tone_s = 16'hFBD3;
            6'd26: tone_s = 16'hF7B8;
            6'd27: tone_s = 16'hF3C1;
            6'd28: tone_s = 16'hF000;
            6'd29: tone_s = 16'hEC85;
            6'd30: tone_s = 16'hE95F;
            6'd31: tone_s = 16'hE69D;
            6'd32: tone_s = 16'hE44A;
            6'd33: tone_s = 16'hE270;
            6'd34: tone_s = 16'hE117;
            6'd35: tone_s = 16'hE046;
            6'd36: tone_s = 16'hE000;
            6'd37: tone_s = 16'hE046;
            6'd38: tone_s = 16'hE117;
            6'd39: tone_s = 16'hE270;
            6'd40: tone_s = 16'hE44A;
            6'd41: tone_s = 16'hE69D;
            6'd42: tone_s = 16'hE95F;
            6'd43: tone_s = 16'hEC85;
            6'd44: tone_s = 16'hF000;
            6'd45: tone_s = 16'hF3C1;
            6'd46: tone_s = 16'hF7B8;
            default: tone_s = 16'hFBD3;
        endcase
    end

    wire        cap_tone = tick & aud_en & tone_en;
    wire        cap_zero = tick & use_fifo & fifo_rempty;
    wire        cap      = cap_tone | cap_zero | ren_d;
    wire [15:0] cap_l    = ren_d ? fifo_rdata[15:0]  : (cap_tone & tone_lr[0]) ? tone_s : 16'h0000;
    wire [15:0] cap_r    = ren_d ? fifo_rdata[31:16] : (cap_tone & tone_lr[1]) ? tone_s : 16'h0000;

    // IEC 60958 frame index (0..191) -> B flag and channel status bit
    // channel status: bit 2 = no copyright, bits 24-27 = 0100 (48 kHz),
    // bits 32-35 = 0100 (16-bit word length)
    reg  [7:0]  fidx;
    wire        cap_b = (fidx == 8'd0);
    wire        cap_c = (fidx == 8'd2) | (fidx == 8'd25) | (fidx == 8'd33);

    // ---------------------------------------------------------------- pending samples
    reg  [15:0] pl0, pl1, pl2, pl3, pr0, pr1, pr2, pr3;
    reg         cb0, cb1, cb2, cb3;    // per-slot channel status bit
    reg         bb0, bb1, bb2, bb3;    // per-slot block start
    wire [3:0]  pcb = {cb3, cb2, cb1, cb0};
    wire [3:0]  pbb = {bb3, bb2, bb1, bb0};
    reg  [2:0]  pcnt;

    // packet copy (taken at the line latch)
    reg  [15:0] kl0, kl1, kl2, kl3, kr0, kr1, kr2, kr3;
    reg  [3:0]  kcb, kbb;
    reg  [2:0]  kcnt;

    // ---------------------------------------------------------------- line schedule
    wire        latch   = (h_cnt == LATCH_H);
    wire        s_aud   = aud_en & (pcnt != 3'd0);
    wire        s_acr   = aud_en & acr_due;
    wire        s_avi   = (v_cnt == V_AVI);
    wire        s_aif   = aud_en & (v_cnt == V_AIF);
    wire        s_if    = s_avi | s_aif;
    wire [1:0]  if_ty   = s_avi ? 2'd2 : 2'd3;

    reg  [1:0]  ln;                    // packets on this line (0 = no island)
    reg  [1:0]  ty0, ty1, ty2;         // 0 AUD, 1 ACR, 2 AVI, 3 AIF
    reg         lhdmi;                 // HDMI mode for this line
    reg  [1:0]  slot;                  // next packet to load

    reg  [1:0]  n_ln;
    reg  [1:0]  n_t0, n_t1, n_t2;
    always @(*) begin
        n_t0 = 2'd0; n_t1 = 2'd0; n_t2 = 2'd0; n_ln = 2'd0;
        case ({s_aud, s_acr, s_if})
            3'b100: begin n_ln = 2'd1; n_t0 = 2'd0; end
            3'b010: begin n_ln = 2'd1; n_t0 = 2'd1; end
            3'b001: begin n_ln = 2'd1; n_t0 = if_ty; end
            3'b110: begin n_ln = 2'd2; n_t0 = 2'd0; n_t1 = 2'd1; end
            3'b101: begin n_ln = 2'd2; n_t0 = 2'd0; n_t1 = if_ty; end
            3'b011: begin n_ln = 2'd2; n_t0 = 2'd1; n_t1 = if_ty; end
            3'b111: begin n_ln = 2'd3; n_t0 = 2'd0; n_t1 = 2'd1; n_t2 = if_ty; end
            default: n_ln = 2'd0;
        endcase
        if (!hdmi_en) n_ln = 2'd0;
    end

    always @(posedge clk) begin
        if (rst) begin
            acr_cnt <= 20'd0;
            acr_due <= 1'b0;
            tone_ph <= 6'd0;
            ren_d   <= 1'b0;
            fidx    <= 8'd0;
            smp_tog <= 1'b0;
            uf_tog  <= 1'b0;
            pcnt    <= 3'd0;
            kcnt    <= 3'd0;
            ln      <= 2'd0;
            lhdmi   <= 1'b0;
            ty0 <= 2'd0; ty1 <= 2'd0; ty2 <= 2'd0;
        end else begin
            // ACR due once per CTS pixel clocks
            if (acr_cnt == ACR_CTS - 20'd1) acr_cnt <= 20'd0;
            else                            acr_cnt <= acr_cnt + 20'd1;
            if (acr_cnt == 20'd0)            acr_due <= 1'b1;
            else if (latch & s_acr & hdmi_en) acr_due <= 1'b0;

            ren_d <= fifo_ren;
            if (cap_tone)
                tone_ph <= (tone_ph == 6'd47) ? 6'd0 : tone_ph + 6'd1;
            if (cap) begin
                fidx    <= (fidx == 8'd191) ? 8'd0 : fidx + 8'd1;
                smp_tog <= ~smp_tog;
            end
            if (cap_zero) uf_tog <= ~uf_tog;

            // line latch: schedule packets, hand pending samples to the packet
            if (latch) begin
                ln    <= n_ln;
                ty0   <= n_t0;
                ty1   <= n_t1;
                ty2   <= n_t2;
                lhdmi <= hdmi_en;
                kl0 <= pl0; kl1 <= pl1; kl2 <= pl2; kl3 <= pl3;
                kr0 <= pr0; kr1 <= pr1; kr2 <= pr2; kr3 <= pr3;
                kcb <= pcb; kbb <= pbb;
                kcnt <= pcnt;
            end

            // pending: cleared by the latch, appended by captures (max 4)
            if (latch | (cap & (pcnt != 3'd4))) begin
                case (latch ? 3'd0 : pcnt)
                    3'd0: begin pl0 <= cap_l; pr0 <= cap_r; cb0 <= cap_c; bb0 <= cap_b; end
                    3'd1: begin pl1 <= cap_l; pr1 <= cap_r; cb1 <= cap_c; bb1 <= cap_b; end
                    3'd2: begin pl2 <= cap_l; pr2 <= cap_r; cb2 <= cap_c; bb2 <= cap_b; end
                    default: begin pl3 <= cap_l; pr3 <= cap_r; cb3 <= cap_c; bb3 <= cap_b; end
                endcase
                if (latch) pcnt <= cap ? 3'd1 : 3'd0;
                else       pcnt <= pcnt + 3'd1;
            end
        end
    end

    // ---------------------------------------------------------------- island geometry
    wire [11:0] iend  = IDAT_H + {5'd0, ln, 5'd0};          // first trailing guard char
    wire        isl   = (ln != 2'd0);
    wire        ipre0 = isl & (h_cnt >= IPRE_H) & (h_cnt < IGB_H);
    wire        igb0  = isl & (((h_cnt >= IGB_H) & (h_cnt < IDAT_H)) |
                               ((h_cnt >= iend) & (h_cnt < iend + 12'd2)));
    wire        idat0 = isl & (h_cnt >= IDAT_H) & (h_cnt < iend);
    wire [11:0] hoff  = h_cnt - IDAT_H;
    wire        load0 = idat0 & (hoff[4:0] == 5'd0);
    wire        nla   = (v_cnt < V_ACTM1) | (v_cnt == V_LAST);   // next line is active
    wire        vpre0 = lhdmi & nla & (h_cnt >= VPRE_H) & (h_cnt < VGB_H);
    wire        vgb0  = lhdmi & nla & (h_cnt >= VGB_H);

    always @(posedge clk) begin
        if (rst) begin
            vpre1 <= 1'b0; vgb1 <= 1'b0; ipre1 <= 1'b0;
            igb1 <= 1'b0; idat1 <= 1'b0; ifirst1 <= 1'b0;
        end else begin
            vpre1   <= vpre0;
            vgb1    <= vgb0;
            ipre1   <= ipre0;
            igb1    <= igb0;
            idat1   <= idat0;
            ifirst1 <= isl & (h_cnt == IDAT_H);
        end
    end

    // ---------------------------------------------------------------- packet contents
    wire [1:0]  lty = (slot == 2'd0) ? ty0 : (slot == 2'd1) ? ty1 : ty2;

    // audio sample packet (layout 0, 2 channels, 16-bit samples left-aligned in 24)
    wire [3:0]  kpres = (kcnt == 3'd0) ? 4'b0000 : (kcnt == 3'd1) ? 4'b0001 :
                        (kcnt == 3'd2) ? 4'b0011 : (kcnt == 3'd3) ? 4'b0111 : 4'b1111;
    wire [23:0] h_aud = {kbb & kpres, 4'b0000, 3'b000, 1'b0, kpres, 8'h02};
    // byte 6 = {P_R, P_L, C_R, C_L, U_R, U_L, V_R, V_L}; P = even parity over 24 data bits + V U C
    // HDMI 1.4 table 5-13, SB6[7:0] = PR,CR,UR,VR,PL,CL,UL,VL.
    // Each channel has its own 4-bit nibble; grouping both parity bits
    // together corrupts channel status / validity at the receiver.
    wire [55:0] s_aud0 = {^{kr0, kcb[0]}, kcb[0], 2'b00, ^{kl0, kcb[0]}, kcb[0], 2'b00,
                          kr0, 8'h00, kl0, 8'h00};
    wire [55:0] s_aud1 = {^{kr1, kcb[1]}, kcb[1], 2'b00, ^{kl1, kcb[1]}, kcb[1], 2'b00,
                          kr1, 8'h00, kl1, 8'h00};
    wire [55:0] s_aud2 = {^{kr2, kcb[2]}, kcb[2], 2'b00, ^{kl2, kcb[2]}, kcb[2], 2'b00,
                          kr2, 8'h00, kl2, 8'h00};
    wire [55:0] s_aud3 = {^{kr3, kcb[3]}, kcb[3], 2'b00, ^{kl3, kcb[3]}, kcb[3], 2'b00,
                          kr3, 8'h00, kl3, 8'h00};

    // ACR: SB0 0, SB1-3 CTS[19:0], SB4-6 N[19:0] (same in all four subpackets)
    wire [55:0] sp_acr = {ACR_N[7:0], ACR_N[15:8], 4'h0, ACR_N[19:16],
                         ACR_CTS[7:0], ACR_CTS[15:8], 4'h0, ACR_CTS[19:16], 8'h00};
    // AVI InfoFrame v2, len 13: RGB, aspect / VIC from the parameters
    //   640x480p:  PB0 checksum 0x56, PB1 0x00, PB2 0x18 (4:3),  PB3 0x00, PB4 0x01
    //   1280x720p: PB0 checksum 0x43, PB1 0x00, PB2 0x28 (16:9), PB3 0x00, PB4 0x04
    // Audio InfoFrame v1, len 10: 2 channels, coding/rate from stream
    //   PB0 checksum 0x70, PB1 0x01
    reg  [23:0] l_hdr;
    reg  [55:0] l_s0, l_s1, l_s2, l_s3;
    always @(*) begin
        case (lty)
            2'd0: begin
                l_hdr = h_aud;
                l_s0  = kpres[0] ? s_aud0 : 56'd0;
                l_s1  = kpres[1] ? s_aud1 : 56'd0;
                l_s2  = kpres[2] ? s_aud2 : 56'd0;
                l_s3  = kpres[3] ? s_aud3 : 56'd0;
            end
            2'd1: begin
                l_hdr = 24'h000001;
                l_s0 = sp_acr; l_s1 = sp_acr; l_s2 = sp_acr; l_s3 = sp_acr;
            end
            2'd2: begin
                l_hdr = 24'h0D0282;
                l_s0 = {16'h0000, avi_vic, 8'h00, AVI_PB2, 8'h00, AVI_CSUM};
                l_s1 = 56'd0; l_s2 = 56'd0; l_s3 = 56'd0;
            end
            default: begin
                l_hdr = 24'h0A0184;
                l_s0 = 56'h00_00_00_00_00_01_70; l_s1 = 56'd0; l_s2 = 56'd0; l_s3 = 56'd0;
            end
        endcase
    end

    // ---------------------------------------------------------------- packet bit generator
    // header: 24 bits LSB first + 8 ECC bits; subpackets: 56 bits, 2 per clock, + 8 ECC.
    // BCH generator 1 + x^6 + x^7 + x^8 as LSB-first LFSR: e' = (e >> 1) ^ ((e[0]^b) ? 0x83 : 0)
    reg  [4:0]  pc;
    reg  [23:0] hsr;
    reg  [7:0]  eh;
    reg  [55:0] q0, q1, q2, q3;
    reg  [7:0]  e0, e1, e2, e3;

    wire        hd   = (pc < 5'd24);
    wire        sd   = (pc < 5'd28);
    assign hbit1 = hd ? hsr[0] : eh[0];
    assign sbe1  = sd ? {q3[0], q2[0], q1[0], q0[0]} : {e3[0], e2[0], e1[0], e0[0]};
    assign sbo1  = sd ? {q3[1], q2[1], q1[1], q0[1]} : {e3[1], e2[1], e1[1], e0[1]};

    wire [7:0]  ehn  = {1'b0, eh[7:1]} ^ ((eh[0] ^ hsr[0]) ? 8'h83 : 8'h00);
    wire [7:0]  e0a  = {1'b0, e0[7:1]} ^ ((e0[0] ^ q0[0]) ? 8'h83 : 8'h00);
    wire [7:0]  e0b  = {1'b0, e0a[7:1]} ^ ((e0a[0] ^ q0[1]) ? 8'h83 : 8'h00);
    wire [7:0]  e1a  = {1'b0, e1[7:1]} ^ ((e1[0] ^ q1[0]) ? 8'h83 : 8'h00);
    wire [7:0]  e1b  = {1'b0, e1a[7:1]} ^ ((e1a[0] ^ q1[1]) ? 8'h83 : 8'h00);
    wire [7:0]  e2a  = {1'b0, e2[7:1]} ^ ((e2[0] ^ q2[0]) ? 8'h83 : 8'h00);
    wire [7:0]  e2b  = {1'b0, e2a[7:1]} ^ ((e2a[0] ^ q2[1]) ? 8'h83 : 8'h00);
    wire [7:0]  e3a  = {1'b0, e3[7:1]} ^ ((e3[0] ^ q3[0]) ? 8'h83 : 8'h00);
    wire [7:0]  e3b  = {1'b0, e3a[7:1]} ^ ((e3a[0] ^ q3[1]) ? 8'h83 : 8'h00);

    always @(posedge clk) begin
        if (rst) begin
            slot <= 2'd0;
            pc   <= 5'd0;
        end else if (latch) begin
            slot <= 2'd0;
        end else if (load0) begin
            slot <= slot + 2'd1;
            pc   <= 5'd0;
            hsr  <= l_hdr;
            eh   <= 8'd0;
            q0 <= l_s0; q1 <= l_s1; q2 <= l_s2; q3 <= l_s3;
            e0 <= 8'd0; e1 <= 8'd0; e2 <= 8'd0; e3 <= 8'd0;
        end else if (idat0) begin
            pc <= pc + 5'd1;
            if (hd) begin
                hsr <= {1'b0, hsr[23:1]};
                eh  <= ehn;
            end else begin
                eh  <= {1'b0, eh[7:1]};
            end
            if (sd) begin
                q0 <= {2'b00, q0[55:2]}; e0 <= e0b;
                q1 <= {2'b00, q1[55:2]}; e1 <= e1b;
                q2 <= {2'b00, q2[55:2]}; e2 <= e2b;
                q3 <= {2'b00, q3[55:2]}; e3 <= e3b;
            end else begin
                e0 <= {2'b00, e0[7:2]};
                e1 <= {2'b00, e1[7:2]};
                e2 <= {2'b00, e2[7:2]};
                e3 <= {2'b00, e3[7:2]};
            end
        end
    end
endmodule


// =============================================================================
// One TMDS channel. mode: 0 control (c), 1 video (d), 2 TERC4 (t), 3 fixed (f).
// Latency 3 clocks for every mode (video path identical to encode.v).
// Symbols are written q_out[9:0] as in encode.v (bit 0 is sent first).
// =============================================================================
module hdmi_tmds_ch (
    input  wire       clk,
    input  wire       rst,
    input  wire [1:0] mode,
    input  wire [7:0] d,
    input  wire [1:0] c,
    input  wire [3:0] t,
    input  wire [9:0] f,
    output reg  [9:0] dout
);
    // ---- non-video symbol (computed in stage A, delayed to stage C)
    reg [9:0] ctl_sym, terc_sym;
    always @(*) begin
        case (c)
            2'b00:   ctl_sym = 10'b1101010100;
            2'b01:   ctl_sym = 10'b0010101011;
            2'b10:   ctl_sym = 10'b0101010100;
            default: ctl_sym = 10'b1010101011;
        endcase
        case (t)
            4'h0: terc_sym = 10'b1010011100;
            4'h1: terc_sym = 10'b1001100011;
            4'h2: terc_sym = 10'b1011100100;
            4'h3: terc_sym = 10'b1011100010;
            4'h4: terc_sym = 10'b0101110001;
            4'h5: terc_sym = 10'b0100011110;
            4'h6: terc_sym = 10'b0110001110;
            4'h7: terc_sym = 10'b0100111100;
            4'h8: terc_sym = 10'b1011001100;
            4'h9: terc_sym = 10'b0100111001;
            4'hA: terc_sym = 10'b0110011100;
            4'hB: terc_sym = 10'b1011000110;
            4'hC: terc_sym = 10'b1010001110;
            4'hD: terc_sym = 10'b1001110001;
            4'hE: terc_sym = 10'b0101100011;
            default: terc_sym = 10'b1011000011;
        endcase
    end
    wire [9:0] sym0 = (mode == 2'd0) ? ctl_sym : (mode == 2'd2) ? terc_sym : f;

    // ---- stage A
    reg  [3:0] n1d;
    reg  [7:0] din_q;
    reg        vid_a, vid_b;
    reg  [9:0] sym_a, sym_b;
    always @(posedge clk) begin
        n1d   <= d[0] + d[1] + d[2] + d[3] + d[4] + d[5] + d[6] + d[7];
        din_q <= d;
        vid_a <= (mode == 2'd1);
        sym_a <= sym0;
        vid_b <= vid_a;
        sym_b <= sym_a;
    end

    // ---- 8 -> 9 bit (combinational on stage A)
    wire decision1 = (n1d > 4'h4) | ((n1d == 4'h4) & (din_q[0] == 1'b0));
    wire qm0 = din_q[0];
    wire qm1 = decision1 ? ~(qm0 ^ din_q[1]) : (qm0 ^ din_q[1]);
    wire qm2 = decision1 ? ~(qm1 ^ din_q[2]) : (qm1 ^ din_q[2]);
    wire qm3 = decision1 ? ~(qm2 ^ din_q[3]) : (qm2 ^ din_q[3]);
    wire qm4 = decision1 ? ~(qm3 ^ din_q[4]) : (qm3 ^ din_q[4]);
    wire qm5 = decision1 ? ~(qm4 ^ din_q[5]) : (qm4 ^ din_q[5]);
    wire qm6 = decision1 ? ~(qm5 ^ din_q[6]) : (qm5 ^ din_q[6]);
    wire qm7 = decision1 ? ~(qm6 ^ din_q[7]) : (qm6 ^ din_q[7]);
    wire [8:0] q_m = {~decision1, qm7, qm6, qm5, qm4, qm3, qm2, qm1, qm0};

    // ---- stage B
    reg [3:0] n1q_m, n0q_m;
    reg [8:0] q_m_reg;
    always @(posedge clk) begin
        n1q_m   <= q_m[0] + q_m[1] + q_m[2] + q_m[3] + q_m[4] + q_m[5] + q_m[6] + q_m[7];
        n0q_m   <= 4'h8 - (q_m[0] + q_m[1] + q_m[2] + q_m[3] + q_m[4] + q_m[5] + q_m[6] + q_m[7]);
        q_m_reg <= q_m;
    end

    // ---- stage C: 9 -> 10 bit with running disparity (reset outside video)
    reg  [4:0] cnt;
    wire decision2 = (cnt == 5'h0) | (n1q_m == n0q_m);
    wire decision3 = (~cnt[4] & (n1q_m > n0q_m)) | (cnt[4] & (n0q_m > n1q_m));

    always @(posedge clk) begin
        if (rst) begin
            dout <= 10'h0;
            cnt  <= 5'h0;
        end else if (vid_b) begin
            if (decision2) begin
                dout <= {~q_m_reg[8], q_m_reg[8], q_m_reg[8] ? q_m_reg[7:0] : ~q_m_reg[7:0]};
                cnt  <= (~q_m_reg[8]) ? (cnt + n0q_m - n1q_m) : (cnt + n1q_m - n0q_m);
            end else if (decision3) begin
                dout <= {1'b1, q_m_reg[8], ~q_m_reg[7:0]};
                cnt  <= cnt + {q_m_reg[8], 1'b0} + (n0q_m - n1q_m);
            end else begin
                dout <= {1'b0, q_m_reg[8], q_m_reg[7:0]};
                cnt  <= cnt - {~q_m_reg[8], 1'b0} + (n1q_m - n0q_m);
            end
        end else begin
            dout <= sym_b;
            cnt  <= 5'h0;
        end
    end
endmodule
