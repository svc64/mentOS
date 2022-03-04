.section ".text"

.global exception_vectors
.global irq_handler

exception_vectors:
// EL1t
sync_el1t:      // Synchronous, EL1t
    mov x0, 0
    b   panic_unhandled_exc
    .align 7
irq_el1t:       // IRQ, EL1t
    mov x0, 1
    b   panic_unhandled_exc
    .align 7
fiq_el1t:       // FIQ, EL1t
    mov x0, 2
    b   panic_unhandled_exc
    .align 7
serror_el1t:    // SError, EL1t
    mov x0, 3
    b   panic_unhandled_exc
    .align 7

// EL1h
sync_el1h:      // Synchronous, EL1h
    mov x0, 4
    b   panic_unhandled_exc
    .align 7
irq_el1h:       // IRQ, EL1h
    bl irq_handler
    eret
    .align 7
fiq_el1h:       // FIQ, EL1h
    mov x0, 6
    b   panic_unhandled_exc
    .align 7
serror_el1h:    // SError, EL1h
    mov x0, 7
    b   panic_unhandled_exc
    .align 7

// EL0, 64 bit
sync_el0_64:    // Synchronous, EL0 (64 bit)
    mov x0, 8
    b   panic_unhandled_exc
    .align 7
irq_el0_64:     // IRQ, EL0 (64 bit)
    mov x0, 9
    b   panic_unhandled_exc
    .align 7
fiq_el0_64:     // FIQ, EL0 (64 bit)
    mov x0, 10
    b   panic_unhandled_exc
    .align 7
serror_el0_64:  // SError, EL0 (64 bit)
    mov x0, 11
    b   panic_unhandled_exc
    .align 7

// EL0, 32 bit
sync_el0_32:    // Synchronous, EL0 (32 bit)
    mov x0, 12
    b   panic_unhandled_exc
    .align 7
irq_el0_32:     // IRQ, EL0 (32 bit)
    mov x0, 13
    b   panic_unhandled_exc
    .align 7
fiq_el0_32:     // FIQ, EL0 (32 bit)
    mov x0, 14
    b   panic_unhandled_exc
    .align 7
serror_el0_32:  // SError, EL0 (32 bit)
    mov x0, 15
    b   panic_unhandled_exc
    .align 7
