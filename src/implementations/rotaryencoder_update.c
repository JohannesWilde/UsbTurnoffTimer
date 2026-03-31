#include "../rotaryencoder.h"

#include <limits.h>


void rotaryEncoderUpdate(RotaryEncoder * rotaryEncoder, bool const stateA, bool const stateB)
{
    bool const toggledA = (stateA != rotaryEncoder->stateAPrevious);
    bool const toggledB = (stateB != rotaryEncoder->stateBPrevious);
    if (false && toggledA && toggledB)
    {
        rotaryEncoder->stateAPrevious = stateA;
        rotaryEncoder->stateBPrevious = stateB;

        // I must have missed something, but I can't know what. So do nothing else.
        #ifndef NDEBUG
        while (true)
        {
        }
        #endif // NDEBUG
    }
    else if (toggledA)
    {
        rotaryEncoder->stateAPrevious = stateA;

        if (stateA != stateB)
        {
            if (SCHAR_MAX > rotaryEncoder->rotation)
            {
                rotaryEncoder->rotation += 1;
            }
            else
            {
                // intentionally empty
            }
        }
        else // (stateA == stateB)
        {
            if (SCHAR_MIN < rotaryEncoder->rotation)
            {
                rotaryEncoder->rotation -= 1;
            }
            else
            {
                // intentionally empty
            }
        }
    }
    else if (toggledB)
    {
        rotaryEncoder->stateBPrevious = stateB;

        // Only count for toggling of A - because of the HW layout this would otherwise mean
        // an increase of rotation by 4 with each notch.

    //     if (stateA == stateB)
    //     {
    //         if (SCHAR_MAX > rotaryEncoder->rotation)
    //         {
    //             rotaryEncoder->rotation += 1;
    //         }
    //         else
    //         {
    //             // intentionally empty
    //         }
    //     }
    //     else // (stateA != stateB)
    //     {
    //         if (SCHAR_MIN < rotaryEncoder->rotation)
    //         {
    //             rotaryEncoder->rotation -= 1;
    //         }
    //         else
    //         {
    //             // intentionally empty
    //         }
    //     }
    }
    else
    {
        // intentionally empty
    }
}
