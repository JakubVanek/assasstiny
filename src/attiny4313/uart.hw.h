#ifndef UART_HW_H
#define UART_HW_H

#include <avr/io.h>
#include "config.h"
#include <util/setbaud.h>

#define RECEIVED_INTR USART_RX_vect
#define SEND_INTR     USART_UDRE_vect

#if USE_2X
  #define UART_U2X (_BV(U2X));
#else
  #define UART_U2X (0x0);
#endif

#define UART_INIT() do { \
	UBRRH = UBRRH_VALUE; \
	UBRRL = UBRRL_VALUE; \
	UCSRA = UART_U2X   ; \
	UCSRB = _BV(TXEN)  | \
	        _BV(RXEN)  ; \
	UCSRC = _BV(UCSZ1) | \
	        _BV(UCSZ0) ; \
} while(0)

#define UART_TXON() do {  \
	UCSRB |=  _BV(UDRIE); \
} while(0)

#define UART_TXOFF() do { \
	UCSRB &= ~_BV(UDRIE); \
} while(0)

#define UART_RXON() do {  \
	UCSRB |=  _BV(RXCIE); \
} while(0)

#define UART_RXOFF() do { \
	UCSRB &= ~_BV(RXCIE); \
} while(0)

#define UART_WRITE(src) do { UDR = (src); } while(0)
#define UART_READ(dst)  do { (dst) = UDR; } while(0)

#endif
