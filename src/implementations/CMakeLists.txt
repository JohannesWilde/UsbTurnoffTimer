
# Auxiliaries

add_library(Auxiliaries_absoluteValue_int8
    auxiliaries_absoluteValue_int8.c
)

add_library(Auxiliaries INTERFACE)

target_link_libraries(Auxiliaries
    INTERFACE Auxiliaries_absoluteValue_int8
)


# ButtonTimed

add_library(ButtonTimed_init
    buttontimed_init.c
)
add_library(ButtonTimed_releasedAfterLong
    buttontimed_releasedAfterLong.c
)
add_library(ButtonTimed_releasedAfterShort
    buttontimed_releasedAfterShort.c
)
add_library(ButtonTimed_update
    buttontimed_update.c
)

add_library(ButtonTimed INTERFACE)

target_link_libraries(ButtonTimed
    INTERFACE ButtonTimed_init
    INTERFACE ButtonTimed_releasedAfterLong
    INTERFACE ButtonTimed_releasedAfterShort
    INTERFACE ButtonTimed_update
)


# RotaryEncoder

add_library(RotaryEncoder_getAndResetAccumulatedRotation
    rotaryencoder_getAndResetAccumulatedRotation.c
)
add_library(RotaryEncoder_init
    rotaryencoder_init.c
)
add_library(RotaryEncoder_update
    rotaryencoder_update.c
)

add_library(RotaryEncoder INTERFACE)

target_link_libraries(RotaryEncoder
    INTERFACE RotaryEncoder_getAndResetAccumulatedRotation
    INTERFACE RotaryEncoder_init
    INTERFACE RotaryEncoder_update
)

# Specifics

add_library(Statemachine_Basics
    statemachine_Basics.c
)
add_library(Statemachine_Reset
    statemachine_Reset.c
)

add_library(Statemachine INTERFACE)

target_link_libraries(Statemachine
    INTERFACE Statemachine_Basics
    INTERFACE Statemachine_Reset
)

# Specifics

add_library(Specifics_buttonRawDurationConversion_
    specifics_buttonRawDurationConversion_.c
)
add_library(Specifics_millis
    specifics_millis.c
)
add_library(Specifics_minutesOrSecondsToDuration
    specifics_minutesOrSecondsToDuration.c
)
add_library(Specifics_rotaryEncoderRotationAppliedToMinutes
    specifics_rotaryEncoderRotationAppliedToMinutes.c
)
add_library(Specifics_rotaryEncoderRotationAppliedToMinutesOrSeconds
    specifics_rotaryEncoderRotationAppliedToMinutesOrSeconds.c
)
add_library(Specifics_rotaryEncoderRotationToMinutesConversion
    specifics_rotaryEncoderRotationToMinutesConversion.c
)

add_library(Specifics INTERFACE)

target_link_libraries(Specifics
    INTERFACE Specifics_buttonRawDurationConversion_
    INTERFACE Specifics_millis
    INTERFACE Specifics_minutesOrSecondsToDuration
    INTERFACE Specifics_rotaryEncoderRotationAppliedToMinutes
    INTERFACE Specifics_rotaryEncoderRotationAppliedToMinutesOrSeconds
    INTERFACE Specifics_rotaryEncoderRotationToMinutesConversion
)

# Tm1637Display

add_library(Tm1637Display_Characters
    tm1637display_Characters.c
)
add_library(Tm1637Display_DisplayData
    tm1637display_DisplayData.c
)
add_library(Tm1637Display_RenderColon
    tm1637display_RenderColon.c
)
add_library(Tm1637Display_RenderDuration
    tm1637display_RenderDuration.c
)
add_library(Tm1637Display_RenderDurationMinutes
    tm1637display_RenderDurationMinutes.c
)
add_library(Tm1637Display_RenderDurationMinutesOrSeconds
    tm1637display_RenderDurationMinutesOrSeconds.c
)
add_library(Tm1637Display_RenderNumberSigned
    tm1637display_RenderNumberSigned.c
)
add_library(Tm1637display_RenderNumberUnsigned
    tm1637display_RenderNumberUnsigned.c
)

add_library(Tm1637Display INTERFACE)

target_link_libraries(Tm1637Display
    INTERFACE Tm1637Display_Characters
    INTERFACE Tm1637Display_DisplayData
    INTERFACE Tm1637Display_RenderColon
    INTERFACE Tm1637Display_RenderDuration
    INTERFACE Tm1637Display_RenderDurationMinutes
    INTERFACE Tm1637Display_RenderDurationMinutesOrSeconds
    INTERFACE Tm1637Display_RenderNumberSigned
    INTERFACE Tm1637display_RenderNumberUnsigned
)


