#include "startscreen.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // Enable Qt Virtual Keyboard for touch input
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QApplication a(argc, argv);
    StartScreen w;
    w.show();
    return a.exec();
}
