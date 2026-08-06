#ifndef TST_SENSOROPERATOR_H
#define TST_SENSOROPERATOR_H

#include <QObject>

class TestSensorOperator : public QObject
{
    Q_OBJECT

private slots:
    // takeMeasure - parameter validation
    void test_takeMeasure_invalidTimeFactorZero();
    void test_takeMeasure_invalidTimeFactorNegative();
    void test_takeMeasure_invalidDurationZero();
    void test_takeMeasure_invalidDurationNegative();

    // takeMeasure - behavior with stubs (sensor reads 0, never detects)
    void test_takeMeasure_allTimeoutReturnsNegativeLatencies();
    void test_takeMeasure_meanLatencyZeroWhenAllFail();
    void test_takeMeasure_returnsFalseWhenAllFail();

    // takeMeasure - stop interruption
    void test_takeMeasure_stopInterruptsMeasurement();

    // takeMeasure - number of measurements calculated correctly
    void test_takeMeasure_correctNumberOfMeasurements();
    void test_takeMeasure_atLeastOneMeasureWhenDurationLessThanTimeFactor();

    // calibrateSensor - with stubs (no real sensor readings)
    void test_calibrate_failsWithNoSensorReadings();

    // calibrateSensor - stop interruption
    void test_calibrate_stopInterruptsCalibration();

    // isTakingMeasure
    void test_isTakingMeasure_falseByDefault();

    // stopMeasure
    void test_stopMeasure_setsFlag();
};

#endif // TST_SENSOROPERATOR_H
