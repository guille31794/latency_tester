#ifndef SENSOROPERATOR_H
#define SENSOROPERATOR_H

#include "dataModel.hpp"

/**
 * @brief c like high level library to operate rpi gpio.
 */
#include "pigpio.h"

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
     * @brief calibrateSensor calibrates the sensor to adapt it to environmental conditions
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
     * @brief switchOnSensor allows the sensor to start reading.
     */
    void switchOnSensor();

    /**
     * @brief switchOffSensor switches off the sensor.
     */
    void switchOffSensor();

    /**
     * @brief readFromSensor reads a light value from sensor.
     * @return value read from sensor.
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
};

#endif // SENSOROPERATOR_H
