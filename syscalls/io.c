#include "uart.h"

void syscall_putc(char c) {
    uart_putc(c);
}
