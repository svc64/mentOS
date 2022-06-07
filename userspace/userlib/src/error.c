#include <stddef.h>
#include "errors.h"

char *err_str(int err) {
    switch (err) {
        case E_NXFILE:
            return "No such file or directory";
        case E_INVALID_PATH:
            return "Invalid path string";
        case E_UNKNOWN:
            return "Unknown error";
        case E_LIMIT:
            return "Limit reached";
        case E_IOERR:
            return "I/O error";
        case E_INVALID_DESCRIPTOR:
            return "Invalid descriptor";
        case E_ACCESS:
            return "Access denied";
        case E_OOB:
            return "Out of bounds";
        case E_NOMEM:
            return "Out of memory";
        case E_BUSY:
            return "Resource busy";
        case E_EXISTS:
            return "Already exists";
        case E_FORMAT:
            return "Incorrect format";
        case E_NXPROC:
            return "No such process";
        case E_PARAM:
            return "Invalid parameter";
        default:
            return NULL;
    }
}
