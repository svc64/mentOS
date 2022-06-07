#include <stddef.h>
__attribute__((noreturn)) void exit(int status);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *memcpy(void *s, void *d, size_t n);
void bzero(void *s, size_t n);
int memcmp(void *s, void *d, size_t n);
void *memset(void *s, int value, size_t n);
char *strchr(const char *s, int c);
size_t strlen(const char *str);
char *strcpy(char *dst, const char *src);
char *strcat(char *dst, const char *src);
int strcmp(const char *s1, const char *s2);
int atoi(char *s);
