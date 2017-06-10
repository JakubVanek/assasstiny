#ifndef BUSYWAIT_H
#define BUSYWAIT_H
#include <avr/sleep.h>

//
// FIDGET SPINNER huehuehue
//

#define busywait(cond)       \
	do {                     \
	test:                    \
		cli();               \
		if (cond) {          \
			sei();           \
			sleep_enable();  \
			sleep_cpu();     \
			sleep_disable(); \
			goto test;       \
		}                    \
	} while(0)

#endif
