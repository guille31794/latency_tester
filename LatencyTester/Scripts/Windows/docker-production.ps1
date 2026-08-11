<#
.SYNOPSIS
    Docker Production Build Manager for LatencyTester (Windows PowerShell)

.DESCRIPTION
    Builds the ENTIRE project (Qt + libraries + LatencyTester) inside a Docker
    container. Fully portable — no Qt installation or cross-compilers needed.
    Only Docker Desktop or Rancher Desktop is required.

.PARAMETER Command
    build     - Build the production Docker image (~2-3h first time)
    extract   - Extract ARM64 production binary to .\output\
    tests     - Run unit tests (ARM64 emulated)
    valgrind  - Run Valgrind memory analysis
    vnc       - Launch app with VNC on port 5900
    shell     - Interactive shell inside the container
    clean     - Remove the Docker image

.EXAMPLE
    .\docker-production.ps1 build
    .\docker-production.ps1 extract
    .\docker-production.ps1 tests
#>

param(
    [Parameter(Position=0)]
    [ValidateSet("build","extract","tests","valgrind","vnc","shell","clean","help")]
    [string]$Command = "help"
)

$ErrorActionPreference = "Continue"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectDir = (Resolve-Path "$ScriptDir\..\..\..").Path
$ImageName = "latencytester-production"

# Dockerfile lives in the Linux scripts folder
$DockerfilePath = Join-Path $ProjectDir "LatencyTester\Scripts\Linux\Dockerfile.production"

function Write-Step($msg) { Write-Host ">> $msg" -ForegroundColor Cyan }
function Write-Ok($msg)   { Write-Host "   $msg" -ForegroundColor Green }
function Write-Err($msg)  { Write-Host "ERROR: $msg" -ForegroundColor Red; exit 1 }

switch ($Command) {

    "build" {
        Write-Step "Building production image (this takes 2-3 hours on first run)..."
        Write-Host ""
        Write-Host "  Image:      $ImageName"
        Write-Host "  Dockerfile: $DockerfilePath"
        Write-Host "  Context:    $ProjectDir"
        Write-Host ""

        # Ensure ARM64 emulation is available (may already be enabled on Docker Desktop/Rancher)
        Write-Step "Ensuring ARM64 emulation is available..."
        try {
            docker run --rm --privileged multiarch/qemu-user-static --reset -p yes 2>$null | Out-Null
        } catch {
            Write-Host "   (ARM64 emulation likely already enabled by Docker Desktop)" -ForegroundColor DarkGray
        }
        
        docker build `
            --platform linux/amd64 `
            -f "$DockerfilePath" `
            -t $ImageName `
            "$ProjectDir"

        if ($LASTEXITCODE -ne 0) { Write-Err "Docker build failed." }
        
        Write-Ok "Image built: $ImageName"
        Write-Host ""
        Write-Host "  Extract binary:  .\docker-production.ps1 extract"
        Write-Host "  Run tests:       .\docker-production.ps1 tests"
        Write-Host "  Run VNC:         .\docker-production.ps1 vnc"
    }

    "extract" {
        Write-Step "Extracting production binary..."
        
        $OutputDir = Join-Path $ProjectDir "output"
        if (-not (Test-Path $OutputDir)) { New-Item -ItemType Directory -Path $OutputDir | Out-Null }

        # Extract production binary
        docker run --rm $ImageName cat /app/LatencyTester | Set-Content -Path "$OutputDir\LatencyTester" -AsByteStream
        docker run --rm $ImageName cat /app/LatencyTesterTests | Set-Content -Path "$OutputDir\LatencyTesterTests" -AsByteStream

        Write-Ok "Binaries extracted to: $OutputDir"
        Write-Host ""
        Write-Host "  Production binary: $OutputDir\LatencyTester (ARM64 ELF)"
        Write-Host "  Test binary:       $OutputDir\LatencyTesterTests (ARM64 ELF)"
        Write-Host ""
        Write-Host "  Deploy to Raspberry Pi:"
        Write-Host "    scp output/LatencyTester pi@<ip>:/home/pi/LatencyTester/bin/"
    }

    "tests" {
        Write-Step "Running unit tests (ARM64 emulated)..."
        docker run --rm --platform linux/arm64 $ImageName /run-tests.sh
        if ($LASTEXITCODE -ne 0) { Write-Err "Tests failed." }
    }

    "valgrind" {
        Write-Step "Running Valgrind (ARM64 emulated)..."
        docker run --rm --platform linux/arm64 $ImageName /run-valgrind.sh
    }

    "vnc" {
        Write-Step "Launching with VNC on port 5900..."
        Write-Host "  Connect a VNC client to localhost:5900"
        docker run --rm --platform linux/arm64 -p 5900:5900 -it $ImageName /run-vnc.sh
    }

    "shell" {
        Write-Step "Starting interactive shell..."
        docker run --rm --platform linux/arm64 -it $ImageName bash
    }

    "clean" {
        Write-Step "Removing image..."
        docker rmi $ImageName 2>$null
        if ($LASTEXITCODE -eq 0) { Write-Ok "Removed." } else { Write-Host "  Not found." }
    }

    default {
        Write-Host ""
        Write-Host "LatencyTester Docker Production Builder (Windows)" -ForegroundColor White
        Write-Host ""
        Write-Host "Builds the ENTIRE project inside Docker - fully portable."
        Write-Host "No Qt installation or cross-compilers needed. Just Docker."
        Write-Host ""
        Write-Host "Usage: .\docker-production.ps1 <command>" -ForegroundColor Yellow
        Write-Host ""
        Write-Host "Commands:"
        Write-Host "  build      Build the production image (~2-3h first time)"
        Write-Host "  extract    Extract ARM64 binaries to .\output\"
        Write-Host "  tests      Run unit tests (ARM64 emulated)"
        Write-Host "  valgrind   Run Valgrind memory analysis"
        Write-Host "  vnc        Launch app with VNC (localhost:5900)"
        Write-Host "  shell      Interactive shell inside the container"
        Write-Host "  clean      Remove the Docker image"
        Write-Host ""
        Write-Host "Prerequisites:" -ForegroundColor Yellow
        Write-Host "  - Docker Desktop or Rancher Desktop (with dockerd/moby engine)"
        Write-Host "  - ARM64 emulation enabled (automatic with Docker Desktop)"
        Write-Host ""
    }
}
