.global syscall_table
.global syscall_table_size

.macro syscall func, ret_size
    .dword \func
    .dword \ret_size
.endm

syscall_table:
/* 0 */     syscall syscall_putc 0
/* 1 */     syscall syscall_test_return32 32
/* 2 */     syscall syscall_test_return64 64
/* 3 */     syscall syscall_getc 32
/* 4 */     syscall open_syscall 32
/* 5 */     syscall read_syscall 64
/* 6 */     syscall ftell_syscall 64
/* 7 */     syscall write_syscall 64
/* 8 */     syscall close_syscall 32
/* 9 */     syscall fsize_syscall 64
/* 10 */    syscall lseek_syscall 32
/* 11 */    syscall ftruncate_syscall 32
/* 12 */    syscall opendir_syscall 32
/* 13 */    syscall read_dir_syscall 32
/* 14 */    syscall closedir_syscall 32
/* 15 */    syscall mkdir_syscall 32
/* 16 */    syscall unlink_syscall 32
/* 17 */    syscall rename_syscall 32
/* 18 */    syscall exit_syscall 0
/* 19 */    syscall syscall_input_read 64
/* 20 */    syscall malloc_syscall 64
/* 21 */    syscall free_syscall 64
/* 22 */    syscall realloc_syscall 64
/* 23 */    syscall exec_syscall 32
/* 24 */    syscall chdir_syscall 32
/* 25 */    syscall syscall_input_flush 0
/* 26 */    syscall block_sigint_syscall 0
/* 27 */    syscall unblock_sigint_syscall 0
syscall_table_size: .dword 28
