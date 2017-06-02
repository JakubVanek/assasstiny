#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <avr/io.h>

// ISR settings
#define TIMER_PERIOD_US 50
#define TIMER_PRESCALER_NUM      8
#define TIMER_PRESCALER_MAGIC    _BV(CS01)
#define TIMER_TICKS              (TIMER_PERIOD_US * F_CPU / (TIMER_PRESCALER_NUM * 1000000))

#define IR_IDLE_THRESHOLD (10000/TIMER_PERIOD_US)


#define IR_DELTA        (200)
#define IR_HDRHI_MIN    ((9000-IR_DELTA)/TIMER_PERIOD_US)
#define IR_HDRHI_MAX    ((9000+IR_DELTA)/TIMER_PERIOD_US)
#define IR_HDRLO_MIN    ((4500-IR_DELTA)/TIMER_PERIOD_US)
#define IR_HDRLO_MAX    ((4500+IR_DELTA)/TIMER_PERIOD_US)
#define IR_DATAHI_MIN   (( 550-IR_DELTA)/TIMER_PERIOD_US)
#define IR_DATAHI_MAX   (( 550+IR_DELTA)/TIMER_PERIOD_US)
#define IR_DATALO0_MIN  (( 550-IR_DELTA)/TIMER_PERIOD_US)
#define IR_DATALO0_MAX  (( 550+IR_DELTA)/TIMER_PERIOD_US)
#define IR_DATALO1_MIN  ((1650-IR_DELTA)/TIMER_PERIOD_US)
#define IR_DATALO1_MAX  ((1650+IR_DELTA)/TIMER_PERIOD_US)

#define IR_MAXLEN (32)

// Size of IR buffer
typedef uint32_t ir_payload_t;

// IR time datatype
typedef int16_t ir_time_t;

// IR buffer size datatype
typedef uint8_t ir_size_t;

#define IR_DDR      DDRD
#define IR_PORT     PORTD
#define IR_PIN      PIND
#define LED_DDR     DDRD
#define LED_PORT    PORTD
#define LED_RCV_OUT _BV(PORTD4)
#define LED_ACK_OUT _BV(PORTD3)
#define LED_NAK_OUT _BV(PORTD2)
#define IR_IN       _BV(PORTD5)

#endif
