#include "../tm1637display.h"


void tm1637RenderNumberUnsigned(uint8_t const number)
{
    tm1637DisplayData[0] = tm1637Characters[tm1637Character_none];
    tm1637DisplayData[1] = (tm1637DisplayData[1] & 0x80) | tm1637Characters[(number / 100) /*% 10*/];
    tm1637DisplayData[2] = tm1637Characters[(number / 10) % 10];
    tm1637DisplayData[3] = tm1637Characters[number % 10];
}
