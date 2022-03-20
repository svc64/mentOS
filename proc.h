#include <stdint.h>

#define STACK_SIZE  524288  // 512KB
#define PAGE_SIZE   4096
#define MAX_PROC    1024    // Maximum processes

struct __attribute__((__packed__)) arm64_thread_state {
    uintptr_t x[31];
    uintptr_t sp;
    uintptr_t pc;
};
struct __attribute__((__packed__)) proc {
    void *stack;
    struct arm64_thread_state state;
    unsigned int pid;
};

void proc_init();
int proc_new(uintptr_t pc);
void proc_enter(int pid, unsigned int time);
void proc_exit(struct arm64_thread_state *state);
void el0_drop(struct arm64_thread_state *);
