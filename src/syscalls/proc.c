#include <stddef.h>
#include "proc.h"
#include "time.h"
#include "exceptions.h"
#include "files.h"
#include "path.h"
#include "errors.h"
#include "alloc.h"
#include "stdlib.h"

void exit_syscall(int exit_code) {
    if (current_proc == NULL) {
        panic("exit_syscall(): current_proc == NULL");
    }
    current_proc_kill(exit_code);
}

int exec_syscall(char *path, char **argp, bool background) {
    enter_critical_section();
    bool front = false;
    int ret;
    if (current_proc == NULL) {
        panic("exec_syscall(): current_proc == NULL");
    }
    char *sane_path = sanitize_path(path, current_proc->cwd);
    if (!sane_path) {
        return E_NOMEM;
    }
    if (!argp) {
        argp = malloc(2 * sizeof(char *));
        if (!argp) {
            free(sane_path);
            return E_NOMEM;
        }
        argp[0] = path;
        argp[1] = NULL;
        ret = proc_new_executable(sane_path, argp, current_proc->cwd);
        free(argp);
    } else {
        ret = proc_new_executable(sane_path, argp, current_proc->cwd);
    }
    free(sane_path);
    if (ret < 0) {
        exit_critical_section();
        return ret;
    }
    if (!background) {
        current_proc->blocking_child = proc_list[ret];
        if (front_proc == current_proc) {
            front = true;
            front_proc = NULL;
        }
    }
    exit_critical_section();
    proc_next();
    if (front) {
        front_proc = current_proc;
    }
    return ret;
}

int chdir_syscall(char *path) {
    char *sane_path = sanitize_path(path, current_proc->cwd);
    if (!sane_path) {
        return E_NOMEM;
    }
    int ret = proc_chdir(sane_path, current_proc);
    free(sane_path);
    return ret;
}

int kill_syscall(int pid, int signal) {
    if (!pid) {
        panic("attempted to kill init!\n");
    }
    if (signal >= 0) {
        return E_PARAM;
    }
    enter_critical_section();
    if (pid >= MAX_PROC) {
        return E_NXPROC;
    }
    if (!proc_list[pid]) {
        return E_NXPROC;
    }
    bool next = false;
    if (proc_list[pid] == current_proc) {
        next = true;
    }
    proc_kill(pid, signal);
    exit_critical_section();
    if (next) {
        proc_next();
    }
    return 0;
}

void block_sigint_syscall() {
    current_proc->sigint_blocked = true;
}

void unblock_sigint_syscall() {
    current_proc->sigint_blocked = false;
}

int getcwd_syscall(char *buf, unsigned long size) {
    if (size < strlen(current_proc->cwd) + 1) {
        return E_NOMEM;
    }
    strcpy(buf, current_proc->cwd);
    return 0;
}
