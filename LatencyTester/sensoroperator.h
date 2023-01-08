#ifndef SENSOROPERATOR_H
#define SENSOROPERATOR_H

#include "dataModel.hpp"
#include "pigpio.h"

class SensorOperator
{
public:
    SensorOperator();
    ~SensorOperator();
    bool isTakingMeasure() const;
    void takeMeasure(Measures& registry);
    void calibrateSensor();
private:
    bool mTakingMeasure;
};

#endif // SENSOROPERATOR_H
