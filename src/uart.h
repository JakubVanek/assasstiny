#ifndef UART_H
#define UART_H

#include "config.h"
#include "circbuf.h"

typedef enum uart_notif_t {
	UART_NONE,
	UART_REPLY_COMPLETE
} uart_notif_t;

typedef struct uart_data_t {
	circbuf_t in;
	circbuf_t out;
	uart_notif_t status;
} uart_data_t;

extern volatile uart_data_t uartdata;

extern void    uart_init      ();
extern void    uart_write     (uint8_t byte);
extern void    uart_write_str (char *data);
extern uint8_t uart_read      ();
extern bool    uart_available ();

#endif
