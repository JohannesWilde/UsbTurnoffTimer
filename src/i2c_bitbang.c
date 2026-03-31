#include "i2c_bitbang.h"

#include "configuration.h"
#include "pinout.h"
#include "static_assert.h"
#include "stc8g.h"

#include <stdbool.h>


// Bit-banging I2C interface [send only for now].

inline void i2cDelay_()
{
    COMPILE_TIME_ASSERT((2000000 == F_CPU));
    // Fast [5 us pulse width for 50:50 duty cycle pulses -> period 2-times as long].
    NOP();
    NOP();
    NOP();
}

void i2cStart()
{
    // CLK high, DIO high -> low
    SEVEN_SEGMENT_DATA_PIN = 0;
    i2cDelay_();
}

void i2cStop()
{
    // To end acknowledgement from TM1637 another CLK pulse seems to be required before stop.
    SEVEN_SEGMENT_CLK_PIN = 0;

    // Prepare DATA for stop-condition while CLK LOW.
    SEVEN_SEGMENT_DATA_PIN = 0;

    i2cDelay_();
    SEVEN_SEGMENT_CLK_PIN = 1;
    i2cDelay_();

    // Actual stop:  CLK high, DIO low -> high
    SEVEN_SEGMENT_DATA_PIN = 1;
    i2cDelay_();
}

uint8_t i2cWrite(uint8_t const * const data, uint8_t const count)
{
    uint8_t numberOfBytesSuccessfullyWritten = 0;
    while (count > numberOfBytesSuccessfullyWritten)
    {
        uint8_t datum = data[numberOfBytesSuccessfullyWritten];
        uint8_t numberBitsToSend = 8;
        do
        {
            // pull down CLK
            SEVEN_SEGMENT_CLK_PIN = 0;

            // set bit
            SEVEN_SEGMENT_DATA_PIN = datum & 0x01; // LSb first

            // CLK-low-pulse width
            i2cDelay_();

            // release CLK up
            SEVEN_SEGMENT_CLK_PIN = 1;

            // Shift datum right 1 bit [do so at the HIGH CLK, as we set DATA during LOW CLK].
            datum >>= 1;

            // Let the counterpart sample the data.
            i2cDelay_();

            --numberBitsToSend;
        }
        while (0 < numberBitsToSend);

        // release data line
        SEVEN_SEGMENT_DATA_PIN = 1;
        // Read in ACK.
        SEVEN_SEGMENT_CLK_PIN = 0;
        i2cDelay_();
        SEVEN_SEGMENT_CLK_PIN = 1;
        i2cDelay_();

        uint8_t const ack = SEVEN_SEGMENT_DATA_PIN;

        bool const successfullySentByte = (0 == ack);

        if (successfullySentByte)
        {
            ++numberOfBytesSuccessfullyWritten;
        }
        else
        {
            break;
        }
    }
    return numberOfBytesSuccessfullyWritten;
}
