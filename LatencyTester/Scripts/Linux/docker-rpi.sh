#!/usr/bin/env bash
# =============================================================================
# Docker ARM64 RPi emulation manager for LatencyTester
#
# Usage:
#   ./docker-rpi.sh setup      Install Docker + enable ARM64 emulation
#   ./docker-rpi.sh build      Build the ARM64 container image
#   ./docker-rpi.sh run        Start the container with VNC (port 5900)
#   ./docker-rpi.sh valgrind   Run Valgrind memory check (offscreen)
#   ./docker-rpi.sh shell      Open a bash shell inside the container
#   ./docker-rpi.sh stop       Stop the running container
#   ./docker-rpi.sh clean      Remove container and image
# =============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
IMAGE_NAME="latencytester-arm64"
CONTAINER_NAME="latencytester-rpi"
VNC_PORT=5900

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
NC='\033[0m'

step()  { echo -e "${CYAN}>> $1${NC}"; }
ok()    { echo -e "${GREEN}   $1${NC}"; }
err()   { echo -e "${RED}ERROR: $1${NC}" >&2; exit 1; }

# --- SETUP ---
cmd_setup() {
    step "Installing Docker..."
    if command -v docker &>/dev/null; then
        ok "Docker already installed: $(docker --version)"
    else
        curl -fsSL https://get.docker.com | sh
        sudo usermod -aG docker "$USER"
        ok "Docker installed. Log out and back in for group changes to take effect."
    fi

    step "Enabling ARM64 emulation (qemu-user-static)..."
    docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
    ok "ARM64 emulation enabled."
}

# --- BUILD ---
cmd_build() {
    step "Building ARM64 Docker image: $IMAGE_NAME"
    docker build --platform linux/arm64 -t "$IMAGE_NAME" "$SCRIPT_DIR"
    ok "Image built successfully."
}

# --- RUN ---
cmd_run() {
    # Stop existing container if running
    if docker ps -q -f name="$CONTAINER_NAME" | grep -q .; then
        step "Stopping existing container..."
        docker stop "$CONTAINER_NAME" >/dev/null
    fi
    # Remove old container
    docker rm -f "$CONTAINER_NAME" 2>/dev/null || true

    step "Starting container with VNC on port $VNC_PORT..."
    docker run --platform linux/arm64 \
        --name "$CONTAINER_NAME" \
        -p ${VNC_PORT}:5900 \
        -v "$PROJECT_DIR:/app" \
        -v "/opt/Qt/6.11.1/arm64:/opt/Qt/6.11.1/arm64:ro" \
        -e "LD_LIBRARY_PATH=/opt/Qt/6.11.1/arm64/lib" \
        -it "$IMAGE_NAME"

    # Note: the entrypoint starts Xvfb + x11vnc automatically.
    # Connect your VNC client to localhost:5900
}

# --- VALGRIND ---
cmd_valgrind() {
    if ! docker ps -q -f name="$CONTAINER_NAME" | grep -q .; then
        # Start a temporary container for valgrind
        step "Starting temporary container for Valgrind..."
        docker run --platform linux/arm64 \
            --rm \
            -v "$PROJECT_DIR:/app" \
            -v "/opt/Qt/6.11.1/arm64:/opt/Qt/6.11.1/arm64:ro" \
            -e "LD_LIBRARY_PATH=/opt/Qt/6.11.1/arm64/lib" \
            "$IMAGE_NAME" \
            bash -c "cd /app/build_arm64 && valgrind --leak-check=full --show-leak-kinds=definite ./LatencyTester -platform offscreen"
    else
        # Exec valgrind in the running container
        step "Running Valgrind in existing container..."
        docker exec "$CONTAINER_NAME" \
            bash -c "cd /app/build_arm64 && LD_LIBRARY_PATH=/opt/Qt/6.11.1/arm64/lib valgrind --leak-check=full --show-leak-kinds=definite ./LatencyTester -platform offscreen"
    fi
}

# --- TESTS ---
cmd_tests() {
    if ! docker ps -q -f name="$CONTAINER_NAME" | grep -q .; then
        step "Starting temporary container for tests..."
        docker run --platform linux/arm64 \
            --rm \
            -v "$PROJECT_DIR:/app" \
            -v "/opt/Qt/6.11.1/arm64:/opt/Qt/6.11.1/arm64:ro" \
            -e "LD_LIBRARY_PATH=/opt/Qt/6.11.1/arm64/lib" \
            "$IMAGE_NAME" \
            bash -c "cd /app/build_arm64 && valgrind --leak-check=full --show-leak-kinds=definite ./LatencyTesterTests -platform offscreen"
    else
        step "Running tests with Valgrind in existing container..."
        docker exec "$CONTAINER_NAME" \
            bash -c "cd /app/build_arm64 && LD_LIBRARY_PATH=/opt/Qt/6.11.1/arm64/lib valgrind --leak-check=full --show-leak-kinds=definite ./LatencyTesterTests -platform offscreen"
    fi
}

# --- SHELL ---
cmd_shell() {
    if docker ps -q -f name="$CONTAINER_NAME" | grep -q .; then
        step "Attaching to running container..."
        docker exec -it "$CONTAINER_NAME" bash
    else
        step "Starting new shell container..."
        docker run --platform linux/arm64 \
            --rm \
            -v "$PROJECT_DIR:/app" \
            -it "$IMAGE_NAME" bash
    fi
}

# --- STOP ---
cmd_stop() {
    step "Stopping container..."
    docker stop "$CONTAINER_NAME" 2>/dev/null && ok "Stopped." || echo "  Not running."
}

# --- CLEAN ---
cmd_clean() {
    step "Removing container and image..."
    docker rm -f "$CONTAINER_NAME" 2>/dev/null || true
    docker rmi "$IMAGE_NAME" 2>/dev/null || true
    ok "Cleaned."
}

# --- MAIN ---
case "${1:-help}" in
    setup)    cmd_setup ;;
    build)    cmd_build ;;
    run)      cmd_run ;;
    valgrind) cmd_valgrind ;;
    tests)    cmd_tests ;;
    shell)    cmd_shell ;;
    stop)     cmd_stop ;;
    clean)    cmd_clean ;;
    *)
        echo "LatencyTester Docker ARM64 Manager"
        echo ""
        echo "Usage: $0 <command>"
        echo ""
        echo "Commands:"
        echo "  setup      Install Docker + enable ARM64 emulation"
        echo "  build      Build the ARM64 container image"
        echo "  run        Start container with VNC (connect to localhost:$VNC_PORT)"
        echo "  valgrind   Run Valgrind memory analysis on the app (offscreen)"
        echo "  tests      Run unit tests with Valgrind (offscreen)"
        echo "  shell      Open a bash shell inside the container"
        echo "  stop       Stop the running container"
        echo "  clean      Remove container and image"
        exit 1
        ;;
esac
