#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "fatfs/ff.h"
#include "alloc.h"
#include "stdlib.h"
#include "files.h"
#include "proc.h"
#include "errors.h"

struct dir_d *dirs[MAX_DESCRIPTORS];
struct fd *fds[MAX_DESCRIPTORS];

int open(char *path, int mode) {
    BYTE fatfs_mode = FA_READ;
    if (mode & O_WRITE) {
        fatfs_mode = FA_WRITE;
    }
    if (mode & O_CREATE) {
        fatfs_mode |= FA_CREATE_NEW;
    }
    int fd = -1;
    enter_critical_section();
    for (int i = 0; i < MAX_DESCRIPTORS; i++) {
        if (fds[i] == NULL) {
            fd = i;
            break;
        }
    }
    if (fd == -1) {
        exit_critical_section();
        return E_LIMIT; // max FDs reached
    }
    fds[fd] = malloc(sizeof(struct fd));
    if (fds[fd] == NULL) {
        exit_critical_section();
        return E_NOMEM;
    }
    fds[fd]->f = malloc(sizeof(FIL));
    if (fds[fd]->f == NULL) {
        exit_critical_section();
        return E_NOMEM;
    }
    FRESULT res = f_open(fds[fd]->f, path, fatfs_mode);
    if (res) {
        free(fds[fd]->f);
        free(fds[fd]);
        fds[fd] = NULL;
        exit_critical_section();
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
    exit_critical_section();
    return fd;
}

ssize_t read(int fd, void *buf, size_t count) {
    UINT bytes_read = -1;
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

ssize_t ftell(int fd) {
    return f_tell(fds[fd]->f);
}

ssize_t write(int fd, void *buf, size_t count) {
    enter_critical_section();
    UINT bytes_written = -1;
    FRESULT res = f_write(fds[fd]->f, buf, count, &bytes_written);
    exit_critical_section();
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

// should never fail when a file is opened for reading only
int close(int fd) {
    enter_critical_section();
    FRESULT res = f_close(fds[fd]->f);
    if (res) {
        exit_critical_section();
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
    exit_critical_section();
    return 0;
}

ssize_t fsize(int fd) {
    return f_size(fds[fd]->f);
}

int lseek(int fd, uintptr_t where) {
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

int ftruncate(int fd, uintptr_t length) {
    enter_critical_section();
    if (length > f_size(fds[fd]->f)) {
        exit_critical_section();
        return E_OOB;
    }
    uintptr_t current_off = f_tell(fds[fd]->f);
    int ret = lseek(fd, length);
    if (ret) {
        exit_critical_section();
        return ret;
    }
    FRESULT res = f_truncate(fds[fd]->f);
    if (res) {
        exit_critical_section();
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
        int ret = lseek(fd, length);
        if (ret) {
            exit_critical_section();
            return ret;
        }
    } else {
        int ret = lseek(fd, current_off);
        if (ret) {
            exit_critical_section();
            return ret;
        }
    }
    exit_critical_section();
    return 0;
}

int opendir(char *path) {
    enter_critical_section();
    int dir = -1;
    for (int i = 0; i < MAX_DESCRIPTORS; i++) {
        if (dirs[i] == NULL) {
            dir = i;
            break;
        }
    }
    if (dir == -1) {
        return E_LIMIT; // max open dirs reached
    }
    dirs[dir] = malloc(sizeof(struct dir_d));
    if (dirs[dir] == NULL) {
        print("malloc dir_d failed\n");
        return E_NOMEM;
    }
    dirs[dir]->d = malloc(sizeof(DIR));
    if (dirs[dir]->d == NULL) {
        print("malloc DIR failed\n");
        return E_NOMEM;
    }
    dirs[dir]->end = false;
    FRESULT res = f_opendir(dirs[dir]->d, path);
    if (res) {
        free(dirs[dir]->d);
        free(dirs[dir]);
        dirs[dir] = NULL;
        exit_critical_section();
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
    exit_critical_section();
    return dir;
}

int read_dir(int dir, struct dirent *ent) {
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

int closedir(int dir) {
    enter_critical_section();
    FRESULT res = f_closedir(dirs[dir]->d);
    if (res) {
        exit_critical_section();
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
    exit_critical_section();
    return 0;
}

int mkdir(char *path) {
    enter_critical_section();
    FRESULT res = f_mkdir(path);
    exit_critical_section();
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

int unlink(char *path) {
    int dir_d = opendir(path);
    if (dir_d >= 0) {
        struct dirent ent;
        int err = read_dir(dir_d, &ent);
        closedir(dir_d);
        if (err < 0 && err != E_OOB) {
            return err;
        } else if (!err) {
            return E_NOTEMPTY;
        }
    }
    enter_critical_section();
    FRESULT res = f_unlink(path);
    exit_critical_section();
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
            case FR_DENIED:
                return E_ACCESS;
            default:
                return E_UNKNOWN;
        }
    }
    return 0;
}

int rename(char *old_name, char *new_name) {
    enter_critical_section();
    FRESULT res = f_rename(old_name, new_name);
    exit_critical_section();
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

int stat(const char *path, struct stat *out) {
    enter_critical_section();
    FILINFO info;
    FRESULT res = f_stat(path, &info);
    exit_critical_section();
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
            default:
                return E_UNKNOWN;
        }
    }
    out->size = 0;
    if (info.fattrib & AM_DIR) {
        out->type = DT_DIR;
    } else {
        out->type = DT_REG;
    }
    return 0;
}
