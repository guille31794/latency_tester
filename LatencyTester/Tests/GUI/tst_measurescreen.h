#ifndef TST_MEASURESCREEN_H
#define TST_MEASURESCREEN_H

#include <QObject>

class TestMeasureScreen : public QObject
{
    Q_OBJECT

private slots:
    // Instantiation
    void test_canInstantiate();

    // Back button
    void test_backButton_exists();
    void test_backButton_emitsSignal();

    // Measure buttons exist
    void test_calibrateButton_exists();
    void test_startMeasureButton_exists();
    void test_stopMeasureButton_exists();
    void test_stopMeasureButton_disabledByDefault();

    // Sliders exist and have correct range
    void test_timeFactorSlider_exists();
    void test_timeFactorSlider_range();
    void test_durationSlider_exists();
    void test_durationSlider_range();

    // Slider labels update on value change
    void test_timeFactorSlider_labelUpdates();
    void test_durationSlider_labelUpdates();

    // Graph widget exists
    void test_plotWidget_exists();
};

#endif // TST_MEASURESCREEN_H
