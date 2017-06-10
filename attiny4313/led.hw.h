#ifndef IR_HW_H
#define IR_HW_H

#include "config.h"
#include <avr/io.h>

#define LED_HWDDR      CONCAT_EVAL(DDR,  LED_PORT)
#define LED_HWPORT     CONCAT_EVAL(PORT, LED_PORT)
#define LED_HWBIT_RCV  _BV(LED_PIN_RCV)
#define LED_HWBIT_ACK  _BV(LED_PIN_ACK)
#define LED_HWBIT_NAK  _BV(LED_PIN_NAK)

// mapping LED->PORT
static const uint8_t led_map[LED_COUNT] = {
	[LED_RECEIVE]      = LED_HWBIT_RCV,
	[LED_TRANSMIT_ACK] = LED_HWBIT_ACK,
	[LED_TRANSMIT_NAK] = LED_HWBIT_NAK,
};

#define INIT_LED() do {                           \
	for (led_t led = 0; led < LED_COUNT; led++) { \
		LED_HWPORT &= ~led_map[led];              \
		LED_HWDDR  |=  led_map[led];              \
	}                                             \
} while(0)

#define WRITE_LED(which,rly) do {  \
	uint8_t mask = led_map[which]; \
	if (rly) {                     \
		LED_HWPORT |=  mask;       \
	} else {                       \
		LED_HWPORT &= ~mask;       \
	}                              \
} while(0)

#endif
