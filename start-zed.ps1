
$ErrorActionPreference = "Stop"

# Locate Visual Studio
$VsWhere = Join-Path ${env:ProgramFiles(x86)} `
    "Microsoft Visual Studio\Installer\vswhere.exe"

if (-not (Test-Path $VsWhere)) {
    throw "vswhere.exe not found."
}

$VsInstallPath = & $VsWhere -latest -products '*' -property installationPath

if (-not $VsInstallPath) {
    throw "Visual Studio installation not found."
}

$VsDevCmd = Join-Path $VsInstallPath "Common7\Tools\VsDevCmd.bat"

if (-not (Test-Path $VsDevCmd)) {
    throw "VsDevCmd.bat not found."
}

# Locate Zed
$Zed = (Get-Command zed -ErrorAction Stop).Source

# Create temporary launcher
$Launcher = Join-Path $env:TEMP "zed-msvc-launcher.bat"

@"
@echo off
call "$VsDevCmd" -arch=x64 -host_arch=x64
if errorlevel 1 exit /b 1

cd /d "$PSScriptRoot"
start "" "$Zed" .
"@ | Set-Content -Path $Launcher -Encoding ASCII

Write-Host "Visual Studio: $VsInstallPath" -ForegroundColor Cyan
Write-Host "Project: $PSScriptRoot" -ForegroundColor Gray
Write-Host "Launching Zed with MSVC x64 environment..." -ForegroundColor Green

try {
    & $Launcher

    if ($LASTEXITCODE -ne 0) {
        throw "Failed to launch Zed. Exit code: $LASTEXITCODE"
    }
}
finally {
    Remove-Item $Launcher -Force -ErrorAction SilentlyContinue
}
