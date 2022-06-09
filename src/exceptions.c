#include <stddef.h>
#include <stdint.h>
#include "exceptions.h"
#include "print.h"
#include "proc.h"
#include "irq.h"
#include "sysreg.h"

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

void el0_sync_handler(struct arm64_thread_state *state, int exc, uint64_t esr) {
    print("ESR: 0x%x\n", esr);
    switch (ESR_ELx_EC(esr)) {
        case ESR_EL1_EC_ILLEGAL_EXEC:
            panic("Illegal Execution state!");
        case ESR_EL1_EC_WFI_WFE:
        case ESR_EC_BRK64:
        case ESR_EC_BRK_DBG_EL0:
        case ESR_EL1_EC_MRS_MSR_64:
            panic("Illegal instruction!");
        case ESR_EL1_EC_IABT_LEL:
            panic("Instruction fetch error: Invalid PC <TODO: print pc here>");
        case ESR_EL1_EC_SVC_64:
            panic("syscall hit el0_sync_handler instead of the syscall handler");
        case ESR_EL1_EC_PC_ALIGN:
            panic("PC alignment fault!");
        case ESR_EL1_EC_DABT_LEL:
            panic("Data abort!\n");
        case ESR_EL1_EC_SP_ALIGN:
            panic("SP alignment fault!");
        case ESR_EL1_EC_FP_64:
            panic("FPU exception!");
        case ESR_EL1_EC_SError:
            panic("EL0 SError");
        case ESR_EL1_EC_UNKNOWN:
            panic("unknown EL0 sync exception?!");
        case ESR_EL1_EC_ENFP:
            panic("FPU instructions are trapped when they shouldn't be!");
        case ESR_EL1_EC_SVC_32:
        case ESR_EL1_EC_FP_32:
            print("32 bit exception! ESR: 0x%x\n", esr);
            panic("32 bit exception!");
        case ESR_EC_SS_DBG_EL0:
            panic("Software step is unsupported!");
        case ESR_EC_WATCHP_EL0:
            panic("EL0 watchpoint exception!");
        case ESR_EC_WATCHP_EL1:
        case ESR_EL1_EC_DABT_CEL:
        case ESR_EL1_EC_IABT_CEL:
        case ESR_EC_BRK_DBG_EL1:
        case ESR_EC_SS_DBG_EL1:
            print("EL1 exception! ESR: 0x%x\n", esr);
            panic("EL1 exception!");
    }
}

void panic_unhandled_exc(int exception_type) {
    disable_irqs();
    uint64_t pc;
    __asm__ __volatile__("mrs %0, elr_el1\n\t" : "=r" (pc) :  : "memory");
    print("PANIC:\nUnhandled exception: %s\n", exc_str(exception_type));
    print("pc: 0x%x\n", pc);
    while (1);
}

void panic(const char *panic_msg) {
    disable_irqs();
    print("PANIC: %s\n", panic_msg);
    while (1);
}
