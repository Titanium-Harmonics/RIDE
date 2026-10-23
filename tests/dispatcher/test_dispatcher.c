#include "ride_command_dispatcher.h"

#include <assert.h>
#include <stdio.h>

static unsigned int events;
static ride_button_event_t last;

static void on_event(ride_button_event_t event, void* context)
{
    (void)context;
    last = event;
    ++events;
}

static ride_dispatch_result_t execute(ride_command_dispatcher_t* dispatcher, const char* text,
                                      uint32_t time_ms)
{
    ride_terminal_command_t command;
    assert(ride_terminal_parse(text, &command));
    return ride_command_dispatcher_execute(dispatcher, &command, time_ms);
}

int main(void)
{
    ride_button_engine_t engine;
    ride_input_manager_t inputs;
    ride_command_dispatcher_t dispatcher;
    ride_buttons_init(&engine, 800U);
    ride_input_manager_init(&inputs, &engine);
    ride_command_dispatcher_init(&dispatcher, &inputs, on_event, NULL);

    assert(execute(&dispatcher, "help", 0U) == RIDE_DISPATCH_OK);
    assert(execute(&dispatcher, "buttons status", 0U) == RIDE_DISPATCH_OK);
    assert(execute(&dispatcher, "button rec press", 100U) == RIDE_DISPATCH_OK);
    assert(ride_input_manager_is_pressed(&inputs, RIDE_BTN_REC));
    assert(execute(&dispatcher, "button rec release", 250U) == RIDE_DISPATCH_OK);
    assert(events == 1U && last.button == RIDE_BTN_REC && last.press == RIDE_PRESS_SHORT);
    assert(ride_buttons_resolve(&engine, last) == RIDE_CMD_RECORD_TOGGLE);

    assert(execute(&dispatcher, "button back press", 300U) == RIDE_DISPATCH_OK);
    assert(execute(&dispatcher, "button back release", 1300U) == RIDE_DISPATCH_OK);
    assert(events == 2U && last.press == RIDE_PRESS_LONG);
    assert(ride_buttons_resolve(&engine, last) == RIDE_CMD_UI_GO_HOME);

    ride_input_manager_set(&inputs, RIDE_INPUT_GPIO, RIDE_BTN_LEFT, true, 1400U, on_event, NULL);
    assert(execute(&dispatcher, "button left press", 1450U) == RIDE_DISPATCH_OK);
    assert(execute(&dispatcher, "button left release", 1500U) == RIDE_DISPATCH_OK);
    assert(events == 2U);
    ride_input_manager_set(&inputs, RIDE_INPUT_GPIO, RIDE_BTN_LEFT, false, 1600U, on_event, NULL);
    assert(events == 3U);

    ride_command_dispatcher_init(&dispatcher, NULL, on_event, NULL);
    assert(execute(&dispatcher, "button rec press", 1700U) == RIDE_DISPATCH_UNAVAILABLE);
    puts("PASS: command dispatcher");
    return 0;
}
