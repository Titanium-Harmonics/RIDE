#include "ride_buttons.h"

#include <assert.h>
#include <stdio.h>

typedef struct
{
    unsigned count;
    ride_button_event_t last;
} collector_t;

static void collect(ride_button_event_t event, void* context)
{
    collector_t* collector = context;
    collector->count++;
    collector->last = event;
}

int main(void)
{
    ride_button_engine_t engine;
    collector_t events = {0};
    ride_buttons_init(&engine, 800);
    assert(engine.long_press_ms == 800);
    assert(ride_buttons_resolve(&engine, (ride_button_event_t){RIDE_BTN_BACK, RIDE_PRESS_LONG}) ==
           RIDE_CMD_UI_GO_HOME);
    assert(
        ride_buttons_resolve(&engine, (ride_button_event_t){RIDE_BTN_ACTION, RIDE_PRESS_SHORT}) ==
        RIDE_CMD_EVENT_LOG);

    ride_buttons_edge(&engine, RIDE_BTN_REC, true, 100, collect, &events);
    ride_buttons_tick(&engine, 200, collect, &events);
    assert(events.count == 0);
    ride_buttons_edge(&engine, RIDE_BTN_REC, false, 250, collect, &events);
    assert(events.count == 1 && events.last.press == RIDE_PRESS_SHORT);

    ride_buttons_edge(&engine, RIDE_BTN_BACK, true, 1000, collect, &events);
    ride_buttons_tick(&engine, 1800, collect, &events);
    assert(events.count == 1);
    ride_buttons_edge(&engine, RIDE_BTN_BACK, false, 1900, collect, &events);
    assert(events.count == 2 && events.last.press == RIDE_PRESS_LONG);

    ride_buttons_edge(&engine, RIDE_BTN_ACTION, true, 2000, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_ACTION, false, 2800, collect, &events);
    assert(events.count == 3 && events.last.press == RIDE_PRESS_LONG);

    assert(ride_buttons_set_action_mapping(
        &engine, (ride_action_mapping_t){RIDE_CMD_NAV_SELECT, RIDE_CMD_EVENT_LOG}));
    assert(ride_buttons_resolve(&engine, (ride_button_event_t){RIDE_BTN_ACTION, RIDE_PRESS_LONG}) ==
           RIDE_CMD_EVENT_LOG);
    assert(ride_buttons_resolve(&engine, (ride_button_event_t){RIDE_BTN_BACK, RIDE_PRESS_LONG}) ==
           RIDE_CMD_UI_GO_HOME);
    assert(!ride_buttons_set_action_mapping(
        &engine, (ride_action_mapping_t){(ride_command_t)999, RIDE_CMD_NONE}));

    ride_buttons_edge(&engine, RIDE_BTN_LEFT, true, UINT32_MAX - 200, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_LEFT, false, 100, collect, &events);
    assert(events.count == 4 && events.last.press == RIDE_PRESS_SHORT);

    /* Overlapping presses: only the first button may generate an event. */
    ride_buttons_edge(&engine, RIDE_BTN_REC, true, 3000, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_LEFT, true, 3010, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_REC, false, 3100, collect, &events);
    assert(events.count == 5 && events.last.button == RIDE_BTN_REC);
    ride_buttons_edge(&engine, RIDE_BTN_LEFT, true, 3110, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_LEFT, false, 3120, collect, &events);
    assert(events.count == 5);
    ride_buttons_edge(&engine, RIDE_BTN_LEFT, true, 3200, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_LEFT, false, 3300, collect, &events);
    assert(events.count == 6 && events.last.button == RIDE_BTN_LEFT);

    /* An ignored button released before the owner cannot steal ownership. */
    ride_buttons_edge(&engine, RIDE_BTN_BACK, true, 4000, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_ACTION, true, 4010, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_ACTION, false, 4020, collect, &events);
    assert(events.count == 6);
    ride_buttons_edge(&engine, RIDE_BTN_BACK, false, 4100, collect, &events);
    assert(events.count == 7 && events.last.button == RIDE_BTN_BACK);

    for (int b = RIDE_BTN_REC; b < RIDE_BTN_COUNT; ++b)
    {
        for (int p = RIDE_PRESS_SHORT; p <= RIDE_PRESS_LONG; ++p)
        {
            ride_command_t cmd = ride_buttons_resolve(&engine, (ride_button_event_t){b, p});
            assert(cmd >= RIDE_CMD_NONE && cmd <= RIDE_CMD_EVENT_LOG);
        }
    }
    puts("PASS: button engine and mappings");
    return 0;
}
