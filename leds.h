#ifndef LEDS_H
#define LEDS_H

typedef enum {
	LED_RECEIVE,
	LED_TRANSMIT_ACK,
	LED_TRANSMIT_NAK,
	LED_COUNT
} led_t;

typedef struct {
	int16_t usecs[LED_COUNT];
} led_timer_t;

extern volatile led_timer_t led_timers;

inline void led_on(led_t which, int16_t usec) {
	led_timers.usecs[which] = usec;
}

inline void led_off(led_t which) {
	led_on(which, 0);
}

#endif
