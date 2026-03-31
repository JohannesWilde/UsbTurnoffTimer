#include "../tm1637display.h"


void tm1637RenderDuration(Duration const * const duration)
{
    if ((1000ull * 60ull * 60ull * 100ull) <= *duration)
    {
        // memset(tm1637DisplayData, tm1637Characters[tm1637Character_minus], 4);
        tm1637DisplayData[0] = tm1637Characters[tm1637Character_minus];
        tm1637DisplayData[1] = (tm1637DisplayData[1] & 0x80) | tm1637Characters[tm1637Character_minus];
        tm1637DisplayData[2] = tm1637Characters[tm1637Character_minus];
        tm1637DisplayData[3] = tm1637Characters[tm1637Character_minus];
    }
    else
    {
        Duration durationCopy = *duration;
        // // milliseconds -> seconds
        // durationCopy /= 1000;
        // uint8_t const seconds = durationCopy % 60;
        // // seconds -> minutes
        // durationCopy /= 60;
        Duration const divisor = 60000ull;
        // milliseconds -> minutes
        durationCopy /= divisor;
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
