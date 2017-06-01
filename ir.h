#ifndef IR_H
#define IR_H

#include "config.h"

typedef struct {
    ir_sample_t data[IR_FIFO_SIZE];
    ir_size_t   length;
} ir_buffer_t;

typedef struct {
    ir_buffer_t volatile *current_buffer;
    ir_sample_t           current_ticks;
    ir_size_t             buffer_size;
    bool                  current_state;
    bool                  notify_main;
    bool                  current_processed;
} irdata_t;

extern volatile irdata_t irdata;

void initIR();

#endif
