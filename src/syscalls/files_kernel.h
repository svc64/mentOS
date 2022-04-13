#include <stdbool.h>
#include "fatfs/ff.h"
#define MAX_DESCRIPTORS 1024
typedef struct { // Directory descriptor
    DIR *d; // FatFs directory
    struct proc *proc; // Owning proc
    bool end; // Set if we reached the end of this directory. Makes the whole descriptor unusable.
} dir_d;
typedef struct {
    FIL *f; // FatFs file
    struct proc *proc; // Owning proc
} fd;
extern dir_d *dirs[MAX_DESCRIPTORS];
extern fd *fds[MAX_DESCRIPTORS];
int close_syscall(int fd);
int closedir_syscall(int dir);
