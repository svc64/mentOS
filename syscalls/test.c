#include <stdint.h>
#include "print.h"
uint32_t syscall_test_return32() {
    print("return32\n");
    return 0x41414141;
}

uint64_t syscall_test_return64() {
    return 0x4141414141414141;
}
