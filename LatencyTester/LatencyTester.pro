# LatencyTester - Qt Project File
# Supports local (Desktop) and cross-compilation (Raspberry Pi ARM) builds.

QT += core gui widgets printsupport concurrent virtualkeyboard

CONFIG += c++20

# --- Directory layout ---
INCLUDEPATH += Core
INCLUDEPATH += Core/Helpers
INCLUDEPATH += Core/AppSettings
INCLUDEPATH += Core/JsonOperator
INCLUDEPATH += Core/SensorOperator
INCLUDEPATH += GUI/MainWindow
INCLUDEPATH += GUI/HomeScreen
INCLUDEPATH += GUI/MeasureScreen
INCLUDEPATH += GUI/SettingsScreen
INCLUDEPATH += GUI/HelpScreen
INCLUDEPATH += GUI/HelpInfoScreen
INCLUDEPATH += GUI/RegistryScreen
INCLUDEPATH += GUI/RegistryDisplayScreen
INCLUDEPATH += Libs/QCustomPlot/Headers
INCLUDEPATH += Libs/rpi_ads1115

# --- Platform-specific configuration ---
#
# Build modes (controlled via qmake variable USE_STUBS):
#   qmake USE_STUBS=1    → ARM64 cross-compile with stubs (for Docker testing)
#   qmake USE_STUBS=0    → ARM64 cross-compile with real drivers (production RPi)
#   qmake (desktop)      → Desktop build with stubs (default)
#
# Detection: ARM target is identified by QT_ARCH or QMAKE_HOST.arch

contains(QMAKE_HOST.arch, arm.*) | contains(QT_ARCH, arm.*) | contains(QMAKE_PLATFORM, linux-rasp-pi*) {
    target.path = /home/pi/$${TARGET}/bin
    INSTALLS += target

    equals(USE_STUBS, 1) {
        message("Building for ARM64 with STUBS (cross-compile / Docker testing)")
        # No RASPBERRY_PI define → uses pigpio_stub.h and ads1115rpi_stub.h
        HEADERS += Core/Helpers/ads1115rpi_stub.h
    } else {
        message("Building for ARM64 with REAL DRIVERS (production)")
        DEFINES += RASPBERRY_PI
        LIBS += -L/usr/local/lib -L/usr/lib -lpigpio -lrt -lgpiod -lgpiodcxx
        # When cross-compiling with a custom sysroot, add its lib paths
        !isEmpty(SYSROOT_PATH) {
            LIBS += -L$$SYSROOT_PATH/usr/lib/aarch64-linux-gnu \
                    -L$$SYSROOT_PATH/usr/local/lib
        }
        SOURCES += Libs/rpi_ads1115/ads1115rpi.cpp
        HEADERS += Libs/rpi_ads1115/ads1115rpi.h
    }
} else {
    message("Building for Desktop (local development)")
    HEADERS += Core/Helpers/ads1115rpi_stub.h
}

# --- MinGW big-obj fix (qcustomplot generates too many sections in debug) ---
win32-g++|win32-g++-*|mingw {
    QMAKE_CXXFLAGS += -Wa,-mbig-obj
}

# --- AddressSanitizer (Debug only) ---
# Detects memory leaks, buffer overflows, use-after-free at runtime.
# Only available on Linux (GCC ships libasan). MinGW from Qt does not include it.
CONFIG(debug, debug|release):linux {
    QMAKE_CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
    QMAKE_LFLAGS += -fsanitize=address
}

# --- QCustomPlot compatibility with Qt 6.8+ MOC ---
# QCustomPlot 2.x uses Q_GADGET in namespace QCP which triggers issues with
# the new constexpr metaobject generation in Qt 6.8+. Disable it.
DEFINES += QT_NO_CONSTEXPR_METAOBJECT_DATA

# Suppress misleading-indentation warning from QCustomPlot (third-party code)
QMAKE_CXXFLAGS += -Wno-misleading-indentation

# --- Sources ---

SOURCES += \
    main.cpp \
    Core/AppSettings/appsettings.cpp \
    Core/JsonOperator/jsonoperator.cpp \
    Core/SensorOperator/sensoroperator.cpp \
    GUI/MainWindow/mainwindow.cpp \
    GUI/HomeScreen/homescreen.cpp \
    GUI/MeasureScreen/measurescreen.cpp \
    GUI/SettingsScreen/settingsscreen.cpp \
    GUI/HelpScreen/helpscreen.cpp \
    GUI/HelpInfoScreen/helpinfoscreen.cpp \
    GUI/RegistryScreen/registryscreen.cpp \
    GUI/RegistryScreen/dialog.cpp \
    GUI/RegistryScreen/renamepopup.cpp \
    GUI/RegistryDisplayScreen/registrydisplayscreen.cpp \
    Libs/QCustomPlot/Sources/qcustomplot.cpp

HEADERS += \
    Core/Helpers/ads1115.h \
    Core/Helpers/pigpio_stub.h \
    Core/AppSettings/appsettings.h \
    Core/dataModel.hpp \
    Core/JsonOperator/jsonoperator.h \
    Core/SensorOperator/sensoroperator.h \
    GUI/MainWindow/mainwindow.h \
    GUI/HomeScreen/homescreen.h \
    GUI/MeasureScreen/measurescreen.h \
    GUI/SettingsScreen/settingsscreen.h \
    GUI/HelpScreen/helpscreen.h \
    GUI/HelpInfoScreen/helpinfoscreen.h \
    GUI/RegistryScreen/registryscreen.h \
    GUI/RegistryScreen/dialog.h \
    GUI/RegistryScreen/renamepopup.h \
    GUI/RegistryScreen/extensionfiledelegate.h \
    GUI/RegistryDisplayScreen/registrydisplayscreen.h \
    Libs/QCustomPlot/Headers/qcustomplot.h

FORMS += \
    GUI/HomeScreen/homescreen.ui \
    GUI/MeasureScreen/measurescreen.ui \
    GUI/SettingsScreen/settingsscreen.ui \
    GUI/HelpScreen/helpscreen.ui \
    GUI/HelpInfoScreen/helpinfoscreen.ui \
    GUI/RegistryScreen/registryscreen.ui \
    GUI/RegistryScreen/dialog.ui \
    GUI/RegistryScreen/renamepopup.ui \
    GUI/RegistryDisplayScreen/registrydisplayscreen.ui

# --- Translations ---
# Qt 6: lrelease + embed_translations generates and embeds .qm automatically from .ts files.
# lrelease is invoked via a custom target to keep .ts files out of "Sources" in Qt Creator.

CONFIG += lrelease
CONFIG += embed_translations

TRANSLATIONS += \
    Translations/LatencyTester_es_ES.ts \
    Translations/LatencyTester_en_EN.ts \
    Translations/LatencyTester_pl_PL.ts

DISTFILES += \
    Translations/LatencyTester_es_ES.ts \
    Translations/LatencyTester_en_EN.ts \
    Translations/LatencyTester_pl_PL.ts

# --- Resources ---
RESOURCES += \
    Resources/help.qrc
