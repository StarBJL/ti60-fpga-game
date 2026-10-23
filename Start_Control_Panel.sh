#!/usr/bin/env bash
set -euo pipefail
panel_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
exec python3 "$panel_root/tools/control_panel/bootstrap.py" --system-python --open "$@"
