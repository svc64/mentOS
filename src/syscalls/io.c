#include <stddef.h>
#include "input_buffer.h"
#include "uart.h"
#include "proc.h"

void syscall_putc(char c) {
    uart_putc(c);
}

unsigned char syscall_getc() {
    return uart_getc();
}

size_t syscall_input_read(char *dest, size_t size) {
    while (front_proc != NULL) {
        proc_idle();
    }
    front_proc = current_proc;
    char c = '\0';
    size_t read = 0;
    while (input_buffer_last(front_proc->input_buffer) != '\0' && input_buffer_last(front_proc->input_buffer) != '\n') {
        proc_idle();
    }
    do {
        c = input_buffer_pop(front_proc->input_buffer);
        *dest++ = c;
        read++;
    } while (c != '\0' && c != '\n');
    front_proc = NULL;
    proc_idle_release();
    return read;
}

void syscall_input_flush() {}
