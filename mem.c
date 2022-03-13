#include <stdint.h>
#include "exceptions.h"
#include "mem.h"
#include "print.h"
#include "mmio.h"
extern volatile unsigned char _end; // where our kernel image ends
#define HEAP_START  ((uintptr_t)(&_end))
#define HEAP_END    MMIO_BASE
#define ALIGNMENT   16 // align allocations by 8 bytes
void *heap = (void *)HEAP_START; // actual heap with actual data
struct metadata {
    size_t size;
    struct metadata *next;
    uint8_t free;
};
static uint8_t first = 1;
void *malloc(size_t size) {
    // it's the first allocation, there's nothing to walk over.
    if (first) {
        uintptr_t md_ptr = (uintptr_t)heap;
        uint64_t data_ptr = md_ptr + sizeof(struct metadata);
        if ((data_ptr & -ALIGNMENT) != data_ptr) {
            data_ptr = (data_ptr + ALIGNMENT) & -ALIGNMENT;
            md_ptr = data_ptr - sizeof(struct metadata);
            heap = (void *)md_ptr;
        }
        struct metadata *md = (struct metadata *)md_ptr;
        md->free = 0;
        md->size = size;
        md->next = NULL;
        first = 0;
        return (void *)((uintptr_t)md + sizeof(struct metadata));
    }
    // walk over all allocations
    struct metadata *md = heap;
    struct metadata *prev_md = NULL;
    while (md != NULL) {
        prev_md = md;
        // check if it's free'd
        if (md->free) {
            /* we have a free'd allocation. does our size fit?
            if md is the first AND it's free'd AND has no next md, it means we have no allocations
            and can allocate there */
            uint8_t no_allocations = (md == heap && md->next == NULL);
            if (no_allocations && (uintptr_t)heap + sizeof(struct metadata) + size > HEAP_END) {
                // no space
                return NULL;
            }
            if (size <= md->size || no_allocations) {
                // it's free real estate
                md->size = size;
                md->free = 0;
                return (void *)((uintptr_t)md + sizeof(struct metadata));
            }
        }
        // detect a hole if we have one
        if (md->next != NULL) {
            uintptr_t maybe_next = (uintptr_t)md + sizeof(struct metadata) + md->size;
            if (maybe_next < (uintptr_t)md->next) {
                // we have a hole, great. does our size fit?
                size_t hole_size = (uintptr_t)md->next - maybe_next;
                if (hole_size >= size) {
                    // there are 'hole_size' bytes between maybe_next and md->next, maybe we can allocate there.
                    // check alignment
                    uintptr_t md_ptr = maybe_next;
                    uintptr_t data_ptr = md_ptr + sizeof(struct metadata);
                    if ((data_ptr & -ALIGNMENT) != data_ptr) {
                        data_ptr = (data_ptr + ALIGNMENT) & -ALIGNMENT;
                        md_ptr = data_ptr - sizeof(struct metadata);
                    }
                    // size check
                    hole_size = (uintptr_t)md->next - md_ptr;
                    if (hole_size >= size) {
                        struct metadata *new_md = (struct metadata *)(md_ptr);
                        new_md->size = size;
                        new_md->next = md->next;
                        new_md->free = 0;
                        md->next = new_md;
                        return (void *)((uintptr_t)new_md + sizeof(struct metadata));
                    }
                }
            } else if (maybe_next > (uintptr_t)md->next) {
                //panic("memory allocated on heap metadata, that's bad...\n");
                return NULL; // panic'd already, should never run
            }
        }
        // no hole.
        md = md->next;
    }
    // alright, there are no holes.
    uintptr_t new_md_ptr = (uintptr_t)prev_md + sizeof(struct metadata) + prev_md->size;
    uint64_t data_ptr = new_md_ptr + sizeof(struct metadata);
    if ((data_ptr & -ALIGNMENT) != data_ptr) {
        data_ptr = (data_ptr + ALIGNMENT) & -ALIGNMENT;
        new_md_ptr = data_ptr - sizeof(struct metadata);
    }
    struct metadata *new_md = (struct metadata *)(new_md_ptr);
    if ((uintptr_t)new_md + sizeof(struct metadata) + size > HEAP_END) {
        // no space
        return NULL;
    }
    new_md->free = 0;
    new_md->size = size;
    new_md->next = NULL;
    prev_md->next = new_md;
    uintptr_t retval = (uintptr_t)new_md + sizeof(struct metadata);
    return (struct metadata *)(retval);
}

void free(void *mem) {
    struct metadata *md = (struct metadata *)((uintptr_t)mem - sizeof(struct metadata));
    // if md is not at the start of the heap, there is a previous md that links to it.
    // we should change that previous md to link to md->next
    if (md != heap) {
        struct metadata *prev_md = heap;
        while (prev_md->next != md) {
            prev_md = prev_md->next;
        }
        prev_md->next = md->next;
    }
    md->free = 1;
}

void bzero(void *s, size_t n) {
    size_t blocks_64 = n / sizeof(uint64_t);
    size_t blocks_32 = n % sizeof(uint64_t) / sizeof(uint32_t);
    size_t blocks_16 = n % sizeof(uint64_t) % sizeof(uint32_t) / sizeof(uint16_t);
    size_t blocks_8 = n % sizeof(uint64_t) % sizeof(uint32_t) % sizeof(uint16_t) / sizeof(uint8_t);
    for (size_t i = 0; i < blocks_64; i++) {
        *((uint64_t *)s) = 0;
        s += sizeof(uint64_t);
    }
    for (size_t i = 0; i < blocks_32; i++) {
        *((uint32_t *)s) = 0;
        s += sizeof(uint32_t);
    }
    for (size_t i = 0; i < blocks_16; i++) {
        *((uint16_t *)s) = 0;
        s += sizeof(uint16_t);
    }
    for (size_t i = 0; i < blocks_8; i++) {
        *((uint8_t *)s) = 0;
        s += sizeof(uint8_t);
    }
}
