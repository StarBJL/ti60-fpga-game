# Build on Windows, with Python 3.10+ and the requirements in requirements-build.txt.
from pathlib import Path
panel = Path(SPECPATH)
a = Analysis([str(panel / 'server.py')], pathex=[str(panel)],
             binaries=[], datas=[(str(panel / name), '.') for name in ('index.html', 'app.js', 'style.css', 'wiring.html', 'keys.html', 'matrix_keys.def')],
             hiddenimports=['serial.tools.list_ports', 'serial.tools.list_ports_windows'],
             hookspath=[], hooksconfig={}, runtime_hooks=[], excludes=[], noarchive=False)
pyz = PYZ(a.pure)
exe = EXE(pyz, a.scripts, a.binaries, a.datas, [], name='Ti60Control',
          debug=False, bootloader_ignore_signals=False, strip=False, upx=False,
          console=True, disable_windowed_traceback=False)
