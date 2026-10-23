#include "ride_terminal.h"

#include <ctype.h>
#include <stddef.h>
#include <string.h>

#define RIDE_TERMINAL_MAX_LINE 96U
#define RIDE_TERMINAL_MAX_TOKENS 4U

static bool token_equals(const char* token, const char* name)
{
    return strcmp(token, name) == 0;
}

static bool parse_button(const char* token, ride_button_t* button)
{
    static const char* const names[] = {"rec", "left", "right", "enter", "back", "action"};

    for (unsigned int i = 0U; i < RIDE_BTN_COUNT; ++i)
    {
        if (token_equals(token, names[i]))
        {
            *button = (ride_button_t)i;
            return true;
        }
    }

    return false;
}

static bool parse_action(const char* token, ride_terminal_button_action_t* action)
{
    static const char* const names[] = {"press", "release"};

    for (unsigned int i = 0U; i < 2U; ++i)
    {
        if (token_equals(token, names[i]))
        {
            *action = (ride_terminal_button_action_t)i;
            return true;
        }
    }

    return false;
}

bool ride_terminal_parse(const char* line, ride_terminal_command_t* output)
{
    char buffer[RIDE_TERMINAL_MAX_LINE];
    char* tokens[RIDE_TERMINAL_MAX_TOKENS] = {NULL};
    size_t length;
    unsigned int count = 0U;
    char* cursor;

    if (output == NULL)
    {
        return false;
    }

    memset(output, 0, sizeof(*output));
    output->type = RIDE_TERMINAL_INVALID;

    if (line == NULL)
    {
        return false;
    }

    length = strlen(line);
    if (length == 0U || length >= sizeof(buffer))
    {
        return false;
    }

    memcpy(buffer, line, length + 1U);
    cursor = buffer;

    while (*cursor != '\0')
    {
        while (isspace((unsigned char)*cursor))
        {
            ++cursor;
        }

        if (*cursor == '\0')
        {
            break;
        }

        if (count >= RIDE_TERMINAL_MAX_TOKENS)
        {
            return false;
        }

        tokens[count++] = cursor;
        while (*cursor != '\0' && !isspace((unsigned char)*cursor))
        {
            ++cursor;
        }

        if (*cursor != '\0')
        {
            *cursor++ = '\0';
        }
    }

    if (count == 1U && token_equals(tokens[0], "help"))
    {
        output->type = RIDE_TERMINAL_HELP;
    }
    else if (count == 1U && token_equals(tokens[0], "status"))
    {
        output->type = RIDE_TERMINAL_STATUS;
    }
    else if (count == 2U && token_equals(tokens[0], "buttons") && token_equals(tokens[1], "status"))
    {
        output->type = RIDE_TERMINAL_BUTTONS_STATUS;
    }
    else if (count == 3U && token_equals(tokens[0], "button") &&
             parse_button(tokens[1], &output->button) && parse_action(tokens[2], &output->action))
    {
        output->type = RIDE_TERMINAL_BUTTON;
    }

    return output->type != RIDE_TERMINAL_INVALID;
}
