#include <stddef.h>
#include "print.h"
#include "proc.h"
#include "mem.h"

struct proc **proc_list = NULL;

void proc_init() {
    if (!proc_list) {
        size_t proc_list_size = MAX_PROC * sizeof(struct proc *);
        proc_list = malloc(proc_list_size);
        bzero(proc_list, proc_list_size);
    }
}

// we get here when we return from a proc
void proc_return(struct arm64_thread_state *state) {
    print("returned from pc 0x%x\n", state->pc);
    while (1);
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

// switch to proc `p`
void move_to_proc(int pid) {
    el0_drop(&(proc_list[pid]->state));
}
