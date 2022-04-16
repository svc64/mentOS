#include <stddef.h>
#include "print.h"
#include "proc.h"
#include "mem.h"
#include "stdlib.h"
#include "time.h"
#include "exceptions.h"
#include "signal.h"
#include "syscalls/files_kernel.h"

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

// once we support executables, maybe instead of providing pc
// we'll provide an executable address? idk, we'll see.
int proc_new(uintptr_t pc) {
    for (int i = 0; i < MAX_PROC; i++) {
        if (proc_list[i] == NULL) {
            // TODO: deal with allocation errors
            proc_list[i] = malloc(sizeof(struct proc));
            if (proc_list[i] == NULL) {
                print("proc_new: failed to allocate proc_list entry!\n");
                return -1;
            }
            bzero(proc_list[i], sizeof(struct proc));
            proc_list[i]->pid = i;
            proc_list[i]->stack = malloc(PAGE_SIZE + STACK_SIZE);
            if (proc_list[i]->stack == NULL) {
                print("proc_new: failed to allocate stack!\n");
                free(proc_list[i]);
            }
            bzero(proc_list[i]->stack, PAGE_SIZE + STACK_SIZE);
            proc_list[i]->state.pc = pc;
            proc_list[i]->state.sp = (((uintptr_t)proc_list[i]->stack + PAGE_SIZE) & -PAGE_SIZE) + STACK_SIZE;
            return proc_list[i]->pid;
        }
    }
    return -1;
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
