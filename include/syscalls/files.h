#include <stdbool.h>
#include "fatfs/ff.h"
// Signed size
typedef long ssize_t;

#define MAX_DESCRIPTORS 1024
#define O_READ      0
#define O_WRITE     1
#define O_CREATE    2

#define E_NXFILE                -1 // No such file or directory
#define E_INVALID_PATH          -2 // Invalid path string
#define E_UNKNOWN               -3 // Unknown system error
#define E_MAX_REACHED           -4 // Maximum file descriptors reached
#define E_IOERR                 -5 // I/O error
#define E_INVALID_DESCRIPTOR    -6 // Invalid file descriptor
#define E_ACCESS                -7 // Access denied
#define E_OOB                   -8 // Out of bounds
#define E_NOMEM                 -9 // Out of memory
#define E_BUSY                 -10 // Resource busy
#define E_EXISTS               -11 // File already exists

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
int open_syscall(char *path, int mode);
int fd_valid(int fd);
ssize_t read_syscall(int fd, void *buf, size_t count);
ssize_t ftell_syscall(int fd);
ssize_t write_syscall(int fd, void *buf, size_t count);
int close_syscall(int fd);
ssize_t fsize_syscall(int fd);
int lseek_syscall(int fd, uintptr_t where);
int ftruncate_syscall(int fd, uintptr_t length);
int opendir_syscall(char *path);
int dir_valid(int dir);
int read_dir_syscall(int dir, struct dirent *ent);
int closedir_syscall(int dir);
int mkdir_syscall(char *path);
int unlink_syscall(char *path);
int rename_syscall(char *old_name, char *new_name);
