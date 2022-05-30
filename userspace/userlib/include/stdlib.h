#include <stddef.h>
__attribute__((noreturn)) void exit(int status);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
