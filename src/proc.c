#include <stddef.h>
#include "print.h"
#include "proc.h"
#include "alloc.h"
#include "stdlib.h"
#include "time.h"
#include "exceptions.h"
#include "signal.h"
#include "syscalls/files.h"
#include "errors.h"
#include "irq.h"
#include "input_buffer.h"

struct proc **proc_list = NULL;
struct proc *current_proc = NULL;
struct proc *front_proc = NULL;

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
            struct proc *new_proc = malloc(sizeof(struct proc));
            if (!new_proc) {
                print("proc_new: failed to allocate proc_list entry!\n");
                return E_NOMEM;
            }
            bzero(new_proc, sizeof(struct proc));
            new_proc->pid = i;
            void *stack = malloc_aligned_tagged(STACK_SIZE, PAGE_SIZE, new_proc);
            if (!stack) {
                print("proc_new: failed to allocate stack!\n");
                free(new_proc);
                return E_NOMEM;
            }
            bzero(stack, STACK_SIZE);
            new_proc->exception_stack = malloc_aligned_tagged(STACK_SIZE, PAGE_SIZE, new_proc);
            if (!new_proc->exception_stack) {
                print("proc_new: failed to allocate exception stack!\n");
                free_tag(new_proc);
                free(new_proc);
                return E_NOMEM;
            }
            bzero(new_proc->exception_stack, STACK_SIZE);
            new_proc->input_buffer = input_buffer_new();
            if (!new_proc->input_buffer) {
                print("proc_new: failed to allocate input buffer!\n");
                free_tag(new_proc);
                free(new_proc);
                return E_NOMEM;
            }
            new_proc->state.sp = (uintptr_t)stack + STACK_SIZE;
            new_proc->state.cpsr = 0; // EL0
            proc_list[i] = new_proc;
            return new_proc->pid;
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
    uintptr_t executable_mem = (uintptr_t)malloc_aligned(file_size, PAGE_SIZE);
    ssize_t size_read = read_syscall(fd, executable_mem, file_size);
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
    struct mentos_executable *exec = (struct mentos_executable *)executable_mem;
    if (exec->magic != EXECUTABLE_MAGIC) {
        print("incorrect executable magic: 0x%x\n", exec->magic);
        return E_FORMAT;
    }
    uintptr_t pc = executable_mem + exec->entry_offset;
    if (pc >= executable_mem + file_size) {
        print("entry point offset out of bounds in %s\n", path);
        return E_OOB;
    }
    int pid = proc_new();
    if (pid < 0) {
        free(executable_mem);
        return pid;
    }
    struct proc *p = proc_list[pid];
    p->state.pc = pc;
    return pid;
}

// switch to proc `p` for `time`, then return
void proc_enter(int pid, unsigned int time) {
    current_proc = proc_list[pid];
    timer_irq_after(time);
    proc_state_drop(&(current_proc->state), current_proc->exception_stack);
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
    // free memory
    free_tag(p);
    input_buffer_free(p->input_buffer);
    free(p);
}

// process tried doing an invalid syscall, kill it
void kill_invalid_syscall() {
    disable_irqs();
    proc_kill(current_proc->pid, SIGSYS);
    proc_exit(NULL); // move on
}

// We land here to handle an exception from a process.
void proc_exit(struct arm64_thread_state *state) {
    unsigned int pid;
    enter_critical_section();
    if (current_proc != NULL && state != NULL) {
        current_proc->state = *state;
        pid = current_proc->pid + 1;
    } else {
        pid = 1;
    }
    while (true) {
        bool has_procs = false;
        for (unsigned int i = pid; i < MAX_PROC * 2; i++) {
            unsigned int idx = i % MAX_PROC;
            if (proc_list[idx] != NULL) {
                has_procs = true;
                if (!proc_list[idx]->idle) {
                    exit_critical_section();
                    proc_enter(idx, PROC_TIME);
                }
            }
        }
        if (!has_procs) {
            panic("no processes running!");
        } else {
            proc_wait();
        }
    }
    panic("reached the end of proc_exit - we shouldn't be here");
}

/* run this when we're in a syscall and the scheduler must not
move to another process (file operations, etc) */
unsigned int critical_sections = 0;
void enter_critical_section() {
    if (!critical_sections) {
        disable_timer_irq();
    }
    critical_sections++;
}

// run when we're done doing critical stuff and can move on
void exit_critical_section() {
    critical_sections--;
    if (!critical_sections) {
        enable_timer_irq();
    }
}

// proc is idle - move to others or sleep
void proc_idle() {
    current_proc->idle = true;
    proc_next();
}

void proc_idle_release() {
    for (unsigned int i = 0; i < MAX_PROC; i++) {
        proc_list[i]->idle = false;
    }
}
