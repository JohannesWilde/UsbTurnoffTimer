#include "../specifics.h"

#include "../auxiliaries.h"


// Limited to [0 hours, 24 hours].
uint16_t rotaryEncoderRotationAppliedToMinutes(uint16_t const minutes, int8_t const rotation)
{
    #define MAX_24HOURS_MINUTES ((uint16_t)(24) * 60)

    uint16_t const deltaMinutes = rotaryEncoderRotationToMinutesConversion(absoluteValue_int8(rotation));

    uint16_t adaptedMinutes = minutes;

    if (0 == rotation)
    {
        // adaptedMinutes = minutes;
    }
    else if (0 > rotation)
    {
        // subtract
        if (minutes > deltaMinutes)
        {
            adaptedMinutes = minutes - deltaMinutes;
        }
        else
        {
            adaptedMinutes = 0;
        }
    }
    else // if (0 < rotation)
    {
        // add
        if ((MAX_24HOURS_MINUTES - minutes) > deltaMinutes)
        {
            adaptedMinutes = minutes + deltaMinutes;
        }
        else
        {
            adaptedMinutes = MAX_24HOURS_MINUTES;
        }
    }

    return adaptedMinutes;
}
