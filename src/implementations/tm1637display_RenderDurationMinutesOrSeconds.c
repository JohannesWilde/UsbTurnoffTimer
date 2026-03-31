#include "../tm1637display.h"


void tm1637RenderDurationMinutesOrSeconds(MinutesOrSeconds const * duration)
{
    tm1637RenderDurationMinutes(duration->value);

    // Overwrite first two characters with "--" to signifiy the difference.
    if (!duration->minutesNotSeconds)
    {
        tm1637DisplayData[0] = tm1637Characters[tm1637Character_minus];
        tm1637DisplayData[1] = tm1637Characters[tm1637Character_minus];
    }
    else
    {
        // intentionally empty
    }
}
