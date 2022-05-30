#include <stddef.h>
#include <stdbool.h>

// Private allocator-related stuff (struct metadata, etc)...
extern void *heap;
struct metadata {
    uint8_t free;
    size_t size;
    void *tag;
    struct metadata *next;
};
extern struct metadata *first_alloc;
bool allocation_exists(void *ptr);
bool allocation_exists_tag(void *ptr, void *tag);
