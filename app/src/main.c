#include "ride_command_dispatcher.h"

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define RIDE_LINE_CAPACITY 96U

static ride_button_engine_t button_engine;
static ride_input_manager_t input_manager;
static ride_command_dispatcher_t dispatcher;

static const char* command_name(ride_command_t command)
{
    switch (command)
    {
        case RIDE_CMD_RECORD_TOGGLE: return "RECORD_TOGGLE";
        case RIDE_CMD_NAV_PREVIOUS: return "NAV_PREVIOUS";
        case RIDE_CMD_NAV_NEXT: return "NAV_NEXT";
        case RIDE_CMD_NAV_SELECT: return "NAV_SELECT";
        case RIDE_CMD_NAV_BACK: return "NAV_BACK";
        case RIDE_CMD_UI_GO_HOME: return "UI_GO_HOME";
        case RIDE_CMD_EVENT_LOG: return "EVENT_LOG";
        default: return "NONE";
    }
}

static void on_button_event(ride_button_event_t event, void* context)
{
    ride_button_engine_t* engine = context;
    printk("[E] %s\n", command_name(ride_buttons_resolve(engine, event)));
}

static void handle_line(const char* line)
{
    ride_terminal_command_t command;
    if (!ride_terminal_parse(line, &command))
    {
        printk("[ERR] invalid command; try help\n");
        return;
    }

    ride_dispatch_result_t result =
        ride_command_dispatcher_execute(&dispatcher, &command, k_uptime_get_32());
    if (result != RIDE_DISPATCH_OK)
    {
        printk("[ERR] command unavailable or invalid\n");
        return;
    }

    switch (command.type)
    {
        case RIDE_TERMINAL_HELP:
            printk("[OK] help | status | buttons status | button <name> press|release\n");
            break;
        case RIDE_TERMINAL_STATUS:
            printk("[OK] RIDE firmware running\n");
            break;
        case RIDE_TERMINAL_BUTTONS_STATUS:
            printk("[OK] active=%d\n", (int)button_engine.active_button);
            break;
        default:
            printk("[OK] button edge accepted\n");
            break;
    }
}

int main(void)
{
    const struct device* console = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
    char line[RIDE_LINE_CAPACITY];
    size_t used = 0U;

    ride_buttons_init(&button_engine, 800U);
    ride_input_manager_init(&input_manager, &button_engine);
    ride_command_dispatcher_init(&dispatcher, &input_manager, on_button_event, &button_engine);

    if (!device_is_ready(console))
    {
        printk("RIDE: console device unavailable\n");
        return 0;
    }

    printk("RIDE command terminal ready. Type help and press Enter.\n");
    while (1)
    {
        unsigned char ch;
        if (uart_poll_in(console, &ch) != 0)
        {
            k_sleep(K_MSEC(10));
            continue;
        }

        if (ch == '\r' || ch == '\n')
        {
            if (used != 0U)
            {
                line[used] = '\0';
                handle_line(line);
                used = 0U;
            }
        }
        else if (ch == 8U || ch == 127U)
        {
            if (used != 0U)
            {
                --used;
            }
        }
        else if (ch >= 32U && ch < 127U)
        {
            if (used + 1U < sizeof(line))
            {
                line[used++] = (char)ch;
            }
            else
            {
                used = 0U;
                printk("[ERR] line too long\n");
            }
        }
    }
}
