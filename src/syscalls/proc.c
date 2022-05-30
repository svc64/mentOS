#include <stddef.h>
#include "proc.h"
#include "exceptions.h"
void exit_syscall(int exit_code) {
    if (current_proc == NULL) {
        panic("exit_syscall(): current_proc == NULL");
    }
    current_proc_kill(exit_code);
}
