# LatencyTester - Medidor de Latencias para Sistemas de Videovigilancia Críticos

Dispositivo portátil de medición de latencias para sistemas de videovigilancia en tiempo real. Cuantifica el retardo de vídeo entre la captura de una imagen por la cámara y su visualización en el monitor, determinando si un sistema puede clasificarse como crítico.

## Descripción

Un sistema de videovigilancia es crítico cuando opera en tiempo real con un retardo lo suficientemente bajo como para que el operador no perciba disonancia entre lo que ocurre y lo que ve en pantalla. Este proyecto proporciona un instrumento de medición de bajo coste y alta efectividad para cuantificar dicho retardo.

El dispositivo emite un impulso luminoso (LED) hacia la cámara del sistema bajo prueba, y un fotosensor detecta cuándo el monitor reproduce ese estímulo. La diferencia temporal entre ambos eventos es la latencia del sistema.

## Objetivos

- Desarrollar un dispositivo portátil y autónomo (con batería) para medir latencias de sistemas de vídeo.
- Proporcionar una interfaz de usuario táctil, sencilla e intuitiva.
- Establecer pautas de medición para distintos tipos de sistemas de videovigilancia.
- Ofrecer una solución de bajo coste basada en hardware open source.
- Permitir la clasificación de sistemas como "de tiempo real" en base a su latencia medida.

## Tecnologías

| Componente | Tecnología |
|---|---|
| Hardware | Raspberry Pi 3 Model B |
| Lenguaje | C++17 |
| Framework UI | Qt 6.11 (Widgets) |
| Gráficas | QCustomPlot 2.1.0 |
| GPIO | pigpio |
| ADC (fotosensor) | ADS1115 vía I2C (rpi_ads1115) |
| Sistema operativo | Raspi OS Lite personalizado |
| Teclado virtual | Qt Virtual Keyboard |
| Idiomas | Español, Inglés, Polaco |

## Estructura del Proyecto

```
latency_tester/
├── LatencyTester/            ← Código fuente de la aplicación
│   ├── Headers/              ← Cabeceras propias (.h, .hpp)
│   ├── Sources/              ← Implementaciones (.cpp)
│   ├── Forms/                ← Formularios Qt Designer (.ui)
│   ├── Translations/         ← Traducciones (.ts)
│   ├── Libs/                 ← Bibliotecas de terceros
│   │   ├── QCustomPlot/      ← Gráficas 2D
│   │   └── rpi_ads1115/      ← Driver ADC ADS1115 (git submodule)
│   ├── Measures/             ← Mediciones guardadas (.json)
│   └── LatencyTester.pro     ← Proyecto qmake
├── Medidor_de_latencias_.../  ← Memoria técnica (LaTeX)
└── README.md
```

## Compilación

### Requisitos

- **Qt 6.11+** con módulos: Core, Gui, Widgets, PrintSupport
- **MinGW 13+** (Windows) o **GCC 11+** (Linux)
- (Opcional) **Qt Virtual Keyboard** — instalar via Qt Maintenance Tool

### Desktop (desarrollo local)

```bash
# Clonar con submodules
git clone --recurse-submodules <url-del-repo>

# Abrir en Qt Creator
# File → Open File or Project → LatencyTester/LatencyTester.pro
# Seleccionar kit: Desktop Qt 6.x MinGW 64-bit
# Build → Build All
```

En modo Desktop, el hardware GPIO se sustituye por stubs (no-op) que permiten desarrollar y probar la interfaz sin dispositivo físico.

### Raspberry Pi (producción)

Requiere compilación cruzada ARM o compilación nativa en la RPi:

```bash
# En la Raspberry Pi con Qt 6 instalado
qmake LatencyTester.pro
make -j4
```

El sistema define automáticamente `RASPBERRY_PI` cuando detecta arquitectura ARM y linkea `pigpio` y `libgpiod`.

### Componentes hardware

- Raspberry Pi 3 Model B
- Pantalla táctil capacitiva 7" (conexión SPI)
- LED de alta luminancia (GPIO 24)
- Fotosensor OPT101 + ADS1115 ADC (I2C)
- Batería Pi Sugar 3+ (≥3000 mAh)
- Carcasa impresa en 3D (PLA)

## Licencia

Proyecto académico — Universidad de Cádiz (UCA).
QCustomPlot: GPL v3. rpi_ads1115: GPL v2.
