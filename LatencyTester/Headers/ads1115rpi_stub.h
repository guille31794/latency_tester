#ifndef ADS1115RPI_STUB_H
#define ADS1115RPI_STUB_H

/**
 * @brief Stub header for rpi_ads1115 library.
 * Used when compiling on non-Raspberry Pi platforms (Windows, x86 Linux)
 * to allow building and testing the UI without real I2C/GPIO hardware.
 *
 * Provides the same API surface as ads1115rpi.h but with no-op implementations.
 */

#include <cstdint>
#include <functional>

struct ADS1115settings
{
    int i2c_bus = 1;
    static constexpr uint8_t DEFAULT_ADS1115_ADDRESS = 0x48;
    uint8_t address = DEFAULT_ADS1115_ADDRESS;

    enum SamplingRates
    {
        FS8HZ = 0,
        FS16HZ = 1,
        FS32HZ = 2,
        FS64HZ = 3,
        FS128HZ = 4,
        FS250HZ = 5,
        FS475HZ = 6,
        FS860HZ = 7
    };

    inline unsigned getSamplingRate() const
    {
        const unsigned SamplingRateEnum2Value[8] =
            {8, 16, 32, 64, 128, 250, 475, 860};
        return SamplingRateEnum2Value[samplingRate];
    }

    SamplingRates samplingRate = FS8HZ;

    enum PGA
    {
        FSR2_048 = 2,
        FSR1_024 = 3,
        FSR0_512 = 4,
        FSR0_256 = 5
    };

    PGA pgaGain = FSR2_048;

    enum Input
    {
        AIN0 = 0,
        AIN1 = 1,
        AIN2 = 2,
        AIN3 = 3
    };

    Input channel = AIN0;
    int drdy_chip = 0;
    static constexpr int DEFAULT_ALERT_RDY_TO_GPIO = 17;
    int drdy_gpio = DEFAULT_ALERT_RDY_TO_GPIO;
};

class ADS1115rpi
{
public:
    ~ADS1115rpi() {}

    using ADSCallbackInterface = std::function<void(float)>;

    void registerCallback(ADSCallbackInterface) {}
    void setChannel(ADS1115settings::Input) {}
    void start(ADS1115settings = ADS1115settings()) {}
    ADS1115settings getADS1115settings() const { return {}; }
    void stop() {}
};

#endif // ADS1115RPI_STUB_H
