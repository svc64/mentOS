#include <stdint.h>
extern uint8_t exception_vectors;
#define EXC_SYNC_EL1t       0
#define EXC_IRQ_EL1t        1
#define EXC_FIQ_EL1t        2
#define EXC_SERROR_EL1t     3

#define EXC_SYNC_EL1h       4
#define EXC_IRQ_EL1h        5
#define EXC_FIQ_EL1h        6
#define EXC_SERROR_EL1h     7

#define EXC_SYNC_EL0_64     8
#define EXC_IRQ_EL0_64      9
#define EXC_FIQ_EL0_64      10
#define EXC_SERROR_EL0_64   11

#define EXC_SYNC_EL0_32     12
#define EXC_IRQ_EL0_32      13
#define EXC_FIQ_EL0_32      14
#define EXC_SERROR_EL0_32   15

#define PANIC_UNHANDLED_IRQ EXC_IRQ_EL1h

// ESR (Exception Syndrome Register) EC (Exception Class, bits [31:26]) values
#define EC_SVC64            0b010101 // SVC instruction execution in AArch64 state.


void panic_unhandled_exc(int exception_type);
void panic(const char *panic_msg);
const char *exc_str(int exc);
