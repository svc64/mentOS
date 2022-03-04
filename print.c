#include "print.h"
#include "uart.h"

void print_num(uint64_t num, int base) {
    uint64_t digitCount = 0;
    // count how many digits we have to print
    for (uint64_t c = num; c; c /= base) {
        digitCount++;
    }
    if (!digitCount) {
        // it's a zero
        uart_putc('0');
        return;
    }
    // put them all in an array
    char digits[digitCount];
    uint64_t arrayPos = digitCount-1;
    for (uint64_t c = num; (c && arrayPos >= 0); c /= base) {
        char digit = c % base;
        // convert to ASCII
        switch (base) {
            case 10: // decimal
                digit += 48;
                break;
            case 16: // hex
                if (digit >= 0xA) {
                    digit += 0x37;
                } else {
                    digit += 0x30;
                }
                break;
            default: // uh we should never get here
                break;
        }
        digits[arrayPos] = digit;
        arrayPos--;
    }
    // print
    for (uint64_t i = 0; i < digitCount; i++) {
        uart_putc(digits[i]);
    }
}

void print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    while (fmt[0] != 0) {
        if (fmt[0] == '%') {
            fmt++;
            if (fmt[0] == 'd') {
                uint64_t num = va_arg(args, uint64_t);
                print_num(num, 10);
            }
            if (fmt[0] == 'x') {
                uint64_t num = va_arg(args, uint64_t);
                print_num(num, 16);
            }
            if (fmt[0] == 's') {
                const char *s = va_arg(args, const char *);
                uart_puts(s);
            }
        } else {
            uart_putc(fmt[0]);
        }
        fmt++;
    }
    va_end(args);
}
