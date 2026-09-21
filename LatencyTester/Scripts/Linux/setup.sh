#!/bin/bash
# ============================================================================
# LatencyTester - Setup Script (Linux x64)
# ============================================================================
# Verifies that all required tools are installed and configured for building
# the project on Linux (Desktop debug/release and ARM cross-compilation).
#
# Usage: chmod +x setup.sh && ./setup.sh
# ============================================================================

set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
GRAY='\033[0;37m'
NC='\033[0m'

echo -e "${CYAN}============================================${NC}"
echo -e "${CYAN} LatencyTester - Environment Setup (Linux) ${NC}"
echo -e "${CYAN}============================================${NC}"
echo ""

ALL_OK=true

# --- 1. Check Git ---
echo -n "[1/7] Checking Git..."
if command -v git &> /dev/null; then
    GIT_VERSION=$(git --version | sed 's/git version //')
    echo -e " ${GREEN}OK ($GIT_VERSION)${NC}"
else
    echo -e " ${RED}NOT FOUND${NC}"
    echo "       Install: sudo apt install git"
    ALL_OK=false
fi

# --- 2. Check Qt (qmake) ---
echo -n "[2/7] Checking Qt (qmake)..."
if command -v qmake &> /dev/null; then
    QT_VERSION=$(qmake -query QT_VERSION 2>/dev/null || echo "unknown")
    echo -e " ${GREEN}OK (Qt $QT_VERSION)${NC}"
elif command -v qmake6 &> /dev/null; then
    QT_VERSION=$(qmake6 -query QT_VERSION 2>/dev/null || echo "unknown")
    echo -e " ${GREEN}OK (Qt $QT_VERSION) [as qmake6]${NC}"
else
    echo -e " ${RED}NOT FOUND${NC}"
    echo "       Install Qt 6.x from: https://www.qt.io/download-qt-installer-oss"
    echo "       Or via package manager: sudo apt install qt6-base-dev qt6-tools-dev"
    ALL_OK=false
fi

# --- 3. Check g++ ---
echo -n "[3/7] Checking g++..."
if command -v g++ &> /dev/null; then
    GPP_VERSION=$(g++ --version | head -1 | sed 's/.*) //')
    echo -e " ${GREEN}OK ($GPP_VERSION)${NC}"
else
    echo -e " ${RED}NOT FOUND${NC}"
    echo "       Install: sudo apt install g++ build-essential"
    ALL_OK=false
fi

# --- 4. Check Make ---
echo -n "[4/7] Checking Make..."
if command -v make &> /dev/null; then
    MAKE_VERSION=$(make --version | head -1)
    echo -e " ${GREEN}OK${NC}"
else
    echo -e " ${RED}NOT FOUND${NC}"
    echo "       Install: sudo apt install make build-essential"
    ALL_OK=false
fi

# --- 5. Check ARM Toolchain (optional) ---
echo -n "[5/7] Checking ARM Toolchain (optional)..."
if command -v aarch64-linux-gnu-g++ &> /dev/null; then
    ARM_VERSION=$(aarch64-linux-gnu-g++ --version | head -1)
    echo -e " ${GREEN}OK${NC}"
    echo -e "       ${GRAY}$ARM_VERSION${NC}"
elif command -v arm-linux-gnueabihf-g++ &> /dev/null; then
    ARM_VERSION=$(arm-linux-gnueabihf-g++ --version | head -1)
    echo -e " ${GREEN}OK (32-bit)${NC}"
    echo -e "       ${GRAY}$ARM_VERSION${NC}"
else
    echo -e " ${YELLOW}NOT INSTALLED (optional)${NC}"
    echo "       Only needed for Raspberry Pi cross-compilation"
    echo "       Install: sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu"
fi

# --- 6. Check pigpio dev headers (optional, for native RPi build) ---
echo -n "[6/7] Checking pigpio (optional)..."
if [ -f /usr/local/include/pigpio.h ] || [ -f /usr/include/pigpio.h ]; then
    echo -e " ${GREEN}OK${NC}"
elif dpkg -l libpigpio-dev &> /dev/null 2>&1; then
    echo -e " ${GREEN}OK (via apt)${NC}"
else
    echo -e " ${YELLOW}NOT INSTALLED (optional)${NC}"
    echo "       Only needed when compiling natively on Raspberry Pi"
    echo "       Install on RPi: sudo apt install libpigpio-dev"
fi

# --- 7. Check Git Submodules ---
echo -n "[7/7] Checking Git submodules..."
SUBMODULE_STATUS=$(git submodule status 2>/dev/null || echo "")
if echo "$SUBMODULE_STATUS" | grep -q "^-"; then
    echo -e " ${YELLOW}NOT INITIALIZED${NC}"
    echo "       Running: git submodule update --init --recursive"
    git submodule update --init --recursive
    echo -e "       ${GREEN}Done.${NC}"
elif [ -n "$SUBMODULE_STATUS" ]; then
    echo -e " ${GREEN}OK${NC}"
else
    echo -e " ${GREEN}OK${NC}"
fi

# --- Summary ---
echo ""
echo -e "${CYAN}============================================${NC}"
if [ "$ALL_OK" = true ]; then
    echo -e " ${GREEN}All required tools found!${NC}"
    echo -e " ${GREEN}You can now build the project:${NC}"
    echo ""
    echo -e "   cd LatencyTester"
    echo -e "   qmake LatencyTester.pro"
    echo -e "   make -j\$(nproc)"
    echo ""
    echo -e " Or open LatencyTester.pro in Qt Creator."
else
    echo -e " ${RED}Some tools are missing. Install them and run again.${NC}"
fi
echo -e "${CYAN}============================================${NC}"
