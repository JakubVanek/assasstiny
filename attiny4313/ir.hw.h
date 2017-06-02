#ifndef IR_HW_H
#define IR_HW_H

#include "config.h"
#include <avr/interrupt.h>
#include <avr/io.h>

#define ISR_NAME TIMER0_COMPA_vect


// ISR settings
#define TIMER_PRESCALER_NUM      8
#define TIMER_PRESCALER_MAGIC    _BV(CS01)
#define TIMER_TICKS              (TIMER_PERIOD_US * F_CPU / (TIMER_PRESCALER_NUM * 1000000))


#define TIMER_INIT() do { \
	uint8_t sreg = SREG;  \
	cli(); \
	PRR    &= ~_BV(PRTIM0); \
	OCR0A   = TIMER_TICKS;  \
	TCCR0A  = (_BV(COM0A1 & 0x0)) | (_BV(COM0A2 & 0x0)) | /* no OC0A out */ \
	          (_BV(COM0B1 & 0x0)) | (_BV(COM0B2 & 0x0)) | /* no OC0B out */ \
	          (_BV(WGM01  & 0xF)) | (_BV(WGM00  & 0x0));  /* CTC mode    */ \
	TCCR0B  = (_BV(FOC0A  & 0x0)) | (_BV(FOC0B  & 0x0)) | /* no forced compare */ \
	          (_BV(WGM02  & 0x0)) |                       /* CTC mode    */ \
	          TIMER_PRESCALER_MAGIC;                      /* prescaler   */ \
	TCNT0   = 0;                                          /* reset count */ \
	TIFR   &= ~(_BV(OCF0B  & 0xF)) | (_BV(TOF0  & 0xF)) | (_BV(OCF0A  & 0xF)); /* clean interrupt flags */ \
	TIMSK  |=  (_BV(OCIE0B & 0x0)) | (_BV(TOIE0 & 0x0)) | (_BV(OCIE0A & 0xF)); /* enable CTC interrupt  */ \
	SREG = sreg; \
} while(0)

// mapping LED->PORT
static const uint8_t led_map[LED_COUNT] = {
	[LED_RECEIVE]      = LED_RCV_OUT,
	[LED_TRANSMIT_ACK] = LED_ACK_OUT,
	[LED_TRANSMIT_NAK] = LED_NAK_OUT,
};

#define IR_DDR      DDRD
#define IR_PORT     PORTD
#define IR_PIN      PIND
#define LED_DDR     DDRD
#define LED_PORT    PORTD
#define LED_RCV_OUT _BV(PORTD4)
#define LED_ACK_OUT _BV(PORTD3)
#define LED_NAK_OUT _BV(PORTD2)
#define IR_IN       _BV(PORTD5)


#define INIT_IR() do {\
	IR_DDR  &= IR_IN; \
    IR_PORT &= IR_IN; \
} while(0)

#define INIT_LED() do {\
	for (led_t led = 0; led < LED_COUNT; led++) { \
		LED_PORT &= ~led_map[led]; \
		LED_DDR  |=  led_map[led]; \
	} \
} while(0)

#define READ_IR() (IR_PIN & IR_IN)
#define WRITE_LED(which,rly) do { \
	if (rly) { \
		LED_PORT |=  led_map[which]; \
	} else { \
		LED_PORT &= ~led_map[which]; \
	} \
} while(0)

#endif
