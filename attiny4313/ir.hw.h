#ifndef IR_HW_H
#define IR_HW_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include "config.h"


#define IR_HWDDR      CONCAT_EVAL(DDR,  IR_PORT)
#define IR_HWPORT     CONCAT_EVAL(PORT, IR_PORT)
#define IR_HWPIN      CONCAT_EVAL(PIN,  IR_PORT)
#define IR_HWBIT      _BV(IR_PIN)

#define INIT_IR() do {\
	IR_HWDDR  &= IR_HWBIT; \
    IR_HWPORT &= IR_HWBIT; \
} while(0)

#define READ_IR() ((IR_HWPIN & IR_HWBIT) == 0)

#endif
