#include "../rotaryencoder.h"


int8_t rotaryEncoderGetAndResetAccumulatedRotation(RotaryEncoder * rotaryEncoder)
{
    int8_t const copy = rotaryEncoder->rotation;
    rotaryEncoder->rotation = 0;
    return copy;
}
