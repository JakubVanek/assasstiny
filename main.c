#include "ir.h"
#include "uart.h"
#include "leds.h"
#include "config.h"

void main() __attribute__((OS_main));

inline char    upperToHex (uint8_t byte);
inline char    lowerToHex (uint8_t byte);
inline char    nibbleToHex(uint8_t nibble);
inline uint8_t ir2uart    (uint8_t ir);


#define uart_fail read_x
#define request_char(c)        do { if (uart_read() != (c)) goto uart_fail; } while(0)
#define request_char_cached(c) do { if (ch          != (c)) goto uart_fail; } while(0)
#define consume_int() do { ch = uart_read(); }           \
                      while (('0' <= ch && ch <= '9') || \
			                 ('a' <= ch && ch <= 'f') || \
			                 ('A' <= ch && ch <= 'F'))

void main() {
	ir_buffer_t localIR;
	ir_init();
	uart_init();
	sei();
	for(;;) {
		while(!irdata.notify_main && !uartdata.notify_main) {}
		if (irdata.notify_main) { // full code received, decode & forward
			localIR = irdata.current_buffer;
			irdata.notify_main = false;
			irdata.current_processed = true;
			led_on(LED_RECEIVE, LED_MS);

			if (localIR.length != 32)
				goto end_ir;

			uint8_t addr       = (uint8_t)(localIR >> 24);
			uint8_t addr_compl = (uint8_t)(localIR >> 16);
			uint8_t cmd        = (uint8_t)(localIR >>  8);
			uint8_t cmd_compl  = (uint8_t)(localIR >>  0);

			if (~addr != addr_compl || ~cmd != cmd_compl)
				goto end_ir;

			if (addr != IR_ADDR)
				goto end_ir;

			uint8_t real_cmd = ir2uart(cmd);


			uart_write('m');
			uart_write('c');
			uart_write(' ');
			uart_write('0');
			uart_write(' ');
			uart_write(upperToHex(real_cmd));
			uart_write(lowerToHex(real_cmd));
			uart_write('\r');

		end_ir:;
		}
		if (uartdata.notify_main) { // 'x' received, blink LED
			uint8_t ch;
			request_char('c');
			request_char(' ');
			consume_int();
			request_char_cached(' ');
			request_char('O');
			request_char('K');
			consume_int();
			request_char_cached('x');

			led_on(LED_TRANSMIT_ACK, LED_MS);
			goto end;
		uart_fail:
			led_on(LED_TRANSMIT_NAK, LED_MS);
			while (uart_read() != 'x')
			{}
		end:;
		}
	}
}

char upperToHex(uint8_t byte) {
	return nibbleToHex(byte >> 4);
}
char lowerToHex(uint8_t byte) {
	return nibbleToHex(byte & 0xF);
}
char nibbleToHex(uint8_t nibble) {
	return (nibble < 10) ? (nibble + '0') : (nibble - 10 + 'a');
}
inline uint8_t ir2uart(uint8_t ir) {
	return ir; // for now
}
