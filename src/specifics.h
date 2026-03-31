#ifndef SPECIFICS_H
#define SPECIFICS_H

#include "buttontimed.h"

#include <stdint.h>


#define MAX_24HOURS_MINUTES ((uint16_t)(24) * 60)
#define MAX_60MINUTES_SECONDS ((uint16_t)(60) * 60)

typedef uint32_t Duration;
typedef uint32_t Timestamp;

typedef struct
{
    uint16_t minutesNotSeconds : 1;
    uint16_t value : 15;
}
MinutesOrSeconds;


Timestamp millis();

ButtonStateDuration buttonRawDurationConversion_(uint8_t const rawDuration);

uint8_t rotaryEncoderRotationToValueConversion(uint8_t const rotation);
uint16_t rotaryEncoderRotationAppliedSexagesimal(uint16_t const value, int8_t const rotation, uint16_t const max);

void rotaryEncoderRotationAppliedToMinutesOrSeconds(MinutesOrSeconds * value, int8_t rotation);

Duration minutesOrSecondsToDuration(MinutesOrSeconds const * value);

#endif // SPECIFICS_H
