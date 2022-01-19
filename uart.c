#include <stddef.h>
#include "uart.h"
#include "time.h"
#include "mmio.h"

void init_uart() {
    // disable UART0 (while we're setting it up)
	mmio_write(UART0_CR, 0);

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
    volatile unsigned int  __attribute__((aligned(16))) mbox[9] = {
        sizeof(mbox), // buffer size
        0, // request code 0 = process request
        MBOX_TAG_SETCLKRATE, // tag: set clock rate 
        12, // tag request length
        MBOX_CH_PROP, // property channel
        MBOX_CH_VUART, // VUART channel
        3000000, // 3MHz
        0, // turbo
        MBOX_TAG_LAST // end our message
    };
    // The last 4 bits in a mailbox message specify the channel
	uint64_t r = (((uint64_t)(&mbox) & ~0xF) | MBOX_CH_PROP);

	// wait until we can talk to the VC
	while ( mmio_read(MBOX_STATUS) & 0x80000000 ) { }
	// send our message to property channel and wait for the response
	mmio_write(MBOX_WRITE, r);
	while ( (mmio_read(MBOX_STATUS) & 0x40000000) || mmio_read(MBOX_READ) != r ) { }
 
	// Divider = 3000000 / (16 * 115200) = 1.627 = ~1.
	mmio_write(UART0_IBRD, 1);
	// Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
	mmio_write(UART0_FBRD, 40);
 
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
