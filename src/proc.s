.global el0_drop

.macro restore_state
    ldp     x30, x2, [x0, #16 * 15]
    ldr     x1, [x0, #16 * 16]
    msr     sp_el0, x2
	msr     elr_el1, x1
    ldp     x2, x3, [x0, #16 * 1]
    ldp     x4, x5, [x0, #16 * 2]
    ldp     x6, x7, [x0, #16 * 3]
    ldp     x8, x9, [x0, #16 * 4]
    ldp     x10, x11, [x0, #16 * 5]
    ldp     x12, x13, [x0, #16 * 6]
    ldp     x14, x15, [x0, #16 * 7]
    ldp     x16, x17, [x0, #16 * 8]
    ldp     x18, x19, [x0, #16 * 9]
    ldp     x20, x21, [x0, #16 * 10]
    ldp     x22, x23, [x0, #16 * 11]
    ldp     x24, x25, [x0, #16 * 12]
    ldp     x26, x27, [x0, #16 * 13]
    ldp     x28, x29, [x0, #16 * 14]
    // restore NEON state
    ldp     q0, q1, [x0, #32 * 8]
    ldp     q2, q3, [x0, #32 * 9]
    ldp     q4, q5, [x0, #32 * 10]
    ldp     q6, q7, [x0, #32 * 11]
    ldp     q8, q9, [x0, #32 * 12]
    ldp     q10, q11, [x0, #32 * 13]
    ldp     q12, q13, [x0, #32 * 14]
    ldp     q14, q15, [x0, #32 * 15]
    ldp     q16, q17, [x0, #32 * 16]
    ldp     q18, q19, [x0, #32 * 17]
    ldp     q20, q21, [x0, #32 * 18]
    ldp     q22, q23, [x0, #32 * 19]
    ldp     q24, q25, [x0, #32 * 20]
    ldp     q26, q27, [x0, #32 * 21]
    ldp     q28, q29, [x0, #32 * 22]
    ldp     q30, q31, [x0, #32 * 23]
    ldp     x0, x1, [x0, #16 * 0]
.endm

el0_drop:
    mov     x1, #0b0000
    msr     spsr_el1, x1
    ldr     x30, =_start
    mov     sp, x30
	restore_state
    msr     daifclr, #2 // enable IRQs
	eret
