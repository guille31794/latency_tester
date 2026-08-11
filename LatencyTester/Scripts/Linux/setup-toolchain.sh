#!/usr/bin/env bash
# =============================================================================
# LatencyTester — Complete ARM64 Toolchain Setup & Production Build
#
# This script automates the ENTIRE process of setting up the cross-compilation
# toolchain and generating a production ARM64 binary from a fresh clone.
#
# It performs the following steps:
#   1. Install host dependencies (GCC 13 cross-compiler, Docker, build tools)
#   2. Enable ARM64 emulation (qemu-user-static + binfmt_misc)
#   3. Build the Docker ARM64 image (runtime environment)
#   4. Extract sysroot from Docker container
#   5. Compile Qt 6.11.1 from source for ARM64 (~2h, one-time)
#   6. Compile libgpiod v2.2 with C++ bindings for ARM64
#   7. Compile pigpio for ARM64
#   8. Cross-compile LatencyTester (production binary)
#
# Prerequisites:
#   - Ubuntu 24.04 LTS (or compatible Debian-based distro)
#   - Qt 6.11.1 host installation at ~/Qt/6.11.1/gcc_64 (from Qt Online Installer)
#   - Internet connection (downloads Qt source ~900MB, Docker images, etc.)
#   - ~20 GB free disk space
#   - sudo access (for package installation and Qt ARM64 install to /opt)
#
# Usage:
#   ./setup-toolchain.sh              Run all steps (interactive, skips completed)
#   ./setup-toolchain.sh --step N     Run from step N onwards
#   ./setup-toolchain.sh --only N     Run only step N
#   ./setup-toolchain.sh --status     Show which steps are completed
#   ./setup-toolchain.sh --help       Show this help
#
# Duration: ~2-3 hours on first run (Qt compilation dominates)
#           ~5 minutes on subsequent runs (skips completed steps)
# =============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
SYSROOT="$HOME/ARM64-sysroot"
QT_HOST="$HOME/Qt/6.11.1/gcc_64"
QT_ARM64="/opt/Qt/6.11.1/arm64"

# Colors
CYAN='\033[0;36m'
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BOLD='\033[1m'
NC='\033[0m'

step()  { echo -e "\n${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"; echo -e "${BOLD}  STEP $1: $2${NC}"; echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}\n"; }
ok()    { echo -e "${GREEN}✓ $1${NC}"; }
warn()  { echo -e "${YELLOW}⚠ $1${NC}"; }
err()   { echo -e "${RED}✗ ERROR: $1${NC}" >&2; exit 1; }
skip()  { echo -e "${GREEN}⏭ Already done: $1${NC}"; }
ask()   { echo -en "${YELLOW}? $1 [Y/n]: ${NC}"; read -r ans; [[ -z "$ans" || "$ans" =~ ^[Yy] ]]; }

# --- Parse arguments ---
START_STEP=1
ONLY_STEP=0
SHOW_STATUS=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        --step)   START_STEP="$2"; shift 2 ;;
        --only)   ONLY_STEP="$2"; shift 2 ;;
        --status) SHOW_STATUS=true; shift ;;
        --help|-h)
            head -35 "$0" | tail -30
            exit 0 ;;
        *) echo "Unknown argument: $1"; exit 1 ;;
    esac
done

# =============================================================================
# STATUS CHECK FUNCTIONS
# =============================================================================

check_step1() { command -v aarch64-linux-gnu-g++-13 &>/dev/null && command -v docker &>/dev/null; }
check_step2() { docker run --rm --platform linux/arm64 arm64v8/ubuntu:24.04 uname -m &>/dev/null 2>&1; }
check_step3() { docker image inspect latencytester-arm64 &>/dev/null 2>&1; }
check_step4() { [[ -d "$SYSROOT/usr/lib/aarch64-linux-gnu" && -d "$SYSROOT/usr/include" && -L "$SYSROOT/lib/ld-linux-aarch64.so.1" ]]; }
check_step5() { [[ -x "$QT_ARM64/bin/qmake" ]]; }
check_step6() { [[ -f "$SYSROOT/usr/lib/aarch64-linux-gnu/libgpiodcxx.so" || -f "$SYSROOT/usr/lib/aarch64-linux-gnu/libgpiodcxx.so.1" ]]; }
check_step7() { [[ -f "$SYSROOT/usr/local/lib/libpigpio.so" || -f "$SYSROOT/usr/local/lib/libpigpio.so.1" ]]; }
check_step8() { [[ -f "$PROJECT_DIR/build_arm64/LatencyTester" ]]; }

if $SHOW_STATUS; then
    echo ""
    echo -e "${BOLD}  LatencyTester ARM64 Toolchain Status${NC}"
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    for i in 1 2 3 4 5 6 7 8; do
        if check_step$i 2>/dev/null; then
            echo -e "  ${GREEN}✓${NC} Step $i"
        else
            echo -e "  ${RED}✗${NC} Step $i"
        fi
    done
    desc=("" "Host dependencies" "ARM64 emulation" "Docker image" "Sysroot" "Qt 6.11.1 ARM64" "libgpiod v2 C++" "pigpio" "Production binary")
    for i in 1 2 3 4 5 6 7 8; do
        :  # descriptions printed above
    done
    echo ""
    exit 0
fi

should_run() {
    local n=$1
    if [[ $ONLY_STEP -gt 0 ]]; then
        [[ $n -eq $ONLY_STEP ]]
    else
        [[ $n -ge $START_STEP ]]
    fi
}

# =============================================================================
echo ""
echo -e "${BOLD}╔══════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BOLD}║   LatencyTester — ARM64 Production Toolchain Setup         ║${NC}"
echo -e "${BOLD}╚══════════════════════════════════════════════════════════════╝${NC}"
echo ""
echo "  Project:  $PROJECT_DIR"
echo "  Sysroot:  $SYSROOT"
echo "  Qt Host:  $QT_HOST"
echo "  Qt ARM64: $QT_ARM64"
echo ""

# =============================================================================
# STEP 1: Host Dependencies
# =============================================================================
if should_run 1; then
    step 1 "Install host dependencies"
    
    if check_step1; then
        skip "GCC 13 cross-compiler and Docker already installed"
    else
        echo "Installing: GCC 13 cross-compiler, lld, Docker, build tools..."
        sudo apt-get update
        sudo apt-get install -y \
            gcc-13-aarch64-linux-gnu g++-13-aarch64-linux-gnu \
            lld \
            cmake ninja-build pkg-config \
            autoconf automake libtool autoconf-archive \
            binfmt-support qemu-user-static \
            docker.io docker-buildx-plugin \
            wget tar
        
        # Add user to docker group if not already
        if ! groups | grep -q docker; then
            sudo usermod -aG docker "$USER"
            warn "Added $USER to docker group. You may need to log out and back in."
            warn "If docker commands fail, run: newgrp docker"
        fi
        
        ok "Host dependencies installed"
    fi
fi

# =============================================================================
# STEP 2: Enable ARM64 Emulation
# =============================================================================
if should_run 2; then
    step 2 "Enable ARM64 emulation (qemu-user-static)"
    
    if check_step2 2>/dev/null; then
        skip "ARM64 emulation already working"
    else
        echo "Registering ARM64 binary format with QEMU..."
        docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
        ok "ARM64 emulation enabled"
    fi
fi

# =============================================================================
# STEP 3: Build Docker ARM64 Image
# =============================================================================
if should_run 3; then
    step 3 "Build Docker ARM64 image"
    
    if check_step3; then
        skip "Docker image 'latencytester-arm64' already exists"
    else
        echo "Building ARM64 container image (this may take a few minutes)..."
        "$SCRIPT_DIR/docker-rpi.sh" build
        ok "Docker image built"
    fi
fi

# =============================================================================
# STEP 4: Extract Sysroot
# =============================================================================
if should_run 4; then
    step 4 "Extract ARM64 sysroot from Docker container"
    
    if check_step4; then
        skip "Sysroot already exists at $SYSROOT"
    else
        echo "Extracting libraries, headers, and creating symlinks..."
        "$SCRIPT_DIR/extract-sysroot.sh"
        ok "Sysroot extracted to $SYSROOT"
    fi
fi

# =============================================================================
# STEP 5: Compile Qt 6.11.1 for ARM64
# =============================================================================
if should_run 5; then
    step 5 "Compile Qt 6.11.1 for ARM64 (this takes ~2 hours)"
    
    if check_step5; then
        skip "Qt ARM64 already installed at $QT_ARM64"
    else
        if [[ ! -d "$QT_HOST" ]]; then
            err "Qt host installation not found at $QT_HOST.
    Please install Qt 6.11.1 via the Qt Online Installer first.
    Download from: https://www.qt.io/download-qt-installer"
        fi
        
        echo ""
        echo "  This step compiles Qt 6.11.1 from source for ARM64."
        echo "  It requires ~15 GB of disk space and takes 1-2 hours."
        echo ""
        
        if ask "Proceed with Qt ARM64 compilation?"; then
            "$SCRIPT_DIR/build-qt6-arm64.sh"
            ok "Qt 6.11.1 ARM64 installed at $QT_ARM64"
        else
            warn "Skipped Qt compilation. Production build will not be possible without it."
        fi
    fi
fi

# =============================================================================
# STEP 6: Compile libgpiod v2.2 with C++ bindings
# =============================================================================
if should_run 6; then
    step 6 "Compile libgpiod v2.2 with C++ bindings for ARM64"
    
    if check_step6; then
        skip "libgpiodcxx already in sysroot"
    else
        echo "Downloading and cross-compiling libgpiod v2.2..."
        "$SCRIPT_DIR/build-libgpiod-arm64.sh"
        ok "libgpiod v2.2 + C++ bindings installed to sysroot"
    fi
fi

# =============================================================================
# STEP 7: Compile pigpio
# =============================================================================
if should_run 7; then
    step 7 "Compile pigpio for ARM64"
    
    if check_step7; then
        skip "libpigpio already in sysroot"
    else
        echo "Downloading and cross-compiling pigpio..."
        "$SCRIPT_DIR/build-pigpio-arm64.sh"
        ok "pigpio installed to sysroot"
    fi
fi

# =============================================================================
# STEP 8: Build Production Binary
# =============================================================================
if should_run 8; then
    step 8 "Cross-compile LatencyTester (production binary)"
    
    echo "Compiling with real drivers (USE_STUBS=0)..."
    "$SCRIPT_DIR/build-arm64.sh" --clean
    "$SCRIPT_DIR/build-arm64.sh" --production
    
    if [[ -f "$PROJECT_DIR/build_arm64/LatencyTester" ]]; then
        echo ""
        echo -e "${GREEN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
        echo -e "${GREEN}  ✓ PRODUCTION BINARY READY${NC}"
        echo -e "${GREEN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
        echo ""
        echo "  Binary: $PROJECT_DIR/build_arm64/LatencyTester"
        echo ""
        file "$PROJECT_DIR/build_arm64/LatencyTester"
        echo ""
        echo "  Deploy to Raspberry Pi:"
        echo "    scp $PROJECT_DIR/build_arm64/LatencyTester pi@<ip>:/home/pi/LatencyTester/bin/"
        echo ""
        echo "  Test in Docker (stubs mode):"
        echo "    ./build-arm64.sh            # rebuild with stubs"
        echo "    ./docker-rpi.sh run         # launch with VNC"
        echo "    ./docker-rpi.sh tests       # run unit tests"
        echo ""
    else
        err "Build completed but binary not found."
    fi
fi

echo ""
echo -e "${GREEN}All done!${NC}"
