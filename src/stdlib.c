#include "stdlib.h"
#include "print.h"

#define ALIGNED(__x, __size) (!((uintptr_t)(__x) % (__size)))

void *memcpy(void *s, void *d, size_t n) {
    void *dest = d;
    while (n) {
        if (ALIGNED(s, sizeof(__uint128_t)) && ALIGNED(d, sizeof(__uint128_t)) && n >= sizeof(__uint128_t)) {
            *((__uint128_t *)s) = *((__uint128_t *)d);
            s += sizeof(__uint128_t);
            d += sizeof(__uint128_t);
            n -= sizeof(__uint128_t);
        } else if (ALIGNED(s, sizeof(uint64_t)) && ALIGNED(d, sizeof(uint64_t)) && n >= sizeof(uint64_t)) {
            *((uint64_t *)s) = *((uint64_t *)d);
            s += sizeof(uint64_t);
            d += sizeof(uint64_t);
            n -= sizeof(uint64_t);
        } else if (ALIGNED(s, sizeof(uint32_t)) && ALIGNED(d, sizeof(uint32_t)) && n >= sizeof(uint32_t)) {
            *((uint32_t *)s) = *((uint32_t *)d);
            s += sizeof(uint32_t);
            d += sizeof(uint32_t);
            n -= sizeof(uint32_t);
        } else if (ALIGNED(s, sizeof(uint16_t) && ALIGNED(d, sizeof(uint16_t))) && n >= sizeof(uint16_t)) {
            *((uint16_t *)s) = *((uint16_t *)d);
            s += sizeof(uint16_t);
            d += sizeof(uint16_t);
            n -= sizeof(uint16_t);
        } else {
            *((uint8_t *)s) = *((uint8_t *)d);
            s += sizeof(uint8_t);
            d += sizeof(uint8_t);
            n -= sizeof(uint8_t);
        }
    }
    return dest;
}

void bzero(void *s, size_t n) {
    while (n) {
        if (ALIGNED(s, sizeof(__uint128_t)) && n >= sizeof(__uint128_t)) {
            *((__uint128_t *)s) = 0;
            s += sizeof(__uint128_t);
            n -= sizeof(__uint128_t);
        } else if (ALIGNED(s, sizeof(uint64_t)) && n >= sizeof(uint64_t)) {
            *((uint64_t *)s) = 0;
            s += sizeof(uint64_t);
            n -= sizeof(uint64_t);
        } else if (ALIGNED(s, sizeof(uint32_t)) && n >= sizeof(uint32_t)) {
            *((uint32_t *)s) = 0;
            s += sizeof(uint32_t);
            n -= sizeof(uint32_t);
        } else if (ALIGNED(s, sizeof(uint16_t)) && n >= sizeof(uint16_t)) {
            *((uint16_t *)s) = 0;
            s += sizeof(uint16_t);
            n -= sizeof(uint16_t);
        } else {
            *((uint8_t *)s) = 0;
            s += sizeof(uint8_t);
            n -= sizeof(uint8_t);
        }
    }
}

int memcmp(void *s, void *d, size_t n) {
    while (n) {
        if (ALIGNED(s, sizeof(__uint128_t)) && ALIGNED(d, sizeof(__uint128_t)) && n >= sizeof(__uint128_t)) {
            if (*((__uint128_t *)s) != *((__uint128_t *)d)) {
                return (int)(*((__uint128_t *)s) - *((__uint128_t *)d));
            }
            s += sizeof(__uint128_t);
            d += sizeof(__uint128_t);
            n -= sizeof(__uint128_t);
        } else if (ALIGNED(s, sizeof(uint64_t)) && ALIGNED(d, sizeof(uint64_t)) && n >= sizeof(uint64_t)) {
            if (*((uint64_t *)s) != *((uint64_t *)d)) {
                return (int)(*((uint64_t *)s) - *((uint64_t *)d));
            }
            s += sizeof(uint64_t);
            d += sizeof(uint64_t);
            n -= sizeof(uint64_t);
        } else if (ALIGNED(s, sizeof(uint32_t)) && ALIGNED(d, sizeof(uint32_t)) && n >= sizeof(uint32_t)) {
            if (*((uint32_t *)s) != *((uint32_t *)d)) {
                return (int)(*((uint32_t *)s) - *((uint32_t *)d));
            }
            s += sizeof(uint32_t);
            d += sizeof(uint32_t);
            n -= sizeof(uint32_t);
        } else if (ALIGNED(s, sizeof(uint16_t) && ALIGNED(d, sizeof(uint16_t))) && n >= sizeof(uint16_t)) {
            if (*((uint16_t *)s) != *((uint16_t *)d)) {
                return (int)(*((uint16_t *)s) - *((uint16_t *)d));
            }
            s += sizeof(uint16_t);
            d += sizeof(uint16_t);
            n -= sizeof(uint16_t);
        } else {
            if (*((uint8_t *)s) != *((uint8_t *)d)) {
                return (int)(*((uint8_t *)s) - *((uint8_t *)d));
            }
            s += sizeof(uint8_t);
            d += sizeof(uint8_t);
            n -= sizeof(uint8_t);
        }
    }
    return 0;
}

void *memset(void *s, int value, size_t n) {
    void *ret = s;
    unsigned char v = value;
    /* the size is specified in v_arr just to trigger a warning in case the size is different for
     some reason. we just fill v_arr with the value we fill memory with to fill memory in blocks
     for performance reasons. */
    unsigned char v_arr[sizeof(__uint128_t)] = {v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v};
    while (n) {
        if (ALIGNED(s, sizeof(__uint128_t)) && n >= sizeof(__uint128_t)) {
            *((__uint128_t *)s) = *((__uint128_t *)v_arr);
            s += sizeof(__uint128_t);
            n -= sizeof(__uint128_t);
        } else if (ALIGNED(s, sizeof(uint64_t)) && n >= sizeof(uint64_t)) {
            *((uint64_t *)s) = *((uint64_t *)v_arr);
            s += sizeof(uint64_t);
            n -= sizeof(uint64_t);
        } else if (ALIGNED(s, sizeof(uint32_t)) && n >= sizeof(uint32_t)) {
            *((uint32_t *)s) = *((uint32_t *)v_arr);
            s += sizeof(uint32_t);
            n -= sizeof(uint32_t);
        } else if (ALIGNED(s, sizeof(uint16_t)) && n >= sizeof(uint16_t)) {
            *((uint16_t *)s) = *((uint16_t *)v_arr);
            s += sizeof(uint16_t);
            n -= sizeof(uint16_t);
        } else {
            *((uint8_t *)s) = *((uint8_t *)v_arr);
            s += sizeof(uint8_t);
            n -= sizeof(uint8_t);
        }
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
