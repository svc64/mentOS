#include "misc.h"

uint64_t read64(uint64_t *ptr) {
    uint64_t volatile r;
    __asm__ __volatile__("ldr %0, [%1]" : "=r" (r) : "r" (ptr) : "memory");
    return r;
}

uint32_t read32(uint32_t *ptr) {
    uint32_t volatile r;
    __asm__ __volatile__("ldr %0, [%1]" : "=r" (r) : "r" (ptr) : "memory");
    return r;
}
