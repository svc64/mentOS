.global syscall_table
.global syscall_table_size

.macro syscall func, ret_size
    .dword \func
    .dword \ret_size
.endm

syscall_table:
/* 0 */    syscall syscall_putc 0
/* 1 */    syscall syscall_test_return32 32
/* 2 */    syscall syscall_test_return64 64
/* 3 */    syscall syscall_getc 32
/* 4 */    syscall open_syscall 32
/* 5 */    syscall read_syscall 64
/* 6 */    syscall ftell_syscall 64
/* 7 */    syscall write_syscall 64
/* 8 */    syscall close_syscall 32
/* 9 */    syscall fsize_syscall 64
/* 10 */   syscall lseek_syscall 32
/* 11 */   syscall ftruncate_syscall 32
/* 12 */   syscall opendir_syscall 32
/* 13 */   syscall read_dir_syscall 32
/* 14 */   syscall closedir_syscall 32
syscall_table_size: .dword 15
