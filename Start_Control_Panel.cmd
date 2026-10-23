@echo off
setlocal
set "PYTHONUTF8=1"
set "PYTHONIOENCODING=utf-8"
py -3 -c "import sys; sys.exit(0 if sys.version_info >= (3,10) else 1)" >nul 2>nul
if not errorlevel 1 goto python_launcher
python -c "import sys; sys.exit(0 if sys.version_info >= (3,10) else 1)" >nul 2>nul
if not errorlevel 1 goto python_command
echo Python 3.10 or later was not found.
echo Install the Windows Python package from https://www.python.org/downloads/windows/
echo Enable the Python launcher during installation, then run this file again.
pause
exit /b 1
:python_launcher
py -3 "%~dp0tools\control_panel\bootstrap.py" --open %*
goto finished
:python_command
python "%~dp0tools\control_panel\bootstrap.py" --open %*
:finished
set "PANEL_EXIT=%ERRORLEVEL%"
if not "%PANEL_EXIT%"=="0" pause
exit /b %PANEL_EXIT%
