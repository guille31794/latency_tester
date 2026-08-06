#include "tst_appsettings.h"
#include "appsettings.h"
#include <QtTest>
#include <QSignalSpy>
#include <QSettings>

// Helper: clear all stored settings before each test to ensure isolation
void TestAppSettings::init()
{
    QSettings settings("TFG Guillermo Giron Garcia", "Latency Tester");
    settings.clear();
    settings.sync();

    // Reset the singleton state by reloading with cleared settings
    AppSettings::instance().load();
}

// --- Singleton ---

void TestAppSettings::test_instance_returnsSameObject()
{
    AppSettings& a = AppSettings::instance();
    AppSettings& b = AppSettings::instance();
    QCOMPARE(&a, &b);
}

// --- Defaults after load with no prior config ---

void TestAppSettings::test_load_defaultLanguageSpanish()
{
    QCOMPARE(AppSettings::instance().language(), Languages::SPANISH);
}

void TestAppSettings::test_load_defaultFontSize9()
{
    QCOMPARE(AppSettings::instance().fontSize(), quint8(9));
}

void TestAppSettings::test_load_defaultDarkModeOff()
{
    QCOMPARE(AppSettings::instance().darkMode(), false);
}

void TestAppSettings::test_load_defaultDaltonicModeOff()
{
    QCOMPARE(AppSettings::instance().daltonicMode(), false);
}

// --- Save and reload ---

void TestAppSettings::test_saveAndReload_language()
{
    AppSettings::instance().setLanguage(Languages::ENGLISH);
    AppSettings::instance().save();
    AppSettings::instance().load();
    QCOMPARE(AppSettings::instance().language(), Languages::ENGLISH);
}

void TestAppSettings::test_saveAndReload_fontSize()
{
    AppSettings::instance().setFontSize(13);
    AppSettings::instance().save();
    AppSettings::instance().load();
    QCOMPARE(AppSettings::instance().fontSize(), quint8(13));
}

void TestAppSettings::test_saveAndReload_darkMode()
{
    AppSettings::instance().setDarkMode(true);
    AppSettings::instance().save();
    AppSettings::instance().load();
    QCOMPARE(AppSettings::instance().darkMode(), true);
}

void TestAppSettings::test_saveAndReload_daltonicMode()
{
    AppSettings::instance().setDaltonicMode(true);
    AppSettings::instance().save();
    AppSettings::instance().load();
    QCOMPARE(AppSettings::instance().daltonicMode(), true);
}

// --- Setters emit signals ---

void TestAppSettings::test_setLanguage_emitsSignal()
{
    QSignalSpy spy(&AppSettings::instance(), &AppSettings::languageChanged);
    AppSettings::instance().setLanguage(Languages::POLISH);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().first().value<Languages>(), Languages::POLISH);
}

void TestAppSettings::test_setFontSize_emitsSignal()
{
    QSignalSpy spy(&AppSettings::instance(), &AppSettings::fontSizeChanged);
    AppSettings::instance().setFontSize(12);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().first().value<quint8>(), quint8(12));
}

void TestAppSettings::test_setDarkMode_emitsThemeChanged()
{
    QSignalSpy spy(&AppSettings::instance(), &AppSettings::themeChanged);
    AppSettings::instance().setDarkMode(true);
    QCOMPARE(spy.count(), 1);
    // Signal carries the new stylesheet (non-empty string)
    QVERIFY(!spy.first().first().toString().isEmpty());
}

void TestAppSettings::test_setDaltonicMode_emitsSignal()
{
    QSignalSpy spy(&AppSettings::instance(), &AppSettings::daltonicModeChanged);
    AppSettings::instance().setDaltonicMode(true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().first().toBool(), true);
}

// --- applySettings ---

void TestAppSettings::test_applySettings_emitsSettingsApplied()
{
    QSignalSpy spy(&AppSettings::instance(), &AppSettings::settingsApplied);
    GeneralConfigSettings newSettings;
    newSettings.language = Languages::ENGLISH;
    newSettings.fontSize = 11;
    newSettings.darkMode = true;
    newSettings.daltonicMode = false;
    AppSettings::instance().applySettings(newSettings);
    QCOMPARE(spy.count(), 1);
}

void TestAppSettings::test_applySettings_emitsOnlyChangedSignals()
{
    // Start from known state (defaults after init())
    QSignalSpy themeSpy(&AppSettings::instance(), &AppSettings::themeChanged);
    QSignalSpy fontSpy(&AppSettings::instance(), &AppSettings::fontSizeChanged);
    QSignalSpy langSpy(&AppSettings::instance(), &AppSettings::languageChanged);
    QSignalSpy daltonicSpy(&AppSettings::instance(), &AppSettings::daltonicModeChanged);

    // Apply settings where only language changes
    GeneralConfigSettings same = AppSettings::instance().current();
    same.language = Languages::POLISH;
    AppSettings::instance().applySettings(same);

    QCOMPARE(langSpy.count(), 1);
    QCOMPARE(themeSpy.count(), 0);   // darkMode didn't change
    QCOMPARE(fontSpy.count(), 0);    // fontSize didn't change
    QCOMPARE(daltonicSpy.count(), 0); // daltonicMode didn't change
}

void TestAppSettings::test_applySettings_savesToDisk()
{
    GeneralConfigSettings newSettings;
    newSettings.language = Languages::POLISH;
    newSettings.fontSize = 14;
    newSettings.darkMode = true;
    newSettings.daltonicMode = true;
    AppSettings::instance().applySettings(newSettings);

    // Reload from disk and verify
    AppSettings::instance().load();
    QCOMPARE(AppSettings::instance().language(), Languages::POLISH);
    QCOMPARE(AppSettings::instance().fontSize(), quint8(14));
    QCOMPARE(AppSettings::instance().darkMode(), true);
    QCOMPARE(AppSettings::instance().daltonicMode(), true);
}

// --- Stylesheets ---

void TestAppSettings::test_currentStylesheet_lightWhenDarkModeOff()
{
    AppSettings::instance().setDarkMode(false);
    QString sheet = AppSettings::instance().currentStylesheet();
    // Light theme does NOT contain the dark background selector
    QVERIFY(!sheet.contains("background-color: rgb(35, 35, 50)"));
    // But does contain button styles
    QVERIFY(sheet.contains("QPushButton"));
}

void TestAppSettings::test_currentStylesheet_darkWhenDarkModeOn()
{
    AppSettings::instance().setDarkMode(true);
    QString sheet = AppSettings::instance().currentStylesheet();
    QVERIFY(sheet.contains("background-color: rgb(35, 35, 50)"));
}

void TestAppSettings::test_daltonicWidgetStyle_emptyWhenOff()
{
    AppSettings::instance().setDaltonicMode(false);
    QVERIFY(AppSettings::instance().daltonicWidgetStyle().isEmpty());
}

void TestAppSettings::test_daltonicWidgetStyle_nonEmptyWhenOn()
{
    AppSettings::instance().setDaltonicMode(true);
    QVERIFY(!AppSettings::instance().daltonicWidgetStyle().isEmpty());
    QVERIFY(AppSettings::instance().daltonicWidgetStyle().contains("rgb(82, 183, 136)"));
}

void TestAppSettings::test_daltonicBackButtonStyle_emptyWhenOff()
{
    AppSettings::instance().setDaltonicMode(false);
    QVERIFY(AppSettings::instance().daltonicBackButtonStyle().isEmpty());
}

void TestAppSettings::test_daltonicBackButtonStyle_nonEmptyWhenOn()
{
    AppSettings::instance().setDaltonicMode(true);
    QVERIFY(!AppSettings::instance().daltonicBackButtonStyle().isEmpty());
    QVERIFY(AppSettings::instance().daltonicBackButtonStyle().contains("rgb(157, 2, 8)"));
}

// --- Edge cases ---

void TestAppSettings::test_load_fontSizeZeroDefaultsTo9()
{
    // Write a zero font size to QSettings
    QSettings settings("TFG Guillermo Giron Garcia", "Latency Tester");
    settings.setValue("FontSize", 0);
    settings.sync();

    AppSettings::instance().load();
    QCOMPARE(AppSettings::instance().fontSize(), quint8(9));
}
