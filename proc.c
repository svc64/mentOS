#include <stddef.h>
#include "print.h"
#include "proc.h"
#include "malloc.h"

struct proc **proc_list = NULL;

void proc_init() {
    if (!proc_list) {
        proc_list = malloc(MAX_PROC * sizeof(struct proc *));
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
            proc_list[i]->pid = i;
            proc_list[i]->stack = (uintptr_t)malloc(PAGE_SIZE + STACK_SIZE);
            proc_list[i]->state.pc = pc;
            proc_list[i]->state.sp = proc_list[i]->stack & -PAGE_SIZE;
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
