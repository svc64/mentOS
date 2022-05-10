#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include "sysreg.h"
#include "stdlib.h"
#include "mem.h"
#include "print.h"
#include "mmio.h"
#include "exceptions.h"
#include "mmu.h"
/*
The BCM2837 CPU has a bug(?) regarding memory alignment.
A quick explainer: What is memory alignment?
Let's say we try reading 64 bits (8 bytes) from 0xB8018. The CPU takes some cycles for this read (sometimes 1 cycle, sometimes 2 cycles
because it's a memory accesss. depends on the CPU).
The CPU accesses memory like it's an array of 32/64 bit words. So, to read from 0xB8018 it accesses the 94211th (0xB8018 / 8) element of the array.
Now, if we try reading 64 bits from 0xB8019 the story is different.
The CPU tries to read the... 94211.125th element of the array...?
Yeah, no. The CPU reads 2 elements instead and uses them to form what should be in the address.
This takes more cycles, and harms performance.
However, it's not practical for us to write a program that does *absolutely no* unaligned memory access.

The A bit (bit 1) in SCTLR_EL1 enables memory alignment checking.
Every access to unaligned memory will generate an exception when this bit is enabled.
When this bit is disabled, access to unaligned memory is permitted so no exception should
be generated.
Right?
Nah. On the BCM2837, access to unaligned memory will still generate an exception.
...unless we enable the MMU.
The engineers at Broadcom were smoking some real good stuff... But we do what we gotta do,
so we're enabling the MMU and mapping physical memory into the same layout as it is already.
All addresses will be the same, this is just to enable the MMU and have unalinged memory access.
*/

extern volatile unsigned char _data;
extern volatile unsigned char _end;


// TODO: come on, we need to figure these out ourselves.
int address_length;
int upper_addr_bits;
int level_bits;
int levels_count;
// constants for map_region
int entries_per_level;
size_t max_L_size;

#define BITS(___src, ___start, ___end) (((1 << ((___end) - (___start))) - 1) & ((___src) >> ((___start) - 1)))
#define ___extract_L_bits(___addr, ___start) BITS((___addr), (___start), (___start) + (level_bits))
#define L_index(___level, ___addr) ___extract_L_bits((___addr), ((address_length - upper_addr_bits) + ((levels_count - (___level)) * level_bits) + 1))
#define strip_table_entry(___entry) ((((___entry) << (64 - address_length)) >> (64 - address_length)) & -PAGE_SIZE)

// bump_page() and bzero
void *mmu_table_alloc() {
    void *bumped = bump_page();
    bzero(bumped, PAGE_SIZE);
    return bumped;
}

// get size of `n` in bits
unsigned int size_in_bits(uintptr_t n) {
   unsigned int count = 0;
   while (n) {
        count++;
        n >>= 1;
    }
    return count;
}

void map_region(uintptr_t *first_level_table, uintptr_t virt, uintptr_t phys_start, uintptr_t phys_end, uintptr_t block_attribs) {
    uintptr_t current_virt = virt &~ ((0xffffffffffffffff >> address_length) << address_length);
    uintptr_t current_phys = phys_start;
    uintptr_t virt_end = phys_end - phys_start + virt;
    while (current_virt < virt_end) {
        int current_level = 1;
        for (size_t L_size = max_L_size; L_size >= PAGE_SIZE; L_size /= entries_per_level) {
            if (!(current_virt % L_size) && ((virt_end - current_virt) / L_size)) {
                /* make an array of level indexes for this address
                levels[0] is L1 index, levels[1] is L2 index and so on... */
                int levels[current_level];
                for (int i = current_level - 1; i >= 0; i--) {
                    uintptr_t li = L_index(i + 1, current_virt);
                    levels[i] = li;
                }
                uintptr_t *L = first_level_table;
                /* This loop walks over the entire table and gets us
                to the level where we need to place our mapping.
                It also allocates new pages for new L2/L3/etc. */
                for (int i = 0; i < current_level - 1; i++) {
                    if (L[levels[i]] == 0) {
                        uintptr_t allocated = (uintptr_t)mmu_table_alloc();
                        uintptr_t entry = allocated | PT_PAGE | PT_AF | PT_ISH | PT_MEM | (block_attribs & PT_KERNEL ? PT_KERNEL : PT_USER);
                        L[levels[i]] = entry;
                    }
                    L = strip_table_entry(L[levels[i]]);
                }
                uintptr_t entry = current_phys | block_attribs;
                if (current_level != levels_count) {
                    entry |= PT_BLOCK;
                } else {
                    entry |= PT_PAGE;
                }
                int index = L_index(current_level, current_virt);
                L[index] = entry;
                current_virt += L_size;
                current_phys += L_size;
                break;
            }
            current_level++;
        }
    }    
}
void init_mmu() {
    //uint64_t id_aa64mmfr0_el1;
    //asm volatile ("mrs %0, id_aa64mmfr0_el1" : "=r" (id_aa64mmfr0_el1));
    // The granule size we actually use is defined by PAGE_SIZE
    // Address length
    address_length = 36;
    // Page offset size
    int offset_size = size_in_bits(PAGE_SIZE - 1);
    // Size of the upper address, without the page offset
    upper_addr_bits = address_length - offset_size;
    // Size of each translation level in the address
    level_bits = size_in_bits((PAGE_SIZE / 8) - 1);
    // Calculate the number of levels (3 or 4)
    levels_count = upper_addr_bits / level_bits + (upper_addr_bits % level_bits ? 1 : 0);
    /* calculate the number of entries per level and the size of the block
    that the first level can point to */
    entries_per_level = PAGE_SIZE / 8;
    max_L_size = PAGE_SIZE;
    for (int i = 0; i < levels_count - 1; i++) {
        max_L_size *= entries_per_level;
    }
    uintptr_t mem_start = 0x0;
    uintptr_t code_start = 0x80000;
    uintptr_t code_end = ((uintptr_t)(&_data));
    uintptr_t mem_end = 0x40000000;
    uintptr_t *first_level_table = mmu_table_alloc();
    map_region(first_level_table, mem_start, mem_start, code_start, PT_AF | PT_USER | PT_ISH | PT_MEM | PT_RW);
    map_region(first_level_table, code_start, code_start, code_end, PT_AF | PT_USER | PT_ISH | PT_MEM | PT_RO);
    map_region(first_level_table, code_end, code_end, MMIO_BASE, PT_AF | PT_USER | PT_ISH | PT_MEM | PT_RW);
    map_region(first_level_table, MMIO_BASE, MMIO_BASE, mem_end, PT_AF | PT_USER | PT_OSH | PT_DEV | PT_RW);
    // Set TTBR0_EL1 to where our translation tables are
    asm volatile ("msr ttbr0_el1, %0" : : "r" ((uintptr_t)first_level_table + TTBR_CNP));
    // Set MAIR with our memory attributes indexed in the right places
    uint64_t reg = MAIR_ATTRIDX(MAIR_ATTR_NORMAL, PT_MEM) |
                    MAIR_ATTRIDX(MAIR_ATTR_DEVICE_nGnRE, PT_DEV) |
                    MAIR_ATTRIDX(MAIR_ATTR_NORMAL_NC, PT_NC);
    asm volatile ("msr mair_el1, %0" : : "r" (reg));
    // Configure TCR_EL1
    reg = TCR_IPS(PARange_36) | TCR_TG1_4K | TCR_SH1_INNER | TCR_ORGN1_WBWA
    | TCR_TG0_4K | TCR_SH0_INNER | TCR_ORGN0_WBWA | TCR_EPD1_MASK | TCR_TxSZ(address_length);
    asm volatile ("msr tcr_el1, %0; isb" : : "r" (reg));
    // Enable the MMU
    asm volatile ("dsb ish; isb; mrs %0, sctlr_el1" : "=r" (reg));
    reg &= ~SCTLR_ELx_A; // Disable alignment check
    reg |= SCTLR_ELx_M; // Enable the MMU
    asm volatile ("msr sctlr_el1, %0; isb" : : "r" (reg));
}
