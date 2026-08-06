#ifndef TST_HELPSCREEN_H
#define TST_HELPSCREEN_H

#include <QObject>

class TestHelpScreen : public QObject
{
    Q_OBJECT

private slots:
    void init();

    // Instantiation
    void test_canInstantiate();

    // Back button
    void test_backButton_exists();
    void test_backButton_emitsSignal();

    // Help buttons exist
    void test_usersManualButton_exists();
    void test_generalInfoButton_exists();

    // Signals emitted
    void test_generalInfoButton_emitsSignal();
    void test_usersManualButton_emitsSignal();

    // Manual button enabled/disabled based on resource availability
    void test_manualButton_enabledForSpanish();
    void test_manualButton_enabledForEnglish();
    void test_manualButton_enabledForPolish();
};

#endif // TST_HELPSCREEN_H
