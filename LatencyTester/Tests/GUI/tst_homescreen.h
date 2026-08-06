#ifndef TST_HOMESCREEN_H
#define TST_HOMESCREEN_H

#include <QObject>

class TestHomeScreen : public QObject
{
    Q_OBJECT

private slots:
    // Widget instantiation
    void test_canInstantiate();
    void test_hasCorrectWindowTitle();

    // Button signals
    void test_startMeasuringButton_emitsSignal();
    void test_historicButton_emitsSignal();
    void test_settingsButton_emitsSignal();
    void test_helpButton_emitsSignal();

    // Buttons exist and are enabled
    void test_startMeasuringButton_exists();
    void test_historicButton_exists();
    void test_settingsButton_exists();
    void test_helpButton_exists();
};

#endif // TST_HOMESCREEN_H
