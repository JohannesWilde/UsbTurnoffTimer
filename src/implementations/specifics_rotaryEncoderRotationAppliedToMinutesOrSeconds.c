#include "../specifics.h"



void rotaryEncoderRotationAppliedToMinutesOrSeconds(MinutesOrSeconds * const value, int8_t const rotation)
{
    if (value->minutesNotSeconds)
    {
        value->value =
            rotaryEncoderRotationAppliedSexagesimal(value->value, rotation, MAX_24HOURS_MINUTES);
        if (0 == value->value)
        {
            value->minutesNotSeconds = false;
            value->value = 59;
        }
        else
        {
            // intentionally empty
        }
    }
    else
    {
        value->value =
            rotaryEncoderRotationAppliedSexagesimal(value->value, rotation, 60);
        if (60 == value->value)
        {
            value->minutesNotSeconds = true;
            value->value = 1;
        }
        else
        {
            // intentionally empty
        }
    }
}
