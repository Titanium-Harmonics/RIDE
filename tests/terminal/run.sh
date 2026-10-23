#!/bin/sh
set -eu

mkdir -p build/tests
${CC:-cc} -std=c99 -Wall -Wextra -Werror -pedantic \
    -Iapp/include app/src/ride_terminal.c tests/terminal/test_terminal.c \
    -o build/tests/ride_terminal_test
build/tests/ride_terminal_test
