#ifndef TST_REGISTRYDISPLAYSCREEN_H
#define TST_REGISTRYDISPLAYSCREEN_H

#include <QObject>

class TestRegistryDisplayScreen : public QObject
{
    Q_OBJECT

private slots:
    // Instantiation
    void test_canInstantiate();

    // Back button
    void test_backButton_exists();
    void test_backButton_emitsSignal();

    // Data widgets exist
    void test_nameLabel_exists();
    void test_dateTimeEdit_exists();
    void test_latencyText_exists();
    void test_timeText_exists();

    // Plot widget
    void test_plotWidget_exists();

    // displayMeasure populates data
    void test_displayMeasure_setsName();
    void test_displayMeasure_setsDate();
    void test_displayMeasure_setsLatency();
    void test_displayMeasure_setsTimeFactor();
    void test_displayMeasure_plotsGraph();

    // Edge cases
    void test_displayMeasure_emptyLatencies();
};

#endif // TST_REGISTRYDISPLAYSCREEN_H
