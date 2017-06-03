#ifndef UART_HW_H
#define UART_HW_H

#include <avr/io.h>

#define RXDONE_ISR_NAME   USART0_RX_vect
#define TXEMPTY_ISR_NAME  USART0_UDRE_vect

#if     F_CPU == 1000000
#define UART_U2X (_BV(U2X) & 0xFF)
#define UART_BR 12
#elseif F_CPU == 8000000
#define UART_U2X (_BV(U2X) & 0x00)
#define UART_BR 51
#else
#error unknown cpu freq
#endif


#define UART_INIT() do { \
	UBBR  = UART_BR    ; \
	UCSRA = UART_U2X   ; \
	UCSRB = _BV(RXIE)  | \
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
