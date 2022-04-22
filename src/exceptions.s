.global irq_handler

.macro unhandled_exc num
    ldr     x1, =_start
    mov     sp, x1
    mov     x0, \num
    b       panic_unhandled_exc
.endm

.macro exc_handle_el0 num, handler
    push_el0_state
    run_el0_handler \num, \handler
.endm

.macro run_el0_handler num, handler
    mov     x0, sp
    add     x0, x0, #0x310 // x0 = pointer to state
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

.macro enable_irqs
    msr    daifclr, #2
.endm

.macro push_el0_state
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
    // save pc, spsr and sp
    // read pc to x1
    mrs     x20, elr_el1
    stp     x20, x0, [sp, #-16]! // save x0 and pc
    // save sp and spsr
    mrs     x20, sp_el0
    mrs     x21, spsr_el1
    stp     x21, x20, [sp, #-16]! // x1 = spsr, x0 = sp
    stp     q30, q31, [sp, #-32]!
    stp     q28, q29, [sp, #-32]!
    stp     q26, q27, [sp, #-32]!
    stp     q24, q25, [sp, #-32]!
    stp     q22, q23, [sp, #-32]!
    stp     q20, q21, [sp, #-32]!
    stp     q18, q19, [sp, #-32]!
    stp     q16, q17, [sp, #-32]!
    stp     q14, q15, [sp, #-32]!
    stp     q12, q13, [sp, #-32]!
    stp     q10, q11, [sp, #-32]!
    stp     q8, q9, [sp, #-32]!
    stp     q6, q7, [sp, #-32]!
    stp     q4, q5, [sp, #-32]!
    stp     q2, q3, [sp, #-32]!
    stp     q0, q1, [sp, #-32]!
.endm

.macro pop_el0_state
    ldp     q0, q1, [sp], #32
    ldp     q2, q3, [sp], #32
    ldp     q4, q5, [sp], #32
    ldp     q6, q7, [sp], #32
    ldp     q8, q9, [sp], #32
    ldp     q10, q11, [sp], #32
    ldp     q12, q13, [sp], #32
    ldp     q14, q15, [sp], #32
    ldp     q16, q17, [sp], #32
    ldp     q18, q19, [sp], #32
    ldp     q20, q21, [sp], #32
    ldp     q22, q23, [sp], #32
    ldp     q24, q25, [sp], #32
    ldp     q26, q27, [sp], #32
    ldp     q28, q29, [sp], #32
    ldp     q30, q31, [sp], #32
    ldp     x1, x0, [sp], #16 // x1 = spsr, x0 = sp
    msr     spsr_el1, x1
    msr     sp_el0, x0
    ldp     x1, x0, [sp], #16 // restore x0 and pc
    msr     elr_el1, x1
    // load the rest
    ldp     x1, x2, [sp], #16
    ldp     x3, x4, [sp], #16
    ldp     x5, x6, [sp], #16
    ldp     x7, x8, [sp], #16
    ldp     x9, x10, [sp], #16
    ldp     x11, x12, [sp], #16
    ldp     x13, x14, [sp], #16
    ldp     x15, x16, [sp], #16
    ldp     x17, x18, [sp], #16
    ldp     x19, x20, [sp], #16
    ldp     x21, x22, [sp], #16
    ldp     x23, x24, [sp], #16
    ldp     x25, x26, [sp], #16
    ldp     x27, x28, [sp], #16
    ldp     x29, x30, [sp], #16
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
    push_el0_state
    // TODO: fix this. handle other cases of el0 sync
    // is it a syscall?
    // note: don't touch x0-x7, they are parameters for our syscall.
    mrs     x19, esr_el1
    lsr     x18, x19, #26 // shift ESR to get the EC
    cmp     x18, #0b010101 // EC_SVC64
    b.eq    syscall_handler
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
    blr     x18
    // point x18 to the saved x0 register
    mov     x18, sp
    add     x18, x18, 536 // x18 = &x0
    # we have to check if the return value exists and if it's in w0 (32 bit) or x0 (64 bit)
    cmp     x19, 0
    b.ne    has_retval
ret_from_syscall:
    mov    x1, #0b00000
    msr    spsr_el1, x1
    pop_el0_state
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
