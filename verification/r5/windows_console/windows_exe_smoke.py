"""Run on Windows after packaging; never opens a physical serial port."""
from pathlib import Path
import json
import os
import queue
import re
import subprocess
import sys
import tempfile
import threading
import time
import urllib.request

ROOT = Path(__file__).resolve().parents[3]

def main():
    if os.name != 'nt':
        print('This smoke test requires Windows; it cannot validate a Windows EXE on Linux.', file=sys.stderr)
        return 2
    executable = ROOT / 'dist/windows/Ti60Control.exe'
    lines = queue.Queue()
    result = {'platform': sys.platform, 'windows_executed': True, 'serial_device_opened': False, 'passed': False}
    with tempfile.TemporaryDirectory(prefix='Ti60 中文 路径 ') as temporary:
        process = subprocess.Popen([str(executable), '--no-browser', '--port', '0', '--log-root', temporary],
                                   stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                   text=True, encoding='utf-8', errors='replace',
                                   env={**os.environ, 'PYTHONUTF8':'1', 'PYTHONIOENCODING':'utf-8'})
        def reader():
            for line in process.stdout: lines.put(line)
        threading.Thread(target=reader, daemon=True).start()
        try:
            deadline = time.monotonic() + 30
            base = None
            output = []
            while time.monotonic() < deadline:
                if process.poll() is not None: raise RuntimeError('EXE exited before HTTP startup: ' + ''.join(output))
                try: line = lines.get(timeout=.2)
                except queue.Empty: continue
                output.append(line)
                match = re.search(r'http://127\.0\.0\.1:\d+', line)
                if match: base = match.group(); break
            if not base: raise RuntimeError('EXE did not report its loopback URL: ' + ''.join(output))
            for asset in ('/', '/app.js', '/style.css', '/wiring'):
                with urllib.request.urlopen(base + asset, timeout=5) as response:
                    payload=response.read()
                    assert response.status == 200 and payload, asset
                    if asset=='/':assert b'matrixCalBegin' in payload and b'keyboardByteTail' in payload and b'r6State' in payload
                    if asset=='/wiring':assert b'R5.9' in payload and b'J7-20' in payload
            with urllib.request.urlopen(base + '/api/state', timeout=5) as response:
                state = json.load(response)
            assert state['connected'] is False
            assert len(state['token']) > 10
            result.update(passed=True, checks=['packaged UI assets and offline wiring page', 'loopback API', 'Unicode/space log path', 'no serial auto-connect'])
            print('PASS Windows packaged executable HTTP smoke test')
        finally:
            process.terminate()
            try: process.wait(timeout=5)
            except subprocess.TimeoutExpired: process.kill(); process.wait()
            (Path(__file__).parent / 'windows_exe_results.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
    return 0

if __name__ == '__main__':
    raise SystemExit(main())
