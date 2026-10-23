#include "ride_command_dispatcher.h"

void ride_command_dispatcher_init(ride_command_dispatcher_t* dispatcher,
                                  ride_input_manager_t* inputs, ride_button_event_fn emit,
                                  void* context)
{
    if (!dispatcher)
        return;
    dispatcher->inputs = inputs;
    dispatcher->emit = emit;
    dispatcher->context = context;
}

ride_dispatch_result_t ride_command_dispatcher_execute(ride_command_dispatcher_t* dispatcher,
                                                       const ride_terminal_command_t* command,
                                                       uint32_t now_ms)
{
    if (!dispatcher || !command)
        return RIDE_DISPATCH_INVALID;
    if (command->type == RIDE_TERMINAL_HELP || command->type == RIDE_TERMINAL_STATUS ||
        command->type == RIDE_TERMINAL_BUTTONS_STATUS)
        return RIDE_DISPATCH_OK;
    if (command->type != RIDE_TERMINAL_BUTTON || command->button >= RIDE_BTN_COUNT ||
        (command->action != RIDE_TERMINAL_PRESS && command->action != RIDE_TERMINAL_RELEASE))
        return RIDE_DISPATCH_INVALID;
    if (!dispatcher->inputs || !dispatcher->inputs->engine)
        return RIDE_DISPATCH_UNAVAILABLE;
    ride_input_manager_set(dispatcher->inputs, RIDE_INPUT_CONSOLE, command->button,
                           command->action == RIDE_TERMINAL_PRESS, now_ms, dispatcher->emit,
                           dispatcher->context);
    return RIDE_DISPATCH_OK;
}
