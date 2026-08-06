#include "tst_homescreen.h"
#include "homescreen.h"
#include <QtTest>
#include <QSignalSpy>
#include <QPushButton>

// --- Widget instantiation ---

void TestHomeScreen::test_canInstantiate()
{
    HomeScreen screen;
    QVERIFY(&screen != nullptr);
}

void TestHomeScreen::test_hasCorrectWindowTitle()
{
    HomeScreen screen;
    QCOMPARE(screen.windowTitle(), QString("Medidor de latencias"));
}

// --- Button signals ---

void TestHomeScreen::test_startMeasuringButton_emitsSignal()
{
    HomeScreen screen;
    QSignalSpy spy(&screen, &HomeScreen::startMeasuringRequested);

    QPushButton* btn = screen.findChild<QPushButton*>("startMeasuringButton");
    QVERIFY(btn != nullptr);
    QTest::mouseClick(btn, Qt::LeftButton);

    QCOMPARE(spy.count(), 1);
}

void TestHomeScreen::test_historicButton_emitsSignal()
{
    HomeScreen screen;
    QSignalSpy spy(&screen, &HomeScreen::historyRequested);

    QPushButton* btn = screen.findChild<QPushButton*>("historicOfMeasuresButton");
    QVERIFY(btn != nullptr);
    QTest::mouseClick(btn, Qt::LeftButton);

    QCOMPARE(spy.count(), 1);
}

void TestHomeScreen::test_settingsButton_emitsSignal()
{
    HomeScreen screen;
    QSignalSpy spy(&screen, &HomeScreen::settingsRequested);

    QPushButton* btn = screen.findChild<QPushButton*>("settingsButton");
    QVERIFY(btn != nullptr);
    QTest::mouseClick(btn, Qt::LeftButton);

    QCOMPARE(spy.count(), 1);
}

void TestHomeScreen::test_helpButton_emitsSignal()
{
    HomeScreen screen;
    QSignalSpy spy(&screen, &HomeScreen::helpRequested);

    QPushButton* btn = screen.findChild<QPushButton*>("helpButton");
    QVERIFY(btn != nullptr);
    QTest::mouseClick(btn, Qt::LeftButton);

    QCOMPARE(spy.count(), 1);
}

// --- Buttons exist and are enabled ---

void TestHomeScreen::test_startMeasuringButton_exists()
{
    HomeScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("startMeasuringButton");
    QVERIFY(btn != nullptr);
    QVERIFY(btn->isEnabled());
    QVERIFY(!btn->text().isEmpty());
}

void TestHomeScreen::test_historicButton_exists()
{
    HomeScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("historicOfMeasuresButton");
    QVERIFY(btn != nullptr);
    QVERIFY(btn->isEnabled());
    QVERIFY(!btn->text().isEmpty());
}

void TestHomeScreen::test_settingsButton_exists()
{
    HomeScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("settingsButton");
    QVERIFY(btn != nullptr);
    QVERIFY(btn->isEnabled());
    QVERIFY(!btn->text().isEmpty());
}

void TestHomeScreen::test_helpButton_exists()
{
    HomeScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("helpButton");
    QVERIFY(btn != nullptr);
    QVERIFY(btn->isEnabled());
    QVERIFY(!btn->text().isEmpty());
}
