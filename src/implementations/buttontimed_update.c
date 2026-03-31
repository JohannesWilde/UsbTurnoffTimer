#include "../buttontimed.h"

#include <limits.h>


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
