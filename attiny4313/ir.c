#include "ir.h"
#include "leds.h"
#include "config.h"
#include <avr/interrupt.h>

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

// mapping LED->PORT
const uint8_t led_map[LED_COUNT] = {
	[LED_RECEIVE]      = LED_RCV_OUT,
	[LED_TRANSMIT_ACK] = LED_ACK_OUT,
	[LED_TRANSMIT_NAK] = LED_NAK_OUT,
};

// initialize periodic interrupt
void initISR() {
	PRR    &= ~_BV(PRTIM0);
	OCR0A   = TIMER_TICKS;
	TCCR0A  = (_BV(COM0A1 & 0x0)) | (_BV(COM0A2 & 0x0)) | // no OC0A out
	          (_BV(COM0B1 & 0x0)) | (_BV(COM0B2 & 0x0)) | // no OC0B out
	          (_BV(WGM01  & 0xF)) | (_BV(WGM00  & 0x0));  // CTC mode
	TCCR0B  = (_BV(FOC0A  & 0x0)) | (_BV(FOC0B  & 0x0)) | // no forced compare
	          (_BV(WGM02  & 0x0)) |                       // CTC mode
	          TIMER_PRESCALER_MAGIC;                      // prescaler
	TCNT0   = 0;                                          // reset count
	TIFR   &= ~(_BV(OCF0B  & 0xF)) | (_BV(TOF0  & 0xF)) | (_BV(OCF0A  & 0xF)); // clean interrupt flags
	TIMSK  |=  (_BV(OCIE0B & 0x0)) | (_BV(TOIE0 & 0x0)) | (_BV(OCIE0A & 0xF)); // enable CTC interrupt
}

// initialize IR service
void initIR() {
	// initialize shared data; other members are zeroed out by default
    back_buffer              = bufferA;
    irdata.current_buffer    = bufferB;
    irdata.current_processed = true;
    // initialize pins
    IR_DDR  &= _BV(PORTD5);
    IR_PORT &= _BV(PORTD5);
}

// initialize LED service
void initLED() {
	// initialize pins
	for (led_t led = 0; led < LED_COUNT; led++) {
		LED_PORT &= ~led_map[led];
		LED_DDR  |=  led_map[led];
	}
}

// main ISR
ISR(TIMER0_COMPA_vect) {
	bool on = IR_PIN & IR_IN;
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
		if (led_timers[led] > 0) {
			LED_PORT |=  led_map[led];
		} else {
			LED_PORT &= ~led_map[led];
		}
		// decrease timeout
		led_timers[led].usecs -= TIMER_PERIOD_US;
	}
}
