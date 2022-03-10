#include <stddef.h>
#include "exceptions.h"
#include "print.h"

// exception number to string
const char *exc_str(int exc) {
    switch (exc)
    {
    case EXC_SYNC_EL1t:
        return "Synchronous, EL1t";
    case EXC_IRQ_EL1t:
        return "IRQ, EL1t";
    case EXC_FIQ_EL1t:
        return "FIQ, EL1t";
    case EXC_SERROR_EL1t:
        return "SError, EL1t";

    case EXC_SYNC_EL1h:
        return "Synchronous, EL1h";
    case EXC_IRQ_EL1h:
        return "IRQ, EL1h";
    case EXC_FIQ_EL1h:
        return "FIQ, EL1h";
    case EXC_SERROR_EL1h:
        return "SError, EL1h";

    case EXC_SYNC_EL0_64:
        return "Synchronous, EL0 (64 bit)";
    case EXC_IRQ_EL0_64:
        return "IRQ, EL0 (64 bit)";
    case EXC_FIQ_EL0_64:
        return "FIQ, EL0 (64 bit)";
    case EXC_SERROR_EL0_64:
        return "SError, EL0 (64 bit)";

    case EXC_SYNC_EL0_32:
        return "Synchronous, EL0 (32 bit)";
    case EXC_IRQ_EL0_32:
        return "IRQ, EL0 (32 bit)";
    case EXC_FIQ_EL0_32:
        return "FIQ, EL0 (32 bit)";
    case EXC_SERROR_EL0_32:
        return "SError, EL0 (32 bit)";

    default:
        return NULL;
    }
}

void panic_unhandled_exc(int exception_type) {
    uint64_t pc;
    __asm__ __volatile__("mrs %0, elr_el1\n\t" : "=r" (pc) :  : "memory");
    print("PANIC:\nUnhandled exception: %s\n", exc_str(exception_type));
    print("pc: 0x%x\n", pc);
    while (1);
}
void panic(const char *panic_msg) {
    print("%s\n", panic_msg);
    while (1);
}
