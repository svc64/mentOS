#include <stddef.h>
#include "print.h"
#include "proc.h"
#include "alloc.h"
#include "stdlib.h"
#include "time.h"
#include "exceptions.h"
#include "signal.h"
#include "files.h"
#include "errors.h"
#include "irq.h"
#include "input_buffer.h"
#include "path.h"

#define MAX_ARGS    30

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
int proc_new(char *cwd) {
    enter_critical_section();
    for (int i = 0; i < MAX_PROC; i++) {
        if (proc_list[i] == NULL) {
            struct proc *new_proc = malloc(sizeof(struct proc));
            if (!new_proc) {
                print("proc_new: failed to allocate proc_list entry!\n");
                exit_critical_section();
                return E_NOMEM;
            }
            bzero(new_proc, sizeof(struct proc));
            char *cwd_copy = malloc_tagged(strlen(cwd) + 1, new_proc);
            strcpy(cwd_copy, cwd);
            if (!cwd_copy) {
                print("proc_new: failed to allocate cwd!\n");
                free_tag(new_proc);
                free(new_proc);
                exit_critical_section();
                return E_NOMEM;
            }
            void *stack = malloc_aligned_tagged(STACK_SIZE, PAGE_SIZE, new_proc);
            if (!stack) {
                print("proc_new: failed to allocate stack!\n");
                free_tag(new_proc);
                free(new_proc);
                exit_critical_section();
                return E_NOMEM;
            }
            bzero(stack, STACK_SIZE);
            void *exception_stack = malloc_aligned_tagged(STACK_SIZE, PAGE_SIZE, new_proc);
            if (!exception_stack) {
                print("proc_new: failed to allocate exception stack!\n");
                free_tag(new_proc);
                free(new_proc);
                exit_critical_section();
                return E_NOMEM;
            }
            bzero(exception_stack, STACK_SIZE);
            new_proc->input_buffer = input_buffer_new();
            if (!new_proc->input_buffer) {
                print("proc_new: failed to allocate input buffer!\n");
                free_tag(new_proc);
                free(new_proc);
                exit_critical_section();
                return E_NOMEM;
            }
            new_proc->state.sp = (uintptr_t)stack + STACK_SIZE;
            new_proc->exception_stack = (uintptr_t)exception_stack + STACK_SIZE;
            new_proc->state.cpsr = 0; // EL0
            new_proc->pid = i;
            new_proc->cwd = cwd_copy;
            proc_list[i] = new_proc;
            exit_critical_section();
            return new_proc->pid;
        }
    }
    exit_critical_section();
    return E_LIMIT;
}

// Create a new process from a function
int proc_new_func(uintptr_t pc) {
    int pid = proc_new(DEFAULT_CWD);
    if (pid >= 0) {
        struct proc *p = proc_list[pid];
        p->state.pc = pc;
    }
    return pid;
}

// Create a new process from an executable file
int proc_new_executable(const char *path, char **argp, char *cwd) {
    enter_critical_section();
    int err = 0;
    size_t argp_count = 0;
    char **argp_copy = NULL;
    int pid = proc_new(cwd);
    if (pid < 0) {
        err = pid;
        goto fail;
    }
    int fd = open(path, O_READ);
    if (fd < 0) {
        err = fd;
        goto fail;
    }
    ssize_t file_size = fsize(fd);
    if (file_size > 0xffffffff) {
        close(fd);
        err = E_OOB;
        goto fail;
    }
    void *executable_mem = malloc_aligned_tagged(file_size, PAGE_SIZE, proc_list[pid]);
    if (!executable_mem) {
        close(fd);
        err = E_OOB;
        goto fail;
    }
    ssize_t size_read = read(fd, executable_mem, file_size);
    if (size_read < 0) {
        close(fd);
        err = size_read;
        goto fail;
    }
    close(fd);
    if (size_read != file_size) {
        close(fd);
        err = E_IOERR;
        goto fail;
    }
    struct mentos_executable *exec = (struct mentos_executable *)executable_mem;
    if (exec->magic != EXECUTABLE_MAGIC) {
        err = E_FORMAT;
        goto fail;
    }
    uintptr_t executable_end = executable_mem + file_size;
    void *pc = executable_mem + exec->entry_offset;
    struct rela_entry *rela = executable_mem + exec->rela_start;
    struct rela_entry *rela_end = executable_mem + exec->rela_end;
    if ((uintptr_t)pc >= executable_end || (uintptr_t)rela >= executable_end
        || (uintptr_t)rela_end >= executable_end || rela_end < rela ||
        exec->bss_start >= executable_end || exec->bss_end >= executable_end ||
        exec->bss_end < exec->bss_start) {
        err = E_OOB;
        goto fail;
    }
    while (rela < rela_end) {
        switch (rela->type) {
            case R_AARCH64_RELATIVE:
                *(uint64_t *)(executable_mem + rela->off) = executable_mem + rela->addend;
                break;
            default:
                print("unknown relocation type %d\n", rela->type);
                break;
        }
        rela++;
    }
    bzero(executable_mem + exec->bss_start, exec->bss_end - exec->bss_start);
    argp_copy = malloc_tagged(MAX_ARGS * sizeof(char *), proc_list[pid]);
    if (!argp_copy) {
        err = E_NOMEM;
        goto fail;
    }
    while (argp[argp_count] && argp_count < MAX_ARGS) {
        char *str_copy = malloc_tagged(malloc(strlen(argp[argp_count]) + 1), proc_list[pid]);
        if (!str_copy) {
            err = E_NOMEM;
            goto fail;
        }
        strcpy(str_copy, argp[argp_count]);
        argp_copy[argp_count++] = str_copy;
    }
    argp_copy[argp_count] = NULL;
    char **argp_copy_realloc = realloc(argp_copy, argp_count * sizeof(char *));
    if (!argp_copy_realloc) {
        err = E_NOMEM;
        goto fail;
    }
    argp_copy = argp_copy_realloc;
    proc_list[pid]->state.pc = pc;
    proc_list[pid]->state.x[0] = argp_count;
    proc_list[pid]->state.x[1] = argp_copy;
    exit_critical_section();
    return pid;
fail:
    if (pid >= 0) {
        free_tag(proc_list[pid]);
        free(proc_list[pid]);
        proc_list[pid] = NULL;
    }
    exit_critical_section();
    return err;
}

// switch to proc `p` for `time`, then return
void proc_enter(int pid, unsigned int time) {
    current_proc = proc_list[pid];
    timer_irq_after(time);
    proc_state_drop(&(current_proc->state), current_proc->exception_stack);
}

// kill proc `pid` with `signal`
void proc_kill(unsigned int pid, int signal) {
    if (signal) {
        print("[%d] killed with signal %d\n", pid, signal);
    }
    struct proc *p = proc_list[pid];
    for (size_t i = 0; i < MAX_DESCRIPTORS; i++) {
        if (fds[i] != NULL) {
            if (fds[i]->proc == p) {
                if (close(i)) {
                    print("[%d] failed to close file descriptor %d\n", pid, i);
                } else {
                    print("[%d] kernel closed file %d\n", pid, i);
                }
            }
        }
        if (dirs[i] != NULL) {
            if (dirs[i]->proc == p) {
                if (closedir(i)) {
                    print("[%d] failed to close directory descriptor %d\n", pid, i);
                } else {
                    print("[%d] kernel closed directory %d\n", pid, i);
                }
            }
        }
    }
    // unblock any processes that are blocked by this (former) one
    for (int i = 0; i < MAX_PROC; i++) {
        if (proc_list[i] != NULL) {
            if (proc_list[i]->blocking_child == p) {
                proc_list[i]->blocking_child = NULL;
            }
        }
    }
    // TODO: this can race with the next line (proc_list[pid] = NULL) in proc_exit. fix this if and when we implement SMP.
    if (current_proc == p) {
        current_proc = NULL;
    }
    proc_list[pid] = NULL; // context switcher shall not switch anymore.
    // free memory
    input_buffer_free(p->input_buffer);
    free_tag(p);
    free(p);
}

// kill the current process and move to another one
void current_proc_kill(int signal) {
    disable_irqs();
    proc_kill(current_proc->pid, signal);
    proc_exit(NULL); // move on
}

// process tried doing an invalid syscall, kill it
void kill_invalid_syscall() {
    current_proc_kill(SIGSYS);
}

// We land here to handle an exception from a process.
// state - state of the process
void proc_exit(struct arm64_thread_state *state) {
    enter_critical_section();
    unsigned int pid;
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
                if (!proc_list[idx]->idle && !proc_list[idx]->blocking_child) {
                    exit_critical_section();
                    proc_enter(idx, PROC_TIME);
                }
            }
        }
        if (!has_procs) {
            panic("no processes running!");
        } else {
            proc_wait();
            proc_idle_release();
        }
    }
    panic("reached the end of proc_exit - we shouldn't be here");
}

// runs when we get a ctrl-d to stop the current process
void proc_sigint() {
    disable_irqs();
    if (current_proc != NULL) {
        if (!current_proc->sigint_blocked) {
            current_proc_kill(SIGINT);
        }
    }
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

int proc_chdir(char *path, struct proc *p) {
    enter_critical_section();
    int dd = opendir(path);
    if (dd < 0) {
        return dd;
    }
    closedir(dd);
    char *old_cwd = p->cwd;
    char *new_cwd = malloc(strlen(path) + 1);
    if (!new_cwd) {
        return E_NOMEM;
    }
    strcpy(new_cwd, path);
    p->cwd = new_cwd;
    free(old_cwd);
    exit_critical_section();
    return 0;
}
