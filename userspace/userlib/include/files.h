#include <stddef.h>
#include <stdint.h>
// Signed size
typedef long ssize_t;

struct dirent {
    uint8_t type;
    char name[255];
};

#define DT_REG  0
#define DT_DIR  1

struct stat {
    uint8_t type;
    size_t size;
};

#define O_READ      0
#define O_WRITE     1
#define O_CREATE    2

int open(char *path, int mode);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, void *buf, size_t count);
int close(int fd);
int opendir(char *path);
int read_dir(int dir, struct dirent *ent);
int closedir(int dir);
int mkdir(char *path);
int unlink(char *path);
int rename(char *old_name, char *new_name);
int stat(const char *path, struct stat *out);
