#include <io.h>
#include <stdbool.h>
int main(int argc, const char *argv[]) {
    print("Hi!\n");
    while (true) {
        print("Got character: 0x%x\n", getc());
    }
    return 4141;
}
