// Sarah Yandell
// syandell@hmc.edu
// 2025-10-23
// main.h
// Description:  Public definitions for the main.c file. Declares LED pin, 
// buffer length, and includes STM32L432KC.h.

#ifndef MAIN_H
#define MAIN_H

#include "C:\Users\syandell\Desktop\lab06\lib\STM32L432KC.h"

#define LED_PIN PB6 // LED pin for blinking on Port B pin 6
#define BUFF_LEN 32 // UART receive buffer length for requests

#endif // MAIN_H