#ifndef PIGPIO_STUB_H
#define PIGPIO_STUB_H

/**
 * @brief Stub header for pigpio library.
 * Used when compiling on non-Raspberry Pi platforms (Windows, x86 Linux)
 * to allow building and testing the UI and logic without real GPIO hardware.
 *
 * All GPIO calls return safe default values.
 */

#include <cstdint>

// pigpio constants
#define PI_OUTPUT 1
#define PI_INPUT  0

// Stub implementations (inline to avoid linker issues)
inline int gpioInitialise()    { return 0; }  // Success
inline void gpioTerminate()    {}
inline int gpioSetMode(unsigned, unsigned) { return 0; }
inline int gpioWrite(unsigned, unsigned)   { return 0; }
inline int gpioRead(unsigned)              { return 0; }

#endif // PIGPIO_STUB_H
