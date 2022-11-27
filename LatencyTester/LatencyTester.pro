# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /home/pi/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17 dbus

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialog.cpp \
    jsonoperator.cpp \
    main.cpp \
    qcustomplot.cpp \
    renamepopup.cpp \
    sensoroperator.cpp \
    startscreen.cpp \
    virtualkeyboard.cpp

HEADERS += \
    dataModel.hpp \
    dialog.h \
    jsonoperator.h \
    qcustomplot.h \
    renamepopup.h \
    sensoroperator.h \
    startscreen.h \
    extensionfiledelegate.h \
    virtualkeyboard.h

FORMS += \
    dialog.ui \
    renamepopup.ui \
    startscreen.ui

TRANSLATIONS += \
    LatencyTester_es_ES.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

