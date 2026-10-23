#ifndef RIDE_COMMAND_DISPATCHER_H_
#define RIDE_COMMAND_DISPATCHER_H_

#include "ride_input_manager.h"
#include "ride_terminal.h"

typedef enum
{
    RIDE_DISPATCH_OK,
    RIDE_DISPATCH_INVALID,
    RIDE_DISPATCH_UNAVAILABLE
} ride_dispatch_result_t;

typedef struct
{
    ride_input_manager_t* inputs;
    ride_button_event_fn emit;
    void* context;
} ride_command_dispatcher_t;

void ride_command_dispatcher_init(ride_command_dispatcher_t* dispatcher,
                                  ride_input_manager_t* inputs, ride_button_event_fn emit,
                                  void* context);
ride_dispatch_result_t ride_command_dispatcher_execute(ride_command_dispatcher_t* dispatcher,
                                                       const ride_terminal_command_t* command,
                                                       uint32_t now_ms);

#endif
