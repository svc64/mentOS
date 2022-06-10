#include <stdint.h>

#define GPIO_PINS           54

#define GPIO_TX_PIN         14
#define GPIO_RX_PIN         15

#define GPIO_INPUT           0
#define GPIO_OUTPUT          1

int gpio_pin_set_io(uint8_t pin, uint8_t io_state);
int gpio_set(uint8_t pin, uint8_t state);
int gpio_get(uint8_t pin);
