typedef void (*cmd_handler_func)(int argc, char *argv[]);
struct cmd_handler {
    char *cmd;
    cmd_handler_func handler;
};
#define CMD_HANDLER(__cmd, __handler) ((struct cmd_handler) {.cmd = (__cmd), .handler = (__handler)})
extern struct cmd_handler builtin_cmds[];
extern size_t builtin_cmds_count;
