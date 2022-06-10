#include "gpio.h"
#include "proc.h"

int gpio_pin_set_io_syscall(uint8_t pin, uint8_t io_state) {
    enter_critical_section();
    int ret = gpio_pin_set_io(pin, io_state);
    exit_critical_section();
    return ret;
}

int gpio_set_syscall(uint8_t pin, uint8_t state) {
    enter_critical_section();
    int ret = gpio_set(pin, state);
    exit_critical_section();
    return ret;
}

int gpio_get_syscall(uint8_t pin) {
    enter_critical_section();
    int ret = gpio_get(pin);
    exit_critical_section();
    return ret;
}
