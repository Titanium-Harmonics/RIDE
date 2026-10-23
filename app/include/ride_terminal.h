#ifndef RIDE_TERMINAL_H_
#define RIDE_TERMINAL_H_

#include "ride_buttons.h"

typedef enum
{
    RIDE_TERMINAL_INVALID,
    RIDE_TERMINAL_HELP,
    RIDE_TERMINAL_STATUS,
    RIDE_TERMINAL_BUTTONS_STATUS,
    RIDE_TERMINAL_BUTTON
} ride_terminal_command_type_t;

typedef enum
{
    RIDE_TERMINAL_PRESS,
    RIDE_TERMINAL_RELEASE,
    RIDE_TERMINAL_SHORT,
    RIDE_TERMINAL_LONG
} ride_terminal_button_action_t;

typedef struct
{
    ride_terminal_command_type_t type;
    ride_button_t button;
    ride_terminal_button_action_t action;
} ride_terminal_command_t;

/* Parses one command without allocating memory or modifying the input. */
bool ride_terminal_parse(const char *line, ride_terminal_command_t *output);

#endif
