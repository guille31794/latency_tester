# Tests - Unit test project for LatencyTester
# Uses QTest framework. Compiles Core and GUI sources for testing.

QT += core gui widgets printsupport concurrent testlib virtualkeyboard

CONFIG += c++17
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app
TARGET = LatencyTesterTests

# --- Include paths (same as main project) ---
INCLUDEPATH += .
INCLUDEPATH += ../Core
INCLUDEPATH += ../Core/Helpers
INCLUDEPATH += ../Core/AppSettings
INCLUDEPATH += ../Core/JsonOperator
INCLUDEPATH += ../Core/SensorOperator
INCLUDEPATH += ../GUI/MainWindow
INCLUDEPATH += ../GUI/HomeScreen
INCLUDEPATH += ../GUI/MeasureScreen
INCLUDEPATH += ../GUI/SettingsScreen
INCLUDEPATH += ../GUI/HelpScreen
INCLUDEPATH += ../GUI/HelpInfoScreen
INCLUDEPATH += ../GUI/RegistryScreen
INCLUDEPATH += ../GUI/RegistryDisplayScreen
INCLUDEPATH += ../Libs/QCustomPlot/Headers
INCLUDEPATH += ../Libs/rpi_ads1115

# --- MinGW big-obj fix ---
win32-g++|win32-g++-*|mingw {
    QMAKE_CXXFLAGS += -Wa,-mbig-obj
}

# --- AddressSanitizer ---
# ASan is used only for native Linux Desktop builds (via Qt Creator or build.sh).
# Cross-compiled ARM64 binaries are validated with Valgrind in Docker instead.
# To enable ASan natively: qmake "QMAKE_CXXFLAGS+=-fsanitize=address -fno-omit-frame-pointer" "QMAKE_LFLAGS+=-fsanitize=address"

DEFINES += QT_NO_CONSTEXPR_METAOBJECT_DATA

# Suppress misleading-indentation warning from QCustomPlot (third-party code)
QMAKE_CXXFLAGS += -Wno-misleading-indentation

# --- Desktop stubs ---
HEADERS += ../Core/Helpers/ads1115rpi_stub.h

# --- Core sources under test ---
SOURCES += \
    ../Core/AppSettings/appsettings.cpp \
    ../Core/JsonOperator/jsonoperator.cpp \
    ../Core/SensorOperator/sensoroperator.cpp

HEADERS += \
    ../Core/Helpers/ads1115.h \
    ../Core/Helpers/pigpio_stub.h \
    ../Core/AppSettings/appsettings.h \
    ../Core/dataModel.hpp \
    ../Core/JsonOperator/jsonoperator.h \
    ../Core/SensorOperator/sensoroperator.h

# --- GUI sources under test ---
SOURCES += \
    ../GUI/MainWindow/mainwindow.cpp \
    ../GUI/HomeScreen/homescreen.cpp \
    ../GUI/MeasureScreen/measurescreen.cpp \
    ../GUI/SettingsScreen/settingsscreen.cpp \
    ../GUI/HelpScreen/helpscreen.cpp \
    ../GUI/HelpInfoScreen/helpinfoscreen.cpp \
    ../GUI/RegistryScreen/registryscreen.cpp \
    ../GUI/RegistryScreen/dialog.cpp \
    ../GUI/RegistryScreen/renamepopup.cpp \
    ../GUI/RegistryDisplayScreen/registrydisplayscreen.cpp \
    ../Libs/QCustomPlot/Sources/qcustomplot.cpp

HEADERS += \
    ../GUI/MainWindow/mainwindow.h \
    ../GUI/HomeScreen/homescreen.h \
    ../GUI/MeasureScreen/measurescreen.h \
    ../GUI/SettingsScreen/settingsscreen.h \
    ../GUI/HelpScreen/helpscreen.h \
    ../GUI/HelpInfoScreen/helpinfoscreen.h \
    ../GUI/RegistryScreen/registryscreen.h \
    ../GUI/RegistryScreen/dialog.h \
    ../GUI/RegistryScreen/renamepopup.h \
    ../GUI/RegistryScreen/extensionfiledelegate.h \
    ../GUI/RegistryDisplayScreen/registrydisplayscreen.h \
    ../Libs/QCustomPlot/Headers/qcustomplot.h

FORMS += \
    ../GUI/HomeScreen/homescreen.ui \
    ../GUI/MeasureScreen/measurescreen.ui \
    ../GUI/SettingsScreen/settingsscreen.ui \
    ../GUI/HelpScreen/helpscreen.ui \
    ../GUI/HelpInfoScreen/helpinfoscreen.ui \
    ../GUI/RegistryScreen/registryscreen.ui \
    ../GUI/RegistryScreen/dialog.ui \
    ../GUI/RegistryScreen/renamepopup.ui \
    ../GUI/RegistryDisplayScreen/registrydisplayscreen.ui

RESOURCES += \
    ../Resources/help.qrc

# --- Test sources ---
HEADERS += \
    Core/tst_datamodel.h \
    Core/tst_appsettings.h \
    Core/tst_jsonoperator.h \
    Core/tst_sensoroperator.h \
    GUI/tst_homescreen.h \
    GUI/tst_measurescreen.h \
    GUI/tst_settingsscreen.h \
    GUI/tst_helpscreen.h \
    GUI/tst_registryscreen.h \
    GUI/tst_registrydisplayscreen.h

SOURCES += \
    main.cpp \
    Core/tst_datamodel.cpp \
    Core/tst_appsettings.cpp \
    Core/tst_jsonoperator.cpp \
    Core/tst_sensoroperator.cpp \
    GUI/tst_homescreen.cpp \
    GUI/tst_measurescreen.cpp \
    GUI/tst_settingsscreen.cpp \
    GUI/tst_helpscreen.cpp \
    GUI/tst_registryscreen.cpp \
    GUI/tst_registrydisplayscreen.cpp

