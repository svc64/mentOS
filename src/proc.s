#include "macros.s"
.global proc_state_drop

proc_state_drop:
    mov     x30, x0
    // set exception stack
    msr     tpidr_el1, x1
    msr     spsel, 1
    mov     sp, x1
    // restore state and bye
	restore_state_x30
	eret
