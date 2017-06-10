#ifndef LEDS_H
#define LEDS_H
#include <avr/interrupt.h>
#include "config.h"

typedef enum {
	LED_RECEIVE,
	LED_TRANSMIT_ACK,
	LED_TRANSMIT_NAK,
	LED_COUNT
} led_t;

typedef struct {
	volatile int16_t ticks[LED_COUNT];
} led_timer_t;

extern led_timer_t led_timers;

inline static void led_on(led_t which, uint32_t usec) {
	uint8_t sreg = SREG;
	cli();
	led_timers.ticks[which] = usec / T0_PERIOD_US;
	SREG = sreg;
}

inline static void led_off(led_t which) {
	led_on(which, 0);
}

void led_init();
void led_tick(uint8_t n);

#endif
