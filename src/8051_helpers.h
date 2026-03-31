#ifndef REG8051_HELPERS_H
#define REG8051_HELPERS_H

#include <mcs51/8051.h>


inline void noInterrupts()
{
    EA = 0;
}

inline void interrupts()
{
    EA = 1;
}

#endif // REG8051_HELPERS_H