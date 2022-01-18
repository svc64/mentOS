#include <stdint.h>
#include "mmio.h"
const uint32_t MMIO_BASE = 0x3F000000; // Specific for the RPi3/3b

static uint32_t mmio_read(uint32_t reg) {
    return *(volatile uint32_t *)(MMIO_BASE + reg);
}

static void mmio_write(uint32_t reg, uint32_t data) {
    *(volatile uint32_t *)(MMIO_BASE + reg) = data;
}
