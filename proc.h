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
    unsigned int pid;
    uintptr_t stack;
    struct arm64_thread_state state;
};

void proc_init();
int proc_new(uintptr_t pc);
void move_to_proc(int pid);
void el0_drop(struct arm64_thread_state *);
