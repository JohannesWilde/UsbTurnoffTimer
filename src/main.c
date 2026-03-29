#include "static_assert.h"
#include "stc8g.h"

#include <stdint.h>


static void delay(unsigned int t)
{
    while (t--)
    {
        volatile int i = 0xFFF;
        while (i--);
    }
}

#define F_IRC 24000000ull  // Hz
#define CLOCK_DIVISOR 12
#define F_CPU (F_IRC / CLOCK_DIVISOR)  // Hz
#define F_SYS_TICK 100ull  // Hz

#define MAKE_PIN_NAME_(port, pin) P##port##_##pin
#define MAKE_PIN_NAME(port, pin) MAKE_PIN_NAME_(port, pin)

#define DIO_MODE_BIDIRECTIONAL_M0 0
#define DIO_MODE_BIDIRECTIONAL_M1 0
#define DIO_MODE_PUSH_PULL_OUTPUT_M0 1
#define DIO_MODE_PUSH_PULL_OUTPUT_M1 0
#define DIO_MODE_HIGH_Z_INPUT_M0 0
#define DIO_MODE_HIGH_Z_INPUT_M1 1
#define DIO_MODE_OPEN_DRAIN_M0 1
#define DIO_MODE_OPEN_DRAIN_M1 1


#define PUSH_BUTTON_PORT_NUMBER 3
#define PUSH_BUTTON_PIN_NUMBER 4  // for final layout 0 - but right now avoid conflicts with programming pins.

#define PWR_SWITCH_PORT_NUMBER 1  // 3
#define PWR_SWITCH_PIN_NUMBER 2  // 1

#define ROTARY_ENCODER_A_PORT_NUMBER 5
#define ROTARY_ENCODER_A_PIN_NUMBER 4

#define ROTARY_ENCODER_B_PORT_NUMBER 5
#define ROTARY_ENCODER_B_PIN_NUMBER 5

#define SEVEN_SEGMENT_DATA_PORT_NUMBER 3
#define SEVEN_SEGMENT_DATA_PIN_NUMBER 2

#define SEVEN_SEGMENT_CLK_PORT_NUMBER 3
#define SEVEN_SEGMENT_CLK_PIN_NUMBER 3


#define PUSH_BUTTON_PIN MAKE_PIN_NAME(PUSH_BUTTON_PORT_NUMBER, PUSH_BUTTON_PIN_NUMBER)
#define PWR_SWITCH_PIN MAKE_PIN_NAME(PWR_SWITCH_PORT_NUMBER, PWR_SWITCH_PIN_NUMBER)
#define ROTARY_ENCODER_A_PIN MAKE_PIN_NAME(ROTARY_ENCODER_A_PORT_NUMBER, ROTARY_ENCODER_A_PIN_NUMBER)
#define ROTARY_ENCODER_B_PIN MAKE_PIN_NAME(ROTARY_ENCODER_B_PORT_NUMBER, ROTARY_ENCODER_B_PIN_NUMBER)
#define SEVEN_SEGMENT_DATA_PIN MAKE_PIN_NAME(SEVEN_SEGMENT_DATA_PORT_NUMBER, SEVEN_SEGMENT_DATA_PIN_NUMBER)
#define SEVEN_SEGMENT_CLK_PIN MAKE_PIN_NAME(SEVEN_SEGMENT_CLK_PORT_NUMBER, SEVEN_SEGMENT_CLK_PIN_NUMBER)


inline void noInterrupts()
{
    EA = 0;
}

inline void interrupts()
{
    EA = 1;
}

typedef uint32_t Duration;
static volatile Duration milliseconds_ = 0;

Duration millis()
{
    Duration copy = 0;
    noInterrupts();
    copy = milliseconds_;
    interrupts();
    return copy;
}

// The name of this function does not really matter [apart from good coding practices], what
// is im portant is that it is declared the ISR handler via "__interrupt (x)" for the ISR-vector.
void TM0_Isr(void) __interrupt (TF0_VECTOR)
{
    /* action to be taken when timer 0 overflows */
    COMPILE_TIME_ASSERT((1000ull / F_SYS_TICK) * F_SYS_TICK == 1000ull); // Prevent numeric precision loss in accumulation of ms.
    milliseconds_ += 1000 / F_SYS_TICK;
}



void main()
{
    // Set DATA and CLK to HIGH while they are still high impedance, so that open-drain configuration keeps them there.
    // Both being high is the IDLE state of the I2C "bus" [no address for the TM1637, so only 2 users].
    SEVEN_SEGMENT_DATA_PIN = 1;
    SEVEN_SEGMENT_CLK_PIN = 1;


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

    // TMOD = (0 * T0_GATE) | (0 * T0_CT) | (0 * T0_M1) | (0 * T0_M0); // un-gated [0], timer [0], 16-bit auto-reload [00]
    // AUXR &= ~(1 << 7); // AUXR.T0x12[7] = 0 -> 0: The clock source of T0 is SYSclk/12, 1: The clock source of T0 is SYSclk/1.
    #define TIMER0_COUNT (65536ull - (F_CPU / 12 / F_SYS_TICK))
    // TR0 = 0; // Disable Timer0 so that the newly configured TH0 and TL0 will be used from the first cycle.
    TL0 = TIMER0_COUNT % 256;
    TH0 = TIMER0_COUNT / 256;
    TR0 = 1; // Timer0 run control bit
    ET0 = 1; // Enable Timer0 interrupt.

    interrupts(); // enable interrupts

    // uint8_t rotaryEncoderAPrevious = ROTARY_ENCODER_A_PIN;
    #define PRE_SCALER_INIT 50
    uint8_t preScaler = PRE_SCALER_INIT;
    while (1)
    {
        PCON |= (1 << 0);  // PCON.IDL[0] = 1 - Enter idle mode

        --preScaler;

        if (0 == preScaler)
        {
            preScaler = PRE_SCALER_INIT;
            PWR_SWITCH_PIN = (0 != PWR_SWITCH_PIN) ? 0 : 1;  // Toggle P5.5
        }
        // delay(50);

        // uint8_t const rotaryEncoderA = ROTARY_ENCODER_A_PIN;
        // uint8_t const rotaryEncoderB = ROTARY_ENCODER_B_PIN;

        // if (rotaryEncoderA != rotaryEncoderAPrevious)
        // {
        //     rotaryEncoderAPrevious = rotaryEncoderA;

        //     PWR_SWITCH_PIN = rotaryEncoderB ^ rotaryEncoderA;

        //     // Observation: clockwise: on, counter-clockwise: off
        // }
        // else
        // {
        //     // intentionally empty
        // }

        // PWR_SWITCH_PIN = PUSH_BUTTON_PIN;
        // PWR_SWITCH_PIN = ROTARY_ENCODER_A_PIN;
        // PWR_SWITCH_PIN = ROTARY_ENCODER_B_PIN;
    }
}
