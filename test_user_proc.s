.global invalid_syscall
.global putc_syscall
.global test_ret32
.global test_ret64

invalid_syscall:
    mov x16, #1337
    svc #0x80
    ret
helloworld: .ascii "Hello World!"
.byte 0
.align 4
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
