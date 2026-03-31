#include "../specifics.h"


uint8_t rotaryEncoderRotationToMinutesConversion(uint8_t const rotation)
{
    uint8_t minutes = 0;

    if (0 == rotation)
    {
        // minutes = 0;
    }
    else if (3 > rotation)
    {
        minutes = 1;
    }
    else if (5 > rotation)
    {
        minutes = 5;
    }
    else if (7 > rotation)
    {
        minutes = 15;
    }
    else if (9 > rotation)
    {
        minutes = 30;
    }
    else if (11 > rotation)
    {
        minutes = 60;
    }
    else
    {
        minutes = 240;
    }

    return minutes;
}
