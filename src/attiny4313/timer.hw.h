#ifndef TIMER_HW_H
#define TIMER_HW_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include "config.h"

#define T0_CLK_OFF     0x0
#define T0_CLK_DIV1    0x1
#define T0_CLK_DIV8    0x2
#define T0_CLK_DIV64   0x3
#define T0_CLK_DIV256  0x4
#define T0_CLK_DIV1024 0x5
#define T0_CLK_EXTFALL 0x6
#define T0_CLK_EXTRISE 0x7

#ifndef T0_CLK
  #warning Timer/Counter0 clock not defined, turning off.
  #define T0_CLK T0_CLK_OFF
#endif

#if   (T0_CLK == T0_CLK_OFF)
  #define T0_PRESCALE 0
#elif (T0_CLK == T0_CLK_DIV1)
  #define T0_PRESCALE 1
#elif (T0_CLK == T0_CLK_DIV8)
  #define T0_PRESCALE 8
#elif (T0_CLK == T0_CLK_DIV64)
  #define T0_PRESCALE 64
#elif (T0_CLK == T0_CLK_DIV256)
  #define T0_PRESCALE 256
#elif (T0_CLK == T0_CLK_DIV1024)
  #define T0_PRESCALE 1024
#elif (T0_CLK == T0_CLK_EXTFALL || T0_CLK == T0_CLK_EXTRISE)
  #ifndef T0_PRESCALE
    #error Timer/Counter0 external frequency not defined.
  #endif
#else
  #error Timer/Counter0 has invalid clock source.
#endif

#define T0_TICKS (T0_PERIOD_US * F_CPU / (T0_PRESCALE * 1000000))


#define T0_TRIG_INT TIMER0_COMPA_vect

#define T0_INIT() do {   \
	uint8_t sreg = SREG; \
	cli();               \
	TCNT0   = 0;         \
	OCR0A   = T0_TICKS;  \
	TIMSK  |= 0x1;       \
	TCCR0A  = 0x2;       \
	TCCR0B  = T0_CLK;    \
	SREG = sreg;         \
} while(0)

#endif
