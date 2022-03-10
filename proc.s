.global el0_drop
.macro save_state reg
    stp     x0, x1, [\reg, #16 * 0]
    stp     x2, x3, [\reg, #16 * 1]
    stp     x4, x5, [\reg, #16 * 2]
    stp     x6, x7, [\reg, #16 * 3]
    stp     x8, x9, [\reg, #16 * 4]
    stp     x10, x11, [\reg, #16 * 5]
    stp     x12, x13, [\reg, #16 * 6]
    stp     x14, x15, [\reg, #16 * 7]
    stp     x16, x17, [\reg, #16 * 8]
    stp     x18, x19, [\reg, #16 * 9]
    stp     x20, x21, [\reg, #16 * 10]
    stp     x22, x23, [\reg, #16 * 11]
    stp     x24, x25, [\reg, #16 * 12]
    stp     x26, x27, [\reg, #16 * 13]
    stp     x28, x29, [\reg, #16 * 14]
    mrs     x2, sp_el0
    mrs     x1, elr_el1
    stp     x30, x2, [\reg, #16 * 15]
    str     x1, [\reg, #16 * 16]
.endm

.macro restore_state reg
    ldp     x30, x2, [\reg, #16 * 15]
    ldr     x1, [\reg, #16 * 16]
    msr     sp_el0, x2
	msr     elr_el1, x1
    ldp     x2, x3, [\reg, #16 * 1]
    ldp     x4, x5, [\reg, #16 * 2]
    ldp     x6, x7, [\reg, #16 * 3]
    ldp     x8, x9, [\reg, #16 * 4]
    ldp     x10, x11, [\reg, #16 * 5]
    ldp     x12, x13, [\reg, #16 * 6]
    ldp     x14, x15, [\reg, #16 * 7]
    ldp     x16, x17, [\reg, #16 * 8]
    ldp     x18, x19, [\reg, #16 * 9]
    ldp     x20, x21, [\reg, #16 * 10]
    ldp     x22, x23, [\reg, #16 * 11]
    ldp     x24, x25, [\reg, #16 * 12]
    ldp     x26, x27, [\reg, #16 * 13]
    ldp     x28, x29, [\reg, #16 * 14]
    ldp     x0, x1, [\reg, #16 * 0]
.endm


el0_drop:
	mov    x1, #0b00000
    msr    spsr_el1, x1
	restore_state x0
	eret
