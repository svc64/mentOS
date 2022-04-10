#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "fatfs/ff.h"
#include "mem.h"
#include "files.h"

FIL *fds[1024];
DIR *dirs[1024];

int open_syscall(char *path, int mode) {
    BYTE fatfs_mode = FA_READ;
    if (mode & O_WRITE) {
        fatfs_mode = FA_WRITE;
    }
    int fd = -1;
    for (int i = 0; i < sizeof(fds); i++) {
        if (fds[i] == NULL) {
            fd = i;
            break;
        }
    }
    if (fd == -1) {
        return E_MAX_REACHED; // max FDs reached
    }
    fds[fd] = malloc(sizeof(FIL));
    FRESULT res = f_open(fds[fd], path, fatfs_mode);
    if (res) {
        free(fds[fd]);
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
    return fd;
}

int fd_valid(int fd) {
    if (fd < 0 || fd > sizeof(fds) - 1) {
        return false;
    }
    if (fds[fd] == NULL) {
        return false;
    }
    return true;
}

size_t read_syscall(int fd, void *buf, size_t count) {
    UINT bytes_read = -1;
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    FRESULT res = f_read(fds[fd], buf, count, &bytes_read);
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

size_t ftell_syscall(int fd) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    return f_tell(fds[fd]);
}

size_t write_syscall(int fd, void *buf, size_t count) {
    UINT bytes_written = -1;
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    FRESULT res = f_write(fds[fd], buf, count, &bytes_written);
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
    FRESULT res = f_close(fds[fd]);
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

size_t fsize_syscall(int fd) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    return f_size(fds[fd]);
}

int lseek_syscall(int fd, uintptr_t where) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    if (where > f_size(fds[fd])) {
        return E_OOB;
    }
    FRESULT res = f_lseek(fds[fd], where);
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
    if (length > f_size(fds[fd])) {
        return E_OOB;
    }
    uintptr_t current_off = f_tell(fds[fd]);
    int ret = lseek_syscall(fd, length);
    if (ret) {
        return ret;
    }
    FRESULT res = f_truncate(fds[fd]);
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
    for (int i = 0; i < sizeof(dirs); i++) {
        if (dirs[i] == NULL) {
            dir = i;
            break;
        }
    }
    if (dir == -1) {
        return E_MAX_REACHED; // max open dirs reached
    }
    dirs[dir] = malloc(sizeof(DIR));
    FRESULT res = f_opendir(dirs[dir], path);
    if (res) {
        free(dirs[dir]);
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
    return dir;
}

int dir_valid(int dir) {
    if (dir < 0 || dir > sizeof(dirs) - 1) {
        return false;
    }
    if (dirs[dir] == NULL) {
        return false;
    }
    return true;
}

int read_dir_syscall(int dir, struct dirent *ent) {
    if (!dir_valid(dir)) {
        return E_INVALID_DESCRIPTOR;
    }
    FILINFO fno;
    FRESULT res = f_readdir(dirs[dir], &fno);
    if (res) {
        switch (res)
        {
            case FR_DISK_ERR:
                return E_IOERR;
            default:
                return E_UNKNOWN;
        }
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
    FRESULT res = f_closedir(dirs[dir]);
    if (res) {
        switch (res)
        {
            case FR_DISK_ERR:
                return E_IOERR;
            default:
                return E_UNKNOWN;
        }
    }
    free(dirs[dir]);
    return 0;
}
