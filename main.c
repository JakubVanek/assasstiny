#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include "ir.h"
#include "uart.h"
#include "leds.h"
#include "config.h"
#include "hex.h"
#include "busywait.h"

// main loop
int main() __attribute__((OS_main));


// convert IR received code to RS232 transmitted code
inline bool ir2uart(uint8_t ir, uint8_t *real);


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
	ir_payload_t ir_code;
	ir_type_t    ir_type;
	// saved last command for NEC repeats
	uint8_t real_cmd = 0x0;
	uint16_t repeat_timeout = 0;
	uint16_t repeat_timein = 0;
	/* SETUP */

	// enable interrupts
	set_sleep_mode(SLEEP_MODE_IDLE);
	// initialize IR
	ir_init();
	// initialize LEDs
	led_init();
	// initialize UART
	uart_init();


	/* EVENT LOOP */

	// loop forever
	for(;;) {
		// wait for event notification
		busywait(irdata.notify   == IR_NONE   &&
		         uartdata.status == UART_NONE &&
		         irdata.ledticks == 0);
		// IR needs attention
		if (irdata.notify != IR_NONE) { // full code received, decode & forward
			ir_code = irdata.code;
			ir_type = irdata.which;
			irdata.notify = IR_NONE;            // notification received
			sei();                              // enable interrupts

			// new code received
			if (ir_type == NEC_COMMAND) {

				// shift out address & command bytes
				uint8_t addr       = (ir_code >> 24) & 0xFF;
				uint8_t addr_compl = (ir_code >> 16) & 0xFF;
				uint8_t cmd        = (ir_code >>  8) & 0xFF;
				uint8_t cmd_compl  = (ir_code >>  0) & 0xFF;

				// check code validity
				if ((addr ^ addr_compl) != 0xFF)
					goto end_ir;

				if ((cmd  ^ cmd_compl)  != 0xFF)
					goto end_ir;

				// check TV address
				if (addr != IR_ADDR)
					goto end_ir;

				// convert IR code to RS232
				if(!ir2uart(cmd, &real_cmd))
					goto end_ir;
			} else {
				if (repeat_timeout == 0)
					goto end_ir;
				if (repeat_timein != 0)
					goto end_ir;
			}
			repeat_timeout = REPEAT_TIMEOUT_US / T0_PERIOD_US;
			repeat_timein  = REPEAT_TIMEIN_US  / T0_PERIOD_US;
			led_on(LED_RECEIVE, LED_US);        // blink LED
			// send RS232 command to TV
			// format: "mc [hex id] [hex cmd]\r"
			uart_write_str("mc 0 ");
			uart_write(upperToHex(real_cmd));
			uart_write(lowerToHex(real_cmd));
			uart_write('\r');
		end_ir:;
		}

		cli();
		// TV needs attention
		if (uartdata.status != UART_NONE) { // 'x' received, blink LED
			// notification received
			uartdata.status = UART_NONE;
			sei();

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
			led_on(LED_TRANSMIT_ACK, LED_US);
			goto end;
		uart_fail:
			// parse KO - blink not acknowledge LED
			led_on(LED_TRANSMIT_NAK, LED_US);
			// consume until "x" that triggered the notification is out
			while (uart_read() != 'x')
			{}
		end:;
		}

		cli();
		if (irdata.ledticks != 0) {
			uint8_t ticks = irdata.ledticks;
			irdata.ledticks = 0;
			sei();
			led_tick(ticks);
			if (repeat_timeout >= ticks) {
				repeat_timeout -= ticks;
			} else {
				repeat_timeout = 0;
			}
			if (repeat_timein >= ticks) {
				repeat_timein -= ticks;
			} else {
				repeat_timein = 0;
			}
		}
	}

	// to make gcc happy
	return 0;
}


const uint8_t codes[] PROGMEM = {
	0xE2, 0x00, // CH+  -> CH+
	0xA2, 0x01, // CH-  -> CH-
	0x62, 0x08, // CH   -> POWER
	0xA8, 0x02, // VOL+ -> VOL+
	0xE0, 0x03, // VOL- -> VOL-
	0x90, 0x09, // EQ   -> MUTE
	0x38, 0x44, // 5    -> OK
	0x10, 0x07, // 4    -> LEFT
	0x5A, 0x06, // 6    -> RIGHT
	0x18, 0x40, // 2    -> UP
	0x4A, 0x41, // 6    -> DOWN
	0xC2, 0x95, // PLAY -> ECO
	0x68, 0x28, // 0    -> RETURN
};
uint8_t codes_size = sizeof(codes) / sizeof(codes[0]);

bool ir2uart(uint8_t ir, uint8_t *real) {
	for (const uint8_t *ptr = codes; ptr != codes + codes_size; ptr += 2) {
		if (pgm_read_byte_near(ptr) == ir) {
			*real = pgm_read_byte_near(ptr + 1);
			return true;
		}
	}
	return false;
}
