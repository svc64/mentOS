#include <stddef.h>
#include "uart.h"
#include "time.h"
#include "mmio.h"

void init_uart() {
	// disable UART0 (while we're setting it up)
	mmio_write(UART0_CR, 0);

	// configure GPIO functions
	uint32_t sel1 = mmio_read(GPFSEL1);
	// bits 12:14 = 0b100 (alternative function: UART TX)
	// bits 15:17 = 0b100 (alternative function: UART RX)
	sel1 &= ~((0b111 << 12) | (0b111 << 15)); // clear GPIO 14/15
	sel1 |= (0b100 << 12) | (0b100 << 15);    // set alternative functions
	mmio_write(GPFSEL1, sel1);

	// disable all GPIO pins
	mmio_write(GPPUD, 0);
	delay(150);
 
	// we're using pins 14 and 15 for UART0 so we have to disable them for GPIO
	mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
	delay(150);
 
	// write 0 to GPPUDCLK0 to apply our changes
	mmio_write(GPPUDCLK0, 0);
 
	// clear whatever pending interrupts we have here
	// ICR = Interrupt Clear Register.
	mmio_write(UART0_ICR, 0x7FF);

	// set integer & fractional clock dividers
	// divider = UART_CLOCK/(16 * baudrate) (broadcom docs page 183)
	// Fraction part register = (Fractional part * 64) + 0.5
	// we want a baudrate of 115200 here
 
	// set UART_CLOCK to 3MHz
	mbox[0] = 9 * 4; // buffer size
	mbox[1] = 0; // request code 0 = process request
	mbox[2] = MBOX_TAG_SETCLKRATE; // tag: set clock rate
	mbox[3] = 12; // tag request length
	mbox[4] = MBOX_CH_PROP; // property channel
	mbox[5] = MBOX_CH_VUART; // VUART channel
	mbox[6] = 4000000; // 4MHz
	mbox[7] = 0; // turbo off
	mbox[8] = MBOX_TAG_LAST; // end our message
	// send it
	mbox_send(MBOX_CH_PROP);

	// Divider = 4000000 / (16 * 115200) = 2.170138888888889 = ~2.
	mmio_write(UART0_IBRD, 2);
	// Fractional part register = (.170138888888889 * 64) + 0.5 = 11.388888888888896 = ~11.
	mmio_write(UART0_FBRD, 11);
 
	// Enable FIFO & 8 bit data transmission (1 stop bit, no parity).
	mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
 
	// Mask all interrupts.
	mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
	                       (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
 
	// Enable UART0, receive & transfer part of UART.
	mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

void uart_putc(unsigned char c)
{
	// Wait for UART to become ready to transmit.
	while ( mmio_read(UART0_FR) & (1 << 5) ) { }
	mmio_write(UART0_DR, c);
}
 
unsigned char uart_getc()
{
    // Wait for UART to have received something.
    while ( mmio_read(UART0_FR) & (1 << 4) ) { }
    return mmio_read(UART0_DR);
}
 
void uart_puts(const char* str)
{
	for (size_t i = 0; str[i] != '\0'; i ++)
		uart_putc((unsigned char)str[i]);
}
