#include <stdint.h>

struct __attribute__((__packed__)) ramdisk_header {
    char magic[2]; // "RD"
    uint64_t big_endian_size; // size (in big endian)
};
