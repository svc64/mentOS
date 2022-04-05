#include <stdint.h>
#include "irq.h"

#define TIMER_CS        0x00003000
#define TIMER_CLO       0x00003004
#define TIMER_CHI       0x00003008
#define TIMER_C0        0x0000300C
#define TIMER_C1        0x00003010
#define TIMER_C2        0x00003014
#define TIMER_C3        0x00003018

#define TIMER_CS_M0	    (1 << 0)
#define TIMER_CS_M1	    (1 << 1)
#define TIMER_CS_M2	    (1 << 2)
#define TIMER_CS_M3	    (1 << 3)

void init_timer();
void timer_irq_after(uint64_t time);
void timer_irq_handled();
void delay(int32_t count);
