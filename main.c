#include "ir.h"
#include "uart.h"
#include "leds.h"
#include "config.h"
#include <avr/interrupt.h>

// main loop
int main() __attribute__((OS_main));

// convert upper nibble of byte to hex char
inline char    upperToHex (uint8_t byte);
// convert lower nibble of byte to hex char
inline char    lowerToHex (uint8_t byte);
// convert lower nibble of byte to hex char (unsanitized)
inline char    nibbleToHex(uint8_t nibble);
// convert IR received code to RS232 transmitted code
inline uint8_t ir2uart    (uint8_t ir);


// macro to signal failure
#define uart_fail              read_x
// parser macro to request character
#define request_char(c)        do { if (uart_read() != (c)) goto uart_fail; } while(0)
// parser macro to request character
#define request_char_cached(c) do { if (ch          != (c)) goto uart_fail; } while(0)
// parser macro to request hex string
#define consume_int()          do { ch = uart_read(); }           \
                               while (('0' <= ch && ch <= '9') || \
                                      ('a' <= ch && ch <= 'f') || \
                                      ('A' <= ch && ch <= 'F'))

// main loop
int main() {
	/* VARIABLES */

	// local storage of received IR data
	ir_buffer_t localIR;
	// saved last command for NEC repeats
	uint8_t real_cmd = 0x0;

	/* SETUP */

	// initialize IR
	ir_init();
	// initialize UART
	uart_init();
	// enable interrupts
	sei();

	/* EVENT LOOP */

	// loop forever
	for(;;) {
		// wait for event notification
		while(!irdata.notify_main && !uartdata.notify_main) { /* fidget spinner */ }

		// IR needs attention
		if (irdata.notify_main) { // full code received, decode & forward

			// save data to local variable (performance, buffering)
			cli();                              // disable interupts
			localIR = irdata.buffer;            // copy multibyte packet
			irdata.notify_main = false;         // notification received
			irdata.current_processed = true;    // data copied
			led_on(LED_RECEIVE, LED_MS);        // blink LED
			sei();                              // enable interrupts

			// new code received
			if (localIR.command == NEC_CODE) {
				// we accept only 4byte NEC codes
				if (localIR.length != 32)
					goto end_ir;

				// shift out address & command bytes
				uint8_t addr       = (uint8_t)(localIR.data >> 24);
				uint8_t addr_compl = (uint8_t)(localIR.data >> 16);
				uint8_t cmd        = (uint8_t)(localIR.data >>  8);
				uint8_t cmd_compl  = (uint8_t)(localIR.data >>  0);

				// check code validity
				if (~addr != addr_compl || ~cmd != cmd_compl)
					goto end_ir;

				// check TV address
				if (addr != IR_ADDR)
					goto end_ir;

				// convert IR code to RS232
				real_cmd = ir2uart(cmd);
			} // else repeat code received; send the last code again

			// send RS232 command to TV
			// format: "mc [hex id] [hex cmd]\r"
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

		// TV needs attention
		if (uartdata.notify_main) { // 'x' received, blink LED
			// notification received
			uartdata.notify_main = false;

			// temporary buffer
			uint8_t ch;

			// parse answer
			request_char('c');
			request_char(' ');
			consume_int();
			request_char_cached(' ');
			request_char('O');
			request_char('K');
			consume_int();
			request_char_cached('x');

			// parse OK - blink acknowledge LED
			led_on(LED_TRANSMIT_ACK, LED_MS);
			goto end;
		uart_fail:
			// parse KO - blink not acknowledge LED
			led_on(LED_TRANSMIT_NAK, LED_MS);
			// consume until "x" that triggered the notification is out
			while (uart_read() != 'x')
			{}
		end:;
		}
	}

	// to make gcc happy
	return 0;
}

char upperToHex(uint8_t byte) {
	return nibbleToHex(byte >> 4); // shift upper nibble to lower half
}
char lowerToHex(uint8_t byte) {
	return nibbleToHex(byte & 0xF); // mask upper nibble
}
char nibbleToHex(uint8_t nibble) {
	return (nibble < 10) ? (nibble + '0') : (nibble - 10 + 'a'); // add right chars
}
uint8_t ir2uart(uint8_t ir) {
	return ir; // simple passthrough
}
