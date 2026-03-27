
add_executable(${PROJECT_NAME}
    main.c
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE stc8hLib
)

ihx_to_hex(${PROJECT_NAME})

include_directories("c:/Program Files/SDCC/include/")

# C:\Users\User\AppData\Local\Python\pythoncore-3.14-64\python.exe -m stcgal --port COM8 --baud 115200 --protocol stc8g "C:\Users\User\Documents\STC\build\UsbTurnoffTimer-SDCC-Release\src\UsbTurnoffTimer.hex"
