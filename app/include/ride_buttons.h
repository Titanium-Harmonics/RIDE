#ifndef RIDE_BUTTONS_H_
#define RIDE_BUTTONS_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    RIDE_BTN_REC,
    RIDE_BTN_LEFT,
    RIDE_BTN_RIGHT,
    RIDE_BTN_ENTER,
    RIDE_BTN_BACK,
    RIDE_BTN_ACTION,
    RIDE_BTN_COUNT
} ride_button_t;

typedef enum
{
    RIDE_PRESS_SHORT,
    RIDE_PRESS_LONG
} ride_press_t;

typedef enum
{
    RIDE_CMD_NONE,
    RIDE_CMD_RECORD_TOGGLE,
    RIDE_CMD_NAV_PREVIOUS,
    RIDE_CMD_NAV_NEXT,
    RIDE_CMD_NAV_SELECT,
    RIDE_CMD_NAV_BACK,
    RIDE_CMD_UI_GO_HOME,
    RIDE_CMD_EVENT_LOG
} ride_command_t;

typedef struct
{
    ride_button_t button;
    ride_press_t press;
} ride_button_event_t;

typedef struct
{
    ride_command_t short_press;
    ride_command_t long_press;
} ride_action_mapping_t;

typedef struct
{
    bool down;
    bool long_emitted;
    uint32_t pressed_at_ms;
} ride_button_state_t;

typedef struct
{
    ride_button_state_t states[RIDE_BTN_COUNT];
    bool ignored_down[RIDE_BTN_COUNT];
    ride_button_t active_button;
    ride_action_mapping_t action;
    uint32_t long_press_ms;
} ride_button_engine_t;

typedef void (*ride_button_event_fn)(ride_button_event_t event, void* context);

void ride_buttons_init(ride_button_engine_t* engine, uint32_t long_press_ms);
bool ride_buttons_set_action_mapping(ride_button_engine_t* engine, ride_action_mapping_t mapping);
ride_command_t ride_buttons_resolve(const ride_button_engine_t* engine, ride_button_event_t event);
void ride_buttons_edge(ride_button_engine_t* engine, ride_button_t button, bool down,
                       uint32_t now_ms, ride_button_event_fn emit, void* context);
void ride_buttons_tick(ride_button_engine_t* engine, uint32_t now_ms, ride_button_event_fn emit,
                       void* context);

#endif
