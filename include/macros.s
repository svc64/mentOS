.macro unhandled_exc num
    ldr     x1, =_start
    mov     sp, x1
    mov     x0, \num
    b       panic_unhandled_exc
.endm

.macro exc_handler num, handler
    push_state
    run_handler \num, \handler
    pop_state
    eret
.endm

.macro run_handler num, handler
    mov     x0, sp
    mov     x1, \num
    mrs     x2, esr_el1
    bl      \handler
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

.macro push_state
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
    mrs     x21, spsr_el1
    mrs     x22, spsel // save spsel to x22
    msr     spsel, x21
    mov     x20, sp
    msr     spsel, x22
    stp     x21, x20, [sp, #-16]! // x21 = spsr, x20 = sp
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

.macro pop_state
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
    mrs     x2, spsel // save spsel to x2
    msr     spsel, x1
    mov     sp, x0
    msr     spsel, x2 // restore spsel
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

.macro push_state_el1
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
    mrs     x21, spsr_el1
    mov     x20, sp
    add     x20, x20, #256 // fix sp
    stp     x21, x20, [sp, #-16]! // x21 = spsr, x20 = sp
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

.macro pop_state_el1
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

.macro restore_state_x30 // restore state from x30
    ldp     q0, q1, [x30], #32
    ldp     q2, q3, [x30], #32
    ldp     q4, q5, [x30], #32
    ldp     q6, q7, [x30], #32
    ldp     q8, q9, [x30], #32
    ldp     q10, q11, [x30], #32
    ldp     q12, q13, [x30], #32
    ldp     q14, q15, [x30], #32
    ldp     q16, q17, [x30], #32
    ldp     q18, q19, [x30], #32
    ldp     q20, q21, [x30], #32
    ldp     q22, q23, [x30], #32
    ldp     q24, q25, [x30], #32
    ldp     q26, q27, [x30], #32
    ldp     q28, q29, [x30], #32
    ldp     q30, q31, [x30], #32
    ldp     x1, x0, [x30], #16 // x1 = spsr, x0 = sp
    msr     spsr_el1, x1
    msr     spsel, x1 // select the right sp
    mov     sp, x0
    ldp     x1, x0, [x30], #16 // restore x0 and pc
    msr     elr_el1, x1
    // load the rest
    ldp     x1, x2, [x30], #16
    ldp     x3, x4, [x30], #16
    ldp     x5, x6, [x30], #16
    ldp     x7, x8, [x30], #16
    ldp     x9, x10, [x30], #16
    ldp     x11, x12, [x30], #16
    ldp     x13, x14, [x30], #16
    ldp     x15, x16, [x30], #16
    ldp     x17, x18, [x30], #16
    ldp     x19, x20, [x30], #16
    ldp     x21, x22, [x30], #16
    ldp     x23, x24, [x30], #16
    ldp     x25, x26, [x30], #16
    ldp     x27, x28, [x30], #16
    ldp     x29, x30, [x30]
.endm
