#!/usr/bin/env bash
# =============================================================================
# Cross-compile pigpio for ARM64 and install into the sysroot.
#
# pigpio is a Raspberry Pi-specific GPIO library that is NOT available in
# Debian/Ubuntu ARM64 repositories. It must be compiled from source using
# the cross-compiler.
#
# Prerequisites:
#   - Cross-compiler: sudo apt install gcc-13-aarch64-linux-gnu g++-13-aarch64-linux-gnu
#   - Sysroot extracted: ./extract-sysroot.sh
#
# Usage:
#   ./build-pigpio-arm64.sh              Build and install to ~/ARM64-sysroot
#   ./build-pigpio-arm64.sh --sysroot /path   Custom sysroot
#   ./build-pigpio-arm64.sh --clean      Remove pigpio source directory
# =============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SYSROOT="${HOME}/ARM64-sysroot"
PIGPIO_DIR="/tmp/pigpio-master"
PIGPIO_URL="https://github.com/joan2937/pigpio/archive/master.tar.gz"
JOBS=$(nproc 2>/dev/null || echo 4)

# Colors
CYAN='\033[0;36m'
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

step() { echo -e "${CYAN}>> $1${NC}"; }
ok()   { echo -e "${GREEN}   $1${NC}"; }
warn() { echo -e "${YELLOW}   WARNING: $1${NC}"; }
err()  { echo -e "${RED}ERROR: $1${NC}" >&2; exit 1; }

# --- Parse arguments ---
CLEAN=false
while [[ $# -gt 0 ]]; do
    case "$1" in
        --clean)   CLEAN=true; shift ;;
        --sysroot) SYSROOT="$2"; shift 2 ;;
        *)         echo "Unknown argument: $1"; exit 1 ;;
    esac
done

if $CLEAN; then
    step "Removing pigpio source directory"
    rm -rf "$PIGPIO_DIR"
    ok "Clean complete."
    exit 0
fi

# --- Verify prerequisites ---
step "Checking prerequisites"

command -v aarch64-linux-gnu-gcc-13 >/dev/null 2>&1 || \
    err "aarch64-linux-gnu-gcc-13 not found. Install: sudo apt install gcc-13-aarch64-linux-gnu"
echo "  Cross-compiler: $(aarch64-linux-gnu-gcc-13 --version | head -1)"

[[ -d "$SYSROOT/usr/lib/aarch64-linux-gnu" ]] || \
    err "Sysroot not found at $SYSROOT. Run ./extract-sysroot.sh first."
echo "  Sysroot: $SYSROOT"

# --- Download source ---
if [[ -d "$PIGPIO_DIR" ]]; then
    ok "Source already exists at $PIGPIO_DIR"
else
    step "Downloading pigpio source..."
    cd /tmp
    wget -q --show-progress "$PIGPIO_URL" -O pigpio-master.tar.gz
    tar xzf pigpio-master.tar.gz
    rm pigpio-master.tar.gz
    ok "Source extracted to $PIGPIO_DIR"
fi

# --- Cross-compile ---
step "Cross-compiling pigpio for ARM64..."
cd "$PIGPIO_DIR"

# pigpio's Makefile supports CROSS_PREFIX for cross-compilation.
# We only need the shared library (libpigpio.so) — not the daemon or utilities.
# Build just the library object files and link them.

# Compile the core source files
aarch64-linux-gnu-gcc-13 -O2 -Wall -pthread -fPIC \
    -c pigpio.c -o pigpio.o

aarch64-linux-gnu-gcc-13 -O2 -Wall -pthread -fPIC \
    -c command.c -o command.o

# Create shared library
aarch64-linux-gnu-gcc-13 -shared -pthread -Wl,-soname,libpigpio.so.1 \
    -o libpigpio.so.1 pigpio.o command.o -lrt

# Create symlinks
ln -sf libpigpio.so.1 libpigpio.so

# Create static library too (useful for fully-static builds)
aarch64-linux-gnu-ar rcs libpigpio.a pigpio.o command.o

ok "Libraries built:"
ls -la libpigpio.*

# --- Install to sysroot ---
step "Installing to sysroot at $SYSROOT"

# Install libraries
mkdir -p "$SYSROOT/usr/local/lib"
cp -P libpigpio.so libpigpio.so.1 libpigpio.a "$SYSROOT/usr/local/lib/"

# Install header (may already exist from extract-sysroot.sh)
mkdir -p "$SYSROOT/usr/local/include"
cp pigpio.h "$SYSROOT/usr/local/include/"

# Also ensure it's findable via /usr/include (where extract-sysroot.sh puts it)
cp pigpio.h "$SYSROOT/usr/include/" 2>/dev/null || true

ok "Installed to $SYSROOT/usr/local/lib/"
echo ""
echo "  Libraries:"
echo "    $SYSROOT/usr/local/lib/libpigpio.so -> libpigpio.so.1"
echo "    $SYSROOT/usr/local/lib/libpigpio.a"
echo ""
echo "  Headers:"
echo "    $SYSROOT/usr/local/include/pigpio.h"
echo ""
echo "  You can now build for production:"
echo "    ./build-arm64.sh --production"
