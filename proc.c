#include <stddef.h>
#include "print.h"
#include "proc.h"
#include "mem.h"
#include "time.h"
#include "exceptions.h"

struct proc **proc_list = NULL;
struct proc *current_proc = NULL;

void proc_init() {
    if (!proc_list) {
        size_t proc_list_size = MAX_PROC * sizeof(struct proc *);
        proc_list = malloc(proc_list_size);
        bzero(proc_list, proc_list_size);
    }
}

// once we support executables, maybe instead of providing pc
// we'll provide an executable address? idk, we'll see.
int proc_new(uintptr_t pc) {
    for (int i = 0; i < MAX_PROC; i++) {
        if (proc_list[i] == NULL) {
            proc_list[i] = malloc(sizeof(struct proc));
            bzero(proc_list[i], sizeof(struct proc));
            proc_list[i]->pid = i;
            proc_list[i]->stack = malloc(PAGE_SIZE + STACK_SIZE);
            bzero(proc_list[i]->stack, PAGE_SIZE + STACK_SIZE);
            proc_list[i]->state.pc = pc;
            proc_list[i]->state.sp = (((uintptr_t)proc_list[i]->stack + PAGE_SIZE) & -PAGE_SIZE) + STACK_SIZE;
            return proc_list[i]->pid;
            break;
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

// We land here to handle an exception from a process.
void proc_exit(struct arm64_thread_state *state, int exc) {
    timer_irq_handled();
    print("switch state: \n");
    for (int i = 0; i < 31; i++) {
        print("x%d = 0x%x\n", i, state->x[i]);
    }
    print("sp: 0x%x\n", state->sp);
    print("pc: 0x%x\n", state->pc);
    current_proc->state = *state;
    for (int i = current_proc->pid + 1; i < MAX_PROC * 2; i++) {
        int idx = i % MAX_PROC;
        if (proc_list[idx] != NULL) {
            current_proc = proc_list[idx];
            proc_enter(current_proc->pid, 200000);
        }
    }
    // if we get here. init is killed. terrible news.
    panic("init proc killed!");
}
