#include "uart.h"
#include "attiny4313/uart.hw.h"
#include "util/delay.h"


volatile uart_data_t uartdata;

#define buf_wrap(n) do { if ((n) == UART_BUF) { (n) = 0; } } while(0)
#define buf_write(buf,src)         do { buf.data[buf.end  ] = (src);   buf_wrap(buf.end  ); } while(0)
#define buf_write_advance(buf,src) do { buf.data[buf.end++] = (src);   buf_wrap(buf.end  ); } while(0)
#define buf_read(buf,dst)          do { (dst) = buf.data[buf.start  ]; buf_wrap(buf.start); } while(0)
#define buf_read_advance(buf,dst)  do { (dst) = buf.data[buf.start++]; buf_wrap(buf.start); } while(0)
#define avail_write(buf) ((buf.end + 1) % UART_BUF != buf.start)
#define avail_read(buf)  (buf.start != buf.end)



ISR(RXDONE_ISR_NAME) {
	uint8_t flags = UCSRA;
	uint8_t byte  = UDR;
	if (avail_write(uartdata.in)) {
		buf_write_advance(uartdata.in, byte);
		if (byte == 'x') // notify byte
			uartdata.notify_main = true;
	} // else discard
}

ISR(TXEMPTY_ISR_NAME) {
	if (avail_read(uartdata.out)) {
		buf_read_advance(uartdata.out, UDR);
	} else {
		UART_DISABLE_TX();
	}
}

void uart_init() {
	UART_INIT();
	UART_ENABLE_RX();
	UART_ENABLE_TX();
}

void uart_write(uint8_t byte) {
	INT_ENABLE
	
	while (!avail_write(uartdata.out)) { /* fidget spinner */ }
	buf_write_advance(uartdata.out, byte);
	UART_ENABLE_TX();
	
	INT_RESTORE
}

uint8_t uart_write_many(uint8_t *data, uint8_t len) {
	uint8_t i = 0;
	INT_DISABLE
	
	while (avail_write(uartdata.out) && len > 0) {
		buf_write_advance(uartdata.out, data[i]);
		
		i++;
		len--;
	}
	UART_ENABLE_TX();
	
	INT_RESTORE
	return i;
}

uint8_t uart_read() {
	uint8_t byte;
	INT_ENABLE
	
	while (!avail_read(uartdata.in)) { /* fidget spinner */ }
	buf_read_advance(uartdata.in, byte);
	
	INT_RESTORE
	return byte;
}

uint8_t uart_read_many(uint8_t *data, uint8_t len) {
	uint8_t i = 0;
	INT_DISABLE
	while (avail_read(uartdata.in) && len > 0) {
		buf_read_advance(uartdata.in, data[i]);
		
		i++;
		len--;
	}
	INT_RESTORE
	return i;
}

bool uart_available() {
	uint8_t rly;
	INT_DISABLE
	
	rly = avail_read(uartdata.in);
	
	INT_RESTORE
	return rly;
}
