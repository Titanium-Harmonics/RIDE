cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ride_mk1)

target_sources(app PRIVATE
    src/main.c
    src/ride_buttons.c
    src/ride_input_manager.c
    src/ride_terminal.c
    src/ride_command_dispatcher.c
    src/ride_shell.c
)
target_include_directories(app PRIVATE include)
