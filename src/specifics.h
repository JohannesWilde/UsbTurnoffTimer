#ifndef SPECIFICS_H
#define SPECIFICS_H

#include "buttontimed.h"

#include <stdint.h>


typedef uint32_t Duration;

Duration millis();

ButtonStateDuration buttonRawDurationConversion_(uint8_t const rawDuration);

uint8_t rotaryEncoderRotationToMinutesConversion(uint8_t const rotation);
uint16_t rotaryEncoderRotationAppliedToMinutes(uint16_t const minutes, int8_t const rotation);

#endif // SPECIFICS_H
