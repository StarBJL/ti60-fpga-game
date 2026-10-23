#!/usr/bin/env python3
"""Build the optional standalone Windows executable on a Windows machine."""
from pathlib import Path
import os
import subprocess
import sys
import venv

def main():
    if os.name != 'nt':
        print('This target must be built on Windows. PyInstaller does not cross-compile a Windows EXE on Linux.', file=sys.stderr)
        return 2
    if sys.version_info < (3, 10):
        print('Python 3.10 or later is required.', file=sys.stderr)
        return 2
    here = Path(__file__).resolve().parent
    root = here.parents[1]
    environment = root / '.venv-control-build'
    python = environment / 'Scripts/python.exe'
    try:
        if not python.exists():
            venv.EnvBuilder(with_pip=True).create(environment)
        subprocess.run([str(python), '-m', 'pip', 'install', '--disable-pip-version-check', '--no-input', '-r', str(here / 'requirements-build.txt')], check=True)
        output = root / 'dist/windows'
        subprocess.run([str(python), '-m', 'PyInstaller', '--noconfirm', '--clean',
                        '--distpath', str(output), '--workpath', str(root / 'build/control-windows'),
                        str(here / 'Ti60Control.spec')], cwd=root, check=True)
        (output / 'Start_Ti60Control.cmd').write_bytes(b'@echo off\r\n"%~dp0Ti60Control.exe" --open %*\r\nif errorlevel 1 pause\r\n')
        (output / 'README.txt').write_text('Ti60 R5 Windows console\nRun Start_Ti60Control.cmd to open the browser.\nLogs: %LOCALAPPDATA%\\Ti60R5\\logs\nThe console does not open a serial device until you click Connect.\nClose the console window or press Ctrl+C to stop it.\n', encoding='utf-8')
        print('Windows executable created:', output / 'Ti60Control.exe')
        return 0
    except (OSError, subprocess.CalledProcessError) as exc:
        print('Windows build failed:', exc, file=sys.stderr)
        return 1

if __name__ == '__main__':
    raise SystemExit(main())
