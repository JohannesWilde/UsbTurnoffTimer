#ifndef I2C_BITBANG_H
#define I2C_BITBANG_H

#include <stdint.h>


void i2cStart();
void i2cStop();
uint8_t i2cWrite(uint8_t const * const data, uint8_t const count);


#endif // I2C_BITBANG_H
