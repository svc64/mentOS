#include "print.h"
#include "exceptions.h"
#include "time.h"
#include "uart.h"
#include "mem.h"
#include "test_user_proc.h"
#include "proc.h"

extern volatile unsigned char _end; // where our kernel image ends

void main() {
    init_uart();
    print("_end: 0x%x\n", &_end);
    print("mentOS\n");
    uint64_t current_el;
    __asm__ __volatile__("mrs %0, CurrentEL\n\t" : "=r" (current_el) :  : "memory");
    current_el = (current_el >> 2) & 0x3;
    print("Running in EL%d\n", current_el);

    // Set exception vectors
    __asm__ __volatile__("msr vbar_el1, %0\n\t" : : "r" (&exception_vectors) : "memory");
    // do something stupid, just to make an exception
    // totally random constant, btw
    //uint64_t *wut = (uint64_t *)0x5be3749d0211f909;
    //*(uint64_t *)wut = 0xc0885663c55641d8;
    print("main addr: 0x%x\n", &main);
    void *allocation_test = malloc(8192);
    print("malloc(8192): 0x%x\n", (uintptr_t)allocation_test);
    for (uintptr_t *p = (uintptr_t *)allocation_test; (uintptr_t)allocation_test < (uintptr_t)allocation_test + 8192; allocation_test += sizeof(uintptr_t)) {
        *p = 0x4141414141414141;
    }
    //enable_irqs();
    //init_timer();
    //timer_irq_after(200000);
    int pid = proc_new((uintptr_t)&test_proc_1);
    print("created pid %d\n", pid);
    move_to_proc(pid);
}
