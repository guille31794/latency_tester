#ifndef SENSOROPERATOR_H
#define SENSOROPERATOR_H

#include "dataModel.hpp"
#include "ads1115.h"

/**
 * @brief c like high level library to operate rpi gpio.
 * On non-RPi platforms, a stub is used instead.
 */
#ifdef RASPBERRY_PI
#include "pigpio.h"
#else
#include "pigpio_stub.h"
#endif

#include <atomic>

/**
 * @brief The SensorOperator class controls the logic behind the process of taking measures
 * and sensor calibration.
 */
class SensorOperator
{
public:
    /**
     * @brief SensorOperator constructor. If it's not possible to access gpio pins.
     */
    SensorOperator();

    /**
     * @brief ~SensorOperator destructor. It ends the connection with gpio daemon.
     */
    ~SensorOperator();

    /**
     * @brief isTakingMeasure answer if the device it's performing a measure.
     * @return true is taking a measure or false if not.
     */
    bool isTakingMeasure() const;

    /**
     * @brief takeMeasure takes a measure using the led and sensor.
     * @param registry where the measure data is stored.
     */
    void takeMeasure(Measures& registry);

    /**
     * @brief calibrateSensor calibrates the sensor to adapt it to environmental conditions.
     * Takes ambient light readings and establishes a baseline reference value.
     */
    void calibrateSensor();

    /**
     * @brief stopMeasure it stops the taking of a measure.
     */
    void stopMeasure();
private:
    /**
     * @brief switchOnLed switch on the led.
     */
    void switchOnLed();

    /**
     * @brief switchOffLed switch off the led.
     */
    void switchOffLed();

    /**
     * @brief switchOnSensor starts the ADS1115 data acquisition.
     */
    void switchOnSensor();

    /**
     * @brief switchOffSensor stops the ADS1115 data acquisition.
     */
    void switchOffSensor();

    /**
     * @brief readFromSensor reads the latest light value from the ADS1115.
     * @return voltage reading in millivolts.
     */
    int readFromSensor();

    /**
     * @brief meanLatency computes the mean of all latencies.
     * @param latencies latencies to compute the mean.
     * @return the mean of all latencies.
     */
    double meanLatency(QVector<double>& lantencies);

    bool mStopMeasure;
    bool mTakingMeasure;
    quint32 mSensorReferenceCalibration;

    ADS1115rpi mAdc;
    std::atomic<float> mLastSensorReading{0.0f};
};

#endif // SENSOROPERATOR_H
