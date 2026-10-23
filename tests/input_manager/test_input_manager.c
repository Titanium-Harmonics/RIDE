#include "ride_input_manager.h"

#include <assert.h>
#include <stdio.h>

typedef struct
{
    unsigned int count;
    ride_button_event_t last;
} capture_t;

static void capture(ride_button_event_t event, void* context)
{
    capture_t* result = context;
    result->count++;
    result->last = event;
}

int main(void)
{
    ride_button_engine_t engine;
    ride_input_manager_t manager;
    capture_t result = {0};

    ride_buttons_init(&engine, 800U);
    ride_input_manager_init(&manager, &engine);

    ride_input_manager_set(&manager, RIDE_INPUT_GPIO, RIDE_BTN_REC, true, 100U, capture, &result);
    ride_input_manager_set(&manager, RIDE_INPUT_CONSOLE, RIDE_BTN_REC, true, 120U, capture,
                           &result);
    ride_input_manager_set(&manager, RIDE_INPUT_GPIO, RIDE_BTN_REC, false, 200U, capture, &result);
    assert(ride_input_manager_is_pressed(&manager, RIDE_BTN_REC));
    assert(result.count == 0U);

    ride_input_manager_set(&manager, RIDE_INPUT_CONSOLE, RIDE_BTN_REC, false, 300U, capture,
                           &result);
    assert(result.count == 1U);
    assert(result.last.button == RIDE_BTN_REC);
    assert(result.last.press == RIDE_PRESS_SHORT);

    ride_input_manager_set(&manager, RIDE_INPUT_CONSOLE, RIDE_BTN_BACK, true, 400U, capture,
                           &result);
    ride_input_manager_set(&manager, RIDE_INPUT_KEYBOARD, RIDE_BTN_BACK, true, 500U, capture,
                           &result);
    ride_input_manager_clear_source(&manager, RIDE_INPUT_CONSOLE, 600U, capture, &result);
    assert(ride_input_manager_is_pressed(&manager, RIDE_BTN_BACK));
    assert(result.count == 1U);
    ride_input_manager_set(&manager, RIDE_INPUT_KEYBOARD, RIDE_BTN_BACK, false, 1300U, capture,
                           &result);
    assert(result.count == 2U);
    assert(result.last.button == RIDE_BTN_BACK);
    assert(result.last.press == RIDE_PRESS_LONG);

    ride_input_manager_set(&manager, RIDE_INPUT_GPIO, RIDE_BTN_LEFT, true, 1400U, capture, &result);
    ride_input_manager_set(&manager, RIDE_INPUT_CONSOLE, RIDE_BTN_RIGHT, true, 1450U, capture,
                           &result);
    ride_input_manager_set(&manager, RIDE_INPUT_GPIO, RIDE_BTN_LEFT, false, 1500U, capture,
                           &result);
    ride_input_manager_set(&manager, RIDE_INPUT_CONSOLE, RIDE_BTN_RIGHT, false, 1550U, capture,
                           &result);
    assert(result.count == 3U);
    assert(result.last.button == RIDE_BTN_LEFT);

    ride_input_manager_set(&manager, RIDE_INPUT_CONSOLE, RIDE_BTN_REC, true, 1600U, capture,
                           &result);
    ride_input_manager_clear_source(&manager, RIDE_INPUT_CONSOLE, 1700U, capture, &result);
    assert(result.count == 4U);
    assert(!ride_input_manager_is_pressed(&manager, RIDE_BTN_REC));

    ride_input_manager_set(&manager, RIDE_INPUT_SOURCE_COUNT, RIDE_BTN_REC, true, 1800U, capture,
                           &result);
    ride_input_manager_set(&manager, RIDE_INPUT_GPIO, RIDE_BTN_COUNT, true, 1800U, capture,
                           &result);
    assert(result.count == 4U);

    puts("PASS: input manager parallel sources");
    return 0;
}
