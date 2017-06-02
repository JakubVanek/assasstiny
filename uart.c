#include "uart.h"
#include "attiny4313/uart.hw.h"

void initUART() {
	UART_INIT();
}

ISR(RXDONE_ISR_NAME) {
	
}

ISR(TXEMPTY_ISR_NAME) {
	
}

ISR(TXDONE_ISR_NAME) {
	
}
