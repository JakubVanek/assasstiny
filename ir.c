#include "ir.h"
#include "leds.h"
#include "config.h"
#include "attiny4313/ir.hw.h"

// individual services
inline void led_isr();
inline void ir_isr();

// double buffered IR receiver
volatile ir_buffer_t bufferA;
volatile ir_buffer_t bufferB;

// current IR backbuffer (offline)
volatile ir_buffer_t * volatile back_buffer;

// shared IR service data
volatile irdata_t    irdata;

// shared LED service data
volatile led_timer_t led_timers; 


// initialize IR service
void initIR() {
	// initialize shared data; other members are zeroed out by default
    back_buffer              = bufferA;
    irdata.current_buffer    = bufferB;
    irdata.current_processed = true;
    
    // initialize IR pins
    INIT_IR();
    
	// initialize LED pins
	INIT_LED();
	
	// initialize periodic interrupt
	TIMER_INIT();
}

// main ISR
ISR(ISR_NAME) {
	bool on = READ_IR();
	ir_sample_t ticks = irdata.current_ticks;
	
	if (on) {
		switch (irdata.current_state) {
			case IDLE:
				irdata.current_state = HEADER_ON;
				goto restart_ticks;
			case HEADER_ON:
			case DATA_ON:
				irdata.current_ticks++;
				goto end;
			case HEADER_OFF:
				if (IR_HDRLO_MIN <= ticks && ticks <= IR_HDRLO_MAX) {
					back_buffer.command = NEC_CODE;
					irdata.current_state = DATA_ON;
					goto restart_ticks;
				} else if (IR_REPLO_MIN <= ticks && ticks <= IR_REPLO_MAX) {
					back_buffer.command = NEC_REPEAT;
					irdata.current_state = DATA_ON;
					goto restart_ticks;
				} else {
					// reset
					goto idle;
				}
			case DATA_OFF:
				if (back_buffer.length == IR_MAXLEN) {
					back_buffer.length = 0;
				}
				back_buffer.data <<= 1;
				if (IR_DATALO0_MIN <= ticks && ticks <= IR_DATALO0_MAX) {
					back_buffer.data |= 0x0;
				} else if (IR_DATALO1_MIN <= ticks && ticks <= IR_DATALO1_MAX) {
					back_buffer.data |= 0x1;
				} else {
					// reset
					goto idle;
				}
				irdata.current_state = DATA_ON;
				goto restart_ticks;
		}
	} else { // off
		switch (irdata.current_state) {
			case IDLE:
				goto end;
			case HEADER_ON:
				if (IR_HDRHI_MIN <= ticks && ticks <= IR_HDRHI_MAX) {
					irdata.current_state = HEADER_OFF;
					goto restart_ticks;
				} else {
					goto idle;
				}
			case DATA_OFF:
				if (ticks >= IR_HDRLO_MAX) {
					if (irdata.current_processed) {
						ir_buffer_t volatile *tmp = back_buffer;
						back_buffer = irdata.current_buffer;
						irdata.current_buffer = tmp;
						irdata.notify_main = true;
					}
					goto idle;
				}
				// no break intended
			case HEADER_OFF:
				irdata.current_ticks++;
				goto end;
			case DATA_ON:
				if (IR_HDRHI_MIN <= ticks && ticks <= IR_HDRHI_MAX) {
					irdata.current_state = HEADER_OFF;
					goto restart_ticks;
				} else {
					goto idle;
				}
		}
	}
	goto end;
idle:
	irdata.current_state = IDLE;
	back_buffer.length = 0;
restart_ticks:
	irdata.current_ticks = 0;
end:;

	for (led_t led = 0; led < LED_COUNT; led++) {
		// set diode according to timeout
		WRITE_LED(led, led_timers[led] > 0);
		// decrease timeout
		led_timers[led].usecs -= TIMER_PERIOD_US;
	}
}
