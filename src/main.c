#include "buttontimed.h"
#include "configuration.h"
#include "i2c_bitbang.h"
#include "pinout.h"
#include "rotaryencoder.h"
#include "specifics.h"
#include "static_assert.h"
#include "stc8g.h"
#include "tm1637driver.h"

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>


#define DIO_MODE_BIDIRECTIONAL_M0 0
#define DIO_MODE_BIDIRECTIONAL_M1 0
#define DIO_MODE_PUSH_PULL_OUTPUT_M0 1
#define DIO_MODE_PUSH_PULL_OUTPUT_M1 0
#define DIO_MODE_HIGH_Z_INPUT_M0 0
#define DIO_MODE_HIGH_Z_INPUT_M1 1
#define DIO_MODE_OPEN_DRAIN_M0 1
#define DIO_MODE_OPEN_DRAIN_M1 1


inline void noInterrupts()
{
    EA = 0;
}

inline void interrupts()
{
    EA = 1;
}


typedef uint32_t Duration;
// Keep this static variable uninitialized for smaller code size.
// It will be filled with 0s before the program is executed anyway.
static volatile Duration milliseconds_;

Duration millis()
{
    Duration copy = 0;
    noInterrupts();
    copy = milliseconds_;
    interrupts();
    return copy;
}

// The name of this function does not really matter [apart from good coding practices], what
// is important is that it is declared the ISR handler via "__interrupt (x)" for the ISR-vector.
void TM0_Isr(void) __interrupt (TF0_VECTOR)
{
    /* action to be taken when timer 0 overflows */
    COMPILE_TIME_ASSERT((1000ull / F_SYS_TICK) * F_SYS_TICK == 1000ull); // Prevent numeric precision loss in accumulation of ms.
    milliseconds_ += 1000 / F_SYS_TICK;
}


// TM1637 driver

// Keep these static variables uninitialized for smaller code size.
// They will be filled with 0s before the program is executed anyway.
static uint8_t tm1637DisplayData[4]; // remember display data as I can't read it back
// static uint8_t tm1637DisplayControl; // remember On/Off and brightness as I can't read it back.



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
    tm1637Character_o = tm1637Character_0,
} Tm1637Character;


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
static uint8_t const tm1637Characters[] =
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

bool tm1637GetRenderColon()
{
    return /*0 != */(tm1637DisplayData[1] & 0x80);
}

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

void tm1637RenderNumberUnsigned(uint8_t const number)
{
    tm1637DisplayData[0] = tm1637Characters[tm1637Character_none];
    tm1637DisplayData[1] = (tm1637DisplayData[1] & 0x80) | tm1637Characters[(number / 100) /*% 10*/];
    tm1637DisplayData[2] = tm1637Characters[(number / 10) % 10];
    tm1637DisplayData[3] = tm1637Characters[number % 10];
}

inline void tm1637Show()
{
    tm1637AddressCommand(/*address*/ 0, tm1637DisplayData, /*count*/ 4);
}

// Prescaler for different time domains.

// Update prescaler value. Return true on underrun [i.e. processing should happen].
// The priodicity of the prescaler is (initialValue + 1).
inline bool updatePrescaler(uint8_t * value, uint8_t const initialValue)
{
    bool const valueUnderrun = (0 == (*value));
    if (valueUnderrun)
    {
        *value = initialValue;
    }
    else
    {
        --(*value);
    }
    return valueUnderrun;
}

#define PRE_SCALER_ONE_INIT (50 - 1)    // 1000 Hz -> 20 Hz
#define PRE_SCALER_TWO_INIT (10 - 1)    // 20 Hz -> 2 Hz

static uint8_t preScalerOne = PRE_SCALER_ONE_INIT;
static uint8_t preScalerTwo = PRE_SCALER_TWO_INIT;

// HW inputs
static ButtonTimed pushButton;
static RotaryEncoder rotaryEncoder;


void main()
{
    // Set DATA and CLK to HIGH while they are still high impedance, so that open-drain configuration keeps them there.
    // Both being high is the IDLE state of the I2C "bus" [no address for the TM1637, so only 2 users].
    SEVEN_SEGMENT_DATA_PIN = 1;
    SEVEN_SEGMENT_CLK_PIN = 1;

    SFRX_ON();
    // Pull-Up resistors enabled for push-button [which closes to GND] and I2C signals [which are open drain].
    P3PU = (1 << PUSH_BUTTON_PIN_NUMBER) |
           (1 << SEVEN_SEGMENT_DATA_PIN_NUMBER) |
           (1 << SEVEN_SEGMENT_CLK_PIN_NUMBER);
    // Pull-Up resistors enabled for rotary encoder signals.
    P5PU = (1 << ROTARY_ENCODER_A_PIN_NUMBER) |
           (1 << ROTARY_ENCODER_B_PIN_NUMBER);

    // Precise timing is more important than less current for this application.
    // CLKSEL = 0b00; // 0b00 - internal high-precision IRC, 0b11 - internal 32KHz low speed IRC
    COMPILE_TIME_ASSERT(0 < CLOCK_DIVISOR);
    CLKDIV = CLOCK_DIVISOR;
    // HIRCCR = 1 << 7; // ENHIRC[7]
    // IRC32KCR = 0 << 7; // ENIRC32K[7]
    SFRX_OFF();


    COMPILE_TIME_ASSERT((0 <= PWR_SWITCH_PIN_NUMBER) && (8 > PWR_SWITCH_PIN_NUMBER));
    COMPILE_TIME_ASSERT(1 == PWR_SWITCH_PORT_NUMBER);

    P1M0 = (0x00 /* DIO_MODE_HIGH_Z_INPUT_M0 */) |
           (DIO_MODE_PUSH_PULL_OUTPUT_M0 << PWR_SWITCH_PIN_NUMBER);
    P1M1 = (0xff /* DIO_MODE_HIGH_Z_INPUT_M1 */) &
           ((DIO_MODE_PUSH_PULL_OUTPUT_M1 << PWR_SWITCH_PIN_NUMBER) | ~(1 << PWR_SWITCH_PIN_NUMBER)) ;

    COMPILE_TIME_ASSERT((0 <= PUSH_BUTTON_PIN_NUMBER) && (8 > PUSH_BUTTON_PIN_NUMBER));
    COMPILE_TIME_ASSERT((0 <= SEVEN_SEGMENT_DATA_PIN_NUMBER) && (8 > SEVEN_SEGMENT_DATA_PIN_NUMBER));
    COMPILE_TIME_ASSERT((0 <= SEVEN_SEGMENT_CLK_PIN_NUMBER) && (8 > SEVEN_SEGMENT_CLK_PIN_NUMBER));
    COMPILE_TIME_ASSERT(3 == PUSH_BUTTON_PORT_NUMBER);
    COMPILE_TIME_ASSERT(3 == SEVEN_SEGMENT_DATA_PORT_NUMBER);
    COMPILE_TIME_ASSERT(3 == SEVEN_SEGMENT_CLK_PORT_NUMBER);

    P3M0 = (0x00 /* DIO_MODE_HIGH_Z_INPUT_M0 */) |
           (DIO_MODE_HIGH_Z_INPUT_M0 << PUSH_BUTTON_PIN_NUMBER) |
           (DIO_MODE_OPEN_DRAIN_M0 << SEVEN_SEGMENT_DATA_PIN_NUMBER) |
           (DIO_MODE_OPEN_DRAIN_M0 << SEVEN_SEGMENT_CLK_PIN_NUMBER);
    P3M1 = (0xff /* DIO_MODE_HIGH_Z_INPUT_M1 */) &
           ((DIO_MODE_HIGH_Z_INPUT_M1 << PUSH_BUTTON_PIN_NUMBER) | ~(1 << PUSH_BUTTON_PIN_NUMBER)) &
           ((DIO_MODE_OPEN_DRAIN_M0 << SEVEN_SEGMENT_DATA_PIN_NUMBER) | ~(1 << SEVEN_SEGMENT_DATA_PIN_NUMBER)) &
           ((DIO_MODE_OPEN_DRAIN_M0 << SEVEN_SEGMENT_CLK_PIN_NUMBER) | ~(1 << SEVEN_SEGMENT_CLK_PIN_NUMBER));

    COMPILE_TIME_ASSERT((0 <= ROTARY_ENCODER_A_PIN_NUMBER) && (8 > ROTARY_ENCODER_A_PIN_NUMBER));
    COMPILE_TIME_ASSERT((0 <= ROTARY_ENCODER_B_PIN_NUMBER) && (8 > ROTARY_ENCODER_B_PIN_NUMBER));
    COMPILE_TIME_ASSERT(5 == ROTARY_ENCODER_A_PORT_NUMBER);
    COMPILE_TIME_ASSERT(5 == ROTARY_ENCODER_B_PORT_NUMBER);

    // P5M0 = (0x00 /* DIO_MODE_HIGH_Z_INPUT_M0 */) |
    //        (DIO_MODE_HIGH_Z_INPUT_M0 << ROTARY_ENCODER_A_PIN_NUMBER) |
    //        (DIO_MODE_HIGH_Z_INPUT_M0 << ROTARY_ENCODER_B_PIN_NUMBER);
    // P5M1 = (0xff /* DIO_MODE_HIGH_Z_INPUT_M1 */) &
    //        ((DIO_MODE_HIGH_Z_INPUT_M1 << ROTARY_ENCODER_A_PIN_NUMBER) | ~(1 << ROTARY_ENCODER_A_PIN_NUMBER)) &
    //        ((DIO_MODE_HIGH_Z_INPUT_M1 << ROTARY_ENCODER_B_PIN_NUMBER) | ~(1 << ROTARY_ENCODER_B_PIN_NUMBER));

    PWR_SWITCH_PIN = 1; // PWR_SWITCH

    // TMOD = (0 * T0_GATE) | (0 * T0_CT) | (0 * T0_M1) | (0 * T0_M0); // un-gated [0], timer [0], 16-bit auto-reload [00]
    AUXR |= (1 << 7); // AUXR.T0x12[7] = 0 -> 0: The clock source of T0 is SYSclk/12, 1: The clock source of T0 is SYSclk/1.
    #define TIMER0_COUNT (65536ull - (F_CPU /*/ 12*/ / F_SYS_TICK))
    // TR0 = 0; // Disable Timer0 so that the newly configured TH0 and TL0 will be used from the first cycle.
    TL0 = TIMER0_COUNT % 256;
    TH0 = TIMER0_COUNT / 256;
    TR0 = 1; // Timer0 run control bit
    ET0 = 1; // Enable Timer0 interrupt.

    interrupts(); // enable interrupts


    tm1637DisplayData[0] = (1 * 0x7f);
    tm1637DisplayData[1] = (1 * 0x7f) | (1 * 0x80); // MSb is colon
    tm1637DisplayData[2] = (1 * 0x7f);
    tm1637DisplayData[3] = (1 * 0x7f);

    tm1637DataCommand(/*fixedAddress*/ false, /*readKeyAndDontWriteDisplay*/ false);
    tm1637AddressCommand(/*address*/ 0, tm1637DisplayData, /*count*/ 4);
    tm1637DisplayCommand(/*on*/ true, /*brightness*/ 0x01);

    // buttonTimedInit(&pushButton);
    rotaryEncoderInit(&rotaryEncoder, ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN);
    uint16_t selectedDurationMinutes = 0;

    while (true)
    {
        // Update with complete F_SYS_CLK.
        rotaryEncoderUpdate(&rotaryEncoder, ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN);

        if (updatePrescaler(&preScalerOne, PRE_SCALER_ONE_INIT))
        {
            // Update with F_SYS_CLK / (PRE_SCALER_ONE_INIT + 1).
            buttonTimedUpdate(&pushButton, PUSH_BUTTON_PIN);

            if (updatePrescaler(&preScalerTwo, PRE_SCALER_TWO_INIT))
            {
                // Update with F_SYS_CLK / (PRE_SCALER_ONE_INIT + 1) / (PRE_SCALER_TWO_INIT + 1).

                PWR_SWITCH_PIN ^= 1;

                // tm1637RenderColon(/*enabled*/ !tm1637GetRenderColon());
                // // tm1637AddressCommand(/*address*/ 1, &tm1637DisplayData[1], /*count*/ 1);
            }
            else
            {
                // intentionally empty
            }




            if (buttonReleasedAfterLong(&pushButton))
            {
                selectedDurationMinutes = 0;
            }

            // int8_t const rotation = rotaryEncoderPeekAccumulatedRotation(&rotaryEncoder);
            int8_t const rotation = rotaryEncoderGetAndResetAccumulatedRotation(&rotaryEncoder);

            selectedDurationMinutes = rotaryEncoderRotationAppliedToMinutes(selectedDurationMinutes, rotation);
            tm1637RenderDurationMinutes(selectedDurationMinutes);

            // Duration const duration = millis();
            // tm1637RenderTime(&duration);

            tm1637Show();
        }
        else
        {
            // intentionally empty
        }



        PCON |= (1 << 0);  // PCON.IDL[0] = 1 - Enter idle mode
    }
}
