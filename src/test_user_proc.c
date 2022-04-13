#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "print.h"
#include "time.h"
#include "proc.h"
#include "syscalls/files.h"
// a test "userspace process"

void invalid_syscall();
void putc_syscall(char c);
uint32_t test_ret32();
uint64_t test_ret64();
int open(char *path, int mode);
size_t read(int fd, void *buf, size_t count);
int close(int fd);

int opendir(char *path);
int read_dir(int dir, struct dirent *ent);
int closedir(int dir);

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

int terrible = 0;

// Test automatic file/folder closing
void file_close_test() {
    if (terrible) {
        print("TERRIBLE NEWS!!!!\n");
    }
    terrible = 1;
    print("open() 0x%x\n", open("/test_dir/file.txt", O_READ));
    print("opendir: 0x%x\n", opendir("/"));
    print("crashing!\n");
    // crash
    invalid_syscall();
}

void test_proc_2() {
    print("test userspace proc 2\n");
    int dir = opendir("/test_dir");
    //print("dir = %d\n", dir);
    print("closing directory...\n");
    //print("closedir return value: %d\n", closedir(dir));
    while (1);
}

void test_proc_3() {
    print("test userspace proc 3\n");
    while (1);
}
