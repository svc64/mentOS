.section ".text.boot"

.global _start

_start:
    // zero bss
    adr     x0, __bss_start
    adr     x1, __bss_end
zero_bss:
    str     xzr, [x0]
    cmp     x0, x1
    b.ne    iter
    b       check_el
iter:
    add     x0, x0, #8
    b       zero_bss
check_el:
    mrs     x0, currentel
    lsr     x0, x0, 2
    and     x0, x0, 3
    cmp     x0, 3
    b.ne    el2_to_el1
    b       el3_to_el1

el3_to_el1:
    mov     x0, #(3 << 20)
    msr     cpacr_el1, x0 // enable FP/SIMD

    // set all res1 bits to 1
    mov     x0, xzr
    orr     x0, x0, #(1 << 29)
    orr     x0, x0, #(1 << 28)
    orr     x0, x0, #(1 << 23)
    orr     x0, x0, #(1 << 22)
    orr     x0, x0, #(1 << 20)
    orr     x0, x0, #(1 << 11)
    // apply sctlr_el1 configuration
    msr     sctlr_el1, x0

    mrs     x0, scr_el3
    orr     x0, x0, #(1 << 10) // all lower ELs are 64 bit
    msr     scr_el3, x0

    mov     x0, #0b101 // EL1
    msr     spsr_el3, x0

    // set elr_el3 to where we drop to el1
    adr     x0, el1_start
    msr     elr_el3, x0

    // cya on the other side!
    eret

el2_to_el1:
    mov     x0, #(3 << 20)
    msr     cpacr_el1, x0 // enable FP/SIMD

    // set all res1 bits to 1
    mov     x0, xzr
    orr     x0, x0, #(1 << 29)
    orr     x0, x0, #(1 << 28)
    orr     x0, x0, #(1 << 23)
    orr     x0, x0, #(1 << 22)
    orr     x0, x0, #(1 << 20)
    orr     x0, x0, #(1 << 11)
    // apply sctlr_el1 configuration
    msr     sctlr_el1, x0

    mov     x0, #(1 << 31)      // AArch64
    orr     x0, x0, #(1 << 1)   // SWIO hardwired on Pi3
    msr     hcr_el2, x0
    mrs     x0, hcr_el2

    mov     x2, #0x0800
    movk    x2, #0x30d0, lsl #16
    msr     sctlr_el1, x2

    mov     x2, #0x3c5
    msr     spsr_el2, x2

    // set elr_el3 to where we drop to el1
    adr     x0, el1_start
    msr     elr_el2, x0

    // cya on the other side!
    eret

el1_start:
    // hang when running on a non primary CPU.
    mrs     x0, mpidr_el1
    and     x0, x0,#0xFF
    cbz     x0, cont
    b       .

cont:
    ldr     x1, =_start
    mov     sp, x1
    bl      main
    b       .
