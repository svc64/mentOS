#include "time.h"
#include "print.h"

uint64_t current_timer_value = 0;

void init_timer() {
    // enable timer IRQ 1 interrupt
    mmio_write(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
}

// Make an IRQ 'time' ticks from now
void timer_irq_after(uint64_t time) {
    current_timer_value = mmio_read(TIMER_CLO);
    current_timer_value += time;
    mmio_write(TIMER_C1, current_timer_value);
}

void timer_irq_handler() {
    mmio_write(TIMER_CS, TIMER_CS_M1); // tell the timer that we got the IRQ
    print("got a timer IRQ!\n");
    timer_irq_after(200000);
}

void delay(int32_t count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
         : "=r"(count): [count]"0"(count) : "cc");
}
