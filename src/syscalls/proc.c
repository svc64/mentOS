#include <stddef.h>
#include "proc.h"
#include "time.h"
#include "exceptions.h"
#include "files.h"

void exit_syscall(int exit_code) {
    if (current_proc == NULL) {
        panic("exit_syscall(): current_proc == NULL");
    }
    current_proc_kill(exit_code);
}

int exec_syscall(char *path, bool background) {
    enter_critical_section();
    if (current_proc == NULL) {
        panic("exec_syscall(): current_proc == NULL");
    }
    int ret = proc_new_executable(path);
    if (ret < 0) {
        exit_critical_section();
        return ret;
    }
    if (!background) {
        current_proc->blocking_child = proc_list[ret];
    }
    exit_critical_section();
    proc_next();
    return ret;
}
