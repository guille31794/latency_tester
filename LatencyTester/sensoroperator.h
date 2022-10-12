#ifndef SENSOROPERATOR_H
#define SENSOROPERATOR_H

#include "dataModel.hpp"

class SensorOperator
{
public:
    SensorOperator();
    bool isTakingMeasure() const;
    void takeMeasure(Measures& registry);
    void calibrateSensor();
private:
    bool mTakingMeasure;
};

#endif // SENSOROPERATOR_H
