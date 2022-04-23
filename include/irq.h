#include "mmio.h"
#define IRQ_BASIC_PENDING   0x0000B200
#define IRQ_PENDING_1       0x0000B204
#define IRQ_PENDING_2       0x0000B208
#define FIQ_CONTROL         0x0000B20C
#define ENABLE_IRQS_1       0x0000B210
#define ENABLE_IRQS_2       0x0000B214
#define ENABLE_BASIC_IRQS   0x0000B218
#define DISABLE_IRQS_1      0x0000B21C
#define DISABLE_IRQS_2      0x0000B220
#define DISABLE_BASIC_IRQS  0x0000B224

#define SYSTEM_TIMER_IRQ_0  (1 << 0)
#define SYSTEM_TIMER_IRQ_1  (1 << 1)
#define SYSTEM_TIMER_IRQ_2  (1 << 2)
#define SYSTEM_TIMER_IRQ_3  (1 << 3)

void disable_irqs();
void enable_irqs();
