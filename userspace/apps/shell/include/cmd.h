#include <stdbool.h>

struct command {
    char *executable;
    char **argv;
    int argc;
    bool background;
};

struct command *cmd_new(void);
void cmd_run(struct command *cmd);
void cmd_free(struct command *cmd);
void cmd_add_arg(struct command *cmd, char *arg);
