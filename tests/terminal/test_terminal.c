#include "ride_terminal.h"

#include <assert.h>
#include <stdio.h>

static void expect_simple(const char* input, ride_terminal_command_type_t type)
{
    ride_terminal_command_t command;

    assert(ride_terminal_parse(input, &command));
    assert(command.type == type);
}

static void expect_invalid(const char* input)
{
    ride_terminal_command_t command;

    assert(!ride_terminal_parse(input, &command));
    assert(command.type == RIDE_TERMINAL_INVALID);
}

int main(void)
{
    ride_terminal_command_t command;

    expect_simple("help", RIDE_TERMINAL_HELP);
    expect_simple(" status ", RIDE_TERMINAL_STATUS);
    expect_simple("buttons status", RIDE_TERMINAL_BUTTONS_STATUS);

    assert(ride_terminal_parse("button left press", &command));
    assert(command.button == RIDE_BTN_LEFT);
    assert(command.action == RIDE_TERMINAL_PRESS);

    assert(ride_terminal_parse("button left release", &command));
    assert(command.action == RIDE_TERMINAL_RELEASE);

    expect_invalid("");
    expect_invalid(" ");
    expect_invalid("button left");
    expect_invalid("button unknown short");
    expect_invalid("button rec hold");
    expect_invalid("button rec short");
    expect_invalid("button back long");
    expect_invalid("help extra");
    expect_invalid("button rec short extra");
    expect_invalid("BUTTON REC SHORT");
    expect_invalid(NULL);
    assert(!ride_terminal_parse("help", NULL));

    puts("PASS: terminal command parser");
    return 0;
}
