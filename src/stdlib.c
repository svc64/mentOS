#include "stdlib.h"
#include "print.h"
void *memcpy(void *s, void *d, size_t n) {
    void *dest = d;
    size_t blocks_64 = n / sizeof(uint64_t);
    size_t blocks_32 = n % sizeof(uint64_t) / sizeof(uint32_t);
    size_t blocks_16 = n % sizeof(uint64_t) % sizeof(uint32_t) / sizeof(uint16_t);
    size_t blocks_8 = n % sizeof(uint64_t) % sizeof(uint32_t) % sizeof(uint16_t) / sizeof(uint8_t);
    for (size_t i = 0; i < blocks_64; i++) {
        *((uint64_t *)s) = *((uint64_t *)d);
        s += sizeof(uint64_t);
        d += sizeof(uint64_t);
    }
    for (size_t i = 0; i < blocks_32; i++) {
        *((uint32_t *)s) = *((uint32_t *)d);
        s += sizeof(uint32_t);
        d += sizeof(uint32_t);
    }
    for (size_t i = 0; i < blocks_16; i++) {
        *((uint16_t *)s) = *((uint16_t *)d);
        s += sizeof(uint16_t);
        d += sizeof(uint16_t);
    }
    for (size_t i = 0; i < blocks_8; i++) {
        *((uint8_t *)s) = *((uint8_t *)d);
        s += sizeof(uint8_t);
        d += sizeof(uint8_t);
    }
    return dest;
}

void bzero(void *s, size_t n) {
    size_t blocks_64 = n / sizeof(uint64_t);
    size_t blocks_32 = n % sizeof(uint64_t) / sizeof(uint32_t);
    size_t blocks_16 = n % sizeof(uint64_t) % sizeof(uint32_t) / sizeof(uint16_t);
    size_t blocks_8 = n % sizeof(uint64_t) % sizeof(uint32_t) % sizeof(uint16_t) / sizeof(uint8_t);
    for (size_t i = 0; i < blocks_64; i++) {
        *((uint64_t *)s) = 0;
        s += sizeof(uint64_t);
    }
    for (size_t i = 0; i < blocks_32; i++) {
        *((uint32_t *)s) = 0;
        s += sizeof(uint32_t);
    }
    for (size_t i = 0; i < blocks_16; i++) {
        *((uint16_t *)s) = 0;
        s += sizeof(uint16_t);
    }
    for (size_t i = 0; i < blocks_8; i++) {
        *((uint8_t *)s) = 0;
        s += sizeof(uint8_t);
    }
}

int memcmp(void *s, void *d, size_t n) {
    size_t blocks_64 = n / sizeof(uint64_t);
    size_t blocks_32 = n % sizeof(uint64_t) / sizeof(uint32_t);
    size_t blocks_16 = n % sizeof(uint64_t) % sizeof(uint32_t) / sizeof(uint16_t);
    size_t blocks_8 = n % sizeof(uint64_t) % sizeof(uint32_t) % sizeof(uint16_t) / sizeof(uint8_t);
    for (size_t i = 0; i < blocks_64; i++) {
        if (*((uint64_t *)s) != *((uint64_t *)d)) {
            return (int)(*((uint64_t *)s) - *((uint64_t *)d));
        }
    }
    for (size_t i = 0; i < blocks_32; i++) {
        if (*((uint32_t *)s) != *((uint32_t *)d)) {
            return (int)(*((uint32_t *)s) - *((uint32_t *)d));
        }
    }
    for (size_t i = 0; i < blocks_16; i++) {
        if (*((uint16_t *)s) != *((uint16_t *)d)) {
            return (int)(*((uint16_t *)s) - *((uint16_t *)d));
        }
    }
    for (size_t i = 0; i < blocks_8; i++) {
        if (*((uint8_t *)s) != *((uint8_t *)d)) {
            return (int)(*((uint8_t *)s) - *((uint8_t *)d));
        }
    }
    return 0;
}

void *memset(void *s, int value, size_t n) {
    void *ret = s;
    size_t blocks_64 = n / sizeof(uint64_t);
    size_t blocks_32 = n % sizeof(uint64_t) / sizeof(uint32_t);
    size_t blocks_16 = n % sizeof(uint64_t) % sizeof(uint32_t) / sizeof(uint16_t);
    size_t blocks_8 = n % sizeof(uint64_t) % sizeof(uint32_t) % sizeof(uint16_t) / sizeof(uint8_t);
    unsigned char v = value;
    /* the size is specified in v_arr just to trigger a warning in case the size is different for
     some reason. we just fill v_arr with the value we fill memory with to fill memory in blocks
     for performance reasons. */
    unsigned char v_arr[sizeof(uint64_t)] = {v, v, v, v, v, v, v, v};
    for (size_t i = 0; i < blocks_64; i++) {
        *((uint64_t *)s) = *((uint64_t *)v_arr);
        s += sizeof(uint64_t);
    }
    for (size_t i = 0; i < blocks_32; i++) {
        *((uint32_t *)s) = *((uint32_t *)v_arr);;
        s += sizeof(uint32_t);
    }
    for (size_t i = 0; i < blocks_16; i++) {
        *((uint16_t *)s) = *((uint16_t *)v_arr);
        s += sizeof(uint16_t);
    }
    for (size_t i = 0; i < blocks_8; i++) {
        *((uint8_t *)s) = *((uint8_t *)v_arr);
        s += sizeof(uint8_t);
    }
    return ret;
}

char *strchr(const char *s, int c) {
    while (*s != 0) {
        if (*s == c) {
            return (char *)s;
        }
        s++;
    }
    return NULL;
}

size_t strlen(const char *str) {
    size_t c = 0;
    for (size_t i = 0; str[i] != 0; i++) {
        c++;
    }
    return c;
}
