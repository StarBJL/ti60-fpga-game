// =============================================================================
// audio_mixer.v  -  16-voice PCM sample mixer for HDMI audio (core_clk domain)
//
//   * APB3 slave (CPU 0xF810_2000, 4 KB window)
//   * AXI4 read master, 32-bit, 8-beat (32 B) bursts, shares the HDMI scan-out
//     read port through aud_rd_mux (in-order, HDMI first)
//   * output: one stereo 16-bit sample per push into the HDMI audio FIFO;
//     a mixing pass runs whenever the FIFO holds fewer than TARGET samples
//     (the HDMI side pulls exactly 48 kHz, so the mixer is demand driven)
//
// Per voice (serial, ~30 clocks per active voice; 4166 clocks per sample @200 MHz):
//   sample data in DDR: s8 mono, s16 mono or s16 stereo (L,R interleaved),
//   position 24.12 fixed point, step 4.12 (pitch, max 15.99x),
//   linear interpolation between s[i] and s[i+1] (loop aware),
//   volume left / right 0..256 (256 = 1.0), loop start / end, one-shot or loop.
// Sample cache: 2 lines x 32 B per voice (way = address bit 5), prefetch of the
// line ahead after every sample; a miss stalls the mixer until the line is in.
//
// Mix:  acc = sum(interp * vol)            (32 bit)
//       out = sat16(((acc >>> 8) * MVOL) >>> 8)
//
// Register map (byte offsets):
//   0x000 ID       0x4D495831 "MIX1" (RO)
//   0x004 CTRL     [0] enable (reset 0)
//   0x008 MVOL     [8:0] master volume 0..256 (reset 256)
//   0x00C TARGET   [8:0] FIFO fill target in samples (reset 64 = 1.3 ms)
//   0x010 KEYON    W: bit v = (re)start voice v at its START position
//   0x014 KEYOFF   W: bit v = stop voice v
//   0x018 ACTIVE   R: bit v = voice v playing (cleared at the end of a one-shot)
//   0x01C SAMPLES  R: samples mixed
//   0x020 MISSES   R: lookups that had to wait for DDR (should stay small)
//   0x024 FETCHES  R: 32-byte lines read
//   0x028 CLIPS    R: output samples that saturated
//   0x02C CAPS     R: [7:0] voices 16, [15:8] line bytes 32, [23:16] version 1
//   0x200 + v*0x20 voice v (write only; reads return 0):
//     +0x00 ADDR   byte address of sample 0 (s16: 2-byte, stereo: 4-byte aligned)
//     +0x04 LEN    [23:0] number of sample frames
//     +0x08 LOOP   [23:0] loop start frame, [31] loop enable
//     +0x0C PITCH  [15:0] step per output sample, 4.12 (0x1000 = source rate 48 kHz)
//                  [17:16] format 0 s8 mono, 1 s16 mono, 2 s16 stereo
//     +0x10 VOL    [8:0] left, [24:16] right (0..256)
//     +0x14 START  [23:0] frame where KEYON starts
//   Parameters may be changed while a voice plays (taken at its next sample).
//
// Kept inside the vsim.py subset (no functions / loops / signed / bit lvalues).
// =============================================================================
`timescale 1ns / 1ps

module audio_mixer #(
    parameter LVL_W = 9                    // width of the FIFO level input
)(
    input  wire        clk,
    input  wire        rst,                // APB / system reset
    input  wire        axi_rst,

    input  wire [11:0] apb_paddr,
    input  wire        apb_psel,
    input  wire        apb_penable,
    input  wire        apb_pwrite,
    input  wire [31:0] apb_pwdata,
    output reg  [31:0] apb_prdata,
    output wire        apb_pready,
    output wire        apb_pslverror,

    output wire        axi_arvalid,
    input  wire        axi_arready,
    output wire [31:0] axi_araddr,
    output wire [7:0]  axi_arlen,
    output wire [2:0]  axi_arsize,
    output wire [1:0]  axi_arburst,
    input  wire        axi_rvalid,
    output wire        axi_rready,
    input  wire [31:0] axi_rdata,
    input  wire        axi_rlast,

    output reg         o_valid,
    input  wire        o_ready,
    output reg  [31:0] o_data,             // [15:0] left, [31:16] right
    input  wire [LVL_W-1:0] o_level
);
    // ------------------------------------------------------------------ states
    localparam [5:0] S_IDLE = 6'd0,  S_V0 = 6'd1,  S_RD = 6'd2,  S_C0 = 6'd3,
                     S_C1 = 6'd4,  S_C2 = 6'd5,  S_C3 = 6'd6,  S_LK = 6'd7,
                     S_CK = 6'd8,  S_FW = 6'd9,  S_RK = 6'd10, S_XK = 6'd11,
                     S_M0 = 6'd12, S_M1 = 6'd13, S_M2 = 6'd14, S_M3 = 6'd15,
                     S_M4 = 6'd16, S_M5 = 6'd17, S_M6 = 6'd18, S_A1 = 6'd19,
                     S_A2 = 6'd20, S_A3 = 6'd21, S_A4 = 6'd22, S_P1 = 6'd23,
                     S_P2 = 6'd24, S_P3 = 6'd25, S_P4 = 6'd26, S_NX = 6'd27,
                     S_O1 = 6'd28, S_O2 = 6'd29, S_O3 = 6'd30, S_O4 = 6'd31,
                     S_CQ = 6'd32, S_P5 = 6'd33;

    reg  [5:0]  st;
    reg  [3:0]  v;
    reg  [2:0]  rk;

    // ------------------------------------------------------------------ registers
    reg         en;
    reg  [8:0]  mvol;
    reg  [8:0]  target;
    reg  [15:0] active, kon;
    reg  [31:0] cnt_smp, cnt_miss, cnt_fetch, cnt_clip;

    // ------------------------------------------------------------------ voice RAM (BRAM)
    // word 0..5 = ADDR LEN LOOP PITCH VOL START (APB), word 6 = position 24.12 (mixer)
    reg  [35:0] vram [0:127];
    reg  [35:0] vq;
    wire [6:0]  vra = {v, rk};

    wire        apb_wr  = apb_psel & apb_penable & apb_pwrite;
    wire        apb_vw  = apb_wr & apb_paddr[9] & (apb_paddr[4:2] < 3'd6);
    wire        mix_vw  = (st == S_A4);
    assign apb_pready    = ~(apb_vw & mix_vw);
    assign apb_pslverror = 1'b0;

    reg  [35:0] fpos;                      // new position (written in S_A4)
    wire        vwe = mix_vw | apb_vw;
    wire [6:0]  vwa = mix_vw ? {v, 3'd6} : {apb_paddr[8:5], apb_paddr[4:2]};
    wire [35:0] vwd = mix_vw ? fpos : {4'd0, apb_pwdata};

    always @(posedge clk) begin
        if (vwe) vram[vwa] <= vwd;
        vq <= vram[vra];
    end

    // ------------------------------------------------------------------ sample cache
    // data: 16 voices x 2 ways x 8 words (BRAM); tags: abs[27:6] per {voice, way}
    reg  [31:0] cram [0:255];
    reg  [31:0] cq;
    reg  [7:0]  cra;
    (* syn_ramstyle = "registers" *) reg [21:0] tagm [0:31];
    reg  [31:0] tvalid;

    // fetcher
    reg         f_busy, f_arv;
    reg  [3:0]  f_v;
    reg         f_way;
    reg  [22:0] f_line;                    // abs[27:5]
    reg  [2:0]  f_beat;
    wire        f_go;                      // start a fetch this clock
    wire [3:0]  f_go_v;
    wire        f_go_way;
    wire [22:0] f_go_line;

    assign axi_arvalid = f_arv;
    assign axi_araddr  = {4'd0, f_line, 5'd0};
    assign axi_arlen   = 8'd7;
    assign axi_arsize  = 3'b010;
    assign axi_arburst = 2'b01;
    assign axi_rready  = 1'b1;

    wire        r_hs  = axi_rvalid & f_busy;
    wire        f_done = r_hs & axi_rlast;
    wire [7:0]  cwa  = {f_v, f_way, f_beat};

    always @(posedge clk) begin
        if (r_hs) cram[cwa] <= axi_rdata;
        cq <= cram[cra];
    end

    always @(posedge clk) begin
        if (f_done) tagm[{f_v, f_way}] <= f_line[22:1];
    end

    always @(posedge clk) begin
        if (rst | axi_rst) begin
            f_busy <= 1'b0;
            f_arv  <= 1'b0;
            f_beat <= 3'd0;
            f_v    <= 4'd0;
            f_way  <= 1'b0;
            f_line <= 23'd0;
        end else begin
            if (f_go) begin
                f_busy <= 1'b1;
                f_arv  <= 1'b1;
                f_beat <= 3'd0;
                f_v    <= f_go_v;
                f_way  <= f_go_way;
                f_line <= f_go_line;
            end else begin
                if (f_arv & axi_arready) f_arv <= 1'b0;
                if (r_hs) f_beat <= f_beat + 3'd1;
                if (f_done) f_busy <= 1'b0;
            end
        end
    end

    // ------------------------------------------------------------------ voice parameters (latched)
    reg  [31:0] p_base;
    reg  [23:0] p_len, p_lst, p_llen, p_start;
    reg         p_lpen;
    reg  [15:0] p_step;
    reg  [1:0]  p_fmt;
    reg  [8:0]  p_vl, p_vr;
    reg  [35:0] p_pos;
    wire [1:0]  sh  = (p_fmt == 2'd0) ? 2'd0 : (p_fmt == 2'd1) ? 2'd1 : 2'd2;
    wire [5:0]  spl = (p_fmt == 2'd0) ? 6'd32 : (p_fmt == 2'd1) ? 6'd16 : 6'd8;

    reg  [23:0] r_i0, r_i1, rj;
    reg         r_has1, sk, r_end, pj_ok;
    reg  [31:0] abs0, abs1, absp, absn;
    reg  [21:0] tq;
    reg         tv;
    reg         hitq;                      // registered tag compare (S_CQ)
    reg         preq;                      // registered prefetch decision (S_P4)
    reg  [15:0] s0l, s0r, s1l, s1r;
    reg  [16:0] dl, dr;
    reg  [28:0] pl, pr;
    reg  [16:0] il, ir;
    reg  [24:0] ql, qr;
    reg  [31:0] accl, accr;
    reg  [35:0] np;
    reg         nge;
    reg  [23:0] nwr;
    reg  [31:0] ol, or_;

    wire [31:0] absk  = sk ? abs1 : abs0;
    wire [4:0]  tidx  = {v, absk[5]};
    wire        hit   = tv & (tq == absk[27:6]);      // used in S_CQ only
    wire        von   = active[v] | kon[v];
    wire [15:0] vbit  = 16'd1 << v;

    // sample extraction from the cache word
    wire [7:0]  b8  = (absk[1:0] == 2'd0) ? cq[7:0] : (absk[1:0] == 2'd1) ? cq[15:8] :
                      (absk[1:0] == 2'd2) ? cq[23:16] : cq[31:24];
    wire [15:0] h16 = absk[1] ? cq[31:16] : cq[15:0];
    wire [15:0] xl  = (p_fmt == 2'd0) ? {b8, 8'h00} : (p_fmt == 2'd1) ? h16 : cq[15:0];
    wire [15:0] xr  = (p_fmt == 2'd2) ? cq[31:16] : xl;

    // fetch requests: demand miss in S_CK, prefetch in S_P4
    wire        dem_go = (st == S_CK) & ~hitq & ~f_busy;
    wire        pre_ok = pj_ok & ~(tv & (tq == absp[27:6])) &
                         ~((absp[5] == absn[5]) & (absp[27:6] != absn[27:6]));
    wire        pre_go = (st == S_P5) & preq & ~f_busy;
    assign f_go      = dem_go | pre_go;
    assign f_go_v    = v;
    assign f_go_way  = dem_go ? absk[5] : absp[5];
    assign f_go_line = dem_go ? absk[27:5] : absp[27:5];

    // output: (acc >>> 8) * mvol, then >>> 8 and saturate
    wire [23:0] a8l = accl[31:8];
    wire [23:0] a8r = accr[31:8];
    wire [31:0] xol = ol;
    wire [31:0] xor_ = or_;
    wire [23:0] fl  = xol[31:8];
    wire [23:0] fr  = xor_[31:8];
    wire        hil = ~fl[23] & (fl[22:15] != 8'h00);
    wire        lol =  fl[23] & (fl[22:15] != 8'hFF);
    wire        hir = ~fr[23] & (fr[22:15] != 8'h00);
    wire        lor =  fr[23] & (fr[22:15] != 8'hFF);
    wire [15:0] satl = hil ? 16'h7FFF : lol ? 16'h8000 : fl[15:0];
    wire [15:0] satr = hir ? 16'h7FFF : lor ? 16'h8000 : fr[15:0];

    wire        lvl_ok = ({{(16-LVL_W){1'b0}}, o_level} < {7'd0, target});

    // keyon / keyoff / end-of-sample masks
    wire        kw   = apb_wr & ~apb_paddr[9] & (apb_paddr[8:2] == 7'h04);
    wire        kfw  = apb_wr & ~apb_paddr[9] & (apb_paddr[8:2] == 7'h05);
    wire [15:0] kset = kw  ? apb_pwdata[15:0] : 16'd0;
    wire [15:0] koff = kfw ? apb_pwdata[15:0] : 16'd0;
    wire        take = (st == S_C0) & kon[v];                         // key-on consumed
    wire        stop = ((st == S_C2) & (r_i0 >= p_len)) |
                       ((st == S_A3) & nge & ~p_lpen);
    wire [31:0] tv_clr = (take ? (32'd3 << {v, 1'b0}) : 32'd0) |
                         (f_go ? (32'd1 << {f_go_v, f_go_way}) : 32'd0);
    wire [31:0] tv_set = f_done ? (32'd1 << {f_v, f_way}) : 32'd0;

    always @(posedge clk) begin
        if (rst) begin
            active <= 16'd0;
            kon    <= 16'd0;
            tvalid <= 32'd0;
        end else begin
            active <= ((active | (take ? vbit : 16'd0)) & ~(stop ? vbit : 16'd0)) & ~koff;
            kon    <= ((kon & ~(take ? vbit : 16'd0)) | kset) & ~koff;
            tvalid <= (tvalid & ~tv_clr) | tv_set;
        end
    end

    // ------------------------------------------------------------------ main FSM
    always @(posedge clk) begin
        if (rst) begin
            st <= S_IDLE;
            v  <= 4'd0;
            rk <= 3'd0;
            o_valid <= 1'b0;
            o_data  <= 32'd0;
            cnt_smp <= 32'd0; cnt_miss <= 32'd0; cnt_fetch <= 32'd0; cnt_clip <= 32'd0;
            accl <= 32'd0; accr <= 32'd0;
            sk <= 1'b0;
        end else begin
            if (f_done) cnt_fetch <= cnt_fetch + 32'd1;
            case (st)
            S_IDLE: begin
                v <= 4'd0;
                accl <= 32'd0;
                accr <= 32'd0;
                if (en & lvl_ok) st <= S_V0;
            end
            S_V0: begin
                rk <= 3'd0;
                st <= von ? S_RD : S_NX;
            end
            S_RD: begin                    // vq = word rk-1
                rk <= rk + 3'd1;
                case (rk)
                    3'd1: p_base <= vq[31:0];
                    3'd2: p_len  <= vq[23:0];
                    3'd3: begin p_lst <= vq[23:0]; p_lpen <= vq[31]; end
                    3'd4: begin p_step <= vq[15:0]; p_fmt <= vq[17:16]; end
                    3'd5: begin p_vl <= vq[8:0]; p_vr <= vq[24:16]; end
                    3'd6: p_start <= vq[23:0];
                    3'd7: p_pos <= vq;
                    default: ;
                endcase
                if (rk == 3'd7) st <= S_C0;
            end
            S_C0: begin
                if (kon[v]) p_pos <= {p_start, 12'd0};
                p_llen <= p_len - p_lst;
                if (p_fmt == 2'd3) p_fmt <= 2'd1;
                st <= S_C1;
            end
            S_C1: begin
                r_i0 <= p_pos[35:12];
                r_i1 <= p_pos[35:12] + 24'd1;
                st <= S_C2;
            end
            S_C2: begin
                if (r_i0 >= p_len) begin
                    st <= S_NX;                    // past the end: voice stops
                end else begin
                    r_has1 <= (r_i1 < p_len) | p_lpen;
                    if (r_i1 >= p_len) r_i1 <= p_lst;
                    abs0 <= p_base + ({8'd0, r_i0} << sh);
                    st <= S_C3;
                end
            end
            S_C3: begin
                abs1 <= p_base + ({8'd0, r_i1} << sh);
                sk <= 1'b0;
                st <= S_LK;
            end
            S_LK: begin
                tq <= tagm[tidx];
                tv <= tvalid[tidx];
                st <= S_CQ;
            end
            S_CQ: begin
                hitq <= hit;
                st <= S_CK;
            end
            S_CK: begin
                if (hitq) begin
                    cra <= {v, absk[5], absk[4:2]};
                    st <= S_RK;
                end else begin
                    if (dem_go) cnt_miss <= cnt_miss + 32'd1;
                    st <= S_FW;
                end
            end
            S_FW: begin
                if (~f_busy) st <= S_LK;
            end
            S_RK: st <= S_XK;
            S_XK: begin
                if (~sk) begin
                    s0l <= xl; s0r <= xr;
                    s1l <= xl; s1r <= xr;
                end else begin
                    s1l <= xl; s1r <= xr;
                end
                if (~sk & r_has1) begin
                    sk <= 1'b1;
                    st <= S_LK;
                end else begin
                    st <= S_M0;
                end
            end
            // ---- interpolation: s0 + ((s1 - s0) * frac) >>> 12
            S_M0: begin
                dl <= {s1l[15], s1l} - {s0l[15], s0l};
                dr <= {s1r[15], s1r} - {s0r[15], s0r};
                st <= S_M1;
            end
            S_M1: begin
                pl <= {12'd0, dl} * {17'd0, p_pos[11:0]};
                pr <= {12'd0, dr} * {17'd0, p_pos[11:0]};
                st <= S_M2;
            end
            S_M2: begin                    // signed correction: d negative -> - frac << 17
                pl <= pl - (dl[16] ? {p_pos[11:0], 17'd0} : 29'd0);
                pr <= pr - (dr[16] ? {p_pos[11:0], 17'd0} : 29'd0);
                st <= S_M3;
            end
            S_M3: begin
                il <= {s0l[15], s0l} + pl[28:12];
                ir <= {s0r[15], s0r} + pr[28:12];
                st <= S_M4;
            end
            // ---- volume: interp (16-bit signed) * vol (0..256)
            S_M4: begin
                ql <= {9'd0, il[15:0]} * {16'd0, p_vl};
                qr <= {9'd0, ir[15:0]} * {16'd0, p_vr};
                st <= S_M5;
            end
            S_M5: begin
                ql <= ql - (il[15] ? {p_vl, 16'd0} : 25'd0);
                qr <= qr - (ir[15] ? {p_vr, 16'd0} : 25'd0);
                st <= S_M6;
            end
            S_M6: begin
                accl <= accl + {{7{ql[24]}}, ql};
                accr <= accr + {{7{qr[24]}}, qr};
                st <= S_A1;
            end
            // ---- advance position, loop / end
            S_A1: begin
                np <= p_pos + {20'd0, p_step};
                st <= S_A2;
            end
            S_A2: begin
                nge <= (np[35:12] >= p_len);
                nwr <= np[35:12] - p_llen;
                st <= S_A3;
            end
            S_A3: begin
                if (nge & p_lpen)
                    fpos <= (nwr >= p_len) ? {p_lst, np[11:0]} : {nwr, np[11:0]};
                else
                    fpos <= np;
                r_end <= nge & ~p_lpen;
                st <= S_A4;
            end
            S_A4: begin                    // vram write (APB voice writes wait)
                rj <= fpos[35:12] + {18'd0, spl};
                absn <= p_base + ({8'd0, fpos[35:12]} << sh);
                st <= S_P1;
            end
            // ---- prefetch the line one line ahead (loop aware)
            S_P1: begin
                pj_ok <= ~r_end & (~(rj >= p_len) | p_lpen);
                if (rj >= p_len) rj <= rj - p_llen;
                st <= S_P2;
            end
            S_P2: begin
                if (rj >= p_len) pj_ok <= 1'b0;
                absp <= p_base + ({8'd0, rj} << sh);
                st <= S_P3;
            end
            S_P3: begin
                tq <= tagm[{v, absp[5]}];
                tv <= tvalid[{v, absp[5]}];
                st <= S_P4;
            end
            S_P4: begin
                preq <= pre_ok;
                st <= S_P5;
            end
            S_P5: st <= S_NX;              // prefetch issued by pre_go if possible
            S_NX: begin
                v  <= v + 4'd1;
                st <= (v == 4'd15) ? S_O1 : S_V0;
            end
            // ---- output
            S_O1: begin
                ol  <= {8'd0, a8l} * {23'd0, mvol};
                or_ <= {8'd0, a8r} * {23'd0, mvol};
                st <= S_O2;
            end
            S_O2: begin
                ol  <= ol  - (a8l[23] ? {mvol[7:0], 24'd0} : 32'd0);
                or_ <= or_ - (a8r[23] ? {mvol[7:0], 24'd0} : 32'd0);
                st <= S_O3;
            end
            S_O3: begin
                o_valid <= 1'b1;
                o_data  <= {satr, satl};
                if (hil | lol | hir | lor) cnt_clip <= cnt_clip + 32'd1;
                st <= S_O4;
            end
            S_O4: begin
                if (o_ready) begin
                    o_valid <= 1'b0;
                    cnt_smp <= cnt_smp + 32'd1;
                    st <= S_IDLE;
                end
            end
            default: st <= S_IDLE;
            endcase
        end
    end

    // ------------------------------------------------------------------ APB registers
    always @(posedge clk) begin
        if (rst) begin
            en     <= 1'b0;
            mvol   <= 9'd256;
            target <= 9'd64;
        end else if (apb_wr & ~apb_paddr[9]) begin
            case (apb_paddr[8:2])
                7'h01: en     <= apb_pwdata[0];
                7'h02: mvol   <= apb_pwdata[8:0];
                7'h03: target <= apb_pwdata[8:0];
                default: ;
            endcase
        end
    end

    always @(*) begin
        apb_prdata = 32'd0;
        if (~apb_paddr[9]) begin
            case (apb_paddr[8:2])
                7'h00: apb_prdata = 32'h4D495831;
                7'h01: apb_prdata = {31'd0, en};
                7'h02: apb_prdata = {23'd0, mvol};
                7'h03: apb_prdata = {23'd0, target};
                7'h06: apb_prdata = {16'd0, active};
                7'h07: apb_prdata = cnt_smp;
                7'h08: apb_prdata = cnt_miss;
                7'h09: apb_prdata = cnt_fetch;
                7'h0A: apb_prdata = cnt_clip;
                7'h0B: apb_prdata = 32'h00012010;
                default: apb_prdata = 32'd0;
            endcase
        end
    end
endmodule


// =============================================================================
// aud_rd_mux - two read masters (HDMI scan-out = a, mixer = b) on one AXI read
// port (m0 of blit_axi_rd_arb). a has priority; an AR that is waiting keeps
// its grant. Both use the same ARID downstream, so R returns in request order.
// b (the mixer) has at most ONE burst in flight: when it is issued, the number
// of a-bursts still in flight ahead of it is recorded; R beats belong to b
// once those have finished (registered decision, no wide mux on R).
// Both masters keep RREADY = 1 (space reserved before AR).
// =============================================================================
module aud_rd_mux (
    input  wire        clk,
    input  wire        rst,

    input  wire        a_arvalid,
    output wire        a_arready,
    input  wire [31:0] a_araddr,
    input  wire [7:0]  a_arlen,
    input  wire [2:0]  a_arsize,
    input  wire [1:0]  a_arburst,
    output wire        a_rvalid,

    input  wire        b_arvalid,
    output wire        b_arready,
    input  wire [31:0] b_araddr,
    input  wire [7:0]  b_arlen,
    input  wire [2:0]  b_arsize,
    input  wire [1:0]  b_arburst,
    output wire        b_rvalid,

    output wire        s_arvalid,
    input  wire        s_arready,
    output wire [31:0] s_araddr,
    output wire [7:0]  s_arlen,
    output wire [2:0]  s_arsize,
    output wire [1:0]  s_arburst,
    input  wire        s_rvalid,
    input  wire        s_rlast
);
    reg  lock, lock_sel;
    wire sel = lock ? lock_sel : ~a_arvalid;      // 0 = a (HDMI), 1 = b (mixer)

    assign s_arvalid = sel ? b_arvalid : a_arvalid;
    assign s_araddr  = sel ? b_araddr  : a_araddr;
    assign s_arlen   = sel ? b_arlen   : a_arlen;
    assign s_arsize  = sel ? b_arsize  : a_arsize;
    assign s_arburst = sel ? b_arburst : a_arburst;
    assign a_arready = s_arready & ~sel;
    assign b_arready = s_arready &  sel;

    reg  [5:0]  a_out;                    // a-bursts in flight
    reg  [5:0]  b_ahead;                  // a-bursts ahead of the b-burst
    reg         b_pend;
    wire        cur_b = b_pend & (b_ahead == 6'd0);   // R beats now belong to b
    wire        ar_a  = s_arvalid & s_arready & ~sel;
    wire        ar_b  = s_arvalid & s_arready &  sel;
    wire        r_end = s_rvalid & s_rlast;
    wire        end_a = r_end & ~cur_b;
    wire        end_b = r_end &  cur_b;

    always @(posedge clk) begin
        if (rst) begin
            lock <= 1'b0;
            lock_sel <= 1'b0;
            a_out <= 6'd0;
            b_ahead <= 6'd0;
            b_pend <= 1'b0;
        end else begin
            lock     <= s_arvalid & ~s_arready;
            lock_sel <= sel;
            a_out <= a_out + {5'd0, ar_a} - {5'd0, end_a};
            if (ar_b) begin
                b_pend  <= 1'b1;
                b_ahead <= a_out - {5'd0, end_a};
            end else begin
                if (end_b) b_pend <= 1'b0;
                if (end_a & (b_ahead != 6'd0)) b_ahead <= b_ahead - 6'd1;
            end
        end
    end

    assign a_rvalid = s_rvalid & ~cur_b;
    assign b_rvalid = s_rvalid &  cur_b;
endmodule
