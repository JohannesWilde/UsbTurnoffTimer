#ifndef CONFIGURATION_H
#define CONFIGURATION_H


#define F_IRC 24000000ull  // Hz
#define CLOCK_DIVISOR 12
#define F_CPU (F_IRC / CLOCK_DIVISOR)  // Hz
#define F_SYS_TICK 1000ull  // Hz


/**
 * STC8G1K08 [SOP8]
 *
 *                     +----------+
 *  rotary encoder A --+ 1      8 +-- TM1637 I2C CLK
 *               VCC --+ 2      7 +-- TM1637 I2C Data
 *  rotary encoder B --+ 3      6 +-- MOSFET            [HIGH - conductive, LOW - blocking]
 *               GND --+ 4      5 +-- push button       [pull-up, LOW when pushed]
 *                     +----------+
 */

#define PUSH_BUTTON_PORT_NUMBER 3
#define PUSH_BUTTON_PIN_NUMBER 0

#define PWR_SWITCH_PORT_NUMBER 3
#define PWR_SWITCH_PIN_NUMBER 1

#define ROTARY_ENCODER_A_PORT_NUMBER 5
#define ROTARY_ENCODER_A_PIN_NUMBER 4

#define ROTARY_ENCODER_B_PORT_NUMBER 5
#define ROTARY_ENCODER_B_PIN_NUMBER 5

#define SEVEN_SEGMENT_DATA_PORT_NUMBER 3
#define SEVEN_SEGMENT_DATA_PIN_NUMBER 2

#define SEVEN_SEGMENT_CLK_PORT_NUMBER 3
#define SEVEN_SEGMENT_CLK_PIN_NUMBER 3


#endif // CONFIGURATION_H
