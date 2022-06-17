#include <stdlib.h>
#include <stdint.h>
#include <proc.h>
#include <errors.h>
#include <io.h>
#include "cmd.h"
#include "path.h"
#include "alloc.h"
#include "builtin_cmds.h"

struct command *cmd_new(void) {
    struct command *cmd = malloc_noerror(sizeof(struct command));
    bzero(cmd, sizeof(struct command));
    return cmd;
}

void cmd_run(struct command *cmd) {
    char **split = NULL;
    size_t count = 0;
    int exec_ret;
    for (int i = 0; i < builtin_cmds_count; i++) {
        if (!strcmp(cmd->executable, builtin_cmds[i].cmd)) {
            builtin_cmds[i].handler(cmd->argc, cmd->argv);
            return;
        }
    }
    split_path(cmd->executable, &split, &count);
    char *executable;
    if (count == 1) {
        // run this from PATH
        executable = join_paths("/bin/", cmd->executable);
        exec_ret = exec(executable, cmd->argv, cmd->background);
        free(executable);
    } else {
        executable = cmd->executable;
        exec_ret = exec(cmd->executable, cmd->argv, cmd->background);
    }
    if (exec_ret < 0) {
        char *err_s;
        switch (exec_ret) {
            case E_NXFILE:
                print("File not found: %s\n", executable);
                break;
            case E_FORMAT:
                print("exec format error: %s\n", executable);
                break;
            default:
                err_s = err_str(exec_ret);
                if (err_s) {
                    print("%s: %s\n", err_s, executable);
                } else {
                    print("Unknown error (%d): %s\n", exec_ret, executable);
                }
                break;
        }
    }
    free_split_path(split, count);
}

void cmd_free(struct command *cmd) {
    if (cmd->executable) {
        free(cmd->executable);
    }
    for (int i = 0; i < cmd->argc; i++) {
        if (cmd->argv[i]) {
            free(cmd->argv[i]);
        }
    }
    if (cmd->argv) {
        free(cmd->argv);
    }
    free(cmd);
}

void cmd_add_arg(struct command *cmd, char *arg) {
    if (!cmd->executable) {
        cmd->executable = malloc_noerror(strlen(arg) + 1);
        strcpy(cmd->executable, arg);
    }
    if (!cmd->argv) {
        cmd->argc = 1;
        cmd->argv = malloc_noerror((cmd->argc + 1) * sizeof(char *));
        cmd->argv[0] = malloc_noerror(strlen(arg) + 1);
        strcpy(cmd->argv[0], arg);
    } else {
        int new_argc = cmd->argc + 1;
        char **argv = realloc_noerror(cmd->argv, (new_argc + 1) * sizeof(char *));
        argv[cmd->argc] = malloc_noerror(strlen(arg) + 1);
        strcpy(argv[cmd->argc], arg);
        cmd->argv = argv;
        cmd->argc = new_argc;
    }
    cmd->argv[cmd->argc] = NULL;
}
