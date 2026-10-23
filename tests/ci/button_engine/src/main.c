#include "ride_buttons.h"
#include <zephyr/ztest.h>

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

ZTEST(ride_button_engine, test_default_mapping)
{
    ride_button_engine_t engine;
    ride_buttons_init(&engine, 800);
    zassert_equal(engine.long_press_ms, 800U);
    const ride_command_t short_commands[] = {RIDE_CMD_RECORD_TOGGLE, RIDE_CMD_NAV_PREVIOUS,
                                             RIDE_CMD_NAV_NEXT,      RIDE_CMD_NAV_SELECT,
                                             RIDE_CMD_NAV_BACK,      RIDE_CMD_EVENT_LOG};
    const ride_command_t long_commands[] = {RIDE_CMD_RECORD_TOGGLE, RIDE_CMD_NAV_PREVIOUS,
                                            RIDE_CMD_NAV_NEXT,      RIDE_CMD_NAV_SELECT,
                                            RIDE_CMD_UI_GO_HOME,    RIDE_CMD_EVENT_LOG};
    for (int i = 0; i < RIDE_BTN_COUNT; ++i)
    {
        zassert_equal(ride_buttons_resolve(&engine, (ride_button_event_t){i, RIDE_PRESS_SHORT}),
                      short_commands[i]);
        zassert_equal(ride_buttons_resolve(&engine, (ride_button_event_t){i, RIDE_PRESS_LONG}),
                      long_commands[i]);
    }
}

ZTEST(ride_button_engine, test_short_press_on_release)
{
    ride_button_engine_t engine;
    collector_t events = {0};
    ride_buttons_init(&engine, 800);
    ride_buttons_edge(&engine, RIDE_BTN_REC, true, 100, collect, &events);
    ride_buttons_tick(&engine, 200, collect, &events);
    zassert_equal(events.count, 0U);
    ride_buttons_edge(&engine, RIDE_BTN_REC, false, 250, collect, &events);
    zassert_equal(events.count, 1U);
    zassert_equal(events.last.button, RIDE_BTN_REC);
    zassert_equal(events.last.press, RIDE_PRESS_SHORT);
}

ZTEST(ride_button_engine, test_long_press_on_release_only)
{
    ride_button_engine_t engine;
    collector_t events = {0};
    ride_buttons_init(&engine, 800);
    ride_buttons_edge(&engine, RIDE_BTN_BACK, true, 1000, collect, &events);
    ride_buttons_tick(&engine, 1800, collect, &events);
    zassert_equal(events.count, 0U);
    ride_buttons_edge(&engine, RIDE_BTN_BACK, false, 1800, collect, &events);
    zassert_equal(events.count, 1U);
    zassert_equal(events.last.press, RIDE_PRESS_LONG);
    zassert_equal(ride_buttons_resolve(&engine, events.last), RIDE_CMD_UI_GO_HOME);
    ride_buttons_edge(&engine, RIDE_BTN_BACK, false, 1900, collect, &events);
    zassert_equal(events.count, 1U);
}

ZTEST(ride_button_engine, test_action_customization)
{
    ride_button_engine_t engine;
    ride_buttons_init(&engine, 800);
    zassert_true(ride_buttons_set_action_mapping(
        &engine, (ride_action_mapping_t){RIDE_CMD_NAV_SELECT, RIDE_CMD_EVENT_LOG}));
    zassert_equal(
        ride_buttons_resolve(&engine, (ride_button_event_t){RIDE_BTN_ACTION, RIDE_PRESS_SHORT}),
        RIDE_CMD_NAV_SELECT);
    zassert_equal(
        ride_buttons_resolve(&engine, (ride_button_event_t){RIDE_BTN_ACTION, RIDE_PRESS_LONG}),
        RIDE_CMD_EVENT_LOG);
    zassert_false(ride_buttons_set_action_mapping(
        &engine, (ride_action_mapping_t){(ride_command_t)999, RIDE_CMD_NONE}));
    zassert_equal(
        ride_buttons_resolve(&engine, (ride_button_event_t){RIDE_BTN_BACK, RIDE_PRESS_LONG}),
        RIDE_CMD_UI_GO_HOME);
}

ZTEST(ride_button_engine, test_exclusive_button_ownership)
{
    ride_button_engine_t engine;
    collector_t events = {0};
    ride_buttons_init(&engine, 800);
    ride_buttons_edge(&engine, RIDE_BTN_REC, true, 100, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_LEFT, true, 110, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_REC, false, 200, collect, &events);
    zassert_equal(events.count, 1U);
    zassert_equal(events.last.button, RIDE_BTN_REC);
    ride_buttons_edge(&engine, RIDE_BTN_LEFT, true, 210, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_LEFT, false, 220, collect, &events);
    zassert_equal(events.count, 1U);
    ride_buttons_edge(&engine, RIDE_BTN_LEFT, true, 300, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_LEFT, false, 400, collect, &events);
    zassert_equal(events.count, 2U);
    zassert_equal(events.last.button, RIDE_BTN_LEFT);
}

ZTEST(ride_button_engine, test_ignored_button_released_early)
{
    ride_button_engine_t engine;
    collector_t events = {0};
    ride_buttons_init(&engine, 800);
    ride_buttons_edge(&engine, RIDE_BTN_BACK, true, 100, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_ACTION, true, 110, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_ACTION, false, 120, collect, &events);
    zassert_equal(events.count, 0U);
    ride_buttons_edge(&engine, RIDE_BTN_BACK, false, 200, collect, &events);
    zassert_equal(events.count, 1U);
    zassert_equal(events.last.button, RIDE_BTN_BACK);
}

ZTEST(ride_button_engine, test_timestamp_rollover)
{
    ride_button_engine_t engine;
    collector_t events = {0};
    ride_buttons_init(&engine, 800);
    ride_buttons_edge(&engine, RIDE_BTN_LEFT, true, UINT32_MAX - 200U, collect, &events);
    ride_buttons_edge(&engine, RIDE_BTN_LEFT, false, 100U, collect, &events);
    zassert_equal(events.count, 1U);
    zassert_equal(events.last.press, RIDE_PRESS_SHORT);
}

ZTEST_SUITE(ride_button_engine, NULL, NULL, NULL, NULL, NULL);
