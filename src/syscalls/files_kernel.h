#include <stdbool.h>
#include "fatfs/ff.h"
#define MAX_DESCRIPTORS 1024
struct dir_d { // Directory descriptor
    DIR *d; // FatFs directory
    struct proc *proc; // Owning proc
    bool end; // Set if we reached the end of this directory. Makes the whole descriptor unusable.
};
struct fd {
    FIL *f; // FatFs file
    struct proc *proc; // Owning proc
};
extern struct dir_d *dirs[MAX_DESCRIPTORS];
extern struct fd *fds[MAX_DESCRIPTORS];
int close_syscall(int fd);
int closedir_syscall(int dir);
