
add_executable(${PROJECT_NAME}
    main.c
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE stc8hLib
)

ihx_to_hex(${PROJECT_NAME})

include_directories("c:/Program Files/SDCC/include/")
