param([ValidateSet(0,1)][int]$Stats=1)
$ErrorActionPreference='Stop'
$sdk='D:\Efinity\efinity-riscv-ide-2026.1'
$game=Join-Path $PSScriptRoot 'PROJECT\par\ddr_demo_ti60\embedded_sw\soc\software\standalone\game'
$oldPath=$env:PATH
$env:PATH="$sdk\build_tools\bin;$sdk\toolchain\bin;$oldPath"
$objdir=if($Stats -eq 1){'build'}else{'build_release'}
try {
 Push-Location -LiteralPath $game
 & "$sdk\build_tools\bin\make.exe" -B -j4 "OBJDIR=$objdir" "STATS=$Stats" DEBUG=no BENCH=yes SOUND=1 *> (Join-Path $PSScriptRoot "verification\software_stats$Stats.log")
 if($LASTEXITCODE -ne 0){throw "Software build failed; see verification/software_stats$Stats.log"}
 & "$sdk\toolchain\bin\riscv-none-elf-size.exe" "$objdir/game.elf"
} finally {Pop-Location;$env:PATH=$oldPath}
