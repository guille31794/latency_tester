#include "tst_helpscreen.h"
#include "helpscreen.h"
#include "appsettings.h"
#include <QtTest>
#include <QSignalSpy>
#include <QPushButton>
#include <QSettings>

void TestHelpScreen::init()
{
    QSettings settings("TFG Guillermo Giron Garcia", "Latency Tester");
    settings.clear();
    settings.sync();
    AppSettings::instance().load();
}

// --- Instantiation ---

void TestHelpScreen::test_canInstantiate()
{
    HelpScreen screen;
    QVERIFY(screen.findChild<QPushButton*>("generalInfoButton"));
}

// --- Back button ---

void TestHelpScreen::test_backButton_exists()
{
    HelpScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("backButton");
    QVERIFY(btn);
    QVERIFY(btn->isEnabled());
}

void TestHelpScreen::test_backButton_emitsSignal()
{
    HelpScreen screen;
    QSignalSpy spy(&screen, &HelpScreen::backRequested);
    QPushButton* btn = screen.findChild<QPushButton*>("backButton");
    QVERIFY(btn);
    QTest::mouseClick(btn, Qt::LeftButton);
    QCOMPARE(spy.count(), 1);
}

// --- Help buttons exist ---

void TestHelpScreen::test_usersManualButton_exists()
{
    HelpScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("usersManualButton");
    QVERIFY(btn);
    QVERIFY(!btn->text().isEmpty());
}

void TestHelpScreen::test_generalInfoButton_exists()
{
    HelpScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("generalInfoButton");
    QVERIFY(btn);
    QVERIFY(btn->isEnabled());
    QVERIFY(!btn->text().isEmpty());
}

// --- Signals emitted ---

void TestHelpScreen::test_generalInfoButton_emitsSignal()
{
    HelpScreen screen;
    QSignalSpy spy(&screen, &HelpScreen::generalInfoRequested);
    QPushButton* btn = screen.findChild<QPushButton*>("generalInfoButton");
    QVERIFY(btn);
    QTest::mouseClick(btn, Qt::LeftButton);
    QCOMPARE(spy.count(), 1);
}

void TestHelpScreen::test_usersManualButton_emitsSignal()
{
    HelpScreen screen;
    // Ensure manual button is enabled (Spanish resource exists)
    AppSettings::instance().setLanguage(Languages::SPANISH);
    screen.show();
    QVERIFY(QTest::qWaitForWindowExposed(&screen));

    QSignalSpy spy(&screen, &HelpScreen::usersManualRequested);
    QPushButton* btn = screen.findChild<QPushButton*>("usersManualButton");
    QVERIFY(btn);

    if (btn->isEnabled())
    {
        QTest::mouseClick(btn, Qt::LeftButton);
        QCOMPARE(spy.count(), 1);
    }
    else
    {
        QSKIP("Manual button disabled (resource not embedded in test binary)");
    }
}

// --- Manual button enabled/disabled based on resource availability ---

void TestHelpScreen::test_manualButton_enabledForSpanish()
{
    AppSettings::instance().setLanguage(Languages::SPANISH);
    HelpScreen screen;
    screen.show();
    QVERIFY(QTest::qWaitForWindowExposed(&screen));

    QPushButton* btn = screen.findChild<QPushButton*>("usersManualButton");
    QVERIFY(btn);
    // Resource :/help/help/manual_es.html should exist (embedded via .qrc)
    QCOMPARE(btn->isEnabled(), true);
}

void TestHelpScreen::test_manualButton_enabledForEnglish()
{
    AppSettings::instance().setLanguage(Languages::ENGLISH);
    HelpScreen screen;
    screen.show();
    QVERIFY(QTest::qWaitForWindowExposed(&screen));

    QPushButton* btn = screen.findChild<QPushButton*>("usersManualButton");
    QVERIFY(btn);
    QCOMPARE(btn->isEnabled(), true);
}

void TestHelpScreen::test_manualButton_enabledForPolish()
{
    AppSettings::instance().setLanguage(Languages::POLISH);
    HelpScreen screen;
    screen.show();
    QVERIFY(QTest::qWaitForWindowExposed(&screen));

    QPushButton* btn = screen.findChild<QPushButton*>("usersManualButton");
    QVERIFY(btn);
    QCOMPARE(btn->isEnabled(), true);
}
