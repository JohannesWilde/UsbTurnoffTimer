#include "../specifics.h"

#include "../configuration.h"
#include "../static_assert.h"


ButtonStateDuration buttonRawDurationConversion_(uint8_t const rawDuration)
{
    // 20 Hz update rate for button - but 1000 Hz for rotaryEncoder
    COMPILE_TIME_ASSERT(1000 == F_SYS_TICK);

    ButtonStateDuration duration = buttonDurationShort;
    if (10 < rawDuration) // 500 ms
    {
        duration = buttonDurationLong;
    }
    else
    {
        // duration = buttonDurationShort;
    }

    return duration;
}
