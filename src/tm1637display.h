#ifndef TM1637_DISPLAY_H
#define TM1637_DISPLAY_H

#include "tm1637driver.h"

#include "specifics.h"

#include <stdbool.h>
#include <stdint.h>


typedef enum
{
    tm1637Character_0,
    tm1637Character_1,
    tm1637Character_2,
    tm1637Character_3,
    tm1637Character_4,
    tm1637Character_5,
    tm1637Character_6,
    tm1637Character_7,
    tm1637Character_8,
    tm1637Character_9,
    tm1637Character_a,
    tm1637Character_b,
    tm1637Character_c,
    tm1637Character_d,
    tm1637Character_e,
    tm1637Character_f,
    // tm1637Character_g,
    // tm1637Character_h,
    tm1637Character_i,
    // tm1637Character_j,
    // tm1637Character_k,
    tm1637Character_l,
    // tm1637Character_m,
    // tm1637Character_n,
    // tm1637Character_p,
    // tm1637Character_q,
    // tm1637Character_r,
    tm1637Character_t,
    // tm1637Character_u,
    // tm1637Character_v,
    tm1637Character_w,
    // tm1637Character_x,
    tm1637Character_y,
    tm1637Character_minus,
    tm1637Character_none,
    // alias
    tm1637Character_o = tm1637Character_0,
    tm1637Character_s = tm1637Character_5,
    tm1637Character_z = tm1637Character_2,
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
