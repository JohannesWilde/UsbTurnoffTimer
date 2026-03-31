
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
