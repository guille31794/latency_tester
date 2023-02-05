#include "sensoroperator.h"
#include <QDebug>
#include <QTime>

SensorOperator::SensorOperator(): mStopMeasure{false}, mTakingMeasure{false}, mSensorReferenceCalibration{0}
{
    if (gpioInitialise() < 0)
    {
       qDebug() << "Fallo al inicializar gpio lib";
       this->~SensorOperator();
    }
    else
    {
        gpioSetMode(24, PI_OUTPUT); // Set GPIO24 as output for led.
        gpioSetMode(25, PI_INPUT); // Set GPIO25 as input for sensor.
        gpioSetMode(8, PI_OUTPUT); // Set GPIO8 as output for sensor.
    }
}

SensorOperator::~SensorOperator()
{
    gpioTerminate();
}

bool SensorOperator::isTakingMeasure() const
{
    return mTakingMeasure;
}

void SensorOperator::takeMeasure(Measures &registry)
{
    quint32 numberOfMeasuresToTake = registry.duration / registry.timeFactor;
    mTakingMeasure = true;
    mStopMeasure = false;
    QTime startTime;
    int measureTime{0};
    int measuredValue{0};

    switchOnSensor();

    for(quint32 measure = 0; measure < numberOfMeasuresToTake; ++measure)
    {
        if(mStopMeasure)
        {
            break;
        }

        startTime = QTime::currentTime();
        switchOnLed();

        do
        {
            measuredValue = readFromSensor();
        }while(measuredValue < (mSensorReferenceCalibration + mSensorReferenceCalibration * 0.2));

        measureTime = startTime.msecsTo(QTime::currentTime());
        switchOffLed();
        registry.lantencies.append(measureTime);
    }

    switchOffSensor();
    registry.meanLatency = meanLatency(registry.lantencies);
}

void SensorOperator::calibrateSensor()
{

}

void SensorOperator::stopMeasure()
{
    mStopMeasure = true;
}

void SensorOperator::switchOnLed()
{
    gpioWrite(24, 1); // Turn on the led.
}

void SensorOperator::switchOffLed()
{
    gpioWrite(24, 0); // Turn off the led
}

void SensorOperator::switchOnSensor()
{
    gpioWrite(8, 1); // Turn on the sensor
}

void SensorOperator::switchOffSensor()
{
    gpioWrite(8, 0); // Turn off the sensor
}

int SensorOperator::readFromSensor()
{
    return gpioRead(25);
}

double SensorOperator::meanLatency(QVector<double>& lantencies)
{
    double mean{0.0};

    for(int latency : lantencies)
    {
        mean += latency;
    }

    mean /= lantencies.size();
    return mean;
}
