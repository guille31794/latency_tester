# LatencyTester — Video Surveillance Latency Measurement Device

A portable device for measuring end-to-end latency in real-time video surveillance systems. It quantifies the delay between a camera capturing an image and a monitor displaying it, determining whether a system qualifies as real-time critical.

## Overview

A video surveillance system is considered *critical* when it operates in real-time with a delay low enough that the operator perceives no disjunction between reality and what is shown on screen. This project provides a low-cost, high-accuracy instrument to measure that delay.

The device emits a light pulse (LED) into the camera under test, and a photosensor detects when the monitor reproduces that stimulus. The time difference between both events is the system's latency.

## Key Features

- Portable, battery-powered, standalone measurement device
- Touch-friendly Qt GUI on a 7" capacitive display
- Calibration workflow with LED blink feedback
- Real-time latency graph (QCustomPlot)
- Measurement history with JSON persistence
- Multi-language UI (Spanish, English, Polish)
- Dark mode and colorblind-accessible mode
- Runs on Raspberry Pi 3 Model B

## Technology Stack

| Component | Technology |
|---|---|
| Hardware | Raspberry Pi 3 Model B |
| Language | C++17 |
| UI Framework | Qt 6.11 (Widgets) |
| Charts | QCustomPlot 2.1.0 |
| GPIO | pigpio |
| ADC (photosensor) | ADS1115 via I2C (rpi_ads1115) |
| OS | Raspi OS Lite (custom) |
| Virtual keyboard | Qt Virtual Keyboard |
| Build system | qmake + GNU Make |
| Languages | Spanish, English, Polish |

## Project Structure

```
latency_tester/
├── LatencyTester/                ← Application source code
│   ├── main.cpp                  ← Entry point
│   ├── LatencyTester.pro         ← qmake project file
│   ├── Core/                     ← Business logic (no GUI)
│   │   ├── dataModel.hpp         ← Shared data structures
│   │   ├── Helpers/              ← Stubs & header-only utilities
│   │   │   ├── ads1115.h
│   │   │   ├── ads1115rpi_stub.h
│   │   │   └── pigpio_stub.h
│   │   ├── AppSettings/          ← Singleton config manager
│   │   │   └── appsettings.h/.cpp
│   │   ├── JsonOperator/         ← JSON read/write for measurements
│   │   │   └── jsonoperator.h/.cpp
│   │   └── SensorOperator/       ← LED + photosensor control (ADS1115)
│   │       └── sensoroperator.h/.cpp
│   ├── GUI/                      ← UI layer (one folder per screen)
│   │   ├── MainWindow/           ← Navigation controller (QStackedWidget)
│   │   ├── HomeScreen/           ← Main menu (4 buttons)
│   │   ├── MeasureScreen/        ← Calibration, measurement, live graph
│   │   ├── SettingsScreen/       ← Language, font size, dark/colorblind modes
│   │   ├── HelpScreen/           ← Help menu
│   │   ├── HelpInfoScreen/       ← Help content display
│   │   ├── RegistryScreen/       ← Measurement history (TreeView + actions)
│   │   └── RegistryDisplayScreen/← Saved measurement viewer (graph + data)
│   ├── Libs/                     ← Third-party libraries
│   │   ├── QCustomPlot/          ← 2D plotting (patched for Qt 6.11)
│   │   └── rpi_ads1115/          ← ADS1115 ADC driver (git submodule)
│   ├── Measures/                 ← Saved measurement files (.json)
│   ├── Tests/                    ← (reserved)
│   └── Translations/             ← i18n files (.ts: es_ES, en_EN, pl_PL)
├── Medidor_de_latencias_.../     ← Academic report (LaTeX)
├── setup.ps1                     ← Windows setup script
├── setup.sh                      ← Linux setup script
├── toolchain_setup.md            ← Cross-compilation guide
└── README.md
```

## Building

### Prerequisites

- **Qt 6.11+** with modules: Core, Gui, Widgets, PrintSupport, Concurrent
- **MinGW 13+** (Windows) or **GCC 11+** (Linux)
- **Git** (for submodules)
- (Optional) **Qt Virtual Keyboard** — install via Qt Maintenance Tool

### Clone

```bash
git clone --recurse-submodules https://github.com/guille31794/latency_tester.git
cd latency_tester/LatencyTester
```

### Desktop (local development)

#### Using Qt Creator

1. Open `LatencyTester/LatencyTester.pro`
2. Select kit: **Desktop Qt 6.11+ MinGW 64-bit**
3. Build → Build All

#### From the command line (Windows)

```powershell
# Ensure Qt's bin and MinGW bin are in PATH
qmake LatencyTester.pro
mingw32-make -j8
```

#### From the command line (Linux)

```bash
qmake LatencyTester.pro
make -j$(nproc)
```

In Desktop mode, GPIO hardware is replaced by no-op stubs, allowing full UI development without physical hardware.

### Raspberry Pi (production)

Cross-compilation or native build on the Pi:

```bash
# Native build on Raspberry Pi with Qt 6 installed
qmake LatencyTester.pro
make -j4
```

The build system automatically defines `RASPBERRY_PI` when it detects ARM architecture, linking `pigpio` and `libgpiod`.

For cross-compilation setup, see [toolchain_setup.md](toolchain_setup.md).

### Build output

The resulting binary is `LatencyTester` (Linux/RPi) or `release/LatencyTester.exe` (Windows).

Working directory must contain a `Measures/` folder for JSON persistence (created automatically if absent).

## Architecture

The application follows a **Core/GUI separation**:

- **Core** — Pure business logic with no Qt GUI dependencies (settings, sensor control, JSON I/O, data models)
- **GUI** — Qt Widgets screens, each in its own self-contained folder with `.h`, `.cpp`, and `.ui`

Navigation is managed by `MainWindow` using a `QStackedWidget` with 7 screens. Screens communicate via Qt signals/slots.

Settings are managed by a singleton (`AppSettings`) that emits signals when configuration changes, allowing the `MainWindow` to apply themes, font sizes, and accessibility modes globally.

### Screens

| Index | Screen | Description |
|---|---|---|
| 0 | HomeScreen | Main menu with 4 navigation buttons |
| 1 | SettingsScreen | Language, font size, dark mode, colorblind mode |
| 2 | HelpScreen | Help menu (manual + general info) |
| 3 | HelpInfoScreen | Read-only help content |
| 4 | RegistryScreen | Measurement history (list + check/delete/rename) |
| 5 | RegistryDisplayScreen | Saved measurement graph and data |
| 6 | MeasureScreen | Calibration, measurement, live graph, sliders |

## Hardware Components

| Component | Purpose |
|---|---|
| Raspberry Pi 3 Model B | Main computer |
| 7" capacitive touch display | User interface (SPI) |
| High-luminance LED (GPIO 24) | Light stimulus emitter |
| OPT101 photosensor + ADS1115 ADC | Light stimulus detector (I2C) |
| PiSugar 3+ battery (≥3000 mAh) | Portable power supply |
| 3D-printed case (PLA) | Enclosure |

## License

Academic project — University of Cádiz (UCA).

Third-party licenses:
- QCustomPlot: GPL v3
- rpi_ads1115: GPL v2
