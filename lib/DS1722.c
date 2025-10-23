// Sarah Yandell
// syandell@hmc.edu
// 2025-10-23
// DS1722.c
// Description: SPI driver for the DS1722 digital thermometer. Provides
// initialization, resolution selection (8–12 bits), and temperature reads
// in Celsius. 

#include "DS1722.h"
#include "STM32L432KC_SPI.h"
#include "STM32L432KC_GPIO.h"

// Write one byte to the configuration register (0x00).
// Data is configuration byte for DS1722 .
// Assert CS, send write command (A7=1), send data byte, deassert CS.
static void ds1722_write_reg(uint8_t data) {
    digitalWrite(SPI_CE, 1);
    spiSendReceive((char)(0x80)); // A7=1 write
    spiSendReceive((char)data);
    digitalWrite(SPI_CE, 0);
    printf ("data: %d \n", data);
}


// DS1722_CONFIG_12BIT_CONT is 0xE2
// Initialize DS1722 at default 9-bit resolution.
// Ensures CS idles low and writes DS1722_DEFAULT_CONFIG_9BIT.
void ds1722_init(void) {
    // Ensure CS is low when idle
    digitalWrite(SPI_CE, 0);
    // Set inital 9-bit resolution
    ds1722_write_reg(DS1722_DEFAULT_CONFIG_9BIT);
}


// Set measurement resolution (valid: 8, 9, 10, 11, or 12 bits).
// Values outside 8–12 are ignored and no change is applied.
void ds1722_res(int res) {
    if (res < 8 || res > 12) {
        return; // ignore invalid
    }
    printf ("res: %d \n", res);
    uint8_t resConfig = 0xE2; // default 9-bit
    // Config bits: [1 1 1 1SHOT R2 R1 R0 SD]
    // SD=0, 1SHOT=0
    // res: 8=000, 9=001, 10=010, 11=011, 12=100
    if (res == 8)
        resConfig = 0xE0;
    else if (res == 9)
        resConfig = 0xE2;
    else if (res == 10)
        resConfig = 0xE4;
    else if (res == 11)
        resConfig = 0xE6;
    else if (res == 12)
        resConfig = 0xE8;
    ds1722_write_reg(resConfig);
}


// Read temperature in degrees Celsius.
// Reads LSB (0x01) and MSB (0x02). Converts to Celsius and returns it.
float ds1722_read_celsius(void) {
    // Read LSB
    digitalWrite(SPI_CE, 1);
    spiSendReceive((char)0x01);
    uint8_t lsb = (uint8_t) spiSendReceive((char)(0x00)); // A7=0 read
    digitalWrite(SPI_CE, 0);

    // Read MSB
    digitalWrite(SPI_CE, 1);
    spiSendReceive((char)0x02);
    uint8_t msb = (uint8_t) spiSendReceive((char)(0x00)); // A7=0 read
    digitalWrite(SPI_CE, 0);

    // Convert to Celsius
    printf("lsb: %d \n", lsb);
    printf("msb: %d \n", msb);
    int16_t raw = (int16_t)((msb << 8) | lsb);
    printf("raw: %d \n", raw);
    float a = (float)raw /256.0f;
    printf ("temp: %f \n", a);
    return ((float)raw) / 256.0f;
}
