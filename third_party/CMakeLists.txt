
set(TARGET_NAME stc8hLib)
set(TARGET_DIRECTORY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/STC8H1k08-demos-master)

add_library(${TARGET_NAME}
    ${TARGET_DIRECTORY_PATH}/src/hardware.c
    ${TARGET_DIRECTORY_PATH}/src/i2c.c
    ${TARGET_DIRECTORY_PATH}/src/putchar.c
    ${TARGET_DIRECTORY_PATH}/src/usart.c
)

target_include_directories(${TARGET_NAME}
    PUBLIC ${TARGET_DIRECTORY_PATH}/inc
)

unset(TARGET_DIRECTORY_PATH)
unset(TARGET_NAME)
