# Toolchain Setup — ARM64 Cross-Compilation & Docker Testing

This document describes the complete toolchain for compiling LatencyTester targeting ARM64 (Raspberry Pi 3B) and testing it using Docker-based emulation. It reflects the final proven workflow after extensive iteration.

---

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Prerequisites (Linux Host)](#prerequisites-linux-host)
3. [Step 1: Build the Docker Image](#step-1-build-the-docker-image)
4. [Step 2: Extract the Sysroot](#step-2-extract-the-sysroot)
5. [Step 3: Compile Qt 6.11.1 for ARM64 (One-Time)](#step-3-compile-qt-6111-for-arm64-one-time)
6. [Step 4: Cross-Compile LatencyTester](#step-4-cross-compile-latencytester)
7. [Step 5: Run & Test in Docker](#step-5-run--test-in-docker)
8. [USE_STUBS Mechanism](#use_stubs-mechanism)
9. [Docker Container Reference](#docker-container-reference)
10. [Troubleshooting](#troubleshooting)
11. [Known Limitations & Future Work](#known-limitations--future-work)
12. [Toolchain Configuration — Biggest Challenges](#toolchain-configuration--biggest-challenges)

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│ Linux Host (Ubuntu 24.04 x86_64)                                │
│                                                                 │
│  ┌──────────────┐    ┌──────────────────┐    ┌───────────────┐ │
│  │ GCC 13       │    │ Qt 6.11.1 ARM64  │    │ Sysroot       │ │
│  │ aarch64-     │    │ /opt/Qt/6.11.1/  │    │ ~/ARM64-      │ │
│  │ linux-gnu-   │    │ arm64/           │    │ sysroot/      │ │
│  │ g++-13       │    │ (compiled from   │    │ (extracted    │ │
│  │              │    │  source)         │    │  from Docker) │ │
│  └──────┬───────┘    └────────┬─────────┘    └───────┬───────┘ │
│         │                     │                      │         │
│         └─────────────────────┼──────────────────────┘         │
│                               ▼                                 │
│                    ┌──────────────────────┐                     │
│                    │  build-arm64.sh      │                     │
│                    │  (qmake + make)      │                     │
│                    └──────────┬───────────┘                     │
│                               │                                 │
│                               ▼                                 │
│                    build_arm64/LatencyTester (ARM64 ELF)         │
│                               │                                 │
│                               ▼                                 │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │ Docker Container (arm64v8/ubuntu:24.04 via QEMU)          │ │
│  │   - Xvfb (800x480) + x11vnc (port 5900)                  │ │
│  │   - Qt 6.11.1 ARM64 mounted at /opt/Qt/6.11.1/arm64:ro   │ │
│  │   - Project mounted at /app                               │ │
│  │   - Valgrind for memory analysis                          │ │
│  └────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

**Key design decisions:**
- Cross-compilation on host (qmake under QEMU user-mode fails to parse compiler macros)
- lld linker (avoids GLIBC version mismatch between host ld.bfd and sysroot)
- GCC 13 cross-compiler (required for `<format>` header, C++20 features)
- Docker container used only for runtime execution and testing
- Volume mounts exclusively (no docker cp needed)
- Ubuntu 24.04 base (glibc 2.39, matching host)

---

## Prerequisites (Linux Host)

| Component | Version | Install Command |
|-----------|---------|-----------------|
| Ubuntu | 24.04 LTS | — |
| Docker | Latest | `curl -fsSL https://get.docker.com \| sh` |
| docker-buildx-plugin | Latest | `sudo apt install docker-buildx-plugin` |
| binfmt-support | Latest | `sudo apt install binfmt-support qemu-user-static` |
| GCC 13 Cross-Compiler | 13.x | `sudo apt install gcc-13-aarch64-linux-gnu g++-13-aarch64-linux-gnu` |
| lld | Latest | `sudo apt install lld` |
| CMake | ≥3.16 | `sudo apt install cmake` |
| Ninja | ≥1.10 | `sudo apt install ninja-build` |
| Qt 6.11.1 (host) | 6.11.1 | Qt Online Installer → `~/Qt/6.11.1/gcc_64` |
| Qt 6.11.1 (ARM64) | 6.11.1 | Compiled from source → `/opt/Qt/6.11.1/arm64/` |

Enable permanent ARM64 emulation (survives reboot):

```bash
sudo apt install binfmt-support qemu-user-static
# Verify: file /usr/bin/qemu-aarch64-static should exist
# Docker automatically uses this for --platform linux/arm64
```

---

## Step 1: Build the Docker Image

The Docker image provides the ARM64 runtime environment (Qt 6 libs, Xvfb, VNC, Valgrind).

```bash
cd LatencyTester/Scripts/Linux
./docker-rpi.sh build
```

This builds an `arm64v8/ubuntu:24.04` image with:
- Qt 6 runtime libraries (libqt6widgets6, libqt6concurrent6, etc.)
- Qt Virtual Keyboard plugin
- libgpiod-dev (for production builds in the future)
- Xvfb + x11vnc (800×480 virtual display)
- Valgrind (memory analysis)
- Development headers (for sysroot extraction)

---

## Step 2: Extract the Sysroot

The sysroot contains ARM64 libraries and headers extracted from the Docker image, needed by the cross-compiler to resolve dependencies.

```bash
cd LatencyTester/Scripts/Linux
./extract-sysroot.sh
```

**Default output:** `~/ARM64-sysroot/`

The script:
1. Starts a temporary container from the ARM64 image
2. Copies `/usr/lib/aarch64-linux-gnu/`, `/usr/include/`, `/lib/aarch64-linux-gnu/`
3. Creates `.so` symlinks for the linker (distro packages only ship versioned .so.X.Y.Z)
4. Downloads `pigpio.h` header (not in Ubuntu repos — RPi-specific)
5. Links the Qt ARM64 installation into the sysroot if present
6. Removes the temporary container

To refresh after updating the Docker image:

```bash
./extract-sysroot.sh --refresh
```

---

## Step 3: Compile Qt 6.11.1 for ARM64 (One-Time)

Qt must be compiled from source because:
- No prebuilt ARM64 Qt for cross-compilation is available
- The host Qt (gcc_64) provides build tools (moc, rcc, uic) but libraries target x86_64
- We need ARM64 libraries + a qmake that generates ARM64 Makefiles

```bash
cd LatencyTester/Scripts/Linux
./build-qt6-arm64.sh
```

**Duration:** ~1-2 hours, ~15 GB disk space  
**Output:** `/opt/Qt/6.11.1/arm64/`

### What the script does:

1. Generates a CMake toolchain file (`qt6-arm64-toolchain.cmake`) pointing to:
   - Cross-compiler: `aarch64-linux-gnu-gcc/g++`
   - Sysroot: `~/ARM64-sysroot`
   - Linker: lld (critical for avoiding GLIBC version issues)
2. Downloads Qt 6.11.1 source (~900 MB)
3. Configures with reduced submodules: `qtbase, qtdeclarative, qtvirtualkeyboard, qtsvg, qtshadertools`
4. Builds with Ninja (parallel)
5. Installs to `/opt/Qt/6.11.1/arm64/` (requires sudo)

### Partial build options:

```bash
./build-qt6-arm64.sh --skip-download    # Source already extracted
./build-qt6-arm64.sh --configure-only   # Only configure, don't build
./build-qt6-arm64.sh --install-only     # Install after successful build
```

### Critical configuration flags:

| Flag | Purpose |
|------|---------|
| `-qt-host-path ~/Qt/6.11.1/gcc_64` | Host tools (moc, rcc, uic) from Online Installer |
| `-DCMAKE_TOOLCHAIN_FILE=...` | Cross-compiler + sysroot paths |
| `-fuse-ld=lld` | Avoids GLIBC version mismatch with ld.bfd |
| `-submodules qtbase,...` | Only build what we need (saves time) |
| `-DFEATURE_hunspell=OFF` | Avoid hunspell linking issues |
| `-GNinja` | Fast parallel builds |

---

## Step 4: Cross-Compile LatencyTester

```bash
cd LatencyTester/Scripts/Linux
./build-arm64.sh
```

**Output:** `LatencyTester/build_arm64/LatencyTester` (ARM64 ELF binary) + `build_arm64/Tests/LatencyTesterTests`

### What the script does:

1. Verifies prerequisites (GCC 13 cross-compiler, qmake, sysroot)
2. Resolves qmake: `/opt/Qt/6.11.1/arm64/bin/qmake` → system qmake6 → system qmake
3. Runs qmake with:
   ```
   qmake LatencyTester.pro -spec linux-aarch64-gnu-g++ \
     "QMAKE_CC=aarch64-linux-gnu-gcc-13" \
     "QMAKE_CXX=aarch64-linux-gnu-g++-13" \
     "QMAKE_LINK=aarch64-linux-gnu-g++-13" \
     "USE_STUBS=1"
   ```
4. Compiles with `make -j$(nproc)`
5. Also compiles tests (`Tests/Tests.pro`) with `CONFIG+=no_sanitize` (ASan not for cross)
6. Verifies the output binary exists

### Options:

```bash
./build-arm64.sh --clean              # Remove build_arm64/ directory
./build-arm64.sh --sysroot /custom    # Custom sysroot path
./build-arm64.sh --qmake /path/qmake  # Explicit qmake path
./build-arm64.sh -j 8                 # Override parallel jobs
```

---

## Step 5: Run & Test in Docker

### Run with VNC (GUI testing)

```bash
./docker-rpi.sh run
# In the container shell:
cd /app/build_arm64 && ./LatencyTester
```

Connect a VNC client to `localhost:5900` (800×480, no password).

### Run unit tests

```bash
./docker-rpi.sh tests
```

Executes `build_arm64/Tests/LatencyTesterTests -platform offscreen` inside the container.

### Valgrind memory analysis

```bash
./docker-rpi.sh valgrind
```

Runs the app under Valgrind with `--leak-check=full --show-leak-kinds=definite`.

### Interactive shell

```bash
./docker-rpi.sh shell
```

---

## USE_STUBS Mechanism

The `.pro` file supports three build modes controlled by target architecture and the `USE_STUBS` variable:

| Mode | Triggered by | RASPBERRY_PI defined | GPIO/ADC drivers | Use case |
|------|-------------|---------------------|-----------------|----------|
| Desktop | x86_64 host, no flags | ❌ | Stubs | Local development (Windows/Linux) |
| ARM64 Stubs | ARM target + `USE_STUBS=1` | ❌ | Stubs | Docker testing, CI |
| ARM64 Production | ARM target + `USE_STUBS=0` (default) | ✅ | pigpio + libgpiod | Real Raspberry Pi deployment |

### How it works in `LatencyTester.pro`:

```qmake
contains(QMAKE_HOST.arch, arm.*) | contains(QT_ARCH, arm.*) {
    equals(USE_STUBS, 1) {
        # Stubs mode: no RASPBERRY_PI define, uses stub headers
    } else {
        # Production: defines RASPBERRY_PI, links -lpigpio -lrt -lgpiod
    }
} else {
    # Desktop: always uses stubs
}
```

### In code (conditional includes):

```cpp
// Core/Helpers/ads1115.h
#ifdef RASPBERRY_PI
    #include "ads1115rpi.h"      // Real I2C driver
#else
    #include "ads1115rpi_stub.h" // Returns simulated values
#endif

// Core/SensorOperator/sensoroperator.cpp
#ifdef RASPBERRY_PI
    #include <pigpio.h>          // Real GPIO
#else
    #include "pigpio_stub.h"     // Noop stubs
#endif
```

---

## Docker Container Reference

### Commands (`docker-rpi.sh`)

| Command | Description |
|---------|-------------|
| `setup` | Install Docker + enable ARM64 emulation (one-time) |
| `build` | Build the ARM64 container image |
| `run` | Start container with VNC (port 5900), mounts project at `/app` |
| `tests` | Run unit tests (offscreen) |
| `valgrind` | Run Valgrind memory analysis (offscreen) |
| `shell` | Bash shell inside the container |
| `stop` | Stop the running container |
| `clean` | Remove container and image |

### Volume mounts (automatic)

| Host path | Container path | Mode |
|-----------|---------------|------|
| `LatencyTester/` (project root) | `/app` | read-write |
| `/opt/Qt/6.11.1/arm64/` | `/opt/Qt/6.11.1/arm64/` | read-only |

### Environment variables (set by script)

| Variable | Value | Purpose |
|----------|-------|---------|
| `LD_LIBRARY_PATH` | `/opt/Qt/6.11.1/arm64/lib` | Find Qt ARM64 .so at runtime |
| `DISPLAY` | `:0` | Xvfb virtual display |
| `QT_QPA_PLATFORM` | `vnc` | Qt VNC backend (fallback) |

---

## Troubleshooting

### qmake fails under QEMU with "Cannot determine architecture"

**Cause:** qmake runs g++ with macro parsing, which fails under QEMU user-mode.  
**Solution:** Always cross-compile on the host. Docker is only for execution.

### GLIBC version mismatch (undefined symbol GLIBC_2.XX)

**Cause:** Host's `ld.bfd` picks up system libc instead of sysroot's.  
**Solution:** Use lld linker (`-fuse-ld=lld` in the toolchain CMake file). This is already configured in `build-qt6-arm64.sh`.

### `<format>` header not found

**Cause:** GCC 12 doesn't fully support `<format>`.  
**Solution:** Use GCC 13: `sudo apt install g++-13-aarch64-linux-gnu`. The build script explicitly uses `aarch64-linux-gnu-g++-13`.

### "exec format error" when running docker-entrypoint.sh

**Cause:** Windows line endings (CRLF) in the shell script.  
**Solution:** Ensure `docker-entrypoint.sh` has Unix line endings (LF only):
```bash
sed -i 's/\r$//' docker-entrypoint.sh
```

### ICU version mismatch

**Cause:** Host ICU and sysroot ICU are different versions.  
**Solution:** The Docker image (Ubuntu 24.04) and host (Ubuntu 24.04) share the same ICU. If using a different base, rebuild the sysroot.

### pigpio.h not found during cross-compilation

**Cause:** pigpio is RPi-specific and not in Ubuntu repos.  
**Solution:** `extract-sysroot.sh` downloads the header automatically from GitHub. For production builds, pigpio must be compiled from source or installed on the target RPi.

### Docker deprecated builder warning

**Cause:** Legacy Docker builder.  
**Solution:** `sudo apt install docker-buildx-plugin` (already in setup).

### Qt Virtual Keyboard module not found

**Cause:** qmake can't find the VK module.  
**Solution:** The Qt ARM64 build includes `qtvirtualkeyboard` submodule. Verify `/opt/Qt/6.11.1/arm64/lib/libQt6VirtualKeyboard.so` exists.

---

## Known Limitations & Future Work

### Current limitations

1. **Production builds (USE_STUBS=0) fully functional:**  
   - libgpiod v2.2 compiled from source with C++ bindings: `./build-libgpiod-arm64.sh`
   - pigpio compiled from source: `./build-pigpio-arm64.sh`
   - Production binary generated: `./build-arm64.sh --production`

2. **ASan not available in cross-compilation:**  
   - AddressSanitizer works only in native Desktop debug builds
   - ARM64 uses Valgrind instead (via `./docker-rpi.sh valgrind`)

3. **No remote deployment script yet:**  
   - Manual SCP/SSH to RPi for production deployment
   - Future: `deploy.sh` to consolidate build + transfer + restart

### Production deployment path (future)

```bash
# 1. Build libgpiod v2 with C++ bindings (one-time)
./build-libgpiod-arm64.sh

# 2. Build pigpio for ARM64 (one-time)
./build-pigpio-arm64.sh

# 3. Build the production binary (no stubs, real drivers)
./build-arm64.sh --production

# 4. Deploy to Raspberry Pi
scp build_arm64/LatencyTester pi@<rpi-ip>:/home/pi/LatencyTester/bin/
ssh pi@<rpi-ip> "sudo systemctl restart latencytester"
```

---

## Toolchain Configuration — Biggest Challenges

This section documents the major obstacles encountered during toolchain setup. This was by far the most complex and time-consuming part of the project.

### Challenge 1: qmake + QEMU Incompatibility

**Problem:** Running qmake inside the ARM64 Docker container (via QEMU user-mode) fails because qmake invokes the compiler to parse macros, and this interaction breaks under emulation.

**Attempted solutions:** Native ARM64 qmake in Docker, QEMU user-mode for just qmake.  
**Final solution:** All compilation done on the host with cross-compiler flags passed to qmake directly.

### Challenge 2: GLIBC Version Mismatch

**Problem:** Binaries linked with the host's `ld.bfd` referenced GLIBC symbols not available in the sysroot, causing runtime crashes in the container.

**Attempted solutions:** Matching glibc versions, using Debian Bookworm as base.  
**Final solution:** Switch to Ubuntu 24.04 for both host and container (glibc 2.39), and use `lld` linker which correctly resolves symbols against the sysroot.

### Challenge 3: GCC 12 → GCC 13 (`<format>` header)

**Problem:** The project uses C++20 `<format>` which is only fully supported starting from GCC 13. Ubuntu 22.04's default cross-compiler was GCC 12.

**Attempted solutions:** Finding `<format>` in GCC 12 include paths, custom builds.  
**Final solution:** Upgrade to Ubuntu 24.04 which provides `g++-13-aarch64-linux-gnu` in repos.

### Challenge 4: Qt 6 Must Be Compiled from Source

**Problem:** No prebuilt Qt 6 for ARM64 cross-compilation exists. The Qt Online Installer only provides host-native builds.

**Impact:** Added ~2 hours of one-time build time and ~15 GB disk usage.  
**Solution:** Automated build script (`build-qt6-arm64.sh`) with resume capability.

### Challenge 5: Missing Headers in Sysroot

**Problem:** Initial sysroot extraction missed headers for GL, EGL, DRM, GBM, fontconfig, harfbuzz, and many others required by Qt's configure step.

**Solution:** Iteratively added dev packages to the Dockerfile (`libgl-dev`, `libegl-dev`, `libgles-dev`, `libdrm-dev`, `libgbm-dev`, `libfontconfig-dev`, `libharfbuzz-dev`, etc.) and re-extracted the sysroot each time.

### Challenge 6: pigpio Not in Repos

**Problem:** pigpio is a Raspberry Pi-specific library not packaged in Debian/Ubuntu repos.

**Solution:** For stubs mode (testing), only the header is needed — downloaded from GitHub. For production, pigpio must be cross-compiled from source (future work).

### Challenge 7: libgpiod v1 vs v2 API Break

**Problem:** The submodule `rpi_ads1115` uses the libgpiod v2 C++ bindings API (`gpiod::line_config`, `gpiod::line_settings`, `chip->prepare_request()`, etc.), but Ubuntu 24.04 only packages libgpiod v1.6.x as `libgpiod-dev`. The v2 C++ bindings are not available as a `.deb` package at all.

**Current workaround:** `USE_STUBS=1` bypasses the need for libgpiod entirely.  
**Production solution:** Cross-compile libgpiod v2.2 from source with `--enable-bindings-cxx` using `./build-libgpiod-arm64.sh`. This installs `libgpiodcxx.so` and the v2 `gpiod.hpp` header into the sysroot.

### Challenge 8: Docker exec format error

**Problem:** Shell scripts with Windows line endings (CRLF) cause "exec format error" when used as Docker entrypoints.

**Solution:** Ensure all shell scripts committed with LF line endings. Add `.gitattributes` or use `dos2unix` before building.

---

## File Reference

| Script | Purpose |
|--------|---------|
| `Scripts/Linux/setup-toolchain.sh` | **One-command full setup**: installs everything and builds production binary |
| `Scripts/Linux/docker-rpi.sh` | Docker ARM64 container management (setup/build/run/tests/valgrind/shell/stop/clean) |
| `Scripts/Linux/Dockerfile` | ARM64 runtime image definition (Ubuntu 24.04, Qt 6 libs, Xvfb, VNC, Valgrind) |
| `Scripts/Linux/docker-entrypoint.sh` | Container startup (Xvfb 800×480 + x11vnc on port 5900) |
| `Scripts/Linux/build-arm64.sh` | Cross-compile the project for ARM64 (stubs or production mode) |
| `Scripts/Linux/build-arm64.sh --production` | Cross-compile with real drivers (pigpio + libgpiod) |
| `Scripts/Linux/build-pigpio-arm64.sh` | Cross-compile pigpio from source and install into sysroot |
| `Scripts/Linux/build-libgpiod-arm64.sh` | Cross-compile libgpiod v2 (with C++ bindings) from source |
| `Scripts/Linux/build-qt6-arm64.sh` | One-time: compile Qt 6.11.1 from source for ARM64 |
| `Scripts/Linux/extract-sysroot.sh` | Extract ARM64 libs/headers from Docker for cross-compilation |
| `Scripts/Linux/qt6-arm64-toolchain.cmake` | CMake toolchain file (generated by build-qt6-arm64.sh) |
| `Scripts/Linux/build.sh` | Native Linux Desktop build + Doxygen docs |
| `Scripts/Windows/build.ps1` | Native Windows Desktop build + Doxygen docs |

---

## Quick Reference — Full Workflow from Scratch

The entire toolchain setup is automated in a single script:

```bash
cd LatencyTester/Scripts/Linux
./setup-toolchain.sh
```

This runs all 8 steps interactively, skipping any that are already completed. It takes ~2-3 hours on first run (Qt compilation dominates) and ~5 minutes on subsequent runs.

### Manual step-by-step (equivalent)

```bash
# 1. Install prerequisites (one-time)
sudo apt install gcc-13-aarch64-linux-gnu g++-13-aarch64-linux-gnu \
    lld cmake ninja-build binfmt-support qemu-user-static docker-buildx-plugin

# 2. Enable ARM64 emulation (one-time)
cd LatencyTester/Scripts/Linux
./docker-rpi.sh setup

# 3. Build Docker image
./docker-rpi.sh build

# 4. Extract sysroot from container
./extract-sysroot.sh

# 5. Compile Qt 6.11.1 for ARM64 (one-time, ~2h)
./build-qt6-arm64.sh

# 6a. Cross-compile LatencyTester (STUBS — for Docker testing)
./build-arm64.sh

# 6b. Cross-compile LatencyTester (PRODUCTION — for real RPi)
./build-libgpiod-arm64.sh        # one-time: build libgpiod v2 C++ bindings
./build-pigpio-arm64.sh          # one-time: build pigpio for ARM64
./build-arm64.sh --production    # build with real drivers

# 7. Test with VNC
./docker-rpi.sh run
# In container: cd /app/build_arm64 && ./LatencyTester
# VNC client → localhost:5900

# 8. Run tests (stubs build only)
./docker-rpi.sh tests

# 9. Valgrind memory check
./docker-rpi.sh valgrind

# 10. Deploy to Raspberry Pi (production build)
scp build_arm64/LatencyTester pi@<rpi-ip>:/home/pi/LatencyTester/bin/
```

### Check toolchain status

```bash
./setup-toolchain.sh --status
```

### Re-run a specific step

```bash
./setup-toolchain.sh --only 8    # Only rebuild the production binary
./setup-toolchain.sh --step 6    # Run from step 6 onwards
```
