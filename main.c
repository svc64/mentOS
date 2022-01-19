#include "uart.h"

void main() {
	int a = 1337;
	int b = a + 420;
	init_uart();
	uart_puts("hi lmao\n");
}
