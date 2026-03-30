#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <stdint.h>
#include <stdbool.h>


typedef struct
{
    int8_t rotation;
    bool stateAPrevious : 1;
    bool stateBPrevious : 1;
}
RotaryEncoder;


void rotaryEncoderInit(RotaryEncoder * rotaryEncoder, bool const stateA, bool const stateB);
void rotaryEncoderUpdate(RotaryEncoder * rotaryEncoder, bool const stateA, bool const stateB);

inline int8_t rotaryEncoderPeekAccumulatedRotation(RotaryEncoder const * rotaryEncoder)
{
    return rotaryEncoder->rotation;
}

int8_t rotaryEncoderGetAndResetAccumulatedRotation(RotaryEncoder * rotaryEncoder);

#endif // ROTARY_ENCODER_H
