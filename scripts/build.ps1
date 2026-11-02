# Builds the AFRONOVA VST3 plugin and standalone app with MSVC.
#
# Usage:
#   .\scripts\build.ps1                 # Release build
#   .\scripts\build.ps1 -Config Debug   # Debug build
#   .\scripts\build.ps1 -Clean          # wipe build folder first
#
# Requires: Visual Studio 2022 Build Tools (C++ workload) and CMake 3.22+.

[CmdletBinding()]
param(
    [ValidateSet("Debug", "Release", "RelWithDebInfo")]
    [string] $Config = "Release",
    [switch] $Clean
)

$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $projectRoot ("build-" + $Config.ToLowerInvariant())

function Resolve-CMake {
    $candidates = @(
        "D:\devtools\afronova\cmake\bin\cmake.exe",
        "C:\Program Files\CMake\bin\cmake.exe"
    )

    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) { return $candidate }
    }

    $command = Get-Command cmake -ErrorAction SilentlyContinue
    if ($command) { return $command.Source }

    throw "CMake not found. Install CMake 3.22+ or place it in D:\devtools\afronova\cmake."
}

if ($Clean -and (Test-Path $buildDir)) {
    Write-Host "Removing $buildDir" -ForegroundColor Yellow
    Remove-Item -Recurse -Force $buildDir
}

$cmake = Resolve-CMake
Write-Host "CMake:    $cmake"
Write-Host "Build:    $buildDir ($Config)" -ForegroundColor Cyan

& $cmake -S $projectRoot -B $buildDir -G "Visual Studio 17 2022" -A x64
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed." }

& $cmake --build $buildDir --config $Config --parallel
if ($LASTEXITCODE -ne 0) { throw "Build failed." }

$artefacts = Join-Path $buildDir "AFRONOVA_artefacts\$Config"
Write-Host ""
Write-Host "Build succeeded." -ForegroundColor Green
Write-Host "  VST3:       $artefacts\VST3\AFRONOVA.vst3"
Write-Host "  Standalone: $artefacts\Standalone\AFRONOVA.exe"
