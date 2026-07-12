# Latency Tester - Análisis del Documento LaTeX

## Contexto General del Proyecto

El proyecto consiste en un **medidor de latencias para sistemas de videovigilancia críticos**. Se trata de un dispositivo portátil basado en Raspberry Pi 3 Model B que mide el retardo de vídeo en sistemas CCTV mediante un LED estimulador y un fotosensor receptor. El documento LaTeX (`document.tex`) es la memoria técnica/académica del proyecto (PFC - Proyecto Fin de Carrera, Universidad de Cádiz).

### Estructura del documento:
- **Cap. 1 - Introducción**: Motivación, objetivos, alcance, glosario
- **Cap. 2 - Transfondo tecnológico**: Visión nocturna, CCTV, hardware
- **Cap. 3 - Desarrollo del calendario**: Fases, Gantt
- **Cap. 4 - Descripción general del proyecto**: Perspectiva, funciones, requisitos, casos de uso, diseño UI, fabricación, implementación
- **Cap. 5 - Resumen** (vacío)
- **Cap. 6 - Conclusiones y trabajo futuro** (vacío)
- **Cap. 7 - Agradecimientos**
- **Apéndices**: Manuales (vacíos)

---

## Análisis de Warnings en la Compilación LaTeX

Se han identificado las siguientes categorías de warnings al compilar con pdfLaTeX (MiKTeX 2026):

---

### 1. KOMA-Script (scrbook) - Opciones Deprecadas [ALTA PRIORIDAD]

**Problema**: Se usan opciones antiguas de KOMA-Script que activan modo de compatibilidad legacy.

| Opción deprecada | Reemplazo moderno |
|---|---|
| `bibtotoc` | `bibliography=totoc` |
| `idxtotoc` | `index=totoc` |
| `tablecaptionabove` | `captions=tableheading` |
| `intoc` | ❌ No válida (unused global option) |

**Solución**: Cambiar la línea `\documentclass` de:
```latex
\documentclass[12pt,spanish,intoc,bibtotoc,idxtotoc,BCOR=0mm,tablecaptionabove]{scrbook}
```
a:
```latex
\documentclass[12pt,spanish,bibliography=totoc,index=totoc,BCOR=0mm,captions=tableheading]{scrbook}
```

**Estado**: ⬜ Pendiente

---

### 2. PDF Version Mismatch [MEDIA PRIORIDAD]

**Problema**: Los PDFs incluidos (portadas) son versión 1.7, pero pdfLaTeX por defecto genera PDF 1.5.

**Archivos afectados**:
- `Documents/Portada-externa.pdf`
- `Documents/Primera-interna.pdf`
- `Documents/Segunda-interna.pdf`

**Solución**: Añadir antes de `\begin{document}`:
```latex
\pdfminorversion=7
```

**Estado**: ⬜ Pendiente

---

### 3. Duplicate Page Destinations (hyperref) [ALTA PRIORIDAD]

**Problema**: La numeración de páginas se reinicia con `\frontmatter`/`\mainmatter`, causando que hyperref genere identificadores de página duplicados (page.1, page.2, etc. ya usados por las portadas).

**Solución**: Añadir al preámbulo:
```latex
\usepackage{atbegshi}
\AtBeginDocument{\AtBeginShipoutFirst{\AtBeginShipoutDiscard}}
```
O mejor, usar la opción `pdfpagelabels` de hyperref (ya incluida como `plainpages=false`) y añadir `\pagenumbering{Alph}` antes de las portadas, o usar:
```latex
\hypersetup{pdfpagelabels=true}
```
Y asegurar que se usa `\pagenumbering{roman}` en frontmatter y `\pagenumbering{arabic}` en mainmatter (que ya se hace implícitamente). La solución más limpia es añadir al preámbulo:
```latex
\usepackage{atbegshi}
```
Y redefinir el inicio del documento para que las páginas del PDF incluido no generen destinos hyperref duplicados. Alternativa más simple:
```latex
\hypersetup{pageanchor=false}
% ... portadas ...
\hypersetup{pageanchor=true}
```

**Estado**: ⬜ Pendiente

---

### 4. Package geometry - Marginal Notes Overflow [BAJA PRIORIDAD]

**Problema**: Las notas marginales sobrepasan el papel por la configuración de márgenes.

**Solución**: Añadir `marginparwidth=0pt` o ajustar en geometry:
```latex
\geometry{verbose,lmargin=3.5cm,rmargin=2.5cm,marginparwidth=0pt}
```
O si no se usan notas marginales, simplemente ignorar.

**Estado**: ⬜ Pendiente

---

### 5. `\float@addtolists` Deprecated [MEDIA PRIORIDAD]

**Problema**: El paquete `float` usa una interfaz deprecada con KOMA-Script moderno.

**Solución**: Actualizar el paquete `float` o añadir después de cargarlo:
```latex
\usepackage{scrhack}
```
Esto parcela los paquetes incompatibles con KOMA-Script moderno.

**Estado**: ⬜ Pendiente

---

### 6. Package tracklang - No datatool support for Spanish [BAJA PRIORIDAD]

**Problema**: El paquete `glossaries` a través de `tracklang` no encuentra soporte datatool para español.

**Solución**: Cargar `datatool` antes de `glossaries`, o ignorar si no se usa funcionalidad de datatool con glossaries:
```latex
\usepackage[spanish]{datatool}
```
O simplemente ignorar este warning ya que no afecta al resultado.

**Estado**: ⬜ Pendiente (ignorable)

---

### 7. Overfull \hbox [MEDIA PRIORIDAD]

**Problema**: Tres instancias de contenido que desborda el ancho de línea:

1. **Líneas 522-557** (tabla comparativa hardware): 21.6pt de desborde
2. **Líneas 996-1042** (tabla CU): 40.7pt de desborde  
3. **Línea 108-110 de bibliografía**: URL larga (116.8pt de desborde)

**Solución**:
- Tablas: Reducir contenido, usar `\resizebox` o ajustar anchos de columna
- URL en bibliografía: Añadir `\usepackage[hyphens]{url}` ANTES de hyperref, o usar `\sloppy` localmente

**Estado**: ⬜ Pendiente

---

### 8. Underfull \vbox (numerosas) [BAJA PRIORIDAD]

**Problema**: ~25 instancias de páginas con contenido insuficiente para llenar la caja vertical (casi todas badness 10000). Causado principalmente por el uso excesivo de `[H]` en los floats que fuerza figuras en posición exacta.

**Solución**: 
- Considerar usar `[htbp]` en lugar de `[H]` donde sea posible
- Añadir `\raggedbottom` después de `\begin{document}` para permitir espacio variable al pie de página
- Alternativa mínima: `\raggedbottom` elimina todos estos warnings

**Estado**: ⬜ Pendiente

---

### 9. Hyperref cargado dos veces [BAJA PRIORIDAD]

**Problema**: `hyperref` se carga una vez en el preámbulo principal y otra en la sección "User specified LaTeX commands":
```latex
\usepackage{hyperref}  % línea ~88 del preámbulo "user specified"
```
Esto puede causar conflictos.

**Solución**: Eliminar la segunda carga de `\usepackage{hyperref}` en la sección de comandos del usuario.

**Estado**: ⬜ Pendiente

---

### 10. inputenc utf8x vs utf8 [BAJA PRIORIDAD]

**Problema**: Se carga `utf8x` pero el sistema lo redirige a `utf8` estándar (el paquete ucs no está cargado).

**Solución**: Cambiar:
```latex
\usepackage[utf8x]{inputenc}
```
a:
```latex
\usepackage[utf8]{inputenc}
```

**Estado**: ⬜ Pendiente

---

## Plan de Acción (Orden de Ejecución Recomendado)

1. ✅ Actualizar opciones de `\documentclass` (eliminó ~10 warnings de KOMA-Script)
2. ✅ Añadir `\usepackage{scrhack}` (eliminó warning de float@addtolists)
3. ✅ Añadir `\pdfminorversion=7` (eliminó ~9 warnings de PDF version)
4. ✅ Resolver duplicación de page destinations con hyperref (pageanchor=false/true)
5. ✅ Eliminar la carga duplicada de `\usepackage{hyperref}`
6. ✅ Cambiar `utf8x` por `utf8`
7. ✅ Añadir `\raggedbottom` (eliminó ~25 warnings de underfull vbox)
8. ✅ Ajustar geometry para marginal notes (marginparwidth=0pt)
9. ✅ Resolver overfull hbox en tablas (columnas p{}) y URL de bibliografía (\sloppy + fix bib)
10. ⬜ (Opcional) Resolver tracklang/datatool warning - ÚNICO WARNING RESTANTE (cosmético)

---

## Resultado Final

**Antes**: ~50 warnings (KOMA-Script, PDF version, hyperref duplicates, underfull vbox, overfull hbox, etc.)  
**Después**: 1 warning cosmético (`tracklang` - no afecta al documento)

### Cambios realizados en `document.tex`:
- `\pdfminorversion=7` añadido antes de `\documentclass`
- `\documentclass` actualizado con opciones modernas de KOMA-Script
- `utf8x` → `utf8` en inputenc
- `\usepackage{scrhack}` añadido después de `float`
- `\usepackage[hyphens]{url}` en lugar de `\usepackage{url}`
- `marginparwidth=0pt` en geometry
- Eliminada carga duplicada de `\usepackage{hyperref}`
- `\raggedbottom` añadido tras `\begin{document}`
- `\hypersetup{pageanchor=false/true}` alrededor de las portadas
- Tabla hardware: columnas `p{5cm}` + `\small` para evitar desborde
- Tabla CU1: columna `p{10cm}` para evitar desborde
- Bibliografía envuelta en `{\sloppy ... }`

### Cambios realizados en `referencias.bib`:
- Corregido `url:{...}` → `\url{...}` en entrada `videovigilancia_historia`
- Corregido `url{...}` → `\url{...}` en entrada `sionyx_night_vision`

---

## Resumen de lo que Queda por Hacer

### En el documento LaTeX:
- Aplicar las correcciones de los 10 puntos listados arriba → ✅ COMPLETADO
- Recompilar y verificar que los warnings desaparecen → ✅ Solo queda 1 warning cosmético (tracklang)

### En el proyecto general (secciones vacías del documento):
- Capítulo "Implementación" - vacío
- Capítulo "Pruebas y validación" - vacío
- Capítulo "Resumen" - vacío
- Capítulo "Conclusiones y trabajo futuro" - vacío
- Apéndices (Manual de instalación, usuario, desarrollador) - vacíos
- Subsección "Modelo conceptual de datos del dominio" - vacía

---

## Análisis del Código Fuente (LatencyTester/)

### Arquitectura General

Aplicación Qt (C++17) diseñada para ejecutarse en Raspberry Pi 3 Model B con pantalla táctil. Usa la biblioteca `pigpio` para acceso GPIO y `QCustomPlot` para gráficos. Ahora compila tanto en Desktop (Windows MinGW) como en ARM (Raspberry Pi).

### Entorno de Desarrollo Actual

- **Qt**: 6.11.1 MinGW 64-bit (Desktop)
- **Compilador**: g++ 13.1.0 (MinGW-w64)
- **IDE**: Qt Creator
- **OS desarrollo**: Windows

### Estructura de Archivos

| Archivo | Responsabilidad |
|---|---|
| `main.cpp` | Entry point, lanza `StartScreen` |
| `startscreen.h/cpp/ui` | Ventana principal, navegación entre pantallas, lógica de UI |
| `dialog.h/cpp/ui` | Diálogo simple con nombre de archivo |
| `renamepopup.h/cpp/ui` | Popup para renombrar mediciones |
| `sensoroperator.h/cpp` | Control del LED y fotosensor vía GPIO (pigpio) |
| `jsonoperator.h/cpp` | Persistencia de mediciones en formato JSON |
| `dataModel.hpp` | Structs y enums: `Measures`, `GeneralConfigSettings`, `MenuScreen`, `Languages` |
| `extensionfiledelegate.h` | Delegado Qt para mostrar archivos sin extensión en listas |
| `qcustomplot.h/cpp` | Biblioteca externa para gráficas (terceros, parcheada) |
| `pigpio_stub.h` | Stub de pigpio para compilación Desktop (sin hardware) |
| `LatencyTester.pro` | Archivo de proyecto qmake |
| `LatencyTester_en_EN.ts` | Traducción inglés |
| `LatencyTester_pl_PL.ts` | Traducción polaco |
| `Measures/` | Carpeta donde se guardan las mediciones JSON |

### Flujo de Medición

1. **StartScreen** → usuario navega a pantalla de mediciones
2. **SensorOperator::takeMeasure()** → enciende LED, espera respuesta del fotosensor vía GPIO/I2C
3. Calcula latencia como diferencia temporal entre emisión de luz y detección
4. Resultados graficados en tiempo real con **QCustomPlot**
5. Al finalizar, se guarda vía **JsonOperator** en `Measures/*.json`

### Pantallas (MenuScreen enum)

- `START_SCREEN` - Pantalla principal
- `START_MEASURE_SCREEN` - Toma de mediciones
- `MEASURES_REGISTRY_SCREEN` - Registro histórico
- `SETTINGS_SCREEN` - Configuración (idioma, fuente, daltonismo)
- `HELP_SCREEN` - Ayuda
- `GENERAL_INFO_SCREEN` - Info general
- `USERS_MANUAL_SCREEN` - Manual de usuario
- `REGISTRY_DISPLAYER_SCREEN` - Visualización de medición guardada

### Dependencias Externas

- **Qt 6.11.1** (core, gui, widgets, printsupport) — migrado desde Qt 5.15
- **pigpio** - biblioteca C para GPIO en Raspberry Pi (stub en Desktop)
- **QCustomPlot 2.1.0** - gráficas 2D (incluida como fuente, parcheada para Qt 6.11)

### Configuración de Deploy

- **Desktop**: Compilación local para desarrollo/debug (pigpio stub)
- **Raspberry Pi**: Compilación cruzada ARM, target `/home/pi/LatencyTester/bin`
- Sistema operativo RPi: Raspi OS Lite personalizado

### Toolchain - Cambios Realizados

| Cambio | Motivo |
|---|---|
| `pigpio_stub.h` creado | Permite compilar en Desktop sin hardware GPIO |
| `sensoroperator.h` — include condicional `#ifdef RASPBERRY_PI` | Selección automática pigpio real vs stub |
| `.pro` — detección ARM/Desktop automática | Define `RASPBERRY_PI` solo en ARM, linkea pigpio solo ahí |
| `.pro` — `QMAKE_CXXFLAGS += -Wa,-mbig-obj` | Fix "too many sections" de qcustomplot.o en Debug MinGW |
| `.pro` — `DEFINES += QT_NO_CONSTEXPR_METAOBJECT_DATA` | Intento de fix MOC (parcial) |
| `qcustomplot.h` — parcheado namespace QCP | `#ifndef Q_MOC_RUN` → `#if 1` para evitar Q_GADGET fake class incompatible con MOC Qt 6.8+ |
| `qcustomplot.h` — comentado `Q_DECLARE_METATYPE` para enums QCP | Sin MOC class trick, estos no aplican |
| `translations.qrc` eliminado | Qt 6 `embed_translations` gestiona .qm automáticamente |
| `startscreen.cpp` — paths traducción → `:/i18n/...` | Rutas de recurso embebido Qt 6 |
| `startscreen.cpp` — `(void)mTranslator.load()` | Silenciar `[[nodiscard]]` de Qt 6 |
| `.pro` — eliminado `QT += gui-private` | No se usaba, causaba warnings |

---

## Gitignore

Se ha creado `.gitignore` en la raíz del repositorio que excluye:
- `build-LatencyTester-*/` (carpetas de build Qt Creator)
- Archivos auxiliares de LaTeX (*.aux, *.log, *.bbl, etc.)
- `document.pdf` (generado)
- `Latency_tester.md` (fichero de contexto local)

---

## Tareas Pendientes

### PRIORITARIAS — Código fuente (LatencyTester)

1. ⬜ **Revisar warnings de compilación** — qcustomplot.cpp emite ~9 warnings de APIs deprecadas en Qt 6 (`toTimeSpec`, `mirrored`, `startOfDay`, `implicit-fallthrough`). No bloquean pero ensucian la salida.
2. ⬜ **Actualizar proyecto de Qt 5 a Qt 6.11.1 de forma completa** — Actualmente funciona con parches mínimos. Una migración completa implicaría:
   - Actualizar QCustomPlot a una versión compatible nativamente con Qt 6.8+ (cuando exista) o portar los cambios de API deprecada
   - Revisar uso de APIs Qt 5 que están deprecadas o eliminadas en Qt 6
   - Eliminar el define `QT_NO_CONSTEXPR_METAOBJECT_DATA` cuando ya no sea necesario
   - Migrar de qmake a CMake (recomendado por Qt para Qt 6)
   - Revisar `Q_DECLARE_METATYPE` y registro de tipos con el nuevo sistema de metatypes Qt 6
3. ⬜ **Revisar startscreen.cpp** — Posibles incompatibilidades de API Qt 6 en runtime (signals/slots, modelos, etc.)

### DOCUMENTO LaTeX

4. ⬜ Capítulo "Implementación" - vacío
5. ⬜ Capítulo "Pruebas y validación" - vacío
6. ⬜ Capítulo "Resumen" - vacío
7. ⬜ Capítulo "Conclusiones y trabajo futuro" - vacío
8. ⬜ Apéndices (Manual de instalación, usuario, desarrollador) - vacíos
9. ⬜ Subsección "Modelo conceptual de datos del dominio" - vacía
10. ⬜ (Opcional) Resolver tracklang/datatool warning LaTeX
