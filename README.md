# LatencyTester — Video Surveillance Latency Measurement Device

A portable device for measuring end-to-end latency in real-time video surveillance systems. It quantifies the delay between a camera capturing an image and a monitor displaying it, determining whether a system qualifies as real-time critical.

## Overview

A video surveillance system is considered *critical* when it operates in real-time with a delay low enough that the operator perceives no disjunction between reality and what is shown on screen. This project provides a low-cost, high-accuracy instrument to measure that delay.

The device emits a light pulse (LED) into the camera under test, and a photosensor detects when the monitor reproduces that stimulus. The time difference between both events is the system's latency.

## Key Features

- Portable, battery-powered, standalone measurement device
- Touch-friendly Qt GUI on a 7" capacitive display (800×480)
- Calibration workflow with LED blink feedback
- Real-time latency graph (QCustomPlot)
- Measurement history with JSON persistence
- Multi-language UI (Spanish, English, Polish)
- Dark mode and colorblind-accessible mode
- Integrated user manual (HTML, 3 languages)
- 147 automated unit tests (QTest)
- Runs on Raspberry Pi 3 Model B

## Technology Stack

| Component | Technology |
|---|---|
| Hardware | Raspberry Pi 3 Model B + 7" touch display |
| Language | C++20 |
| UI Framework | Qt 6.11.1 (Widgets, Concurrent, VirtualKeyboard) |
| Charts | QCustomPlot 2.1.0 (patched for Qt 6.11) |
| GPIO | pigpio |
| ADC (photosensor) | ADS1115 via I2C (libgpiod v2 + rpi_ads1115) |
| OS | Raspi OS Lite (custom) |
| Build system | qmake + GNU Make |
| Cross-compiler | GCC 13 (aarch64-linux-gnu-g++-13) |
| Testing | QTest (147 tests), AddressSanitizer, Valgrind |
| Emulation | Docker ARM64 + QEMU user-mode + VNC |

## Project Structure

```
latency_tester/
├── LatencyTester/                    ← Application source code
│   ├── main.cpp                      ← Entry point
│   ├── LatencyTester.pro             ← qmake project file
│   ├── Core/                         ← Business logic (no GUI)
│   │   ├── dataModel.hpp             ← Shared data structures
│   │   ├── Helpers/                  ← Stubs & conditional headers
│   │   ├── AppSettings/              ← Singleton config manager
│   │   ├── JsonOperator/             ← JSON read/write for measurements
│   │   └── SensorOperator/           ← LED + photosensor control
│   ├── GUI/                          ← UI layer (one folder per screen)
│   │   ├── MainWindow/               ← Navigation (QStackedWidget, 7 screens)
│   │   ├── HomeScreen/               ← Main menu
│   │   ├── MeasureScreen/            ← Calibration, measurement, live graph
│   │   ├── SettingsScreen/           ← Language, font, dark/colorblind modes
│   │   ├── HelpScreen/               ← Help menu
│   │   ├── HelpInfoScreen/           ← Help content viewer (QTextBrowser)
│   │   ├── RegistryScreen/           ← Measurement history
│   │   └── RegistryDisplayScreen/    ← Saved measurement viewer
│   ├── Tests/                        ← 147 unit tests (10 suites)
│   ├── Scripts/                      ← Build & deployment automation
│   │   ├── Doxyfile                  ← Doxygen configuration
│   │   ├── Windows/                  ← build.ps1, setup.ps1
│   │   └── Linux/                    ← All Linux/ARM64 scripts (see below)
│   ├── Libs/                         ← Third-party libraries
│   │   ├── QCustomPlot/              ← 2D plotting
│   │   └── rpi_ads1115/              ← ADS1115 ADC driver (git submodule)
│   ├── Resources/                    ← Embedded HTML help (es/en/pl)
│   ├── Measures/                     ← Saved measurement files (.json)
│   └── Translations/                 ← i18n files (.ts)
├── Medidor_de_latencias_.../         ← Academic report (LaTeX)
├── toolchain_setup.md                ← Detailed cross-compilation guide
└── README.md
```

## Building

### Quick Start — Desktop (development)

Desktop builds use hardware stubs, enabling full UI development without a Raspberry Pi.

```bash
git clone --recurse-submodules https://github.com/guille31794/latency_tester.git
cd latency_tester/LatencyTester
```

**Windows (Qt Creator):**
1. Open `LatencyTester.pro`, select kit *Desktop Qt 6.11 MinGW 64-bit*, Build All.

**Windows (CLI):**
```powershell
.\Scripts\Windows\build.ps1
```

**Linux (CLI):**
```bash
./Scripts/Linux/build.sh
```

### Quick Start — ARM64 Production Binary

**Option A: Automated local setup** (requires Ubuntu 24.04 + Qt Online Installer)

```bash
cd LatencyTester/Scripts/Linux
./setup-toolchain.sh
```

This automates 8 steps: host dependencies → ARM64 emulation → Docker image → sysroot extraction → Qt ARM64 from source → libgpiod v2 C++ bindings → pigpio → production binary.

- First run: ~2-3 hours (Qt compilation dominates)
- Subsequent runs: ~5 minutes (skips completed steps)
- Prerequisite: Qt 6.11.1 host installation via [Qt Online Installer](https://www.qt.io/download-qt-installer) at `~/Qt/6.11.1/gcc_64`

**Option B: Fully containerized** (works on any OS with Docker — Linux, Windows WSL2, macOS)

```bash
cd LatencyTester/Scripts/Linux
./docker-production.sh build      # Build everything inside Docker (~2-3h)
./docker-production.sh extract    # Get the ARM64 binary → ./output/LatencyTester
./docker-production.sh tests      # Run unit tests (ARM64 emulated)
```

No Qt installation, no cross-compilers, no sysroot setup needed on the host. Just Docker.

> **Windows users:** Install [Docker Desktop](https://www.docker.com/products/docker-desktop/) or [Rancher Desktop](https://rancherdesktop.io/) (with dockerd/moby engine). You have two options:
>
> - **PowerShell (native):** Run `.\LatencyTester\Scripts\Windows\docker-production.ps1 build` directly from PowerShell. No WSL needed.
> - **WSL2 terminal:** Navigate to the repo inside WSL and run the `.sh` scripts as shown above.

Check toolchain status (Option A only):
```bash
./setup-toolchain.sh --status
```

### Build Modes

| Mode | Command | Use case |
|------|---------|----------|
| Desktop (Windows) | `build.ps1` or Qt Creator | Development, UI testing |
| Desktop (Linux) | `build.sh` or Qt Creator | Development + ASan |
| ARM64 Stubs (Docker) | `./build-arm64.sh` | Functional testing via VNC |
| ARM64 Production | `./build-arm64.sh --production` | Raspberry Pi deployment |

### ARM64 Testing with Docker

```bash
cd LatencyTester/Scripts/Linux

# Build stubs binary and run in Docker with VNC:
./build-arm64.sh
./docker-rpi.sh run
# Connect VNC client to localhost:5900

# Run unit tests:
./docker-rpi.sh tests

# Valgrind memory analysis:
./docker-rpi.sh valgrind
```

### Deploy to Raspberry Pi

```bash
scp build_arm64/LatencyTester pi@<ip>:/home/pi/LatencyTester/bin/
```

## Testing

147 automated tests in 10 suites, covering Core logic and all GUI screens:

```bash
# Desktop (from build directory):
./LatencyTesterTests

# ARM64 (Docker):
./docker-rpi.sh tests
```

| Suite | Tests | Coverage |
|-------|-------|----------|
| tst_datamodel | 17 | Data structures, defaults, edge cases |
| tst_appsettings | 23 | Singleton, QSettings, signals, themes |
| tst_jsonoperator | 14 | JSON parse/save, corrupt files |
| tst_sensoroperator | 14 | Calibration, measurement, timeout |
| tst_homescreen | 10 | Navigation buttons, signals |
| tst_measurescreen | 14 | Buttons, sliders, graph, states |
| tst_settingsscreen | 20 | Apply/cancel, widget sync |
| tst_helpscreen | 11 | Manual button state per language |
| tst_registryscreen | 10 | TreeView, actions, signals |
| tst_registrydisplayscreen | 14 | Data display, plot, edge cases |

All tests pass on Windows x86_64, Linux x86_64, and ARM64 (Docker emulated).

## Scripts Reference

| Script | Purpose |
|--------|---------|
| `Scripts/Linux/setup-toolchain.sh` | **One-command local setup** — installs everything, builds production binary |
| `Scripts/Linux/docker-production.sh` | **Fully containerized build** — no host deps needed, just Docker |
| `Scripts/Windows/docker-production.ps1` | Same as above, native PowerShell for Windows (no WSL needed) |
| `Scripts/Linux/build-arm64.sh` | Cross-compile for ARM64 (stubs or `--production`) |
| `Scripts/Linux/build-qt6-arm64.sh` | Compile Qt 6.11.1 from source for ARM64 (one-time) |
| `Scripts/Linux/build-libgpiod-arm64.sh` | Compile libgpiod v2.2 + C++ bindings for ARM64 |
| `Scripts/Linux/build-pigpio-arm64.sh` | Compile pigpio from source for ARM64 |
| `Scripts/Linux/extract-sysroot.sh` | Extract ARM64 sysroot from Docker |
| `Scripts/Linux/docker-rpi.sh` | Docker container management (build/run/tests/valgrind/shell/stop/clean) |
| `Scripts/Linux/build.sh` | Native Linux Desktop build + Doxygen |
| `Scripts/Windows/build.ps1` | Native Windows Desktop build + Doxygen |

## Architecture

The application follows a **Core/GUI separation**:

- **Core** — Pure business logic with no Qt GUI dependencies (settings, sensor control, JSON I/O, data models)
- **GUI** — Qt Widgets screens, each self-contained with `.h`, `.cpp`, and `.ui`

Navigation is managed by `MainWindow` using a `QStackedWidget` (7 screens). Screens communicate via Qt signals/slots. Settings are centralized in a singleton (`AppSettings`) that emits signals for theme, font, and accessibility changes.

### Screens

| Index | Screen | Description |
|---|---|---|
| 0 | HomeScreen | Main menu with 4 navigation buttons |
| 1 | SettingsScreen | Language, font size, dark mode, colorblind mode |
| 2 | HelpScreen | Help menu (manual + general info) |
| 3 | HelpInfoScreen | Read-only help content (HTML) |
| 4 | RegistryScreen | Measurement history (list + check/delete/rename) |
| 5 | RegistryDisplayScreen | Saved measurement graph and data |
| 6 | MeasureScreen | Calibration, measurement, live graph, sliders |

## Hardware Components

| Component | Purpose |
|---|---|
| Raspberry Pi 3 Model B | Main computer |
| 7" capacitive touch display (800×480) | User interface (SPI) |
| High-luminance LED (GPIO 24) | Light stimulus emitter |
| OPT101 photosensor + ADS1115 ADC | Light stimulus detector (I2C) |
| PiSugar 3+ battery (≥3000 mAh) | Portable power supply |
| 3D-printed case (PLA) | Enclosure |

## Documentation

- **[toolchain_setup.md](toolchain_setup.md)** — Complete cross-compilation guide (architecture, troubleshooting, known issues)
- **Doxygen** — Auto-generated API docs: `./Scripts/Linux/build.sh --docs-only`
- **LaTeX report** — Full academic document in `Medidor_de_latencias_.../`

## License

Academic project — University of Cádiz (UCA).

Third-party licenses:
- QCustomPlot: GPL v3
- rpi_ads1115: GPL v2
- pigpio: Public Domain
- libgpiod: LGPL v2.1+ (C library), LGPL v3+ (C++ bindings)
