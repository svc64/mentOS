#include "irq.h"
#include "exceptions.h"
#include "print.h"
#include "time.h"

void irq_handler() {
    print("irq_handler()\n");
    uint32_t irq = mmio_read(IRQ_PENDING_1);
    switch (irq) {
        case SYSTEM_TIMER_IRQ_1:
            timer_irq_handler();
            break;
        default:
            print("Unhandled IRQ: %d\n", irq);
            panic_unhandled_exc(PANIC_UNHANDLED_IRQ);
    }
    enable_irqs();
}
