@echo off
rem ---------------------------------------------------------------------------
rem Build the SproutRun HDMI demo with the Efinity RISC-V toolchain on Windows.
rem Double-click this file. All output goes to build_log.txt next to this file,
rem so Claude can read the result directly.
rem
rem If make / riscv-none-elf-gcc are not found, set SDK below to your
rem Efinity RISC-V IDE install folder (the one that contains "toolchain").
rem ---------------------------------------------------------------------------
setlocal enabledelayedexpansion
cd /d "%~dp0"

set "SDK="
if "%SDK%"=="" for /d %%D in ("C:\Efinity\efinity-riscv-ide*" "C:\Efinity\*\efinity-riscv-ide*" "D:\Efinity\efinity-riscv-ide*") do if exist "%%~D\toolchain" set "SDK=%%~D"

if not "%SDK%"=="" (
  for /d %%T in ("%SDK%\toolchain\*") do if exist "%%~T\bin\riscv-none-elf-gcc.exe" set "PATH=%%~T\bin;!PATH!"
  if exist "%SDK%\toolchain\bin\riscv-none-elf-gcc.exe" set "PATH=%SDK%\toolchain\bin;!PATH!"
  for /d %%B in ("%SDK%\build-tools\*" "%SDK%\build_tools\*") do if exist "%%~B\bin\make.exe" set "PATH=%%~B\bin;!PATH!"
  if exist "%SDK%\build-tools\bin\make.exe" set "PATH=%SDK%\build-tools\bin;!PATH!"
)

> build_log.txt (
  echo ==== build_game.bat  %DATE% %TIME%
  echo SDK=%SDK%
  echo ---- where make / gcc
  where make 2>&1
  where riscv-none-elf-gcc 2>&1
  riscv-none-elf-gcc --version 2>&1
  echo ---- make %*
  make %* 2>&1
  echo ---- exit code !ERRORLEVEL!
  if exist build\game.elf riscv-none-elf-size build\game.elf
)
type build_log.txt
echo.
echo Log written to %~dp0build_log.txt
pause
