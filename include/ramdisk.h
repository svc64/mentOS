#include <stdint.h>

#define RAMDISK_MAGIC   0x5244524452445244 // "RDRDRDRD"

struct __attribute__((__packed__)) ramdisk_header {
    uint64_t magic; // should be RAMDISK_MAGIC
    uint64_t big_endian_size; // size (in big endian)
};
