#include "sensoroperator.h"
#include <QDebug>

SensorOperator::SensorOperator(): mTakingMeasure{false}
{
    if (gpioInitialise() < 0)
    {
       qDebug() << "Fallo al inicializar gpio lib";
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
    Q_UNUSED(registry);
}

void SensorOperator::calibrateSensor()
{

}
