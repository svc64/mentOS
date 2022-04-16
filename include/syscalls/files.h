#include "fatfs/ff.h"
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
