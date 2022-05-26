#include "time.h"

uint64_t current_timer_value = 0;

void enable_timer_irq() {
    // enable timer IRQ 1 interrupt
    mmio_write(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
}

void disable_timer_irq() {
    // disable timer IRQ 1 interrupt
    mmio_write(DISABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
}

// Make an IRQ 'time' ticks from now
void timer_irq_after(uint64_t time) {
    current_timer_value = mmio_read(TIMER_CLO);
    current_timer_value += time;
    mmio_write(TIMER_C1, current_timer_value);
}

// tell the timer that we handled this IRQ.
void timer_irq_handled() {
    mmio_write(TIMER_CS, TIMER_CS_M1);
}

void delay(int32_t count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
         : "=r"(count): [count]"0"(count) : "cc");
}
