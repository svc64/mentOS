#include "exceptions.h"
#include "proc.h"
#include "mmio.h"
#include "irq.h"
#include "print.h"
#include "time.h"
#include "uart.h"

void handle_irq_1(uint32_t irq, struct arm64_thread_state *thread_state) {
    switch (irq) {
        case SYSTEM_TIMER_IRQ_1:
            timer_irq_handled();
            proc_exit(thread_state);
            break;
        default:
            print("hmmmm... got unknown IRQ1 %d. ignoring...\n", irq);
    }
}

void handle_irq_2(uint32_t irq, struct arm64_thread_state *thread_state, int exc) {
    switch (irq) {
        case UART0_IRQ:
            uart_recieved(uart_getc());
            uart_interrupts_clear(UART0_INT_RXR);
            break;
        default:
            print("hmmmm... got unknown IRQ2 %d. ignoring...\n", irq);
    }
}

void handle_irq(struct arm64_thread_state *thread_state, int exc) {
    uint32_t irq = mmio_read(IRQ_PENDING_1);
    if (irq) {
        handle_irq_1(irq, thread_state);
    }
    irq = mmio_read(IRQ_PENDING_2);
    if (irq) {
        handle_irq_2(irq, thread_state, exc);
    }
}
