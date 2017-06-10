#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "busywait.h"
#include "uart.h"
#include "attiny4313/uart.hw.h"
volatile uart_data_t uartdata;

ISR(RECEIVED_INTR) {
	uint8_t byte;
	UART_READ(byte);

	if (cbuf_availw(uartdata.in)) {
		cbuf_writea(uartdata.in, byte);
	}
	if (byte == 'x') // notify byte
		uartdata.status = UART_REPLY_COMPLETE;
}

ISR(SEND_INTR) {
	if (cbuf_availr(uartdata.out)) {
		cbuf_reada(uartdata.out, UDR);
	} else {
		UART_TXOFF();
	}
}

void uart_init() {
	UART_INIT();
	UART_RXON();
	UART_TXON();
}

void uart_write(uint8_t byte) {
	uint8_t sreg = SREG;
	busywait(!cbuf_availw(uartdata.out));
	cbuf_writea(uartdata.out, byte);
	UART_TXON();
	SREG = sreg;
}

void uart_write_str(char *data) {
	uint8_t sreg = SREG;
	while(*data != '\0') {
		busywait(!cbuf_availw(uartdata.out));
		cbuf_writea(uartdata.out, *data++);
		sei();
		UART_TXON();
	}
	SREG = sreg;
}

uint8_t uart_read() {
	uint8_t byte, sreg = SREG;
	busywait(!cbuf_availr(uartdata.in));
	cbuf_reada(uartdata.in, byte);
	SREG = sreg;
	return byte;
}

bool uart_available() {
	uint8_t rly, sreg = SREG;
	cli();
	rly = cbuf_availr(uartdata.in);
	SREG = sreg;
	return rly;
}
