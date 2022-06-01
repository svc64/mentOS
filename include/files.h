#include <stdbool.h>
#include <stddef.h>
#include "fatfs/ff.h"
// Signed size
typedef long ssize_t;

#define MAX_DESCRIPTORS 1024
#define O_READ      0
#define O_WRITE     1
#define O_CREATE    2

struct dirent {
    uint8_t type;
    char name[FF_MAX_LFN];
};

#define DT_REG  0
#define DT_DIR  1

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
int open(char *path, int mode);
ssize_t read(int fd, void *buf, size_t count);
ssize_t ftell(int fd);
ssize_t write(int fd, void *buf, size_t count);
int close(int fd);
ssize_t fsize(int fd);
int lseek(int fd, uintptr_t where);
int ftruncate(int fd, uintptr_t length);
int opendir(char *path);
int read_dir(int dir, struct dirent *ent);
int closedir(int dir);
int mkdir(char *path);
int unlink(char *path);
int rename(char *old_name, char *new_name);
