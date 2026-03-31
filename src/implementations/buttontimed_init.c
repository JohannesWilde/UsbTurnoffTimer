#include "../buttontimed.h"


void buttonTimedInit(ButtonTimed * button)
{
    button->duration = 0;
    button->previousDuration = 0;
    button->state = buttonUp;
}
