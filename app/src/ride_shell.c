#include "ride_shell.h"

#include "ride_command_dispatcher.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#include <errno.h>
#include <string.h>
#include <zephyr/shell/shell.h>

static ride_button_engine_t button_engine;
static ride_input_manager_t input_manager;
static ride_command_dispatcher_t dispatcher;
static const struct shell* active_shell;

static const char* command_name(ride_command_t command)
{
    switch (command)
    {
        case RIDE_CMD_RECORD_TOGGLE:
            return "RECORD_TOGGLE";
        case RIDE_CMD_NAV_PREVIOUS:
            return "NAV_PREVIOUS";
        case RIDE_CMD_NAV_NEXT:
            return "NAV_NEXT";
        case RIDE_CMD_NAV_SELECT:
            return "NAV_SELECT";
        case RIDE_CMD_NAV_BACK:
            return "NAV_BACK";
        case RIDE_CMD_UI_GO_HOME:
            return "UI_GO_HOME";
        case RIDE_CMD_EVENT_LOG:
            return "EVENT_LOG";
        default:
            return "NONE";
    }
}

static void on_button_event(ride_button_event_t event, void* context)
{
    const struct shell* sh = context != NULL ? context : active_shell;
    if (sh != NULL)
    {
        shell_print(sh, "[E] %s", command_name(ride_buttons_resolve(&button_engine, event)));
    }
}

static int dispatch(const struct shell* sh, ride_terminal_command_t command)
{
    active_shell = sh;
    dispatcher.context = (void*)sh;
    ride_dispatch_result_t result =
        ride_command_dispatcher_execute(&dispatcher, &command, k_uptime_get_32());
    if (result != RIDE_DISPATCH_OK)
    {
        shell_error(sh, "Command unavailable or invalid");
        return -EINVAL;
    }

    switch (command.type)
    {
        case RIDE_TERMINAL_HELP:
            shell_print(sh, "ride help | ride status | ride buttons status | ride button <name> press|release");
            break;
        case RIDE_TERMINAL_STATUS:
            shell_print(sh, "RIDE firmware running");
            break;
        case RIDE_TERMINAL_BUTTONS_STATUS:
            shell_print(sh, "active=%d", (int)button_engine.active_button);
            break;
        default:
            shell_print(sh, "[OK] button edge accepted");
            break;
    }
    return 0;
}

static int cmd_help(const struct shell* sh, size_t argc, char** argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    return dispatch(sh, (ride_terminal_command_t){.type = RIDE_TERMINAL_HELP});
}

static int cmd_status(const struct shell* sh, size_t argc, char** argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    return dispatch(sh, (ride_terminal_command_t){.type = RIDE_TERMINAL_STATUS});
}

static int cmd_buttons_status(const struct shell* sh, size_t argc, char** argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    return dispatch(sh, (ride_terminal_command_t){.type = RIDE_TERMINAL_BUTTONS_STATUS});
}

static int cmd_button(const struct shell* sh, size_t argc, char** argv)
{
    ARG_UNUSED(argc);
    static const char* const names[] = {"rec", "left", "right", "enter", "back", "action"};
    ride_button_t button = RIDE_BTN_COUNT;
    for (size_t i = 0; i < ARRAY_SIZE(names); ++i)
    {
        if (strcmp(argv[1], names[i]) == 0)
        {
            button = (ride_button_t)i;
            break;
        }
    }
    if (button == RIDE_BTN_COUNT)
    {
        shell_error(sh, "Unknown button");
        return -EINVAL;
    }

    ride_terminal_button_action_t action;
    if (strcmp(argv[2], "press") == 0)
        action = RIDE_TERMINAL_PRESS;
    else if (strcmp(argv[2], "release") == 0)
        action = RIDE_TERMINAL_RELEASE;
    else
    {
        shell_error(sh, "Expected press or release");
        return -EINVAL;
    }

    return dispatch(sh, (ride_terminal_command_t){
                            .type = RIDE_TERMINAL_BUTTON, .button = button, .action = action});

}

SHELL_STATIC_SUBCMD_SET_CREATE(
    buttons_subcommands,
    SHELL_CMD_ARG(status, NULL, "Show effective button state", cmd_buttons_status, 1, 0),
    SHELL_SUBCMD_SET_END);

SHELL_STATIC_SUBCMD_SET_CREATE(
    ride_subcommands,
    SHELL_CMD_ARG(help, NULL, "Show RIDE commands", cmd_help, 1, 0),
    SHELL_CMD_ARG(status, NULL, "Show firmware status", cmd_status, 1, 0),
    SHELL_CMD_ARG(buttons, &buttons_subcommands, "Button state commands", NULL, 1, 0),
    SHELL_CMD_ARG(button, NULL, "Inject a button edge: <name> press|release", cmd_button, 3, 0),
    SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(ride, &ride_subcommands, "R.I.D.E. firmware commands", NULL);

void ride_shell_init(void)
{
    ride_buttons_init(&button_engine, 800U);
    ride_input_manager_init(&input_manager, &button_engine);
    ride_command_dispatcher_init(&dispatcher, &input_manager, on_button_event, NULL);
}
