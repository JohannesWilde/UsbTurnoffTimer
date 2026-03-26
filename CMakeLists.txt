cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE "toolchain.cmake")
project(UsbTurnoffTimer LANGUAGES C)

function(ihx_to_hex bin)
    add_custom_command(TARGET ${bin} POST_BUILD COMMAND  packihx ${bin}.ihx > ${bin}.hex)
endfunction(ihx_to_hex)

add_subdirectory(third_party)
add_subdirectory(src)
