#include "../specifics.h"

#include "../8051_helpers.h"


extern volatile Duration milliseconds_;

Duration millis()
{
    Duration copy = 0;
    noInterrupts();
    copy = milliseconds_;
    interrupts();
    return copy;
}
