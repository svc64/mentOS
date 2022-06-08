#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "proc.h"
#include "files.h"
#include "errors.h"
#include "path.h"
#include "alloc.h"

int open_syscall(char *path, int mode) {
    char *sanitized_path = sanitize_path(path, current_proc->cwd);
    if (!sanitized_path) {
        return E_NOMEM;
    }
    int fd = open(sanitized_path, mode);
    if (fd >= 0) {
        fds[fd]->proc = current_proc;
    }
    free(sanitized_path);
    return fd;
}

int fd_valid(int fd) {
    enter_critical_section();
    if (fd < 0 || fd >= MAX_DESCRIPTORS) {
        return false;
    }
    if (fds[fd]->proc != current_proc) {
        return false;
    }
    if (fds[fd] == NULL) {
        return false;
    }
    exit_critical_section();
    return true;
}

ssize_t read_syscall(int fd, void *buf, size_t count) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    if (fds[fd]->proc != current_proc) {
        return E_INVALID_DESCRIPTOR;
    }
    return read(fd, buf, count);
}

ssize_t ftell_syscall(int fd) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    return ftell(fd);
}

ssize_t write_syscall(int fd, void *buf, size_t count) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    return write(fd, buf, count);
}

int close_syscall(int fd) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    return close(fd);
}

ssize_t fsize_syscall(int fd) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    return fsize(fd);
}

int lseek_syscall(int fd, uintptr_t where) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    return lseek(fd, where);
}

int ftruncate_syscall(int fd, uintptr_t length) {
    if (!fd_valid(fd)) {
        return E_INVALID_DESCRIPTOR;
    }
    return ftruncate(fd, length);
}

int opendir_syscall(char *path) {
    char *sanitized_path = sanitize_path(path, current_proc->cwd);
    if (!sanitized_path) {
        return E_NOMEM;
    }
    int ret = opendir(sanitized_path);
    free(sanitized_path);
    return ret;
}

int dir_valid(int dir) {
    enter_critical_section();
    if (dir < 0 || dir >= MAX_DESCRIPTORS) {
        return false;
    }
    if (dirs[dir] == NULL) {
        return false;
    }
    if (dirs[dir]->proc != current_proc) {
        return false;
    }
    exit_critical_section();
    return true;
}

int read_dir_syscall(int dir, struct dirent *ent) {
    if (!dir_valid(dir)) {
        return E_INVALID_DESCRIPTOR;
    }
    return read_dir(dir, ent);
}

int closedir_syscall(int dir) {
    if (!dir_valid(dir)) {
        return E_INVALID_DESCRIPTOR;
    }
    return closedir(dir);
}

int mkdir_syscall(char *path) {
    char *sanitized_path = sanitize_path(path, current_proc->cwd);
    if (!sanitized_path) {
        return E_NOMEM;
    }
    int ret = mkdir(sanitized_path);
    free(sanitized_path);
    return ret;
}

int unlink_syscall(char *path) {
    char *sanitized_path = sanitize_path(path, current_proc->cwd);
    if (!sanitized_path) {
        return E_NOMEM;
    }
    int ret = unlink(sanitized_path);
    free(sanitized_path);
    return ret;
}

int rename_syscall(char *old_name, char *new_name) {
    char *sanitized_old_name = sanitize_path(old_name, current_proc->cwd);
    if (!sanitized_old_name) {
        return E_NOMEM;
    }
    char *sanitized_new_name = sanitize_path(new_name, current_proc->cwd);
    if (!sanitized_new_name) {
        free(sanitized_old_name);
        return E_NOMEM;
    }
    int ret = rename(old_name, new_name);
    free(sanitized_old_name);
    free(sanitized_new_name);
    return ret;
}

int stat_syscall(const char *path, struct stat *out) {
    char *sanitized_path = sanitize_path(path, current_proc->cwd);
    if (!sanitized_path) {
        return E_NOMEM;
    }
    int ret = stat(sanitized_path, out);
    free(sanitized_path);
    return ret;
}
