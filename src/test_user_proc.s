.global invalid_syscall
.global putc_syscall
.global test_ret32
.global test_ret64
.global open
.global read
.global close
.global opendir
.global read_dir
.global closedir

.align 4
invalid_syscall:
    mov x16, #1337
    svc #0x80
    ret
putc_syscall:
    mov x16, #0
    svc #0x80
    ret
test_ret32:
    mov x16, #1
    svc #0x80
    ret
test_ret64:
    mov x16, #2
    svc #0x80
    ret
open:
    mov x16, #4
    svc #0x80
    ret
read:
    mov x16, #5
    svc #0x80
    ret
close:
    mov x16, #8
    svc #0x80
    ret
opendir:
    mov x16, #12
    svc #0x80
    ret
read_dir:
    mov x16, #13
    svc #0x80
    ret
closedir:
    mov x16, #14
    svc #0x80
    ret
