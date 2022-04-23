#include <stddef.h>

#define PAGE_SIZE   4096

void *malloc_aligned(size_t size, size_t alignment);
void *malloc(size_t size);
void free(void *mem);
void *bump_page();
