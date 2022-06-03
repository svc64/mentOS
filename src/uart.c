#include <stddef.h>
#include "uart.h"
#include "time.h"
#include "mmio.h"
#include "irq.h"
#include "proc.h"
#include "input_buffer.h"

void uart_putc(unsigned char c)
{
    // Wait for UART to become ready to transmit.
    while (mmio_read(UART0_FR) & UART0_FR_TXFF);
    mmio_write(UART0_DR, c);
}

unsigned char uart_getc()
{
    // Wait for UART to have received something.
    while (mmio_read(UART0_FR) & UART0_FR_RXFE);
    return mmio_read(UART0_DR);
}

void uart_puts(const char* str)
{
    for (size_t i = 0; str[i] != 0; i++) {
        uart_putc((unsigned char)str[i]);
    }
}

void uart_interrupts_clear(uint32_t interrupts) {
    mmio_write(UART0_ICR, interrupts);
}

size_t uart_buffer_size = 0;
void uart_recieved(char c) {
    if (c == '\x7f' || c == '\x8') {
        if (uart_buffer_size) {
            uart_puts("\x1B[1D");
            uart_puts("\x1B[1P");
            if (front_proc != NULL) {
                input_buffer_remove(front_proc->input_buffer);
            }
            uart_buffer_size--;
        }
    } else if (c == '\r') {
        if (front_proc != NULL) {
            input_buffer_push(front_proc->input_buffer, '\n');
        }
        uart_puts("\r\n");
        uart_buffer_size = 0;
    } else if (c == '\x4') {
        proc_sigint();
    } else if (c > 31 && c != 127) {
        if (front_proc != NULL) {
            input_buffer_push(front_proc->input_buffer, c);
        }
        uart_putc(c);
        uart_buffer_size++;
    }
}

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
    uart_interrupts_clear(UART0_INT_ALL);

    // set integer & fractional clock dividers
    // divider = UART_CLOCK/(16 * baudrate) (broadcom docs page 183)
    // Fraction part register = (Fractional part * 64) + 0.5
    // we want a baudrate of 115200 here

    // set UART_CLOCK to 4MHz
    mbox[0] = 9 * 4; // buffer size
    mbox[1] = 0; // request code 0 = process request
    mbox[2] = MBOX_TAG_SETCLKRATE; // tag: set clock rate
    mbox[3] = 12; // tag request length
    mbox[4] = 8; // size
// --------------mbox[3] is the size of this------------------|
//                                                            |
//  ------mbox[4] is the size of this------------------|      |
    mbox[5] = MBOX_CH_VUART; // VUART channel          |      |
    mbox[6] = 4000000; // 4MHz                         |      |
//  ---------------------------------------------------|      |
//                                                            |
    mbox[7] = 0; // turbo off                                 |
// -----------------------------------------------------------|
    mbox[8] = MBOX_TAG_LAST; // end our message
    // send it
    mbox_send(MBOX_CH_PROP);

    // Divider = 4000000 / (16 * 115200) = 2.170138888888889 = ~2.
    mmio_write(UART0_IBRD, 2);
    // Fractional part register = (.170138888888889 * 64) + 0.5 = 11.388888888888896 = ~11.
    mmio_write(UART0_FBRD, 11);

    // Enable FIFO & 8 bit data transmission (1 stop bit, no parity).
    mmio_write(UART0_LCRH, UART0_LCRH_WLEN8);

    // Enable the recieve interrupt.
    mmio_write(UART0_IMSC, UART0_INT_RXR);
    // Enable the UART IRQ.
    mmio_write(ENABLE_IRQS_2, UART0_IRQ);

    // Enable UART0, receive & transfer part of UART.
    mmio_write(UART0_CR, UART0_CR_UARTEN | UART0_CR_TXW | UART0_CR_RXE);
}
