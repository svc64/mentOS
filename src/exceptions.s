#include "macros.s"

.align 11
.global exception_vectors
exception_vectors:
    // EL1t
    ventry sync_el1t
    ventry irq_el1t
    ventry fiq_el1t
    ventry serror_el1t
    // EL1h
    ventry sync_el1h
    ventry irq_el1h
    ventry fiq_el1h
    ventry serror_el1h
    // EL0, 64 bit
    ventry sync_el0_64
    ventry irq_el0_64
    ventry fiq_el0_64
    ventry serror_el0_64
    // EL0, 32 bit
    ventry sync_el0_32
    ventry irq_el0_32
    ventry fiq_el0_32
    ventry serror_el0_32

// EL1t
sync_el1t:      // Synchronous, EL1t
    unhandled_exc 0
irq_el1t:       // IRQ, EL1t
    unhandled_exc 1
fiq_el1t:       // FIQ, EL1t
    unhandled_exc 2
serror_el1t:    // SError, EL1t
    unhandled_exc 3
// EL1h
sync_el1h:      // Synchronous, EL1h
    unhandled_exc 4
irq_el1h:       // IRQ, EL1h
    disable_irqs
    push_state_el1
    run_handler 5, handle_irq
    pop_state_el1
    eret
fiq_el1h:       // FIQ, EL1h
    unhandled_exc 6
serror_el1h:    // SError, EL1h
    unhandled_exc 7
// EL0, 64 bit
sync_el0_64:    // Synchronous, EL0 (64 bit)
    disable_irqs
    push_state
    // TODO: fix this. handle other cases of el0 sync
    // is it a syscall?
    // note: don't touch x0-x7, they are parameters for our syscall.
    mrs     x19, esr_el1
    lsr     x18, x19, #26 // shift ESR to get the EC
    cmp     x18, #0b010101 // EC_SVC64
    b.eq    syscall_handler
    run_handler 8, el0_sync_handler
irq_el0_64:     // IRQ, EL0 (64 bit)
    disable_irqs
    exc_handler 9, handle_irq
fiq_el0_64:     // FIQ, EL0 (64 bit)
    disable_irqs
    unhandled_exc 10
serror_el0_64:  // SError, EL0 (64 bit)
    disable_irqs
    unhandled_exc 11
// we don't support 32 bit. we should NEVER get here.
// EL0, 32 bit
sync_el0_32:    // Synchronous, EL0 (32 bit)
    unhandled_exc 12
irq_el0_32:     // IRQ, EL0 (32 bit)
    unhandled_exc 13
fiq_el0_32:     // FIQ, EL0 (32 bit)
    unhandled_exc 14
serror_el0_32:  // SError, EL0 (32 bit)
    unhandled_exc 15
sigsys:
    bl kill_invalid_syscall
syscall_handler:
    adr     x19, syscall_table
    adr     x18, syscall_table_size
    ldr     x18, [x18]
    cmp     x16, x18 // check if the syscall number is valid
    b.hs    sigsys // invalid :(
    lsl     x16, x16, 4
    ldr     x18, [x19, x16, lsl #0]
    add     x16, x16, 8
    add     x19, x19, x16
    ldr     x19, [x19]
    // x18 = syscall pointer
    // x19 = return size
    enable_irqs
    blr     x18
    disable_irqs
    // point x18 to the saved x0 register
    mov     x18, sp
    add     x18, x18, 536 // x18 = &x0
    // we have to check if the return value exists and if it's in w0 (32 bit) or x0 (64 bit)
    cmp     x19, 0
    b.ne    has_retval
ret_from_syscall:
    pop_state
    eret
has_retval:
    cmp x19, 32
    b.ne ret64
ret32:
    str w0, [x18]
    b ret_from_syscall
ret64:
    str x0, [x18]
    b ret_from_syscall
