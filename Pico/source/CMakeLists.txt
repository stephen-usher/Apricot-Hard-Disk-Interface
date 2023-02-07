cmake_minimum_required(VERSION 3.13)

# Pull in Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(apricot-hd C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise the Pico SDK
pico_sdk_init()

add_subdirectory(FatFs_SPI build)

# Add executable. Default name is the project name, version 0.1
add_executable(apricot-hd
    main.c
    sd_card_config.c
    buffer.c
    gpio_ops.c
    read_config.c
    wd_command_interpreter.c
    host_interface.c
    debug.c
    create_image.c
)
# Add the standard library to the build
target_link_libraries(apricot-hd pico_stdlib pico_multicore)

#target_compile_options(apricot-hd PUBLIC -DAPRIDEBUG=3)
target_compile_options(apricot-hd PUBLIC)

pico_set_program_name(apricot-hd "apricot-hd")
pico_set_program_version(apricot-hd "0.1")

# See 4.1. Serial input and output on Raspberry Pi Pico in Getting started with Raspberry Pi Pico (https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf)
# and 2.7.1. Standard Input/Output (stdio) Support in Raspberry Pi Pico C/C++ SDK (https://datasheets.raspberrypi.org/pico/raspberry-pi-pico-c-sdk.pdf):
#pico_enable_stdio_uart(apricot-hd 1)
#pico_enable_stdio_usb(apricot-hd 1)

target_link_libraries(apricot-hd
    FatFs_SPI
    hardware_clocks
    hardware_adc
)

pico_add_extra_outputs(apricot-hd)

