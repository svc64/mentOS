#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
#include "errors.h"
#include "mmio.h"

#define GPIO_PINS 54
int8_t gpio_pins_state[GPIO_PINS];

void init_gpio() {
    // disable GPIO pull-up/down
    mmio_write(GPPUD, 0);
    delay(150);
    for (int i = 0; i < GPIO_PINS; i++) {
        gpio_pins_state[i] = GPIO_UNINITIALIZED;
    }
}

// reserve a pin for use by some other driver
void gpio_reserve(uint8_t pin) {
    gpio_pins_state[pin] = GPIO_RESERVED;
}

void gpio_unreserve(uint8_t pin) {
    if (gpio_pins_state[pin] == GPIO_RESERVED) {
        gpio_pins_state[pin] = GPIO_UNINITIALIZED;
    }
}

// set pin as input or output
// returns E_PARAM if the pin is reserved
int gpio_pin_set_io(uint8_t pin, uint8_t io_state) {
    if (pin >= GPIO_PINS) {
        return E_OOB;
    }
    if (gpio_pins_state[pin] == GPIO_RESERVED) {
        return E_PARAM;
    }
    uint32_t gpio_reg = GPFSEL0 + ((pin / 10) * sizeof(uint32_t));
    uint8_t reg_pin = pin % 10;
    uint32_t r = mmio_read(gpio_reg);
    if (io_state == GPIO_OUTPUT) {
        r |= ((uint32_t)1 << (reg_pin * 3));
    } else if (io_state == GPIO_INPUT) {
        r &= ~((uint32_t)1 << (reg_pin * 3));
    }
    gpio_pins_state[pin] = io_state;
    mmio_write(gpio_reg, r);
    delay(150);
}

// change the state of an output pin
// returns E_PARAM if the pin isn't an output pin
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
        if (state) {
            reg = GPSET1;
        } else {
            reg = GPCLR1;
        }
        bit = pin - 32;
    } else {
        if (state) {
            reg = GPSET0;
        } else {
            reg = GPCLR0;
        }
        bit = pin;
    }
    uint32_t r = ((uint32_t)1 << bit);
    mmio_write(reg, r);
    delay(150);
    return 0;
}

// get the value of a pin
// returns E_PARAM if the pin isn't an input pin
int gpio_get(uint8_t pin) {
    if (gpio_pins_state[pin] == GPIO_RESERVED || pin >= GPIO_PINS) {
        return E_OOB;
    }
    if (gpio_pins_state[pin] != GPIO_INPUT) {
        return E_PARAM;
    }
    uint32_t reg;
    uint8_t bit;
    if (pin >= 32) {
        reg = GPLEV1;
        bit = pin - 32;
    } else {
        reg = GPLEV0;
        bit = pin;
    }
    uint32_t ret = mmio_read(reg);
    ret = ret << (32 - bit - 1);
    ret = ret >> (32 - 1);
    return ret;
}
