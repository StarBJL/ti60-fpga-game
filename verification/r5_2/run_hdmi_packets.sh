#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
verilator --binary --timing -Wno-fatal --top-module hdmi_packets_tb \
  --Mdir hdmi_packets_obj hdmi_packets_tb.sv \
  ../../PROJECT/rtl/hdmi/hdmi_fb.v ../../PROJECT/rtl/hdmi/hdmi_island.v \
  > hdmi_packets_build.log 2>&1
./hdmi_packets_obj/Vhdmi_packets_tb > hdmi_packets_run.log 2>&1
python3 check_hdmi_packets.py | tee packets_check.log
