#include "../tm1637display.h"


void tm1637RenderDurationMinutes(uint16_t const duration)
{
    if ((((uint16_t)60) * 100) <= duration)
    {
        // memset(tm1637DisplayData, tm1637Characters[tm1637Character_minus], 4);
        tm1637DisplayData[0] = tm1637Characters[tm1637Character_minus];
        tm1637DisplayData[1] = (tm1637DisplayData[1] & 0x80) | tm1637Characters[tm1637Character_minus];
        tm1637DisplayData[2] = tm1637Characters[tm1637Character_minus];
        tm1637DisplayData[3] = tm1637Characters[tm1637Character_minus];
    }
    else
    {
        uint16_t durationCopy = duration;

        uint8_t const minutes = durationCopy % 60;
        // minutes -> hours
        durationCopy /= 60;
        uint8_t const hours = durationCopy;

        tm1637DisplayData[0] = tm1637Characters[hours / 10];
        tm1637DisplayData[1] = (tm1637DisplayData[1] & 0x80) | tm1637Characters[hours % 10];
        tm1637DisplayData[2] = tm1637Characters[minutes / 10];
        tm1637DisplayData[3] = tm1637Characters[minutes % 10];
    }
}
