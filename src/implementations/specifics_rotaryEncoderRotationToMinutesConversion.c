#include "../specifics.h"

#include "../auxiliaries.h"


// Limited to [0, max].
uint16_t rotaryEncoderRotationAppliedSexagesimal(uint16_t const value,
                                                 int8_t const rotation,
                                                 uint16_t const max)
{

    uint16_t const deltaValue = rotaryEncoderRotationToValueConversion(absoluteValue_int8(rotation));

    uint16_t adaptedValue = value;

    if (0 == rotation)
    {
        // adaptedMinutes = minutes;
    }
    else if (0 > rotation)
    {
        // subtract
        if (value > deltaValue)
        {
            adaptedValue = value - deltaValue;
        }
        else
        {
            adaptedValue = 0;
        }
    }
    else // if (0 < rotation)
    {
        // add
        if ((max - value) > deltaValue)
        {
            adaptedValue = value + deltaValue;
        }
        else
        {
            adaptedValue = max;
        }
    }

    return adaptedValue;
}
