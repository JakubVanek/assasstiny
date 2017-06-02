#ifndef IR_H
#define IR_H

#include "config.h"

typedef enum : uint8_t {
	IDLE,
	HEADER_ON,
	HEADER_OFF,
	DATA_ON,
	DATA_OFF
} ir_state_t;

typedef enum : uint8_t {
    NEC_CODE,
    NEC_REPEAT
} ir_command_t;

typedef struct {
    ir_payload_t data;
    ir_size_t    length;
    ir_command_t command;
} ir_buffer_t;

typedef struct {
	// buffer pointer
    ir_buffer_t volatile *current_buffer;
    // state tracker
    ir_state_t            current_state;
    ir_time_t             current_ticks;
    // signaling
    bool                  notify_main;
    bool                  current_processed;
} irdata_t;

extern volatile irdata_t irdata;

void initIR();

#endif
