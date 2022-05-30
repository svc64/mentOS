#include <stddef.h>

// Private allocator-related stuff (struct metadata, etc)...
extern void *heap;
struct metadata {
    uint8_t free;
    size_t size;
    void *tag;
    struct metadata *next;
};
extern struct metadata *first_alloc;
