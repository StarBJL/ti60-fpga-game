#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
if command -v verilator >/dev/null 2>&1; then
  verilator --binary --timing -Wno-fatal --top-module hdmi_dualvideo_tb \
    --Mdir hdmi_verilator_obj hdmi_dualvideo_tb.sv \
    ../../PROJECT/rtl/hdmi/hdmi_fb.v ../../PROJECT/rtl/hdmi/hdmi_island.v \
    > hdmi_verilator_build.log 2>&1
  ./hdmi_verilator_obj/Vhdmi_dualvideo_tb | tee hdmi_verilator.log
elif command -v iverilog >/dev/null 2>&1; then
  iverilog -g2012 -s hdmi_dualvideo_tb -o hdmi_dualvideo.vvp \
    hdmi_dualvideo_tb.sv ../../PROJECT/rtl/hdmi/hdmi_fb.v ../../PROJECT/rtl/hdmi/hdmi_island.v
  vvp hdmi_dualvideo.vvp | tee hdmi_dualvideo.log
else
  echo 'Install Verilator (with --timing) or Icarus Verilog to run the behavioral check.' >&2
  exit 2
fi
