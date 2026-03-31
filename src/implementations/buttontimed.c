#include "../buttontimed.h"

#include <limits.h>


void buttonTimedInit(ButtonTimed * button)
{
    button->duration = 0;
    button->previousDuration = 0;
    button->state = buttonUp;
}

void buttonTimedUpdate(ButtonTimed * button, ButtonState const state)
{
    if (state != button->state)
    {
        // toggled
        button->previousDuration = button->duration;
        button->duration = 1;
        button->state = state;
    }
    else
    {
        if (UCHAR_MAX > button->duration)
        {
            ++button->duration;
        }
        else
        {
            // intentionally empty
        }
    }
}

bool buttonReleasedAfterShort(ButtonTimed const * button)
{
    return (1 == button->duration) &&
           buttonIsUp(button) &&
           (buttonDurationShort == buttonRawDurationConversion_(button->previousDuration));
}

bool buttonReleasedAfterLong(ButtonTimed const * button)
{
    return (1 == button->duration) &&
           buttonIsUp(button) &&
           (buttonDurationLong == buttonRawDurationConversion_(button->previousDuration));
}
