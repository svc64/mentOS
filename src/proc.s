.global el0_drop

.macro restore_el0_state_x30 // restore state from x30
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

el0_drop:
    mov     x30, x0
    msr     spsel, 1 // use SP_EL1 as SP
    // set exception stack
    ldr     x29, =_start
    mov     sp, x29
    msr     spsel, 0 // use SP_EL0 as SP
	restore_el0_state_x30
	eret
