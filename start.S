.section ".text.boot"

.global _start

_start:
    // hang when running on a non primary CPU.
    mrs    x0, mpidr_el1        
    and    x0, x0,#0xFF
    cbz    x0, cont
    b .

cont:
    ldr     x1, =_start
    mov     sp, x1
    bl main
    b .
