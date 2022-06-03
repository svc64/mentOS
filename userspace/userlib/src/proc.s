.include "src/macros.s"
syscall exit 18
syscall exec 23
syscall chdir 24
syscall block_sigint 26
syscall unblock_sigint 27
