#include "print.h"
#include "time.h"
// a test "userspace process"

void invalid_syscall();
void putc_syscall(char c);
uint32_t test_ret32();
uint64_t test_ret64();

void test_proc_1() {
    putc_syscall('y');
    putc_syscall('\n');
    uint32_t ret32 = test_ret32();
    uint64_t ret64 = test_ret64();
    print("ret32: 0x%x\n", ret32);
    print("ret64: 0x%x\n", ret64);
    while (ret32 == 0x41414141 && ret64 == 0x4141414141414141) {
        invalid_syscall();
    }
}

void test_proc_2() {
    print("test userspace proc 2\n");
    while (1);
}

void test_proc_3() {
    print("test userspace proc 3\n");
    while (1);
}
