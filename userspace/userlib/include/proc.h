#include <stdbool.h>
int exec(char *path, bool background);
int chdir(char *path);
void block_sigint();
void unblock_sigint();
