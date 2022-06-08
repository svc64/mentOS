#include <stddef.h>
#include <io.h>
#include <proc.h>
#include <errors.h>
#include "builtin_cmds.h"

void builtin_cmd_cd(int argc, char *argv[]) {
    if (argc > 2) {
        print("cd: too many arguments\n");
    } else if (argc == 2) {
        int err = chdir(argv[1]);
        if (err < 0) {
            switch (err) {
                case E_NXFILE:
                    print("Directory not found: %s\n", argv[1]);
                    break;
                case E_NOMEM:
                    print("error: out of memory!\n");
                    break;
                case E_INVALID_PATH:
                    print("Invalid path: %s\n", argv[1]);
                    break;
                case E_IOERR:
                    print("I/O error: %s\n", argv[1]);
                    break;
                default:
                    print("Unknown error (%d): %s\n", err, err_str(err));
                    break;
            }
        }
    } else {
        print("usage: cd [directory]\n");
    }
}

void builtin_cmd_pwd(int argc, char *argv[]) {
    if (argc > 1) {
        print("usage: pwd\n");
        return;
    }
    char dir[4096];
    int err = getcwd(dir, sizeof(dir));
    if (err < 0) {
        switch (err) {
            case E_NOMEM:
                print("cwd: path is too long\n");
                break;
            default:
                print("Unknown error (%d): %s\n", err, err_str(err));
                break;
        }
        return;
    }
    print("%s\n", dir);
}

void builtin_cmd_exit(int argc, char *argv[]) {
    exit(0);
}

struct cmd_handler builtin_cmds[] = {
    CMD_HANDLER("cd", builtin_cmd_cd),
    CMD_HANDLER("pwd", builtin_cmd_pwd),
    CMD_HANDLER("exit", builtin_cmd_exit),
};
size_t builtin_cmds_count = sizeof(builtin_cmds) / sizeof(struct cmd_handler);
