#include "ir.h"
#include "config.h"
#include "attiny4313/ir.hw.h"
#include "attiny4313/timer.hw.h"

// shared IR service data
ir_data_t    irdata;

// initialize IR service
void ir_init() {
    // initialize IR pins
    INIT_IR();

	// initialize periodic interrupt
	T0_INIT();
}

// main ISR
ISR(T0_TRIG_INT) {
	bool on = READ_IR();
	if (irdata.notify == IR_RECEIVED) return;

	switch(irdata.state) {
		case ST_IDLE:
			if (on) {
				irdata.state = ST_ON_HDR;
			}
			break;
		case ST_ON_HDR:
			if (on) {
				if (irdata.ticks != 0xFF) {
					irdata.ticks++;
				}
			} else {
				if (IR_BOUNDED(irdata.ticks, IR_HDRHI)) {
					irdata.state = ST_OFF_HDR;
				} else {
					irdata.state = ST_IDLE;
				}
				irdata.ticks = 0;
			}
			break;
		case ST_ON_DATA:
			if (on) {
				if (irdata.ticks != 0xFF) {
					irdata.ticks++;
				}
			} else {
				if (IR_BOUNDED(irdata.ticks, IR_DATHI)) {
					irdata.state = ST_OFF_DATA;
				} else {
					irdata.state = ST_IDLE;
				}
				irdata.ticks = 0;
			}
			break;
		case ST_OFF_HDR:
			if (!on) {
				if (irdata.ticks <= IR_HDRIDLE) {
					irdata.ticks++;
				} else {
					irdata.state = ST_IDLE;
					irdata.ticks = 0;
				}
			} else {
				if (IR_BOUNDED(irdata.ticks, IR_HDRLO_CMD)) {
					irdata.state = ST_ON_DATA;
					irdata.which = NEC_COMMAND;
				} else if (IR_BOUNDED(irdata.ticks, IR_HDRLO_REP)) {
					irdata.state = ST_ON_DATA;
					irdata.which = NEC_REPEAT;
				} else {
					irdata.state = ST_IDLE;
				}
				irdata.ticks = 0;
			}
			break;
		case ST_OFF_DATA:
			if (!on) {
				if (irdata.ticks <= IR_DATIDLE) {
					irdata.ticks++;
				} else {
					irdata.state = ST_IDLE;
					irdata.ticks = 0;
					irdata.notify = IR_RECEIVED;
				}
			} else {
				if (IR_BOUNDED(irdata.ticks, IR_DATLO_0)) {
					irdata.state = ST_ON_DATA;
					irdata.code = (irdata.code << 1) | 0x0;
				} else if (IR_BOUNDED(irdata.ticks, IR_DATLO_1)) {
					irdata.state = ST_ON_DATA;
					irdata.code = (irdata.code << 1) | 0x1;
				} else {
					irdata.state = ST_IDLE;
				}
				irdata.ticks = 0;
			}
			break;
	}

	irdata.ledticks++;
}
