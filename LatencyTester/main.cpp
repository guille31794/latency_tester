#include "startscreen.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QtDBus/QDBusConnection>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;

    if (!QDBusConnection::sessionBus().registerService("com.virtualKeyboard.inputmethod"))
    {
        qFatal("Unable to register at DBus");
        return 1;
    }

    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages)
    {
        const QString baseName = "LatencyTester_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName))
        {
            a.installTranslator(&translator);
            break;
        }
    }

    StartScreen w;
    w.show();
    return a.exec();
}
