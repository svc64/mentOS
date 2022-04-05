#include "mmio.h"
#include "mailbox.h"
volatile unsigned int  __attribute__((aligned(16))) mbox[36];

uint32_t mbox_send(uint8_t channel) {
    // The last 4 bits in a mailbox message specify the channel
    uint64_t r = (((uint64_t)(&mbox) & ~0xF) | channel & 0xF);

    // wait until we can talk to the VC
    while ( mmio_read(MBOX_STATUS) & MBOX_FULL) { }
    // send our message to property channel and wait for the response
    mmio_write(MBOX_WRITE, r);
    // wait while the mailbox is empty and while there's no response to our message
    while ( (mmio_read(MBOX_STATUS) & MBOX_EMPTY) || mmio_read(MBOX_READ) != r ) { }
    // if the response is successful, return 0. else return the response code.
    if (mbox[1] == MBOX_RESPONSE) {
        return 0; // success
    }
    return mbox[1]; // fail, return the error code
}
