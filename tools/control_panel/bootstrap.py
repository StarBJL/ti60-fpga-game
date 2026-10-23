#!/usr/bin/env python3
"""Prepare a project-local Python environment, then run the serial console.

No device is opened automatically. All paths are passed as subprocess arguments,
so spaces and non-ASCII project names work without shell quoting tricks.
"""
from __future__ import annotations
import argparse
import os
from pathlib import Path
import subprocess
import sys
import venv

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]

def run(argv):
    print("Running:", " ".join(str(a) for a in argv), flush=True)
    subprocess.run([str(a) for a in argv], check=True)

def interpreter(environment):
    return environment / ("Scripts/python.exe" if os.name == "nt" else "bin/python")

def has_dependency(python):
    try:
        result = subprocess.run([str(python), "-c", "import serial; assert serial.VERSION == '3.5'"],
                                stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        return result.returncode == 0
    except OSError:
        return False

def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--venv-dir", type=Path, default=ROOT / ".venv-control")
    parser.add_argument("--no-install", action="store_true", help="Never access the network or install packages")
    parser.add_argument("--check", action="store_true", help="Check environment only; do not launch a browser/server")
    parser.add_argument("--system-python", action="store_true", help="Reuse this interpreter if it already has pyserial 3.5; never install globally")
    args, server_args = parser.parse_known_args(argv)
    if sys.version_info < (3, 10):
        parser.error("Python 3.10 or later is required. Install Python from python.org and enable its launcher.")
    environment = args.venv_dir.expanduser().resolve()
    python = interpreter(environment)
    try:
        if args.system_python and has_dependency(sys.executable):
            python = Path(sys.executable)
        if not python.is_file():
            if args.no_install:
                parser.error("Local environment is absent. Start normally once, or prepare --venv-dir offline.")
            print("Creating isolated environment:", environment, flush=True)
            try:
                venv.EnvBuilder(with_pip=True).create(environment)
            except SystemExit as exc:
                raise RuntimeError("venv/ensurepip is unavailable. On Debian/Ubuntu install the matching python3-venv package, or use --system-python with pyserial already installed.") from exc
        if not has_dependency(python):
            if args.no_install:
                parser.error("pyserial 3.5 is missing. Install requirements.txt into the local environment first.")
            run([python, "-m", "pip", "install", "--disable-pip-version-check", "--no-input", "-r", HERE / "requirements.txt"])
        if not has_dependency(python):
            raise RuntimeError("Dependency check failed after installation")
        print("Python and pyserial are ready. No serial device has been opened.", flush=True)
        if not args.check:
            run([python, HERE / "server.py", *server_args])
    except KeyboardInterrupt:
        return 0
    except (OSError, subprocess.CalledProcessError, RuntimeError) as exc:
        print("Console startup failed:", exc, file=sys.stderr)
        print("Use a writable extracted folder. If offline, install pyserial 3.5 from a local wheel into", python, file=sys.stderr)
        return 1
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
