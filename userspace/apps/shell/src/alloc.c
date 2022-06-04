#include <io.h>
#include "alloc.h"

void *malloc_noerror(size_t size) {
    void *ret = malloc(size);
    if (!ret) {
        print("allocation of size %d failed!\n", size);
        exit(1);
    }
    return ret;
}

void *realloc_noerror(void *ptr, size_t size) {
    void *ret = realloc(ptr, size);
    if (!ret) {
        print("reallocation to size %d failed!\n", size);
        exit(1);
    }
    return ret;
}
