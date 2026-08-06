#include "tst_registrydisplayscreen.h"
#include "registrydisplayscreen.h"
#include "qcustomplot.h"
#include <QtTest>
#include <QSignalSpy>
#include <QPushButton>
#include <QLabel>
#include <QDateTimeEdit>

// --- Instantiation ---

void TestRegistryDisplayScreen::test_canInstantiate()
{
    RegistryDisplayScreen screen;
    QVERIFY(screen.findChild<QWidget*>("plotMeasures"));
}

// --- Back button ---

void TestRegistryDisplayScreen::test_backButton_exists()
{
    RegistryDisplayScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("backButton");
    QVERIFY(btn);
    QVERIFY(btn->isEnabled());
}

void TestRegistryDisplayScreen::test_backButton_emitsSignal()
{
    RegistryDisplayScreen screen;
    QSignalSpy spy(&screen, &RegistryDisplayScreen::backRequested);
    QPushButton* btn = screen.findChild<QPushButton*>("backButton");
    QVERIFY(btn);
    QTest::mouseClick(btn, Qt::LeftButton);
    QCOMPARE(spy.count(), 1);
}

// --- Data widgets exist ---

void TestRegistryDisplayScreen::test_nameLabel_exists()
{
    RegistryDisplayScreen screen;
    QLabel* lbl = screen.findChild<QLabel*>("nameText");
    QVERIFY(lbl);
}

void TestRegistryDisplayScreen::test_dateTimeEdit_exists()
{
    RegistryDisplayScreen screen;
    QDateTimeEdit* dte = screen.findChild<QDateTimeEdit*>("dateTimeEdit");
    QVERIFY(dte);
    QCOMPARE(dte->isReadOnly(), true);
}

void TestRegistryDisplayScreen::test_latencyText_exists()
{
    RegistryDisplayScreen screen;
    QLabel* lbl = screen.findChild<QLabel*>("latencyText");
    QVERIFY(lbl);
}

void TestRegistryDisplayScreen::test_timeText_exists()
{
    RegistryDisplayScreen screen;
    QLabel* lbl = screen.findChild<QLabel*>("timeText");
    QVERIFY(lbl);
}

// --- Plot widget ---

void TestRegistryDisplayScreen::test_plotWidget_exists()
{
    RegistryDisplayScreen screen;
    QCustomPlot* plot = screen.findChild<QCustomPlot*>("plotMeasures");
    QVERIFY(plot);
}

// --- displayMeasure populates data ---

void TestRegistryDisplayScreen::test_displayMeasure_setsName()
{
    RegistryDisplayScreen screen;
    Measures m;
    m.name = "TestMedicion";
    m.date = QDateTime::fromString("2026-02-10T14:00:00", Qt::ISODate);
    m.meanLatency = 45.5;
    m.timeFactor = 300;
    m.duration = 5000;
    m.lantencies = {40.0, 45.0, 51.0};

    screen.displayMeasure(m);

    QLabel* lbl = screen.findChild<QLabel*>("nameText");
    QVERIFY(lbl);
    QCOMPARE(lbl->text(), QString("TestMedicion"));
}

void TestRegistryDisplayScreen::test_displayMeasure_setsDate()
{
    RegistryDisplayScreen screen;
    Measures m;
    m.name = "DateTest";
    m.date = QDateTime::fromString("2026-06-15T09:30:00", Qt::ISODate);
    m.meanLatency = 50.0;
    m.timeFactor = 200;
    m.duration = 4000;
    m.lantencies = {50.0};

    screen.displayMeasure(m);

    QDateTimeEdit* dte = screen.findChild<QDateTimeEdit*>("dateTimeEdit");
    QVERIFY(dte);
    QCOMPARE(dte->dateTime(), m.date);
}

void TestRegistryDisplayScreen::test_displayMeasure_setsLatency()
{
    RegistryDisplayScreen screen;
    Measures m;
    m.name = "LatencyTest";
    m.date = QDateTime::currentDateTime();
    m.meanLatency = 123.4;
    m.timeFactor = 500;
    m.duration = 10000;
    m.lantencies = {120.0, 125.0, 125.2};

    screen.displayMeasure(m);

    QLabel* lbl = screen.findChild<QLabel*>("latencyText");
    QVERIFY(lbl);
    QVERIFY(lbl->text().contains("123.4"));
}

void TestRegistryDisplayScreen::test_displayMeasure_setsTimeFactor()
{
    RegistryDisplayScreen screen;
    Measures m;
    m.name = "TimeFactorTest";
    m.date = QDateTime::currentDateTime();
    m.meanLatency = 60.0;
    m.timeFactor = 750;
    m.duration = 8000;
    m.lantencies = {60.0};

    screen.displayMeasure(m);

    QLabel* lbl = screen.findChild<QLabel*>("timeText");
    QVERIFY(lbl);
    QCOMPARE(lbl->text(), QString("750"));
}

void TestRegistryDisplayScreen::test_displayMeasure_plotsGraph()
{
    RegistryDisplayScreen screen;
    Measures m;
    m.name = "PlotTest";
    m.date = QDateTime::currentDateTime();
    m.meanLatency = 80.0;
    m.timeFactor = 200;
    m.duration = 2000;
    m.lantencies = {75.0, 80.0, 85.0, 78.0, 82.0};

    screen.displayMeasure(m);

    QCustomPlot* plot = screen.findChild<QCustomPlot*>("plotMeasures");
    QVERIFY(plot);
    QCOMPARE(plot->graphCount(), 1);
    // Graph should have 5 data points (all valid, > 0)
    QCOMPARE(plot->graph(0)->dataCount(), 5);
}

// --- Edge cases ---

void TestRegistryDisplayScreen::test_displayMeasure_emptyLatencies()
{
    RegistryDisplayScreen screen;
    Measures m;
    m.name = "EmptyTest";
    m.date = QDateTime::currentDateTime();
    m.meanLatency = 0;
    m.timeFactor = 100;
    m.duration = 1000;
    m.lantencies = {};  // empty

    screen.displayMeasure(m);

    QCustomPlot* plot = screen.findChild<QCustomPlot*>("plotMeasures");
    QVERIFY(plot);
    // Should not crash, and graph count should be 0 (plotMeasure returns early)
    QCOMPARE(plot->graphCount(), 0);
}
