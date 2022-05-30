#include <stddef.h>

#define PAGE_SIZE   4096

void *malloc_aligned_tagged(size_t size, size_t alignment, void *tag);
void *malloc_aligned(size_t size, size_t alignment);
void *malloc_tagged(size_t size, void *tag);
void *malloc(size_t size);
void free_tag(void *tag);
void free(void *mem);
void *realloc(void *ptr, size_t size);
void *bump_page();
