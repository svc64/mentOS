#include <stddef.h>
#include <stdint.h>
#include "exceptions.h"
#include "print.h"
#include "proc.h"

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

void el0_sync_handler(struct arm64_thread_state *state, int exc, uint64_t elr) {
    // copy state to current_proc
    current_proc->state = *state;
    uint64_t ec = elr << 32 >> 58;
    print("%s EC:0x%x ELR:0x%x PC:0x%x\n", exc_str(exc), ec, elr, current_proc->state.pc);
    switch (ec)
    {
        case EC_SVC64:
            print("got syscall from pc: 0x%x\n", current_proc->state.pc);
            // return to the process
            proc_enter(current_proc->pid, PROC_TIME);
            break;
        default:
            break;
    }
    panic("el0_sync_handler did nothing. this should never happen.\n");
}

void panic_unhandled_exc(int exception_type) {
    uint64_t pc;
    __asm__ __volatile__("mrs %0, elr_el1\n\t" : "=r" (pc) :  : "memory");
    print("PANIC:\nUnhandled exception: %s\n", exc_str(exception_type));
    print("pc: 0x%x\n", pc);
    while (1);
}

void panic(const char *panic_msg) {
    print("PANIC: %s\n", panic_msg);
    while (1);
}
