#!/usr/bin/env bash
# =============================================================================
# Build script for LatencyTester (Linux Desktop).
# Compiles the Qt application and generates Doxygen documentation.
#
# Usage:
#   ./build.sh              Build + generate docs
#   ./build.sh --clean      Remove build artifacts and docs
#   ./build.sh --docs-only  Generate documentation only
#   ./build.sh --no-docs    Build without generating docs
#   ./build.sh --jobs 4     Set parallel compilation jobs (default: nproc)
# =============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SCRIPTS_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
PROJECT_DIR="$(cd "$SCRIPTS_DIR/.." && pwd)"
REPO_ROOT="$(cd "$PROJECT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build/Desktop_Release"
DOCS_DIR="$PROJECT_DIR/Documentation"
DOXYFILE="$SCRIPTS_DIR/Doxyfile"

CLEAN=false
DOCS_ONLY=false
NO_DOCS=false
JOBS=$(nproc 2>/dev/null || echo 4)

# --- Parse arguments ---
while [[ $# -gt 0 ]]; do
    case "$1" in
        --clean)   CLEAN=true; shift ;;
        --docs-only) DOCS_ONLY=true; shift ;;
        --no-docs) NO_DOCS=true; shift ;;
        --jobs)    JOBS="$2"; shift 2 ;;
        -j*)       JOBS="${1#-j}"; shift ;;
        *)         echo "Unknown argument: $1"; exit 1 ;;
    esac
done

step() { echo -e "\n\033[36m>> $1\033[0m"; }
ok()   { echo -e "  \033[32m$1\033[0m"; }
warn() { echo -e "  \033[33mWARNING: $1\033[0m"; }
err()  { echo -e "  \033[31mERROR: $1\033[0m" >&2; exit 1; }

# --- Clean ---
if $CLEAN; then
    step "Cleaning build artifacts"
    rm -rf "$BUILD_DIR" "$DOCS_DIR"
    echo "  Clean complete."
    if ! $DOCS_ONLY; then exit 0; fi
fi

# --- Build application ---
if ! $DOCS_ONLY; then
    step "Checking prerequisites"

    command -v qmake >/dev/null 2>&1 || err "qmake not found in PATH. Ensure Qt 6.11+ is installed."
    echo "  qmake: $(which qmake)"

    command -v make >/dev/null 2>&1 || err "make not found in PATH."
    echo "  make: $(which make)"

    step "Creating build directory"
    mkdir -p "$BUILD_DIR"

    step "Running qmake (Release)"
    cd "$BUILD_DIR"
    qmake "$PROJECT_DIR/LatencyTester.pro" "CONFIG+=release" "CONFIG-=debug"

    step "Compiling ($JOBS parallel jobs)"
    make -j"$JOBS"

    if [[ -f "$BUILD_DIR/LatencyTester" ]]; then
        ok "Build successful: $BUILD_DIR/LatencyTester"
    else
        warn "Build completed but binary not found at expected location."
    fi

    cd "$REPO_ROOT"
fi

# --- Generate documentation ---
if ! $NO_DOCS; then
    step "Generating Doxygen documentation"

    if ! command -v doxygen >/dev/null 2>&1; then
        warn "Doxygen not found in PATH. Skipping documentation generation."
        echo "  Install: sudo apt install doxygen graphviz"
    else
        echo "  doxygen: $(which doxygen)"
        mkdir -p "$DOCS_DIR"

        if [[ ! -f "$DOXYFILE" ]]; then
            err "Doxyfile not found at $DOXYFILE"
        fi

        cd "$PROJECT_DIR"
        doxygen "$DOXYFILE"
        ok "Documentation generated: $DOCS_DIR"
    fi
fi

echo -e "\n\033[32m>> Done.\033[0m"
