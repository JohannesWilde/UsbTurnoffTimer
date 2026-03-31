#include "../auxiliaries.h"

#include <limits.h>


int8_t absoluteValue_int8(int8_t const value)
{
    int8_t absolute = value;
    if (0 > value)
    {
        if (SCHAR_MIN == value)
        {
            absolute = SCHAR_MAX;
        }
        else
        {
            absolute = -1 * value;
        }
    }
    else
    {
        // absolute = value;
    }
    return absolute;
}
