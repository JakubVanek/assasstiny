#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <avr/io.h>

#define TIMER_PERIOD_US 50
#define IR_IDLE_THRESHOLD (10000/TIMER_PERIOD_US)

#define IR_DELTA        (200)
#define IR_HDRHI_MIN    ((9000-IR_DELTA)/TIMER_PERIOD_US)
#define IR_HDRHI_MAX    ((9000+IR_DELTA)/TIMER_PERIOD_US)
#define IR_HDRLO_MIN    ((4500-IR_DELTA)/TIMER_PERIOD_US)
#define IR_HDRLO_MAX    ((4500+IR_DELTA)/TIMER_PERIOD_US)
#define IR_REPLO_MIN    ((2250-IR_DELTA)/TIMER_PERIOD_US)
#define IR_REPLO_MAX    ((2250+IR_DELTA)/TIMER_PERIOD_US)
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

#endif
