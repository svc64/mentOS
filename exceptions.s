.global irq_handler


.macro save_el0_state
    sub     sp, sp, #0x110
    stp     x0, x1, [sp, #16 * 0]
    stp     x2, x3, [sp, #16 * 1]
    stp     x4, x5, [sp, #16 * 2]
    stp     x6, x7, [sp, #16 * 3]
    stp     x8, x9, [sp, #16 * 4]
    stp     x10, x11, [sp, #16 * 5]
    stp     x12, x13, [sp, #16 * 6]
    stp     x14, x15, [sp, #16 * 7]
    stp     x16, x17, [sp, #16 * 8]
    stp     x18, x19, [sp, #16 * 9]
    stp     x20, x21, [sp, #16 * 10]
    stp     x22, x23, [sp, #16 * 11]
    stp     x24, x25, [sp, #16 * 12]
    stp     x26, x27, [sp, #16 * 13]
    stp     x28, x29, [sp, #16 * 14]
    mrs     x2, sp_el0
    mrs     x1, elr_el1
    stp     x30, x2, [sp, #16 * 15]
    str     x1, [sp, #16 * 16]
    mov     x0, sp
    add     sp, sp, #0x110
.endm

.macro unhandled_exc num
    ldr     x1, =_start
    mov     sp, x1
    mov     x0, \num
    b       panic_unhandled_exc
.endm

.macro exc_handle_el0 num
    msr	    tpidr_el1, x0
    ldr     x1, =_start
    mov     sp, x1
    mrs     x0, tpidr_el1
    save_el0_state
    mov     x1, \num
    bl      proc_exit
    eret
.endm

.macro ventry handler
    b \handler
    .align 7
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
    exc_handle_el0 8
irq_el0_64:     // IRQ, EL0 (64 bit)
    exc_handle_el0 9
fiq_el0_64:     // FIQ, EL0 (64 bit)
    exc_handle_el0 10
serror_el0_64:  // SError, EL0 (64 bit)
    exc_handle_el0 11
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
