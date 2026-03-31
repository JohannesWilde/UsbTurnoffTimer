
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
