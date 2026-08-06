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

bool SensorOperator::takeMeasure(Measures &registry)
{
    // Guard against invalid parameters
    if (registry.timeFactor <= 0 || registry.duration <= 0)
    {
        qDebug() << "takeMeasure: parámetros inválidos (timeFactor:" << registry.timeFactor
                 << ", duration:" << registry.duration << ")";
        return false;
    }

    quint32 numberOfMeasuresToTake = static_cast<quint32>(registry.duration / registry.timeFactor);
    if (numberOfMeasuresToTake == 0)
    {
        numberOfMeasuresToTake = 1;
    }
    mTakingMeasure = true;
    mStopMeasure = false;

    // Detection threshold: calibration baseline + 20%
    const int detectionThreshold = static_cast<int>(mSensorReferenceCalibration +
                                                     mSensorReferenceCalibration * 0.2);
    // Timeout per measurement to avoid infinite loops (5 seconds max)
    constexpr int measureTimeoutMs = 5000;

    switchOnSensor();

    for (quint32 measure = 0; measure < numberOfMeasuresToTake; ++measure)
    {
        if (mStopMeasure)
        {
            break;
        }

        // Ensure LED is off and sensor settled before starting
        switchOffLed();
        QTime settleEnd = QTime::currentTime().addMSecs(50);
        while (QTime::currentTime() < settleEnd) {}

        // Record start time and turn on LED (stimulus)
        QTime startTime = QTime::currentTime();
        switchOnLed();

        // Wait until sensor detects light above threshold (or timeout)
        int measuredValue = 0;
        bool detected = false;
        while (!detected && startTime.msecsTo(QTime::currentTime()) < measureTimeoutMs)
        {
            measuredValue = readFromSensor();
            if (measuredValue >= detectionThreshold)
            {
                detected = true;
            }
        }

        int measureTime = startTime.msecsTo(QTime::currentTime());
        switchOffLed();

        if (detected)
        {
            registry.lantencies.append(measureTime);
        }
        else
        {
            // Timeout: register as -1 to indicate failed measurement
            registry.lantencies.append(-1);
        }

        // Wait for the configured interval before next measurement
        if (measure < numberOfMeasuresToTake - 1 && !mStopMeasure)
        {
            QTime intervalEnd = QTime::currentTime().addMSecs(registry.timeFactor);
            while (QTime::currentTime() < intervalEnd && !mStopMeasure) {}
        }
    }

    switchOffSensor();
    mTakingMeasure = false;
    registry.meanLatency = meanLatency(registry.lantencies);

    // Signal measurement complete with quick blink
    quickBlink();

    // Success if at least one valid (>0) measurement was taken
    return registry.meanLatency > 0;
}

bool SensorOperator::calibrateSensor()
{
    // Reset stop flag at the start of each operation
    mStopMeasure = false;

    // Start the sensor acquisition
    switchOnSensor();

    // Blink LED at 1-second intervals during calibration (indicates "calibrating")
    // Only collect ambient readings when LED is OFF (baseline reference)
    float sum = 0.0f;
    int count = 0;
    constexpr int calibrationCycles = 4;
    constexpr int halfPeriodMs = 500;

    for (int i = 0; i < calibrationCycles; ++i)
    {
        if (mStopMeasure) break;

        // LED ON phase (visual feedback only, no readings taken)
        switchOnLed();
        QTime blinkEnd = QTime::currentTime().addMSecs(halfPeriodMs);
        while (QTime::currentTime() < blinkEnd && !mStopMeasure) {}

        if (mStopMeasure) { switchOffLed(); break; }

        // LED OFF phase (collect ambient light readings for calibration)
        switchOffLed();
        QTime offEnd = QTime::currentTime().addMSecs(halfPeriodMs);
        while (QTime::currentTime() < offEnd && !mStopMeasure)
        {
            float reading = mLastSensorReading.load();
            if (reading > 0.0f)
            {
                sum += reading;
                ++count;
            }
        }
    }

    switchOffSensor();

    // If stopped, return failure
    if (mStopMeasure)
    {
        qDebug() << "Calibración interrumpida por el usuario.";
        quickBlink();
        return false;
    }

    // Compute baseline from ambient readings
    bool success = false;
    if (count > 0)
    {
        mSensorReferenceCalibration = static_cast<quint32>((sum / count) * 1000.0f);
        qDebug() << "Calibración completada. Referencia:" << mSensorReferenceCalibration << "mV"
                 << "(" << count << "muestras)";
        success = true;
    }
    else
    {
        qDebug() << "Calibración fallida: no se recibieron lecturas del sensor.";
    }

    // Signal calibration complete with quick blink
    quickBlink();
    return success;
}

void SensorOperator::quickBlink(int blinks, int intervalMs)
{
    for (int i = 0; i < blinks; ++i)
    {
        switchOnLed();
        QTime onEnd = QTime::currentTime().addMSecs(intervalMs);
        while (QTime::currentTime() < onEnd) {}
        switchOffLed();
        QTime offEnd = QTime::currentTime().addMSecs(intervalMs);
        while (QTime::currentTime() < offEnd) {}
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
    double sum{0.0};
    int validCount{0};

    for (double latency : lantencies)
    {
        if (latency >= 0) // Ignore failed measurements (-1)
        {
            sum += latency;
            ++validCount;
        }
    }

    return validCount > 0 ? sum / validCount : 0.0;
}
