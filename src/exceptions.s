.global irq_handler

.macro save_el0_state
    // stash x0
    msr    tpidr_el1, x0
    // save elr and sp using x0
    mrs     x0, elr_el1
    str     x0, [sp, #-8]
    mrs     x0, sp_el0
    str     x0, [sp, #-16]
    sub     sp, sp, #16
    // restore x0
    mrs     x0, tpidr_el1
    stp     x29, x30, [sp, #-16]!
    stp     x27, x28, [sp, #-16]!
    stp     x25, x26, [sp, #-16]!
    stp     x23, x24, [sp, #-16]!
    stp     x21, x22, [sp, #-16]!
    stp     x19, x20, [sp, #-16]!
    stp     x17, x18, [sp, #-16]!
    stp     x15, x16, [sp, #-16]!
    stp     x13, x14, [sp, #-16]!
    stp     x11, x12, [sp, #-16]!
    stp     x9, x10, [sp, #-16]!
    stp     x7, x8, [sp, #-16]!
    stp     x5, x6, [sp, #-16]!
    stp     x3, x4, [sp, #-16]!
    stp     x1, x2, [sp, #-16]!
    stp     xzr, x0, [sp, #-16]!
.endm

.macro restore_state
    ldp     x30, x2, [x18, #16 * 15]
    ldr     x1, [x18, #16 * 16]
    msr     sp_el0, x2
    msr     elr_el1, x1
    ldp     x0, x1, [x18, #16 * 0]
    ldp     x2, x3, [x18, #16 * 1]
    ldp     x4, x5, [x18, #16 * 2]
    ldp     x6, x7, [x18, #16 * 3]
    ldp     x8, x9, [x18, #16 * 4]
    ldp     x10, x11, [x18, #16 * 5]
    ldp     x12, x13, [x18, #16 * 6]
    ldp     x14, x15, [x18, #16 * 7]
    ldp     x16, x17, [x18, #16 * 8]
    ldp     x20, x21, [x18, #16 * 10]
    ldp     x22, x23, [x18, #16 * 11]
    ldp     x24, x25, [x18, #16 * 12]
    ldp     x26, x27, [x18, #16 * 13]
    ldp     x28, x29, [x18, #16 * 14]
    ldp     x18, x19, [x18, #16 * 9]
.endm

.macro unhandled_exc num
    ldr     x1, =_start
    mov     sp, x1
    mov     x0, \num
    b       panic_unhandled_exc
.endm

.macro setup_stack
    msr	    tpidr_el1, x0
    ldr     x1, =_start
    mov     sp, x1
    mrs     x0, tpidr_el1
.endm

.macro exc_handle_el0 num, handler
    setup_stack
    save_el0_state
    run_el0_handler \num, \handler
.endm

.macro run_el0_handler num, handler
    mov     x0, sp
    add     x0, x0, #8 // x0 = pointer to state
    mov     x1, \num
    mrs     x2, esr_el1
    bl      \handler
    eret
.endm

.macro ventry handler
    b \handler
    .align 7
.endm

.macro disable_irqs
    msr    daifset, #2
.endm

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
    unhandled_exc 5
fiq_el1h:       // FIQ, EL1h
    unhandled_exc 6
serror_el1h:    // SError, EL1h
    unhandled_exc 7
// EL0, 64 bit
sync_el0_64:    // Synchronous, EL0 (64 bit)
    disable_irqs
    setup_stack
    save_el0_state
    // is it a syscall?
    // note: don't touch x0-x7, they are parameters for our syscall.
    mrs     x17, esr_el1
    lsr	    x18, x17, #26 // shift ESR to get the EC
    cmp     x18, #0b010101 // EC_SVC64
    b.eq    syscall_handler
handle_el0_sync: // TODO: fix this. handle other cases of el0 syncq
    disable_irqs
    run_el0_handler 8, el0_sync_handler
irq_el0_64:     // IRQ, EL0 (64 bit)
    exc_handle_el0 9, handle_irq
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
    adr     x17, syscall_table
    adr     x18, syscall_table_size
    ldr     x18, [x18]
    cmp     x16, x18 // check if the syscall number is valid
    b.hs    sigsys // invalid :(
    lsl     x16, x16, 4
    ldr     x18, [x17, x16, lsl #0]
    add     x16, x16, 8
    add     x17, x17, x16
    ldr     x19, [x17]
    // x18 = syscall pointer
    // x19 = return size (must preserve according to the calling convention)
    blr     x18
    mov     x18, sp
    add     x18, x18, 8
    # we have to check if the return value exists and if it's in w0 (32 bit) or x0 (64 bit)
    cmp     x19, 0
    b.ne    has_retval
ret_from_syscall:
    mov    x1, #0b00000
    msr    spsr_el1, x1
    restore_state
    msr    daifclr, #2 // enable IRQs
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
