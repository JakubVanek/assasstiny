#include <avr/io.h>
#include <avr/interrupt.h>
#include "leds.h"
#include "attiny4313/led.hw.h"

led_timer_t led_timers;

void led_init() {
	INIT_LED();
}

void led_tick(uint8_t n) {
	for (uint8_t led = 0; led < LED_COUNT; led++) {
		if (led_timers.ticks[led] > 0) {
			WRITE_LED(led, true);
			led_timers.ticks[led] -= n;
		} else {
			WRITE_LED(led, false);
		}
	}
}
