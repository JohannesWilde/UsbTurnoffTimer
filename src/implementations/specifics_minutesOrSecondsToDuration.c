#include "../specifics.h"


Duration minutesOrSecondsToDuration(MinutesOrSeconds const * value)
{
    Duration duration = value->value;
    if (value->minutesNotSeconds)
    {
        duration *= (60ull * 1000ull);
    }
    else
    {
        duration *= 1000ull;
    }
    return duration;
}