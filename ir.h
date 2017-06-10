#ifndef IR_H
#define IR_H

#include "config.h"

typedef enum ir_machinestate_t {
	ST_IDLE,
	ST_ON_HDR,
	ST_ON_DATA,
	ST_OFF_HDR,
	ST_OFF_DATA,

} ir_machinestate_t;

typedef enum ir_notify_t {
	IR_NONE, IR_RECEIVED
} ir_notify_t;

typedef enum ir_type_t {
	NEC_COMMAND,
	NEC_REPEAT
} ir_type_t;

typedef uint32_t ir_payload_t;
typedef uint8_t ir_time_t;

typedef struct ir_data_t {
	volatile ir_payload_t code;
	volatile ir_type_t    which;
	volatile ir_notify_t  notify;
	volatile ir_time_t    ledticks;

	ir_machinestate_t     state;
	ir_time_t             ticks;
} ir_data_t;

extern ir_data_t irdata;

void ir_init();

#endif
