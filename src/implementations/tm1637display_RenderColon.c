#include "../tm1637display.h"


void tm1637RenderColon(bool const enabled)
{
    if (enabled)
    {
        tm1637DisplayData[1] |= (1 * 0x80); // colon
    }
    else
    {
        tm1637DisplayData[1] &= ~(1 * 0x80); // colon
    }
}
