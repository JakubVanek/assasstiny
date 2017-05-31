#ifndef IR_H
#define IR_H

#include "config.h"

typedef struct {
    ir_sample_t buffer[IR_FIFO_SIZE];
    ir_sample_t current_ticks;
    ir_size_t   buffer_size;
    bool        current_state;
    bool        notify_state;
} irdata_t;

extern volatile irdata_t irdata;

#endif