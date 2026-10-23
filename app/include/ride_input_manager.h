#ifndef RIDE_INPUT_MANAGER_H_
#define RIDE_INPUT_MANAGER_H_

#include "ride_buttons.h"

typedef enum
{
    RIDE_INPUT_GPIO,
    RIDE_INPUT_CONSOLE,
    RIDE_INPUT_KEYBOARD,
    RIDE_INPUT_SOURCE_COUNT
} ride_input_source_t;

typedef struct
{
    ride_button_engine_t* engine;
    bool pressed[RIDE_INPUT_SOURCE_COUNT][RIDE_BTN_COUNT];
} ride_input_manager_t;

void ride_input_manager_init(ride_input_manager_t* manager, ride_button_engine_t* engine);
void ride_input_manager_set(ride_input_manager_t* manager, ride_input_source_t source,
                            ride_button_t button, bool pressed, uint32_t now_ms,
                            ride_button_event_fn emit, void* context);
void ride_input_manager_clear_source(ride_input_manager_t* manager, ride_input_source_t source,
                                     uint32_t now_ms, ride_button_event_fn emit, void* context);
bool ride_input_manager_is_pressed(const ride_input_manager_t* manager, ride_button_t button);

#endif
