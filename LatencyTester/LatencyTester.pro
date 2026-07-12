# LatencyTester - Qt Project File
# Supports local (Desktop) and cross-compilation (Raspberry Pi ARM) builds.

QT += core gui widgets printsupport

CONFIG += c++17

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
    dialog.cpp \
    jsonoperator.cpp \
    main.cpp \
    qcustomplot.cpp \
    renamepopup.cpp \
    sensoroperator.cpp \
    startscreen.cpp

HEADERS += \
    dataModel.hpp \
    dialog.h \
    extensionfiledelegate.h \
    jsonoperator.h \
    pigpio_stub.h \
    qcustomplot.h \
    renamepopup.h \
    sensoroperator.h \
    startscreen.h

FORMS += \
    dialog.ui \
    renamepopup.ui \
    startscreen.ui

# --- Translations ---
# Qt 6: lrelease + embed_translations generates and embeds .qm automatically from .ts files.
# No .qrc needed for translations when using embed_translations.

TRANSLATIONS += \
    LatencyTester_en_EN.ts

CONFIG += lrelease
CONFIG += embed_translations

DISTFILES += \
    LatencyTester_pl_PL.ts
