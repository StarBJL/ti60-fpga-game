#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
if ! command -v iverilog >/dev/null 2>&1 || ! command -v vvp >/dev/null 2>&1; then
  echo "ERROR: install Icarus Verilog (iverilog and vvp)" >&2
  exit 2
fi
BUILD_DIR="$(mktemp -d)"
trap 'rm -rf "$BUILD_DIR"' EXIT
iverilog -g2012 -s tb_blit_axi_rd_arb -o "$BUILD_DIR/tb.vvp" \
  "$ROOT/rtl/blit/blit_axi_rd_arb.v" \
  "$ROOT/verification/axi_read_arb/tb_blit_axi_rd_arb.v"
vvp "$BUILD_DIR/tb.vvp"
