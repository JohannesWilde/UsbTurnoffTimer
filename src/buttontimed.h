#ifndef BUTTON_TIMED_H
#define BUTTON_TIMED_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    buttonDown = 0,
    buttonUp = 1,
} ButtonState;

typedef enum
{
    buttonDurationTooShort,
    buttonDurationShort,
    buttonDurationLong,
} ButtonStateDuration;

typedef struct
{
    ButtonState state;
    uint8_t duration;
    uint8_t previousDuration;
} ButtonTimed;


void buttonTimedInit(ButtonTimed * button);
void buttonTimedUpdate(ButtonTimed * button, ButtonState const state);

inline bool buttonIsDown(ButtonTimed const * button)
{
    return buttonDown == button->state;
}

inline bool buttonIsUp(ButtonTimed const * button)
{
    return buttonUp == button->state;
}

bool buttonReleasedAfterShort(ButtonTimed const * button);
bool buttonReleasedAfterLong(ButtonTimed const * button);


// The user will have to implement this function as it depends on the update interval.
extern ButtonStateDuration buttonRawDurationConversion_(uint8_t const rawDuration);

#endif // BUTTON_TIMED_H
