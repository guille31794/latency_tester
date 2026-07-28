# ============================================================================
# LatencyTester - Setup Script (Windows x64)
# ============================================================================
# Verifies that all required tools are installed and configured for building
# the project on Windows (Desktop debug/release and ARM cross-compilation).
#
# Usage: .\setup.ps1
# ============================================================================

$ErrorActionPreference = "Stop"

Write-Host "============================================" -ForegroundColor Cyan
Write-Host " LatencyTester - Environment Setup (Win64) " -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
Write-Host ""

$allOk = $true

# --- 1. Check Git ---
Write-Host "[1/6] Checking Git..." -NoNewline
$git = Get-Command git -ErrorAction SilentlyContinue
if ($git) {
    $gitVersion = (git --version) -replace "git version ", ""
    Write-Host " OK ($gitVersion)" -ForegroundColor Green
} else {
    Write-Host " NOT FOUND" -ForegroundColor Red
    Write-Host "       Install from: https://git-scm.com/download/win"
    $allOk = $false
}

# --- 2. Check Qt (qmake) ---
Write-Host "[2/6] Checking Qt (qmake)..." -NoNewline
$qmake = Get-Command qmake -ErrorAction SilentlyContinue
if (-not $qmake) {
    # Try common Qt install paths
    $qtPaths = @(
        "C:\Qt\6.*\mingw_64\bin\qmake.exe",
        "C:\Qt\6.*\msvc*\bin\qmake.exe",
        "$env:USERPROFILE\Qt\6.*\mingw_64\bin\qmake.exe"
    )
    foreach ($pattern in $qtPaths) {
        $found = Get-Item $pattern -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($found) {
            $qmake = $found
            break
        }
    }
}
if ($qmake) {
    $qmakePath = if ($qmake.Source) { $qmake.Source } else { $qmake.FullName }
    $qtVersion = & $qmakePath -query QT_VERSION 2>$null
    Write-Host " OK (Qt $qtVersion)" -ForegroundColor Green
    Write-Host "       Path: $qmakePath" -ForegroundColor DarkGray
} else {
    Write-Host " NOT FOUND" -ForegroundColor Red
    Write-Host "       Install Qt 6.x from: https://www.qt.io/download-qt-installer-oss"
    Write-Host "       Required modules: Desktop MinGW 64-bit, Qt Virtual Keyboard (optional)"
    $allOk = $false
}

# --- 3. Check MinGW (g++) ---
Write-Host "[3/6] Checking MinGW (g++)..." -NoNewline
$gpp = Get-Command g++ -ErrorAction SilentlyContinue
if (-not $gpp) {
    $mingwPaths = @(
        "C:\Qt\Tools\mingw*\bin\g++.exe",
        "C:\msys64\mingw64\bin\g++.exe"
    )
    foreach ($pattern in $mingwPaths) {
        $found = Get-Item $pattern -ErrorAction SilentlyContinue | Select-Object -Last 1
        if ($found) {
            $gpp = $found
            break
        }
    }
}
if ($gpp) {
    $gppPath = if ($gpp.Source) { $gpp.Source } else { $gpp.FullName }
    $gppVersion = (& $gppPath --version | Select-Object -First 1) -replace ".*\) ", ""
    Write-Host " OK ($gppVersion)" -ForegroundColor Green
} else {
    Write-Host " NOT FOUND" -ForegroundColor Red
    Write-Host "       Installed automatically with Qt (MinGW kit) or from https://www.msys2.org/"
    $allOk = $false
}

# --- 4. Check Make ---
Write-Host "[4/6] Checking Make..." -NoNewline
$make = Get-Command mingw32-make -ErrorAction SilentlyContinue
if (-not $make) {
    $make = Get-Command make -ErrorAction SilentlyContinue
}
if (-not $make) {
    $makePaths = @(
        "C:\Qt\Tools\mingw*\bin\mingw32-make.exe"
    )
    foreach ($pattern in $makePaths) {
        $found = Get-Item $pattern -ErrorAction SilentlyContinue | Select-Object -Last 1
        if ($found) {
            $make = $found
            break
        }
    }
}
if ($make) {
    $makePath = if ($make.Source) { $make.Source } else { $make.FullName }
    Write-Host " OK" -ForegroundColor Green
    Write-Host "       Path: $makePath" -ForegroundColor DarkGray
} else {
    Write-Host " NOT FOUND" -ForegroundColor Red
    Write-Host "       Included with Qt MinGW kit or install MSYS2"
    $allOk = $false
}

# --- 5. Check ARM Toolchain (optional, for cross-compilation) ---
Write-Host "[5/6] Checking ARM Toolchain (optional)..." -NoNewline
$armGcc = Get-Command aarch64-linux-gnu-gcc -ErrorAction SilentlyContinue
if (-not $armGcc) {
    $armGcc = Get-Command arm-linux-gnueabihf-gcc -ErrorAction SilentlyContinue
}
if ($armGcc) {
    $armVersion = (& $armGcc.Source --version | Select-Object -First 1)
    Write-Host " OK" -ForegroundColor Green
    Write-Host "       $armVersion" -ForegroundColor DarkGray
} else {
    Write-Host " NOT INSTALLED (optional)" -ForegroundColor Yellow
    Write-Host "       Only needed for Raspberry Pi cross-compilation"
    Write-Host "       Download: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads"
}

# --- 6. Check Git Submodules ---
Write-Host "[6/6] Checking Git submodules..." -NoNewline
$submoduleStatus = git submodule status 2>$null
if ($submoduleStatus -match "^-") {
    Write-Host " NOT INITIALIZED" -ForegroundColor Yellow
    Write-Host "       Running: git submodule update --init --recursive"
    git submodule update --init --recursive
    Write-Host "       Done." -ForegroundColor Green
} elseif ($submoduleStatus) {
    Write-Host " OK" -ForegroundColor Green
} else {
    Write-Host " OK (no submodules or already up to date)" -ForegroundColor Green
}

# --- Summary ---
Write-Host ""
Write-Host "============================================" -ForegroundColor Cyan
if ($allOk) {
    Write-Host " All required tools found!" -ForegroundColor Green
    Write-Host " You can now open LatencyTester.pro in Qt Creator" -ForegroundColor Green
    Write-Host " or build from command line:" -ForegroundColor Green
    Write-Host ""
    Write-Host "   cd LatencyTester" -ForegroundColor White
    Write-Host "   qmake LatencyTester.pro" -ForegroundColor White
    Write-Host "   mingw32-make -j$(([Environment]::ProcessorCount))" -ForegroundColor White
} else {
    Write-Host " Some tools are missing. Install them and run again." -ForegroundColor Red
}
Write-Host "============================================" -ForegroundColor Cyan
