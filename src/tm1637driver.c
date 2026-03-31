#include "tm1637driver.h"

#include "i2c_bitbang.h"


void tm1637DataCommand(bool const fixedAddress, bool const readKeyAndDontWriteDisplay)
{
    uint8_t const value = /*data command*/ 0x40 |
                           ((readKeyAndDontWriteDisplay ? 1 : 0) << 1) |
                           ((fixedAddress ? 1 : 0) << 2); // |
                           // ((/*test mode*/ false ? 1 : 0) << 3);

    i2cStart();
    uint8_t bytesWrittenSuccessfully = i2cWrite(&value, /*count*/ 1);
    i2cStop();

    #ifndef NDEBUG
    while (1 != bytesWrittenSuccessfully)
    {
    }
    #endif // NDEBUG
}

void tm1637AddressCommand(uint8_t const address, uint8_t const * data, uint8_t const count)
{
    #ifndef NDEBUG
    // No more than the first 4 bytes usable.
    while (4 < (count + address))
    {
    }
    #endif // NDEBUG

    uint8_t const value = /*address command*/ 0xc0 |
                           (address & 0x0f);

    i2cStart();
    uint8_t bytesWrittenSuccessfully = i2cWrite(&value, /*count*/ 1);

    #ifndef NDEBUG
    while (1 != bytesWrittenSuccessfully)
    {
    }
    #endif // NDEBUG


    bytesWrittenSuccessfully = i2cWrite(data, /*count*/ count);
    i2cStop();

    #ifndef NDEBUG
    while (count != bytesWrittenSuccessfully)
    {
    }
    #endif // NDEBUG
}

void tm1637DisplayCommand(bool const on, uint8_t const brightness)
{
    uint8_t const tm1637DisplayControl = /*display command*/ 0x80 |
                                         (brightness & 0x07) |
                                         ((on ? 1 : 0) << 3);

    i2cStart();
    uint8_t const bytesWrittenSuccessfully = i2cWrite(&tm1637DisplayControl, /*count*/ 1);
    i2cStop();

    #ifndef NDEBUG
    while (1 != bytesWrittenSuccessfully)
    {
    }
    #endif // NDEBUG
}
