#include "print.h"
#include "exceptions.h"
#include "time.h"
#include "uart.h"
#include "mem.h"
#include "test_user_proc.h"
#include "proc.h"
#include "fatfs/ff.h"
#include "ramdisk.h"

extern volatile unsigned char _end; // where our kernel image ends
uint8_t *end = (uint8_t *)(&_end);
FATFS *fs = NULL;
extern void *heap; // heap start (from mem.c)

void main() {
    init_uart();
    print("it's alive\n");
    init_timer();
    struct ramdisk_header *rd = (struct ramdisk_header *)(end);
    if (memcmp(rd->magic, "RD", sizeof(rd->magic))) {
        print("invalid ramdisk magic: 0x%x 0x%x\n", end[0], end[1]);
        panic("no ramdisk!\n");
    }
    // get the ramdisk size, unswap it from big endian
    uint64_t ramdisk_size;
    uint8_t *rdsize = (uint8_t *)(&ramdisk_size);
    uint8_t *rdsize_swapped = (uint8_t *)(&(rd->big_endian_size));
    rdsize[0] = rdsize_swapped[7];
    rdsize[1] = rdsize_swapped[6];
    rdsize[2] = rdsize_swapped[5];
    rdsize[3] = rdsize_swapped[4];
    rdsize[4] = rdsize_swapped[3];
    rdsize[5] = rdsize_swapped[2];
    rdsize[6] = rdsize_swapped[1];
    rdsize[7] = rdsize_swapped[0];
    heap = (void *)((uintptr_t)(heap + sizeof(struct ramdisk_header) + ramdisk_size + 2 * PAGE_SIZE) & -PAGE_SIZE); // adjust heap
    print("heap start: 0x%x\n", heap);
    fs = malloc(sizeof(FATFS));
    if (!fs) {
        panic("failed to allocate filesystem data\n");
    }
    bzero(fs, sizeof(FATFS));
    // mount ramdisk
    FRESULT r = f_mount(fs, "", 1);
    if (r) {
        print("mount fail: FatFs error %d\n", r);
        panic("Failed to mount ramdisk!");
    }
    // initialize the proc struct list
    proc_init();
    disable_irqs();
    print("mentOS\n-------\n");
    print("main addr: 0x%x\n", &main);
    print("_end: 0x%x\n", &_end);
    print("ramdisk size: 0x%x\n", ramdisk_size);
    uint64_t current_el;
    __asm__ __volatile__("mrs %0, CurrentEL\n\t" : "=r" (current_el) :  : "memory");
    current_el = (current_el >> 2) & 0x3;
    print("Running in EL%d\n", current_el);
    // Set exception vectors
    __asm__ __volatile__("msr vbar_el1, %0\n\t; isb" : : "r" (&exception_vectors) : "memory");
    // create processes
    int first_proc = proc_new((uintptr_t)&file_close_test);
    print("created pid %d\n", first_proc);
    print("created pid %d\n", proc_new((uintptr_t)&test_proc_1));
    print("created pid %d\n", proc_new((uintptr_t)&test_proc_2));
    print("created pid %d\n", proc_new((uintptr_t)&test_proc_3));
    // move to the first one...
    proc_enter(first_proc, PROC_TIME);
    while (1);
}
