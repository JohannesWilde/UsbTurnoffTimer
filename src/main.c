#include "8051_helpers.h"
#include "buttontimed.h"
#include "configuration.h"
#include "pinout.h"
#include "prescaler.h"
#include "rotaryencoder.h"
#include "specifics.h"
#include "statemachine.h"
#include "static_assert.h"
#include "stc8g.h"
#include "tm1637display.h"
#include "tm1637driver.h"

#include <stdbool.h>
#include <stdint.h>


volatile Timestamp milliseconds_;

// The name of this function does not really matter [apart from good coding practices], what
// is important is that it is declared the ISR handler via "__interrupt (x)" for the ISR-vector.
void TM0_Isr(void) __interrupt (TF0_VECTOR)
{
    /* action to be taken when timer 0 overflows */
    COMPILE_TIME_ASSERT((1000ull / F_SYS_TICK) * F_SYS_TICK == 1000ull); // Prevent numeric precision loss in accumulation of ms.
    milliseconds_ += 1000 / F_SYS_TICK;
}


// TM1637 driver
// static uint8_t tm1637DisplayControl; // remember On/Off and brightness as I can't read it back.


// Prescaler for different time domains.
// 1000 Hz -> 20 Hz
#define PRE_SCALER_ONE_INIT (50 - 1)
// 20 Hz -> 2 Hz
#define PRE_SCALER_TWO_INIT (10 - 1)

static uint8_t preScalerOne = PRE_SCALER_ONE_INIT;
static uint8_t preScalerTwo = PRE_SCALER_TWO_INIT;

// HW inputs
static ButtonTimed pushButton;
static RotaryEncoder rotaryEncoder;

// Statemachine

typedef struct
{
    uint8_t cycleCounter;
    uint16_t delayDurationMinutes;
    MinutesOrSeconds offDuration;
    Timestamp nextTimeToAct;
}
StatemachineData;

static StatemachineData statemachineData;
static Statemachine statemachine;


FunctionPointerPrototype statemachineHandlerConfigureDelay(StatemachineStage stage, void * data);
FunctionPointerPrototype statemachineHandlerConfigureTimeOff(StatemachineStage stage, void * data);
FunctionPointerPrototype statemachineHandlerCountdown(StatemachineStage stage, void * data);


FunctionPointerPrototype statemachineHandlerConfigureDelay(StatemachineStage stage, void * const untypedData)
{
    StatemachineData * const data = (StatemachineData *)untypedData;
    StatemachineHandler nextHandler = &statemachineHandlerConfigureDelay;
    switch (stage)
    {
    case StatemachineStageInit:
    {
        tm1637DisplayData[0] = tm1637Characters[tm1637Character_d];
        tm1637DisplayData[1] = tm1637Characters[tm1637Character_l];
        tm1637DisplayData[2] = tm1637Characters[tm1637Character_a];
        tm1637DisplayData[3] = tm1637Characters[tm1637Character_y];
        tm1637RenderColon(false);
        tm1637Show();

        // Show above text for this long.
        data->cycleCounter = 20;
        break;
    }
    case StatemachineStageProcess:
    {
        int8_t const rotation = rotaryEncoderGetAndResetAccumulatedRotation(&rotaryEncoder);
        bool updateDisplay = false;

        if (0 == data->cycleCounter)
        {
            if (buttonIsDown(&pushButton))
            {
                // Ignore any input while the button is pressed.
            }
            else if (buttonReleasedAfterShort(&pushButton))
            {
                nextHandler = &statemachineHandlerConfigureTimeOff;
            }
            else if (buttonReleasedAfterLong(&pushButton))
            {
                nextHandler = &statemachineHandlerCountdown;
            }
            else
            {
                // handle rotation
                data->delayDurationMinutes =
                        rotaryEncoderRotationAppliedSexagesimal(data->delayDurationMinutes, rotation, MAX_24HOURS_MINUTES);
                updateDisplay = (0 != rotation);
            }
        }
        else if ((1 == data->cycleCounter) || /*early exit*/ (0 != rotation))
        {
            data->cycleCounter = 0;
            updateDisplay = true;
            tm1637RenderColon(true);
        }
        else // if (1 != data->cycleCounter)
        {
            --data->cycleCounter;
        }

        if (updateDisplay)
        {
            tm1637RenderDurationMinutes(data->delayDurationMinutes);
            tm1637Show();
        }
        else
        {
            // intentionally empty
        }
        break;
    }
    case StatemachineStageDeinit:
    {
        // intentionally empty
        break;
    }
    }
    return (FunctionPointerPrototype)nextHandler;
}

FunctionPointerPrototype statemachineHandlerConfigureTimeOff(StatemachineStage stage, void * const untypedData)
{
    StatemachineData * const data = (StatemachineData *)untypedData;
    StatemachineHandler nextHandler = &statemachineHandlerConfigureTimeOff;
    switch (stage)
    {
    case StatemachineStageInit:
    {
        tm1637DisplayData[0] = tm1637Characters[tm1637Character_o];
        tm1637DisplayData[1] = tm1637Characters[tm1637Character_f];
        tm1637DisplayData[2] = tm1637Characters[tm1637Character_f];
        tm1637DisplayData[3] = tm1637Characters[tm1637Character_none];
        tm1637RenderColon(false);
        tm1637Show();

        // Show above text for this long.
        data->cycleCounter = 20;
        break;
    }
    case StatemachineStageProcess:
    {
        break;
    }
    case StatemachineStageDeinit:
    {
        break;
    }
    }
    return (FunctionPointerPrototype)nextHandler;
}

FunctionPointerPrototype statemachineHandlerCountdown(StatemachineStage const stage, void * const untypedData)
{
    StatemachineData * const data = (StatemachineData *)untypedData;
    StatemachineHandler nextHandler = &statemachineHandlerCountdown;
    switch (stage)
    {
    case StatemachineStageInit:
    {
        break;
    }
    case StatemachineStageProcess:
    {
        break;
    }
    case StatemachineStageDeinit:
    {
        break;
    }
    }
    return (FunctionPointerPrototype)nextHandler;
}


// main()

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

    tm1637DisplayData[0] = tm1637Characters[tm1637Character_none]; // (1 * 0x7f);
    tm1637DisplayData[1] = tm1637Characters[tm1637Character_none]; // (1 * 0x7f) | (1 * 0x80); // MSb is colon
    tm1637DisplayData[2] = tm1637Characters[tm1637Character_none]; // (1 * 0x7f);
    tm1637DisplayData[3] = tm1637Characters[tm1637Character_none]; // (1 * 0x7f);

    tm1637DataCommand(/*fixedAddress*/ false, /*readKeyAndDontWriteDisplay*/ false);
    tm1637AddressCommand(/*address*/ 0, tm1637DisplayData, /*count*/ 4);
    tm1637DisplayCommand(/*on*/ true, /*brightness*/ 0x01);

    // buttonTimedInit(&pushButton);
    rotaryEncoderInit(&rotaryEncoder, ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN);

    statemachineInit(&statemachine, &statemachineHandlerConfigureDelay);


    MinutesOrSeconds selectedDuration = {
        .minutesNotSeconds = false,
        .value = 0
    };

    while (true)
    {
        // Update with complete F_SYS_CLK.
        rotaryEncoderUpdate(&rotaryEncoder, ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN);

        if (updatePrescaler(&preScalerOne, PRE_SCALER_ONE_INIT))
        {
            // Update with F_SYS_CLK / (PRE_SCALER_ONE_INIT + 1).
            buttonTimedUpdate(&pushButton, PUSH_BUTTON_PIN);

            statemachineProcess(&statemachine, &statemachineData);

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


            // if (buttonReleasedAfterLong(&pushButton))
            // {
            //     selectedDuration.minutesNotSeconds = false;
            //     selectedDuration.value = 0;
            // }

            // // int8_t const rotation = rotaryEncoderPeekAccumulatedRotation(&rotaryEncoder);
            // int8_t const rotation = rotaryEncoderGetAndResetAccumulatedRotation(&rotaryEncoder);

            // rotaryEncoderRotationAppliedToMinutesOrSeconds(&selectedDuration, rotation);

            // tm1637RenderDurationMinutesOrSeconds(&selectedDuration);

            // // Duration const duration = millis();
            // // tm1637RenderTime(&duration);

            // tm1637Show();
        }
        else
        {
            // intentionally empty
        }

        PCON |= (1 << 0);  // PCON.IDL[0] = 1 - Enter idle mode
    }
}
