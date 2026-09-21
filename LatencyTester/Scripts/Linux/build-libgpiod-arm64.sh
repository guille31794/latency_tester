#!/usr/bin/env bash
# =============================================================================
# Cross-compile libgpiod v2 (with C++ bindings) for ARM64 and install into sysroot.
#
# Ubuntu 24.04 ships libgpiod-dev v1.6.x which does NOT include the v2 C++
# bindings (gpiod::line_config, gpiod::line_settings, etc.) needed by rpi_ads1115.
# This script compiles libgpiod v2.2 from source with --enable-bindings-cxx.
#
# Prerequisites:
#   - Cross-compiler: sudo apt install gcc-13-aarch64-linux-gnu g++-13-aarch64-linux-gnu
#   - Build tools: sudo apt install autoconf automake libtool pkg-config autoconf-archive
#   - Sysroot extracted: ./extract-sysroot.sh
#
# Usage:
#   ./build-libgpiod-arm64.sh              Build and install to ~/ARM64-sysroot
#   ./build-libgpiod-arm64.sh --sysroot /path   Custom sysroot
#   ./build-libgpiod-arm64.sh --clean      Remove source directory
# =============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SYSROOT="${HOME}/ARM64-sysroot"
LIBGPIOD_VERSION="2.2"
LIBGPIOD_DIR="/tmp/libgpiod-${LIBGPIOD_VERSION}"
LIBGPIOD_URL="https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/snapshot/libgpiod-${LIBGPIOD_VERSION}.tar.gz"
INSTALL_PREFIX="/usr"
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
    step "Removing libgpiod source directory"
    rm -rf "$LIBGPIOD_DIR"
    ok "Clean complete."
    exit 0
fi

# --- Verify prerequisites ---
step "Checking prerequisites"

command -v aarch64-linux-gnu-gcc-13 >/dev/null 2>&1 || \
    err "aarch64-linux-gnu-gcc-13 not found. Install: sudo apt install gcc-13-aarch64-linux-gnu"

command -v autoconf >/dev/null 2>&1 || \
    err "autoconf not found. Install: sudo apt install autoconf automake libtool autoconf-archive"

command -v pkg-config >/dev/null 2>&1 || \
    err "pkg-config not found. Install: sudo apt install pkg-config"

[[ -d "$SYSROOT/usr/lib/aarch64-linux-gnu" ]] || \
    err "Sysroot not found at $SYSROOT. Run ./extract-sysroot.sh first."

echo "  Cross-compiler: $(aarch64-linux-gnu-gcc-13 --version | head -1)"
echo "  Sysroot: $SYSROOT"

# --- Download source ---
if [[ -d "$LIBGPIOD_DIR" ]]; then
    ok "Source already exists at $LIBGPIOD_DIR"
else
    step "Downloading libgpiod v${LIBGPIOD_VERSION} source..."
    cd /tmp
    wget -q --show-progress "$LIBGPIOD_URL" -O "libgpiod-${LIBGPIOD_VERSION}.tar.gz"
    tar xzf "libgpiod-${LIBGPIOD_VERSION}.tar.gz"
    rm "libgpiod-${LIBGPIOD_VERSION}.tar.gz"
    ok "Source extracted to $LIBGPIOD_DIR"
fi

# --- Build ---
step "Configuring libgpiod v${LIBGPIOD_VERSION} for ARM64 (with C++ bindings)..."
cd "$LIBGPIOD_DIR"

# Generate configure script if not present
if [[ ! -f configure ]]; then
    step "Running autogen.sh..."
    mkdir -p m4
    autoreconf -fi
fi

# Configure for cross-compilation with C++ bindings enabled
# NOTE: Do NOT pass --sysroot here. The cross-compiler (aarch64-linux-gnu-gcc-13)
# has its own sysroot at /usr/aarch64-linux-gnu/ which contains the CRT objects
# (crt1.o, crti.o) needed for configure's test programs.
# Our custom sysroot is only needed for the final LatencyTester link (Qt libs).
./configure \
    --host=aarch64-linux-gnu \
    --prefix="$INSTALL_PREFIX" \
    --enable-bindings-cxx \
    --disable-tools \
    --disable-tests \
    --disable-bindings-python \
    CC=aarch64-linux-gnu-gcc-13 \
    CXX=aarch64-linux-gnu-g++-13

step "Compiling libgpiod ($JOBS parallel jobs)..."
make -j"$JOBS"

ok "Build complete."

# --- Install to sysroot ---
step "Installing to sysroot at $SYSROOT"

# Install to a temporary prefix, then copy to sysroot
TEMP_INSTALL="/tmp/libgpiod-install"
rm -rf "$TEMP_INSTALL"
make DESTDIR="$TEMP_INSTALL" install

# Copy libraries to sysroot
mkdir -p "$SYSROOT/usr/lib/aarch64-linux-gnu"
cp -P "$TEMP_INSTALL$INSTALL_PREFIX/lib"/libgpiod*.so* "$SYSROOT/usr/lib/aarch64-linux-gnu/" 2>/dev/null || true
cp -P "$TEMP_INSTALL$INSTALL_PREFIX/lib"/libgpiod*.a "$SYSROOT/usr/lib/aarch64-linux-gnu/" 2>/dev/null || true

# Copy headers to sysroot
mkdir -p "$SYSROOT/usr/include"
cp -f "$TEMP_INSTALL$INSTALL_PREFIX/include/gpiod.h" "$SYSROOT/usr/include/" 2>/dev/null || true
cp -f "$TEMP_INSTALL$INSTALL_PREFIX/include/gpiod.hpp" "$SYSROOT/usr/include/" 2>/dev/null || true
# Copy the gpiodcxx/ subdirectory (v2 C++ bindings split headers)
if [[ -d "$TEMP_INSTALL$INSTALL_PREFIX/include/gpiodcxx" ]]; then
    cp -rf "$TEMP_INSTALL$INSTALL_PREFIX/include/gpiodcxx" "$SYSROOT/usr/include/"
    ok "Copied gpiodcxx/ headers directory"
fi
# Also check for gpiod-cxx/ variant
if [[ -d "$TEMP_INSTALL$INSTALL_PREFIX/include/gpiod-cxx" ]]; then
    cp -rf "$TEMP_INSTALL$INSTALL_PREFIX/include/gpiod-cxx" "$SYSROOT/usr/include/"
    ok "Copied gpiod-cxx/ headers directory"
fi

# Copy pkgconfig
mkdir -p "$SYSROOT/usr/lib/aarch64-linux-gnu/pkgconfig"
cp -f "$TEMP_INSTALL$INSTALL_PREFIX/lib/pkgconfig"/libgpiod*.pc "$SYSROOT/usr/lib/aarch64-linux-gnu/pkgconfig/" 2>/dev/null || true

# Cleanup temp
rm -rf "$TEMP_INSTALL"

# --- Summary ---
echo ""
ok "libgpiod v${LIBGPIOD_VERSION} with C++ bindings installed to sysroot!"
echo ""
echo "  Libraries:"
find "$SYSROOT" -name "libgpiod*" -type f 2>/dev/null | sort | sed 's/^/    /'
echo ""
echo "  Headers:"
find "$SYSROOT" -name "gpiod*" -path "*/include/*" 2>/dev/null | sort | sed 's/^/    /'
echo ""
echo "  Next steps:"
echo "    ./build-pigpio-arm64.sh          # Build pigpio for ARM64"
echo "    ./build-arm64.sh --production    # Build LatencyTester with real drivers"
