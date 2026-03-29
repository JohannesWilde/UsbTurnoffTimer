#include "static_assert.h"
#include "stc8g.h"

#include <stdbool.h>
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


inline void i2cDelay_()
{
    COMPILE_TIME_ASSERT((2000000 == F_CPU));
    // Fast [5 us pulse width for 50:50 duty cycle pulses -> period 2-times as long].
    NOP();
    NOP();
    NOP();
}

void i2cStart()
{
    // CLK high, DIO high -> low
    SEVEN_SEGMENT_DATA_PIN = 0;
    i2cDelay_();
}

void i2cStop()
{
    // To end acknowledgement from TM1637 another CLK pulse seems to be required before stop.
    SEVEN_SEGMENT_CLK_PIN = 0;

    // Prepare DATA for stop-condition while CLK LOW.
    SEVEN_SEGMENT_DATA_PIN = 0;

    i2cDelay_();
    SEVEN_SEGMENT_CLK_PIN = 1;
    i2cDelay_();

    // Actual stop:  CLK high, DIO low -> high
    SEVEN_SEGMENT_DATA_PIN = 1;
    i2cDelay_();
}

uint8_t i2cWrite(uint8_t const * const data, uint8_t const count)
{
    uint8_t numberOfBytesSuccessfullyWritten = 0;
    while (count > numberOfBytesSuccessfullyWritten)
    {
        uint8_t datum = data[numberOfBytesSuccessfullyWritten];
        uint8_t numberBitsToSend = 8;
        do
        {
            // pull down CLK
            SEVEN_SEGMENT_CLK_PIN = 0;

            // set bit
            SEVEN_SEGMENT_DATA_PIN = datum & 0x01; // LSb first

            // CLK-low-pulse width
            i2cDelay_();

            // release CLK up
            SEVEN_SEGMENT_CLK_PIN = 1;

            // Shift datum right 1 bit [do so at the HIGH CLK, as we set DATA during LOW CLK].
            datum >>= 1;

            // Let the counterpart sample the data.
            i2cDelay_();

            --numberBitsToSend;
        }
        while (0 < numberBitsToSend);

        // release data line
        SEVEN_SEGMENT_DATA_PIN = 1;
        // Read in ACK.
        SEVEN_SEGMENT_CLK_PIN = 0;
        i2cDelay_();
        SEVEN_SEGMENT_CLK_PIN = 1;
        i2cDelay_();

        uint8_t const ack = SEVEN_SEGMENT_DATA_PIN;

        bool const successfullySentByte = (0 == ack);

        if (successfullySentByte)
        {
            ++numberOfBytesSuccessfullyWritten;
        }
        else
        {
            break;
        }
    }
    return numberOfBytesSuccessfullyWritten;
}



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
    // AUXR &= ~(1 << 7); // AUXR.T0x12[7] = 0 -> 0: The clock source of T0 is SYSclk/12, 1: The clock source of T0 is SYSclk/1.
    #define TIMER0_COUNT (65536ull - (F_CPU / 12 / F_SYS_TICK))
    // TR0 = 0; // Disable Timer0 so that the newly configured TH0 and TL0 will be used from the first cycle.
    TL0 = TIMER0_COUNT % 256;
    TH0 = TIMER0_COUNT / 256;
    TR0 = 1; // Timer0 run control bit
    ET0 = 1; // Enable Timer0 interrupt.

    interrupts(); // enable interrupts


    #define TM1637_I2C_COMM1    0x40        // data command
    #define TM1637_I2C_COMM2    0xC0        // display and control command
    #define TM1637_I2C_COMM3    0x80        // address command

    uint8_t data[6] = {0x00, };

    data[0] = TM1637_I2C_COMM1 | (/*fixed address*/ 0 << 2);
    i2cStart();
    uint8_t bytesWrittenSuccessfully = i2cWrite(data, /*count*/ 1);
    i2cStop();

    while (1 != bytesWrittenSuccessfully)
    {
    }

    data[0] = TM1637_I2C_COMM2 | /*address*/ 0;
    i2cStart();
    bytesWrittenSuccessfully = i2cWrite(data, /*count*/ 1);

    while (1 != bytesWrittenSuccessfully)
    {
    }

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

    data[0] = (1 * 0x7f);
    data[1] = (1 * 0x7f) | (1 * 0x80); // MSb is colon
    data[2] = (1 * 0x7f);
    data[3] = (1 * 0x7f);
    bytesWrittenSuccessfully = i2cWrite(data, /*count*/ 4);
    i2cStop();

    while (4 != bytesWrittenSuccessfully)
    {
    }

    data[0] = TM1637_I2C_COMM3 | /*on*/ 0x08 | /*brightness*/ 0x01;
    i2cStart();
    bytesWrittenSuccessfully = i2cWrite(data, /*count*/ 1);
    i2cStop();

    while (1 != bytesWrittenSuccessfully)
    {
    }


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

            uint8_t const newValue = (0 != PWR_SWITCH_PIN) ? 0 : 1;

            PWR_SWITCH_PIN = newValue;  // Toggle P5.5


            // Todo: cleanup....
            data[0] = TM1637_I2C_COMM2 | /*address*/ 0x01;
            i2cStart();
            bytesWrittenSuccessfully = i2cWrite(data, /*count*/ 1);

            while (1 != bytesWrittenSuccessfully)
            {
            }

            data[0] = (1 * 0x7f) | (newValue * 0x80); // MSb is dots
            bytesWrittenSuccessfully = i2cWrite(data, /*count*/ 1);
            i2cStop();

            while (1 != bytesWrittenSuccessfully)
            {
            }

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
