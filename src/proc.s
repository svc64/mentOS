#include "macros.s"
.global proc_state_drop
.global proc_wait
.global proc_next

proc_state_drop:
    mov     x30, x0
    // set exception stack
    msr     tpidr_el1, x1
    msr     spsel, 1
    mov     sp, x1
    // restore state and bye
    disable_irqs
    restore_state_x30
    eret

proc_wait:
    wfi
    ret

// push el1 state with a custom label as pc
.macro push_el1_state_pc pc_label
    // push the current state
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
    // pc = return trampoline
    adr     x20, \pc_label
    stp     x20, x0, [sp, #-16]! // save x0 and pc
    // save sp and ""cpsr""
    mrs     x21, daif
    mrs     x22, nzcv
    mrs     x23, currentel
    mrs     x24, spsel
    orr     x21, x21, x22
    orr     x21, x21, x23
    orr     x21, x21, x24
    mov     x20, sp
    add     x20, x20, #256 // fix sp
    stp     x21, x20, [sp, #-16]! // x21 = ""cpsr"", x20 = sp
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

proc_next:
    push_el1_state_pc proc_next_ret
    mov     x0, sp
    bl      proc_exit
proc_next_ret:
    ret
