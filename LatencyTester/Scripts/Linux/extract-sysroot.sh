#!/usr/bin/env bash
# =============================================================================
# Extract ARM64 sysroot from the Docker container for cross-compilation.
#
# This script extracts all necessary libraries, headers, and Qt files from the
# latencytester-arm64 Docker container into a local sysroot directory.
#
# Usage:
#   ./extract-sysroot.sh                    Extract to ~/ARM64-sysroot
#   ./extract-sysroot.sh /custom/path       Extract to custom path
#   ./extract-sysroot.sh --refresh          Delete and re-extract
# =============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IMAGE_NAME="latencytester-arm64"
CONTAINER_NAME="sysroot-extractor"
SYSROOT="${1:-$HOME/ARM64-sysroot}"
REFRESH=false

# Colors
CYAN='\033[0;36m'
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

step() { echo -e "${CYAN}>> $1${NC}"; }
ok()   { echo -e "${GREEN}   $1${NC}"; }
err()  { echo -e "${RED}ERROR: $1${NC}" >&2; exit 1; }

# Parse --refresh flag
for arg in "$@"; do
    if [[ "$arg" == "--refresh" ]]; then
        REFRESH=true
        # Use default path if only --refresh was passed
        if [[ "$1" == "--refresh" ]]; then
            SYSROOT="$HOME/ARM64-sysroot"
        fi
    fi
done

# --- Refresh ---
if $REFRESH && [[ -d "$SYSROOT" ]]; then
    step "Removing existing sysroot at $SYSROOT"
    rm -rf "$SYSROOT"
fi

# --- Check if already extracted ---
if [[ -d "$SYSROOT/usr/lib/aarch64-linux-gnu" && -d "$SYSROOT/usr/include" ]]; then
    ok "Sysroot already exists at $SYSROOT"
    echo "  Use --refresh to force re-extraction."
    exit 0
fi

# --- Verify Docker image exists ---
if ! docker image inspect "$IMAGE_NAME" &>/dev/null; then
    step "Docker image '$IMAGE_NAME' not found. Building it..."
    docker build --platform linux/arm64 -t "$IMAGE_NAME" "$SCRIPT_DIR"
fi

# --- Start a temporary container ---
step "Starting temporary container for extraction..."
docker rm -f "$CONTAINER_NAME" 2>/dev/null || true
docker run --platform linux/arm64 --name "$CONTAINER_NAME" -d "$IMAGE_NAME" sleep infinity

# --- Create sysroot directory structure ---
step "Extracting sysroot to $SYSROOT"
mkdir -p "$SYSROOT/usr/lib"
mkdir -p "$SYSROOT/usr/include"
mkdir -p "$SYSROOT/lib"

# --- Copy libraries ---
step "Copying ARM64 libraries..."
docker cp "$CONTAINER_NAME:/usr/lib/aarch64-linux-gnu" "$SYSROOT/usr/lib/aarch64-linux-gnu"
docker cp "$CONTAINER_NAME:/lib/aarch64-linux-gnu" "$SYSROOT/lib/aarch64-linux-gnu"

# --- Copy headers ---
step "Copying system headers..."
docker cp "$CONTAINER_NAME:/usr/include/." "$SYSROOT/usr/include/"

# --- Copy Qt6 specific files (mkspecs, cmake configs) ---
step "Copying Qt6 files..."
docker cp "$CONTAINER_NAME:/usr/lib/aarch64-linux-gnu/qt6" "$SYSROOT/usr/lib/aarch64-linux-gnu/qt6" 2>/dev/null || true

# --- Symlinks for ld (some libs expect /usr/lib/ paths) ---
step "Creating compatibility symlinks..."
mkdir -p "$SYSROOT/usr/lib/aarch64-linux-gnu"
# Link the dynamic linker path
ln -sf "$SYSROOT/lib/aarch64-linux-gnu" "$SYSROOT/lib/aarch64-linux-gnu" 2>/dev/null || true

# --- Cleanup ---
step "Removing temporary container..."
docker rm -f "$CONTAINER_NAME" >/dev/null

# --- Done ---
ok "Sysroot extracted successfully to: $SYSROOT"
echo ""
echo "  Contents:"
echo "    $SYSROOT/usr/lib/aarch64-linux-gnu/  (shared libraries)"
echo "    $SYSROOT/usr/include/                (headers including GL/EGL)"
echo "    $SYSROOT/lib/aarch64-linux-gnu/      (dynamic linker + base libs)"
echo ""
echo "  To cross-compile:"
echo "    ./build-arm64.sh --qmake ~/Qt/6.11.1/gcc_64/bin/qmake --sysroot $SYSROOT"
