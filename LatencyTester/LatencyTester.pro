# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /home/pi/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

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
    startscreen.cpp

HEADERS += \
    dataModel.hpp \
    dialog.h \
    jsonoperator.h \
    qcustomplot.h \
    renamepopup.h \
    sensoroperator.h \
    startscreen.h \
    extensionfiledelegate.h

FORMS += \
    dialog.ui \
    renamepopup.ui \
    startscreen.ui

TRANSLATIONS += \
    LatencyTester_en_EN.ts
    #LatencyTester_pl_PL.ts

CONFIG += lrelease
CONFIG += embed_translations

LIBS += -L/usr/local/lib -lpigpio -lrt

QT += gui-private widgets

DISTFILES += \
    LatencyTester_en_EN.qm \
    LatencyTester_pl_PL.ts

RESOURCES += \
    translations.qrc
