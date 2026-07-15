#ifndef ADS1115_H
#define ADS1115_H

/**
 * @brief Wrapper header for the ADS1115 ADC library.
 * On Raspberry Pi, uses the real rpi_ads1115 library (I2C + GPIO).
 * On Desktop, uses a stub with no-op implementations for compilation.
 */

#ifdef RASPBERRY_PI
#include "ads1115rpi.h"
#else
#include "ads1115rpi_stub.h"
#endif

#endif // ADS1115_H
