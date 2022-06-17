#include <stdint.h>
#include <stdbool.h>
#include "exceptions.h"
#include "alloc_private.h"
#include "alloc.h"
#include "print.h"
#include "proc.h"
#include "mmio.h"
#include "stdlib.h"
#include "defs.h"
extern volatile unsigned char _end; // where our kernel image ends
#define HEAP_END    MMIO_BASE
void *heap = (void *)(&_end); // actual heap with actual data
struct metadata *first_alloc = NULL;
// size: the size of the allocation
// alignment: allocation alignment
// tag: a pointer to "tag" this allocation with. we can later free all allocations with this tag.
void *malloc_aligned_tagged(size_t size, size_t alignment, void *tag) {
    enter_critical_section();
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
            md->free = false;
            md->size = size;
            md->tag = tag;
            md->next = NULL;
            first_alloc = md;
            exit_critical_section();
            return (void *)((uintptr_t)md + sizeof(struct metadata));
        }
        exit_critical_section();
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
            md->free = false;
            md->size = size;
            md->tag = tag;
            md->next = first_alloc;
            first_alloc = md;
            exit_critical_section();
            return (void *)data_ptr;
        }
    }
    // walk over all allocations
    struct metadata *md = first_alloc;
    struct metadata *prev_md = NULL;
    while (md != NULL) {
        // detect a hole if we have one
        if (md->next != NULL) {
            uintptr_t maybe_next_md = (uintptr_t)md + sizeof(struct metadata) + md->size;
            if (maybe_next_md < (uintptr_t)md->next) {
                // we have a hole, great. does our size fit?
                ssize_t hole_size = (uintptr_t)md->next - maybe_next_md;
                if (hole_size >= (ssize_t)(size + sizeof(struct metadata))) {
                    // there are 'hole_size' bytes between maybe_next and md->next, maybe we can allocate there.
                    // check alignment
                    uintptr_t md_ptr = maybe_next_md;
                    uintptr_t data_ptr = md_ptr + sizeof(struct metadata);
                    if (alignment > 1 && (data_ptr & -alignment) != data_ptr) {
                        data_ptr = (data_ptr + alignment) & -alignment;
                        md_ptr = data_ptr - sizeof(struct metadata);
                    }
                    // size check
                    hole_size = (uintptr_t)md->next - md_ptr;
                    if (hole_size >= (ssize_t)(size + sizeof(struct metadata))) {
                        struct metadata *new_md = (struct metadata *)(md_ptr);
                        new_md->size = size;
                        new_md->tag = tag;
                        new_md->next = md->next;
                        new_md->free = false;
                        md->next = new_md;
                        exit_critical_section();
                        return (void *)((uintptr_t)new_md + sizeof(struct metadata));
                    }
                }
            } else if (maybe_next_md > (uintptr_t)md->next) {
                panic("memory allocated on heap metadata, that's bad...\n");
            }
        }
        // no hole.
        prev_md = md;
        md = md->next;
    }
    // alright, there are no holes.
    uintptr_t new_md_ptr = (uintptr_t)prev_md + sizeof(struct metadata) + prev_md->size;
    uint64_t data_ptr = new_md_ptr + sizeof(struct metadata);
    if (alignment > 1 && (data_ptr & -alignment) != data_ptr) {
        data_ptr = (data_ptr + alignment) & -alignment;
        new_md_ptr = data_ptr - sizeof(struct metadata);
    }
    if ((uintptr_t)data_ptr + size > HEAP_END) {
        // no space
        exit_critical_section();
        return NULL;
    }
    struct metadata *new_md = (struct metadata *)(new_md_ptr);
    new_md->size = size;
    new_md->tag = tag;
    new_md->next = NULL;
    new_md->free = false;
    prev_md->next = new_md;
    exit_critical_section();
    return (uintptr_t)new_md + sizeof(struct metadata);
}

void *malloc_aligned(size_t size, size_t alignment) {
    return malloc_aligned_tagged(size, alignment, NULL);
}

void *malloc_tagged(size_t size, void *tag) {
    return malloc_aligned_tagged(size, 0, tag);
}

void *malloc(size_t size) {
    return malloc_aligned_tagged(size, 0, NULL);
}

void free(void *mem) {
    enter_critical_section();
    struct metadata *md = (struct metadata *)((uintptr_t)mem - sizeof(struct metadata));
    /* if md is not at the start of the heap, there is a previous md that links to it.
    we should change that previous md to link to md->next */
    if (md == first_alloc) {
        first_alloc = first_alloc->next;
    } else {
        struct metadata *prev_md = first_alloc;
        while (prev_md->next != md) {
            prev_md = prev_md->next;
        }
        prev_md->next = md->next;
    }
    md->free = true;
    exit_critical_section();
}

// free all allocations with tag `tag`
void free_tag(void *tag) {
    if (!tag) {
        panic("free_tag: tag is NULL");
    }
    enter_critical_section();
    struct metadata *md = first_alloc;
    struct metadata *prev_md = NULL;
    while (md) {
        if (md->tag == tag) {
            free((void *)((uintptr_t)md + sizeof(struct metadata)));
            if (!prev_md) {
                prev_md = first_alloc;
            }
            md = prev_md;
        } else {
            prev_md = md;
            md = md->next;
        }
    }
    exit_critical_section();
}

void *realloc(void *ptr, size_t size) {
    enter_critical_section();
    if (ptr == NULL) {
        return malloc(size);
    }
    struct metadata *md = (struct metadata *)((uintptr_t)ptr - sizeof(struct metadata));
    if (md->free) {
        panic("attempting to reallocate free memory!");
    }
    if (!size) {
        // free
        free(ptr);
        exit_critical_section();
        return NULL;
    }
    if (size == md->size) {
        exit_critical_section();
        return ptr;
    }
    if (size < md->size) {
        md->size = size;
        exit_critical_section();
        return ptr;
    }
    size_t expandable_size = md->size; // size of free memory after the allocation, the size that we can expand the allocation to
    if (md->next) {
        // we should never actually have a situation where the next allocations are marked as free
        expandable_size = (uintptr_t)md->next - (uintptr_t)ptr;
    } else {
        expandable_size = HEAP_END - (uintptr_t)ptr;
    }
    if (expandable_size >= size) {
        // great! we can just modify the size
        md->size = size;
    } else {
        // re-allocate the memory at a new address
        void *new_alloc = malloc_tagged(size, md->tag);
        if (new_alloc == NULL) {
            exit_critical_section();
            return NULL;
        }
        memcpy(new_alloc, ptr, md->size);
        free(ptr);
        exit_critical_section();
        return new_alloc;
    }
    exit_critical_section();
    return ptr;
}

// check if an allocation exists
bool allocation_exists(void *ptr) {
    enter_critical_section();
    struct metadata *current_md = first_alloc;
    while (current_md != NULL) {
        if ((uintptr_t)current_md + sizeof(struct metadata) == (uintptr_t)ptr) {
            exit_critical_section();
            return true;
        }
        current_md = current_md->next;
    }
    exit_critical_section();
    return false;
}

// check if an allocation exists and is tagged by `tag`
bool allocation_exists_tag(void *ptr, void *tag) {
    enter_critical_section();
    if (allocation_exists(ptr)) {
        struct metadata *md = (struct metadata *)((uintptr_t)ptr - sizeof(struct metadata));
        if (md->tag == tag) {
            exit_critical_section();
            return true;
        }
    }
    exit_critical_section();
    return false;
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
