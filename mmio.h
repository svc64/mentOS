#define MMIO_BASE 0x3F000000 // Specific for the RPi3/3b
uint32_t mmio_read(uint32_t reg);
void mmio_write(uint32_t reg, uint32_t data);
