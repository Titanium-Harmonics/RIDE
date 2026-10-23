#include "ride_input_manager.h"

#include <stddef.h>
#include <string.h>

static bool valid_source(ride_input_source_t source)
{
    return source >= RIDE_INPUT_GPIO && source < RIDE_INPUT_SOURCE_COUNT;
}

static bool valid_button(ride_button_t button)
{
    return button >= RIDE_BTN_REC && button < RIDE_BTN_COUNT;
}

bool ride_input_manager_is_pressed(const ride_input_manager_t* manager, ride_button_t button)
{
    if (manager == NULL || !valid_button(button))
    {
        return false;
    }

    for (unsigned int source = 0U; source < RIDE_INPUT_SOURCE_COUNT; ++source)
    {
        if (manager->pressed[source][button])
        {
            return true;
        }
    }
    return false;
}

void ride_input_manager_init(ride_input_manager_t* manager, ride_button_engine_t* engine)
{
    if (manager == NULL)
    {
        return;
    }
    memset(manager, 0, sizeof(*manager));
    manager->engine = engine;
}

void ride_input_manager_set(ride_input_manager_t* manager, ride_input_source_t source,
                            ride_button_t button, bool pressed, uint32_t now_ms,
                            ride_button_event_fn emit, void* context)
{
    if (manager == NULL || manager->engine == NULL || !valid_source(source) ||
        !valid_button(button))
    {
        return;
    }

    bool before = ride_input_manager_is_pressed(manager, button);
    manager->pressed[source][button] = pressed;
    bool after = ride_input_manager_is_pressed(manager, button);
    if (before != after)
    {
        ride_buttons_edge(manager->engine, button, after, now_ms, emit, context);
    }
}

void ride_input_manager_clear_source(ride_input_manager_t* manager, ride_input_source_t source,
                                     uint32_t now_ms, ride_button_event_fn emit, void* context)
{
    if (manager == NULL || !valid_source(source))
    {
        return;
    }

    for (unsigned int button = 0U; button < RIDE_BTN_COUNT; ++button)
    {
        ride_input_manager_set(manager, source, (ride_button_t)button, false, now_ms, emit,
                               context);
    }
}
