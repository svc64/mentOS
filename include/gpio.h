#include <stdbool.h>
#include <stdint.h>
#define GPIO_BASE           0x200000
#define GPFSEL0             (GPIO_BASE + 0x00)
#define GPFSEL1             (GPIO_BASE + 0x04)
#define GPFSEL2             (GPIO_BASE + 0x08)
#define GPFSEL3             (GPIO_BASE + 0x0C)
#define GPFSEL4             (GPIO_BASE + 0x10)
#define GPFSEL5             (GPIO_BASE + 0x14)
#define GPSET0              (GPIO_BASE + 0x1C)
#define GPSET1              (GPIO_BASE + 0x20)
#define GPCLR0              (GPIO_BASE + 0x28)
#define GPCLR1              (GPIO_BASE + 0x2C)
#define GPLEV0              (GPIO_BASE + 0x34)
#define GPLEV1              (GPIO_BASE + 0x38)
#define GPEDS0              (GPIO_BASE + 0x40)
#define GPEDS1              (GPIO_BASE + 0x44)
#define GPHEN0              (GPIO_BASE + 0x64)
#define GPHEN1              (GPIO_BASE + 0x68)
#define GPPUD               (GPIO_BASE + 0x94)
#define GPPUDCLK0           (GPIO_BASE + 0x98)
#define GPPUDCLK1           (GPIO_BASE + 0x9C)

#define GPIO_RESERVED       -2
#define GPIO_UNINITIALIZED  -1
#define GPIO_INPUT           0
#define GPIO_OUTPUT          1

#define GPIO_DOWN            0
#define GPIO_UP              1

void init_gpio();
void gpio_reserve(uint8_t pin);
void gpio_unreserve(uint8_t pin);
int gpio_pin_set_io(uint8_t pin, uint8_t io_state);
int gpio_set(uint8_t pin, uint8_t state);
int gpio_get(uint8_t pin);
