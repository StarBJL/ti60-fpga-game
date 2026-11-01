@echo off
setlocal
set "PYTHONUTF8=1"
set "PYTHONNOUSERSITE=1"
"%~dp0tools\windows_runtime\python.exe" "%~dp0tools\control_panel\server.py" --open --port 0 --log-root "%LOCALAPPDATA%\Ti60V6\logs" %*
if errorlevel 1 pause
endlocal
