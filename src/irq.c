#include "exceptions.h"
#include "proc.h"
#include "mmio.h"
#include "irq.h"
#include "print.h"
#include "time.h"

void handle_irq(struct arm64_thread_state *thread_state, int exc) {
    uint32_t irq = mmio_read(IRQ_PENDING_1);
    switch (irq) {
        case SYSTEM_TIMER_IRQ_1:
            if (exc == EXC_IRQ_EL0_64 || exc == EXC_IRQ_EL1h) {
                timer_irq_handled();
                proc_exit(thread_state);
            } else {
                print("got a timer IRQ for the wrong exception... bug? ignoring. exception: %s\n", exc_str(exc));
                timer_irq_handled();
            }
            break;
        default:
            print("hmmmm... got unknown IRQ %d while handling %s. ignoring...\n", irq, exc_str(exc));
    }
}
