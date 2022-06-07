#include <stdbool.h>
int exec(char *path, char **argp, bool background);
int chdir(char *path);
int kill(int pid, int signal);
void block_sigint();
void unblock_sigint();
