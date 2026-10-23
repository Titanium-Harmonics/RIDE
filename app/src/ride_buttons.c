#include "ride_buttons.h"

#include <stddef.h>
#include <string.h>

static const ride_action_mapping_t defaults[RIDE_BTN_COUNT] = {
    [RIDE_BTN_REC] = {RIDE_CMD_RECORD_TOGGLE, RIDE_CMD_RECORD_TOGGLE},
    [RIDE_BTN_LEFT] = {RIDE_CMD_NAV_PREVIOUS, RIDE_CMD_NAV_PREVIOUS},
    [RIDE_BTN_RIGHT] = {RIDE_CMD_NAV_NEXT, RIDE_CMD_NAV_NEXT},
    [RIDE_BTN_ENTER] = {RIDE_CMD_NAV_SELECT, RIDE_CMD_NAV_SELECT},
    [RIDE_BTN_BACK] = {RIDE_CMD_NAV_BACK, RIDE_CMD_UI_GO_HOME},
    [RIDE_BTN_ACTION] = {RIDE_CMD_EVENT_LOG, RIDE_CMD_EVENT_LOG},
};

static bool valid_command(ride_command_t command)
{
    return command >= RIDE_CMD_NONE && command <= RIDE_CMD_EVENT_LOG;
}

void ride_buttons_init(ride_button_engine_t* engine, uint32_t long_press_ms)
{
    if (engine == NULL)
    {
        return;
    }
    memset(engine, 0, sizeof(*engine));
    engine->active_button = RIDE_BTN_COUNT;
    engine->action = defaults[RIDE_BTN_ACTION];
    engine->long_press_ms = long_press_ms == 0 ? 800U : long_press_ms;
}

bool ride_buttons_set_action_mapping(ride_button_engine_t* engine, ride_action_mapping_t mapping)
{
    if (engine == NULL || !valid_command(mapping.short_press) || !valid_command(mapping.long_press))
    {
        return false;
    }
    engine->action = mapping;
    return true;
}

ride_command_t ride_buttons_resolve(const ride_button_engine_t* engine, ride_button_event_t event)
{
    if (engine == NULL || event.button < RIDE_BTN_REC || event.button >= RIDE_BTN_COUNT ||
        (event.press != RIDE_PRESS_SHORT && event.press != RIDE_PRESS_LONG))
    {
        return RIDE_CMD_NONE;
    }
    ride_action_mapping_t mapping =
        event.button == RIDE_BTN_ACTION ? engine->action : defaults[event.button];
    return event.press == RIDE_PRESS_SHORT ? mapping.short_press : mapping.long_press;
}

static void emit_event(ride_button_event_fn emit, void* context, ride_button_t button,
                       ride_press_t press)
{
    if (emit != NULL)
    {
        emit((ride_button_event_t){button, press}, context);
    }
}

void ride_buttons_edge(ride_button_engine_t* engine, ride_button_t button, bool down,
                       uint32_t now_ms, ride_button_event_fn emit, void* context)
{
    if (engine == NULL || button < RIDE_BTN_REC || button >= RIDE_BTN_COUNT)
    {
        return;
    }
    ride_button_state_t* state = &engine->states[button];
    if (down)
    {
        if (state->down || engine->ignored_down[button])
        {
            return;
        }
        if (engine->active_button != RIDE_BTN_COUNT)
        {
            engine->ignored_down[button] = true;
            return;
        }
        engine->active_button = button;
        state->down = true;
        state->pressed_at_ms = now_ms;
        return;
    }
    if (engine->ignored_down[button])
    {
        engine->ignored_down[button] = false;
        return;
    }
    if (!state->down || engine->active_button != button)
    {
        return;
    }
    state->down = false;
    engine->active_button = RIDE_BTN_COUNT;
    uint32_t duration = now_ms - state->pressed_at_ms;
    emit_event(emit, context, button,
               duration >= engine->long_press_ms ? RIDE_PRESS_LONG : RIDE_PRESS_SHORT);
}

void ride_buttons_tick(ride_button_engine_t* engine, uint32_t now_ms, ride_button_event_fn emit,
                       void* context)
{
    /* Classification is determined at release. No action fires while held. */
    (void)engine;
    (void)now_ms;
    (void)emit;
    (void)context;
}
