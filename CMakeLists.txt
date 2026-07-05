cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico2_w CACHE STRING "Board type")

include(pico-sdk/pico_sdk_init.cmake)

project(pico2w_dht22_ssd1306 C CXX ASM)

pico_sdk_init()

add_executable(pico2w_dht22_ssd1306 main.c dht22.c ssd1306.c)

pico_enable_stdio_usb(pico2w_dht22_ssd1306 1)

target_link_libraries(pico2w_dht22_ssd1306 pico_stdlib hardware_i2c)

pico_add_extra_outputs(pico2w_dht22_ssd1306)
