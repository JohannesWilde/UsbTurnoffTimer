#include "static_assert.h"
#include "stc8g.h"

#include <stdbool.h>
#include <stdint.h>


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
// is im portant is that it is declared the ISR handler via "__interrupt (x)" for the ISR-vector.
void TM0_Isr(void) __interrupt (TF0_VECTOR)
{
    /* action to be taken when timer 0 overflows */
    COMPILE_TIME_ASSERT((1000ull / F_SYS_TICK) * F_SYS_TICK == 1000ull); // Prevent numeric precision loss in accumulation of ms.
    milliseconds_ += 1000 / F_SYS_TICK;
}


// Bit-banging I2C interface [send only for now].

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


// TM1637 driver

// Keep these static variables uninitialized for smaller code size.
// They will be filled with 0s before the program is executed anyway.
static uint8_t tm1637DisplayData[4]; // remember display data as I can't read it back
static uint8_t tm1637DisplayControl; // remember On/Off and brightness as I can't read it back.


void tm1637DataCommand(bool const fixedAddress, bool const readKeyAndDontWriteDisplay)
{
    uint8_t const value = /*data command*/ 0x40 |
                           ((readKeyAndDontWriteDisplay ? 1 : 0) << 1) |
                           ((fixedAddress ? 1 : 0) << 2); // |
                           // ((/*test mode*/ false ? 1 : 0) << 3);

    i2cStart();
    uint8_t bytesWrittenSuccessfully = i2cWrite(&value, /*count*/ 1);
    i2cStop();

    #ifndef NDEBUG
    while (1 != bytesWrittenSuccessfully)
    {
    }
    #endif // NDEBUG
}

void tm1637AddressCommand(uint8_t const address, uint8_t const * data, uint8_t const count)
{
    #ifndef NDEBUG
    // No more than the first 4 bytes usable.
    while (4 < (count + address))
    {
    }
    #endif // NDEBUG

    uint8_t const value = /*address command*/ 0xc0 |
                           (address & 0x0f);

    i2cStart();
    uint8_t bytesWrittenSuccessfully = i2cWrite(&value, /*count*/ 1);

    #ifndef NDEBUG
    while (1 != bytesWrittenSuccessfully)
    {
    }
    #endif // NDEBUG


    bytesWrittenSuccessfully = i2cWrite(data, /*count*/ count);
    i2cStop();

    #ifndef NDEBUG
    while (count != bytesWrittenSuccessfully)
    {
    }
    #endif // NDEBUG
}

void tm1637DisplayCommand(bool const on, uint8_t const brightness)
{
    tm1637DisplayControl = /*display command*/ 0x80 |
                           (brightness & 0x07) |
                           ((on ? 1 : 0) << 3);

    i2cStart();
    uint8_t const bytesWrittenSuccessfully = i2cWrite(&tm1637DisplayControl, /*count*/ 1);
    i2cStop();

    #ifndef NDEBUG
    while (1 != bytesWrittenSuccessfully)
    {
    }
    #endif // NDEBUG
}

typedef enum
{
    n0 = 0,
    n1 = 1,
    n2 = 2,
    n3 = 3,
    n4 = 4,
    n5 = 5,
    n6 = 6,
    n7= 7,
    n8= 8,
    n9= 9,
    a = 10,
    b = 11,
    c = 12,
    d = 13,
    e = 14,
    f = 15,
    minus = 16,
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
};


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


    tm1637DisplayData[0] = (1 * 0x7f);
    tm1637DisplayData[1] = (1 * 0x7f) | (1 * 0x80); // MSb is colon
    tm1637DisplayData[2] = (1 * 0x7f);
    tm1637DisplayData[3] = (1 * 0x7f);

    tm1637DataCommand(/*fixedAddress*/ false, /*readKeyAndDontWriteDisplay*/ false);
    tm1637AddressCommand(/*address*/ 0, tm1637DisplayData, /*count*/ 4);
    tm1637DisplayCommand(/*on*/ true, /*brightness*/ 0x01);


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



            if (0 != newValue)
            {
                tm1637DisplayData[1] |= (1 * 0x80); // colon
            }
            else
            {
                tm1637DisplayData[1] &= ~(1 * 0x80); // colon
            }

            tm1637AddressCommand(/*address*/ 1, &tm1637DisplayData[1], /*count*/ 1);

        }

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
