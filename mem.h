#include <stddef.h>

void *malloc(size_t size);
void free(void *mem);
void bzero(void *s, size_t n);
int memcmp(void *s, void *d, size_t n);
void *memset(void *s, int value, size_t n);
char *strchr(const char *s, int c);
void *memcpy(void *s, void *d, size_t n);
