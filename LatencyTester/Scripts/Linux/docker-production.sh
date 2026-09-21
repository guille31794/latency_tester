#!/usr/bin/env bash
# =============================================================================
# Docker Production Build Manager
#
# Builds the ENTIRE project (Qt + libraries + LatencyTester) inside a Docker
# container. Fully portable — works on any machine with Docker (Linux, Windows
# WSL2, macOS). No pre-installed Qt or cross-compilers needed on the host.
#
# Usage:
#   ./docker-production.sh build      Build the image (~2-3h first time)
#   ./docker-production.sh extract    Extract production binary to ./output/
#   ./docker-production.sh tests      Run unit tests (ARM64 emulated)
#   ./docker-production.sh valgrind   Run Valgrind memory check
#   ./docker-production.sh vnc        Launch with VNC on port 5900
#   ./docker-production.sh shell      Interactive shell
#   ./docker-production.sh clean      Remove image
# =============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/../../.." && pwd)"
IMAGE_NAME="latencytester-production"
DOCKERFILE="$SCRIPT_DIR/Dockerfile.production"

# Colors
CYAN='\033[0;36m'
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

step()  { echo -e "${CYAN}>> $1${NC}"; }
ok()    { echo -e "${GREEN}   $1${NC}"; }
err()   { echo -e "${RED}ERROR: $1${NC}" >&2; exit 1; }

# --- BUILD ---
cmd_build() {
    step "Building production image (this takes 2-3 hours on first run)..."
    
    # Ensure submodules are initialized
    if [[ ! -f "$PROJECT_DIR/LatencyTester/Libs/rpi_ads1115/ads1115rpi.cpp" ]]; then
        step "Initializing git submodules..."
        cd "$PROJECT_DIR" && git submodule update --init --recursive
    fi
    
    # Ensure ARM64 emulation is available
    if ! docker run --rm --platform linux/arm64 arm64v8/ubuntu:24.04 uname -m &>/dev/null 2>&1; then
        step "Enabling ARM64 emulation..."
        docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
    fi
    
    docker build \
        -f "$DOCKERFILE" \
        -t "$IMAGE_NAME" \
        "$PROJECT_DIR"
    
    ok "Image built: $IMAGE_NAME"
    echo ""
    echo "  Extract binary:  $0 extract"
    echo "  Run tests:       $0 tests"
    echo "  Run VNC:         $0 vnc"
}

# --- EXTRACT ---
cmd_extract() {
    step "Extracting production binary..."
    mkdir -p "$PROJECT_DIR/output"
    
    docker run --rm "$IMAGE_NAME" cat /app/LatencyTester > "$PROJECT_DIR/output/LatencyTester"
    chmod +x "$PROJECT_DIR/output/LatencyTester"
    
    docker run --rm "$IMAGE_NAME" cat /app/LatencyTesterTests > "$PROJECT_DIR/output/LatencyTesterTests"
    chmod +x "$PROJECT_DIR/output/LatencyTesterTests"
    
    ok "Binaries extracted to: $PROJECT_DIR/output/"
    echo ""
    file "$PROJECT_DIR/output/LatencyTester"
    echo ""
    echo "  Deploy: scp output/LatencyTester pi@<ip>:/home/pi/LatencyTester/bin/"
}

# --- TESTS ---
cmd_tests() {
    step "Running unit tests (ARM64 emulated)..."
    docker run --rm --platform linux/arm64 "$IMAGE_NAME" /run-tests.sh
}

# --- VALGRIND ---
cmd_valgrind() {
    step "Running Valgrind (ARM64 emulated)..."
    docker run --rm --platform linux/arm64 "$IMAGE_NAME" /run-valgrind.sh
}

# --- VNC ---
cmd_vnc() {
    step "Launching with VNC on port 5900..."
    docker run --rm --platform linux/arm64 \
        -p 5900:5900 \
        -it "$IMAGE_NAME" /run-vnc.sh
}

# --- SHELL ---
cmd_shell() {
    step "Starting interactive shell..."
    docker run --rm --platform linux/arm64 \
        -it "$IMAGE_NAME" bash
}

# --- CLEAN ---
cmd_clean() {
    step "Removing image..."
    docker rmi "$IMAGE_NAME" 2>/dev/null && ok "Removed." || echo "  Not found."
}

# --- MAIN ---
case "${1:-help}" in
    build)    cmd_build ;;
    extract)  cmd_extract ;;
    tests)    cmd_tests ;;
    valgrind) cmd_valgrind ;;
    vnc)      cmd_vnc ;;
    shell)    cmd_shell ;;
    clean)    cmd_clean ;;
    *)
        echo "LatencyTester Docker Production Builder"
        echo ""
        echo "Builds the ENTIRE project inside Docker — fully portable."
        echo "No Qt installation or cross-compilers needed on the host."
        echo ""
        echo "Usage: $0 <command>"
        echo ""
        echo "Commands:"
        echo "  build      Build the production image (~2-3h first time)"
        echo "  extract    Extract ARM64 binaries to ./output/"
        echo "  tests      Run unit tests (ARM64 emulated)"
        echo "  valgrind   Run Valgrind memory analysis"
        echo "  vnc        Launch app with VNC (localhost:5900)"
        echo "  shell      Interactive shell inside the container"
        echo "  clean      Remove the Docker image"
        exit 1
        ;;
esac
