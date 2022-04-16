#define GPIO_BASE       0x200000
#define GPFSEL1         (GPIO_BASE + 0x4) // GPIO Function Select 1 (Broadcom manual page 90)
#define GPPUD           (GPIO_BASE + 0x94) // controls pulls up/down for all GPIO pins
#define GPPUDCLK0       (GPIO_BASE + 0x98) // controls pulls up/down for specific GPIO pins
