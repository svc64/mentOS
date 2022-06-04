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
                    break;
            }
        }
    } else {
        print("usage: cd [directory]\n");
    }
}
struct cmd_handler builtin_cmds[] = {
    CMD_HANDLER("cd", builtin_cmd_cd),
};
size_t builtin_cmds_count = sizeof(builtin_cmds) / sizeof(struct cmd_handler);
