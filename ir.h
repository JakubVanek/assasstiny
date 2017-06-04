#ifndef IR_H
#define IR_H

#include "config.h"

// IR buffer
typedef uint32_t ir_payload_t;

// IR time datatype
typedef int16_t ir_time_t;

// IR buffer size datatype
typedef uint8_t ir_size_t;

typedef enum {
	IDLE,
	HEADER_ON,
	HEADER_OFF,
	DATA_ON,
	DATA_OFF
} ir_state_t;

typedef enum {
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
    ir_buffer_t buffer;
    // state tracker
    ir_state_t  current_state;
    ir_time_t   current_ticks;
    // signaling
    bool        notify_main;
    bool        current_processed;
} irdata_t;

extern volatile irdata_t irdata;

void ir_init();

#endif
