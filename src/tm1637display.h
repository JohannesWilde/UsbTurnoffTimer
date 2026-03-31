#ifndef TM1637_DISPLAY_H
#define TM1637_DISPLAY_H

#include "tm1637driver.h"

#include "specifics.h"

#include <stdbool.h>
#include <stdint.h>


typedef enum
{
    tm1637Character_0 = 0,
    tm1637Character_1 = 1,
    tm1637Character_2 = 2,
    tm1637Character_3 = 3,
    tm1637Character_4 = 4,
    tm1637Character_5 = 5,
    tm1637Character_6 = 6,
    tm1637Character_7 = 7,
    tm1637Character_8 = 8,
    tm1637Character_9 = 9,
    tm1637Character_a = 10,
    tm1637Character_b = 11,
    tm1637Character_c = 12,
    tm1637Character_d = 13,
    tm1637Character_e = 14,
    tm1637Character_f = 15,
    tm1637Character_minus = 16,
    tm1637Character_none = 17,
    tm1637Character_l = 18,
    tm1637Character_y = 19,
    tm1637Character_h = 20,
    tm1637Character_m = 21,
    tm1637Character_o = tm1637Character_0,
    tm1637Character_s = tm1637Character_5,
} Tm1637Character;

extern uint8_t const tm1637Characters[];
extern uint8_t tm1637DisplayData[4];


void tm1637RenderDuration(Duration const * const duration);
void tm1637RenderDurationMinutes(uint16_t const duration);
void tm1637RenderDurationMinutesOrSeconds(MinutesOrSeconds const * duration);

void tm1637RenderColon(bool const enabled);

inline bool tm1637GetRenderColon()
{
    return /*0 != */(tm1637DisplayData[1] & 0x80);
}

void tm1637RenderNumberSigned(int8_t const number);
void tm1637RenderNumberUnsigned(uint8_t const number);

inline void tm1637Show()
{
    tm1637AddressCommand(/*address*/ 0, tm1637DisplayData, /*count*/ 4);
}

#endif // TM1637_DISPLAY_H
