#include <stddef.h>
#include "proc.h"
#include "exceptions.h"
void exit_syscall(int exit_code) {
    if (current_proc == NULL) {
        panic("exit_syscall(): current_proc == NULL");
    }
    proc_kill(current_proc->pid, exit_code);
    // process is killed, move on
    proc_exit(NULL);
}
