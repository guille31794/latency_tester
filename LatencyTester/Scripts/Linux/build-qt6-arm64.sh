#!/usr/bin/env bash
# =============================================================================
# Build Qt 6.11.1 from source for ARM64 cross-compilation
#
# This script downloads, configures, compiles and installs Qt 6.11.1 targeting
# ARM64 (aarch64) using the host's cross-compiler and a sysroot extracted from
# the Docker container.
#
# Prerequisites:
#   - Cross-compiler: sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
#   - Build tools: sudo apt install cmake ninja-build pkg-config
#   - Sysroot extracted: ./extract-sysroot.sh
#   - Qt 6.11.1 host installation: ~/Qt/6.11.1/gcc_64 (from Online Installer)
#   - ~15 GB disk space, ~1-2 hours compilation time
#
# Usage:
#   ./build-qt6-arm64.sh                Full build (download + configure + compile + install)
#   ./build-qt6-arm64.sh --skip-download Skip download (source already extracted)
#   ./build-qt6-arm64.sh --configure-only Only run configure step
#   ./build-qt6-arm64.sh --install-only  Only run install step (after successful build)
# =============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# --- Configuration (adjust paths if needed) ---
QT_VERSION="6.11.1"
QT_MAJOR_MINOR="6.11"
QT_SOURCE_URL="https://download.qt.io/official_releases/qt/${QT_MAJOR_MINOR}/${QT_VERSION}/single/qt-everywhere-src-${QT_VERSION}.tar.xz"
QT_SOURCE_DIR="$HOME/qt-everywhere-src-${QT_VERSION}"
QT_BUILD_DIR="$HOME/qt6-arm64-build"
QT_INSTALL_PREFIX="/opt/Qt/${QT_VERSION}/arm64"
QT_HOST_PATH="$HOME/Qt/${QT_VERSION}/gcc_64"
SYSROOT="$HOME/ARM64-sysroot"
TOOLCHAIN_FILE="$SCRIPT_DIR/qt6-arm64-toolchain.cmake"
JOBS=$(nproc 2>/dev/null || echo 4)

# Colors
CYAN='\033[0;36m'
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

step()  { echo -e "${CYAN}>> $1${NC}"; }
ok()    { echo -e "${GREEN}   $1${NC}"; }
warn()  { echo -e "${YELLOW}   WARNING: $1${NC}"; }
err()   { echo -e "${RED}ERROR: $1${NC}" >&2; exit 1; }

# --- Parse arguments ---
SKIP_DOWNLOAD=false
CONFIGURE_ONLY=false
INSTALL_ONLY=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        --skip-download)  SKIP_DOWNLOAD=true; shift ;;
        --configure-only) CONFIGURE_ONLY=true; shift ;;
        --install-only)   INSTALL_ONLY=true; shift ;;
        --jobs|-j)        JOBS="$2"; shift 2 ;;
        --sysroot)        SYSROOT="$2"; shift 2 ;;
        --prefix)         QT_INSTALL_PREFIX="$2"; shift 2 ;;
        *)                echo "Unknown argument: $1"; exit 1 ;;
    esac
done

# --- Verify prerequisites ---
step "Checking prerequisites"

command -v aarch64-linux-gnu-g++ >/dev/null 2>&1 || \
    err "Cross-compiler not found. Install: sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu"
echo "  Cross-compiler: $(aarch64-linux-gnu-g++ --version | head -1)"

command -v cmake >/dev/null 2>&1 || \
    err "cmake not found. Install: sudo apt install cmake"
echo "  cmake: $(cmake --version | head -1)"

command -v ninja >/dev/null 2>&1 || \
    err "ninja not found. Install: sudo apt install ninja-build"
echo "  ninja: $(ninja --version)"

[[ -d "$QT_HOST_PATH" ]] || \
    err "Qt host installation not found at $QT_HOST_PATH. Install Qt 6.11.1 via Online Installer."
echo "  Qt host: $QT_HOST_PATH"

[[ -d "$SYSROOT/usr/lib/aarch64-linux-gnu" ]] || \
    err "Sysroot not found at $SYSROOT. Run ./extract-sysroot.sh first."
echo "  Sysroot: $SYSROOT"

# --- Generate CMake toolchain file ---
step "Generating CMake toolchain file"
cat > "$TOOLCHAIN_FILE" << EOF
# Auto-generated CMake toolchain for ARM64 cross-compilation
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)

set(CMAKE_SYSROOT ${SYSROOT})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Ensure pkg-config looks in the sysroot
set(ENV{PKG_CONFIG_PATH} "${SYSROOT}/usr/lib/aarch64-linux-gnu/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} "${SYSROOT}")
EOF
ok "Toolchain file: $TOOLCHAIN_FILE"

# --- Download Qt source ---
if ! $SKIP_DOWNLOAD && ! $INSTALL_ONLY; then
    if [[ -d "$QT_SOURCE_DIR" ]]; then
        warn "Source directory already exists: $QT_SOURCE_DIR"
        echo "  Reusing existing source."
    else
        TARBALL="$HOME/qt-everywhere-src-${QT_VERSION}.tar.xz"
        if [[ -f "$TARBALL" ]]; then
            ok "Tarball already downloaded: $TARBALL"
        else
            step "Downloading Qt ${QT_VERSION} source (~900 MB)"
            wget -q --show-progress "$QT_SOURCE_URL" -O "$TARBALL"
        fi

        step "Extracting source (this takes a few minutes)..."
        cd "$HOME"
        tar xf "$TARBALL"
        ok "Source extracted to: $QT_SOURCE_DIR"
    fi
fi

# --- Configure ---
if ! $INSTALL_ONLY; then
    step "Configuring Qt ${QT_VERSION} for ARM64"
    mkdir -p "$QT_BUILD_DIR"
    cd "$QT_BUILD_DIR"

    "$QT_SOURCE_DIR/configure" \
        -prefix "$QT_INSTALL_PREFIX" \
        -qt-host-path "$QT_HOST_PATH" \
        -platform linux-g++ \
        -device-option CROSS_COMPILE=aarch64-linux-gnu- \
        -nomake examples \
        -nomake tests \
        -submodules qtbase,qtdeclarative,qtvirtualkeyboard,qtsvg,qtshadertools \
        -- \
        -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
        -DCMAKE_SYSROOT="$SYSROOT" \
        -DQT_BUILD_EXAMPLES=OFF \
        -DQT_BUILD_TESTS=OFF \
        -DFEATURE_hunspell=OFF \
        -GNinja

    ok "Configuration complete."

    if $CONFIGURE_ONLY; then
        echo ""
        echo "Configure-only mode. To build, run:"
        echo "  cd $QT_BUILD_DIR && cmake --build . --parallel $JOBS"
        exit 0
    fi

    # --- Build ---
    step "Building Qt ${QT_VERSION} for ARM64 ($JOBS parallel jobs)"
    echo "  This will take 1-2 hours..."
    cmake --build . --parallel "$JOBS"
    ok "Build complete."
fi

# --- Install ---
step "Installing Qt ${QT_VERSION} ARM64 to $QT_INSTALL_PREFIX"
cd "$QT_BUILD_DIR"
sudo cmake --install .
ok "Qt ${QT_VERSION} ARM64 installed to: $QT_INSTALL_PREFIX"

# --- Summary ---
echo ""
echo -e "${GREEN}============================================${NC}"
echo -e "${GREEN} Qt ${QT_VERSION} ARM64 ready!${NC}"
echo -e "${GREEN}============================================${NC}"
echo ""
echo "  Installation: $QT_INSTALL_PREFIX"
echo "  qmake:        $QT_INSTALL_PREFIX/bin/qmake (if available)"
echo "  qt-cmake:     $QT_INSTALL_PREFIX/bin/qt-cmake"
echo ""
echo "  To cross-compile your project:"
echo "    $QT_INSTALL_PREFIX/bin/qt-cmake ~/Documentos/latency_tester/LatencyTester"
echo "    cmake --build . --parallel $JOBS"
echo ""
echo "  Or update build-arm64.sh to use:"
echo "    --qmake $QT_INSTALL_PREFIX/bin/qmake"
