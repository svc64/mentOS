#include <stdint.h>
#include <stdbool.h>
#include "fatfs/ff.h"

#define STACK_SIZE  524288  // 512KB
#define MAX_PROC    1024    // Maximum processes
#define PROC_TIME   50000  // The time we give to processes until we switch (for now).
struct __attribute__((__packed__)) arm64_thread_state {
    __uint128_t q[32];
    uintptr_t cpsr;
    uintptr_t sp;
    uintptr_t pc;
    uintptr_t x[31];
};
struct proc {
    void *exception_stack;
    struct arm64_thread_state state;
    struct input_buffer *input_buffer;
    struct proc *blocking_child;
    char *cwd;
    unsigned int pid;
    bool idle;
    bool sigint_blocked;
};

#define EXECUTABLE_MAGIC 0x73746e6d
struct __attribute__((__packed__)) mentos_executable {
    uint32_t magic; // Executable magic
    uint32_t entry_offset; // Entry point offset
    uint32_t rela_start;
    uint32_t rela_end;
};

#define R_AARCH64_RELATIVE 1027
struct rela_entry {
    uint64_t off, type, addend;
};

void proc_init();
int proc_new_executable(const char *path);
int proc_new_func(uintptr_t pc);
void proc_enter(int pid, unsigned int time);
void proc_kill(unsigned int pid, unsigned int signal);
void current_proc_kill(int signal);
void proc_exit(struct arm64_thread_state *state);
void proc_state_drop(struct arm64_thread_state *, void *exception_stack);
void enter_critical_section();
void exit_critical_section();
void proc_wait();
void proc_next();
void proc_idle();
void proc_idle_release();
int proc_chdir(char *path, struct proc *p);
void proc_sigint();

// the current process
extern struct proc **proc_list;
extern struct proc *current_proc;
extern struct proc *front_proc;
