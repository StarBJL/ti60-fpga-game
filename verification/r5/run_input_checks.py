#!/usr/bin/env python3
"""Run independent C adapter and RTL input-hub checks; does not access hardware."""
from pathlib import Path
import json
import os
import shutil
import subprocess
import tempfile

here = Path(__file__).resolve().parent
root = here.parents[1]
src = root / 'PROJECT/par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src'
results = []


def tool(name):
    found = shutil.which(name)
    if not found:
        raise SystemExit(f'Required tool not found: {name}')
    return found


def run(name, command):
    result = subprocess.run(command, text=True, capture_output=True, timeout=120)
    (here / f'{name}.log').write_text(result.stdout + result.stderr)
    if result.returncode:
        raise RuntimeError(f'{name} failed; inspect {here / (name + ".log")}')
    return result.stdout


with tempfile.TemporaryDirectory(prefix='ti60-r5-input-') as tmp:
    c_exe = str(Path(tmp) / 'test_input_devices')
    run('input_devices_build', [tool(os.environ.get('CC', 'gcc')), '-O2', '-g',
        '-Wall', '-Wextra', '-Werror', '-fsanitize=address,undefined',
        '-fno-omit-frame-pointer', '-I' + str(src),
        str(here / 'test_input_devices.c'), str(src / 'input_devices.c'), '-o', c_exe])
    print(run('input_devices', [c_exe]), end='')
    results.append({'test': 'C input adapters', 'passed': True,
                    'checks': 'ASan/UBSan; Fn; held/release; merged sources; ghost detection; HID; 100/10/1; framing'})
    vvp = str(Path(tmp) / 'input_hub_tb')
    run('input_hub_build', [tool('iverilog'), '-g2012', '-Wall', '-s', 'input_hub_tb',
        '-o', vvp, str(here / 'input_hub_tb.v'), str(root / 'PROJECT/rtl/input/input_hub.v')])
    print(run('input_hub', [tool('vvp'), vvp]), end='')
    results.append({'test': 'RTL input hub', 'passed': True,
                    'checks': '8-frame debounce; bounce; onehot open-drain; UART RX/TX; FIFO wrap/overflow; framing; clear'})

(here / 'input_checks.json').write_text(json.dumps({'board_tested': False, 'results': results}, indent=2) + '\n')
