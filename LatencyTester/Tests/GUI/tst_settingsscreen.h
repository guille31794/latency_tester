#ifndef TST_SETTINGSSCREEN_H
#define TST_SETTINGSSCREEN_H

#include <QObject>

class TestSettingsScreen : public QObject
{
    Q_OBJECT

private slots:
    void init();  // Reset AppSettings before each test

    // Instantiation
    void test_canInstantiate();

    // Back button
    void test_backButton_emitsSignal();

    // Widgets exist
    void test_languageComboBox_exists();
    void test_fontSizeSlider_exists();
    void test_daltonicCheckbox_exists();
    void test_darkModeCheckbox_exists();
    void test_settingsButtonBox_exists();

    // Initial state from AppSettings
    void test_loadFromSettings_languageComboCorrect();
    void test_loadFromSettings_fontSliderCorrect();
    void test_loadFromSettings_daltonicCorrect();
    void test_loadFromSettings_darkModeCorrect();
    void test_buttonBox_disabledInitially();

    // Widget changes enable buttonBox
    void test_fontSliderChange_enablesButtonBox();
    void test_languageChange_enablesButtonBox();
    void test_daltonicChange_enablesButtonBox();
    void test_darkModeChange_enablesButtonBox();

    // Apply saves to AppSettings
    void test_apply_savesToAppSettings();
    void test_apply_disablesButtonBox();

    // Cancel reverts widgets
    void test_cancel_revertsWidgets();
    void test_cancel_disablesButtonBox();
};

#endif // TST_SETTINGSSCREEN_H
