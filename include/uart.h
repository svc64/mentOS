#include "gpio.h"
#include "mailbox.h"
#define UART0_BASE      0x201000
#define UART0_DR        (UART0_BASE + 0x00)
#define UART0_RSRECR    (UART0_BASE + 0x04)
#define UART0_FR        (UART0_BASE + 0x18)
#define UART0_ILPR      (UART0_BASE + 0x20)
#define UART0_IBRD      (UART0_BASE + 0x24)
#define UART0_FBRD      (UART0_BASE + 0x28)
#define UART0_LCRH      (UART0_BASE + 0x2C)
#define UART0_CR        (UART0_BASE + 0x30)
#define UART0_IFLS      (UART0_BASE + 0x34)
#define UART0_IMSC      (UART0_BASE + 0x38)
#define UART0_RIS       (UART0_BASE + 0x3C)
#define UART0_MIS       (UART0_BASE + 0x40)
#define UART0_ICR       (UART0_BASE + 0x44)
#define UART0_DMACR     (UART0_BASE + 0x48)
#define UART0_ITCR      (UART0_BASE + 0x80)
#define UART0_ITIP      (UART0_BASE + 0x84)
#define UART0_ITOP      (UART0_BASE + 0x88)
#define UART0_TDR       (UART0_BASE + 0x8C)

// Data register bits
#define UART0_DR_OE (1 << 11) // Overrun error
#define UART0_DR_BE (1 << 10) // Break error
#define UART0_DR_PE (1 <<  9) // Parity error
#define UART0_DR_FE (1 <<  8) // Framing error

// Receive Status Register / Error Clear Register
#define UART0_RSRECR_OE (1 << 3) // Overrun error
#define UART0_RSRECR_BE (1 << 2) // Break error
#define UART0_RSRECR_PE (1 << 1) // Parity error
#define UART0_RSRECR_FE (1 << 0) // Framing error

// Flag Register (depends on LCRH.FEN)
#define UART0_FR_TXFE (1 << 7) // Transmit FIFO empty
#define UART0_FR_RXFF (1 << 6) // Receive FIFO full
#define UART0_FR_TXFF (1 << 5) // Transmit FIFO full
#define UART0_FR_RXFE (1 << 4) // Receive FIFO empty
#define UART0_FR_BUSY (1 << 3) // BUSY transmitting data
#define UART0_FR_CTS  (1 << 0) // Clear To Send

// Line Control Register
#define UART0_LCRH_SPS (1 << 7) // sticky parity selected
#define UART0_LCRH_WLEN  (3 << 5) // word length (5, 6, 7 or 8 bit)
#define UART0_LCRH_WLEN5 (0 << 5) // word length 5 bit
#define UART0_qLCRH_WLEN6 (1 << 5) // word length 6 bit
#define UART0_LCRH_WLEN7 (2 << 5) // word length 7 bit
#define UART0_LCRH_WLEN8 (3 << 5) // word length 8 bit
#define UART0_LCRH_FEN (1 << 4) // Enable FIFOs
#define UART0_LCRH_STP2  (1 << 3) // Two stop bits select
#define UART0_LCRH_EPS (1 << 2) // Even Parity Select
#define UART0_LCRH_PEN (1 << 1) // Parity enable
#define UART0_LCRH_BRK (1 << 0) // send break

// Control Register
#define UART0_CR_CTSEN  (1 << 15) // CTS hardware flow control
#define UART0_CR_RTSEN  (1 << 14) // RTS hardware flow control
#define UART0_CR_RTS (1 << 11) // (not) Request to send
#define UART0_CR_RXE (1 <<  9) // Receive enable
#define UART0_CR_TXW (1 <<  8) // Transmit enable
#define UART0_CR_LBE (1 <<  7) // Loopback enable
#define UART0_CR_UARTEN (1 <<  0) // UART enable

// Interrupts (IMSC / RIS / MIS / ICR)
#define UART0_INT_OER (1 << 10) // Overrun error interrupt
#define UART0_INT_BER (1 <<  9) // Break error interrupt
#define UART0_INT_PER (1 <<  8) // Parity error interrupt
#define UART0_INT_FER (1 <<  7) // Framing error interrupt
#define UART0_INT_RTR (1 <<  6) // Receive timeout interrupt
#define UART0_INT_TXR (1 <<  5) // Transmit interrupt
#define UART0_INT_RXR (1 <<  4) // Receive interrupt
#define UART0_INT_DSRRM (1 <<  3) // unsupported / write zero
#define UART0_INT_DCDRM (1 <<  2) // unsupported / write zero
#define UART0_INT_CTSRM (1 <<  1) // nUARTCTS modem interrupt
#define UART0_INT_RIRM  (1 <<  0) // unsupported / write zero
#define UART0_INT_ALL (0x7F2)

#define UART0_IFSL_RXIFLSEL (7 << 3)   // Receive interrupt FIFO level select
#define UART0_IFSL_RX_1_8 (0b000 << 3) // Receive FIFO 1/8 full
#define UART0_IFSL_RX_1_4 (0b001 << 3) // Receive FIFO 1/4 full
#define UART0_IFSL_RX_1_2 (0b010 << 3) // Receive FIFO 1/2 full
#define UART0_IFSL_RX_3_4 (0b011 << 3) // Receive FIFO 3/4 full
#define UART0_IFSL_RX_7_8 (0b100 << 3) // Receive FIFO 7/8 full
#define UART0_IFSL_TXIFLSEL (7 << 0)   // Transmit interrupt FIFO level select
#define UART0_IFSL_TX_1_8 (0b000 << 0) // Transmit FIFO 1/8 full
#define UART0_IFSL_TX_1_4 (0b001 << 0) // Transmit FIFO 1/4 full
#define UART0_IFSL_TX_1_2 (0b010 << 0) // Transmit FIFO 1/2 full
#define UART0_IFSL_TX_3_4 (0b011 << 0) // Transmit FIFO 3/4 full
#define UART0_IFSL_TX_7_8 (0b100 << 0) // Transmit FIFO 7/8 full

void uart_putc(unsigned char c);
unsigned char uart_getc();
void uart_puts(const char* str);
void uart_interrupts_clear(uint32_t interrupt);
void init_uart();
void uart_recieved(char c);
