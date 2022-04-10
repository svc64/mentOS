#include "uart.h"

void syscall_putc(char c) {
    uart_putc(c);
}

unsigned char syscall_getc() {
    return uart_getc();
}
