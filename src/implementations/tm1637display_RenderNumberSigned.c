#include "../tm1637display.h"

#include <limits.h>


void tm1637RenderNumberSigned(int8_t const number)
{
    int8_t numberCopy = number;
    if (SCHAR_MIN == number)
    {
        // I can't represent +128 in int8_t - so handle it specially
        tm1637DisplayData[0] = tm1637Characters[tm1637Character_minus];
        tm1637DisplayData[1] = (tm1637DisplayData[1] & 0x80) | tm1637Characters[tm1637Character_1];
        tm1637DisplayData[2] = tm1637Characters[tm1637Character_2];
        tm1637DisplayData[3] = tm1637Characters[tm1637Character_8];
    }
    else
    {
      if (0 > number)
      {
          tm1637DisplayData[0] = tm1637Characters[tm1637Character_minus];
          numberCopy *= -1;
      }
      else
      {
          tm1637DisplayData[0] = tm1637Characters[tm1637Character_none];
      }

      tm1637DisplayData[1] = (tm1637DisplayData[1] & 0x80) | tm1637Characters[(numberCopy / 100) /*% 10*/];
      tm1637DisplayData[2] = tm1637Characters[(numberCopy / 10) % 10];
      tm1637DisplayData[3] = tm1637Characters[numberCopy % 10];
    }
}
