#include <stdint.h>
#include "mmio.h"

uint32_t mmio_read(uint32_t reg) {
    uint64_t addr = MMIO_BASE + reg;
    return *(volatile uint32_t *)(addr);
}

void mmio_write(uint32_t reg, uint32_t data) {
    uint64_t addr = MMIO_BASE + reg;
    *(volatile uint32_t *)(addr) = data;
}
