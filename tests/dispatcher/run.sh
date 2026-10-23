#!/bin/sh
set -eu
mkdir -p build/tests
${CC:-cc} -std=c99 -Wall -Wextra -Werror -pedantic -Iapp/include app/src/ride_buttons.c app/src/ride_input_manager.c app/src/ride_terminal.c app/src/ride_command_dispatcher.c tests/dispatcher/test_dispatcher.c -o build/tests/ride_dispatcher_test
build/tests/ride_dispatcher_test
