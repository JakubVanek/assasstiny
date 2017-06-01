#include "ir.h"

inline void led_isr();
inline void ir_isr();

volatile ir_buffer_t bufferA;
volatile ir_buffer_t bufferB;
volatile irdata_t    irdata;

volatile ir_buffer_t * volatile back_buffer;

void initIR() {
    bufferA.length           = 0;
    bufferB.length           = 0;
    back_buffer              = bufferA;
    irdata.current_buffer    = bufferB;
    irdata.current_ticks     = 0;
    irdata.buffer_size       = 0;
    irdata.current_state     = false;
    irdata.notify_main       = false;
    irdata.current_processed = true;
}



ISR(TIMER0_COMPA_vect) {
    ir_isr();
    led_isr();
}

inline void led_isr() {
    
}

inline void ir_isr() {
    
}
