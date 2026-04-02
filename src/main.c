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

#include <limits.h>
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

// 20 Hz -> 12.5 s
#define DISPLAY_OFF_TIMEOUT (256 - 1)


static uint8_t preScalerOne = PRE_SCALER_ONE_INIT;
static uint8_t displayTurnOffCounter = DISPLAY_OFF_TIMEOUT;

static void tm1637ShowIfOnOnly()
{
    if (0 != displayTurnOffCounter)
    {
        tm1637Show();
    }
    else
    {
        // intentionally empty
    }
}

// HW inputs
static ButtonTimed pushButton;
static RotaryEncoder rotaryEncoder;

// Statemachine

typedef struct
{
    uint16_t delayDurationMinutes;
    MinutesOrSeconds offDuration;
    Timestamp nextTimeToAct;
    bool outputOn;
    uint8_t cycleCounter;
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
        tm1637ShowIfOnOnly();

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
            tm1637ShowIfOnOnly();
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
        tm1637ShowIfOnOnly();

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
                nextHandler = &statemachineHandlerConfigureDelay;
            }
            else if (buttonReleasedAfterLong(&pushButton))
            {
                nextHandler = &statemachineHandlerCountdown;
            }
            else
            {
                // handle rotation
                rotaryEncoderRotationAppliedToMinutesOrSeconds(&data->offDuration, rotation);
                updateDisplay = (0 != rotation);
            }
        }
        else if ((1 == data->cycleCounter) || /*early exit*/ (0 != rotation))
        {
            data->cycleCounter = 0;
            updateDisplay = true;
            tm1637RenderColon(false);
        }
        else // if (1 != data->cycleCounter)
        {
            --data->cycleCounter;
        }

        if (updateDisplay)
        {
            tm1637RenderDurationMinutesOrSeconds(&data->offDuration);
            tm1637ShowIfOnOnly();
        }
        else
        {
            // intentionally empty
        }
        break;
    }
    case StatemachineStageDeinit:
    {
        break;
    }
    }
    return (FunctionPointerPrototype)nextHandler;
}

#define NUMERIC_OVERFLOW_UNCERTAINTY ((Duration)-1000)

FunctionPointerPrototype statemachineHandlerCountdown(StatemachineStage const stage, void * const untypedData)
{
    StatemachineData * const data = (StatemachineData *)untypedData;
    StatemachineHandler nextHandler = &statemachineHandlerCountdown;
    switch (stage)
    {
    case StatemachineStageInit:
    {
        if (0 == data->offDuration.value)
        {
            // Show "idle" if offDuration is 0 - as then this would have no effect anyway.
            tm1637DisplayData[0] = tm1637Characters[tm1637Character_i];
            tm1637DisplayData[1] = tm1637Characters[tm1637Character_d];
            tm1637DisplayData[2] = tm1637Characters[tm1637Character_l];
            tm1637DisplayData[3] = tm1637Characters[tm1637Character_e];
        }
        else
        {
            tm1637DisplayData[0] = tm1637Characters[tm1637Character_w];
            tm1637DisplayData[1] = tm1637Characters[tm1637Character_a];
            tm1637DisplayData[2] = tm1637Characters[tm1637Character_i];
            tm1637DisplayData[3] = tm1637Characters[tm1637Character_t];

            // Show above text for this long.
            COMPILE_TIME_ASSERT(UCHAR_MAX - 20 > PRE_SCALER_TWO_INIT);
            data->cycleCounter = PRE_SCALER_TWO_INIT + 20;
        }

        // Calculate turn off time.
        Timestamp const now = millis();
        data->nextTimeToAct = now + (data->delayDurationMinutes * (60ull * 1000ull));

        PWR_SWITCH_PIN = 1;
        data->outputOn = true;

        tm1637RenderColon(false);
        tm1637ShowIfOnOnly();

        break;
    }
    case StatemachineStageProcess:
    {
        int8_t const rotation = rotaryEncoderGetAndResetAccumulatedRotation(&rotaryEncoder);

        bool updateDisplay = false;

        // Update nextTimeToAct - both if active or idle.
        Timestamp const now = millis();
        Duration remainingDelay = data->nextTimeToAct - now;

        // Allow for numeric overflow.
        if (NUMERIC_OVERFLOW_UNCERTAINTY < remainingDelay)
        {
            remainingDelay = 0;
        }
        else
        {
            // intentionally empty
        }

        if (0 == remainingDelay)
        {
            // Only actually switch off if offDuration is not 0, otherwise simply increment nextTimeToAct.
            if (data->outputOn && (0 != data->offDuration.value))
            {
                PWR_SWITCH_PIN = 0;
                data->outputOn = false;

                data->nextTimeToAct += minutesOrSecondsToDuration(&data->offDuration);
            }
            else
            {
                PWR_SWITCH_PIN = 1;
                data->outputOn = true;

                data->nextTimeToAct += MAX_24HOURS_MINUTES * 60ull * 1000ull;
                data->nextTimeToAct -= minutesOrSecondsToDuration(&data->offDuration);
            }
        }
        else
        {
            // intentionally empty
        }

        if (0 == data->offDuration.value)
        {
            // idle
            if (buttonReleasedAfterLong(&pushButton))
            {
                nextHandler = &statemachineHandlerConfigureDelay;
            }
            else
            {
                // intentionally empty
            }
        }
        else
        {
            if (PRE_SCALER_TWO_INIT >= data->cycleCounter)
            {
                // Initial display of text over.

                if (buttonReleasedAfterLong(&pushButton))
                {
                    nextHandler = &statemachineHandlerConfigureDelay;
                }
                else
                {
                    // ignore every other input

                    if (updatePrescaler(&data->cycleCounter, PRE_SCALER_TWO_INIT))
                    {
                        // Update with F_SYS_CLK / (PRE_SCALER_ONE_INIT + 1) / (PRE_SCALER_TWO_INIT + 1).
                        tm1637RenderColon(/*enabled*/ /*don't blink when off*/ data->outputOn && !tm1637GetRenderColon());
                        // tm1637AddressCommand(/*address*/ 1, &tm1637DisplayData[1], /*count*/ 1);

                        updateDisplay = true;
                    }
                    else
                    {
                        // intentionally empty
                    }
                }
            }
            else if (((1 + PRE_SCALER_TWO_INIT) == data->cycleCounter) || /*early exit*/ (0 != rotation))
            {
                data->cycleCounter = PRE_SCALER_TWO_INIT;
                updateDisplay = true;
                tm1637RenderColon(true);
            }
            else
            {
                // Initial display of text.
                --data->cycleCounter;
            }


            if (updateDisplay)
            {
                // ms -> seconds
                remainingDelay /= 1000;

                if (/*1 hour*/ 60ull * 60ull <= remainingDelay)
                {
                    // seconds -> minutes
                    remainingDelay /= 60;
                }
                else
                {
                    // intentionally empty
                }

                // Always add 1 as to compensate for integer division always rounding down.
                remainingDelay += 1;
                uint16_t const displayDuration = remainingDelay;

                tm1637RenderDurationMinutes(displayDuration);
                tm1637ShowIfOnOnly();
            }
            else
            {
                // intentionally empty
            }
        }

        break;
    }
    case StatemachineStageDeinit:
    {
        // Determine delayDurationMinutes as per current time.

        Timestamp const now = millis();
        Duration durationTillNextAction = data->nextTimeToAct - now;

        if (NUMERIC_OVERFLOW_UNCERTAINTY < durationTillNextAction)
        {
            // If data->nextTimeToAct overstepped between the last process and this deinit clip to 0 here.
            durationTillNextAction = 0;
        }
        else
        {
            // intentionally empty
        }

        // compensate off-duration
        if (!data->outputOn /*&& (0 != data->offDuration.value)*/)
        {
            durationTillNextAction += MAX_24HOURS_MINUTES * 60ull * 1000ull;
            durationTillNextAction -= minutesOrSecondsToDuration(&data->offDuration);
        }
        else
        {
            // intentionally empty
        }

        durationTillNextAction /= 60000u;  // milliseconds -> minutes

        data->delayDurationMinutes = durationTillNextAction;


        // Turn output back on.
        PWR_SWITCH_PIN = 1;
        data->outputOn = true;

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

    // P1M0 = (0x00 /* DIO_MODE_HIGH_Z_INPUT_M0 */);
    // P1M1 = (0xff /* DIO_MODE_HIGH_Z_INPUT_M1 */);

    COMPILE_TIME_ASSERT((0 <= PUSH_BUTTON_PIN_NUMBER) && (8 > PUSH_BUTTON_PIN_NUMBER));
    COMPILE_TIME_ASSERT((0 <= PWR_SWITCH_PIN_NUMBER) && (8 > PWR_SWITCH_PIN_NUMBER));
    COMPILE_TIME_ASSERT((0 <= SEVEN_SEGMENT_DATA_PIN_NUMBER) && (8 > SEVEN_SEGMENT_DATA_PIN_NUMBER));
    COMPILE_TIME_ASSERT((0 <= SEVEN_SEGMENT_CLK_PIN_NUMBER) && (8 > SEVEN_SEGMENT_CLK_PIN_NUMBER));
    COMPILE_TIME_ASSERT(3 == PUSH_BUTTON_PORT_NUMBER);
    COMPILE_TIME_ASSERT(3 == PWR_SWITCH_PORT_NUMBER);
    COMPILE_TIME_ASSERT(3 == SEVEN_SEGMENT_DATA_PORT_NUMBER);
    COMPILE_TIME_ASSERT(3 == SEVEN_SEGMENT_CLK_PORT_NUMBER);

    P3M0 = (0x00 /* DIO_MODE_HIGH_Z_INPUT_M0 */) |
           (DIO_MODE_HIGH_Z_INPUT_M0 << PUSH_BUTTON_PIN_NUMBER) |
           (DIO_MODE_PUSH_PULL_OUTPUT_M0 << PWR_SWITCH_PIN_NUMBER) |
           (DIO_MODE_OPEN_DRAIN_M0 << SEVEN_SEGMENT_DATA_PIN_NUMBER) |
           (DIO_MODE_OPEN_DRAIN_M0 << SEVEN_SEGMENT_CLK_PIN_NUMBER);
    P3M1 = (0xff /* DIO_MODE_HIGH_Z_INPUT_M1 */) &
           ((DIO_MODE_HIGH_Z_INPUT_M1 << PUSH_BUTTON_PIN_NUMBER) | ~(1 << PUSH_BUTTON_PIN_NUMBER)) &
           ((DIO_MODE_PUSH_PULL_OUTPUT_M1 << PWR_SWITCH_PIN_NUMBER) | ~(1 << PWR_SWITCH_PIN_NUMBER)) &
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
    tm1637DisplayCommand(/*on*/ true, /*brightness*/ 0x03);

    // buttonTimedInit(&pushButton);
    rotaryEncoderInit(&rotaryEncoder, ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN);

    // statemachineData.cycleCounter = 0;
    // statemachineData.delayDurationMinutes = 0;
    statemachineData.offDuration.minutesNotSeconds = false;
    statemachineData.offDuration.value = 5;
    // statemachineData.nextTimeToAct = 0ull;
    statemachineData.outputOn = PWR_SWITCH_PIN;

    statemachineInit(&statemachine, &statemachineHandlerConfigureDelay);

    while (true)
    {
        // Update with complete F_SYS_CLK.
        rotaryEncoderUpdate(&rotaryEncoder, ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN);

        if (updatePrescaler(&preScalerOne, PRE_SCALER_ONE_INIT))
        {
            // Update with F_SYS_CLK / (PRE_SCALER_ONE_INIT + 1).
            buttonTimedUpdate(&pushButton, PUSH_BUTTON_PIN);

            // Look for any user interaction - if none, disable the display after a timeout.
            if (buttonIsDown(&pushButton) ||
                    (0 != rotaryEncoderPeekAccumulatedRotation(&rotaryEncoder)))
            {
                if (0 == displayTurnOffCounter)
                {
                    tm1637Show();
                    tm1637DisplayCommand(/*on*/ true, /*brightness*/ 0x03);
                }
                else
                {
                    // intentionally empty
                }

                displayTurnOffCounter = DISPLAY_OFF_TIMEOUT;
            }
            else
            {
                if (0 == displayTurnOffCounter)
                {
                    // intentionally empty
                }
                else if (1 != displayTurnOffCounter)
                {
                    --displayTurnOffCounter;
                }
                else
                {
                    --displayTurnOffCounter;
                    tm1637DisplayCommand(/*on*/ false, /*brightness*/ 0x03);
                }
            }

            statemachineProcess(&statemachine, &statemachineData);
        }
        else
        {
            // intentionally empty
        }

        PCON |= (1 << 0);  // PCON.IDL[0] = 1 - Enter idle mode
    }
}
