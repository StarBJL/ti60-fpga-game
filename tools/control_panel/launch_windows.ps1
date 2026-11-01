param(
    [ValidateSet('Run', 'Build')][string]$Mode = 'Run',
    [Parameter(ValueFromRemainingArguments = $true)][string[]]$PanelArgs
)
$ErrorActionPreference = 'Stop'
$env:PYTHONUTF8 = '1'
$env:PYTHONIOENCODING = 'utf-8'
$projectRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$candidates = [System.Collections.Generic.List[string]]::new()
if ($env:TI60_PYTHON) { $candidates.Add($env:TI60_PYTHON) }
$candidates.Add((Join-Path $projectRoot '.venv-control\Scripts\python.exe'))

$launcher = Get-Command py.exe -ErrorAction SilentlyContinue
if ($launcher) {
    $launchedPath = & $launcher.Source -3 -c 'import sys; print(sys.executable)' 2>$null
    if ($LASTEXITCODE -eq 0 -and $launchedPath) { $candidates.Add(($launchedPath | Select-Object -Last 1).Trim()) }
}
foreach ($command in @(Get-Command python.exe, python3.exe -All -ErrorAction SilentlyContinue)) {
    if ($command.Source -notlike '*\Microsoft\WindowsApps\*') { $candidates.Add($command.Source) }
}
$candidates.Add((Join-Path $env:USERPROFILE '.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'))
foreach ($candidate in @(Get-ChildItem -Path (Join-Path $env:LOCALAPPDATA 'Programs\Python\Python*\python.exe') -File -ErrorAction SilentlyContinue)) {
    $candidates.Add($candidate.FullName)
}

$python = $null
foreach ($candidate in ($candidates | Select-Object -Unique)) {
    if (-not (Test-Path -LiteralPath $candidate -PathType Leaf)) { continue }
    try {
        & $candidate -c 'import sys; sys.exit(0 if sys.version_info >= (3,10) else 1)' >$null 2>$null
        if ($LASTEXITCODE -eq 0) { $python = $candidate; break }
    } catch { }
}
if (-not $python) {
    Write-Host 'Python 3.10+ was not found. Install Python or set TI60_PYTHON to its full executable path.'
    exit 1
}
Write-Host "Python: $python"
if ($Mode -eq 'Build') {
    & $python (Join-Path $PSScriptRoot 'build_windows.py') @PanelArgs
} else {
    & $python (Join-Path $PSScriptRoot 'bootstrap.py') --open @PanelArgs
}
exit $LASTEXITCODE
