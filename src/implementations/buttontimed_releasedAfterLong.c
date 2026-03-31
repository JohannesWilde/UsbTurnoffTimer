#include "../buttontimed.h"


bool buttonReleasedAfterLong(ButtonTimed const * button)
{
    return (1 == button->duration) &&
           buttonIsUp(button) &&
           (buttonDurationLong == buttonRawDurationConversion_(button->previousDuration));
}
