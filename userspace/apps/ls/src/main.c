#include <io.h>
#include <files.h>
#include <errors.h>

int ls(char *path) {
    struct dirent ent;
    int dir = opendir(path);
    if (dir < 0) {
        switch (dir) {
            case E_NXFILE:
                print("Directory not found: %s\n", path);
                break;
            case E_NOMEM:
                print("error: out of memory!\n");
                break;
            case E_INVALID_PATH:
               print("Invalid path: %s\n", path);
               break;
            case E_IOERR:
                print("I/O error: %s\n", path);
                break;
            default:
                print("unknown error: %d\n", dir);
                break;
        }
        return 1;
    }
    int err = 0;
    while (err != E_OOB && err >= 0) {
        err = read_dir(dir, &ent);
        if (err >= 0) {
            char *type;
            switch (ent.type) {
                case DT_DIR:
                    type = "D";
                    break;
                case DT_REG:
                    type = "F";
                    break;
                default:
                    type = "?";
                    break;
            }
            print("%s | %s\n", type, ent.name);
        }
    }
    if (err < 0 && err != E_OOB) {
        switch (err) {
            case E_IOERR:
                print("I/O error: %s\n", path);
                break;
            default:
                print("unknown error: %d\n", err);
                break;
        }
        return 1;
    }
    closedir(dir);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        ls(".");
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        ls(argv[i]);
    }
    return 0;
}
