
// Efinity Top-level template
// Version: 2026.1.132.4.5
// Date: 2026-10-24 01:43

// Copyright (C) 2013 - 2026 Efinix Inc. All rights reserved.

// This file may be used as a starting point for Efinity synthesis top-level target.
// The port list here matches what is expected by Efinity constraint files generated
// by the Efinity Interface Designer.

// To use this:
//     #1)  Save this file with a different name to a different directory, where source files are kept.
//              Example: you may wish to save as ddr_demo_ti60.v
//     #2)  Add the newly saved file into Efinity project as design file
//     #3)  Edit the top level entity in Efinity project to:  ddr_demo_ti60
//     #4)  Insert design content.


module ddr_demo_ti60
(
  (* syn_peri_port = 0 *) input i_arstn,
  (* syn_peri_port = 0 *) input io_jtag_tck,
  (* syn_peri_port = 0 *) input io_jtag_tdi,
  (* syn_peri_port = 0 *) input io_jtag_tms,
  (* syn_peri_port = 0 *) input [3:0] soc_gpio_IN,
  (* syn_peri_port = 0 *) input system_uart_0_io_rxd,
  (* syn_peri_port = 0 *) input keyboard_rx,
  (* syn_peri_port = 0 *) input user_pll_locked,
  (* syn_peri_port = 0 *) input clk_100m,
  (* syn_peri_port = 0 *) input pll_locked,
  (* syn_peri_port = 0 *) input hdmi_tx_locked,
  (* syn_peri_port = 0 *) input hdmi_tx_slow_clk,
  (* syn_peri_port = 0 *) input hdmi_fb_clk,
  (* syn_peri_port = 0 *) input hdmi_tx_fast_clk,
  (* syn_peri_port = 0 *) input core_clk,
  (* syn_peri_port = 0 *) input pll_fb_clk,
  (* syn_peri_port = 0 *) input clk_25m,
  (* syn_peri_port = 0 *) input sdram_clk,
  (* syn_peri_port = 0 *) input tx_cal_clk_90edge,
  (* syn_peri_port = 0 *) input rx_cal_clk,
  (* syn_peri_port = 0 *) input tx_cal_clk,
  (* syn_peri_port = 0 *) input jtag_inst1_CAPTURE,
  (* syn_peri_port = 0 *) input jtag_inst1_DRCK,
  (* syn_peri_port = 0 *) input jtag_inst1_RESET,
  (* syn_peri_port = 0 *) input jtag_inst1_RUNTEST,
  (* syn_peri_port = 0 *) input jtag_inst1_SEL,
  (* syn_peri_port = 0 *) input jtag_inst1_SHIFT,
  (* syn_peri_port = 0 *) input jtag_inst1_TCK,
  (* syn_peri_port = 0 *) input jtag_inst1_TDI,
  (* syn_peri_port = 0 *) input jtag_inst1_TMS,
  (* syn_peri_port = 0 *) input jtag_inst1_UPDATE,
  (* syn_peri_port = 0 *) input [15:0] ddr_dq_in_hi,
  (* syn_peri_port = 0 *) input [15:0] ddr_dq_in_lo,
  (* syn_peri_port = 0 *) input [1:0] ddr_dqs_in_hi,
  (* syn_peri_port = 0 *) input [1:0] ddr_dqs_in_lo,
  (* syn_peri_port = 0 *) input system_spi_0_io_data_0_IN,
  (* syn_peri_port = 0 *) input system_spi_0_io_data_1_IN,
  (* syn_peri_port = 0 *) input [3:0] matrix_row_IN,
  (* syn_peri_port = 0 *) input [3:0] matrix_col,
  (* syn_peri_port = 0 *) output io_jtag_tdo,
  (* syn_peri_port = 0 *) output [3:0] soc_gpio_OUT,
  (* syn_peri_port = 0 *) output [3:0] soc_gpio_OE,
  (* syn_peri_port = 0 *) output system_uart_0_io_txd,
  (* syn_peri_port = 0 *) output keyboard_tx,
  (* syn_peri_port = 0 *) output [2:0] pll_shift,
  (* syn_peri_port = 0 *) output pll_shift_ena,
  (* syn_peri_port = 0 *) output [4:0] pll_shift_sel,
  (* syn_peri_port = 0 *) output jtag_inst1_TDO,
  (* syn_peri_port = 0 *) output tmds_clk_TX_OE,
  (* syn_peri_port = 0 *) output [9:0] tmds_clk_o,
  (* syn_peri_port = 0 *) output tmds_clk_TX_RST,
  (* syn_peri_port = 0 *) output tmds_data0_TX_OE,
  (* syn_peri_port = 0 *) output [9:0] tmds_data0_o,
  (* syn_peri_port = 0 *) output tmds_data0_TX_RST,
  (* syn_peri_port = 0 *) output tmds_data1_TX_OE,
  (* syn_peri_port = 0 *) output [9:0] tmds_data1_o,
  (* syn_peri_port = 0 *) output tmds_data1_TX_RST,
  (* syn_peri_port = 0 *) output tmds_data2_TX_OE,
  (* syn_peri_port = 0 *) output [9:0] tmds_data2_o,
  (* syn_peri_port = 0 *) output tmds_data2_TX_RST,
  (* syn_peri_port = 0 *) output [15:0] ddr_addr,
  (* syn_peri_port = 0 *) output [2:0] ddr_ba,
  (* syn_peri_port = 0 *) output ddr_cas_n,
  (* syn_peri_port = 0 *) output ddr_ck_hi,
  (* syn_peri_port = 0 *) output ddr_ck_lo,
  (* syn_peri_port = 0 *) output [0:0] ddr_cke,
  (* syn_peri_port = 0 *) output [0:0] ddr_cs_n,
  (* syn_peri_port = 0 *) output [1:0] ddr_dm_hi,
  (* syn_peri_port = 0 *) output [1:0] ddr_dm_lo,
  (* syn_peri_port = 0 *) output [15:0] ddr_dq_out_hi,
  (* syn_peri_port = 0 *) output [15:0] ddr_dq_out_lo,
  (* syn_peri_port = 0 *) output [15:0] ddr_dq_oe,
  (* syn_peri_port = 0 *) output [1:0] ddr_dqs_out_hi,
  (* syn_peri_port = 0 *) output [1:0] ddr_dqs_out_lo,
  (* syn_peri_port = 0 *) output [1:0] ddr_dqs_oe,
  (* syn_peri_port = 0 *) output [1:0] ddr_dqs_oe_n,
  (* syn_peri_port = 0 *) output [0:0] ddr_odt,
  (* syn_peri_port = 0 *) output ddr_ras_n,
  (* syn_peri_port = 0 *) output ddr_reset_n,
  (* syn_peri_port = 0 *) output ddr_we_n,
  (* syn_peri_port = 0 *) output system_spi_0_io_data_0_OUT,
  (* syn_peri_port = 0 *) output system_spi_0_io_data_0_OE,
  (* syn_peri_port = 0 *) output system_spi_0_io_data_1_OUT,
  (* syn_peri_port = 0 *) output system_spi_0_io_data_1_OE,
  (* syn_peri_port = 0 *) output system_spi_0_io_sclk_write,
  (* syn_peri_port = 0 *) output system_spi_0_io_ss,
  (* syn_peri_port = 0 *) output [3:0] matrix_row_OUT,
  (* syn_peri_port = 0 *) output [3:0] matrix_row_OE
);


endmodule

