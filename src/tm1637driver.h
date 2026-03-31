#ifndef TM1637_DRIVER_H
#define TM1637_DRIVER_H

#include <stdbool.h>
#include <stdint.h>


void tm1637DataCommand(bool const fixedAddress, bool const readKeyAndDontWriteDisplay);
void tm1637AddressCommand(uint8_t const address, uint8_t const * data, uint8_t const count);
void tm1637DisplayCommand(bool const on, uint8_t const brightness);

#endif // TM1637_DRIVER_H
