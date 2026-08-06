#include "tst_measurescreen.h"
#include "measurescreen.h"
#include <QtTest>
#include <QSignalSpy>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

// --- Instantiation ---

void TestMeasureScreen::test_canInstantiate()
{
    MeasureScreen screen;
    // Verify widget was constructed and has child widgets
    QVERIFY(screen.findChild<QPushButton*>("calibrateButton"));
}

// --- Back button ---

void TestMeasureScreen::test_backButton_exists()
{
    MeasureScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("backButton");
    QVERIFY(btn);
    QVERIFY(btn->isEnabled());
}

void TestMeasureScreen::test_backButton_emitsSignal()
{
    MeasureScreen screen;
    QSignalSpy spy(&screen, &MeasureScreen::backRequested);

    QPushButton* btn = screen.findChild<QPushButton*>("backButton");
    QVERIFY(btn);
    QTest::mouseClick(btn, Qt::LeftButton);

    QCOMPARE(spy.count(), 1);
}

// --- Measure buttons ---

void TestMeasureScreen::test_calibrateButton_exists()
{
    MeasureScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("calibrateButton");
    QVERIFY(btn);
    QVERIFY(btn->isEnabled());
    QVERIFY(!btn->text().isEmpty());
}

void TestMeasureScreen::test_startMeasureButton_exists()
{
    MeasureScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("startMeasureButton");
    QVERIFY(btn);
    QVERIFY(btn->isEnabled());
    QVERIFY(!btn->text().isEmpty());
}

void TestMeasureScreen::test_stopMeasureButton_exists()
{
    MeasureScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("stopMeasureButton");
    QVERIFY(btn);
    QVERIFY(!btn->text().isEmpty());
}

void TestMeasureScreen::test_stopMeasureButton_disabledByDefault()
{
    MeasureScreen screen;
    QPushButton* btn = screen.findChild<QPushButton*>("stopMeasureButton");
    QVERIFY(btn);
    QCOMPARE(btn->isEnabled(), false);
}

// --- Sliders ---

void TestMeasureScreen::test_timeFactorSlider_exists()
{
    MeasureScreen screen;
    QSlider* slider = screen.findChild<QSlider*>("TimeFactorSlider");
    QVERIFY(slider);
    QVERIFY(slider->isEnabled());
}

void TestMeasureScreen::test_timeFactorSlider_range()
{
    MeasureScreen screen;
    QSlider* slider = screen.findChild<QSlider*>("TimeFactorSlider");
    QVERIFY(slider);
    QCOMPARE(slider->minimum(), 100);
    QCOMPARE(slider->maximum(), 2000);
}

void TestMeasureScreen::test_durationSlider_exists()
{
    MeasureScreen screen;
    QSlider* slider = screen.findChild<QSlider*>("DurationSlider");
    QVERIFY(slider);
    QVERIFY(slider->isEnabled());
}

void TestMeasureScreen::test_durationSlider_range()
{
    MeasureScreen screen;
    QSlider* slider = screen.findChild<QSlider*>("DurationSlider");
    QVERIFY(slider);
    QCOMPARE(slider->minimum(), 10);
    QCOMPARE(slider->maximum(), 600);
}

// --- Slider labels ---

void TestMeasureScreen::test_timeFactorSlider_labelUpdates()
{
    MeasureScreen screen;
    QSlider* slider = screen.findChild<QSlider*>("TimeFactorSlider");
    QLabel* label = screen.findChild<QLabel*>("timeFactorLabel");
    QVERIFY(slider);
    QVERIFY(label);

    slider->setValue(500);
    QVERIFY(label->text().contains("500"));
}

void TestMeasureScreen::test_durationSlider_labelUpdates()
{
    MeasureScreen screen;
    QSlider* slider = screen.findChild<QSlider*>("DurationSlider");
    QLabel* label = screen.findChild<QLabel*>("DurationLabel");
    QVERIFY(slider);
    QVERIFY(label);

    slider->setValue(30);
    QVERIFY(label->text().contains("30"));
}

// --- Graph ---

void TestMeasureScreen::test_plotWidget_exists()
{
    MeasureScreen screen;
    QWidget* plot = screen.findChild<QWidget*>("plotMeasures");
    QVERIFY(plot);
}
