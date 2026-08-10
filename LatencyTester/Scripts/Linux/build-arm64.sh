#!/usr/bin/env bash
# =============================================================================
# Cross-compile LatencyTester for ARM64 (Raspberry Pi)
#
# Prerequisites:
#   sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
#   Sysroot extracted from Docker container at ~/ARM64-sysroot
#
# Usage:
#   ./build-arm64.sh              Build for ARM64
#   ./build-arm64.sh --clean      Remove ARM64 build directory
#   ./build-arm64.sh --sysroot /path/to/sysroot   Custom sysroot path
# =============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build_arm64"
SYSROOT="${HOME}/ARM64-sysroot"
JOBS=$(nproc 2>/dev/null || echo 4)
QMAKE_PATH=""

# Colors
CYAN='\033[0;36m'
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

step() { echo -e "${CYAN}>> $1${NC}"; }
ok()   { echo -e "${GREEN}   $1${NC}"; }
err()  { echo -e "${RED}ERROR: $1${NC}" >&2; exit 1; }

# --- Parse arguments ---
CLEAN=false
while [[ $# -gt 0 ]]; do
    case "$1" in
        --clean)   CLEAN=true; shift ;;
        --sysroot) SYSROOT="$2"; shift 2 ;;
        --jobs|-j) JOBS="$2"; shift 2 ;;
        --qmake)   QMAKE_PATH="$2"; shift 2 ;;
        *)         echo "Unknown argument: $1"; exit 1 ;;
    esac
done

# --- Clean ---
if $CLEAN; then
    step "Cleaning ARM64 build directory"
    rm -rf "$BUILD_DIR"
    ok "Clean complete."
    exit 0
fi

# --- Verify prerequisites ---
step "Checking prerequisites"

command -v aarch64-linux-gnu-g++-13 >/dev/null 2>&1 || \
    err "aarch64-linux-gnu-g++-13 not found. Install with: sudo apt install g++-13-aarch64-linux-gnu"
echo "  Cross-compiler: $(aarch64-linux-gnu-g++-13 --version | head -1)"

# Resolve qmake: explicit path > Qt Online Installer > system qmake6 > system qmake
if [[ -n "$QMAKE_PATH" ]]; then
    QMAKE="$QMAKE_PATH"
    [[ -x "$QMAKE" ]] || err "qmake not found at $QMAKE"
else
    # Search for Qt Online Installer qmake (common paths)
    QT_INSTALLER_QMAKE=""
    for candidate in \
        "/opt/Qt/6.11.1/arm64/bin/qmake" \
        "/opt/Qt/6.*/arm64/bin/qmake" \
        "$HOME/Qt/6.11.1/gcc_64/bin/qmake" \
        "$HOME/Qt/6.*/gcc_64/bin/qmake" \
        "/opt/Qt/6.*/gcc_64/bin/qmake"; do
        # shellcheck disable=SC2086
        for match in $candidate; do
            if [[ -x "$match" ]]; then
                QT_INSTALLER_QMAKE="$match"
                break 2
            fi
        done
    done

    if [[ -n "$QT_INSTALLER_QMAKE" ]]; then
        QMAKE="$QT_INSTALLER_QMAKE"
    elif command -v qmake6 >/dev/null 2>&1; then
        QMAKE=qmake6
    elif command -v qmake >/dev/null 2>&1; then
        QMAKE=qmake
    else
        err "No qmake found. Install Qt via Online Installer or: sudo apt install qt6-base-dev-tools"
    fi
fi
echo "  qmake: $QMAKE"

if [[ ! -d "$SYSROOT/usr/lib/aarch64-linux-gnu" ]]; then
    err "Sysroot not found at $SYSROOT. Extract it from Docker with:\n  docker cp latencytester-rpi:/usr/lib/aarch64-linux-gnu $SYSROOT/usr/lib/aarch64-linux-gnu\n  docker cp latencytester-rpi:/usr/include $SYSROOT/usr/include\n  docker cp latencytester-rpi:/lib/aarch64-linux-gnu $SYSROOT/lib/aarch64-linux-gnu"
fi
echo "  Sysroot: $SYSROOT"

# --- Build ---
step "Creating build directory"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

step "Running $QMAKE (ARM64)"
$QMAKE "$PROJECT_DIR/LatencyTester.pro" -spec linux-aarch64-gnu-g++ \
    "QMAKE_CC=aarch64-linux-gnu-gcc-13" \
    "QMAKE_CXX=aarch64-linux-gnu-g++-13" \
    "QMAKE_LINK=aarch64-linux-gnu-g++-13"

step "Compiling ($JOBS parallel jobs)"
make -j"$JOBS"

# --- Result ---
BINARY="$BUILD_DIR/LatencyTester"
if [[ -f "$BINARY" ]]; then
    ok "Build successful: $BINARY"
    echo ""
    file "$BINARY"
    echo ""
    echo "To run in Docker:"
    echo "  ./docker-rpi.sh run"
    echo "  cd /app/build_arm64 && ./LatencyTester"
else
    err "Build completed but binary not found."
fi
