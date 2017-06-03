#ifndef UART_H
#define UART_H

#include "config.h"

typedef uint8_t uart_buf_t[UART_BUF];
typedef uint8_t uart_index_t;

typedef struct {
	uart_buf_t   data;
	uart_index_t start;
	uart_index_t end;
} uart_fifo_t;

typedef struct {
	uart_fifo_t in;
	uart_fifo_t out;
	bool notify_main;
} uart_data_t;

extern volatile uart_data_t uartdata;

extern void    uart_init      ();
extern void    uart_write     (uint8_t byte);
extern uint8_t uart_write_many(uint8_t *data, uint8_t len);
extern uint8_t uart_read      ();
extern uint8_t uart_read_many (uint8_t *data, uint8_t len);
extern bool    uart_available ();

#endif
