// tb_mixer_top.v - audio_mixer + aud_rd_mux with an HDMI-like second master,
// for tb_mixer.py (vsim.py). Shared read port s_* is driven by the testbench.
`timescale 1ns / 1ps

module tb_mixer_top (
    input  wire        clk,
    input  wire        rst,

    input  wire [11:0] apb_paddr,
    input  wire        apb_psel,
    input  wire        apb_penable,
    input  wire        apb_pwrite,
    input  wire [31:0] apb_pwdata,
    output wire [31:0] apb_prdata,
    output wire        apb_pready,

    input  wire        h_arvalid,
    input  wire [31:0] h_araddr,
    output wire        h_arready,
    output wire        h_rvalid,

    output wire        s_arvalid,
    input  wire        s_arready,
    output wire [31:0] s_araddr,
    output wire [7:0]  s_arlen,
    output wire [2:0]  s_arsize,
    output wire [1:0]  s_arburst,
    input  wire        s_rvalid,
    input  wire [31:0] s_rdata,
    input  wire        s_rlast,

    output wire        o_valid,
    input  wire        o_ready,
    output wire [31:0] o_data,
    input  wire [8:0]  o_level
);
    wire        axi_arvalid_mix, axi_arready_mix, axi_rvalid_mix;
    wire [31:0] m_araddr;
    wire [7:0]  m_arlen;
    wire [2:0]  m_arsize;
    wire [1:0]  m_arburst;
    wire        m_rready, pslverr;

    audio_mixer #(.LVL_W(9)) u_mix (
        .clk (clk), .rst (rst), .axi_rst (rst),
        .apb_paddr (apb_paddr), .apb_psel (apb_psel), .apb_penable (apb_penable),
        .apb_pwrite (apb_pwrite), .apb_pwdata (apb_pwdata), .apb_prdata (apb_prdata),
        .apb_pready (apb_pready), .apb_pslverror (pslverr),
        .axi_arvalid (axi_arvalid_mix), .axi_arready (axi_arready_mix),
        .axi_araddr (m_araddr), .axi_arlen (m_arlen), .axi_arsize (m_arsize),
        .axi_arburst (m_arburst), .axi_rvalid (axi_rvalid_mix), .axi_rready (m_rready),
        .axi_rdata (s_rdata), .axi_rlast (s_rlast),
        .o_valid (o_valid), .o_ready (o_ready), .o_data (o_data), .o_level (o_level)
    );

    aud_rd_mux u_mux (
        .clk (clk), .rst (rst),
        .a_arvalid (h_arvalid), .a_arready (h_arready), .a_araddr (h_araddr),
        .a_arlen (8'd63), .a_arsize (3'd2), .a_arburst (2'd1), .a_rvalid (h_rvalid),
        .b_arvalid (axi_arvalid_mix), .b_arready (axi_arready_mix), .b_araddr (m_araddr),
        .b_arlen (m_arlen), .b_arsize (m_arsize), .b_arburst (m_arburst),
        .b_rvalid (axi_rvalid_mix),
        .s_arvalid (s_arvalid), .s_arready (s_arready), .s_araddr (s_araddr),
        .s_arlen (s_arlen), .s_arsize (s_arsize), .s_arburst (s_arburst),
        .s_rvalid (s_rvalid), .s_rlast (s_rlast)
    );
endmodule
