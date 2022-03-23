#include "print.h"
#include "time.h"
// a test "userspace process"

void do_some_syscall();

void test_proc_1() {
    while (1)
    {
        delay(10000);
        print("test userspace proc 1\n");
    }
}

void test_proc_2() {
    do_some_syscall();
    while (1)
    {
        delay(10000);
        print("test userspace proc 2\n");
    }
    while (1);
}

void test_proc_3() {
    while (1)
    {
        delay(10000);
        print("test userspace proc 3\n");
    }
    
    while (1);
}
