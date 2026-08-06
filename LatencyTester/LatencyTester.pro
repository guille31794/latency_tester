# LatencyTester - Qt Project File
# Supports local (Desktop) and cross-compilation (Raspberry Pi ARM) builds.

QT += core gui widgets printsupport concurrent virtualkeyboard

CONFIG += c++17

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

# Detect Raspberry Pi target: defined when using ARM cross-compiler kit
contains(QMAKE_HOST.arch, arm.*) | contains(QT_ARCH, arm.*) | contains(QMAKE_PLATFORM, linux-rasp-pi*) {
    message("Building for Raspberry Pi (ARM)")
    DEFINES += RASPBERRY_PI
    LIBS += -L/usr/local/lib -lpigpio -lrt -lgpiod
    SOURCES += Libs/rpi_ads1115/ads1115rpi.cpp
    HEADERS += Libs/rpi_ads1115/ads1115rpi.h
    target.path = /home/pi/$${TARGET}/bin
    INSTALLS += target
} else {
    message("Building for Desktop (local development)")
    # No pigpio/ads1115 linkage - using stub headers
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
