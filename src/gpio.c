#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
#include "errors.h"
#include "mmio.h"

#define GPIO_PINS 54
int8_t gpio_pins_state[GPIO_PINS];

void init_gpio() {
    for (int i = 0; i < GPIO_PINS; i++) {
        if (i == 14 || i == 15) {
            gpio_pins_state[i] = GPIO_RESERVED;
        } else {
            gpio_pins_state[i] = GPIO_UNINITIALIZED;
        }
    }
}

int gpio_pin_set_io(uint8_t pin, uint8_t io_state) {
    if (gpio_pins_state[pin] == GPIO_RESERVED || pin >= GPIO_PINS) {
        return E_OOB;
    }
    uint32_t gpio_reg = GPFSEL0 + ((pin / 10) * sizeof(uint32_t));
    uint8_t reg_pin = pin % 10;
    uint32_t r = mmio_read(gpio_reg);
    if (io_state) {
        r |= ((uint32_t)1 << (reg_pin * 3));
    } else {
        r &= ~((uint32_t)1 << (reg_pin * 3));
    }
    mmio_write(gpio_reg, r);
    gpio_pins_state[pin] = (int8_t)(io_state);
}

int gpio_set(uint8_t pin, uint8_t state) {
    if (gpio_pins_state[pin] == GPIO_RESERVED || pin >= GPIO_PINS) {
        return E_OOB;
    }
    if (gpio_pins_state[pin] != GPIO_OUTPUT) {
        return E_PARAM;
    }
    uint32_t reg;
    uint8_t bit;
    if (pin >= 32) {
        reg = GPSET1;
        bit = pin - 32;
    } else {
        reg = GPSET0;
        bit = pin;
    }
    uint32_t r = mmio_read(reg);
    if (state) {
        r |= ((uint32_t)1 << bit);
    } else {
        r &= ~((uint32_t)1 << bit);
    }
    mmio_write(reg, r);
    return 0;
}
