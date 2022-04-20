.macro syscall name num
.global \name
\name:
    mov x16, \num
    svc #0x80
    ret
.endm
