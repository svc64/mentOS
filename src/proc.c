#include <stddef.h>
#include "print.h"
#include "proc.h"
#include "mem.h"
#include "stdlib.h"
#include "time.h"
#include "exceptions.h"
#include "signal.h"
#include "syscalls/files.h"
#include "errors.h"

struct proc **proc_list = NULL;
struct proc *current_proc = NULL;

void proc_init() {
    if (!proc_list) {
        size_t proc_list_size = MAX_PROC * sizeof(struct proc *);
        proc_list = malloc(proc_list_size);
        if (proc_list == NULL) {
            panic("proc_init: failed to allocate proc_list!");
        }
        bzero(proc_list, proc_list_size);
    }
}

// create a new process
int proc_new() {
    for (int i = 0; i < MAX_PROC; i++) {
        if (proc_list[i] == NULL) {
            // TODO: deal with allocation errors
            proc_list[i] = malloc(sizeof(struct proc));
            if (proc_list[i] == NULL) {
                print("proc_new: failed to allocate proc_list entry!\n");
                return E_NOMEM;
            }
            bzero(proc_list[i], sizeof(struct proc));
            proc_list[i]->pid = i;
            proc_list[i]->stack = malloc(PAGE_SIZE + STACK_SIZE);
            if (proc_list[i]->stack == NULL) {
                print("proc_new: failed to allocate stack!\n");
                free(proc_list[i]);
                return E_NOMEM;
            }
            bzero(proc_list[i]->stack, PAGE_SIZE + STACK_SIZE);
            proc_list[i]->state.sp = (((uintptr_t)proc_list[i]->stack + PAGE_SIZE) & -PAGE_SIZE) + STACK_SIZE;
            return proc_list[i]->pid;
        }
    }
    return E_LIMIT;
}

// Create a new process from a function
int proc_new_func(uintptr_t pc) {
    int pid = proc_new();
    if (pid >= 0) {
        struct proc *p = proc_list[pid];
        p->state.pc = pc;
    }
    return pid;
}

// Create a new process from an executable file
int proc_new_executable(const char *path) {
    int fd = open_syscall(path, O_READ);
    if (fd < 0) {
        print("failed to open executable %s\n", path);
        return fd; // fd is an error, return it.
    }
    ssize_t file_size = fsize_syscall(fd);
    if (file_size > 0xffffffff) {
        print("executable too big: %s\n", path);
        close_syscall(fd);
        return -1;
    }
    uintptr_t executable_mem = malloc(PAGE_SIZE + file_size);
    uintptr_t executable_start = (executable_mem + PAGE_SIZE) & -PAGE_SIZE;
    ssize_t size_read = read_syscall(fd, executable_start, file_size);
    if (size_read < 0) {
        close_syscall(fd);
        return size_read;
    }
    if (size_read != file_size) {
        print("failed to read executable %s\n", path);
        close_syscall(fd);
        return E_IOERR;
    }
    close_syscall(fd);
    struct mentos_executable *exec = (struct mentos_executable *)executable_start;
    if (exec->magic != EXECUTABLE_MAGIC) {
        print("incorrect executable magic: 0x%x\n", exec->magic);
        return E_FORMAT;
    }
    uintptr_t pc = executable_start + exec->entry_offset;
    if (pc >= executable_start + file_size) {
        print("entry point offset out of bounds in %s\n", path);
        return E_OOB;
    }
    int pid = proc_new();
    if (pid < 0) {
        free(executable_mem);
        return pid;
    }
    struct proc *p = proc_list[pid];
    p->executable = executable_mem;
    p->state.pc = pc;
    return pid;
}

// switch to proc `p` for `time`, then return
void proc_enter(int pid, unsigned int time) {
    current_proc = proc_list[pid];
    timer_irq_after(time);
    el0_drop(&(current_proc->state));
}

// kill proc `pid` with `signal`
void proc_kill(unsigned int pid, unsigned int signal) {
    print("[%d] killed with signal %d\n", pid, signal);
    struct proc *p = proc_list[pid];
    for (size_t i = 0; i < MAX_DESCRIPTORS; i++) {
        if (fds[i] != NULL) {
            if (fds[i]->proc == p) {
                if (close_syscall(i)) {
                    print("[%d] failed to close file descriptor %d\n", pid, i);
                    print("[%d] kernel closed file %d\n", pid, i);
                }
            }
        }
        if (dirs[i] != NULL) {
            if (dirs[i]->proc == p) {
                if (closedir_syscall(i)) {
                    print("[%d] failed to close directory descriptor %d\n", pid, i);
                } else {
                    print("[%d] kernel closed directory %d\n", pid, i);
                }
            }
        }
    }
    current_proc = NULL; // TODO: this can race with the next line (proc_list[pid] = NULL) in proc_exit. fix this if and when we implement SMP.
    proc_list[pid] = NULL; // context switcher shall not switch anymore.
    // free stack and proc struct
    free(p->stack);
    if (p->executable != NULL) {
        free(p->executable);
    }
    free(p);
}

// process tried doing an invalid syscall, kill it
void kill_invalid_syscall() {
    proc_kill(current_proc->pid, SIGSYS);
    proc_exit(NULL); // move on
}

// We land here to handle an exception from a process.
void proc_exit(struct arm64_thread_state *state) {
    unsigned int pid;
    if (current_proc != NULL) {
        current_proc->state = *state;
        pid = current_proc->pid + 1;
    } else {
        pid = 1;
    }
    for (unsigned int i = pid; i < MAX_PROC * 2; i++) {
        unsigned int idx = i % MAX_PROC;
        if (proc_list[idx] != NULL) {
            proc_enter(idx, PROC_TIME);
        }
    }
    // if we get here. no proc is running. terrible news.
    panic("no processes running!");
}
