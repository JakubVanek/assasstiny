#ifndef UART_HW_H
#define UART_HW_H

#include <avr/io.h>

#define BAUD 9600
#include <util/setbaud.h>

#define RXDONE_ISR_NAME   USART0_RX_vect
#define TXEMPTY_ISR_NAME  USART0_UDRE_vect



#if USE_2X
#define UART_U2X (_BV(U2X));
#else
#define UART_U2X (0x0);
#endif

#define UART_INIT() do { \
	UBRRH = UBRRH_VALUE; \
	UBRRL = UBRRL_VALUE; \
	UCSRA = UART_U2X   ; \
	UCSRB = _BV(RXCIE)  | \
	        _BV(UDRIE) ; \
	UCSRC = _BV(UCSZ1) | \
	        _BV(UCSZ0) ; \
} while(0)

#define UART_ENABLE_TX() do { \
	UCSRB |=  _BV(TXEN);      \
} while(0)

#define UART_DISABLE_TX() do { \
	UCSRB &= ~_BV(TXEN);       \
} while(0)

#define UART_ENABLE_RX() do { \
	UCSRB |=  _BV(RXEN);      \
} while(0)

#define UART_DISABLE_RX() do { \
	UCSRB &= ~_BV(RXEN);       \
} while(0)

#define INT_DISABLE uint8_t sreg = SREG; cli();
#define INT_ENABLE  uint8_t sreg = SREG; sei();
#define INT_RESTORE SREG = sreg;

#endif
