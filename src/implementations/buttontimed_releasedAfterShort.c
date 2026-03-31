#include "../buttontimed.h"

bool buttonReleasedAfterShort(ButtonTimed const * button)
{
    return (1 == button->duration) &&
           buttonIsUp(button) &&
           (buttonDurationShort == buttonRawDurationConversion_(button->previousDuration));
}
