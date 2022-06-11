#include <stdbool.h>
#include <stdlib.h>
#include <io.h>
#include <gpio.h>
#include <proc.h>

char *app_name = "gpio";

bool is_str_number(char *s) {
    while (*s != '\0') {
        if (*s < 0x30 || *s > 0x39) {
            return false;
        }
        s++;
    }
    return true;
}

void print_usage_exit() {
        print("usage:\n");
        print("%s [get/set_as_output/set_as_input] [pin number]\n", app_name);
        print("%s set [pin number] [state]\n", app_name);
        exit(1);
}

int main(int argc, const char *argv[]) {
    if (argc) {
        app_name = argv[0];
    }
    if (argc < 3) {
        print_usage_exit();
    }
    char *action = argv[1];
    char *pin_num_str = argv[2];
    if (!is_str_number(pin_num_str)) {
        print("invalid pin number: %s\n", pin_num_str);
        return 1;
    }
    int pin_num = atoi(pin_num_str);
    if (pin_num >= GPIO_PINS) {
        print("invalid pin number: %d\n", pin_num);
        return 1;
    }
    int ret;
    if (!strcmp(action, "get")) {
        if (argc != 3) {
            print_usage_exit();
        }
        ret = gpio_get(pin_num);
         if (ret >= 0) {
            print("Current state of input pin %d: %d\n", pin_num, ret);
        }
    } else if (!strcmp(action, "set")) {
        if (argc != 4) {
            print_usage_exit();
        }
        char *state_arg = argv[3];
        if (!is_str_number(state_arg)) {
            print("invalid pin state: %s\n", pin_num_str);
            return 1;
        }
        int pin_state = atoi(state_arg);
        if (pin_state != 0 && pin_state != 1) {
            print("invalid pin state: %d\n", pin_state);
            return 1;
        }
        ret = gpio_set(pin_num, pin_state);
        if (ret >= 0) {
            print("Set output pin %d to %d\n", pin_num, pin_state);
        }
    } else if (!strcmp(action, "set_as_output")) {
        if (argc != 3) {
            print_usage_exit();
        }
        ret = gpio_pin_set_io(pin_num, GPIO_OUTPUT);
        if (ret >= 0) {
            print("Pin %d set as output\n", pin_num);
        }
    } else if (!strcmp(action, "set_as_input")) {
        if (argc != 3) {
            print_usage_exit();
        }
        ret = gpio_pin_set_io(pin_num, GPIO_INPUT);
        if (ret >= 0) {
            print("Pin %d set as input\n", pin_num);
        }
    } else {
        print("unknown action: %s\n", action);
        return 1;
    }
    if (ret < 0) {
        print("Error: %s\n", err_str(ret));
        return 1;
    }
    return 0;
}
