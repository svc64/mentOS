.global syscall_table
.global syscall_table_size

.macro syscall func, ret_size
    .dword \func
    .dword \ret_size
.endm

syscall_table:
    syscall syscall_putc 0
    syscall syscall_test_return32 32
    syscall syscall_test_return64 64
syscall_table_size: .dword 3