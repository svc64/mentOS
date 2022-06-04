#include "exceptions.h"
#include "proc.h"
#include "alloc.h"
#include "alloc_private.h"
#include "signal.h"
#include "print.h"

void *malloc_syscall(size_t size) {
    if (!current_proc) {
        panic("malloc syscall called when current_proc is NULL");
    }
    void *ret = malloc_tagged(size, current_proc);
    return ret;
}

void free_syscall(void *ptr) {
    if (!current_proc) {
        panic("free syscall called when current_proc is NULL");
    }
    if (allocation_exists_tag(ptr, current_proc)) {
        free(ptr);
    } else {
        print("oops attempted to free nonexistant thingy: 0x%x\n", ptr);
        current_proc_kill(SIGSEGV);
        panic("free syscall: current_proc_kill returned");
    }
}

void *realloc_syscall(void *ptr, size_t size) {
    if (!current_proc) {
        panic("realloc syscall called when current_proc is NULL");
    }
    if (ptr) {
        if (!allocation_exists_tag(ptr, current_proc)) {
            current_proc_kill(SIGSEGV);
            panic("realloc syscall: current_proc_kill returned");
            return NULL;
        }
    }
    return realloc(ptr, size);
}
