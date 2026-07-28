#include "sensoroperator.h"
#include <QDebug>
#include <QTime>

SensorOperator::SensorOperator(): mStopMeasure{false}, mTakingMeasure{false}, mSensorReferenceCalibration{0}
{
    if (gpioInitialise() < 0)
    {
       qDebug() << "Fallo al inicializar gpio lib";
    }
    else
    {
        gpioSetMode(24, PI_OUTPUT); // Set GPIO24 as output for led.
    }

    // Register ADS1115 callback to update the latest sensor reading
    mAdc.registerCallback([this](float voltage) {
        mLastSensorReading.store(voltage);
    });
}

SensorOperator::~SensorOperator()
{
    mAdc.stop();
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
        } while(measuredValue < static_cast<int>(mSensorReferenceCalibration + mSensorReferenceCalibration * 0.2));

        measureTime = startTime.msecsTo(QTime::currentTime());
        switchOffLed();
        registry.lantencies.append(measureTime);
    }

    switchOffSensor();
    mTakingMeasure = false;
    registry.meanLatency = meanLatency(registry.lantencies);
}

void SensorOperator::calibrateSensor()
{
    // Start the sensor, take readings for a short period, and compute the baseline
    switchOnSensor();

    // Allow the sensor to stabilize (collect samples for ~2 seconds)
    QTime calibrationEnd = QTime::currentTime().addSecs(2);
    float sum = 0.0f;
    int count = 0;

    while (QTime::currentTime() < calibrationEnd)
    {
        float reading = mLastSensorReading.load();
        if (reading > 0.0f)
        {
            sum += reading;
            ++count;
        }
    }

    switchOffSensor();

    if (count > 0)
    {
        // Store calibration as millivolts (baseline ambient light level)
        mSensorReferenceCalibration = static_cast<quint32>((sum / count) * 1000.0f);
        qDebug() << "Calibración completada. Referencia:" << mSensorReferenceCalibration << "mV";
    }
    else
    {
        qDebug() << "Calibración fallida: no se recibieron lecturas del sensor.";
    }
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
    ADS1115settings settings;
    settings.samplingRate = ADS1115settings::FS860HZ; // Max speed for low-latency detection
    settings.channel = ADS1115settings::AIN0;         // Photosensor connected to AIN0
    settings.pgaGain = ADS1115settings::FSR2_048;     // 2.048V full scale (suitable for OPT101 output)
    mAdc.start(settings);
}

void SensorOperator::switchOffSensor()
{
    mAdc.stop();
}

int SensorOperator::readFromSensor()
{
    // Convert voltage (float, in volts) to millivolts (int)
    return static_cast<int>(mLastSensorReading.load() * 1000.0f);
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
