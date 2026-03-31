
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

add_library(Specifics_buttonRawDurationConversion_
    specifics_buttonRawDurationConversion_.c
)
add_library(Specifics_rotaryEncoderRotationAppliedToMinutes
    specifics_rotaryEncoderRotationAppliedToMinutes.c
)
add_library(Specifics_rotaryEncoderRotationToMinutesConversion
    specifics_rotaryEncoderRotationToMinutesConversion.c
)

add_library(Specifics INTERFACE)

target_link_libraries(Specifics
    INTERFACE Specifics_buttonRawDurationConversion_
    INTERFACE Specifics_rotaryEncoderRotationAppliedToMinutes
    INTERFACE Specifics_rotaryEncoderRotationToMinutesConversion
)