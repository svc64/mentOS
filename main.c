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
    init_timer();
    // initialize the proc struct list
    proc_init();
    disable_irqs();
    print("mentOS\n-------\n");
    print("main addr: 0x%x\n", &main);
    print("_end: 0x%x\n", &_end);
    uint64_t current_el;
    __asm__ __volatile__("mrs %0, CurrentEL\n\t" : "=r" (current_el) :  : "memory");
    current_el = (current_el >> 2) & 0x3;
    print("Running in EL%d\n", current_el);
    // Set exception vectors
    __asm__ __volatile__("msr vbar_el1, %0\n\t; isb" : : "r" (&exception_vectors) : "memory");
    // do something stupid, just to make an exception
    // totally random constant, btw
    //uint64_t *wut = (uint64_t *)0x5be3749d0211f909;
    //*(uint64_t *)wut = 0xc0885663c55641d8;
    // create a new process
    int pid0 = proc_new((uintptr_t)&test_proc_1);
    print("created pid %d\n", pid0);
    // create another
    int pid1 = proc_new((uintptr_t)&test_proc_2);
    print("created pid %d\n", pid1);
    int pid2 = proc_new((uintptr_t)&test_proc_3);
    print("created pid %d\n", pid2);
    // move to it
    proc_enter(pid0, PROC_TIME);
    while (1);
}
