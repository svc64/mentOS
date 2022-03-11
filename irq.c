#include "irq.h"
#include "exceptions.h"
#include "print.h"
#include "time.h"

void irq_handler(struct arm64_thread_state *state) {
    print("irq_handler()\n");
    for (int i = 0; i < 31; i++) {
        print("x%d: 0x%x\n", i, state->x[i]);
    }
    uint32_t irq = mmio_read(IRQ_PENDING_1);
    switch (irq) {
        case SYSTEM_TIMER_IRQ_1:
            timer_irq_handler();
            break;
        default:
            print("Unhandled IRQ: %d\n", irq);
            panic_unhandled_exc(state, PANIC_UNHANDLED_IRQ);
    }
    enable_irqs();
}
