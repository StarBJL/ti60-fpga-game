// tb_av_top.v - hdmi_fb_top + audio_mixer + aud_rd_mux wired as in
// ddr3_example_top.v (APB split on paddr[13], shared read port s_* = m0 of
// blit_axi_rd_arb), for tb_av.py (vsim.py, one clock for clk and pix_clk).
`timescale 1ns / 1ps

module tb_av_top #(
    parameter V_ACT = 480
)(
    input  wire        clk,
    input  wire        pix_clk,
    input  wire        rst,
    input  wire        pix_rst_n,

    input  wire [15:0] apb_paddr,
    input  wire        apb_psel,
    input  wire        apb_penable,
    input  wire        apb_pwrite,
    input  wire [31:0] apb_pwdata,
    output wire [31:0] apb_prdata,
    output wire        apb_pready,

    output wire        s_arvalid,
    input  wire        s_arready,
    output wire [31:0] s_araddr,
    output wire [7:0]  s_arlen,
    input  wire        s_rvalid,
    input  wire [31:0] s_rdata,
    input  wire        s_rlast,

    output wire [9:0]  tmds_clk,
    output wire [9:0]  tmds_d0,
    output wire [9:0]  tmds_d1,
    output wire [9:0]  tmds_d2
);
    wire        apb_sel_mix  = apb_paddr[13];
    wire [31:0] hd_prdata, mx_prdata;
    wire        hd_pready, mx_pready, hd_pslverr, mx_pslverr, hdmi_irq;
    assign apb_prdata = apb_sel_mix ? mx_prdata : hd_prdata;
    assign apb_pready = apb_sel_mix ? mx_pready : hd_pready;

    wire        vd_arvalid, vd_arready, vd_rvalid, vd_rready;
    wire [31:0] vd_araddr;
    wire [7:0]  vd_arlen;
    wire [2:0]  vd_arsize;
    wire [1:0]  vd_arburst;
    wire [3:0]  vd_arid;
    wire        mx_arvalid, mx_arready, mx_rvalid, mx_rready;
    wire [31:0] mx_araddr;
    wire [7:0]  mx_arlen;
    wire [2:0]  mx_arsize, s_arsize;
    wire [1:0]  mx_arburst, s_arburst;
    wire        aud_wvalid, aud_wready;
    wire [31:0] aud_wdata;
    wire [8:0]  aud_level;

    hdmi_fb_top #(.V_ACT(V_ACT)) u_hdmi_fb (
        .clk (clk), .rst (rst), .axi_rst (rst),
        .apb_paddr (apb_paddr), .apb_psel (apb_psel & ~apb_sel_mix), .apb_penable (apb_penable),
        .apb_pwrite (apb_pwrite), .apb_pwdata (apb_pwdata), .apb_prdata (hd_prdata),
        .apb_pready (hd_pready), .apb_pslverror (hd_pslverr), .irq (hdmi_irq),
        .axi_arvalid (vd_arvalid), .axi_arready (vd_arready), .axi_araddr (vd_araddr),
        .axi_arlen (vd_arlen), .axi_arsize (vd_arsize), .axi_arburst (vd_arburst),
        .axi_arid (vd_arid), .axi_rvalid (vd_rvalid), .axi_rready (vd_rready),
        .axi_rdata (s_rdata), .axi_rlast (s_rlast), .axi_rresp (2'd0), .axi_rid (4'd0),
        .pix_clk (pix_clk), .pix_rst_n (pix_rst_n),
        .tmds_clk (tmds_clk), .tmds_d0 (tmds_d0), .tmds_d1 (tmds_d1), .tmds_d2 (tmds_d2),
        .aud_wvalid (aud_wvalid), .aud_wready (aud_wready), .aud_wdata (aud_wdata),
        .aud_level (aud_level)
    );

    audio_mixer #(.LVL_W(9)) u_audio_mixer (
        .clk (clk), .rst (rst), .axi_rst (rst),
        .apb_paddr (apb_paddr[11:0]), .apb_psel (apb_psel & apb_sel_mix), .apb_penable (apb_penable),
        .apb_pwrite (apb_pwrite), .apb_pwdata (apb_pwdata), .apb_prdata (mx_prdata),
        .apb_pready (mx_pready), .apb_pslverror (mx_pslverr),
        .axi_arvalid (mx_arvalid), .axi_arready (mx_arready), .axi_araddr (mx_araddr),
        .axi_arlen (mx_arlen), .axi_arsize (mx_arsize), .axi_arburst (mx_arburst),
        .axi_rvalid (mx_rvalid), .axi_rready (mx_rready), .axi_rdata (s_rdata), .axi_rlast (s_rlast),
        .o_valid (aud_wvalid), .o_ready (aud_wready), .o_data (aud_wdata), .o_level (aud_level)
    );

    aud_rd_mux u_aud_rd_mux (
        .clk (clk), .rst (rst),
        .a_arvalid (vd_arvalid), .a_arready (vd_arready), .a_araddr (vd_araddr),
        .a_arlen (vd_arlen), .a_arsize (vd_arsize), .a_arburst (vd_arburst), .a_rvalid (vd_rvalid),
        .b_arvalid (mx_arvalid), .b_arready (mx_arready), .b_araddr (mx_araddr),
        .b_arlen (mx_arlen), .b_arsize (mx_arsize), .b_arburst (mx_arburst), .b_rvalid (mx_rvalid),
        .s_arvalid (s_arvalid), .s_arready (s_arready), .s_araddr (s_araddr),
        .s_arlen (s_arlen), .s_arsize (s_arsize), .s_arburst (s_arburst),
        .s_rvalid (s_rvalid), .s_rlast (s_rlast)
    );
endmodule
