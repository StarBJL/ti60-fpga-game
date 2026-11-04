# RTL Verification

The independent AXI read arbiter test is located in [axi_read_arb](axi_read_arb/tb_blit_axi_rd_arb.v). Run it with Icarus Verilog using `bash verification/axi_read_arb/run.sh`.

Repository static checks: `python3 tools/check_repo.py --hash`. See [verification scope](../docs/07_VERIFICATION.md).

Passing this single test does not establish complete AXI4 compliance, PnR timing, DDR stability, or HDMI/audio behavior.
