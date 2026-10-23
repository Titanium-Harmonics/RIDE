#!/bin/sh
set -eu
mkdir -p build/tests
${CC:-cc} -std=c99 -Wall -Wextra -Werror -pedantic \
    -Iapp/include app/src/ride_buttons.c app/src/ride_input_manager.c \
    tests/input_manager/test_input_manager.c -o build/tests/ride_input_manager_test
build/tests/ride_input_manager_test
