#include "print.h"

// a test "userspace process"
void test_proc_1() {
    print("test userspace proc 1\n");
    while (1);
}

void test_proc_2() {
    print("test userspace proc 2\n");
    while (1);
}
