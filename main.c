#include "print.h"
#include "exceptions.h"
#include "time.h"
#include "uart.h"

void main() {
    init_uart();
    print("mentOS\n");
    uint64_t current_el;
    __asm__ __volatile__("mrs %0, CurrentEL\n\t" : "=r" (current_el) :  : "memory");
    current_el = (current_el >> 2) & 0x3;
    print("Running in EL%d\n", current_el);

    // Set exception vectors
    __asm__ __volatile__("msr vbar_el1, %0\n\t" : : "r" (&exception_vectors) : "memory");
    // do something stupid, just to make an exception
    // totally random constant, btw
    //uint64_t *wut = (uint64_t *)0x5be3749d0211f909;
    //*(uint64_t *)wut = 0xc0885663c55641d8;
    enable_irqs();
    timer_irq_after(200000);
    init_timer();
    while (1) {}
}
