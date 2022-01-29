#include "log.h"
#include "uart.h"

void logf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    while (fmt[0] != 0) {
        if (fmt[0] == '%') {
            fmt++;
            if (fmt[0] == 'd') {
                uint64_t i = va_arg(args, uint64_t);
                uint64_t digitCount = 0;
                for (uint64_t c = i; c % 10; c /= 10) {
                    digitCount++;
                }
                char digits[digitCount];
                uint64_t arrayPos = digitCount-1;
                for (uint64_t c = i; (c % 10 && arrayPos >= 0); c /= 10) {
                    char digit = c % 10 + 48;
                    digits[arrayPos] = digit;
                    arrayPos--;
                }
                for (uint64_t i = 0; i < digitCount; i++) {
                    uart_putc(digits[i]);
                }
            }
            if (fmt[0] == 'x') {
                uint64_t i = va_arg(args, uint64_t);
                uint64_t digitCount = 0;
                for (uint64_t c = i; c; c /= 16) {
                    digitCount++;
                }
                char digits[digitCount];
                uint64_t arrayPos = digitCount-1;
                for (uint64_t c = i; (c && arrayPos >= 0); c /= 16) {
                    char digit = c % 16;
                    if (digit >= 0xA) {
                        digit += 0x37;
                    } else {
                        digit += 0x30;
                    }
                    digits[arrayPos] = digit;
                    arrayPos--;
                }
                for (uint64_t i = 0; i < digitCount; i++) {
                    uart_putc(digits[i]);
                }
            }
        } else {
            uart_putc(fmt[0]);
        }
        fmt++;
    }

    va_end(args);
}
