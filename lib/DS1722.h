// Sarah Yandell
// syandell@hmc.edu
// 2025-10-23
// DS1722.h
// Description:  Public definitions for the DS1722.c file. Declares
// initialization, resolution selection, and Celsius temperature readout.

#ifndef DS1722_H
#define DS1722_H
#include <stdint.h>
#include "STM32L432KC.h"

// Default configuration: continuous conversion, 9-bit resolution.
#define DS1722_DEFAULT_CONFIG_9BIT  0xE2

// Initialize DS1722 at default (9-bit) resolution.
void ds1722_init(void);
// Read the current temperature in degrees Celsius.
float ds1722_read_celsius(void);
// Set the DS1722 conversion resolution. Valid values: 8, 9, 10, 11,12. Others ignored.
void ds1722_res(int res);

#endif // DS1722_H






