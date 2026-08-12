<# 
.SYNOPSIS
    Build script for LatencyTester (Windows Desktop).
    Compiles the Qt application and generates Doxygen documentation.

.DESCRIPTION
    - Runs qmake + mingw32-make to build the application in Release mode.
    - Generates API documentation via Doxygen into Documentation/.
    - Requires Qt 6.11+ (qmake, mingw32-make) and Doxygen in PATH.

.EXAMPLE
    .\build.ps1
    .\build.ps1 -Clean
    .\build.ps1 -DocsOnly
#>

param(
    [switch]$Clean,
    [switch]$DocsOnly,
    [switch]$NoDocs,
    [int]$Jobs = 8
)

$ErrorActionPreference = "Stop"
$ScriptDir = $PSScriptRoot
$ScriptsDir = Split-Path $ScriptDir -Parent     # LatencyTester/Scripts/
$ProjectDir = Split-Path $ScriptsDir -Parent    # LatencyTester/
$RepoRoot = Split-Path $ProjectDir -Parent      # latency_tester/
$BuildDir = Join-Path $ProjectDir "build\Desktop_Release"
$DocsDir = Join-Path $ProjectDir "Documentation"
$DoxyfilePath = Join-Path $ScriptsDir "Doxyfile"

function Write-Step($msg) { Write-Host "`n>> $msg" -ForegroundColor Cyan }

# --- Clean ---
if ($Clean) {
    Write-Step "Cleaning build artifacts"
    if (Test-Path $BuildDir) { Remove-Item -Recurse -Force $BuildDir }
    if (Test-Path $DocsDir) { Remove-Item -Recurse -Force $DocsDir }
    Write-Host "Clean complete."
    if (-not $DocsOnly) { exit 0 }
}

# --- Build application ---
if (-not $DocsOnly) {
    Write-Step "Checking prerequisites"

    $qmake = Get-Command qmake -ErrorAction SilentlyContinue
    if (-not $qmake) {
        Write-Error "qmake not found in PATH. Ensure Qt 6.11+ bin directory is in your PATH."
        exit 1
    }
    Write-Host "  qmake: $($qmake.Source)"

    $make = Get-Command mingw32-make -ErrorAction SilentlyContinue
    if (-not $make) {
        Write-Error "mingw32-make not found in PATH. Ensure MinGW bin directory is in your PATH."
        exit 1
    }
    Write-Host "  mingw32-make: $($make.Source)"

    Write-Step "Creating build directory"
    if (-not (Test-Path $BuildDir)) {
        New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
    }

    Write-Step "Running qmake (Release)"
    Push-Location $BuildDir
    try {
        & qmake "$ProjectDir\LatencyTester.pro" "CONFIG+=release" "CONFIG-=debug"
        if ($LASTEXITCODE -ne 0) { throw "qmake failed with exit code $LASTEXITCODE" }

        Write-Step "Compiling ($Jobs parallel jobs)"
        & mingw32-make -j$Jobs
        if ($LASTEXITCODE -ne 0) { throw "mingw32-make failed with exit code $LASTEXITCODE" }
    }
    finally {
        Pop-Location
    }

    $exe = Join-Path $BuildDir "release\LatencyTester.exe"
    if (Test-Path $exe) {
        Write-Host "`n  Build successful: $exe" -ForegroundColor Green
    } else {
        # Some configurations output directly
        $exeAlt = Join-Path $BuildDir "LatencyTester.exe"
        if (Test-Path $exeAlt) {
            $exe = $exeAlt
            Write-Host "`n  Build successful: $exeAlt" -ForegroundColor Green
        } else {
            Write-Warning "Build completed but binary not found at expected location."
        }
    }

    # --- Deploy (windeployqt) ---
    if (Test-Path $exe) {
        Write-Step "Running windeployqt (packaging DLLs)"
        $windeployqt = Get-Command windeployqt -ErrorAction SilentlyContinue
        if (-not $windeployqt) {
            # Try to find it relative to qmake
            $qtBinDir = Split-Path $qmake.Source -Parent
            $windeployqtPath = Join-Path $qtBinDir "windeployqt.exe"
            if (Test-Path $windeployqtPath) {
                $windeployqt = Get-Item $windeployqtPath
            }
        }
        if (-not $windeployqt) {
            # Fallback: well-known Qt installation path
            $fallbackPath = "C:\Qt\6.11.1\mingw_64\bin\windeployqt.exe"
            if (Test-Path $fallbackPath) {
                $windeployqt = Get-Item $fallbackPath
            }
        }

        if ($windeployqt) {
            $deployDir = Join-Path $ProjectDir "deploy"
            if (Test-Path $deployDir) { Remove-Item -Recurse -Force $deployDir }
            New-Item -ItemType Directory -Force -Path $deployDir | Out-Null
            Copy-Item $exe -Destination $deployDir

            $deployExe = Join-Path $deployDir "LatencyTester.exe"
            & $windeployqt.FullName $deployExe --release --no-translations --no-system-d3d-compiler --no-opengl-sw 2>$null
            if ($LASTEXITCODE -eq 0) {
                # Copy Measures folder if it exists
                $measuresDir = Join-Path $ProjectDir "Measures"
                if (Test-Path $measuresDir) {
                    Copy-Item -Recurse $measuresDir -Destination $deployDir
                } else {
                    New-Item -ItemType Directory -Force -Path (Join-Path $deployDir "Measures") | Out-Null
                }
                Write-Host "  Deployable package: $deployDir" -ForegroundColor Green
                Write-Host "  (ZIP this folder to distribute)"
            } else {
                Write-Warning "windeployqt returned an error. Package may be incomplete."
            }
        } else {
            Write-Warning "windeployqt not found. Skipping deployment packaging."
            Write-Host "  Add Qt's bin directory to PATH or install windeployqt."
        }
    }
}

# --- Generate documentation ---
if (-not $NoDocs) {
    Write-Step "Generating Doxygen documentation"

    $doxygen = Get-Command doxygen -ErrorAction SilentlyContinue
    if (-not $doxygen) {
        Write-Warning "Doxygen not found in PATH. Skipping documentation generation."
        Write-Host "  Install from: https://www.doxygen.nl/download.html"
    } else {
        Write-Host "  doxygen: $($doxygen.Source)"

        if (-not (Test-Path $DocsDir)) {
            New-Item -ItemType Directory -Force -Path $DocsDir | Out-Null
        }

        if (-not (Test-Path $DoxyfilePath)) {
            Write-Error "Doxyfile not found at $DoxyfilePath"
            exit 1
        }

        Push-Location $ProjectDir
        try {
            & doxygen $DoxyfilePath
            if ($LASTEXITCODE -ne 0) { throw "Doxygen failed with exit code $LASTEXITCODE" }
        }
        finally {
            Pop-Location
        }

        Write-Host "  Documentation generated: $DocsDir" -ForegroundColor Green
    }
}

Write-Host "`n>> Done." -ForegroundColor Green
