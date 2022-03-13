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
    ldp     x0, x1, [x0, #16 * 0]
.endm

el0_drop:
	mov    x1, #0b00000
    msr    spsr_el1, x1
	restore_state
    msr    daifclr, #2 // enable IRQs
	eret
