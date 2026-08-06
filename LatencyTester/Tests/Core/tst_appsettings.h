#ifndef TST_APPSETTINGS_H
#define TST_APPSETTINGS_H

#include <QObject>

class TestAppSettings : public QObject
{
    Q_OBJECT

private slots:
    void init();  // runs before each test

    // Singleton
    void test_instance_returnsSameObject();

    // Defaults after load with no prior config
    void test_load_defaultLanguageSpanish();
    void test_load_defaultFontSize9();
    void test_load_defaultDarkModeOff();
    void test_load_defaultDaltonicModeOff();

    // Save and reload
    void test_saveAndReload_language();
    void test_saveAndReload_fontSize();
    void test_saveAndReload_darkMode();
    void test_saveAndReload_daltonicMode();

    // Setters emit signals
    void test_setLanguage_emitsSignal();
    void test_setFontSize_emitsSignal();
    void test_setDarkMode_emitsThemeChanged();
    void test_setDaltonicMode_emitsSignal();

    // applySettings
    void test_applySettings_emitsSettingsApplied();
    void test_applySettings_emitsOnlyChangedSignals();
    void test_applySettings_savesToDisk();

    // Stylesheets
    void test_currentStylesheet_lightWhenDarkModeOff();
    void test_currentStylesheet_darkWhenDarkModeOn();
    void test_daltonicWidgetStyle_emptyWhenOff();
    void test_daltonicWidgetStyle_nonEmptyWhenOn();
    void test_daltonicBackButtonStyle_emptyWhenOff();
    void test_daltonicBackButtonStyle_nonEmptyWhenOn();

    // Edge cases
    void test_load_fontSizeZeroDefaultsTo9();
};

#endif // TST_APPSETTINGS_H
