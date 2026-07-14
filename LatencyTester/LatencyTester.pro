# LatencyTester - Qt Project File
# Supports local (Desktop) and cross-compilation (Raspberry Pi ARM) builds.

QT += core gui widgets printsupport virtualkeyboard

CONFIG += c++17

# --- Directory layout ---
INCLUDEPATH += Headers
DEPENDPATH += Headers

# --- Platform-specific configuration ---

# Detect Raspberry Pi target: defined when using ARM cross-compiler kit
contains(QMAKE_HOST.arch, arm.*) | contains(QT_ARCH, arm.*) | contains(QMAKE_PLATFORM, linux-rasp-pi*) {
    message("Building for Raspberry Pi (ARM)")
    DEFINES += RASPBERRY_PI
    LIBS += -L/usr/local/lib -lpigpio -lrt
    target.path = /home/pi/$${TARGET}/bin
    INSTALLS += target
} else {
    message("Building for Desktop (local development)")
    # No pigpio linkage needed - using stub header
}

# --- MinGW big-obj fix (qcustomplot generates too many sections in debug) ---
win32-g++|win32-g++-*|mingw {
    QMAKE_CXXFLAGS += -Wa,-mbig-obj
}

# --- QCustomPlot compatibility with Qt 6.8+ MOC ---
# QCustomPlot 2.x uses Q_GADGET in namespace QCP which triggers issues with
# the new constexpr metaobject generation in Qt 6.8+. Disable it.
DEFINES += QT_NO_CONSTEXPR_METAOBJECT_DATA

# --- Sources ---

SOURCES += \
    Sources/dialog.cpp \
    Sources/jsonoperator.cpp \
    Sources/main.cpp \
    Sources/qcustomplot.cpp \
    Sources/renamepopup.cpp \
    Sources/sensoroperator.cpp \
    Sources/startscreen.cpp

HEADERS += \
    Headers/dataModel.hpp \
    Headers/dialog.h \
    Headers/extensionfiledelegate.h \
    Headers/jsonoperator.h \
    Headers/pigpio_stub.h \
    Headers/qcustomplot.h \
    Headers/renamepopup.h \
    Headers/sensoroperator.h \
    Headers/startscreen.h

FORMS += \
    Forms/dialog.ui \
    Forms/renamepopup.ui \
    Forms/startscreen.ui

# --- Translations ---
# Qt 6: lrelease + embed_translations generates and embeds .qm automatically from .ts files.
# lrelease is invoked via a custom target to keep .ts files out of "Sources" in Qt Creator.

CONFIG += lrelease
CONFIG += embed_translations

TRANSLATIONS += \
    Translations/LatencyTester_en_EN.ts \
    Translations/LatencyTester_pl_PL.ts

DISTFILES += \
    Translations/LatencyTester_en_EN.ts \
    Translations/LatencyTester_pl_PL.ts
