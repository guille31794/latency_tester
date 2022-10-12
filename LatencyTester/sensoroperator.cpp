#include "sensoroperator.h"

SensorOperator::SensorOperator(): mTakingMeasure{false}
{}

bool SensorOperator::isTakingMeasure() const
{
    return mTakingMeasure;
}

void SensorOperator::takeMeasure(Measures &registry)
{

}

void SensorOperator::calibrateSensor()
{

}
