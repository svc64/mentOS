#include "uart.h"
#include "log.h"

void main() {
    uint64_t current_el;
    init_uart();
    uart_puts("hi lmao\n");
    __asm__ __volatile__("mrs %0, CurrentEL\n\t" : "=r" (current_el) :  : "memory");
    current_el = (current_el >> 2) & 0x3;
    logf("Running in EL%d\n", current_el);
}
