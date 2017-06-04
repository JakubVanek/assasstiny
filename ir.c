#include "ir.h"
#include "leds.h"
#include "config.h"
#include "attiny4313/ir.hw.h"

// shared IR service data
volatile irdata_t    irdata;

// shared LED service data
volatile led_timer_t led_timers; 


// initialize IR service
void ir_init() {
	// initialize shared data; other members are zeroed out by default
    irdata.current_processed = true;
    
    // initialize IR pins
    INIT_IR();
    
	// initialize LED pins
	INIT_LED();
	
	// initialize periodic interrupt
	TIMER_INIT();
}

#define ir_data  (irdata.buffer.data)
#define ir_len   (irdata.buffer.length)
#define ir_cmd   (irdata.buffer.command)
#define ir_state (irdata.current_state)
#define ir_time  (irdata.current_ticks)
#define ir_avail() (irdata.current_processed)
#define ir_notify() do { irdata.current_processed = false; \
	                     irdata.notify_main       = true; } while(0)

// main ISR
ISR(ISR_NAME) {
	bool on = READ_IR();
	ir_time_t ticks = irdata.current_ticks;
	
	if (!ir_avail())
		goto idle;
	
	if (on) {
		switch (ir_state) {
			case IDLE:
				ir_state = HEADER_ON;
				goto restart_ticks;
			case HEADER_ON:
			case DATA_ON:
				ir_time++;
				goto end;
			case HEADER_OFF:
				if (IR_HDRLO_MIN <= ticks && ticks <= IR_HDRLO_MAX) {
					ir_cmd   = NEC_CODE;
					ir_state = DATA_ON;
					goto restart_ticks;
				} else if (IR_REPLO_MIN <= ticks && ticks <= IR_REPLO_MAX) {
					ir_cmd   = NEC_REPEAT;
					ir_state = DATA_ON;
					goto restart_ticks;
				} else {
					// reset
					goto idle;
				}
			case DATA_OFF:
				if (ir_len == IR_MAXLEN) {
					ir_len = 0;
				}
				ir_data <<= 1;
				if (IR_DATALO0_MIN <= ticks && ticks <= IR_DATALO0_MAX) {
					ir_data |= 0x0;
				} else if (IR_DATALO1_MIN <= ticks && ticks <= IR_DATALO1_MAX) {
					ir_data |= 0x1;
				} else {
					// reset
					goto idle;
				}
				ir_state = DATA_ON;
				goto restart_ticks;
		}
	} else { // off
		switch (ir_state) {
			case IDLE:
				goto end;
			case HEADER_ON:
				if (IR_HDRHI_MIN <= ticks && ticks <= IR_HDRHI_MAX) {
					ir_state = HEADER_OFF;
					goto restart_ticks;
				} else {
					goto idle;
				}
			case DATA_OFF:
				if (ticks >= IR_DATALO1_MAX) {
					ir_notify();
					goto idle;
				}
				// no break intended
			case HEADER_OFF:
				ir_time++;
				goto end;
			case DATA_ON:
				if (IR_HDRHI_MIN <= ticks && ticks <= IR_HDRHI_MAX) {
					ir_state = HEADER_OFF;
					goto restart_ticks;
				} else {
					goto idle;
				}
		}
	}
	goto end;
idle:
	ir_state = IDLE;
	ir_len = 0;
restart_ticks:
	ir_time = 0;
end:;

	for (led_t led = 0; led < LED_COUNT; led++) {
		// set diode according to timeout
		WRITE_LED(led, led_timers.usecs[led] > 0);
		// decrease timeout
		led_timers.usecs[led] -= TIMER_PERIOD_US;
	}
}
