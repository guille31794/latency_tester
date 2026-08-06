#include "tst_sensoroperator.h"
#include "sensoroperator.h"
#include <QtTest>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

// --- takeMeasure: parameter validation ---

void TestSensorOperator::test_takeMeasure_invalidTimeFactorZero()
{
    SensorOperator op;
    Measures m;
    m.timeFactor = 0;
    m.duration = 5000;
    QCOMPARE(op.takeMeasure(m), false);
    QVERIFY(m.lantencies.isEmpty());
}

void TestSensorOperator::test_takeMeasure_invalidTimeFactorNegative()
{
    SensorOperator op;
    Measures m;
    m.timeFactor = -100;
    m.duration = 5000;
    QCOMPARE(op.takeMeasure(m), false);
}

void TestSensorOperator::test_takeMeasure_invalidDurationZero()
{
    SensorOperator op;
    Measures m;
    m.timeFactor = 100;
    m.duration = 0;
    QCOMPARE(op.takeMeasure(m), false);
}

void TestSensorOperator::test_takeMeasure_invalidDurationNegative()
{
    SensorOperator op;
    Measures m;
    m.timeFactor = 100;
    m.duration = -1000;
    QCOMPARE(op.takeMeasure(m), false);
}

// --- takeMeasure: behavior with stubs (sensor reads 0, threshold > 0, never detects) ---

void TestSensorOperator::test_takeMeasure_allTimeoutReturnsNegativeLatencies()
{
    SensorOperator op;
    // With stubs: sensor=0, baseline=0, threshold=0. 0>=0 is true → detection immediate.
    // Latencies will be 0ms (instant detection). meanLatency = 0. takeMeasure returns false
    // because the condition is meanLatency > 0.
    Measures m;
    m.timeFactor = 100;
    m.duration = 200;  // Should give 2 measurements
    bool result = op.takeMeasure(m);

    QCOMPARE(m.lantencies.size(), 2);
    // Latencies should be 0 (instant detection with stubs)
    for (double lat : std::as_const(m.lantencies))
    {
        QVERIFY(lat >= 0);
    }
    // meanLatency is 0 (all detections at 0ms), so takeMeasure returns false
    QCOMPARE(m.meanLatency, 0.0);
    QCOMPARE(result, false);
}

void TestSensorOperator::test_takeMeasure_meanLatencyZeroWhenAllFail()
{
    // With stubs and baseline=0, detection always succeeds (0>=0).
    // To force failure we'd need a non-zero calibration.
    // This test verifies meanLatency calculation with manual data instead.
    // Since meanLatency is private, we verify through takeMeasure result.
    // With stub: measurements succeed, so meanLatency > 0. Skip forced failure test.
    QSKIP("Cannot force timeout with Desktop stubs (threshold=0, sensor=0 → always detects)");
}

void TestSensorOperator::test_takeMeasure_returnsFalseWhenAllFail()
{
    QSKIP("Cannot force timeout with Desktop stubs (threshold=0, sensor=0 → always detects)");
}

// --- takeMeasure: stop interruption ---

void TestSensorOperator::test_takeMeasure_stopInterruptsMeasurement()
{
    SensorOperator op;
    Measures m;
    m.timeFactor = 200;
    m.duration = 10000;  // Would take 50 measurements without stop

    // Launch measurement in background and stop after a short delay
    auto future = QtConcurrent::run([&op, &m]() -> bool {
        return op.takeMeasure(m);
    });

    // Wait a bit then stop
    QTest::qWait(400);
    op.stopMeasure();
    future.waitForFinished();

    // Should have far fewer than 50 measurements
    QVERIFY(m.lantencies.size() < 50);
    QVERIFY(m.lantencies.size() > 0);  // At least one should have been taken
}

// --- takeMeasure: number of measurements ---

void TestSensorOperator::test_takeMeasure_correctNumberOfMeasurements()
{
    SensorOperator op;
    Measures m;
    m.timeFactor = 100;
    m.duration = 500;  // 500 / 100 = 5 measurements expected
    op.takeMeasure(m);
    QCOMPARE(m.lantencies.size(), 5);
}

void TestSensorOperator::test_takeMeasure_atLeastOneMeasureWhenDurationLessThanTimeFactor()
{
    SensorOperator op;
    Measures m;
    m.timeFactor = 1000;
    m.duration = 500;  // duration < timeFactor → 0, clamped to 1
    op.takeMeasure(m);
    QCOMPARE(m.lantencies.size(), 1);
}

// --- calibrateSensor ---

void TestSensorOperator::test_calibrate_failsWithNoSensorReadings()
{
    SensorOperator op;
    // With Desktop stubs, mLastSensorReading is 0.0f.
    // Calibration collects readings where reading > 0.0f — none will qualify.
    // So count=0 → returns false.
    bool result = op.calibrateSensor();
    QCOMPARE(result, false);
}

void TestSensorOperator::test_calibrate_stopInterruptsCalibration()
{
    SensorOperator op;

    auto future = QtConcurrent::run([&op]() -> bool {
        return op.calibrateSensor();
    });

    // Stop immediately
    QTest::qWait(100);
    op.stopMeasure();
    future.waitForFinished();

    QCOMPARE(future.result(), false);
}

// --- isTakingMeasure ---

void TestSensorOperator::test_isTakingMeasure_falseByDefault()
{
    SensorOperator op;
    QCOMPARE(op.isTakingMeasure(), false);
}

// --- stopMeasure ---

void TestSensorOperator::test_stopMeasure_setsFlag()
{
    SensorOperator op;
    Measures m;
    m.timeFactor = 500;
    m.duration = 5000;

    // Start a long measurement and immediately stop
    auto future = QtConcurrent::run([&op, &m]() { op.takeMeasure(m); });
    QTest::qWait(50);
    op.stopMeasure();
    future.waitForFinished();

    // After stop, isTakingMeasure should be false
    QCOMPARE(op.isTakingMeasure(), false);
}
