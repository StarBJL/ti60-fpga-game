@echo off
setlocal
set "PYTHONUTF8=1"
py -3 -c "import sys; sys.exit(0 if sys.version_info >= (3,10) else 1)" >nul 2>nul
if errorlevel 1 goto missing_python
py -3 "%~dp0tools\control_panel\build_windows.py"
set "BUILD_EXIT=%ERRORLEVEL%"
if not "%BUILD_EXIT%"=="0" pause
exit /b %BUILD_EXIT%
:missing_python
echo Install Python 3.10 or later with the Windows Python launcher, then try again.
pause
exit /b 1
