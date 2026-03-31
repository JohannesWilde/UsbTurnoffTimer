#include "../rotaryencoder.h"


void rotaryEncoderInit(RotaryEncoder * rotaryEncoder, bool const stateA, bool const stateB)
{
    rotaryEncoder->rotation = 0;
    rotaryEncoder->stateAPrevious = stateA;
    rotaryEncoder->stateBPrevious = stateB;
}
