.global do_some_syscall

do_some_syscall:
    mov x16, #1337
    svc #0x80
    ret
