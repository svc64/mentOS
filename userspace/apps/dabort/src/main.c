#include <stdlib.h>
int main(int argc, const char *argv[]) {
    *(unsigned long long *)(0x4141414141414141) = 0x4141414141414141;
    return 0;
}
