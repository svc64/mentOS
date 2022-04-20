#include <stddef.h>
int open(char *path, int mode);
size_t read(int fd, void *buf, size_t count);
size_t write(int fd, void *buf, size_t count);
int close(int fd);
int opendir(char *path);
int read_dir(int dir, struct dirent *ent);
int closedir(int dir);
int mkdir(char *path);
int unlink(char *path);
int rename(char *old_name, char *new_name);
