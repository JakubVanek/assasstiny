#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <avr/io.h>

#define CONCAT_PASTE(a,b) a ## b
#define CONCAT_EVAL(a,b) CONCAT_PASTE(a,b)

#define IR_PORT      D
#define IR_PIN       5
#define LED_PORT     D
#define LED_PIN_RCV  4
#define LED_PIN_ACK  3
#define LED_PIN_NAK  2


#define IR_ADDR 0x00

#define LED_US 100000
#define REPEAT_TIMEOUT_US 1000000
#define REPEAT_TIMEIN_US   200000

#define T0_PERIOD_US 50
#define T0_CLK 0x2


#define IR_DELTA_MS    200
#define IR_BOUNDED(x,l) (IR_MIN(l) <= (x) && (x) <= IR_MAX(l))
#define IR_MIN(t)  ((t - IR_DELTA_MS) / T0_PERIOD_US)
#define IR_MAX(t)  ((t + IR_DELTA_MS) / T0_PERIOD_US)

#define IR_HDRHI       9000
#define IR_HDRLO_CMD   4500
#define IR_HDRLO_REP   2250
#define IR_DATHI       550
#define IR_DATLO_0     550
#define IR_DATLO_1     1650
#define IR_HDRIDLE     (5000 / T0_PERIOD_US)
#define IR_DATIDLE     (2000 / T0_PERIOD_US)


#define CIRCBUF_SIZE 16

#endif
