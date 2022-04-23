#include <stdint.h>
#include "exceptions.h"
#include "mem.h"
#include "print.h"
#include "mmio.h"
extern volatile unsigned char _end; // where our kernel image ends
#define HEAP_END    MMIO_BASE
void *heap = (void *)(&_end); // actual heap with actual data
struct metadata {
    size_t size;
    struct metadata *next;
    uint8_t free;
};
struct metadata *first_alloc = NULL;
void *malloc_aligned(size_t size, size_t alignment) {
    // it's the first allocation, there's nothing to walk over.
    if (first_alloc == NULL) {
        uintptr_t md_ptr = (uintptr_t)heap;
        uint64_t data_ptr = md_ptr + sizeof(struct metadata);
        if (alignment > 1 && (data_ptr & -alignment) != data_ptr) {
            data_ptr = (data_ptr + alignment) & -alignment;
            md_ptr = data_ptr - sizeof(struct metadata);
        }
        if ((data_ptr + size) <= HEAP_END) {
            struct metadata *md = (struct metadata *)md_ptr;
            md->free = 0;
            md->size = size;
            md->next = NULL;
            first_alloc = md;
            return (void *)((uintptr_t)md + sizeof(struct metadata));
        }
        return NULL; // heap too small
    }
    /* the first allocation is after the heap starts
     see if we can allocate in this hole between the heap
     and the first allocation */
    if ((uintptr_t)first_alloc > (uintptr_t)heap) {
        uintptr_t md_ptr = (uintptr_t)heap;
        uintptr_t data_ptr = md_ptr + sizeof(struct metadata);
        if (alignment > 1 && (data_ptr & -alignment) != data_ptr) {
            data_ptr = (data_ptr + alignment) & -alignment;
            md_ptr = data_ptr - sizeof(struct metadata);
        }
        if ((data_ptr + size) <= (uintptr_t)first_alloc) {
            // we have space!
            struct metadata *md = (struct metadata *)md_ptr;
            md->free = 0;
            md->size = size;
            md->next = first_alloc;
            first_alloc = md;
            return (void *)data_ptr;
        }
    }
    // walk over all allocations
    struct metadata *md = first_alloc;
    struct metadata *prev_md = NULL;
    while (md != NULL) {
        prev_md = md;
        // check if it's free'd
        if (md->free) {
            /* we have a free'd allocation. does our size fit?
            if md is the first AND it's free'd AND has no next md, it means we have no allocations
            and can allocate there */
            uint8_t no_allocations = (md == first_alloc && md->next == NULL);
            if (no_allocations && (uintptr_t)first_alloc + sizeof(struct metadata) + size > HEAP_END) {
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
                    if (alignment > 1 && (data_ptr & -alignment) != data_ptr) {
                        data_ptr = (data_ptr + alignment) & -alignment;
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
                panic("memory allocated on heap metadata, that's bad...\n");
            }
        }
        // no hole.
        md = md->next;
    }
    // alright, there are no holes.
    uintptr_t new_md_ptr = (uintptr_t)prev_md + sizeof(struct metadata) + prev_md->size;
    uint64_t data_ptr = new_md_ptr + sizeof(struct metadata);
    if (alignment > 1 && (data_ptr & -alignment) != data_ptr) {
        data_ptr = (data_ptr + alignment) & -alignment;
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

void *malloc(size_t size) {
    return malloc_aligned(size, 0);
}

void free(void *mem) {
    struct metadata *md = (struct metadata *)((uintptr_t)mem - sizeof(struct metadata));
    // if md is not at the start of the heap, there is a previous md that links to it.
    // we should change that previous md to link to md->next
    if (md != first_alloc) {
        struct metadata *prev_md = first_alloc;
        while (prev_md->next != md) {
            prev_md = prev_md->next;
        }
        prev_md->next = md->next;
    }
    md->free = 1;
}

// Get a memory page.
/* The MMU mapping code needs page-aligned allocations to work.
The MMU code also runs before we allocate anything using malloc.
So, this function bumps heap start (heap += PAGE_SIZE) and returns
the old value of the heap start to "allocate" a page for us.
WARNING: This function panics if we ever used malloc. It's meant to be
used only in early stages when we set up the MMU. */
void *bump_page() {
    if (first_alloc != NULL) {
        panic("bump_page() called after the heap was used.");
    }
    void *retval = heap;
    heap = (void *)((uintptr_t)heap + PAGE_SIZE);
    return retval;
}
