#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "fatfs/ff.h"
#include "mem.h"
#include "stdlib.h"
#include "syscalls/files.h"
#include "proc.h"

struct dir_d *dirs[MAX_DESCRIPTORS];
struct fd *fds[MAX_DESCRIPTORS];

int open_syscall(char *path, int mode) {
    BYTE fatfs_mode = FA_READ;
    if (mode & O_WRITE) {
        fatfs_mode = FA_WRITE;
    }
    if (mode & O_CREATE) {
        fatfs_mode |= FA_CREATE_NEW;
    }
    int fd = -1;
    for (int i = 0; i < MAX_DESCRIPTORS; i++) {
        if (fds[i] == NULL) {
            fd = i;
            break;
        }
    }
    if (fd == -1) {
        return E_MAX_REACHED; // max FDs reached
    }
    fds[fd] = malloc(sizeof(struct fd));
    if (fds[fd] == NULL) {
        return E_NOMEM;
    }
    fds[fd]->f = malloc(sizeof(FIL));
    if (fds[fd]->f == NULL) {
        return E_NOMEM;
    }
    FRESULT res = f_open(fds[fd]->f, path, fatfs_mode);
    if (res) {
        free(fds[fd]->f);
        free(fds[fd]);
        fds[fd] = NULL;
        switch (res)
        {
            case FR_NO_PATH:
            case FR_NO_FILE:
                return E_NXFILE;
            case FR_INVALID_NAME:
                return E_INVALID_PATH;
            case FR_DISK_ERR:
                return E_IOERR;
            case FR_LOCKED:
                return E_BUSY;
            case FR_EXIST:
                return E_EXISTS;
            default:
                return E_UNKNOWN;
        }
    }
    fds[fd]->proc = current_proc;
    return fd;
}

int fd_valid(int fd) {
    if (fd < 0 || fd >= MAX_DESCRIPTORS) {
        print("fd invalid %d\n", fd);
        return false;
    }
    if (fds[fd] == NULL) {
        return false;
    }
    return true;
}

ssize_t read_syscall(int fd, void *buf, size_t count) {
    UINT bytes_read = -1;
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    FRESULT res = f_read(fds[fd]->f, buf, count, &bytes_read);
    if (res) {
        switch (res)
        {
            case FR_DISK_ERR:
                return E_IOERR;
            case FR_DENIED:
                return E_ACCESS;
            default:
                return E_UNKNOWN;
        }
    }
    return bytes_read;
}

ssize_t ftell_syscall(int fd) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    return f_tell(fds[fd]->f);
}

ssize_t write_syscall(int fd, void *buf, size_t count) {
    UINT bytes_written = -1;
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    FRESULT res = f_write(fds[fd]->f, buf, count, &bytes_written);
    if (res) {
        switch (res)
        {
            case FR_DISK_ERR:
                return E_IOERR;
            case FR_DENIED:
                return E_ACCESS;
            default:
                return E_UNKNOWN;
        }
    }
    return bytes_written;
}

int close_syscall(int fd) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    FRESULT res = f_close(fds[fd]->f);
    if (res) {
        switch (res)
        {
            case FR_DISK_ERR:
                return E_IOERR;
            default:
                return E_UNKNOWN;
        }
    }
    free(fds[fd]->f);
    free(fds[fd]);
    fds[fd] = NULL;
    return 0;
}

ssize_t fsize_syscall(int fd) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    return f_size(fds[fd]->f);
}

int lseek_syscall(int fd, uintptr_t where) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    if (where > f_size(fds[fd]->f)) {
        return E_OOB;
    }
    FRESULT res = f_lseek(fds[fd]->f, where);
    if (res) {
        switch (res)
        {
            case FR_DISK_ERR:
                return E_IOERR;
            default:
                return E_UNKNOWN;
        }
    }
    return 0;
}

int ftruncate_syscall(int fd, uintptr_t length) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    if (length > f_size(fds[fd]->f)) {
        return E_OOB;
    }
    uintptr_t current_off = f_tell(fds[fd]->f);
    int ret = lseek_syscall(fd, length);
    if (ret) {
        return ret;
    }
    FRESULT res = f_truncate(fds[fd]->f);
    if (res) {
        switch (res)
        {
            case FR_DISK_ERR:
                return E_IOERR;
            case FR_DENIED:
                return E_ACCESS;
            default:
                return E_UNKNOWN;
        }
    }
    if (current_off > length) {
        int ret = lseek_syscall(fd, length);
        if (ret) {
            return ret;
        }
    } else {
        int ret = lseek_syscall(fd, current_off);
        if (ret) {
            return ret;
        }
    }
    return 0;
}

int opendir_syscall(char *path) {
    int dir = -1;
    for (int i = 0; i < MAX_DESCRIPTORS; i++) {
        if (dirs[i] == NULL) {
            dir = i;
            break;
        }
    }
    if (dir == -1) {
        return E_MAX_REACHED; // max open dirs reached
    }
    dirs[dir] = malloc(sizeof(struct dir_d));
    if (dirs[dir] == NULL) {
        return E_NOMEM;
    }
    dirs[dir]->d = malloc(sizeof(DIR));
    if (dirs[dir]->d == NULL) {
        return E_NOMEM;
    }
    dirs[dir]->end = false;
    FRESULT res = f_opendir(dirs[dir]->d, path);
    if (res) {
        free(dirs[dir]->d);
        free(dirs[dir]);
        dirs[dir] = NULL;
        switch (res)
        {
            case FR_NO_PATH:
            case FR_NO_FILE:
                return E_NXFILE;
            case FR_INVALID_NAME:
                return E_INVALID_PATH;
            case FR_DISK_ERR:
                return E_IOERR;
            default:
                return E_UNKNOWN;
        }
    }
    dirs[dir]->proc = current_proc;
    return dir;
}

int dir_valid(int dir) {
    if (dir < 0 || dir >= MAX_DESCRIPTORS) {
        return false;
    }
    if (dirs[dir] == NULL) {
        return false;
    }
    if (dirs[dir]->proc != current_proc) {
        return false;
    }
    return true;
}

int read_dir_syscall(int dir, struct dirent *ent) {
    if (!dir_valid(dir)) {
        return E_INVALID_DESCRIPTOR;
    }
    if (dirs[dir]->end) {
        return E_OOB;
    }
    FILINFO fno;
    FRESULT res = f_readdir(dirs[dir]->d, &fno);
    if (res) {
        switch (res)
        {
            case FR_DISK_ERR:
                return E_IOERR;
            default:
                return E_UNKNOWN;
        }
    }
    if (fno.fname[0] == '\0') {
        dirs[dir]->end = true;
        return E_OOB;
    }
    if (fno.fattrib & AM_DIR) {
        ent->type = DT_DIR;
    } else {
        ent->type = DT_REG;
    }
    memcpy(ent->name, fno.fname, strlen(fno.fname) + 1);
    return 0;
}

int closedir_syscall(int dir) {
    if (!dir_valid(dir)) {
        return E_INVALID_DESCRIPTOR;
    }
    FRESULT res = f_closedir(dirs[dir]->d);
    if (res) {
        switch (res)
        {
            case FR_DISK_ERR:
                return E_IOERR;
            default:
                return E_UNKNOWN;
        }
    }
    free(dirs[dir]->d);
    free(dirs[dir]);
    dirs[dir] = NULL;
    return 0;
}

int mkdir_syscall(char *path) {
    FRESULT res = f_mkdir(path);
    if (res) {
        switch (res)
        {
            case FR_NO_PATH:
            case FR_NO_FILE:
                return E_NXFILE;
            case FR_INVALID_NAME:
                return E_INVALID_PATH;
            case FR_DISK_ERR:
                return E_IOERR;
            case FR_LOCKED:
                return E_BUSY;
            default:
                return E_UNKNOWN;
        }
    }
    return 0;
}
int unlink_syscall(char *path) {
    FRESULT res = f_unlink(path);
    if (res) {
        switch (res)
        {
            case FR_NO_PATH:
            case FR_NO_FILE:
                return E_NXFILE;
            case FR_INVALID_NAME:
                return E_INVALID_PATH;
            case FR_DISK_ERR:
                return E_IOERR;
            case FR_LOCKED:
                return E_BUSY;
            default:
                return E_UNKNOWN;
        }
    }
    return 0;
}

int rename_syscall(char *old_name, char *new_name) {
    FRESULT res = f_rename(old_name, new_name);
    if (res) {
        switch (res)
        {
            case FR_NO_PATH:
            case FR_NO_FILE:
                return E_NXFILE;
            case FR_INVALID_NAME:
                return E_INVALID_PATH;
            case FR_DISK_ERR:
                return E_IOERR;
            case FR_LOCKED:
                return E_BUSY;
            default:
                return E_UNKNOWN;
        }
    }
    return 0;
}
