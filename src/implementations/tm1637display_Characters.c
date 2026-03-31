#include "../tm1637display.h"



/**
 * Single segment:
 *
 *      A
 *     ---
 *  F |   | B
 *     -G-
 *  E |   | C
 *     ---
 *      D
 *
 * PCB 7-segments according to addresses in SRAM [commands 0xC0 bis 0xC5]:
 *
 * [0] [1] : [2] [3]
 *
 * with ":" colon corresponding to the MSb of byte 1.
 *
 */
uint8_t const tm1637Characters[] =
{
    // GFEDCBA
    0b00111111,    // 0
    0b00000110,    // 1
    0b01011011,    // 2
    0b01001111,    // 3
    0b01100110,    // 4
    0b01101101,    // 5
    0b01111101,    // 6
    0b00000111,    // 7
    0b01111111,    // 8
    0b01101111,    // 9
    0b01110111,    // A
    0b01111100,    // b
    0b00111001,    // C
    0b01011110,    // d
    0b01111001,    // E
    0b01110001,    // F
    0b01000000,    // -
    0b00000000,    //
    0b00111000,    // L
    0b01110010,    // Y
};

