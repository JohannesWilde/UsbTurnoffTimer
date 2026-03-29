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

int8_t getAndResetAccumulatedRotation(RotaryEncoder * rotaryEncoder);

#endif // ROTARY_ENCODER_H
